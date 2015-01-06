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
  Json::Value *json = Globals::GetConfigurationCards();
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
  pr1.process(evt);
}

void MapCppTrackerRecon::track_fit(SciFiEvent &evt) const {
  std::vector<KalmanSeed*> seeds;
  size_t number_helical_tracks  = evt.helicalprtracks().size();
  size_t number_straight_tracks = evt.straightprtracks().size();

  for ( size_t track_i = 0; track_i < number_helical_tracks; track_i++ ) {
    int tracker = evt.helicalprtracks()[track_i]->get_tracker();
    double Bz = _geometry_helper.GetFieldValue(tracker);
    KalmanSeed *seed = new KalmanSeed(_geometry_helper.GeometryMap());
    seed->SetField(Bz);
    seed->Build<SciFiHelicalPRTrack>(evt.helicalprtracks()[track_i]);
    seeds.push_back(seed);
  }

  for ( size_t track_i = 0; track_i < number_straight_tracks; track_i++ ) {
    KalmanSeed *seed = new KalmanSeed(_geometry_helper.GeometryMap());
    seed->Build<SciFiStraightPRTrack>(evt.straightprtracks()[track_i]);
    seeds.push_back(seed);
  }

  if ( seeds.size() ) {
    KalmanTrackFit fit;
    fit.SaveGeometry(_geometry_helper.RefPos(), _geometry_helper.Rot());
    fit.Process(seeds, evt);
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


  SciFiTrackPArray tracks = event.scifitracks();
  for ( unsigned int i = 0; i < tracks.size(); ++i )
  {
    SciFiTrackPointPArray trackpoints = tracks[i]->scifitrackpoints();
    for ( unsigned int j = 0; j < trackpoints.size(); ++j )
    {
      std::cerr << "Track Point = " << trackpoints[j]->pos()[0] << ", " << trackpoints[j]->pos()[1] << ", " << trackpoints[j]->pos()[2] << '\n';
    }
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
