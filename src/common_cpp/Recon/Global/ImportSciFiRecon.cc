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

#include "Utils/Squeak.hh"

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
      GlobalSciFiTrack->set_p_value(scifi_track->P_value());
      GlobalSciFiTrack->set_tracker_clusters(abs(scifi_track->GetNumberDataPoints()));
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
      SetStationEnum(GlobalSciFiTrackPoint, GlobalSciFiTrack, tracker, station);
      ThreeVector pos_errors = scifi_tp->pos_error();
      ThreeVector mom_errors = scifi_tp->mom_error();
      double x = scifi_tp->pos().x();
      double x_err = pos_errors.x();
      double y = scifi_tp->pos().y();
      double y_err = pos_errors.y();
      double z = scifi_tp->pos().z();
      double z_err = 0.6273;
      // time not provided by tracker, set to unphysical value
      double t = -1000000.0;
      double t_err = 1000000.0;
      TLorentzVector pos(x, y, z, t);
      TLorentzVector pos_err(x_err, y_err, z_err, t_err);
      double px = scifi_tp->mom().x();
      double px_err = mom_errors.x();
      double py = scifi_tp->mom().y();
      double py_err = mom_errors.y();
      double pz = scifi_tp->mom().z();
      double pz_err = mom_errors.z();
      // Energy unknown, set to unphysical value
      double E = -1000000.0;
      double E_err = 1000000.0;
      TLorentzVector mom(px, py, pz, E);
      TLorentzVector mom_err(px_err, py_err, pz_err, E_err);
      GlobalSciFiSpacePoint->set_charge(static_cast<double>(charge));
      GlobalSciFiSpacePoint->set_position(pos);
      GlobalSciFiSpacePoint->set_position_error(pos_err);
      GlobalSciFiSpacePoint->set_detector(GlobalSciFiTrackPoint->get_detector());
      GlobalSciFiTrackPoint->set_space_point(GlobalSciFiSpacePoint);
      GlobalSciFiTrackPoint->set_charge(static_cast<double>(charge));
      GlobalSciFiTrackPoint->set_position(pos);
      GlobalSciFiTrackPoint->set_position_error(pos_err);
      GlobalSciFiTrackPoint->set_momentum(mom);
      GlobalSciFiTrackPoint->set_momentum_error(mom_err);
      GlobalSciFiTrackPoint->set_mapper_name(mapper_name);

      GlobalSciFiTrack->AddTrackPoint(GlobalSciFiTrackPoint);
    }
  }

  void ImportSciFiRecon::SetStationEnum(
    MAUS::DataStructure::Global::TrackPoint* GlobalSciFiTrackPoint,
    MAUS::DataStructure::Global::Track* GlobalSciFiTrack, int tracker,
    int station) {

    MAUS::DataStructure::Global::DetectorPoint detector =
      MAUS::DataStructure::Global::kUndefined;
    if (tracker == 0) {
      switch (station) {
      case 1:
        detector = MAUS::DataStructure::Global::kTracker0_1;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
        break;
      case 2:
        detector = MAUS::DataStructure::Global::kTracker0_2;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
        break;
      case 3:
        detector = MAUS::DataStructure::Global::kTracker0_3;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
        break;
      case 4:
        detector = MAUS::DataStructure::Global::kTracker0_4;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
        break;
      case 5:
        detector = MAUS::DataStructure::Global::kTracker0_5;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker0);
        break;
      }
    } else if (tracker == 1) {
      switch (station) {
      case 1:
        detector = MAUS::DataStructure::Global::kTracker1_1;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
        break;
      case 2:
        detector = MAUS::DataStructure::Global::kTracker1_2;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
        break;
      case 3:
        detector = MAUS::DataStructure::Global::kTracker1_3;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
        break;
      case 4:
        detector = MAUS::DataStructure::Global::kTracker1_4;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
        break;
      case 5:
        detector = MAUS::DataStructure::Global::kTracker1_5;
        GlobalSciFiTrackPoint->set_detector(detector);
        GlobalSciFiTrack->SetDetector(MAUS::DataStructure::Global::kTracker1);
        break;
      }
    }
  }

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
