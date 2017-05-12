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

#include "src/map/MapCppTrackerTrackFit/MapCppTrackerTrackFit.hh"

#include <sstream>

#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/common_cpp/Recon/Kalman/MAUSTrackWrapper.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppTrackerTrackFit(void) {
  PyWrapMapBase<MapCppTrackerTrackFit>::PyWrapMapBaseModInit
                                        ("MapCppTrackerTrackFit", "", "", "", "");
}


MapCppTrackerTrackFit::MapCppTrackerTrackFit() : MapBase<Data>("MapCppTrackerTrackFit"),
                                           _kalman_on(true),
                                           _helical_track_fitter(NULL),
                                           _straight_track_fitter(NULL) {
}


MapCppTrackerTrackFit::~MapCppTrackerTrackFit() {
}


void MapCppTrackerTrackFit::_birth(const std::string& argJsonConfigDocument) {
  // Pull out the global settings
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  Json::Value* json = Globals::GetConfigurationCards();
  _kalman_on          = (*json)["SciFiKalmanOn"].asBool();
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

  // Build the geometery helper instance
  MiceModule* module = Globals::GetReconstructionMiceModules();
  std::vector<const MiceModule*> modules =
    module->findModulesByPropertyString("SensitiveDetector", "SciFi");
  _geometry_helper = SciFiGeometryHelper(modules);
  _geometry_helper.Build();
  SciFiTrackerMap& geo_map = _geometry_helper.GeometryMap();

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


void MapCppTrackerTrackFit::_death() {
  if (_helical_track_fitter) {
    delete _helical_track_fitter;
    _helical_track_fitter = NULL;
  }
  if (_straight_track_fitter) {
    delete _straight_track_fitter;
    _straight_track_fitter = NULL;
  }
}


void MapCppTrackerTrackFit::_process(Data* data) const {
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

      // Kalman Track Fit.
      if (_kalman_on) {
        event->clear_scifitracks();
        SciFiSeedPArray seeds = event->scifiseeds();
        for (SciFiSeedPArray::iterator it = seeds.begin(); it != seeds.end(); ++it) {
          try {
            switch ((*it)->getAlgorithm()) {
              case 0 :
                event->add_scifitrack(track_fit_straight((*it)));
                break;
              case 1 :
                event->add_scifitrack(track_fit_helix((*it)));
                break;
              default:
                break;
            }
          } catch (Exceptions::Exception& e) {
            std::cerr << "Track Fit Failed: " << e.what();
          }
        }
      }
    }
  } else {
    std::cout << "No recon events found\n";
  }
}

void MapCppTrackerTrackFit::_set_field_values(SciFiEvent* event) const {
  event->set_mean_field_up(_geometry_helper.GetFieldValue(0));
  event->set_mean_field_down(_geometry_helper.GetFieldValue(1));

  event->set_variance_field_up(_geometry_helper.GetFieldVariance(0));
  event->set_variance_field_down(_geometry_helper.GetFieldVariance(1));

  event->set_range_field_up(_geometry_helper.GetFieldRange(0));
  event->set_range_field_down(_geometry_helper.GetFieldRange(1));
}


SciFiTrack* MapCppTrackerTrackFit::track_fit_helix(SciFiSeed* seed) const {

  SciFiHelicalPRTrack* helical = static_cast<SciFiHelicalPRTrack*>(seed->getPRTrackTobject());

  Kalman::Track data_track = BuildTrack(helical, &_geometry_helper, 5);
  Kalman::State kalman_seed(seed->getStateVector(), seed->getCovariance());

  _helical_track_fitter->SetTrack(data_track);
  _helical_track_fitter->SetSeed(kalman_seed);

  _helical_track_fitter->Filter(false);
  _helical_track_fitter->Smooth(false);

  SciFiTrack* track = ConvertToSciFiTrack(_helical_track_fitter, &_geometry_helper, helical);
  calculate_track_rating(track);
  track->set_scifi_seed_tobject(seed);

  ThreeVector seed_pos = track->GetSeedPosition();
  ThreeVector seed_mom = track->GetSeedMomentum();

  return track;
}


SciFiTrack* MapCppTrackerTrackFit::track_fit_straight(SciFiSeed* seed) const {

  SciFiStraightPRTrack* straight = static_cast<SciFiStraightPRTrack*>(seed->getPRTrackTobject());

  Kalman::Track data_track = BuildTrack(straight, &_geometry_helper, 4);
  Kalman::State kalman_seed(seed->getStateVector(), seed->getCovariance());

  _straight_track_fitter->SetTrack(data_track);
  _straight_track_fitter->SetSeed(kalman_seed);

  _straight_track_fitter->Filter(false);
  _straight_track_fitter->Smooth(false);

  SciFiTrack* track = ConvertToSciFiTrack(_straight_track_fitter, &_geometry_helper, straight);
  calculate_track_rating(track);
  track->set_scifi_seed_tobject(seed);

  return track;
}



//
//  size_t number_helical_tracks = evt.helicalprtracks().size();
//  size_t number_straight_tracks = evt.straightprtracks().size();
//  for (size_t track_i = 0; track_i < number_helical_tracks; track_i++) {
//    SciFiHelicalPRTrack* helical = evt.helicalprtracks().at(track_i);
//
//    Kalman::Track data_track = BuildTrack(helical, &_geometry_helper, 5);
//    Kalman::State seed = ComputeSeed(helical, &_geometry_helper, _use_eloss, _seed_value);
//
//    _helical_track_fitter->SetTrack(data_track);
//    _helical_track_fitter->SetSeed(seed);
//
//    _helical_track_fitter->Filter(false);
//    _helical_track_fitter->Smooth(false);
//
//    SciFiTrack* track = ConvertToSciFiTrack(_helical_track_fitter, &_geometry_helper, helical);
//    calculate_track_rating(track);
//
//    evt.add_scifitrack(track);
//  }
//  for (size_t track_i = 0; track_i < number_straight_tracks; track_i++) {
//    SciFiStraightPRTrack* straight = evt.straightprtracks().at(track_i);
//    Kalman::Track data_track = BuildTrack(straight, &_geometry_helper, 4);
//
//    Kalman::State seed = ComputeSeed(straight, &_geometry_helper, _seed_value);
//
//    _straight_track_fitter->SetTrack(data_track);
//    _straight_track_fitter->SetSeed(seed);
//
//    _straight_track_fitter->Filter(false);
//    _straight_track_fitter->Smooth(false);
//
//    SciFiTrack* track = ConvertToSciFiTrack(_straight_track_fitter, &_geometry_helper, straight);
//    calculate_track_rating(track);
//
//    evt.add_scifitrack(track);
//  }
//



void MapCppTrackerTrackFit::calculate_track_rating(SciFiTrack* track) const {
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

} // ~namespace MAUS
