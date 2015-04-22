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
  _size_exception  = (*json)["SciFiClustExcept"].asInt();
  _min_npe         = (*json)["SciFiNPECut"].asDouble();
  _use_mcs         = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_eloss       = (*json)["SciFiKalman_use_Eloss"].asBool();
  _use_patrec_seed = (*json)["SciFiKalman_use_seed_PatRec"].asBool();

  _cluster_recon = SciFiClusterRec(_size_exception, _min_npe, _geometry_helper.GeometryMap());

  _spacepoint_recon = SciFiSpacePointRec();

  MiceModule* module = Globals::GetReconstructionMiceModules();
  std::vector<const MiceModule*> modules =
    module->findModulesByPropertyString("SensitiveDetector", "SciFi");
  _geometry_helper = SciFiGeometryHelper(modules);
  _geometry_helper.Build();

  _pattern_recognition.set_helical_pr_on(_helical_pr_on);
  _pattern_recognition.set_straight_pr_on(_straight_pr_on);
  _pattern_recognition.set_bz_t1(fabs(_geometry_helper.GetFieldValue(0)));
  _pattern_recognition.set_bz_t2(fabs(_geometry_helper.GetFieldValue(1)));

#ifdef KALMAN_TEST
  HelicalPropagator* spacepoint_helical_prop = new HelicalPropagator(&_geometry_helper);
  spacepoint_helical_prop->SetIncludeMCS( _use_mcs );
  spacepoint_helical_prop->SetSubtractELoss( _use_eloss );
  _spacepoint_helical_track_fitter = new Kalman::TrackFit(spacepoint_helical_prop,
      new SciFiSpacepointMeasurements<5>(1.0));

  StraightPropagator* spacepoint_straight_prop = new StraightPropagator(&_geometry_helper);
  spacepoint_straight_prop->SetIncludeMCS( _use_mcs );
  _spacepoint_straight_track_fitter = new Kalman::TrackFit(spacepoint_straight_prop,
      new SciFiSpacepointMeasurements<4>(1.0));
#else
  HelicalPropagator* helical_prop = new HelicalPropagator(&_geometry_helper);
  helical_prop->SetIncludeMCS( _use_mcs );
  helical_prop->SetSubtractELoss( _use_eloss );
  _helical_track_fitter = new Kalman::TrackFit( helical_prop, new SciFiHelicalMeasurements(&_geometry_helper));

  StraightPropagator* straight_prop = new StraightPropagator(&_geometry_helper);
  straight_prop->SetIncludeMCS( _use_mcs );
  _straight_track_fitter = new Kalman::TrackFit(straight_prop, new SciFiStraightMeasurements(&_geometry_helper));
#endif

  std::cerr << "Tracker 0 Field = " << _geometry_helper.GetFieldValue(0) << "\n"
            << "Tracker 1 Field = " << _geometry_helper.GetFieldValue(1) << "\n\n";
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
      if ( event->spacepoints().size() ) {
        _pattern_recognition.process(*event);
      }
      // Kalman Track Fit.
      if ( _kalman_on ) {
        if ( event->straightprtracks().size() || event->helicalprtracks().size() ) {
          track_fit(*event);
        }
      }
      print_event_info(*event);
    }
  } else {
    std::cout << "No recon events found\n";
  }
}

