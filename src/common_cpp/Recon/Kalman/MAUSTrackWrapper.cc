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

#include "TMatrix.hh"

namespace MAUS {

  template<class ELEMENT>
  bool SortByZ(const ELEMENT *a, const ELEMENT *b) {
      return ( a->get_position().z() > b->get_position().z() );
  }


  Kalman::State ComputeSeed(SciFiHelicaPRTrack* h_track, SciFiGeometryHelper* geom) {
    TMatrixD vector(1, 5);
    TMatrixD covariance(5, 5);

    double length = 1100.0;
    double c  = CLHEP::c_light;
    double particle_charge = h_track->get_charge();

    // Downstream reconstruction goes in reverse.
    double Bz;
    int tracker = h_track.GetTracker();
    if ( _tracker == 1 ) {
      Bz = - geom->GetFieldValue(tracker);
    } else {
      Bz = geom->GetFieldValue(tracker);
    }

    // Get h_track values.
    double r  = h_track->get_R();
    double pt = fabs(_particle_charge*c*Bz*r);
    double dsdz = fabs(h_track->get_dsdz());
    double x0 = h_track->get_circle_x0(); // Circle Center x
    double y0 = h_track->get_circle_y0(); // Circle Center y
    double s = h_track->get_line_sz_c() - _particle_charge*length*dsdz; // Path length at start plane
    double phi_0 = s / r; // Phi at start plane
    double phi = phi_0 + TMath::PiOver2(); // Direction of momentum

    // TODO: Actually propagate the track parrameters and covariance matrix back to start plane.
    //       This is an approximation.
    ThreeVector patrec_momentum( pt*cos(phi), pt*sin(phi), fabs(pt/dsdz) );
  //  double P = patrec_momentum.mag();
  //  double patrec_bias; // Account for two planes of energy loss
  //  if ( _tracker == 0 ) {
  //    patrec_bias = (P + 1.4) / P;
  //  } else {
  //    patrec_bias = (P - 1.4) / P;
  //  }
  //  patrec_momentum = patrec_bias * patrec_momentum;

    double x = x0 + r*cos(phi_0);
    double px = patrec_momentum.x();
    double y = y0 + r*sin(phi_0);
    double py = patrec_momentum.y();
    double kappa = _particle_charge / patrec_momentum.z();

    TMatrixD a(_n_parameters, 1);
    vector(0, 0) = x;
    vector(1, 0) = px; 
    vector(2, 0) = y;
    vector(3, 0) = py;
    vector(4, 0) = kappa;

//
// METHOD = ED SANTOS
//
    if (!_use_patrec_seed) {
      TMatrixD covariance(_n_parameters, _n_parameters);
      covariance.Zero();
      for ( int i = 0; i < _n_parameters; ++i ) {
        covariance(i, i) = _seed_cov;
      }
    } else {
//
// METHOD = CHRISTOPHER HUNT
//
      std::vector<double> cov = seed->get_covariance();
      TMatrixD patrec_covariance(_n_parameters, _n_parameters);
      if (cov.size() != (unsigned int)_n_parameters*_n_parameters) {
        throw MAUS::Exception(MAUS::Exception::recoverable, 
                              "Dimension of covariance matrix does not match the state vector",
                              "KalmanSeed::ComputeInitalCovariance(Helical)");
      }

      double mc = particle_charge*c*Bz; // Magnetic constant
      double sin = std::sin(phi0);
      double cos = std::cos(phi0);
      double sin_plus = std::sin(phi);
      double cos_plus = std::cos(phi);

      TMatrixD jacobian(_n_parameters, _n_parameters);
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

      TMatrixD jacobianT(_n_parameters, _n_parameters);
      jacobianT.Transpose( jacobian );

      for (int i = 0; i < _n_parameters; ++i) {
        for (int j = 0; j < _n_parameters; ++j) {
          patrec_covariance(i, j) = cov.at(i*_n_parameters + j);
        }
      }
      covariance = jacobian*patrec_covariance*jacobianT;
    }

    Kalman::State seed_state(vector, covariance);
    return seed_state;
  }


