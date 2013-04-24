/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributedTrackReconstructor in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 /* Author: Peter Lane
 */

#include "src/map/MapCppGlobalRawTracks/MapCppGlobalRawTracks.hh"

// C++
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

// External
#include "TMinuit.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "json/json.h"

// Legacy/G4MICE
#include "Interface/Squeal.hh"

// MAUS
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/DataStructure/Global/BasePoint.hh"
#include "src/common_cpp/DataStructure/Global/SpacePoint.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Recon/Global/DataStructureHelper.hh"
#include "src/common_cpp/Recon/Global/Detector.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

namespace GlobalDS = ::MAUS::DataStructure::Global;
using MAUS::recon::global::DataStructureHelper;
using MAUS::recon::global::Detector;
using MAUS::recon::global::DetectorMap;
using MAUS::recon::global::Particle;

MapCppGlobalRawTracks::MapCppGlobalRawTracks() {
}

MapCppGlobalRawTracks::~MapCppGlobalRawTracks() {
}

bool MapCppGlobalRawTracks::birth(std::string configuration) {
  // parse the JSON document.
  try {
    configuration_ = JsonWrapper::StringToJson(configuration);
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(
      squee, MapCppGlobalRawTracks::kClassname);
    return false;
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(
      exc, MapCppGlobalRawTracks::kClassname);
    return false;
  }

  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  switch (reference_pgparticle.pid) {
    case MAUS::DataStructure::Global::kEPlus:
    case MAUS::DataStructure::Global::kMuPlus:
    case MAUS::DataStructure::Global::kPiPlus:
      beam_polarity_ = 1;
      break;
    case MAUS::DataStructure::Global::kEMinus:
    case MAUS::DataStructure::Global::kMuMinus:
    case MAUS::DataStructure::Global::kPiMinus:
      beam_polarity_ = -1;
      break;
    default:
      throw(Squeal(Squeal::nonRecoverable,
                   "Reference particle is not a pion+/-, muon+/-, or e+/-.",
                   "MapCppGlobalRawTracks::birth()"));

  }

  return true;  // Sucessful parsing
}

std::string MapCppGlobalRawTracks::process(std::string run_data_string) {
  // parse the JSON document.
  Json::Value run_data_json
    = Json::Value(JsonWrapper::StringToJson(run_data_string));
  if (run_data_json.isNull() || run_data_json.empty()) {
    return std::string("{\"errors\":{\"bad_json_document\":"
                        "\"Failed to parse input document\"}}");
  }

  JsonCppSpillConverter deserialize;
  MAUS::Data * run_data = deserialize(&run_data_json);
  if (!run_data) {
    return std::string("{\"errors\":{\"failed_json_cpp_conversion\":"
                        "\"Failed to convert Json to C++ Data object\"}}");
  }

  const MAUS::Spill * spill = run_data->GetSpill();
  MAUS::ReconEventPArray * recon_events = spill->GetReconEvents();
  if (!recon_events) {
    return run_data_string;
  }

  MAUS::ReconEventPArray::const_iterator recon_event;
  MAUS::GlobalEvent * global_event;
  for (recon_event = recon_events->begin();
      recon_event < recon_events->end();
      ++recon_event) {
    global_event = new GlobalEvent();
    // Load the ReconEvent, and import it into the GlobalEvent
    AssembleRawTracks(*recon_event, global_event);
  }

  // Serialize the Spill for passing on to the next map in the workflow
  CppJsonSpillConverter serialize;
  Json::FastWriter writer;
  std::string output = writer.write(*serialize(run_data));
  std::cout << "DEBUG MapCppGlobalRawTracks::process(): "
            << "Output: " << std::endl
            << output << std::endl;

  // Delete the GlobalEvent instance as well as any Track and TrackPoint
  // instances added to it using add_track_recursive().
  delete global_event;

  return output;
}

bool MapCppGlobalRawTracks::death() {
  return true;  // successful
}

