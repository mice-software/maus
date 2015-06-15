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

#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/map/MapCppTrackerRecon/MapCppTrackerRecon.hh"
#include "src/common_cpp/Recon/Kalman/MAUSTrackWrapper.hh"

#include <sstream>

namespace MAUS {

//void print_state( Kalman::State& state );

void print_helical( SciFiHelicalPRTrack* helical );
void print_straight( SciFiStraightPRTrack* straight );

SciFiClusterPArray find_clusters( SciFiClusterPArray cluster_array, int tracker );
SciFiSpacePointPArray find_spacepoints( SciFiSpacePointPArray spacepoint_array, int tracker );


PyMODINIT_FUNC init_MapCppTrackerRecon(void) {
  PyWrapMapBase<MapCppTrackerRecon>::PyWrapMapBaseModInit
                                        ("MapCppTrackerRecon", "", "", "", "");
}

MapCppTrackerRecon::MapCppTrackerRecon() : MapBase<Data>("MapCppTrackerRecon") {
}

MapCppTrackerRecon::~MapCppTrackerRecon() {
}

void MapCppTrackerRecon::_birth(const std::string& argJsonConfigDocument) {
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  Json::Value* json = Globals::GetConfigurationCards();
  _helical_pr_on   = (*json)["SciFiPRHelicalOn"].asBool();
  _straight_pr_on  = (*json)["SciFiPRStraightOn"].asBool();
  _kalman_on       = (*json)["SciFiKalmanOn"].asBool();
  _patrec_on       = (*json)["SciFiPatRecOn"].asBool();
  _size_exception  = (*json)["SciFiClustExcept"].asInt();
  _min_npe         = (*json)["SciFiNPECut"].asDouble();
  _use_mcs         = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_eloss       = (*json)["SciFiKalman_use_Eloss"].asBool();
  _use_patrec_seed = (*json)["SciFiSeedPatRec"].asBool();
  _correct_pz      = (*json)["SciFiKalmanCorrectPz"].asBool();

  MiceModule* module = Globals::GetReconstructionMiceModules();
  std::vector<const MiceModule*> modules =
    module->findModulesByPropertyString("SensitiveDetector", "SciFi");
  _geometry_helper = SciFiGeometryHelper(modules);
  _geometry_helper.Build();


  _cluster_recon = SciFiClusterRec(_size_exception, _min_npe, _geometry_helper.GeometryMap());

  _spacepoint_recon = SciFiSpacePointRec();

  _pattern_recognition.set_helical_pr_on(_helical_pr_on);
  _pattern_recognition.set_straight_pr_on(_straight_pr_on);
  _pattern_recognition.set_bz_t1(_geometry_helper.GetFieldValue(0));
  _pattern_recognition.set_bz_t2(_geometry_helper.GetFieldValue(1));

  if (_use_patrec_seed) {
    _seed_value = -1.0;
  } else {
    _seed_value = (*json)["SciFiSeedCovariance"].asDouble();
  }

#ifdef KALMAN_TEST
  HelicalPropagator* spacepoint_helical_prop = new HelicalPropagator(&_geometry_helper);
  spacepoint_helical_prop->SetCorrectPz(_correct_pz);
  spacepoint_helical_prop->SetIncludeMCS( _use_mcs );
  spacepoint_helical_prop->SetSubtractELoss( _use_eloss );
  _spacepoint_helical_track_fitter = new Kalman::TrackFit(spacepoint_helical_prop,
      new SciFiSpacepointMeasurements<5>(0.1));

  StraightPropagator* spacepoint_straight_prop = new StraightPropagator(&_geometry_helper);
  spacepoint_straight_prop->SetIncludeMCS( _use_mcs );
  _spacepoint_straight_track_fitter = new Kalman::TrackFit(spacepoint_straight_prop,
      new SciFiSpacepointMeasurements<4>(0.1));

  _spacepoint_recon_plane = 0;
#else
  HelicalPropagator* helical_prop = new HelicalPropagator(&_geometry_helper);
  helical_prop->SetCorrectPz(_correct_pz);
  helical_prop->SetIncludeMCS( _use_mcs );
  helical_prop->SetSubtractELoss( _use_eloss );
  _helical_measurement = new SciFiHelicalMeasurements(&_geometry_helper);
  _helical_track_fitter = new Kalman::TrackFit(helical_prop, _helical_measurement);

  StraightPropagator* straight_prop = new StraightPropagator(&_geometry_helper);
  straight_prop->SetIncludeMCS( _use_mcs );
  _straight_measurement = new SciFiStraightMeasurements(&_geometry_helper);
  _straight_track_fitter = new Kalman::TrackFit(straight_prop, _straight_measurement);
#endif
}

void MapCppTrackerRecon::_death() {
}

void MapCppTrackerRecon::_process(Data* data) const {
  Spill& spill = *(data->GetSpill());

  if ( spill.GetReconEvents() ) {
    for ( unsigned int k = 0; k < spill.GetReconEvents()->size(); k++ ) {
      SciFiEvent *event = spill.GetReconEvents()->at(k)->GetSciFiEvent();

      // Build Clusters.
      if ( event->digits().size() ) {
        _cluster_recon.process(*event);
      }
      // Build SpacePoints.
      if ( event->clusters().size() ) {
        _spacepoint_recon.process(*event);
      }
      // Pattern Recognition.
      if ( _patrec_on && event->spacepoints().size() ) {
        _pattern_recognition.process(*event);
        extrapolate_helical_reference(*event);
        extrapolate_straight_reference(*event);
      }
      // Kalman Track Fit.
      if ( _kalman_on ) {
        track_fit(*event);
      }
//      print_event_info(*event);
    }
  } else {
    std::cout << "No recon events found\n";
  }
}

void MapCppTrackerRecon::track_fit(SciFiEvent &evt) const {
#ifdef KALMAN_TEST
  std::cerr << "KALMAN_TEST ACTIVE\n";
  if ( _patrec_on ) {
    std::cerr << "PATREC ACTIVE\n";
    size_t number_helical_tracks = evt.helicalprtracks().size();
    size_t number_straight_tracks = evt.straightprtracks().size();

    for ( size_t track_i = 0; track_i < number_helical_tracks; track_i++ ) {
      SciFiHelicalPRTrack* helical = evt.helicalprtracks().at(track_i);
      SciFiSpacePointPArray spacepoints = helical->get_spacepoints_pointers();

      Kalman::Track data_track = BuildSpacepointTrack(spacepoints, &_geometry_helper, _spacepoint_recon_plane);

      Kalman::State seed = ComputeSeed(helical, &_geometry_helper, false, _seed_value);

      std::cerr << Kalman::print_state(seed, "Helical Seed" );
      seed.GetCovariance().Print();

      std::cerr << "Helical Track\n";
      print_helical( helical );

      _spacepoint_helical_track_fitter->SetSeed(seed);
      _spacepoint_helical_track_fitter->SetData(data_track);

      _spacepoint_helical_track_fitter->Filter(false);
      _spacepoint_helical_track_fitter->Smooth(false);

      Kalman::Track predicted = _spacepoint_helical_track_fitter->GetPredicted();
      Kalman::Track filtered = _spacepoint_helical_track_fitter->GetFiltered();
      Kalman::Track smoothed = _spacepoint_helical_track_fitter->GetSmoothed();
      std::cerr << "Data Track\n";
      std::cerr << Kalman::print_track(data_track);
      std::cerr << "Measured Predicted\n";
      SciFiSpacepointMeasurements<5>* temp_measurement = new SciFiSpacepointMeasurements<5>(1.0);
      for ( unsigned int k = 0; k < predicted.GetLength(); ++k )
      {
        Kalman::State temp_state = temp_measurement->Measure(predicted[k]);
        std::cerr << Kalman::print_state( temp_state );
      }
      std::cerr << "Predicted Track\n";
      std::cerr << Kalman::print_track(predicted);
      std::cerr << "Filtered Track\n";
      std::cerr << Kalman::print_track(filtered);
      std::cerr << "Smoothed Track\n";
      std::cerr << Kalman::print_track(smoothed);
      delete temp_measurement;

      SciFiTrack* track = ConvertToSciFiTrack(_spacepoint_helical_track_fitter, &_geometry_helper);
  //    SciFiTrack* track = ConvertToSciFiTrack(_spacepoint_helical_track_fitter->GetFiltered(),
  //                                                             &_geometry_helper);
      evt.add_scifitrack(track);
    }
    for ( size_t track_i = 0; track_i < number_straight_tracks; track_i++ ) {
      SciFiStraightPRTrack* straight = evt.straightprtracks().at(track_i);
      SciFiSpacePointPArray spacepoints = straight->get_spacepoints_pointers();

      Kalman::Track data_track = BuildSpacepointTrack(spacepoints, &_geometry_helper, _spacepoint_recon_plane);
      Kalman::State seed = ComputeSeed(straight, &_geometry_helper, _seed_value);

      std::cerr << Kalman::print_state(seed, "Helical Seed" );
      std::cerr << "Straight Track\n";
      print_straight( straight );

      _spacepoint_straight_track_fitter->SetSeed(seed);
      _spacepoint_straight_track_fitter->SetData(data_track);

      _spacepoint_straight_track_fitter->Filter(false);
      _spacepoint_straight_track_fitter->Smooth(false);

      Kalman::Track predicted = _spacepoint_straight_track_fitter->GetPredicted();
      Kalman::Track filtered = _spacepoint_straight_track_fitter->GetFiltered();
      Kalman::Track smoothed = _spacepoint_straight_track_fitter->GetSmoothed();
      std::cerr << "Data Track\n";
      std::cerr << Kalman::print_track(data_track);
      std::cerr << "Predicted Track\n";
      std::cerr << Kalman::print_track(predicted);
      std::cerr << "Filtered Track\n";
      std::cerr << Kalman::print_track(filtered);
      std::cerr << "Smoothed Track\n";
      std::cerr << Kalman::print_track(smoothed);

      SciFiTrack* track = ConvertToSciFiTrack(_spacepoint_straight_track_fitter, &_geometry_helper);
      evt.add_scifitrack(track);
    }
  } else { // PATREC Not Active
    std::cerr << "PATREC NOT ACTIVE\n";

    SciFiSpacePointPArray event_spacepoints = evt.spacepoints();
    for ( int tracker = 0; tracker < 2; ++tracker ) {

      SciFiSpacePointPArray spacepoints = find_spacepoints( event_spacepoints, tracker );
      std::cerr << " USING " << spacepoints.size() << " SPACEPOINTS from TRACKER : " << tracker << "\n";
      if ( spacepoints.size() != 5 ) break;

      int id = ((4*3 + 1) + _spacepoint_recon_plane) * ( tracker == 0 ? -1 : 1 );
      Kalman::Track data_track = BuildSpacepointTrack( spacepoints, &_geometry_helper, _spacepoint_recon_plane);
      Kalman::State seed(5);
      seed.SetId(id);

      bool seed_found = false;
      for ( SciFiSpacePointPArray::iterator it_s = spacepoints.begin(); it_s != spacepoints.end(); ++it_s ) {
        if ( (*it_s)->get_station() == 5 ) {

          ThreeVector pos = (*it_s)->get_true_position();
          ThreeVector mom = (*it_s)->get_true_momentum();
          std::cerr << pos.x() << ", " << pos.y() << ", " << pos.z() << " ;  " << mom.x() << ", " << mom.y() << ", " << mom.z() << "\n";

          TMatrixD state( 5, 1 );
          state( 0, 0 ) = pos.x();
          state( 1, 0 ) = mom.x();
          state( 2, 0 ) = pos.y();
          state( 3, 0 ) = mom.y();
          state( 4, 0 ) = 1.0 / mom.z();

          seed.SetVector( state );
          seed.SetPosition(pos.z());
          seed_found = true;
          break;
        }
      }
      if (! seed_found) continue;

      TMatrixD seed_cov( 5, 5 );
      seed_cov(0,0) = _seed_value;
      seed_cov(1,1) = _seed_value;
      seed_cov(2,2) = _seed_value;
      seed_cov(3,3) = _seed_value;
      seed_cov(4,4) = _seed_value;
      seed.SetCovariance(seed_cov);

      std::cerr << Kalman::print_state(seed, "Helical Seed" );

      _spacepoint_helical_track_fitter->SetSeed(seed);
      _spacepoint_helical_track_fitter->SetData(data_track);

      _spacepoint_helical_track_fitter->Filter(false);
      _spacepoint_helical_track_fitter->Smooth(false);

      Kalman::Track predicted = _spacepoint_helical_track_fitter->GetPredicted();
      Kalman::Track filtered = _spacepoint_helical_track_fitter->GetFiltered();
      Kalman::Track smoothed = _spacepoint_helical_track_fitter->GetSmoothed();
      std::cerr << "Data Track\n";
      std::cerr << Kalman::print_track(data_track);
      std::cerr << "Measured Predicted\n";
      SciFiSpacepointMeasurements<5>* temp_measurement = new SciFiSpacepointMeasurements<5>(1.0);
      for ( unsigned int k = 0; k < predicted.GetLength(); ++k )
      {
        Kalman::State temp_state = temp_measurement->Measure(predicted[k]);
        std::cerr << Kalman::print_state( temp_state );
      }
      std::cerr << "Predicted Track\n";
      std::cerr << Kalman::print_track(predicted);
      std::cerr << "Filtered Track\n";
      std::cerr << Kalman::print_track(filtered);
      std::cerr << "Smoothed Track\n";
      std::cerr << Kalman::print_track(smoothed);
      delete temp_measurement;

      SciFiTrack* track = ConvertToSciFiTrack(_spacepoint_helical_track_fitter, &_geometry_helper);
  //    SciFiTrack* track = ConvertToSciFiTrack(_spacepoint_helical_track_fitter->GetFiltered(),
  //                                                             &_geometry_helper);
      evt.add_scifitrack(track);
    }
  }
#else
  if ( _patrec_on ) {
    size_t number_helical_tracks = evt.helicalprtracks().size();
    size_t number_straight_tracks = evt.straightprtracks().size();
    for ( size_t track_i = 0; track_i < number_helical_tracks; track_i++ ) {
      SciFiHelicalPRTrack* helical = evt.helicalprtracks().at(track_i);

      Kalman::Track data_track = BuildTrack(helical, &_geometry_helper);
      Kalman::State seed = ComputeSeed(helical, &_geometry_helper, _use_eloss, _seed_value);

      _helical_track_fitter->SetData(data_track);
      _helical_track_fitter->SetSeed(seed);

      _helical_track_fitter->Filter(false);
      _helical_track_fitter->Smooth(false);

      SciFiTrack* track = ConvertToSciFiTrack(_helical_track_fitter, &_geometry_helper);

      evt.add_scifitrack(track);
    }
    for ( size_t track_i = 0; track_i < number_straight_tracks; track_i++ ) {
      SciFiStraightPRTrack* straight = evt.straightprtracks().at(track_i);
      Kalman::Track data_track = BuildTrack(straight, &_geometry_helper);

      Kalman::State seed = ComputeSeed(straight, &_geometry_helper, _seed_value);

      _straight_track_fitter->SetData(data_track);
      _straight_track_fitter->SetSeed(seed);

      _straight_track_fitter->Filter(false);
      _straight_track_fitter->Smooth(false);

      SciFiTrack* track = ConvertToSciFiTrack(_straight_track_fitter, &_geometry_helper);

      evt.add_scifitrack(track);
    }
  } else { // PatRec Not Switched on. => Assume EVERYTHING is a single track.
    SciFiClusterPArray event_clusters = evt.clusters();
    for ( int tracker = 0; tracker < 2; ++tracker ) {

      SciFiClusterPArray clusters = find_clusters( event_clusters, tracker );
      if ( clusters.size() != 15 ) break;

      Kalman::Track data_track = BuildTrack( clusters, &_geometry_helper );
      Kalman::State seed(5);

      bool seed_found = false;
      for ( SciFiClusterPArray::iterator it_c = clusters.begin(); it_c != clusters.end(); ++it_c ) {
        if ( ( (*it_c)->get_station() == 5 ) && ( (*it_c)->get_plane() == 2 ) ) {

          ThreeVector pos = (*it_c)->get_true_position();
          ThreeVector mom = (*it_c)->get_true_momentum();

          TMatrixD state( 5, 1 );
          state( 0, 0 ) = pos.x();
          state( 1, 0 ) = mom.x();
          state( 2, 0 ) = pos.y();
          state( 3, 0 ) = mom.y();
          state( 4, 0 ) = 1.0 / mom.z();

          seed.SetVector( state );
          seed.SetPosition(pos.z());
          seed_found = true;
          break;
        }
      }
      if (! seed_found) continue;

      TMatrixD seed_cov( 5, 5 );
      seed_cov(0,0) = _seed_value;
      seed_cov(1,1) = _seed_value;
      seed_cov(2,2) = _seed_value;
      seed_cov(3,3) = _seed_value;
      seed_cov(4,4) = _seed_value;
      seed.SetCovariance(seed_cov);

      _helical_track_fitter->SetData(data_track);
      _helical_track_fitter->SetSeed(seed);

      _helical_track_fitter->Filter(false);
      _helical_track_fitter->Smooth(false);

      SciFiTrack* track = ConvertToSciFiTrack(_helical_track_fitter, &_geometry_helper);

      evt.add_scifitrack(track);
    }
  }
#endif
}

void MapCppTrackerRecon::extrapolate_helical_reference(SciFiEvent& event) const {
  SciFiHelicalPRTrackPArray helicals = event.helicalprtracks();
  size_t num_tracks = helicals.size();

  for ( size_t i = 0; i < num_tracks; ++i ) {
    SciFiHelicalPRTrack* track = helicals[i];
    ThreeVector pos;
    ThreeVector mom;

    int tracker = track->get_tracker();
    ThreeVector reference = _geometry_helper.GetReferencePosition(tracker);
    CLHEP::HepRotation rotation = _geometry_helper.GetReferenceRotation(tracker);
    rotation.invert();

    double r  = track->get_R();
    double pt = - track->get_charge()*CLHEP::c_light*_geometry_helper.GetFieldValue(tracker)*r;
    double dsdz = - track->get_dsdz();
    double x0 = track->get_circle_x0();
    double y0 = track->get_circle_y0();
    double s = track->get_line_sz_c();
    double phi = s / r;

    pos.setX(x0 + r*cos(phi));
    pos.setY(y0 + r*sin(phi));
    pos.setZ(0.0);
    pos *= rotation;
    pos += reference;

    mom.setX(-pt*sin(phi));
    mom.setY(pt*cos(phi));
    mom.setZ(-pt/dsdz);
    mom *= rotation;

    track->set_reference_position(pos);
    track->set_reference_momentum(mom);
  }
}

void MapCppTrackerRecon::extrapolate_straight_reference(SciFiEvent& event) const {
  SciFiStraightPRTrackPArray straights = event.straightprtracks();
  size_t num_tracks = straights.size();

  for ( size_t i = 0; i < num_tracks; ++i ) {
    SciFiStraightPRTrack* track = straights[i];
    ThreeVector pos;
    ThreeVector mom;

    int tracker = track->get_tracker();
    ThreeVector reference = _geometry_helper.GetReferencePosition(tracker);
    CLHEP::HepRotation rotation = _geometry_helper.GetReferenceRotation(tracker);
    rotation.invert();

    pos.setX(track->get_x0());
    pos.setY(track->get_y0());
    pos.setZ(0.0);
    pos *= rotation;
    pos += reference;

    double mx = track->get_mx();
    double my = track->get_my();
    mom.setX(mx*200.0);
    mom.setY(my*200.0);
    mom *= rotation;
    mom.setZ(sqrt( 40000.0 - mom[0]*mom[0] + mom[1]*mom[1] ));

    track->set_reference_position(pos);
    track->set_reference_momentum(mom);
  }
}


void MapCppTrackerRecon::print_event_info(SciFiEvent &event) const {
  std::cerr << "\n ######  SciFi Event Details  ######\n\n";


  std::cerr << "Num Digits      : " << event.digits().size() << "\n"
            << "Num Clusters    : " << event.clusters().size() << "\n"
            << "Num Spacepoints : " << event.spacepoints().size() << "\n"
            << "Num Straights   : " << event.straightprtracks().size() << "\n"
            << "Num Helicals    : " << event.helicalprtracks().size() << "\n\n";

  SciFiTrackPArray tracks = event.scifitracks();
  for ( unsigned int i = 0; i < tracks.size(); ++i )
  {
    SciFiTrackPointPArray trackpoints = tracks[i]->scifitrackpoints();
    for ( unsigned int j = 0; j < trackpoints.size(); ++j )
    {
//      std::cerr << "Plane, Station, Tracker = " << trackpoints[j]->plane() << ", " << trackpoints[j]->station() << ", " << trackpoints[j]->tracker() << '\n';
//      SciFiCluster* cluster = trackpoints[j]->get_cluster_pointer();

      std::cerr << "Track Point = " << trackpoints[j]->pos()[0] << ", " << trackpoints[j]->pos()[1] << ", " << trackpoints[j]->pos()[2] << " | "
                                    << trackpoints[j]->mom()[0] << ", " << trackpoints[j]->mom()[1] << ", " << trackpoints[j]->mom()[2] << " | "
//                                    << "DATA = " << ( cluster ? cluster->get_alpha() : 0 ) << '\n
                                    << "(" << trackpoints[j]->pull() << ", " << trackpoints[j]->residual()
                                    << ", " << trackpoints[j]->smoothed_residual() << ")\n";
    }
    std::cerr << "Tracker : " << tracks[i]->tracker() << ", Chi2 = " << tracks[i]->chi2() << ", NDF = " << tracks[i]->ndf()
              << ", P_Value = " << tracks[i]->P_value() << ", Charge = " << tracks[i]->charge() << '\n';

    ThreeVector seed_pos = tracks[i]->GetSeedPosition();
    ThreeVector seed_mom = tracks[i]->GetSeedMomentum();
    std::vector<double> seed_cov = tracks[i]->GetSeedCovariance();

    std::cerr << "Seed State = (" << seed_pos[0] << ", " << seed_pos[1] << ", " << seed_pos[2] << ")  (" << seed_mom[0] << ", " << seed_mom[1] << ", " << seed_mom[2] << ")\n";

    std::cerr << "Seed Covariance = ";
    for ( unsigned int j = 0; j < seed_cov.size(); ++j ) {
      std::cerr << seed_cov[j] << ", ";
    }
    std::cerr << '\n';
  }
  std::cerr << std::endl;




  /*
  std::cerr << "Helix Pz Recon:\n";
  SciFiHelicalPRTrackPArray helices = event.helicalprtracks();
  for ( unsigned int i = 0; i < helices.size(); ++i )
  {
    double radius = helices[i]->get_R();
    double patrec_pt = 1.19*radius;
    double patrec_pz = patrec_pt / helices[i]->get_dsdz();
    std::cerr << "Pz = " << patrec_pz << '\n';

    DoubleArray phis = helices[i]->get_phi();
    double start_phi = phis[0];
    double end_phi = phis[ phis.size()-1 ];

    double delta_phi = end_phi - start_phi;

    double Z = ( 1100.0 * 2.0 * CLHEP::pi ) / delta_phi;
    double pz = ( 1.19 * Z ) / ( 2.0 * CLHEP::pi );

    std::cerr << "Pz = " << pz << "\n\n";

    for ( unsigned int j = 0; j < phis.size(); ++j )
    {
      std::cerr << phis[j] << '\n';
    }
  }
  std::cerr << std::endl;
  */

  /*
  Squeak::mout(Squeak::info) << event.digits().size() << " "
                              << event.clusters().size() << " "
                              << event.spacepoints().size() << "; "
                              << event.straightprtracks().size() << " "
                              << event.helicalprtracks().size() << "; ";
  for ( size_t track_i = 0; track_i < event.scifitracks().size(); track_i++ ) {
    Squeak::mout(Squeak::info) << " Chi2: " << event.scifitracks()[track_i]->f_chi2() << "; "
                 << " Chi2: " << event.scifitracks()[track_i]->s_chi2() << "; "
                 << " P-Value: " << event.scifitracks()[track_i]->P_value() << "; ";
  }
  Squeak::mout(Squeak::info) << std::endl;
  */
}

void print_helical( SciFiHelicalPRTrack* helical ) {
  SciFiSpacePointPArray spacepoints = helical->get_spacepoints_pointers();
  size_t numb_spacepoints = spacepoints.size();

  std::cerr << "HELICAL TRACK:\n";

  double r = helical->get_R();
  double sz = helical->get_line_sz_c();
  double charge = helical->get_charge();
  std::cerr << "Radius = " << r << " Sz = " << sz << " Charge = " << charge << " Phi_0 = " << sz/r << "\n";

  for ( size_t i = 0; i < numb_spacepoints; ++i ) {
    SciFiSpacePoint *spacepoint = spacepoints[i];

    std::cerr << spacepoint->get_tracker() << ", " 
              << spacepoint->get_station() << "  -  "
              << spacepoint->get_position().x() << ", "
              << spacepoint->get_position().y() << ", "
              << spacepoint->get_position().z() << "\n";
  }

  std::cerr << std::endl;
}

void print_straight( SciFiStraightPRTrack* straight ) {
  SciFiSpacePointPArray spacepoints = straight->get_spacepoints_pointers();
  size_t numb_spacepoints = spacepoints.size();

  std::cerr << "STRAIGHT TRACK:\n";
  for ( size_t i = 0; i < numb_spacepoints; ++i ) {
    SciFiSpacePoint *spacepoint = spacepoints[i];

    std::cerr << spacepoint->get_tracker() << ", " 
              << spacepoint->get_station() << "  -  "
              << spacepoint->get_position().x() << ", "
              << spacepoint->get_position().y() << ", "
              << spacepoint->get_position().z() << "\n";
  }

  std::cerr << std::endl;
}

SciFiClusterPArray find_clusters( SciFiClusterPArray cluster_array, int tracker ) {
  SciFiClusterPArray found_clusters;

  for ( SciFiClusterPArray::iterator cl_it = cluster_array.begin(); cl_it != cluster_array.end(); ++cl_it ) {
    if ( (*cl_it)->get_tracker() == tracker ) {
      found_clusters.push_back( (*cl_it) );
    }
  }

  return found_clusters;
}

SciFiSpacePointPArray find_spacepoints( SciFiSpacePointPArray spacepoint_array, int tracker ) {
  SciFiSpacePointPArray found_spacepoints;

  for ( SciFiSpacePointPArray::iterator sp_it = spacepoint_array.begin(); sp_it != spacepoint_array.end(); ++sp_it ) {
    if ( (*sp_it)->get_tracker() == tracker ) {
      found_spacepoints.push_back( (*sp_it) );
    }
  }

  return found_spacepoints;
}
} // ~namespace MAUS
