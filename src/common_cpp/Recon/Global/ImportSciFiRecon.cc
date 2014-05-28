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

#include <algorithm>

#include "Interface/Squeak.hh"

#include "Recon/Global/ImportSciFiRecon.hh"

namespace MAUS {
namespace recon {
namespace global {

  void ImportSciFiRecon::process(const MAUS::SciFiEvent &scifi_event,
				 MAUS::GlobalEvent* global_event,
				 std::string mapper_name) {

    // Get array of scifi tracks
    SciFiTrackPArray scifi_track_array = scifi_event.scifitracks();
    // For each track, make a global track
    for (size_t i = 0; i < scifi_track_array.size(); i++) {
      SciFiTrack* scifi_track = scifi_track_array[i];
      MAUS::DataStructure::Global::Track* GlobalSciFiTrack =
	new MAUS::DataStructure::Global::Track();
      ImportSciFiTrack(scifi_track, GlobalSciFiTrack, mapper_name);
      GlobalSciFiTrack->set_mapper_name(mapper_name);
      global_event->add_track_recursive(GlobalSciFiTrack);
    }
  }


  // Loop through trackpoints, make global tp and add to global track
  void ImportSciFiRecon::ImportSciFiTrack(const MAUS::SciFiTrack* scifi_track,
					  MAUS::DataStructure::Global::Track*
					  GlobalSciFiTrack,
					  std::string mapper_name) {

    int charge = scifi_track->charge();
    // Get trackpoint array
    SciFiTrackPointPArray scifi_tp_array = scifi_track->scifitrackpoints();
    // Loop through trackpoints
    for (size_t i = 0; i < scifi_tp_array.size(); i++) {
      SciFiTrackPoint* scifi_tp = scifi_tp_array[i];
      MAUS::DataStructure::Global::TrackPoint* GlobalSciFiTrackPoint =
	new MAUS::DataStructure::Global::TrackPoint();
      MAUS::DataStructure::Global::SpacePoint* GlobalSciFiSpacePoint =
	new MAUS::DataStructure::Global::SpacePoint();
      int tracker = scifi_tp->tracker();
      int station = scifi_tp->station();
      // currently, z values are not returned by scifi trackpoints, so set
      // z position as position of station in Stage4.dat, currently hard
      // coded into the SetStationEnum function
      double z = 0;
      SetStationEnum(GlobalSciFiTrackPoint, GlobalSciFiTrack, tracker,
		     station, z);
      double x = scifi_tp->x();
      double y = scifi_tp->y();
      // double z = scifi_tp->z();
      // time not provided by tracker, set to unphysical value
      double t = -1000000.0;
      TLorentzVector pos(x, y, z, t);
      double px = scifi_tp->px();
      double py = scifi_tp->py();
      double pz = scifi_tp->pz();
      // Energy unknown, set to unphysical value
      double E = -1000000.0;
      TLorentzVector mom(px, py, pz, E);
      GlobalSciFiSpacePoint->set_charge(static_cast<double>(charge));
      GlobalSciFiSpacePoint->set_position(pos);
      GlobalSciFiSpacePoint->set_detector(GlobalSciFiTrackPoint->get_detector());
      GlobalSciFiTrackPoint->set_space_point(GlobalSciFiSpacePoint);
      GlobalSciFiTrackPoint->set_charge(static_cast<double>(charge));
      GlobalSciFiTrackPoint->set_position(pos);
      GlobalSciFiTrackPoint->set_momentum(mom);
      GlobalSciFiTrackPoint->set_mapper_name(mapper_name);

      GlobalSciFiTrack->AddTrackPoint(GlobalSciFiTrackPoint);
    }
  }

  void ImportSciFiRecon::SetStationEnum(
    MAUS::DataStructure::Global::TrackPoint* GlobalSciFiTrackPoint,
    MAUS::DataStructure::Global::Track* GlobalSciFiTrack, int tracker,
    int station, double& z) {

    MAUS::DataStructure::Global::DetectorPoint detector =
      MAUS::DataStructure::Global::kUndefined;
    if (tracker == 0) {
      switch (station) {
      case 1:
	detector = MAUS::DataStructure::Global::kTracker0_1;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
	z = 12303.1189;
	break;
      case 2:
	detector = MAUS::DataStructure::Global::kTracker0_2;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
	z = 12106.1543;
	break;
      case 3:
	detector = MAUS::DataStructure::Global::kTracker0_3;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
	z = 11856.2913;
	break;
      case 4:
	detector = MAUS::DataStructure::Global::kTracker0_4;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
	z = 11556.291;
	break;
      case 5:
	detector = MAUS::DataStructure::Global::kTracker0_5;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
	z = 11206.3611;
	break;
      }
    } else if (tracker == 1) {
      switch (station) {
      case 1:
	detector = MAUS::DataStructure::Global::kTracker1_1;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
	z = 16022.24;
	break;
      case 2:
	detector = MAUS::DataStructure::Global::kTracker1_2;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
	z = 16222.25;
	break;
      case 3:
	detector = MAUS::DataStructure::Global::kTracker1_3;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
	z = 16472.24;
	break;
      case 4:
	detector = MAUS::DataStructure::Global::kTracker1_4;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
	z = 16772.24;
	break;
      case 5:
	detector = MAUS::DataStructure::Global::kTracker1_5;
	GlobalSciFiTrackPoint->set_detector(detector);
	GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
	z = 17122.24;
	break;
      }
    }
  }

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
