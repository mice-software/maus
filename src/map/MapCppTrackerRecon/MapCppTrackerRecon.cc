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

#include "src/map/MapCppTrackerRecon/MapCppTrackerRecon.hh"

#include <sstream>

#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/common_cpp/Recon/Kalman/MAUSTrackWrapper.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppTrackerRecon(void) {
  PyWrapMapBase<MapCppTrackerRecon>::PyWrapMapBaseModInit
                                        ("MapCppTrackerRecon", "", "", "", "");
}


MapCppTrackerRecon::MapCppTrackerRecon() : MapBase<Data>("MapCppTrackerRecon"),
                                           _clusters_on(true),
                                           _spacepoints_on(true),
                                           _up_straight_pr_on(true),
                                           _down_straight_pr_on(true),
                                           _up_helical_pr_on(true),
                                           _down_helical_pr_on(true),
                                           _kalman_on(true),
                                           _patrec_on(true),
                                           _patrec_debug_on(false),
                                           _helical_track_fitter(NULL),
                                           _straight_track_fitter(NULL) {
}


MapCppTrackerRecon::~MapCppTrackerRecon() {
}


void MapCppTrackerRecon::_birth(const std::string& argJsonConfigDocument) {
  // Pull out the global settings
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  Json::Value* json = Globals::GetConfigurationCards();
  int user_up_helical_pr_on    = (*json)["SciFiPRHelicalTkUSOn"].asInt();
  int user_down_helical_pr_on  = (*json)["SciFiPRHelicalTkDSOn"].asInt();
  int user_up_straight_pr_on   = (*json)["SciFiPRStraightTkUSOn"].asInt();
  int user_down_straight_pr_on = (*json)["SciFiPRStraightTkDSOn"].asInt();
  _kalman_on          = (*json)["SciFiKalmanOn"].asBool();
  _clusters_on        = (*json)["SciFiClusterReconOn"].asBool();
  _spacepoints_on     = (*json)["SciFiSpacepointReconOn"].asBool();
  _patrec_on          = (*json)["SciFiPatRecOn"].asBool();
  _patrec_debug_on    = (*json)["SciFiPatRecDebugOn"].asBool();
  _size_exception     = (*json)["SciFiClustExcept"].asInt();
  _min_npe            = (*json)["SciFiNPECut"].asDouble();
  _use_mcs            = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_eloss          = (*json)["SciFiKalman_use_Eloss"].asBool();
  _use_patrec_seed    = (*json)["SciFiSeedPatRec"].asBool();
  _correct_pz         = (*json)["SciFiKalmanCorrectPz"].asBool();
  // Values used to set the track rating:
  _excellent_num_trackpoints     = (*json)["SciFiExcellentNumTrackpoints"].asInt();
  _good_num_trackpoints          = (*json)["SciFiGoodNumTrackpoints"].asInt();
  _poor_num_trackpoints          = (*json)["SciFiPoorNumTrackpoints"].asInt();
  _excellent_p_value             = (*json)["SciFiExcellentPValue"].asDouble();
  _good_p_value                  = (*json)["SciFiGoodPValue"].asDouble();
  _poor_p_value                  = (*json)["SciFiPoorPValue"].asDouble();
  _excellent_num_spacepoints  = (*json)["SciFiExcellentNumSpacepoints"].asInt();
  _good_num_spacepoints       = (*json)["SciFiGoodNumSpacepoints"].asInt();
  _poor_num_spacepoints       = (*json)["SciFiPoorNumSpacepoints"].asInt();

  ofstream logfile;
  std::string root_dir = std::getenv("MAUS_ROOT_DIR");
  std::string path = root_dir + "/tmp/digit_exception.log";
  logfile.open(path);
  logfile.close();

  // Build the geometery helper instance
  MiceModule* module = Globals::GetReconstructionMiceModules();
  std::vector<const MiceModule*> modules =
    module->findModulesByPropertyString("SensitiveDetector", "SciFi");
  _geometry_helper = SciFiGeometryHelper(modules);
  _geometry_helper.Build();
  SciFiTrackerMap& geo_map = _geometry_helper.GeometryMap();

  // Set up cluster reconstruction
  _cluster_recon = SciFiClusterRec(_size_exception, _min_npe, geo_map);

  // Set up spacepoint reconstruction
  _spacepoint_recon = SciFiSpacePointRec();

  // Setup Pattern Recognition
  double up_field = _geometry_helper.GetFieldValue(0);
  double down_field = _geometry_helper.GetFieldValue(1);
  if (user_up_helical_pr_on == 2)
    _up_helical_pr_on = true;
  else if (user_up_helical_pr_on == 1)
    _up_helical_pr_on = false;
  else if (user_up_helical_pr_on == 0 && fabs(up_field) < 0.00001) // 10 Gaus
    _up_helical_pr_on = false;
  else if (user_up_helical_pr_on == 0 && fabs(up_field) >= 0.00001)
    _up_helical_pr_on = true;

  if (user_down_helical_pr_on == 2)
    _down_helical_pr_on = true;
  else if (user_down_helical_pr_on == 1)
    _down_helical_pr_on = false;
  else if (user_down_helical_pr_on == 0 && fabs(down_field) < 0.00001)
    _down_helical_pr_on = false;
  else if (user_down_helical_pr_on == 0 && fabs(down_field) >= 0.00001)
    _down_helical_pr_on = true;

  if (user_up_straight_pr_on == 2)
    _up_straight_pr_on = true;
  else if (user_up_straight_pr_on == 1)
    _up_straight_pr_on = false;
  else if (user_up_straight_pr_on == 0 && fabs(up_field) < 0.00001)
    _up_straight_pr_on = true;
  else if (user_up_straight_pr_on == 0 && fabs(up_field) >= 0.00001)
    _up_straight_pr_on = false;

  if (user_down_straight_pr_on == 2)
    _down_straight_pr_on = true;
  else if (user_down_straight_pr_on == 1)
    _down_straight_pr_on = false;
  else if (user_down_straight_pr_on == 0 && fabs(down_field) < 0.00001)
    _down_straight_pr_on = true;
  else if (user_down_straight_pr_on == 0 && fabs(down_field) >= 0.00001)
    _down_straight_pr_on = false;

  _pattern_recognition = PatternRecognition();
  _pattern_recognition.LoadGlobals();
  _pattern_recognition.set_up_helical_pr_on(_up_helical_pr_on);
  _pattern_recognition.set_down_helical_pr_on(_down_helical_pr_on);
  _pattern_recognition.set_up_straight_pr_on(_up_straight_pr_on);
  _pattern_recognition.set_down_straight_pr_on(_down_straight_pr_on);
  _pattern_recognition.set_bz_t1(up_field);
  _pattern_recognition.set_bz_t2(down_field);
  if (_patrec_debug_on) _pattern_recognition.setup_debug();

  if (_use_patrec_seed) {
    _seed_value = -1.0;
  } else {
    _seed_value = (*json)["SciFiSeedCovariance"].asDouble();
  }

  // Set up final track fit (Kalman filter)
  HelicalPropagator* helical_prop = new HelicalPropagator(&_geometry_helper);
  helical_prop->SetCorrectPz(_correct_pz);
  helical_prop->SetIncludeMCS(_use_mcs);
  helical_prop->SetSubtractELoss(_use_eloss);
  _helical_track_fitter = new Kalman::TrackFit(helical_prop);

  StraightPropagator* straight_prop = new StraightPropagator(&_geometry_helper);
  straight_prop->SetIncludeMCS(_use_mcs);
  _straight_track_fitter = new Kalman::TrackFit(straight_prop);

  // Each measurement plane has a unique alignment and rotation => they all need their own
  //  measurement object.
  for (SciFiTrackerMap::iterator track_it = geo_map.begin();
                                                           track_it != geo_map.end(); ++track_it) {
    int tracker_const = (track_it->first == 0 ? -1 : 1);
    for (SciFiPlaneMap::iterator plane_it = track_it->second.Planes.begin();
                                           plane_it != track_it->second.Planes.end(); ++plane_it) {

      int id = plane_it->first * tracker_const;
      _helical_track_fitter->AddMeasurement(id,
                                             new MAUS::SciFiHelicalMeasurements(plane_it->second));
      _straight_track_fitter->AddMeasurement(id,
                                            new MAUS::SciFiStraightMeasurements(plane_it->second));
    }
  }
}


