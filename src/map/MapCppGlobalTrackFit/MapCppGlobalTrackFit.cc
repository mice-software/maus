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

#include "src/common_cpp/Recon/Kalman/GlobalPropagator.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/Recon/Kalman/GlobalMeasurement.hh"

#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/map/MapCppGlobalTrackFit/DataLoader.hh"
#include "src/map/MapCppGlobalTrackFit/MapCppGlobalTrackFit.hh"

// TODO: 
// * Error propogation through fields
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
// * Add virtual planes/disabled detectors (make a state with 0 length?);
// * Interface to event display
// * Residuals plots (etc) reducer
// * Move Global kalman stuff into subdirectory/namespace
// * Documentation
// * Improve test coverage
// * Dynamic step size to step to volume boundary (using GSL Control type)
// * Enable/disable material material name/conversion logic

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
    Kalman::GlobalMeasurement::SetupDetectorToMeasurementMap();
}


MapCppGlobalTrackFit::~MapCppGlobalTrackFit() {
}


void MapCppGlobalTrackFit::_birth(const std::string& config_str) {
  // Pull out the global settings
  Json::Value config = JsonWrapper::StringToJson(config_str);
  _will_require_triplets = true;
  try {
      _will_require_triplets = JsonWrapper::GetProperty(
        config,
        "global_track_fit_will_require_triplet_space_points",
        JsonWrapper::booleanValue).asBool();
  } catch (MAUS::Exception* exc) {} // do nothing, we take the default

  if (_propagator != NULL)
      delete _propagator;
  _propagator = new Kalman::GlobalPropagator();
  _propagator->SetMass(mu_mass); // BUG
  _propagator->SetField(Globals::GetMCFieldConstructor());


  
  double tof_sigma_t = JsonWrapper::GetProperty(
                        config,
                        "global_track_fit_tof_sigma_t",
                        JsonWrapper::realValue).asDouble();
  double sf_sigma_x = JsonWrapper::GetProperty(
                        config,
                        "global_track_fit_scifi_sigma_x",
                        JsonWrapper::realValue).asDouble();

  if (_measurement != NULL)
      delete _measurement;
  _measurement = new Kalman::GlobalMeasurement();
  _measurement->SetTofSigmaT(tof_sigma_t);
  _measurement->SetSFSigmaX(sf_sigma_x);

  if (_kalman_fit != NULL)
      delete _kalman_fit;
  _kalman_fit = new Kalman::TrackFit(_propagator, _measurement);
}


void MapCppGlobalTrackFit::_death() {
}


void MapCppGlobalTrackFit::_process(Data* data) const {
  Spill& spill = *(data->GetSpill());

  /* return if not physics spill */
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
        Kalman::State state = fit[i];
        Global::TrackPoint* point = new Global::TrackPoint();
        point->set_mapper_name("MapCppGlobalTrackFit");
        point->set_particle_event(-99); // BUG
        TLorentzVector pos(state.GetVector()[0][0], state.GetVector()[1][0], state.GetVector()[2][0], state.GetPosition());
        TLorentzVector mom(0., state.GetVector()[4][0], state.GetVector()[5][0], state.GetVector()[3][0]);
        double E = state.GetVector()[3][0];
        mom[0] = ::sqrt(E*E-(mom[0]*mom[0]+mom[1]*mom[1]+mu_mass*mu_mass));
        point->set_position(pos);
        point->set_momentum(mom);
        track->AddTrackPoint(point);
    }
    track->SortTrackPointsByZ();
    event.add_track_recursive(track);
}

