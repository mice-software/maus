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

#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/map/MapCppGlobalTrackFit/DataLoader.hh"
#include "src/map/MapCppGlobalTrackFit/MapCppGlobalTrackFit.hh"

// TODO: 
// * -Error propagation through fields-
// * Integrate into Kalman fitter
// * Add materials - MCS; dE/dx; energy straggling
// * TrackPoint errors should be a matrix
// * Fix muon assumption
// * Fix positive charge assumption
// * Clean up event/space point selection (load from Global event importer
//   stuff); add in extra detector/hit types (e.g. tracker tracks)
// * Properly write the output Global event stuff (check I understand the datastructure)
// * Do u/s fit, d/s fit, global fit
// * Deal with triplet cut on scifi
// * Soft code detector errors or import errors correctly
// * Clear labelled bugs
// * -Add virtual planes/disabled detectors (make a state with 0 length?)-
// * Interface to event display
// * Residuals plots (etc) reducer
// * -Move Global kalman stuff into subdirectory/namespace-
// * Documentation
// * Improve test coverage
// * Dynamic step size to step to volume boundary (using GSL Control type)
// * -Enable/disable material name/conversion logic-
// * Enable/disable volume names

std::ostream& operator <<(std::ostream& out, MAUS::Kalman::State const& state) {
    std::string state_str = MAUS::Kalman::print_state(state);
    out << state_str;
    return out;
}