void MapCppTrackerRecon::_death() {
  if (_helical_track_fitter) {
    delete _helical_track_fitter;
    _helical_track_fitter = NULL;
  }
  if (_straight_track_fitter) {
    delete _straight_track_fitter;
    _straight_track_fitter = NULL;
  }
}


void MapCppTrackerRecon::_process(Data* data) const {
  std::cerr << "MapCppTrackerRecon::_process 1" << std::endl;
  Spill& spill = *(data->GetSpill());

  /* return if not physics spill */
  if (spill.GetDaqEventType() != "physics_event")
    return;
  std::cerr << "MapCppTrackerRecon::_process 2" << std::endl;

  if (spill.GetReconEvents()) {
      std::cerr << "MapCppTrackerRecon::_process 3" << std::endl;

    for (unsigned int k = 0; k < spill.GetReconEvents()->size(); k++) {
      std::cerr << "MapCppTrackerRecon::_process 4 ... "   << k << std::endl;

      SciFiEvent *event = spill.GetReconEvents()->at(k)->GetSciFiEvent();
      if (!event) {
        continue;
      }
      std::cerr << "MapCppTrackerRecon::_process 5 ... "   << k << std::endl;

      _set_field_values(event);
      std::cerr << "MapCppTrackerRecon::_process 6 ... "   << k << std::endl;

      if ( _clusters_on ) {
      // Clear any exising higher level data
        std::cerr << "MapCppTrackerRecon::_process 7 ... "   << k << std::endl;
        event->clear_clusters();
        // Build Clusters.
        std::cerr << "MapCppTrackerRecon::_process 8 ... "   << k << " ... " << event->digits().size() << std::endl;
        if (event->digits().size()) {
          std::cerr << "MapCppTrackerRecon::_process 9 ... "   << k << std::endl;
          _cluster_recon.process(*event);
          std::cerr << "   ... clusters size " << event->clusters().size() << std::endl;
        }
      }
      if ( _spacepoints_on ) {
        // Build SpacePoints.
        event->clear_spacepoints();
        if (event->clusters().size()) {
          _spacepoint_recon.process(*event);
          set_spacepoint_global_output(event->spacepoints());
        }
      }
      // Pattern Recognition.
      if (_patrec_on && event->spacepoints().size()) {
        event->clear_seeds();
        event->clear_stracks();
        event->clear_htracks();
        _pattern_recognition.process(*event);
        set_straight_prtrack_global_output(event->straightprtracks());
        extrapolate_helical_reference(*event);
        extrapolate_straight_reference(*event);
      }
      // Kalman Track Fit.
      if (_kalman_on) {
        event->clear_scifitracks();
        track_fit(*event);
      }
//      print_event_info(*event);
    }
  } else {
    std::cout << "No recon events found\n";
  }
}