/** AssembleRawTracks - load data provided by the DAQ system or via digitized MC
 *
 *  Pseudocode:
 *    1) Deserialize TOF and SciFi events
 *    2) Assemble all raw track permutations
 *    3) Prune any raw tracks that are unlickly or impossible
 *    4) Serialize raw tracks
 */
void MapCppGlobalRawTracks::AssembleRawTracks(
    MAUS::ReconEvent * recon_event,
    MAUS::GlobalEvent * global_event) {
  DetectorMap detectors;
  DataStructureHelper::GetInstance().GetDetectorAttributes(
      configuration_, detectors);

    // Load TOF and SciFi tracks from the appropriate recon event trees
    GlobalDS::TrackPArray tof_tracks;
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Loading TOF track..." << std::endl;
    LoadTOFTrack(detectors, recon_event, tof_tracks);
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Loaded " << tof_tracks.size() << " TOF tracks." << std::endl;

    GlobalDS::TrackPArray sci_fi_tracks;
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Loading SciFi track..." << std::endl;
    LoadSciFiTrack(detectors, recon_event, sci_fi_tracks);
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Loaded " << sci_fi_tracks.size() << " SciFi tracks." << std::endl;

  // Assuming there are an equal number of TOF and SciFi tracks,
  // merge each pair of tracks based on z position of the detectors
  GlobalDS::TrackPArray::iterator tof_track = tof_tracks.begin();
  GlobalDS::TrackPArray::iterator sci_fi_track = sci_fi_tracks.begin();
  while (tof_track != tof_tracks.end() ||
          sci_fi_track != sci_fi_tracks.end()) {
    std::vector<GlobalDS::TrackPoint const *> tof_track_points
      = (*tof_track)->GetTrackPoints();
    std::vector<GlobalDS::TrackPoint const *>::iterator tof_track_point;
    if (tof_track != tof_tracks.end()) {
      tof_track_point = tof_track_points.begin();
    }

    std::vector<GlobalDS::TrackPoint const *> sci_fi_track_points
      = (*sci_fi_track)->GetTrackPoints();
    std::vector<GlobalDS::TrackPoint const *>::iterator sci_fi_track_point;
    if (sci_fi_track != sci_fi_tracks.end()) {
      sci_fi_track_point = sci_fi_track_points.begin();
    }

    GlobalDS::Track * combined_track = new GlobalDS::Track();
    if (tof_track != tof_tracks.end()) {
      combined_track->AddTrack(*tof_track);
      while (tof_track_point != tof_track_points.end()) {
        combined_track->AddTrackPoint(
          new GlobalDS::TrackPoint(**tof_track_point));
        ++tof_track_point;
      }
    }
    if (sci_fi_track != sci_fi_tracks.end()) {
      combined_track->AddTrack(*sci_fi_track);
      while (sci_fi_track_point != sci_fi_track_points.end()) {
        combined_track->AddTrackPoint(
          new GlobalDS::TrackPoint(**sci_fi_track_point));
        ++sci_fi_track_point;
      }
    }
    combined_track->SortTrackPointsByZ();
    /*
    while ((tof_track != tof_tracks.end() &&
            tof_track_point != tof_track_points.end()) ||
            (sci_fi_track != sci_fi_tracks.end() &&
            sci_fi_track_point != sci_fi_track_points.end())) {
      if (tof_track != tof_tracks.end() &&
          tof_track_point != tof_track_points.end() &&
          (sci_fi_track == sci_fi_tracks.end() ||
          sci_fi_track_point == sci_fi_track_points.end())) {
        combined_track->AddTrackPoint(
          new GlobalDS::TrackPoint(**tof_track_point));
        ++tof_track_point;
      } else if (sci_fi_track != sci_fi_tracks.end() &&
                  sci_fi_track_point != sci_fi_track_points.end() &&
                  (tof_track == tof_tracks.end() ||
                  tof_track_point == tof_track_points.end())) {
        combined_track->AddTrackPoint(
          new GlobalDS::TrackPoint(**sci_fi_track_point));
        ++sci_fi_track_point;
      } else if (tof_track != tof_tracks.end() &&
                  sci_fi_track != sci_fi_tracks.end() &&
                  tof_track_point != tof_track_points.end() &&
                  sci_fi_track_point != sci_fi_track_points.end()) {
        if ((*tof_track_point)->z() <= (*sci_fi_track_point)->z()) {
          combined_track->AddTrackPoint(
            new GlobalDS::TrackPoint(**tof_track_point));
          ++tof_track_point;
        } else {
          combined_track->AddTrackPoint(
            new GlobalDS::TrackPoint(**sci_fi_track_point));
          ++sci_fi_track_point;
        }
      }
    }
    */
    global_event->add_track_recursive(combined_track);
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
        << "Combined track size: " << combined_track->GetTrackPoints().size()
        << std::endl;
    if (tof_track != tof_tracks.end()) {
      ++tof_track;
    }
    if (sci_fi_track != sci_fi_tracks.end()) {
      ++sci_fi_track;
    }
  }

  recon_event->SetGlobalEvent(global_event);
}


