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

#include "src/map/MapCppPolynomialMapResiduals/MapCppPolynomialMapResiduals.hh"

// C++
#include <cmath>
#include <string>
#include <vector>

// External
#include "json/json.h"

// Legacy/G4MICE
#include "Interface/Squeal.hh"

// MAUS
#include "Converter/DataConverters/JsonCppSpillConverter.hh"
#include "Converter/DataConverters/CppJsonSpillConverter.hh"
#include "DataStructure/Data.hh"
#include "DataStructure/GlobalEvent.hh"
#include "DataStructure/MCEvent.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/VirtualHit.hh"
#include "DataStructure/ThreeVector.hh"
#include "DataStructure/Global/ReconEnums.hh"
#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "Optics/PolynomialOpticsModel.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "Utils/Globals.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/CppErrorHandler.hh"

namespace MAUS {

namespace GlobalDS = ::MAUS::DataStructure::Global;
namespace Recon = ::MAUS::recon::global;

MapCppPolynomialMapResiduals::MapCppPolynomialMapResiduals() {
}

MapCppPolynomialMapResiduals::~MapCppPolynomialMapResiduals() {
}

bool MapCppPolynomialMapResiduals::birth(std::string configuration_string) {
  // parse the JSON document.
  try {
    Json::Value configuration = JsonWrapper::StringToJson(configuration_string);

    optics_model_ = new PolynomialOpticsModel(&configuration);

    optics_model_->Build();
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(
      squee, MapCppPolynomialMapResiduals::kClassname);
    return false;
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(
      exc, MapCppPolynomialMapResiduals::kClassname);
    return false;
  }

  return true;  // Sucessful parsing
}

std::string MapCppPolynomialMapResiduals::process(std::string run_data_string) {
  Json::Value run_data = JsonWrapper::StringToJson(run_data_string);
  ProcessableObject<Data> run = DeserializeRun(run_data);
  return ProcessRun(run);
}

ProcessableObject<Data> MapCppPolynomialMapResiduals::DeserializeRun(
    Json::Value & run) const {
  if (run.isNull() || run.empty()) {
    return ProcessableObject<MAUS::Data>(
      std::string("{\"errors\":{\"bad_json_document\":"
                  "\"Failed to parse input document\"}}"));
  } else {
    JsonCppSpillConverter deserialize;
    return ProcessableObject<Data>(deserialize(&run));
  }
}

std::string MapCppPolynomialMapResiduals::ProcessRun(
    ProcessableObject<Data> & run_data) const {
  if (run_data.error_string() != NULL) {
    return *run_data.error_string();
  } else if (run_data.object() == NULL) {
    return std::string("{\"errors\":{\"failed_json_cpp_conversion\":"
                        "\"Failed to convert Json to C++ Data object\"}}");
  } else {
    const Data * run = run_data.object();
    ProcessableObject<bool> result
      = ProcessSpill(run->GetSpill());
    return SerializeRun(run, result);
  }
}

std::string MapCppPolynomialMapResiduals::SerializeRun(
    const Data * run,
    ProcessableObject<bool> result) const {
  if (result.error_string() != NULL) {
    return *result.error_string();
  } else {
    CppJsonSpillConverter serialize;
    Json::FastWriter writer;
    std::string output = writer.write(*serialize(run));

    delete run;

    return output;
  }
}

ProcessableObject<bool> MapCppPolynomialMapResiduals::ProcessSpill(
    const MAUS::Spill * spill) const {
  if (spill == NULL) {
    return ProcessableObject<bool>(
      std::string("{\"errors\":{\"null_spill\":"
                  "\"The run data did not contain a spill.\"}}"));
  } else {
     return GenerateResiduals(spill->GetMCEvents(), spill->GetReconEvents());
  }
}

ProcessableObject<bool> MapCppPolynomialMapResiduals::GenerateResiduals(
    const MCEventPArray * mc_events,
    const MAUS::ReconEventPArray * recon_events) const {
  std::vector<std::vector<PhaseSpaceVector> > mc_tracks
    = ExtractMonteCarloTracks(mc_events);
  std::cout << "DEBUG MapCppPolynomialMapResiduals::GenerateResiduals: "
            << "Extracted " << mc_tracks.size() << " MC tracks."
            << std::endl;
  std::cout << "DEBUG MapCppPolynomialMapResiduals::GenerateResiduals: "
            << "Extracted " << mc_tracks[0].size() << " MC track points per track."
            << std::endl;

  std::vector<Primary *> primaries = ExtractBeamPrimaries(mc_events);
  std::cout << "DEBUG MapCppPolynomialMapResiduals::GenerateResiduals: "
            << "Extracted " << primaries.size() << " primaries."
            << std::endl;
  std::vector<long> z_positions = optics_model_->GetAvailableMapPositions();
  std::cout << "DEBUG MapCppPolynomialMapResiduals::GenerateResiduals: "
            << "There are " << z_positions.size() << " maps available."
            << std::endl;
  std::vector<std::vector<PhaseSpaceVector> > mapped_tracks
    = TransportBeamPrimaries(
        optics_model_,
        z_positions,
        primaries);
  std::cout << "DEBUG MapCppPolynomialMapResiduals::GenerateResiduals: "
            << "Mapped " << mapped_tracks.size() << " primaries."
            << std::endl;
  std::cout << "DEBUG MapCppPolynomialMapResiduals::GenerateResiduals: "
            << "Obtained " << mapped_tracks[0].size() << " mapped hits."
            << std::endl;

  const std::vector<std::vector<PhaseSpaceVector> > residuals
    = CalculateResiduals(mapped_tracks, mc_tracks);
  std::cout << "DEBUG MapCppPolynomialMapResiduals::GenerateResiduals: "
            << "Calculated " << residuals.size() << " residual tracks."
            << std::endl;
  std::cout << "DEBUG MapCppPolynomialMapResiduals::GenerateResiduals: "
            << "Calculated " << residuals[0].size() << " residuals per track."
            << std::endl;

  ProcessableObject<bool> result
    = WriteResiduals(*recon_events, z_positions, residuals);
  return result;
}

std::vector<std::vector<PhaseSpaceVector> >
MapCppPolynomialMapResiduals::ExtractMonteCarloTracks(
    const MCEventPArray * mc_events) const {
  std::vector<std::vector<PhaseSpaceVector> > tracks;
  MCEventPArray::const_iterator mc_event = mc_events->begin();
  while (mc_event != mc_events->end()) {
    std::vector<PhaseSpaceVector> track;
    VirtualHitArray * hits = (*mc_event)->GetVirtualHits();
    VirtualHitArray::const_iterator hit = hits->begin();
    while (hit != hits->end()) {
      track.push_back(VirtualHit2PhaseSpaceVector(*hit));
      ++hit;
    }
    tracks.push_back(track);
    ++mc_event;
  }
  return tracks;
}

std::vector<Primary *>
MapCppPolynomialMapResiduals::ExtractBeamPrimaries(
    const MCEventPArray * mc_events) const {
  std::vector<Primary *> primaries;
  MCEventPArray::const_iterator mc_event = mc_events->begin();
  while (mc_event != mc_events->end()) {
    primaries.push_back((*mc_event)->GetPrimary());
    ++mc_event;
  }
  return primaries;
}

std::vector<std::vector<PhaseSpaceVector> >
MapCppPolynomialMapResiduals::CalculateResiduals(
    const std::vector<std::vector<PhaseSpaceVector> > & mapped_tracks,
    const std::vector<std::vector<PhaseSpaceVector> > & mc_tracks) const {
  std::vector<std::vector<PhaseSpaceVector> > residuals;
  if (mapped_tracks.size() != mc_tracks.size()) {
    return residuals;
  }

  std::vector<std::vector<PhaseSpaceVector> >::const_iterator mapped_track
    = mapped_tracks.begin();
  std::vector<std::vector<PhaseSpaceVector> >::const_iterator mc_track
    = mc_tracks.begin();
  while (mapped_track != mapped_tracks.end()) {
    residuals.push_back(CalculateResiduals(*mapped_track, *mc_track));
    ++mapped_track;
    ++mc_track;
  }
  return residuals;
}

std::vector<PhaseSpaceVector>
MapCppPolynomialMapResiduals::CalculateResiduals(
    const std::vector<PhaseSpaceVector> & mapped_hits,
    const std::vector<PhaseSpaceVector> & mc_hits) const {
  std::vector<PhaseSpaceVector> residuals;
  if (mapped_hits.size() != mc_hits.size()) {
    std::cout << "DEBUG MapCppPolynomialMapResiduals::CalculateResiduals: "
              << "Hit count mismatch. # Mapped Hits: " << mapped_hits.size()
              << "\t# MC Hits: " << mc_hits.size()
              << std::endl;
    return residuals;
  }

  std::vector<PhaseSpaceVector>::const_iterator mapped_hit
    = mapped_hits.begin();
  std::vector<PhaseSpaceVector>::const_iterator mc_hit
    = mc_hits.begin();
  while (mapped_hit != mapped_hits.end()) {
    residuals.push_back(*mapped_hit - *mc_hit);
    ++mapped_hit;
    ++mc_hit;
  }
  std::cout << "DEBUG MapCppPolynomialMapResiduals::CalculateResiduals: "
            << "Calculated " << residuals.size() << " residuals."
            << std::endl;
  return residuals;
}

std::vector<std::vector<PhaseSpaceVector> >
MapCppPolynomialMapResiduals::TransportBeamPrimaries(
    PolynomialOpticsModel const * const optics_model,
    std::vector<long> z_positions,
    std::vector<Primary *> primaries) const {
  std::vector<std::vector<PhaseSpaceVector> > tracks;
  std::vector<Primary *>::const_iterator primary = primaries.begin();
  while (primary != primaries.end()) {
    std::vector<PhaseSpaceVector> track;
    std::vector<long>::const_iterator z_position = z_positions.begin();
    while (z_position != z_positions.end()) {
      track.push_back(optics_model->Transport(
        Primary2PhaseSpaceVector(**primary), *z_position));
      ++z_position;
    }
    std::cout << "DEBUG MapCppPolynomialMapResiduals::TransportBeamPrimaries: "
              << "Transported primary to " << track.size() << " detectors."
              << std::endl;
    tracks.push_back(track);
    ++primary;
  }
  std::cout << "DEBUG MapCppPolynomialMapResiduals::TransportBeamPrimaries: "
            << "Transported " << tracks.size() << " primaries."
            << std::endl;
  return tracks;
}

PhaseSpaceVector MapCppPolynomialMapResiduals::VirtualHit2PhaseSpaceVector(
    const VirtualHit & hit) const {
  const double time = hit.GetTime();
  const double mass = hit.GetMass();
  const ThreeVector positions = hit.GetPosition();
  const ThreeVector momenta = hit.GetMomentum();
  const double momentum = momenta.mag();
  const double p2 = momentum*momentum;
  const double energy = p2 / ::sqrt(mass*mass + p2);
  return PhaseSpaceVector(time, energy, positions.x(), momenta.x(),
                          positions.y(), momenta.y());
}

PhaseSpaceVector MapCppPolynomialMapResiduals::Primary2PhaseSpaceVector(
    const Primary & primary) const {
  const double time = primary.GetTime();
  const double energy = primary.GetEnergy();
  const ThreeVector positions = primary.GetPosition();
  const ThreeVector momenta = primary.GetMomentum();
  return PhaseSpaceVector(time, energy, positions.x(), momenta.x(),
                          positions.y(), momenta.y());
}

ProcessableObject<bool> MapCppPolynomialMapResiduals::WriteResiduals(
    const ReconEventPArray & recon_events,
    const std::vector<long> z_positions,
    const std::vector<std::vector<PhaseSpaceVector> > & residuals) const {
  if (residuals.size() != recon_events.size()) {
    return ProcessableObject<bool>(
      std::string("{\"errors\":{\"size_mismatch\":"
                  "\"The number of track residuals is not the same as the "
                  "number of recon events.\"}}"));
  }
  std::cout << "DEBUG MapCppPolynomialMapResiduals::WriteResiduals: "
            << "writing " << residuals.size() << "residual tracks."
            << std::endl;

  ReconEventPArray::const_iterator recon_event = recon_events.begin();
  std::vector<std::vector<PhaseSpaceVector> >::const_iterator track_residual
    = residuals.begin();
  while (track_residual != residuals.end()) {
    std::cout << "DEBUG MapCppPolynomialMapResiduals::WriteResiduals: "
              << "writing " << track_residual->size() << "residual track points."
              << std::endl;
    GlobalDS::Track * track = new GlobalDS::Track();
    track->set_mapper_name(kClassname);
    std::vector<PhaseSpaceVector>::const_iterator residual
      = track_residual->begin();
    std::vector<long>::const_iterator z_position = z_positions.begin();
    while (residual != track_residual->end()) {
      GlobalDS::TrackPoint track_point
        = Recon::DataStructureHelper::GetInstance().PhaseSpaceVector2TrackPoint(
            *residual, static_cast<double>(*z_position), GlobalDS::kMuMinus);
      track->AddTrackPoint(new GlobalDS::TrackPoint(track_point));
      ++z_position;
      ++residual;
    }
    GlobalEvent * global_event = new GlobalEvent();
    global_event->add_track_recursive(track);
    (*recon_event)->SetGlobalEvent(global_event);
    ++recon_event;
    ++track_residual;
  }
  return ProcessableObject<bool>(new bool(true));
}

bool MapCppPolynomialMapResiduals::death() {
  return true;  // successful
}

const std::string MapCppPolynomialMapResiduals::kClassname
  = "MapCppPolynomialMapResiduals";

}  // namespace MAUS

