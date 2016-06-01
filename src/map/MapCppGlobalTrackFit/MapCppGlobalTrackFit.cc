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

#include <iostream>

#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/Recon/Kalman/Global/Propagator.hh"
#include "src/common_cpp/Recon/Kalman/Global/Measurement.hh"

#include "src/common_cpp/Recon/Global/DataStructureHelper.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/map/MapCppGlobalTrackFit/DataLoader.hh"
#include "src/map/MapCppGlobalTrackFit/SeedBase.hh"
#include "src/map/MapCppGlobalTrackFit/MapCppGlobalTrackFit.hh"

// TODO: 
// * -Error propagation through fields-
// * -Integrate into Kalman fitter-
// * Add materials - -MCS-; -dE/dx-; energy straggling
// * TrackPoint errors should be a matrix
// * -Fix muon assumption-
// * -Fix positive charge assumption-
// * Clean up event/space point selection (load from Global event importer
//   stuff); add in extra detector/hit types (e.g. tracker tracks)
// * Properly write the output Global event stuff (check I understand the datastructure)
// * -Do u/s fit, d/s fit, global fit-
// * -Deal with triplet cut on scifi-
// * Soft code detector errors or import errors correctly
// * Clear labelled bugs
// * -Add virtual planes/disabled detectors (make a state with 0 length?)-
// * Interface to event display
// * Residuals plots (etc) reducer
// * -Move Global kalman stuff into subdirectory/namespace-
// * Documentation
// * Improve test coverage
// * -Dynamic step size to step to volume boundary (using GSL Control type)-
// * -Enable/disable material name/conversion logic-
// * -Seed from TOF01-; seed from tracker; -seed from TOF+tracker-
// * Enable/disable volume names

std::ostream& operator <<(std::ostream& out, MAUS::Kalman::State const& state) {
    std::string state_str = MAUS::Kalman::print_state(state);
    out << state_str;
    return out;
}

