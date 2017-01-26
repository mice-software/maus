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

#include "src/map/MapCppTrackerPatternRecognition/MapCppTrackerPatternRecognition.hh"
#include <sstream>
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppTrackerPatternRecognition(void) {
  PyWrapMapBase<MapCppTrackerPatternRecognition>::PyWrapMapBaseModInit
                                        ("MapCppTrackerPatternRecognition", "", "", "", "");
}


MapCppTrackerPatternRecognition::MapCppTrackerPatternRecognition()
                                          : MapBase<Data>("MapCppTrackerPatternRecognition"),
                                            _upstream_correction(2),
                                            _downstream_correction(2),
                                            _up_straight_pr_on(true),
                                            _down_straight_pr_on(true),
                                            _up_helical_pr_on(true),
                                            _down_helical_pr_on(true),
                                            _patrec_on(true),
                                            _patrec_debug_on(false) {
  // Do nothing
}


MapCppTrackerPatternRecognition::~MapCppTrackerPatternRecognition() {
  // Do nothing
}


void MapCppTrackerPatternRecognition::_birth(const std::string& argJsonConfigDocument) {
  // Pull out the global settings
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  Json::Value* json = Globals::GetConfigurationCards();
  int user_up_helical_pr_on    = (*json)["SciFiPRHelicalTkUSOn"].asInt();
  int user_down_helical_pr_on  = (*json)["SciFiPRHelicalTkDSOn"].asInt();
  int user_up_straight_pr_on   = (*json)["SciFiPRStraightTkUSOn"].asInt();
  int user_down_straight_pr_on = (*json)["SciFiPRStraightTkDSOn"].asInt();
  _patrec_on          = (*json)["SciFiPatRecOn"].asBool();
  _patrec_debug_on    = (*json)["SciFiPatRecDebugOn"].asBool();

  _correct_seed_momentum = static_cast<bool>((*json)["SciFiPRCorrectSeed"].asInt());
  if (_correct_seed_momentum) {
    _load_momentum_corrections((*json)["SciFiPRCorrectionsFile"].asString());
  }

  // Build the geometery helper instance
  MiceModule* module = Globals::GetReconstructionMiceModules();
  std::vector<const MiceModule*> modules =
    module->findModulesByPropertyString("SensitiveDetector", "SciFi");
  _geometry_helper = SciFiGeometryHelper(modules);
  _geometry_helper.Build();

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
  if (_patrec_debug_on) {
    std::cerr << "INFO::MapCppTrackerPatternRecognition: Debug mode on\n";
    _pattern_recognition.setup_debug();
  }
}


void MapCppTrackerPatternRecognition::_death() {
  // Do nothing
}


void MapCppTrackerPatternRecognition::_process(Data* data) const {
  Spill& spill = *(data->GetSpill());

  /* return if not physics spill */
  if (spill.GetDaqEventType() != "physics_event")
    return;

  if (spill.GetReconEvents()) {
    for (unsigned int k = 0; k < spill.GetReconEvents()->size(); k++) {
      SciFiEvent *event = spill.GetReconEvents()->at(k)->GetSciFiEvent();
      if (!event) {
        continue;
      }

      _set_field_values(event);

      // Pattern Recognition.
      if (_patrec_on && event->spacepoints().size()) {
        event->clear_seeds();
        event->clear_stracks();
        event->clear_htracks();
        _pattern_recognition.process(*event);
        set_straight_prtrack_global_output(event->straightprtracks());
        extrapolate_helical_reference(*event);
        extrapolate_helical_seed(*event);
        extrapolate_straight_reference(*event);
        extrapolate_straight_seed(*event);
      }
    }
  } else {
    std::cout << "No recon events found\n";
  }
}

