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

    size_t max_i;
    std::string local_mapper_name;
    std::vector<TOFSpacePoint> tofarray;
    double z;
    double x_err;
    double y_err;
    MAUS::DataStructure::Global::DetectorPoint detector;

    for (int n = 0; n < 3; n++) {
      switch (n) {
      case 0:
	max_i = tofEventSpacepoint.GetTOF0SpacePointArraySize();
	local_mapper_name = mapper_name + "/ImportTOF0spacepoint";
	tofarray = tofEventSpacepoint.GetTOF0SpacePointArray();
	z = 2773.0;
	x_err = 20.0;
	y_err = 20.0;
	detector = MAUS::DataStructure::Global::kTOF0_1;
	break;

      case 1:
	max_i = tofEventSpacepoint.GetTOF1SpacePointArraySize();
	local_mapper_name = mapper_name + "/ImportTOF1spacepoint";
	tofarray = tofEventSpacepoint.GetTOF1SpacePointArray();
	z = 10478.0;
	x_err = 30.0;
	y_err = 30.0;
	detector = MAUS::DataStructure::Global::kTOF1_1;
	break;

      case 2:
	max_i = tofEventSpacepoint.GetTOF2SpacePointArraySize();
	local_mapper_name = mapper_name + "/ImportTOF2spacepoint";
	tofarray = tofEventSpacepoint.GetTOF2SpacePointArray();
	z = 12883.0;
	x_err = 30.0;
	y_err = 30.0;
	detector = MAUS::DataStructure::Global::kTOF2_1;
	break;
      }

      for (unsigned int i = 0; i < max_i; ++i) {

	MAUS::DataStructure::Global::SpacePoint* spoint =
	  new MAUS::DataStructure::Global::SpacePoint();

	// TODO(Pidcott) need to change slab # to a global position
	double x = tofarray[i].GetSlaby();
	double y = tofarray[i].GetSlabx();
	double t = tofarray[i].GetTime();
	TLorentzVector pos(x, y, z, t);

	// TODO(Pidcott) Not sure what z error should actually be, set as 1 for now
	double z_err = 1.0;
	double t_err = tofarray[i].GetDt();
	TLorentzVector pos_err(x_err, y_err, z_err, t_err);

	spoint->set_detector(detector);
	spoint->set_position(pos);
	spoint->set_position_error(pos_err);

	global_event->add_space_point(spoint);
      }
    }
  }
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
