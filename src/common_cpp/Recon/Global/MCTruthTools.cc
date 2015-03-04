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

#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/DataStructure/Hit.hh"

#include "src/common_cpp/Recon/Global/MCTruthTools.hh"

namespace MAUS {
namespace MCTruthTools {

std::map<MAUS::DataStructure::Global::DetectorPoint, bool>
    GetMCDetectors(MAUS::MCEvent* mc_event) {
  std::map<MAUS::DataStructure::Global::DetectorPoint, bool> mc_detectors;
  for (int i = 0; i < 27; i++) {
    mc_detectors[static_cast<MAUS::DataStructure::Global::DetectorPoint>(i)] =
      false;
  }

  // TOFS
  TOFHitArray* tof_hits = mc_event->GetTOFHits();
  if (tof_hits) {
    TOFHitArray::iterator tof_hits_iter;
    for (tof_hits_iter = tof_hits->begin();
         tof_hits_iter != tof_hits->end();
         ++tof_hits_iter) {
      int station_number = tof_hits_iter->GetChannelId()->GetStation();
      int plane_number = tof_hits_iter->GetChannelId()->GetPlane();
      if (station_number == 0) {
        mc_detectors[MAUS::DataStructure::Global::kTOF0] = true;
        if (plane_number == 0) {
          mc_detectors[MAUS::DataStructure::Global::kTOF0_1] = true;
        } else if (plane_number == 1) {
          mc_detectors[MAUS::DataStructure::Global::kTOF0_2] = true;
        }
      } else if (station_number == 1) {
        mc_detectors[MAUS::DataStructure::Global::kTOF1] = true;
        if (plane_number == 0) {
          mc_detectors[MAUS::DataStructure::Global::kTOF1_1] = true;
        } else if (plane_number == 1) {
          mc_detectors[MAUS::DataStructure::Global::kTOF1_2] = true;
        }
      } else if (station_number == 2) {
        mc_detectors[MAUS::DataStructure::Global::kTOF2] = true;
        if (plane_number == 0) {
          mc_detectors[MAUS::DataStructure::Global::kTOF2_1] = true;
        } else if (plane_number == 1) {
          mc_detectors[MAUS::DataStructure::Global::kTOF2_2] = true;
        }
      }
    }
  }

  // Trackers
  SciFiHitArray* scifi_hits = mc_event->GetSciFiHits();
  if (scifi_hits) {
    SciFiHitArray::iterator scifi_hits_iter;
    for (scifi_hits_iter = scifi_hits->begin();
         scifi_hits_iter != scifi_hits->end();
         ++scifi_hits_iter) {
      int tracker_number = scifi_hits_iter->GetChannelId()->GetTrackerNumber();
      int station_number = scifi_hits_iter->GetChannelId()->GetStationNumber();
      // Since they are all consecutive we can make things easier with some casting
      int detector_enum_int = 10 + tracker_number*6 + station_number;
      if (tracker_number == 0) {
        mc_detectors[MAUS::DataStructure::Global::kTracker0] = true;
      } else if (tracker_number == 1) {
        mc_detectors[MAUS::DataStructure::Global::kTracker1] = true;
      }
      mc_detectors[static_cast<MAUS::DataStructure::Global::DetectorPoint>
          (detector_enum_int)] = true;
    }
  }

  // KL
  KLHitArray* kl_hits = mc_event->GetKLHits();
  if (kl_hits) {
    if (kl_hits->size() > 0) {
      mc_detectors[MAUS::DataStructure::Global::kCalorimeter] = true;
    }
  }

  // EMR
  EMRHitArray* emr_hits = mc_event->GetEMRHits();
  if (emr_hits) {
    if (emr_hits->size() > 0) {
      mc_detectors[MAUS::DataStructure::Global::kEMR] = true;
    }
  }
  
  return mc_detectors;
}

} // ~namespace MCTruthTools
} // ~namespace MAUS