void MapCppTrackerPatternRecognition::_set_field_values(SciFiEvent* event) const {
  event->set_mean_field_up(_geometry_helper.GetFieldValue(0));
  event->set_mean_field_down(_geometry_helper.GetFieldValue(1));

  event->set_variance_field_up(_geometry_helper.GetFieldVariance(0));
  event->set_variance_field_down(_geometry_helper.GetFieldVariance(1));

  event->set_range_field_up(_geometry_helper.GetFieldRange(0));
  event->set_range_field_down(_geometry_helper.GetFieldRange(1));
}


void MapCppTrackerPatternRecognition::extrapolate_helical_reference(SciFiEvent& event) const {
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


void MapCppTrackerPatternRecognition::extrapolate_straight_reference(SciFiEvent& event) const {
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


void MapCppTrackerPatternRecognition::extrapolate_helical_seed(SciFiEvent& event) const {
  SciFiHelicalPRTrackPArray helicals = event.helicalprtracks();
  size_t num_tracks = helicals.size();

  for (size_t i = 0; i < num_tracks; ++i) {
    SciFiHelicalPRTrack* track = helicals[i];
    ThreeVector pos;
    ThreeVector mom;

    int tracker = track->get_tracker();
    double length = _geometry_helper.GetPlanePosition(tracker, 5, 2);

    double r  = track->get_R();
    double pt = - track->get_charge()*CLHEP::c_light*_geometry_helper.GetFieldValue(tracker)*r;
    double dsdz = - track->get_dsdz();
    double x0 = track->get_circle_x0();
    double y0 = track->get_circle_y0();
    double s = (track->get_line_sz_c() - length*dsdz);
    double phi = s / r;

    pos.setX(x0 + r*cos(phi));
    pos.setY(y0 + r*sin(phi));
    pos.setZ(length);

    mom.setX(-pt*sin(phi));
    mom.setY(pt*cos(phi));
    mom.setZ(-pt/dsdz);

    if (_correct_seed_momentum) {
      double old_p = sqrt(mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z());
      if (tracker == 0) {
        double new_p = _upstream_correction[0] + _upstream_correction[1]*old_p;
        mom *= new_p/old_p;
      } else {
        double new_p = _downstream_correction[0] + _downstream_correction[1]*old_p;
        mom *= new_p/old_p;
      }
    }

    track->set_seed_position(pos);
    track->set_seed_momentum(mom);
  }
}


void MapCppTrackerPatternRecognition::extrapolate_straight_seed(SciFiEvent& event) const {
  SciFiStraightPRTrackPArray straights = event.straightprtracks();
  size_t num_tracks = straights.size();

  for (size_t i = 0; i < num_tracks; ++i) {
    SciFiStraightPRTrack* track = straights[i];
    ThreeVector pos;
    ThreeVector mom;
    double default_mom = _geometry_helper.GetDefaultMomentum();

    int tracker = track->get_tracker();
    double length = _geometry_helper.GetPlanePosition(tracker, 5, 2);
    ThreeVector reference = _geometry_helper.GetReferencePosition(tracker);

    pos.setX(track->get_x0());
    pos.setY(track->get_y0());
    pos.setZ(length);

    double mx = track->get_mx();
    double my = track->get_my();
    mom.setX(mx*default_mom);
    mom.setY(my*default_mom);
    mom.setZ(default_mom);
    if (tracker == 0) mom *= -1.0;
    mom.setZ(fabs(mom.z()));

    track->set_seed_position(pos);
    track->set_seed_momentum(mom);
  }
}

void MapCppTrackerPatternRecognition::set_straight_prtrack_global_output(
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


void MapCppTrackerPatternRecognition::_load_momentum_corrections(std::string filename) {
  TFile corrections_file(filename.c_str(), "READ");

  TVectorD* upstream = reinterpret_cast<TVectorD*>(
                                           corrections_file.Get("upstream_correction_parameters"));
  TVectorD* downstream = reinterpret_cast<TVectorD*>(
                                         corrections_file.Get("downstream_correction_parameters"));

  _upstream_correction = (*upstream);
  _downstream_correction = (*downstream);

  corrections_file.Close();
}

} // ~namespace MAUS