void MapCppTrackerRecon::_set_field_values(SciFiEvent* event) const {
  event->set_mean_field_up(_geometry_helper.GetFieldValue(0));
  event->set_mean_field_down(_geometry_helper.GetFieldValue(1));

  event->set_variance_field_up(_geometry_helper.GetFieldVariance(0));
  event->set_variance_field_down(_geometry_helper.GetFieldVariance(1));

  event->set_range_field_up(_geometry_helper.GetFieldRange(0));
  event->set_range_field_down(_geometry_helper.GetFieldRange(1));
}


void MapCppTrackerRecon::track_fit(SciFiEvent &evt) const {
  size_t number_helical_tracks = evt.helicalprtracks().size();
  size_t number_straight_tracks = evt.straightprtracks().size();
  for (size_t track_i = 0; track_i < number_helical_tracks; track_i++) {
    SciFiHelicalPRTrack* helical = evt.helicalprtracks().at(track_i);

    Kalman::Track data_track = BuildTrack(helical, &_geometry_helper, 5);
    Kalman::State seed = ComputeSeed(helical, &_geometry_helper, _use_eloss, _seed_value);

    _helical_track_fitter->SetTrack(data_track);
    _helical_track_fitter->SetSeed(seed);

    _helical_track_fitter->Filter(false);
    _helical_track_fitter->Smooth(false);

    SciFiTrack* track = ConvertToSciFiTrack(_helical_track_fitter, &_geometry_helper, helical);
    calculate_track_rating(track);

    evt.add_scifitrack(track);
  }
  for (size_t track_i = 0; track_i < number_straight_tracks; track_i++) {
    SciFiStraightPRTrack* straight = evt.straightprtracks().at(track_i);
    Kalman::Track data_track = BuildTrack(straight, &_geometry_helper, 4);

    Kalman::State seed = ComputeSeed(straight, &_geometry_helper, _seed_value);

    _straight_track_fitter->SetTrack(data_track);
    _straight_track_fitter->SetSeed(seed);

    _straight_track_fitter->Filter(false);
    _straight_track_fitter->Smooth(false);

    SciFiTrack* track = ConvertToSciFiTrack(_straight_track_fitter, &_geometry_helper, straight);
    calculate_track_rating(track);

    evt.add_scifitrack(track);
  }
}


