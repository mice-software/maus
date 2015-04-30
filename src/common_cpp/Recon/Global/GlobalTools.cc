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

std::vector<MAUS::DataStructure::Global::Track*>* GetTracksByMapperName(
    MAUS::GlobalEvent* global_event,
    std::string mapper_name,
    MAUS::DataStructure::Global::PID pid =
        MAUS::DataStructure::Global::kTritium) {
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
      if (pid == MAUS::DataStructure::Global::kTritium) {
        selected_tracks->push_back(*global_track_iter);
      } else if ((*global_track_iter)->get_pid() == pid) {
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

bool approx(double a, double b, double tolerance) {
  if (std::abs(a - b) > std::abs(tolerance)) {
    return false;
  } else {
    return true;
  }
}

} // ~namespace GlobalTools
} // ~namespace MAUS
