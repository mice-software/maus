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

namespace MAUS {
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
  _helical_pr_on  = (*json)["SciFiPRHelicalOn"].asBool();
  _straight_pr_on = (*json)["SciFiPRStraightOn"].asBool();
  _kalman_on      = (*json)["SciFiKalmanOn"].asBool();
  _size_exception = (*json)["SciFiClustExcept"].asInt();
  _min_npe        = (*json)["SciFiNPECut"].asDouble();

  MiceModule* module = Globals::GetReconstructionMiceModules();
  std::vector<const MiceModule*> modules =
    module->findModulesByPropertyString("SensitiveDetector", "SciFi");
  _geometry_helper = SciFiGeometryHelper(modules);
  _geometry_helper.Build();

  _helical_track_fitter = new Kalman::TrackFit(HelicalPropagator(_geometry_helper), SciFiMeasurements(_geometry_helper));
  _straight_track_fitter = new Kalman::TrackFit(StraightPropagator(_geometry_helper), SciFiMeasurements(_geometry_helper));
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
        cluster_recon(*event);
      }
      // Build SpacePoints.
      if ( event->clusters().size() ) {
        spacepoint_recon(*event);
      }
      // Pattern Recognition.
      if ( event->spacepoints().size() ) {
        pattern_recognition(*event);
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

void MapCppTrackerRecon::cluster_recon(SciFiEvent &evt) const {
  SciFiClusterRec clustering(_size_exception, _min_npe, _geometry_helper.GeometryMap());
  clustering.process(evt);
}

void MapCppTrackerRecon::spacepoint_recon(SciFiEvent &evt) const {
  SciFiSpacePointRec spacepoints;
  spacepoints.process(evt);
}

void MapCppTrackerRecon::pattern_recognition(SciFiEvent &evt) const {
  PatternRecognition pr1; // Pat rec constructor calls Globals again
  // We let the Map's helical and straight flags overide the interal pat rec variables for these,
  // (pulled by pat rec from Globals) as this way we can customise which type runs for
  // testing purposes.
  pr1.set_helical_pr_on(_helical_pr_on);
  pr1.set_straight_pr_on(_straight_pr_on);
  // std::cerr << "Bz T1 = " << _geometry_helper.GetFieldValue(0) << std::endl;
  // std::cerr << "Bz T2 = " << _geometry_helper.GetFieldValue(1) << std::endl;
  pr1.set_bz_t1(_geometry_helper.GetFieldValue(0));
  pr1.set_bz_t2(_geometry_helper.GetFieldValue(1));
  pr1.process(evt);
}

void MapCppTrackerRecon::track_fit(SciFiEvent &evt) const {
  size_t number_helical_tracks = evt.helicalprtracks().size();
  size_t number_straight_tracks = evt.straightprtracks().size();

  for ( size_t track_i = 0; track_i < number_helical_tracks; track_i++ ) {
    SciFiHelicalPRTrack* helical = evt.helicalprtracks()->at(track_i);
    Kalman::Track data_track = BuildTrack(helical, _geometry_helper);

    Kalman::State seed = ComputeSeed(helical, _geometry_helper);

    _helical_track_fitter->SetData(data_track);
    _helical_track_fitter->SetSeed(seed);

    _helical_track_fitter->Filter();
    _helical_track_fitter->Smooth();

    SciFiTrack* track = ConvertToSciFiTrack(_helical_track_fitter->GetSmoothed(),
                                                             _geometry_helper);
    evt.add_scifitrack(track);
  }
  for ( size_t track_i = 0; track_i < number_helical_tracks; track_i++ ) {
    SciFiStraightPRTrack* straight = evt.straightprtracks()->at(track_i);
    Kalman::Track data_track = BuildTrack(straight, _geometry_helper);

    Kalman::State seed = ComputeSeed(straight, _geometry_helper);

    _straight_track_fitter->SetData(data_track);
    _straight_track_fitter->SetSeed(seed);

    _straight_track_fitter->Filter();
    _straight_track_fitter->Smooth();

    SciFiTrack* track = ConvertToSciFiTrack(_straight_track_fitter->GetSmoothed(),
                                                             _geometry_helper);
    evt.add_scifitrack(track);
  }
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
      SciFiCluster* cluster = trackpoints[j]->get_cluster_pointer();

      std::cerr << "Track Point = " << trackpoints[j]->pos()[0] << ", " << trackpoints[j]->pos()[1] << ", " << trackpoints[j]->pos()[2] << " | "
                                    << trackpoints[j]->mom()[0] << ", " << trackpoints[j]->mom()[1] << ", " << trackpoints[j]->mom()[2] << " | "
                                    << "DATA = " << ( cluster ? cluster->get_alpha() : 0 ) << '\n';
    }
    std::cerr << "Tracker : " << tracks[i]->tracker() << ", Chi2 = " << tracks[i]->chi2() << ", P_Value = " << tracks[i]->P_value() << '\n';

    std::vector<double> seed_sta = tracks[i]->GetSeedState();
    std::vector<double> seed_cov = tracks[i]->GetSeedCovariance();

    std::cerr << "Seed State = ";
    for ( int j = 0; j < seed_sta.size(); ++j ) {
      std::cerr << seed_sta[j] << ", ";
    }
    std::cerr << '\n';

    std::cerr << "Seed Covariance = ";
    for ( int j = 0; j < seed_cov.size(); ++j ) {
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

} // ~namespace MAUS