void MapCppTrackerRecon::track_fit(SciFiEvent &evt) const {
#ifdef KALMAN_TEST
  size_t number_helical_tracks = evt.helicalprtracks().size();
  size_t number_straight_tracks = evt.straightprtracks().size();

  for ( size_t track_i = 0; track_i < number_helical_tracks; track_i++ ) {
    SciFiHelicalPRTrack* helical = evt.helicalprtracks().at(track_i);
    SciFiSpacePointPArray spacepoints = helical->get_spacepoints_pointers();

    Kalman::Track data_track = BuildSpacepointTrack(spacepoints, &_geometry_helper);

//    std::cerr << "Data Track\n";
//    std::cerr << Kalman::print_track(data_track);

    Kalman::State seed = ComputeSeed(helical, &_geometry_helper);

    std::cerr << Kalman::print_state(seed, "Helical Seed" );

    std::cerr << "Helical Track\n";
    print_helical( helical );

    _spacepoint_helical_track_fitter->SetData(data_track);
    _spacepoint_helical_track_fitter->SetSeed(seed);

    _spacepoint_helical_track_fitter->Filter(false);
    _spacepoint_helical_track_fitter->Smooth(false);

    Kalman::Track predicted = _spacepoint_helical_track_fitter->GetPredicted();
    Kalman::Track filtered = _spacepoint_helical_track_fitter->GetFiltered();
    Kalman::Track smoothed = _spacepoint_helical_track_fitter->GetSmoothed();
    std::cerr << "Data Track\n";
    std::cerr << Kalman::print_track(data_track);
    std::cerr << "Predicted Track\n";
    std::cerr << Kalman::print_track(predicted);
    std::cerr << "Filtered Track\n";
    std::cerr << Kalman::print_track(filtered);
    std::cerr << "Smoothed Track\n";
    std::cerr << Kalman::print_track(smoothed);
    std::cerr << "Measured Filtered\n";
    SciFiSpacepointMeasurements<5>* temp_measurement = new SciFiSpacepointMeasurements<5>(1.0);
    for ( unsigned int k = 0; k < filtered.GetLength(); ++k )
    {
      Kalman::State temp_state = temp_measurement->Measure(filtered[k]);
      std::cerr << Kalman::print_state( temp_state );
    }

    SciFiTrack* track = ConvertToSciFiTrack(smoothed, &_geometry_helper);
//    SciFiTrack* track = ConvertToSciFiTrack(_spacepoint_helical_track_fitter->GetFiltered(),
//                                                             &_geometry_helper);
    evt.add_scifitrack(track);
  }
  for ( size_t track_i = 0; track_i < number_straight_tracks; track_i++ ) {
    SciFiStraightPRTrack* straight = evt.straightprtracks().at(track_i);
    SciFiSpacePointPArray spacepoints = straight->get_spacepoints_pointers();

    Kalman::Track data_track = BuildSpacepointTrack(spacepoints, &_geometry_helper);
    std::cerr << "Data Track\n";
    std::cerr << Kalman::print_track(data_track);

    Kalman::State seed = ComputeSeed(straight, &_geometry_helper, 100.0);
    std::cerr << "Straight Track\n";
    print_straight( straight );

    _spacepoint_straight_track_fitter->SetData(data_track);
    _spacepoint_straight_track_fitter->SetSeed(seed);

    _spacepoint_straight_track_fitter->Filter(false);
    _spacepoint_straight_track_fitter->Smooth(false);

    Kalman::Track filtered = _spacepoint_straight_track_fitter->GetFiltered();
    Kalman::Track smoothed = _spacepoint_straight_track_fitter->GetSmoothed();
    std::cerr << "Filtered Track\n";
    std::cerr << Kalman::print_track(filtered);
    std::cerr << "\nSmoothed Track\n";
    std::cerr << Kalman::print_track(smoothed);

    SciFiTrack* track = ConvertToSciFiTrack(smoothed, &_geometry_helper);
    evt.add_scifitrack(track);
  }
#else
  size_t number_helical_tracks = evt.helicalprtracks().size();
  size_t number_straight_tracks = evt.straightprtracks().size();
  for ( size_t track_i = 0; track_i < number_helical_tracks; track_i++ ) {
    SciFiHelicalPRTrack* helical = evt.helicalprtracks().at(track_i);

    Kalman::Track data_track = BuildTrack(helical, &_geometry_helper);

    Kalman::State seed = ComputeSeed(helical, &_geometry_helper, 100.0);

    _helical_track_fitter->SetData(data_track);
    _helical_track_fitter->SetSeed(seed);

    _helical_track_fitter->Filter(false);
    _helical_track_fitter->Smooth(false);

    SciFiTrack* track = ConvertToSciFiTrack(_helical_track_fitter->GetSmoothed(),
                                                             &_geometry_helper);
    evt.add_scifitrack(track);
  }
  for ( size_t track_i = 0; track_i < number_straight_tracks; track_i++ ) {
    SciFiStraightPRTrack* straight = evt.straightprtracks().at(track_i);
    Kalman::Track data_track = BuildTrack(straight, &_geometry_helper);

    Kalman::State seed = ComputeSeed(straight, &_geometry_helper);

    continue;

    std::cerr << Kalman::print_track( data_track);

    _straight_track_fitter->SetData(data_track);
    _straight_track_fitter->SetSeed(seed);

    _straight_track_fitter->Filter();
    _straight_track_fitter->Smooth();

    SciFiTrack* track = ConvertToSciFiTrack(_straight_track_fitter->GetSmoothed(),
                                                             &_geometry_helper);
    evt.add_scifitrack(track);
  }
#endif
}