namespace MAUS {
using namespace MAUS::DataStructure::Global;

double MapCppGlobalTrackFit::c_light = 300.; // BUG

PyMODINIT_FUNC init_MapCppGlobalTrackFit(void) {
  PyWrapMapBase<MapCppGlobalTrackFit>::PyWrapMapBaseModInit
                                        ("MapCppGlobalTrackFit", "", "", "", "");
}

MapCppGlobalTrackFit::MapCppGlobalTrackFit() : 
    MapBase<Data>("MapCppGlobalTrackFit") {
    using namespace DataStructure::Global;
    Kalman::Global::Measurement::SetupDetectorToMeasurementMap();
}


MapCppGlobalTrackFit::~MapCppGlobalTrackFit() {
}

void MapCppGlobalTrackFit::_birth(const std::string& config_str) {
  // Pull out the global settings
  Json::Value config = JsonWrapper::StringToJson(config_str);

  Json::Value track_fits = JsonWrapper::GetProperty(
                              config,
                              "global_track_fits",
                              JsonWrapper::arrayValue);
  for (size_t i = 0; i < track_fits.size(); ++i) {
      _fit_info.push_back(FitInfo(track_fits[int(i)], _seed_manager));
  }
}


void MapCppGlobalTrackFit::_death() {
  _fit_info = std::vector<FitInfo>();
}


void MapCppGlobalTrackFit::_process(Data* data) const {
  Spill& spill = *(data->GetSpill());

  // return if not physics spill
  if (spill.GetDaqEventType() != "physics_event")
    return;

  for (unsigned int i = 0; i < spill.GetReconEvents()->size(); i++) {
      ReconEvent *event = spill.GetReconEvents()->at(i);
      if (!event) {
        continue;
      }
      track_fit(*event);
  }
}

void MapCppGlobalTrackFit::append_to_data(GlobalEvent& event, Kalman::Track fit, double mass) const {
    using namespace DataStructure;
    Global::Track* track = new Global::Track();
    track->set_pid(Global::kMuPlus);
    track->set_charge(1.);
    track->set_mapper_name("MapCppGlobalTrackFit");
    for (size_t i = 0; i < fit.GetLength(); ++i) {
        Kalman::State state = fit[i].GetSmoothed();
        Global::TrackPoint* point = new Global::TrackPoint();
        point->set_mapper_name("MapCppGlobalTrackFit");
        point->set_particle_event(-99); // BUG
        TLorentzVector pos(state.GetVector()[1][0],  // position
                           state.GetVector()[2][0],
                           fit[i].GetPosition(),
                           state.GetVector()[0][0]);  // time
        TLorentzVector mom(state.GetVector()[4][0], // 3-momentum
                           state.GetVector()[5][0],
                           0.,
                           state.GetVector()[3][0]); // energy
        double E = state.GetVector()[3][0];
        // pz^2 = E^2 - px^2 - py^2 - mass ^2
        mom[2] = ::sqrt(E*E-(mom[0]*mom[0]+mom[1]*mom[1]+mass*mass));
        point->set_position(pos);
        point->set_momentum(mom);
        track->AddTrackPoint(point);
    }
    track->SortTrackPointsByZ();
    event.add_track_recursive(track);
}

void MapCppGlobalTrackFit::track_fit(ReconEvent &event) const {
    if (event.GetGlobalEvent() == NULL)
        event.SetGlobalEvent(new GlobalEvent());
    for (size_t i = 0; i <_fit_info.size(); ++i) {
        FitInfo info = _fit_info[i];
        Kalman::Global::Propagator* propagator = new Kalman::Global::Propagator();
        propagator->SetField(Globals::GetMCFieldConstructor());
        propagator->SetMass(info.mass_hypothesis);
        //propagator->SetCharge(fit_info.charge_hypothesis); // BUG
        propagator->GetTracking()->SetMinStepSize(info.min_step);
        propagator->GetTracking()->SetMaxStepSize(info.max_step);
        propagator->GetTracking()->SetCharge(info.charge_hypothesis);

        Kalman::TrackFit kalman_fit(propagator);
        DataLoader data(info.active_detectors, &kalman_fit);
        data.SetWillRequireTrackerTriplets(info.will_require_triplet_space_points);
        // will_require_triplet_space_points // BUG
        data.load_event(event);
        
        Kalman::State seed_state = info.seed_algorithm->GetSeed(&event);
        kalman_fit.SetSeed(seed_state);

        kalman_fit.Filter(true);
        if (info.will_smooth) {
            kalman_fit.Smooth(true);
        }
        Kalman::Track track = kalman_fit.GetTrack();
        append_to_data(*(event.GetGlobalEvent()), track, info.mass_hypothesis);
    }
}

MapCppGlobalTrackFit::FitInfo::FitInfo(Json::Value fit_info, SeedManager& seeds) {
  will_require_triplet_space_points = JsonWrapper::GetProperty(
        fit_info,
        "will_require_triplet_space_points",
        JsonWrapper::booleanValue).asBool();

  mass_hypothesis = JsonWrapper::GetProperty(
                        fit_info,
                        "mass_hypothesis",
                        JsonWrapper::realValue).asDouble();

  charge_hypothesis = JsonWrapper::GetProperty(
                        fit_info,
                        "charge_hypothesis",
                        JsonWrapper::realValue).asDouble();

  min_step = JsonWrapper::GetProperty(
                        fit_info,
                        "min_step_size",
                        JsonWrapper::realValue).asDouble();

  max_step = JsonWrapper::GetProperty(
                        fit_info,
                        "max_step_size",
                        JsonWrapper::realValue).asDouble();

  will_smooth = JsonWrapper::GetProperty(
                        fit_info,
                        "will_smooth",
                        JsonWrapper::booleanValue).asBool();

  
  std::string seed = JsonWrapper::GetProperty(
                          fit_info,
                          "seed",
                          JsonWrapper::stringValue).asString();
  seed_algorithm = seeds.GetAlgorithm(seed);

  Json::Value detectors = JsonWrapper::GetProperty(
                          fit_info,
                          "detectors",
                          JsonWrapper::arrayValue);
  const recon::global::DataStructureHelper& help = recon::global::DataStructureHelper::GetInstance();
  for (size_t i = 0; i < detectors.size(); ++i) {
      std::string detector_name = JsonWrapper::GetItem(
                                  detectors, i, JsonWrapper::stringValue
                    ).asString();
      active_detectors.push_back(help.StringToDetectorPoint(detector_name));
  }
}
} // ~namespace MAUS