void MapCppGlobalTrackFit::track_fit(ReconEvent &event) const {
    std::cerr << "MapCppGlobalTrackFit A" << std::endl;
    Kalman::Track data_track = build_data(event);
    std::cerr << "MapCppGlobalTrackFit B" << std::endl;
    if (data_track.GetLength() == 0)
        return;
    std::cerr << "MapCppGlobalTrackFit C" << std::endl;
    Kalman::State seed = build_seed(event);

    std::cerr << "MapCppGlobalTrackFit D" << std::endl;
    _kalman_fit->SetData(data_track);
    _kalman_fit->SetSeed(seed);

    std::cerr << "MapCppGlobalTrackFit E1" << std::endl;
    _kalman_fit->Filter(true);
    std::cerr << "MapCppGlobalTrackFit E2" << std::endl;
    _kalman_fit->Smooth(true);
    std::cerr << "MapCppGlobalTrackFit F" << std::endl;
    Kalman::Track smoothed = _kalman_fit->GetSmoothed();

    std::cerr << "Fitting to track" << std::endl;
    Kalman::Track f_track = _kalman_fit->GetFiltered();
    Kalman::Track s_track = _kalman_fit->GetSmoothed();
    for (size_t i = 0; i < data_track.GetLength(); ++i) {
        std::cerr << "Initial track " << i << "\n" << data_track[i] << std::endl;
        std::cerr << "Filtered track " << i << "\n" << f_track[i] << std::endl;
        //std::cerr << "Smoothed track " << i << "\n" << s_track[i] << std::endl;
        std::cerr << "\n" << std::endl;
    }

    if (event.GetGlobalEvent() == NULL)
        event.SetGlobalEvent(new GlobalEvent());
    append_to_data(*(event.GetGlobalEvent()), smoothed);
}

Kalman::State MapCppGlobalTrackFit::build_seed(ReconEvent& event) const {
    Kalman::GlobalMeasurement* dummy_measurement = new Kalman::GlobalMeasurement();
    auto tof_seed_list = {kTOF1, kTOF2};
    std::vector<DetectorPoint> tof_seed_points(tof_seed_list);
    DataLoader tof_data(tof_seed_points);
    tof_data.load_event(event, dummy_measurement);
    Kalman::Track tof_track = tof_data.get_fit_data();
    if (tof_track.GetLength() != 2) {
        throw MAUS::Exception(Exception::recoverable,
                              "tof_track was not of length 2",
                              "MapCppGlobalTrackFit::build_seed");
    }
    double tof_dt = tof_track[1].GetVector()[0][0]-tof_track[0].GetVector()[0][0];
    double tof_dz = (tof_track[1].GetPosition() - tof_track[0].GetPosition());
    double beta = tof_dz/tof_dt/c_light;
    double gamma = 1/::sqrt(1-beta*beta);
    double seed_pz = mu_mass*beta*gamma;
    if (seed_pz != seed_pz)
        throw MAUS::Exception(Exception::recoverable,
                              "Failed to calculate seed pz (maybe beta > 1?)",
                              "MapCppGlobalTrackFit::build_seed");

    auto pos_seed_list = {kTracker0_1, kTracker0_2};
    std::vector<DetectorPoint> pos_seed_points(pos_seed_list);
    DataLoader sf_data(pos_seed_points);
    sf_data.SetWillRequireTrackerTriplets(_will_require_triplets);
    sf_data.load_event(event, dummy_measurement);
    Kalman::Track sf_track = sf_data.get_fit_data();
    if (sf_track.GetLength() != 2) {
        throw MAUS::Exception(Exception::recoverable, "sf_track was not of length 2", "MapCppGlobalTrackFit::build_seed");
    }
    Kalman::State seed_state(6, sf_track[0].GetPosition());
    double dx = sf_track[1].GetVector()[0][0]-sf_track[0].GetVector()[0][0];
    double dy = sf_track[1].GetVector()[1][0]-sf_track[0].GetVector()[1][0];
    double dz = sf_track[1].GetPosition()-sf_track[0].GetPosition();

    TMatrixD vector(6, 1);
    vector[0][0] = tof_dt/tof_dz*(sf_track[0].GetPosition()-tof_track[0].GetPosition());
    vector[1][0] = sf_track[0].GetVector()[0][0];
    vector[2][0] = sf_track[0].GetVector()[1][0];
    vector[4][0] = dx/dz*seed_pz;
    vector[5][0] = dy/dz*seed_pz;
    vector[3][0] = ::sqrt(seed_pz*seed_pz + vector[4][0]*vector[4][0]+vector[5][0]*vector[5][0]+mu_mass*mu_mass);
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
    delete dummy_measurement;
    std::cerr << "MapCppGlobalTrackFit::build_seed built seed with:\n" << seed_state << std::endl;
    return seed_state;
}

Kalman::Track MapCppGlobalTrackFit::build_data(ReconEvent& event) const {
    DataLoader data(_active_detectors);
    data.SetWillRequireTrackerTriplets(_will_require_triplets);
    data.load_event(event, _measurement);
    std::cerr << "MapCppGlobalTrackFit::build_data built data with:" << std::endl;
    std::cerr << data.get_fit_data() << std::endl;;
    return data.get_fit_data();
}

} // ~namespace MAUS
