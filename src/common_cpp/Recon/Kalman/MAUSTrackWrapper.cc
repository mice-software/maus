/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "src/common_cpp/Recon/Kalman/MAUSTrackWrapper.hh"

#include "TMatrixD.h"

namespace MAUS {

  template<class ELEMENT>
  bool SortByZ(const ELEMENT *a, const ELEMENT *b) {
      return ( a->get_position().z() > b->get_position().z() );
  }



  Kalman::Track BuildTrack(SciFiClusterPArray cluster_array, const SciFiGeometryHelper* geom) {
    Kalman::Track new_track(1);
    size_t numbclusters = cluster_array.size();

    if ( numbclusters < 1 ) return new_track;
    int tracker = cluster_array[0]->get_tracker();

    const SciFiPlaneMap& geom_map = geom->GeometryMap().find(tracker)->second.Planes;
    int tracker_const = ( tracker == 0 ? -1 : 1 );

    for ( SciFiPlaneMap::const_iterator iter = geom_map.begin(); iter != geom_map.end(); ++iter ) {
      int id = iter->first * tracker_const;
      Kalman::State new_state = Kalman::State(1, iter->second.Position.z());
      new_state.SetId(id);
      new_track.Append(new_state);
    }

    for ( size_t j = 0; j < numbclusters; ++j ) {
      SciFiCluster* cluster = cluster_array[j];
      
      int id = (cluster->get_station() - 1)*3 + cluster->get_plane(); // Actually (id - 1)

      // TODO : 
      // - APPLY GEOMETRY CORRECTIONS!
      // - Fill covariance matrix correctly!
      TMatrixD state_vector(1, 1);
      TMatrixD covariance(1, 1);

      state_vector(0, 0) = cluster->get_alpha();
      covariance(0, 0) = 0.0;

      new_track[id].SetVector(state_vector);
      new_track[id].SetCovariance(covariance);
    }
    return new_track;
  }