  Kalman::State ComputeSeed(SciFiStraightPRTrack* s_track, SciFiGeometryHelper* geom) {
    TMatrixD vector(1, 4);
    TMatrixD covariance(4, 4);

    // Length of tracker
    double length = 1100.0;

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
    if (!_use_patrec_seed) {
      TMatrixD covariance(_n_parameters, _n_parameters);
      covariance.Zero();
      for ( int i = 0; i < _n_parameters; ++i ) {
        covariance(i, i) = _seed_cov;
      }
    } else {
//
// METHOD = CHRISTOPHER HUNT
//
      std::vector<double> cov = seed->get_covariance();
      TMatrixD patrec_covariance(_n_parameters, _n_parameters);
      TMatrixD covariance(_n_parameters, _n_parameters);

      if (cov.size() != (unsigned int)_n_parameters*_n_parameters) {
        throw MAUS::Exception( MAUS::Exception::recoverable, 
                              "Dimension of covariance matrix does not match the state vector",
                              "KalmanSeed::ComputeInitalCovariance(Straight)");
      }

      TMatrixD jacobian( _n_parameters, _n_parameters );
      jacobian(0,0) = 1.0;
      jacobian(1,1) = 1.0;
      jacobian(2,2) = 1.0;
      jacobian(3,3) = 1.0;
      jacobian(0,1) = 1100.0; // TODO: Read the correct value from the geometry
      jacobian(2,3) = 1100.0;

      TMatrixD jacobianT(_n_parameters, _n_parameters);
      jacobianT.Transpose( jacobian );

      for ( int i = 0; i < _n_parameters; ++i ) {
        for ( int j = 0; j < _n_parameters; ++j ) {
          patrec_covariance(i,j) = cov.at( i*_n_parameters + j );
        }
      }

      covariance = jacobian*patrec_covariance*jacobianT;
    }

    Kalman::State seed_state(vector, covariance);
    return seed_state;
  }


  SciFiTrack* CovertToSciFiTrack(Kalman::Track& k_track, SciFiGeometryHelper* geom) {
    SciFiTrack* new_track = new SciFiTrack();
    
    if ( k_track[0].GetId() > 0 ) {
      new_track->SetTracker( 1 ); 
    } else {
      new_track->SetTracker( 0 );
    }

    for ( unsigned int i = 0; i < k_track.GetLength(); ++i ) {
      SciFiTrackPoint new_point = new SciFiTrackPoint();
      Kalman::State& current_state = k_track[i];

      TMatrixD state_vector = current_state.GetState();
      int dimension = state_vector.GetNrows();
      ThreeVector pos;
      ThreeVector mom;

      if ( dimension == 4 ) {
        pos.setZ(current_state.GetPosition());
        mom.setZ(200.0); // MeV/c
        pos.setX(state_vector(0, 0));
        mom.setX(state_vector(1, 0));
        pos.setY(state_vector(2, 0));
        mom.setY(state_vector(3, 0));
      } else if ( dimension == 5 ) {
        pos.setX(state_vector(0, 0));
        mom.setX(state_vector(1, 0));
        pos.setY(state_vector(2, 0));
        mom.setY(state_vector(3, 0));
        pos.setZ(current_state.GetPosition());
        mom.setZ(1./fabs(state_vector(4, 0)));
      }
      new_point->set_pos(pos);
      new_point->set_mom(mom);
      
      TMatrixD C = current_state.GetCovariance();
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
      new_track->add_scifitrackpoint(new_point);
    }

// TODO:
// - Set Cluster
// - Set tracker properly
// - Calculate ChiSquared
// - Calculate p-value
// - Set Algorithm used
// - Set Seed Info
// - Init track before the fit?

    return new_track;
  }
}