void MapCppGlobalRawTracks::LoadTOFTrack(
    const DetectorMap & detectors,
    MAUS::ReconEvent const * const recon_event,
    GlobalDS::TrackPArray & tof_tracks) {
  const TOFEvent * tof_event = recon_event->GetTOFEvent();
  const TOFEventSpacePoint space_point_events
    = tof_event->GetTOFEventSpacePoint();
  const std::vector<TOFSpacePoint> tof0_space_points
    = space_point_events.GetTOF0SpacePointArray();
  const std::vector<TOFSpacePoint> tof1_space_points
    = space_point_events.GetTOF1SpacePointArray();
  const std::vector<TOFSpacePoint> tof2_space_points
    = space_point_events.GetTOF2SpacePointArray();

  GlobalDS::Track * track = new GlobalDS::Track();
  track->set_mapper_name(kClassname);

  TLorentzVector deltas[2];

  GlobalDS::TrackPointPArray track_points;

  // FIXME(Lane) Replace hard coded slab to coordinate conversion once
  // Mark Reiner's code has been incorporated into TOF space point code.
  const Detector& tof0 = detectors.find(GlobalDS::kTOF0)->second;
  std::vector<TOFSpacePoint>::const_iterator tof0_space_point;
  for (tof0_space_point = tof0_space_points.begin();
       tof0_space_point < tof0_space_points.end();
       ++tof0_space_point) {
    GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
    PopulateTOFTrackPoint(tof0, tof0_space_point, 4., 10, track_point);
    track_points.push_back(track_point);

    deltas[0] = track_point->get_position();
  }

  const Detector& tof1 = detectors.find(GlobalDS::kTOF1)->second;
  std::vector<TOFSpacePoint>::const_iterator tof1_space_point;
  for (tof1_space_point = tof1_space_points.begin();
       tof1_space_point < tof1_space_points.end();
       ++tof1_space_point) {
    GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
    PopulateTOFTrackPoint(tof1, tof1_space_point, 6., 7, track_point);
    track_points.push_back(track_point);

    deltas[0] = track_point->get_position() - deltas[0];
    deltas[1] = track_point->get_position();
  }

  const Detector& tof2 = detectors.find(GlobalDS::kTOF2)->second;
  std::vector<TOFSpacePoint>::const_iterator tof2_space_point;
  for (tof2_space_point = tof2_space_points.begin();
       tof2_space_point < tof2_space_points.end();
       ++tof2_space_point) {
    GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
    PopulateTOFTrackPoint(tof2, tof2_space_point, 6., 10, track_point);
    track_points.push_back(track_point);

    deltas[1] = track_point->get_position() - deltas[1];
  }

  // Approximate momenta by using tof0/tof1 and tof1/tof2 position deltas
  TLorentzVector momenta[2];
  for (size_t index = 0; index < 2; ++index) {
    double delta_x = deltas[index].X();
    double delta_y = deltas[index].Y();
    double delta_z = deltas[index].Z();
    double delta_t = deltas[index].T();
    double delta_l = std::sqrt(delta_x*delta_x + delta_y*delta_y
                                + delta_z*delta_z);
    double beta = delta_l / delta_t / ::CLHEP::c_light;
    double gamma = 1. / std::sqrt(1 - beta*beta);
    /* FIXME(Lane) Forcing muon PID for now. Should be
    * GlobalDS::PID particle_id = IdentifyParticle(beta);
    * plus code to create multiple hypotheses (i.e. pion/muon ambiguity)
    */
    GlobalDS::PID particle_id
      = GlobalDS::PID(GlobalDS::kMuPlus * beam_polarity_);
    double mass = Particle::GetInstance().GetMass(particle_id);

    momenta[index] = TLorentzVector(mass * delta_x / delta_t,
                                    mass * delta_y / delta_t,
                                    gamma * mass * beta,  // pz ~= p
                                    gamma * mass);
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTracks(): "
              << "Particle: " << Particle::GetInstance().GetName(particle_id)
              << "\tbeta: " << beta << "\t 4-momentum: (" << momenta[index].X()
              << ", " << momenta[index].Y() << ", " << momenta[index].Z()
              << std::endl;
  }

  // Set each track point's 4-momentum and add to the track
  std::vector<GlobalDS::TrackPoint *>::iterator track_point;
  for (track_point = track_points.begin();
       track_point < track_points.end();
       ++track_point) {
    const double z = (*track_point)->get_position().Z();
    if (z <= tof1.plane()) {
      (*track_point)->set_position(momenta[0]);
    } else {
      (*track_point)->set_position(momenta[1]);
    }

    track->AddTrackPoint(*track_point);
  }

  track->SortTrackPointsByZ();

  // FIXME(Lane) For now assume we've selected only muon tracks and no decays
  const GlobalDS::PID particle_id
    = GlobalDS::PID(GlobalDS::kMuPlus * beam_polarity_);
  track->set_pid(particle_id);

  tof_tracks.push_back(track);
}

