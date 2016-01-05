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

#include "Recon/Global/ImportTOFRecon.hh"

namespace MAUS {
namespace recon {
namespace global {

  void ImportTOFRecon::process(const MAUS::TOFEvent &tof_event,
			       MAUS::GlobalEvent* global_event,
			       std::string mapper_name) {

    MAUS::TOFEventSpacePoint tofEventSpacepoint =
      tof_event.GetTOFEventSpacePoint();

    ImportTOFSpacePoints(tofEventSpacepoint, global_event, mapper_name);
  }

  void ImportTOFRecon::ImportTOFSpacePoints(
					    const MAUS::TOFEventSpacePoint tofEventSpacepoint,
					    MAUS::GlobalEvent* global_event,
					    std::string mapper_name) {


    for (int n = 0; n < 3; n++) {
      switch (n) {
      case 0:
	ImportTOF0SpacePoints(tofEventSpacepoint, global_event, mapper_name);
	break;

      case 1:
	ImportTOF1SpacePoints(tofEventSpacepoint, global_event, mapper_name);
	break;

      case 2:
	ImportTOF2SpacePoints(tofEventSpacepoint, global_event, mapper_name);
	break;
      }
    }
  }

  void ImportTOFRecon::ImportTOF0SpacePoints(
					     const MAUS::TOFEventSpacePoint tofEventSpacepoint,
					     MAUS::GlobalEvent* global_event,
					     std::string mapper_name) {

    size_t max_i = tofEventSpacepoint.GetTOF0SpacePointArraySize();
    std::vector<TOFSpacePoint> tofarray = tofEventSpacepoint.GetTOF0SpacePointArray();
    MAUS::DataStructure::Global::DetectorPoint detector = MAUS::DataStructure::Global::kTOF0;
    for (unsigned int i = 0; i < max_i; ++i) {
      double x = tofarray[i].GetGlobalPosX();
      double y = tofarray[i].GetGlobalPosY();
      double z = tofarray[i].GetGlobalPosZ();
      double x_err = tofarray[i].GetGlobalPosXErr();
      double y_err = tofarray[i].GetGlobalPosYErr();
      double z_err = tofarray[i].GetGlobalPosZErr();
      double t = tofarray[i].GetTime();
      double t_err = tofarray[i].GetDt();
      MAUS::DataStructure::Global::SpacePoint* spoint =
	new MAUS::DataStructure::Global::SpacePoint();
      TLorentzVector pos(x, y, z, t);
      TLorentzVector pos_err(x_err, y_err, z_err, t_err);
      spoint->set_detector(detector);
      spoint->set_position(pos);
      spoint->set_position_error(pos_err);

      global_event->add_space_point(spoint);
    }
  }

  void ImportTOFRecon::ImportTOF1SpacePoints(
					     const MAUS::TOFEventSpacePoint tofEventSpacepoint,
					     MAUS::GlobalEvent* global_event,
					     std::string mapper_name) {

    size_t max_i = tofEventSpacepoint.GetTOF1SpacePointArraySize();
    std::vector<TOFSpacePoint> tofarray = tofEventSpacepoint.GetTOF1SpacePointArray();
    MAUS::DataStructure::Global::DetectorPoint detector = MAUS::DataStructure::Global::kTOF1;
    for (unsigned int i = 0; i < max_i; ++i) {
      double x = tofarray[i].GetGlobalPosX();
      double y = tofarray[i].GetGlobalPosY();
      double z = tofarray[i].GetGlobalPosZ();
      double x_err = tofarray[i].GetGlobalPosXErr();
      double y_err = tofarray[i].GetGlobalPosYErr();
      double z_err = tofarray[i].GetGlobalPosZErr();
      double t = tofarray[i].GetTime();
      double t_err = tofarray[i].GetDt();
      MAUS::DataStructure::Global::SpacePoint* spoint =
	new MAUS::DataStructure::Global::SpacePoint();
      TLorentzVector pos(x, y, z, t);
      TLorentzVector pos_err(x_err, y_err, z_err, t_err);
      spoint->set_detector(detector);
      spoint->set_position(pos);
      spoint->set_position_error(pos_err);

      global_event->add_space_point(spoint);
    }
  }

  void ImportTOFRecon::ImportTOF2SpacePoints(
					     const MAUS::TOFEventSpacePoint tofEventSpacepoint,
					     MAUS::GlobalEvent* global_event,
					     std::string mapper_name) {

    size_t max_i = tofEventSpacepoint.GetTOF2SpacePointArraySize();
    std::vector<TOFSpacePoint> tofarray = tofEventSpacepoint.GetTOF2SpacePointArray();
    MAUS::DataStructure::Global::DetectorPoint detector = MAUS::DataStructure::Global::kTOF2;
    for (unsigned int i = 0; i < max_i; ++i) {
      double x = tofarray[i].GetGlobalPosX();
      double y = tofarray[i].GetGlobalPosY();
      double z = tofarray[i].GetGlobalPosZ();
      double x_err = tofarray[i].GetGlobalPosXErr();
      double y_err = tofarray[i].GetGlobalPosYErr();
      double z_err = tofarray[i].GetGlobalPosZErr();
      double t = tofarray[i].GetTime();
      double t_err = tofarray[i].GetDt();
      MAUS::DataStructure::Global::SpacePoint* spoint =
	new MAUS::DataStructure::Global::SpacePoint();
      TLorentzVector pos(x, y, z, t);
      TLorentzVector pos_err(x_err, y_err, z_err, t_err);
      spoint->set_detector(detector);
      spoint->set_position(pos);
      spoint->set_position_error(pos_err);

      global_event->add_space_point(spoint);
    }
  }
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
