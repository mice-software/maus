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

#include <cmath>

#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/Utils/Exception.hh"

#include "src/common_cpp/Recon/Global/MCTruthTools.hh"

namespace MAUS {
namespace MCTruthTools {

std::map<DataStructure::Global::DetectorPoint, bool>
    GetMCDetectors(MCEvent* mc_event) {
  std::map<DataStructure::Global::DetectorPoint, bool> mc_detectors;
  for (int i = 0; i < 27; i++) {
    mc_detectors[static_cast<DataStructure::Global::DetectorPoint>(i)] = false;
  }

  // TOFS
  TOFHitArray* tof_hits = mc_event->GetTOFHits();
  if (tof_hits) {
    for (auto tof_hits_iter = tof_hits->begin();
         tof_hits_iter != tof_hits->end();
         ++tof_hits_iter) {
      int station_number = tof_hits_iter->GetChannelId()->GetStation();
      int plane_number = tof_hits_iter->GetChannelId()->GetPlane();
      if (station_number == 0) {
        mc_detectors[DataStructure::Global::kTOF0] = true;
        if (plane_number == 0) {
          mc_detectors[DataStructure::Global::kTOF0_1] = true;
        } else if (plane_number == 1) {
          mc_detectors[DataStructure::Global::kTOF0_2] = true;
        }
      } else if (station_number == 1) {
        mc_detectors[DataStructure::Global::kTOF1] = true;
        if (plane_number == 0) {
          mc_detectors[DataStructure::Global::kTOF1_1] = true;
        } else if (plane_number == 1) {
          mc_detectors[DataStructure::Global::kTOF1_2] = true;
        }
      } else if (station_number == 2) {
        mc_detectors[DataStructure::Global::kTOF2] = true;
        if (plane_number == 0) {
          mc_detectors[DataStructure::Global::kTOF2_1] = true;
        } else if (plane_number == 1) {
          mc_detectors[DataStructure::Global::kTOF2_2] = true;
        }
      }
    }
  }

  // Trackers
  SciFiHitArray* scifi_hits = mc_event->GetSciFiHits();
  if (scifi_hits) {
    for (auto scifi_hits_iter = scifi_hits->begin();
         scifi_hits_iter != scifi_hits->end();
         ++scifi_hits_iter) {
      int tracker_number = scifi_hits_iter->GetChannelId()->GetTrackerNumber();
      int station_number = scifi_hits_iter->GetChannelId()->GetStationNumber();
      // Since they are all consecutive we can make things easier with some casting
      int detector_enum_int = 10 + tracker_number*6 + station_number;
      if (tracker_number == 0) {
        mc_detectors[DataStructure::Global::kTracker0] = true;
      } else if (tracker_number == 1) {
        mc_detectors[DataStructure::Global::kTracker1] = true;
      }
      mc_detectors[static_cast<DataStructure::Global::DetectorPoint>
          (detector_enum_int)] = true;
    }
  }

  // KL
  KLHitArray* kl_hits = mc_event->GetKLHits();
  if (kl_hits) {
    if (kl_hits->size() > 0) {
      mc_detectors[DataStructure::Global::kCalorimeter] = true;
    }
  }

  // EMR
  EMRHitArray* emr_hits = mc_event->GetEMRHits();
  if (emr_hits) {
    if (emr_hits->size() > 0) {
      mc_detectors[DataStructure::Global::kEMR] = true;
    }
  }

  return mc_detectors;
}

TOFHitArray* GetTOFHits(MCEvent* mc_event,
    DataStructure::Global::DetectorPoint detector) {
  if (not (detector == DataStructure::Global::kTOF0 or
           detector == DataStructure::Global::kTOF0_1 or
           detector == DataStructure::Global::kTOF0_2 or
           detector == DataStructure::Global::kTOF1 or
           detector == DataStructure::Global::kTOF1_1 or
           detector == DataStructure::Global::kTOF1_2 or
           detector == DataStructure::Global::kTOF2 or
           detector == DataStructure::Global::kTOF2_1 or
           detector == DataStructure::Global::kTOF2_2)) {
    throw(Exception(Exception::nonRecoverable,
        "Detector Enum not a TOF", "MAUS::MCTruthTools::GetTOFHits()"));
  }
  TOFHitArray* tof_hits = mc_event->GetTOFHits();
  TOFHitArray* selected_tof_hits = new TOFHitArray;
  if (tof_hits) {
    for (auto tof_hits_iter = tof_hits->begin();
         tof_hits_iter != tof_hits->end();
         ++tof_hits_iter) {
      int station_number = tof_hits_iter->GetChannelId()->GetStation();
      int plane_number = tof_hits_iter->GetChannelId()->GetPlane();
      if (detector == DataStructure::Global::kTOF0) {
        if (station_number == 0) {
          selected_tof_hits->push_back(*tof_hits_iter);
        }
      } else if (detector == DataStructure::Global::kTOF0_1) {
        if (station_number == 0 and plane_number == 0) {
          selected_tof_hits->push_back(*tof_hits_iter);
        }
      } else if (detector == DataStructure::Global::kTOF0_2) {
        if (station_number == 0 and plane_number == 1) {
          selected_tof_hits->push_back(*tof_hits_iter);
        }
      } else if (detector == DataStructure::Global::kTOF1) {
        if (station_number == 1) {
          selected_tof_hits->push_back(*tof_hits_iter);
        }
      } else if (detector == DataStructure::Global::kTOF1_1) {
        if (station_number == 1 and plane_number == 0) {
          selected_tof_hits->push_back(*tof_hits_iter);
        }
      } else if (detector == DataStructure::Global::kTOF1_2) {
        if (station_number == 1 and plane_number == 1) {
          selected_tof_hits->push_back(*tof_hits_iter);
        }
      } else if (detector == DataStructure::Global::kTOF2) {
        if (station_number == 2) {
          selected_tof_hits->push_back(*tof_hits_iter);
        }
      } else if (detector == DataStructure::Global::kTOF2_1) {
        if (station_number == 2 and plane_number == 0) {
          selected_tof_hits->push_back(*tof_hits_iter);
        }
      } else if (detector == DataStructure::Global::kTOF2_2) {
        if (station_number == 2 and plane_number == 1) {
          selected_tof_hits->push_back(*tof_hits_iter);
        }
      }
    }
  }
  return selected_tof_hits;
}

SciFiHitArray* GetTrackerHits(MCEvent* mc_event,
    DataStructure::Global::DetectorPoint detector) {
  if (not (detector == DataStructure::Global::kTracker0 or
           detector == DataStructure::Global::kTracker0_1 or
           detector == DataStructure::Global::kTracker0_2 or
           detector == DataStructure::Global::kTracker0_3 or
           detector == DataStructure::Global::kTracker0_4 or
           detector == DataStructure::Global::kTracker0_5 or
           detector == DataStructure::Global::kTracker1 or
           detector == DataStructure::Global::kTracker1_1 or
           detector == DataStructure::Global::kTracker1_2 or
           detector == DataStructure::Global::kTracker1_3 or
           detector == DataStructure::Global::kTracker1_4 or
           detector == DataStructure::Global::kTracker1_5)) {
    throw(Exception(Exception::nonRecoverable,
        "Detector Enum not a Tracker", "MAUS::MCTruthTools::GetTrackerHits()"));
  }
  SciFiHitArray* tracker_hits = mc_event->GetSciFiHits();
  SciFiHitArray* selected_tracker_hits = new SciFiHitArray;
  if (tracker_hits) {
    for (auto tracker_hits_iter = tracker_hits->begin();
         tracker_hits_iter != tracker_hits->end();
         ++tracker_hits_iter) {
      int tracker_number = tracker_hits_iter->GetChannelId()->GetTrackerNumber();
      int station_number = tracker_hits_iter->GetChannelId()->GetStationNumber();
      if (detector == DataStructure::Global::kTracker0) {
        if (tracker_number == 0) {
          selected_tracker_hits->push_back(*tracker_hits_iter);
        }
      } else if (detector == DataStructure::Global::kTracker1) {
        if (tracker_number == 1) {
          selected_tracker_hits->push_back(*tracker_hits_iter);
        }
      } else {
        int detector_enum_int = 10 + tracker_number*6 + station_number;
        if (static_cast<int>(detector) == detector_enum_int) {
          selected_tracker_hits->push_back(*tracker_hits_iter);
        }
      }
    }
  }
  return selected_tracker_hits;
}

SciFiHit* GetTrackerPlaneHit(MCEvent* mc_event,
                             int tracker, int station, int plane) {
  SciFiHitArray* tracker_hits = mc_event->GetSciFiHits();
  if (tracker_hits) {
    for (auto tracker_hits_iter = tracker_hits->begin();
         tracker_hits_iter != tracker_hits->end();
         ++tracker_hits_iter) {
      int tracker_number = tracker_hits_iter->GetChannelId()->GetTrackerNumber();
      int station_number = tracker_hits_iter->GetChannelId()->GetStationNumber();
      int plane_number = tracker_hits_iter->GetChannelId()->GetPlaneNumber();
      if ((tracker_number == tracker) and (station_number == station) and
          (plane_number == plane)) {
        return &(*tracker_hits_iter);
      }
    }
  }
  return 0;
}

TOFHit GetNearestZHit(TOFHitArray* hits, TLorentzVector position) {
  // Find the closest (by z) hit in the corresponding mc event
  size_t nearest_index = 0;
  double z_distance = 1e20;
  for (size_t i = 0; i < hits->size(); i++) {
    if (std::abs(position.Z() - hits->at(i).GetPosition().Z())
        < z_distance) {
      nearest_index = i;
      z_distance =
          std::abs(position.Z() - hits->at(i).GetPosition().Z());
    }
  }
  return hits->at(nearest_index);
}

KLHit GetNearestZHit(KLHitArray* hits, TLorentzVector position) {
  // Find the closest (by z) hit in the corresponding mc event
  size_t nearest_index = 0;
  double z_distance = 1e20;
  for (size_t i = 0; i < hits->size(); i++) {
    if (std::abs(position.Z() - hits->at(i).GetPosition().Z())
        < z_distance) {
      nearest_index = i;
      z_distance =
          std::abs(position.Z() - hits->at(i).GetPosition().Z());
    }
  }
  return hits->at(nearest_index);
}

EMRHit GetNearestZHit(EMRHitArray* hits, TLorentzVector position) {
  // Find the closest (by z) hit in the corresponding mc event
  size_t nearest_index = 0;
  double z_distance = 1e20;
  for (size_t i = 0; i < hits->size(); i++) {
    if (std::abs(position.Z() - hits->at(i).GetPosition().Z())
        < z_distance) {
      nearest_index = i;
      z_distance =
          std::abs(position.Z() - hits->at(i).GetPosition().Z());
    }
  }
  return hits->at(nearest_index);
}

} // ~namespace MCTruthTools
} // ~namespace MAUS