void MapCppTrackerRecon::print_event_info(SciFiEvent &event) const {
  /*
  std::cerr << event.digits().size() << " "
                              << event.clusters().size() << " "
                              << event.spacepoints().size() << "; "
                              << event.straightprtracks().size() << " "
                              << event.helicalprtracks().size() << "; ";
  for ( size_t track_i = 0; track_i < event.scifitracks().size(); track_i++ ) {
    std::cerr << " Chi2: " << event.scifitracks()[track_i]->f_chi2() << "; "
                           << " Chi2: " << event.scifitracks()[track_i]->s_chi2() << "; "
                           << " P-Value: " << event.scifitracks()[track_i]->P_value() << "; ";
  }
  std::cerr << std::endl;
  */

  /*
  SciFiHelicalPRTrackPArray helices = event.helicalprtracks();
  for ( unsigned int i = 0; i < helices.size(); ++i )
  {
    std::cerr << "Helix Charge = " << helices[i]->get_charge() << '\n';
  }
  SciFiStraightPRTrackPArray straights = event.straightprtracks();
  for ( unsigned int i = 0; i < straights.size(); ++i )
  {
    std::cerr << "Straight Track Found!" << '\n';
  }
  SciFiTrackPArray tracks = event.scifitracks();
  for ( unsigned int i = 0; i < tracks.size(); ++i )
  {
    std::cerr << "Track Charge = " << tracks[i]->charge() << '\n';
  }
  std::cerr << std::endl;
  */

  std::cerr << "\nSciFi Event Details:\n\n";

  SciFiTrackPArray tracks = event.scifitracks();
  for ( unsigned int i = 0; i < tracks.size(); ++i )
  {
    SciFiTrackPointPArray trackpoints = tracks[i]->scifitrackpoints();
    for ( unsigned int j = 0; j < trackpoints.size(); ++j )
    {
//      std::cerr << "Plane, Station, Tracker = " << trackpoints[j]->plane() << ", " << trackpoints[j]->station() << ", " << trackpoints[j]->tracker() << '\n';
//      SciFiCluster* cluster = trackpoints[j]->get_cluster_pointer();

      std::cerr << "Track Point = " << trackpoints[j]->pos()[0] << ", " << trackpoints[j]->pos()[1] << ", " << trackpoints[j]->pos()[2] << " | "
                                    << trackpoints[j]->mom()[0] << ", " << trackpoints[j]->mom()[1] << ", " << trackpoints[j]->mom()[2] << "\n";// << " | "
//                                    << "DATA = " << ( cluster ? cluster->get_alpha() : 0 ) << '\n';
    }
    std::cerr << "Tracker : " << tracks[i]->tracker() << ", Chi2 = " << tracks[i]->chi2()
              << ", P_Value = " << tracks[i]->P_value() << ", Charge = " << tracks[i]->charge() <<  '\n';

    std::vector<double> seed_sta = tracks[i]->GetSeedState();
    std::vector<double> seed_cov = tracks[i]->GetSeedCovariance();

    std::cerr << "Seed State = ";
    for ( unsigned int j = 0; j < seed_sta.size(); ++j ) {
      std::cerr << seed_sta[j] << ", ";
    }
    std::cerr << '\n';

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
} // ~namespace MAUS