  Kalman::State ComputeSeed(SciFiHelicalPRTrack* h_track, const SciFiGeometryHelper* geom, bool correct_energy_loss, double seed_cov) {
    TMatrixD vector(5, 1);
    TMatrixD covariance(5, 5);

    int tracker = h_track->get_tracker();
    int seed_id = ( tracker == 0 ? -15 : 15 );
    double seed_pos = geom->GetPlanePosition(tracker, 5, 2);
    double length =  seed_pos;
    double c  = CLHEP::c_light;
    double particle_charge = h_track->get_charge();
    double Bz = geom->GetFieldValue(tracker);

    // Get h_track values.
    double r  = h_track->get_R();
    double pt = - c*Bz*r*particle_charge;
    double dsdz = - h_track->get_dsdz();
    double x0 = h_track->get_circle_x0(); // Circle Center x
    double y0 = h_track->get_circle_y0(); // Circle Center y
    double s = (h_track->get_line_sz_c() - length*dsdz); // Path length at start plane
    double phi_0 = s / r; // Phi at start plane
    double phi = phi_0 + TMath::PiOver2(); // Direction of momentum

    // TODO: Actually propagate the track parrameters and covariance matrix back to start plane.
    //       This is an approximation.
    ThreeVector patrec_momentum(-pt*sin(phi_0), pt*cos(phi_0), - pt/dsdz);

    if ( correct_energy_loss ) {
      double P = patrec_momentum.mag();
      double patrec_bias; // Account for two planes of energy loss
      if ( tracker == 0 ) {
        patrec_bias = (P + 1.4) / P;
      } else {
        patrec_bias = (P + 1.4) / P;
      }
      patrec_momentum = patrec_bias * patrec_momentum;
    }

    double x = x0 + r*cos(phi_0);
    double px = patrec_momentum.x();
    double y = y0 + r*sin(phi_0);
    double py = patrec_momentum.y();
    double kappa = particle_charge / patrec_momentum.z();

    vector(0, 0) = x;
    vector(1, 0) = px; 
    vector(2, 0) = y;
    vector(3, 0) = py;
    vector(4, 0) = kappa;

//
// METHOD = ED SANTOS
//
    if (seed_cov > 0.0) {
      covariance.Zero();
      for ( int i = 0; i < 5; ++i ) {
        covariance(i, i) = seed_cov;
      }
    } else {
//
// METHOD = CHRISTOPHER HUNT
//
      std::vector<double> cov = h_track->get_covariance();
      TMatrixD patrec_covariance(5, 5);
      if (cov.size() != 25) {
        throw MAUS::Exception(MAUS::Exception::recoverable, 
                              "Dimension of covariance matrix does not match the state vector",
                              "KalmanSeed::ComputeInitalCovariance(Helical)");
      }

      double mc = particle_charge*c*Bz; // Magnetic constant
      double sin = std::sin(phi_0);
      double cos = std::cos(phi_0);
      double sin_plus = std::sin(phi);
      double cos_plus = std::cos(phi);

      TMatrixD jacobian(5, 5);
      jacobian(0,0) = 1.0;
      jacobian(0,2) = cos + phi*sin;
      jacobian(0,3) = -sin;

      jacobian(1,2) = mc*cos_plus + mc*phi*sin_plus;
      jacobian(1,3) = -mc*sin_plus;

      jacobian(2,1) = 1.0;
      jacobian(2,2) = sin - phi*cos;
      jacobian(2,3) = cos;

      jacobian(3,2) = mc*sin_plus - mc*phi*cos_plus;
      jacobian(1,3) = mc*cos_plus;

      jacobian(4,3) = -dsdz / (mc*r*r);
      jacobian(4,4) = 1.0 / (mc*r);

      TMatrixD jacobianT(5, 5);
      jacobianT.Transpose( jacobian );

      for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
          patrec_covariance(i, j) = cov.at(i*5 + j);
        }
      }
      covariance = jacobian*patrec_covariance*jacobianT;
    }

    Kalman::State seed_state(vector, covariance, seed_pos);
    seed_state.SetId(seed_id);

    return seed_state;
  }


  Kalman::State ComputeSeed(SciFiStraightPRTrack* s_track, const SciFiGeometryHelper* geom, double seed_cov) {
    TMatrixD vector(4, 1);
    TMatrixD covariance(4, 4);

    int tracker = s_track->get_tracker();
    double seed_pos = geom->GetPlanePosition(tracker, 5, 2);
    double length =  seed_pos;
    int seed_id = ( tracker == 0 ? -15 : 15 );

    double x0 = s_track->get_x0();
    double y0 = s_track->get_y0();
    double mx = s_track->get_mx();
    double my = s_track->get_my();

    // Get position at the starting end of tracker
    double x = x0 + mx*length;
    double y = y0 + my*length;

    vector(0, 0) = x;
    vector(1, 0) = mx;
    vector(2, 0) = y;
    vector(3, 0) = my;

//
// METHOD = ED SANTOS
//
    if (seed_cov > 0.0) {
      covariance.Zero();
      for ( int i = 0; i < 4; ++i ) {
        covariance(i, i) = seed_cov;
      }
    } else {
//
// METHOD = CHRISTOPHER HUNT
//
      std::vector<double> cov = s_track->get_covariance();
      TMatrixD patrec_covariance(4, 4);

      if (cov.size() != (unsigned int)16) {
        throw MAUS::Exception( MAUS::Exception::recoverable, 
                              "Dimension of covariance matrix does not match the state vector",
                              "KalmanSeed::ComputeInitalCovariance(Straight)");
      }

      TMatrixD jacobian(4, 4);
      jacobian(0,0) = 1.0;
      jacobian(1,1) = 1.0;
      jacobian(2,2) = 1.0;
      jacobian(3,3) = 1.0;
      jacobian(0,1) = length;
      jacobian(2,3) = length;

      TMatrixD jacobianT(4, 4);
      jacobianT.Transpose( jacobian );

      for ( int i = 0; i < 4; ++i ) {
        for ( int j = 0; j < 4; ++j ) {
          patrec_covariance(i,j) = cov.at( i*4 + j );
        }
      }

      covariance = jacobian*patrec_covariance*jacobianT;
    }

    Kalman::State seed_state(vector, covariance, seed_pos);
    seed_state.SetId(seed_id);
    return seed_state;
  }


  SciFiTrack* ConvertToSciFiTrack( const Kalman::TrackFit* fitter, const SciFiGeometryHelper* geom) {
    SciFiTrack* new_track = new SciFiTrack();
    const Kalman::Track& smoothed = fitter->Smoothed();
//    const Kalman::Track& smoothed = fitter->Filtered();
//    const Kalman::Track& filtered = fitter->Filtered();
//    const Kalman::Track& predicted = fitter->Predicted();
    const Kalman::Track& data = fitter->Data();
    Kalman::State seed = fitter->GetSeed();

    if (smoothed.GetLength() < 1)
      throw MAUS::Exception(MAUS::Exception::recoverable, 
                            "Not enough points in Kalman Track",
                            "ConvertToSciFiTrack()");

    double chi_squared = fitter->CalculateChiSquared(smoothed);
    int NDF = fitter->GetNDF();
    double p_value = TMath::Prob(chi_squared, NDF);
    
    int tracker;
    if ( smoothed[0].GetId() > 0 ) {
      tracker = 1;
    } else {
      tracker = 0;
    }
    new_track->set_tracker(tracker);

    int dimension = smoothed.GetDimension();
    if (dimension == 4) {
      new_track->SetAlgorithmUsed(SciFiTrack::kalman_straight);
    } else if (dimension == 5) {
      new_track->SetAlgorithmUsed(SciFiTrack::kalman_helical);
    } else {
      throw MAUS::Exception(MAUS::Exception::recoverable, 
                            "Unexpected dimension of Kalman::Track",
                            "ConvertToSciFiTrack()");
    }

//    const SciFiGeometryMap& geom_map = geom->GeometryMap();

    ThreeVector reference_pos = geom->GetReferencePosition(tracker);
    HepRotation reference_rot = geom->GetReferenceRotation(tracker);
    double charge = 0.0;

    for ( unsigned int i = 0; i < smoothed.GetLength(); ++i ) {
      const Kalman::State& smoothed_state = smoothed[i];
//      const Kalman::State& filtered_state = filtered[i];
//      const Kalman::State& predicted_state = predicted[i];
      const Kalman::State& data_state = data[i];

      SciFiTrackPoint* new_point = new SciFiTrackPoint();

      new_point->set_tracker(tracker);

      int id = abs(smoothed_state.GetId());
      new_point->set_station(((id-1)/3)+1);
      new_point->set_plane((id-1)%3);

      ThreeVector pos;
      ThreeVector mom;

      TMatrixD state_vector = smoothed_state.GetVector();
      int charge = 0;

      if ( dimension == 4 ) {
        pos.setZ(smoothed_state.GetPosition());
        pos.setX(state_vector(0, 0));
        mom.setX(state_vector(1, 0)*200.0);
        pos.setY(state_vector(2, 0));
        mom.setY(state_vector(3, 0)*200.0);

        pos *= reference_rot;
        pos += reference_pos;

        mom *= reference_rot;
        if (tracker == 0) mom *= -1.0;
        mom.setZ(200.0); // MeV/c
      } else if ( dimension == 5 ) {
        pos.setX(state_vector(0, 0));
        mom.setX(state_vector(1, 0));
        pos.setY(state_vector(2, 0));
        mom.setY(state_vector(3, 0));
        pos.setZ(smoothed_state.GetPosition());

        pos *= reference_rot;
        pos += reference_pos;

        mom *= reference_rot;
        if (state_vector(4, 0) < 0.0) charge = -1.0;
        else charge = 1.0;
        mom.setZ(fabs(1.0/state_vector(4, 0)));
      }

//      if ( mom.z() < 0.0 ) {
//        mom.setZ( - mom.z() );
//      }
//      if (tracker == 0) {
//        mom.setZ(fabs(mom.z()));
//        mom.setY(-1.0*mom.y());
//      }

      new_point->set_pos(pos);
      new_point->set_mom(mom);

      // TODO
    //  _pull              = kalman_site->residual(KalmanState::Projected)(0, 0);
    //  _residual          = kalman_site->residual(KalmanState::Filtered)(0, 0);
    //  _smoothed_residual = kalman_site->residual(KalmanState::Smoothed)(0, 0);
    //  AND CHARGE!
      if (data_state) {
//        new_point->set_pull(sqrt(fitter->CalculatePredictedResidual(i).GetVector().E2Norm()));
//        new_point->set_residual(sqrt(fitter->CalculateFilteredResidual(i).GetVector().E2Norm()));
//        new_point->set_smoothed_residual(sqrt(fitter->CalculateSmoothedResidual(i).GetVector().E2Norm()));
        new_point->set_pull(fitter->CalculatePredictedResidual(i).GetVector()(0, 0));
        new_point->set_residual(fitter->CalculateFilteredResidual(i).GetVector()(0, 0));
        new_point->set_smoothed_residual(fitter->CalculateSmoothedResidual(i).GetVector()(0, 0));
      } else {
        new_point->set_pull(0.0);
        new_point->set_residual(0.0);
        new_point->set_smoothed_residual(0.0);
      }


      TMatrixD C = smoothed_state.GetCovariance();
      int size = C.GetNrows();
      int num_elements = size*size;

      double* matrix_elements = C.GetMatrixArray();
      std::vector<double> covariance(num_elements);
      for ( int i = 0; i < num_elements; ++i ) {
        covariance[i] = matrix_elements[i];
      }
      new_point->set_covariance(covariance);
      std::vector<double> errors(size);
      for ( int i = 0; i < size; ++i ) {
        errors[i] = std::sqrt(fabs(C(i, i)));
      }
      new_point->set_errors(errors);

  //TODO
//    _cluster = new TRef(kalman_site->cluster());
      new_point->set_cluster(new TRef());
      new_track->add_scifitrackpoint(new_point);
    }

    new_track->set_charge(charge);
    new_track->set_chi2(chi_squared);
    new_track->set_ndf(NDF);
    new_track->set_P_value(p_value);

    TMatrixD seed_vector = seed.GetVector();
    ThreeVector seed_pos;
    ThreeVector seed_mom;
    if ( dimension == 4 ) {
      seed_pos.setZ(seed.GetPosition());
      seed_pos.setX(seed_vector(0, 0));
      seed_mom.setX(seed_vector(1, 0)*200.0);
      seed_pos.setY(seed_vector(2, 0));
      seed_mom.setY(seed_vector(3, 0)*200.0);
    } else if ( dimension == 5 ) {
      seed_pos.setX(seed_vector(0, 0));
      seed_mom.setX(seed_vector(1, 0));
      seed_pos.setY(seed_vector(2, 0));
      seed_mom.setY(seed_vector(3, 0));
      seed_pos.setZ(seed.GetPosition());
    }
    seed_pos *= reference_rot;
    seed_pos += reference_pos;

    seed_mom *= reference_rot;

    if ( dimension == 4 ) {
      seed_mom.setZ(200.0);
    } else if ( dimension == 5 ) {
      seed_mom.setZ(fabs(1.0/seed_vector(4, 0)));
    }

    new_track->SetSeedPosition(seed_pos);
    new_track->SetSeedMomentum(seed_mom);
    new_track->SetSeedCovariance(seed.GetCovariance().GetMatrixArray(), dimension*dimension);

// TODO:
// - Set Cluster
// - Calculate p-value
// - Set Algorithm used
// - Set Seed Info
// - Init track before the fit?

    return new_track;
  }


  Kalman::Track BuildSpacepointTrack(SciFiSpacePointPArray spacepoints, const SciFiGeometryHelper* geom, int plane_num, double smear) {
//    TRandom3 rand;

    Kalman::Track new_track(2);
    int tracker = (*spacepoints.begin())->get_tracker();

    for (unsigned int i = 0; i < 5; ++i) {
      Kalman::State new_state(2, geom->GetPlanePosition(tracker, i+1, plane_num));
      new_state.SetId((1 + i*3 + plane_num)*(tracker == 0 ? -1 : 1 ));
      new_track.Append(new_state);
    }

    for (SciFiSpacePointPArray::iterator it = spacepoints.begin(); it != spacepoints.end(); ++it ) {
      int station = (*it)->get_station();
      TMatrixD vec(2, 1);
      TMatrixD cov(2, 2); cov.Zero();
      vec(0, 0) = (*it)->get_position().x();// * (1.0 + rand.Gaus(0.0, smear));
      vec(1, 0) = (*it)->get_position().y();// * (1.0 + rand.Gaus(0.0, smear));
      cov(0, 0) = 0.0;
      cov(1, 1) = 0.0;
      new_track[station-1].SetVector(vec);
      new_track[station-1].SetCovariance(cov);
      new_track[station-1].SetPosition((*it)->get_position().z());
    }

    return new_track;
  }
}