void MAUS::MapCppGlobalRawTracks::PopulateTOFTrackPoint(
    const Detector & detector,
    const std::vector<TOFSpacePoint>::const_iterator & tof_space_point,
    const double slab_width,
    const size_t number_of_slabs,
    GlobalDS::TrackPoint * track_point) {
  const double max_xy = slab_width * (number_of_slabs - 0.5);

  GlobalDS::SpacePoint * space_point = new GlobalDS::SpacePoint();

  space_point->set_detector(detector.id());

  // FIXME(Lane) not sure what to put here
  space_point->set_geometry_path("");

  const double x = slab_width * tof_space_point->GetSlabx() - max_xy;
  const double y = slab_width * tof_space_point->GetSlabx() - max_xy;
  const double z = detector.plane();
  const double t = tof_space_point->GetTime();
  TLorentzVector position(x, y, z, t);
  space_point->set_position(position);

  dynamic_cast<GlobalDS::BasePoint*>(track_point)->operator=(*space_point);
  track_point->set_space_point(space_point);

  track_point->set_mapper_name(kClassname);

  CovarianceMatrix covariances = detector.uncertainties();
  TLorentzVector position_errors(::sqrt(covariances(3, 3)),
                                  ::sqrt(covariances(5, 5)),
                                  0.,
                                  ::sqrt(covariances(1, 1)));
  track_point->set_position_error(position_errors);

  TLorentzVector momentum_errors(::sqrt(covariances(4, 4)),
                                  ::sqrt(covariances(6, 6)),
                                  0.,
                                  ::sqrt(covariances(2, 2)));
  track_point->set_momentum_error(momentum_errors);

  track_point->set_charge(tof_space_point->GetChargeProduct());
}