void MapCppTrackerRecon::extrapolate_helical_reference(SciFiEvent& event) const {
  SciFiHelicalPRTrackPArray helicals = event.helicalprtracks();
  size_t num_tracks = helicals.size();

  for (size_t i = 0; i < num_tracks; ++i) {
    SciFiHelicalPRTrack* track = helicals[i];
    ThreeVector pos;
    ThreeVector mom;

    int tracker = track->get_tracker();
    ThreeVector reference = _geometry_helper.GetReferencePosition(tracker);
    CLHEP::HepRotation rotation = _geometry_helper.GetReferenceRotation(tracker);

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

  for (size_t i = 0; i < num_tracks; ++i) {
    SciFiStraightPRTrack* track = straights[i];
    ThreeVector pos;
    ThreeVector mom;
    double default_mom = _geometry_helper.GetDefaultMomentum();

    int tracker = track->get_tracker();
    ThreeVector reference = _geometry_helper.GetReferencePosition(tracker);
    CLHEP::HepRotation rotation = _geometry_helper.GetReferenceRotation(tracker);

    pos.setX(track->get_x0());
    pos.setY(track->get_y0());
    pos.setZ(0.0);
    pos *= rotation;
    pos += reference;

    double mx = track->get_mx();
    double my = track->get_my();
    mom.setX(mx*default_mom);
    mom.setY(my*default_mom);
    mom.setZ(default_mom);
    mom *= rotation;
    if (tracker == 0) mom *= -1.0;
    mom.setZ(fabs(mom.z()));

    track->set_reference_position(pos);
    track->set_reference_momentum(mom);
  }
}


void MapCppTrackerRecon::set_spacepoint_global_output(const SciFiSpacePointPArray& spoints) const {
  for (auto sp : spoints) {
    sp->set_global_position(
      _geometry_helper.TransformPositionToGlobal(sp->get_position(), sp->get_tracker()));
  }
}


void MapCppTrackerRecon::set_straight_prtrack_global_output(
  const SciFiStraightPRTrackPArray& trks) const {
  for (auto trk : trks) {
    ThreeVector pos = trk->get_spacepoints_pointers()[0]->get_position();
    std::vector<double> params{ trk->get_x0(), trk->get_mx(), trk->get_y0(), trk->get_my() }; // NOLINT
    std::vector<double> global_params =
      _geometry_helper.TransformStraightParamsToGlobal(params, trk->get_tracker());

    // Update the track
    trk->set_global_x0(global_params[0]);
    trk->set_global_mx(global_params[1]);
    trk->set_global_y0(global_params[2]);
    trk->set_global_my(global_params[3]);
  }
}


void MapCppTrackerRecon::calculate_track_rating(SciFiTrack* track) const {
  SciFiBasePRTrack* pr_track = track->pr_track_pointer();
  int number_spacepoints = pr_track->get_num_points();
  int number_trackpoints = track->GetNumberDataPoints();

  bool excel_numtp = (number_trackpoints >= _excellent_num_trackpoints);
  bool good_numtp = (number_trackpoints >= _good_num_trackpoints);
  bool poor_numtp = (number_trackpoints >= _poor_num_trackpoints);
  bool excel_numsp = (number_spacepoints >= _excellent_num_spacepoints);
  bool good_numsp = (number_spacepoints >= _good_num_spacepoints);
  bool poor_numsp = (number_spacepoints >= _poor_num_spacepoints);
  bool excel_pval = (track->P_value() >= _excellent_p_value);
  bool good_pval = (track->P_value() >= _good_p_value);
  bool poor_pval = (track->P_value() >= _poor_p_value);

  int rating = 0;

  if (excel_numtp && excel_numsp && excel_pval) {
    rating = 1;
  } else if (good_numtp && good_numsp && good_pval) {
    rating = 2;
  } else if (poor_numtp && poor_numsp && poor_pval) {
    rating = 3;
  } else {
    rating = 5;
  }

  track->SetRating(rating);
}


void MapCppTrackerRecon::print_event_info(SciFiEvent &event) const {
  std::cerr << "\n ######  SciFi Event Details  ######\n\n";


  std::cerr << "Num Digits      : " << event.digits().size() << "\n"
            << "Num Clusters    : " << event.clusters().size() << "\n"
            << "Num Spacepoints : " << event.spacepoints().size() << "\n"
            << "Num Straights   : " << event.straightprtracks().size() << "\n"
            << "Num Helicals    : " << event.helicalprtracks().size() << "\n\n";

  SciFiTrackPArray tracks = event.scifitracks();
  for (unsigned int i = 0; i < tracks.size(); ++i) {
    SciFiTrackPointPArray trackpoints = tracks[i]->scifitrackpoints();
    for (unsigned int j = 0; j < trackpoints.size(); ++j) {
      std::cerr << "Track Point = " << trackpoints[j]->pos()[0] << ", "
                                    << trackpoints[j]->pos()[1] << ", "
                                    << trackpoints[j]->pos()[2] << " | "
                                    << trackpoints[j]->mom()[0] << ", "
                                    << trackpoints[j]->mom()[1] << ", "
                                    << trackpoints[j]->mom()[2] << " | "
//                                  << "DATA = " << (cluster ? cluster->get_alpha() : 0) << '\n
                                    << "(" << trackpoints[j]->pull() << ", "
                                    << trackpoints[j]->residual()
                                    << ", " << trackpoints[j]->smoothed_residual() << ")\n";
    }
    std::cerr << "Tracker : " << tracks[i]->tracker()
              << ", Chi2 = " << tracks[i]->chi2()
              << ", NDF = " << tracks[i]->ndf()
              << ", P_Value = " << tracks[i]->P_value()
              << ", Charge = " << tracks[i]->charge() << '\n';

    ThreeVector seed_pos = tracks[i]->GetSeedPosition();
    ThreeVector seed_mom = tracks[i]->GetSeedMomentum();
    std::vector<double> seed_cov = tracks[i]->GetSeedCovariance();

    std::cerr << "Seed State = (" << seed_pos[0] << ", " << seed_pos[1] << ", " << seed_pos[2]
              << ")  (" << seed_mom[0] << ", " << seed_mom[1] << ", " << seed_mom[2] << ")\n";

    std::cerr << "Seed Covariance = ";
    for (unsigned int j = 0; j < seed_cov.size(); ++j) {
      std::cerr << seed_cov[j] << ", ";
    }
    std::cerr << '\n';
  }
  std::cerr << std::endl;
}

} // ~namespace MAUS
