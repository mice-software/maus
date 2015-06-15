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

#include "src/common_cpp/Recon/Global/GlobalTools.hh"

namespace MAUS {
namespace GlobalTools {

std::map<MAUS::DataStructure::Global::DetectorPoint, bool>
    GetMCDetectors(MAUS::GlobalEvent* global_event) {
  std::map<MAUS::DataStructure::Global::DetectorPoint, bool> mc_detectors;
  for (int i = 0; i < 27; i++) {
    mc_detectors[static_cast<MAUS::DataStructure::Global::DetectorPoint>(i)] =
      false;
  }
  MAUS::DataStructure::Global::TrackPArray* imported_tracks =
    global_event->get_tracks();

  MAUS::DataStructure::Global::TrackPArray::iterator imported_track_iter;
  for (imported_track_iter = imported_tracks->begin();
       imported_track_iter != imported_tracks->end();
       ++imported_track_iter) {
    std::vector<MAUS::DataStructure::Global::DetectorPoint> track_detectors =
        (*imported_track_iter)->GetDetectorPoints();
    for (size_t i = 0; i < track_detectors.size(); i++) {
      mc_detectors[track_detectors[i]] = true;
    }
  }

  std::vector<MAUS::DataStructure::Global::SpacePoint*>*
      imported_spacepoints = global_event->get_space_points();
  std::vector<MAUS::DataStructure::Global::SpacePoint*>::iterator sp_iter;
  for (sp_iter = imported_spacepoints->begin();
       sp_iter != imported_spacepoints->end();
       ++sp_iter) {
    mc_detectors[(*sp_iter)->get_detector()] = true;
  }
  return mc_detectors;
}

std::vector<MAUS::DataStructure::Global::Track*>* GetSpillDetectorTracks(
    MAUS::Spill* spill, MAUS::DataStructure::Global::DetectorPoint detector,
    std::string mapper_name) {
  std::vector<MAUS::DataStructure::Global::Track*>* detector_tracks = new
      std::vector<MAUS::DataStructure::Global::Track*>;
  ReconEventPArray* recon_events = spill->GetReconEvents();
  if (recon_events) {
    ReconEventPArray::iterator recon_event_iter;
    for (recon_event_iter = recon_events->begin();
         recon_event_iter != recon_events->end();
         ++recon_event_iter) {
      GlobalEvent* global_event = (*recon_event_iter)->GetGlobalEvent();
      if (global_event) {
        std::vector<MAUS::DataStructure::Global::Track*>* global_tracks =
            global_event->get_tracks();
        std::vector<MAUS::DataStructure::Global::Track*>::iterator track_iter;
        for (track_iter = global_tracks->begin();
             track_iter != global_tracks->end();
             ++track_iter) {
          // The third condition is a bit of a dirty hack here to make sure that
          // if we select for EMR tracks, we only get primaries.
          if (((*track_iter)->HasDetector(detector)) and
              ((*track_iter)->get_mapper_name() == mapper_name) and
              ((*track_iter)->get_emr_range_secondary() < 0.001)) {
            detector_tracks->push_back((*track_iter));
          }
        }
      }
    }
  }
  return detector_tracks;
}

std::vector<MAUS::DataStructure::Global::Track*>* GetTracksByMapperName(
    MAUS::GlobalEvent* global_event,
    std::string mapper_name) {
  std::vector<MAUS::DataStructure::Global::Track*>* global_tracks =
          global_event->get_tracks();
  std::vector<MAUS::DataStructure::Global::Track*>* selected_tracks = new
      std::vector<MAUS::DataStructure::Global::Track*>;
  std::vector<MAUS::DataStructure::Global::Track*>::iterator
      global_track_iter;
  for (global_track_iter = global_tracks->begin();
       global_track_iter != global_tracks->end();
       ++global_track_iter) {
    if ((*global_track_iter)->get_mapper_name() == mapper_name) {
      selected_tracks->push_back(*global_track_iter);
    }
  }
  return selected_tracks;
}

std::vector<MAUS::DataStructure::Global::Track*>* GetTracksByMapperName(
    MAUS::GlobalEvent* global_event,
    std::string mapper_name,
    MAUS::DataStructure::Global::PID pid) {
  std::vector<MAUS::DataStructure::Global::Track*>* global_tracks =
          global_event->get_tracks();
  std::vector<MAUS::DataStructure::Global::Track*>* selected_tracks = new
      std::vector<MAUS::DataStructure::Global::Track*>;
  std::vector<MAUS::DataStructure::Global::Track*>::iterator
      global_track_iter;
  for (global_track_iter = global_tracks->begin();
       global_track_iter != global_tracks->end();
       ++global_track_iter) {
    if ((*global_track_iter)->get_mapper_name() == mapper_name) {
      if ((*global_track_iter)->get_pid() == pid) {
        selected_tracks->push_back(*global_track_iter);
      }
    }
  }
  return selected_tracks;
}

std::vector<int> GetTrackerPlane(const MAUS::DataStructure::Global::TrackPoint*
    track_point) {
  std::vector<int> tracker_plane (3,0);
  double z = track_point->get_position().Z();
  if (approx(z, 11755, 555)) {
    tracker_plane[0] = 0;
    if (approx(z, 11206, 5)) {
      tracker_plane[1] = 5;
      if (approx(z, 11205.7, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 11206.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 11207.0, 0.2)) {
        tracker_plane[2] = 0;
      }
    } else if (approx(z, 11556, 5)) {
      tracker_plane[1] = 4;
      if (approx(z, 11555.6, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 11556.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 11556.9, 0.2)) {
        tracker_plane[2] = 0;
      }
    } else if (approx(z, 11856, 5)) {
      tracker_plane[1] = 3;
      if (approx(z, 11855.6, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 11856.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 11856.9, 0.2)) {
        tracker_plane[2] = 0;
      }
    } else if (approx(z, 12106, 5)) {
      tracker_plane[1] = 2;
      if (approx(z, 12105.5, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 12106.1, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 12106.8, 0.2)) {
        tracker_plane[2] = 0;
      }
    } else if (approx(z, 12306, 5)) {
      tracker_plane[1] = 1;
      if (approx(z, 12305.5, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 12306.1, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 12306.8, 0.2)) {
        tracker_plane[2] = 0;
      }
    }
  } else if (approx(z, 16573, 555)) {
    tracker_plane[0] = 1;
    if (approx(z, 16022, 5)) {
      tracker_plane[1] = 1;
      if (approx(z, 16021.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 16022.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 16022.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    } else if (approx(z, 16222, 5)) {
      tracker_plane[1] = 2;
      if (approx(z, 16221.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 16222.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 16222.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    } else if (approx(z, 16472, 5)) {
      tracker_plane[1] = 3;
      if (approx(z, 16471.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 16472.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 16472.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    } else if (approx(z, 16772, 5)) {
      tracker_plane[1] = 4;
      if (approx(z, 16771.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 16772.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 16772.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    } else if (approx(z, 17122, 5)) {
      tracker_plane[1] = 5;
      if (approx(z, 17121.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 17122.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 17122.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    }
  }
  return tracker_plane;
}

std::vector<MAUS::DataStructure::Global::SpacePoint*>* GetSpillSpacePoints(
    Spill* spill, MAUS::DataStructure::Global::DetectorPoint detector) {
  std::vector<MAUS::DataStructure::Global::SpacePoint*>* spill_spacepoints =
      new std::vector<MAUS::DataStructure::Global::SpacePoint*>;
  MAUS::ReconEventPArray* recon_events = spill->GetReconEvents();
  MAUS::ReconEventPArray::iterator recon_event_iter;
  if (recon_events) {
    for (recon_event_iter = recon_events->begin();
         recon_event_iter != recon_events->end();
         ++recon_event_iter) {
      MAUS::GlobalEvent* global_event = (*recon_event_iter)->GetGlobalEvent();
      if (global_event) {
        std::vector<MAUS::DataStructure::Global::SpacePoint*>*
            spacepoints = global_event->get_space_points();
        std::vector<MAUS::DataStructure::Global::SpacePoint*>::iterator sp_iter;
        for (sp_iter = spacepoints->begin(); sp_iter != spacepoints->end();
             ++sp_iter) {
          if ((*sp_iter)->get_detector() == detector) {
            spill_spacepoints->push_back(*sp_iter);
          }
        }
      }
    }
  }
  if (spill_spacepoints->size() > 0) {
    return spill_spacepoints;
  } else {
    return 0;
  }
}

bool approx(double a, double b, double tolerance) {
  if (std::abs(a - b) > std::abs(tolerance)) {
    return false;
  } else {
    return true;
  }
}

} // ~namespace GlobalTools
} // ~namespace MAUS