namespace MAUS {
using namespace MAUS::DataStructure::Global;

double MapCppGlobalTrackFit::mu_mass = 105.658; // BUG
double MapCppGlobalTrackFit::c_light = 300.; // BUG

PyMODINIT_FUNC init_MapCppGlobalTrackFit(void) {
  PyWrapMapBase<MapCppGlobalTrackFit>::PyWrapMapBaseModInit
                                        ("MapCppGlobalTrackFit", "", "", "", "");
}


MapCppGlobalTrackFit::MapCppGlobalTrackFit() : MapBase<Data>("MapCppGlobalTrackFit") {
    using namespace DataStructure::Global;
    auto det_list = {
      kTOF0, kTOF1,
      kTracker0_1, kTracker0_2, kTracker0_3, kTracker0_4, kTracker0_5,
      kVirtual,
    };
    _active_detectors = std::vector<DetectorPoint>(det_list);
    Kalman::Global::Measurement::SetupDetectorToMeasurementMap();
}


MapCppGlobalTrackFit::~MapCppGlobalTrackFit() {
}


void MapCppGlobalTrackFit::_birth(const std::string& config_str) {
  // Pull out the global settings
  std::cerr << "birth 1" << std::endl;
  Json::Value config = JsonWrapper::StringToJson(config_str);
  _will_require_triplets = true;
  std::cerr << "birth 2" << std::endl;
  try {
      _will_require_triplets = JsonWrapper::GetProperty(
        config,
        "global_track_fit_will_require_triplet_space_points",
        JsonWrapper::booleanValue).asBool();
  } catch (MAUS::Exception* exc) {} // do nothing, we take the default
  std::cerr << "birth 3" << std::endl;

  if (_propagator != NULL)
      delete _propagator;
  std::cerr << "birth 4" << std::endl;
  _propagator = new Kalman::Global::Propagator();
  std::cerr << "birth 5" << std::endl;
  _propagator->SetMass(mu_mass); // BUG
  std::cerr << "birth 6" << std::endl;
  _propagator->SetField(Globals::GetMCFieldConstructor());
  std::cerr << "birth 7" << std::endl;
  
  double tof_sigma_t = JsonWrapper::GetProperty(
                        config,
                        "global_track_fit_tof_sigma_t",
                        JsonWrapper::realValue).asDouble();
  std::cerr << "birth 8" << std::endl;
  double sf_sigma_x = JsonWrapper::GetProperty(
                        config,
                        "global_track_fit_scifi_sigma_x",
                        JsonWrapper::realValue).asDouble();
  std::cerr << "birth 9" << std::endl;

  if (_kalman_fit != NULL)
      delete _kalman_fit;
  std::cerr << "birth 10" << std::endl;
  _kalman_fit = new Kalman::TrackFit(_propagator);
  std::cerr << "birth 11" << std::endl;

}


void MapCppGlobalTrackFit::_death() {
}


void MapCppGlobalTrackFit::_process(Data* data) const {
  Spill& spill = *(data->GetSpill());

  // return if not physics spill
  if (spill.GetDaqEventType() != "physics_event")
    return;

  for (unsigned int i = 0; i < spill.GetReconEvents()->size(); i++) {
      std::cerr << "Recon event " << i << std::endl;
      ReconEvent *event = spill.GetReconEvents()->at(i);
      if (!event) {
        continue;
      }
      track_fit(*event);
  }
  for (unsigned int i = 0; i < spill.GetReconEvents()->size(); i++) {
      GlobalEvent* global_event = spill.GetReconEvents()->at(i)->GetGlobalEvent();
      std::cerr << "Globals " << i << " " << global_event << std::endl;
      if (global_event != NULL)
          std::cerr << "Global Tracks size " << global_event->get_tracks()->size() << std::endl;
  }
}

void MapCppGlobalTrackFit::append_to_data(GlobalEvent& event, Kalman::Track fit) const {
    using namespace DataStructure;
    Global::Track* track = new Global::Track();
    track->set_pid(Global::kMuPlus);
    track->set_charge(1.);
    track->set_mapper_name("MapCppGlobalTrackFit");
    for (size_t i = 0; i < fit.GetLength(); ++i) {
        Kalman::State state = fit[i].GetFiltered(); // BUG should be smoothed
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
        mom[2] = ::sqrt(E*E-(mom[0]*mom[0]+mom[1]*mom[1]+mu_mass*mu_mass));
        point->set_position(pos);
        point->set_momentum(mom);
        track->AddTrackPoint(point);
    }
    track->SortTrackPointsByZ();
    event.add_track_recursive(track);
}

void MapCppGlobalTrackFit::track_fit(ReconEvent &event) const {
    std::cerr << "MapCppGlobalTrackFit A" << std::endl;
    build_data(event);
    std::cerr << "MapCppGlobalTrackFit B" << std::endl;
    build_seed(event);

    std::cerr << "MapCppGlobalTrackFit C" << std::endl;
    _kalman_fit->Filter(true);
    std::cerr << "MapCppGlobalTrackFit D" << std::endl;
    _kalman_fit->Smooth(true);
    std::cerr << "MapCppGlobalTrackFit E" << std::endl;

    Kalman::Track track = _kalman_fit->GetTrack();
    for (size_t i = 0; i < track.GetLength(); ++i) {
        std::cerr << "Initial track " << i << "\n" 
                  << track[i].GetData() << std::endl;
        std::cerr << "Filtered track " << i << "\n"
                  << track[i].GetFiltered() << std::endl;
        //std::cerr << "Smoothed track " << i << "\n" << s_track[i] << std::endl;
        std::cerr << "\n" << std::endl;
    }

    if (event.GetGlobalEvent() == NULL)
        event.SetGlobalEvent(new GlobalEvent());
    append_to_data(*(event.GetGlobalEvent()), track);
}

void MapCppGlobalTrackFit::build_seed(ReconEvent& event) const {
    std::cerr << "Build seed tof" << std::endl;
    auto tof_seed_list = {kTOF1, kTOF2};
    std::vector<DetectorPoint> tof_seed_points(tof_seed_list);
    std::cerr << "  tof container" << std::endl;
    MAUS::Kalman::TrackFit tof_container(new Kalman::Global::Propagator());
    std::cerr << "  tof data" << std::endl;
    DataLoader tof_data(tof_seed_points, &tof_container);
    std::cerr << "  load tof event" << std::endl;
    tof_data.load_event(event);
    std::cerr << "  get tof tracl" << std::endl;
    Kalman::Track tof_track = tof_container.GetTrack();
    std::cerr << "  check track length" << std::endl;
    if (tof_track.GetLength() != 2) {
        std::cerr << "      failed check track length " << tof_track.GetLength() << std::endl;
        throw MAUS::Exception(Exception::recoverable,
                              "tof_track was not of length 2",
                              "MapCppGlobalTrackFit::build_seed");
    }
    std::cerr << "  Get dt" << std::endl;
    double tof_dt = tof_track[1].GetData().GetVector()[0][0]-
                    tof_track[0].GetData().GetVector()[0][0];
    std::cerr << "  Get dz" << std::endl;
    double tof_dz = (tof_track[1].GetPosition() -
                     tof_track[0].GetPosition());
    double beta = tof_dz/tof_dt/c_light;
    std::cerr << "  Beta: " << beta << " dt: " << tof_dt
                  << " dz: " << tof_dz << " c: " << c_light << std::endl;

    if (fabs(beta) >= 1.) {
        throw MAUS::Exception(Exception::recoverable,
                              "beta was >= 1",
                              "MapCppGlobalTrackFit::build_seed");
    }
    double gamma = 1/::sqrt(1-beta*beta);
    double seed_pz = mu_mass*beta*gamma;
    std::cerr << "  Nan check" << std::endl;
    if (seed_pz != seed_pz)
        throw MAUS::Exception(Exception::recoverable,
                              "Failed to calculate seed pz (maybe beta > 1?)",
                              "MapCppGlobalTrackFit::build_seed");

    std::cerr << "Build seed tracker" << std::endl;
    auto pos_seed_list = {kTracker0_1, kTracker0_2};
    std::vector<DetectorPoint> pos_seed_points(pos_seed_list);
    MAUS::Kalman::TrackFit sf_container(new Kalman::Global::Propagator());
    DataLoader sf_data(pos_seed_points, &sf_container);
    sf_data.SetWillRequireTrackerTriplets(_will_require_triplets);
    sf_data.load_event(event);
    Kalman::Track sf_track = sf_container.GetTrack();
    if (sf_track.GetLength() != 2) {
        throw MAUS::Exception(Exception::recoverable, "sf_track was not of length 2", "MapCppGlobalTrackFit::build_seed");
    }
    Kalman::State seed_state(6);
    TMatrixD sf_0 = sf_track[0].GetData().GetVector();
    TMatrixD sf_1 = sf_track[1].GetData().GetVector();
    double dx = sf_1[0][0] - sf_0[0][0];
    double dy = sf_1[1][0] - sf_0[1][0];
    double dz = sf_track[1].GetPosition()-sf_track[0].GetPosition();
    std::cerr << "  dz " << sf_track[1].GetPosition() << " " << sf_track[0].GetPosition() << std::endl;

    TMatrixD vector(6, 1);
    vector[0][0] = tof_dt/tof_dz*(sf_track[0].GetPosition()-tof_track[0].GetPosition());
    vector[1][0] = sf_0[0][0];
    vector[2][0] = sf_0[1][0];
    vector[4][0] = dx/dz*seed_pz;
    vector[5][0] = dy/dz*seed_pz;
    vector[3][0] = ::sqrt(seed_pz*seed_pz + vector[4][0]*vector[4][0] +
                          vector[5][0]*vector[5][0] + mu_mass*mu_mass);
    seed_state.SetVector(vector);
    double err = 1e6;
    double errors[] = {
        err,  0.,  0.,  0.,  0., 0.,
         0., err,  0.,  0.,  0., 0.,
         0.,  0., err,  0.,  0., 0.,
         0.,  0.,  0., err,  0., 0.,
         0.,  0.,  0.,  0., err, 0.,
         0.,  0.,  0.,  0.,  0., err,
    };
    TMatrixD covariance(6, 6, errors);
    seed_state.SetCovariance(covariance);
    std::cerr << "MapCppGlobalTrackFit::build_seed built seed with:\n" << vector << "\n" << seed_state.GetVector() << std::endl;
    _kalman_fit->SetSeed(seed_state);
}

void MapCppGlobalTrackFit::build_data(ReconEvent& event) const {
    DataLoader data(_active_detectors, _kalman_fit);
    data.SetWillRequireTrackerTriplets(_will_require_triplets);
    data.load_event(event);
}

} // ~namespace MAUS