void MapCppGlobalRawTracks::LoadSciFiTrack(
    const DetectorMap & detectors,
    MAUS::ReconEvent const * const recon_event,
    GlobalDS::TrackPArray & sci_fi_tracks) {
  const double z_offset
    = detectors.find(GlobalDS::kTracker1_5)->second.plane();
  std::cout << "DEBUG MapCppGlobalRawTracks::LoadSciFiTrack(): "
            << "z offset: " << z_offset << std::endl;

  SciFiEvent const * const sci_fi_event = recon_event->GetSciFiEvent();
  SciFiSpacePointPArray space_points = sci_fi_event->spacepoints();

  GlobalDS::Track * track = new GlobalDS::Track();
  for (SciFiSpacePointPArray::const_iterator space_point = space_points.begin();
       space_point < space_points.end();
       ++space_point) {

    const int tracker = (*space_point)->get_tracker();
    const int station = (*space_point)->get_station();
    const GlobalDS::DetectorPoint detector_id = GlobalDS::DetectorPoint(
        GlobalDS::kTracker1 + 6 * tracker + station);
    const Detector& detector = detectors.find(detector_id)->second;

  std::cout << "DEBUG MapCppGlobalRawTracks::PopulateSciFiTrackPoint(): "
            << "\tSciFi Space Point:" << std::endl
            << "\tDetector: " << detector_id
            << "\tStation: " << station << std::endl;

    GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
    PopulateSciFiTrackPoint(detector, space_point, track_point);
    track->AddTrackPoint(track_point);
  }

  track->SortTrackPointsByZ();

  // FIXME(Lane) For now assume we've selected only muon tracks and no decays
  const GlobalDS::PID particle_id
    = GlobalDS::PID(GlobalDS::kMuPlus * beam_polarity_);
  track->set_pid(particle_id);

  sci_fi_tracks.push_back(track);
}


void MapCppGlobalRawTracks::PopulateSciFiTrackPoint(
    const MAUS::recon::global::Detector & detector,
    const SciFiSpacePointPArray::const_iterator & scifi_space_point,
    MAUS::DataStructure::Global::TrackPoint * track_point) {
  GlobalDS::SpacePoint * space_point = new GlobalDS::SpacePoint();

  space_point->set_detector(detector.id());

  // FIXME(Lane) not sure what to put here
  space_point->set_geometry_path("");

  ThreeVector position = (*scifi_space_point)->get_position();
  const double time = (*scifi_space_point)->get_time();
  TLorentzVector four_position(position.x(), position.y(), position.z(), time);
  space_point->set_position(four_position);

  // FIXME(Lane) Replace MC momentum with official track momentum when that
  // information becomes available.
  ThreeVector momentum;
  const SciFiClusterPArray clusters = (*scifi_space_point)->get_channels();
  for (SciFiClusterPArray::const_iterator cluster = clusters.begin();
      cluster < clusters.end();
      ++cluster) {
    ThreeVector true_momentum = (*cluster)->get_true_momentum();
std::cout << "DEBUG MapCppGlobalRawTracks::LoadSciFiTrack(): " << std::endl
        << "\tTrue Momentum: " << true_momentum << std::endl;
    momentum += true_momentum;
  }
  if (clusters.size() > 0) {
    momentum /= clusters.size();
  }
  // smear the MC momentum1.293e+04
  momentum.setX(momentum.x() + ::CLHEP::RandGauss::shoot(0., 3.));
  momentum.setY(momentum.y() + ::CLHEP::RandGauss::shoot(0., 3.));
  momentum.setZ(momentum.z() + ::CLHEP::RandGauss::shoot(0., 20.));
std::cout << "DEBUG MapCppGlobalRawTracks::LoadSciFiTrack(): " << std::endl
        << "\tSmeared Momentum: " << momentum << std::endl;

  dynamic_cast<GlobalDS::BasePoint*>(track_point)->operator=(*space_point);
  track_point->set_space_point(space_point);

  track_point->set_mapper_name(kClassname);

  // FIXME(Lane) For now assume we've selected only muon tracks and no decays
  const GlobalDS::PID particle_id
    = GlobalDS::PID(GlobalDS::kMuPlus * beam_polarity_);
  const double beta = Beta(particle_id, momentum.mag());
  const double energy = momentum.mag() / beta;
  TLorentzVector four_momentum(
    momentum.x(), momentum.y(), momentum.z(), energy);
  track_point->set_momentum(four_momentum);

  std::cout << "DEBUG MapCppGlobalRawTracks::PopulateSciFiTrackPoint(): "
            << "\tSciFi Space Point:" << std::endl
            << std::setprecision(10)
            << "\tTime: " << time << "\tEnergy: " << energy << std::endl
            << "\t4-Position: (" << four_position.X() << ", "
            << four_position.Y() << ", " << four_position.Z() << ", "
            << four_position.T() << ")" << std::endl
            << "\tMomentum: (" << four_momentum.Px() << ", "
            << four_momentum.Py() << ", " << four_momentum.Pz() << ", "
            << four_momentum.Pt() << ")" << std::endl;

  CovarianceMatrix covariances = detector.uncertainties();
  TLorentzVector position_errors(::sqrt(covariances(3, 3)),
                                  ::sqrt(covariances(5, 5)),
                                  0.,
                                  ::sqrt(covariances(1, 1)));
  track_point->set_position_error(position_errors);

  TLorentzVector momentum_errors(::sqrt(covariances(4, 4)),
                                  ::sqrt(covariances(6, 6)),
                                  0.,
                                  ::sqrt(covariances(2, 2)));
  track_point->set_momentum_error(momentum_errors);

  track_point->set_charge((*scifi_space_point)->get_npe());
}

/* Take an educated guess at the particle ID based on the axial velocity
 * of the particle (beta)
 */
GlobalDS::PID MapCppGlobalRawTracks::IdentifyParticle(const double beta) {
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const double reference_momentum = reference_pgparticle.pz;  // good enough
  const double beta_pi = Beta(GlobalDS::kPiPlus, reference_momentum);
  const double beta_mu = Beta(GlobalDS::kMuPlus, reference_momentum);
  const double beta_e = Beta(GlobalDS::kEPlus, reference_momentum);
  std::cout << "DEBUG MapCppGlobalRawTracks::IdentifyParticle(): " << std::endl
            << "\tbeta = " << beta << "\tbeta_pi = " << beta_pi << std::endl
            << "\tbeta_mu = " << beta_mu << "\tbeta_e = " << beta_e << std::endl;

  const double beta_fit[3] = {
    std::abs(1. - beta / beta_pi),
    std::abs(1. - beta / beta_mu),
    std::abs(1. - beta / beta_e)
  };
  std::cout << "DEBUG MapCppGlobalRawTracks::IdentifyParticle(): "
            << "beta fits {" << std::endl << std::setprecision(4)
            << "\t" << 1. - beta / beta_pi
            << "\t" << beta_fit[1]
            << "\t" << beta_fit[2]
            << std::endl << "}" << std::endl;
  size_t min_index = 0;
  for (size_t index = 1; index < 3; ++index) {
    if (beta_fit[index] < beta_fit[min_index]) {
      min_index = index;
    }
  }

  GlobalDS::PID particle_id = GlobalDS::kNoPID;
  switch(min_index) {
    case 0: particle_id = GlobalDS::PID(GlobalDS::kPiPlus * beam_polarity_);
            break;
    case 1: particle_id = GlobalDS::PID(GlobalDS::kMuPlus * beam_polarity_);
            break;
    case 2: particle_id = GlobalDS::PID(GlobalDS::kEPlus * beam_polarity_);
            break;
  }

  return particle_id;
}

double MapCppGlobalRawTracks::Beta(GlobalDS::PID pid, const double momentum) {
  const double mass = Particle::GetInstance().GetMass(pid);
  const double one_over_beta_squared
    = 1+ (mass*mass) / (momentum*momentum);
  return 1. / std::sqrt(one_over_beta_squared);
}

const std::string MapCppGlobalRawTracks::kClassname
  = "MapCppGlobalRawTracks";

}  // namespace MAUS

