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
			       std::string mapper_name,
			       std::string geo_filename) {

    MAUS::TOFEventSpacePoint tofEventSpacepoint =
      tof_event.GetTOFEventSpacePoint();

    ImportTOFSpacePoints(tofEventSpacepoint, global_event, mapper_name,
			 geo_filename);
  }

  void ImportTOFRecon::ImportTOFSpacePoints(
					    const MAUS::TOFEventSpacePoint tofEventSpacepoint,
					    MAUS::GlobalEvent* global_event,
					    std::string mapper_name,
					    std::string geo_filename) {

    MiceModule* geo_module = new MiceModule(geo_filename);
    std::vector<const MiceModule*> tof_modules = geo_module->findModulesByPropertyString("Detector", "TOF");

    // This importer is now set up to work with the CAD geometries
    // from the CDB. It will seg fault with the legacy geometries.
    const MiceModule* tof0 = tof_modules[0];
    const MiceModule* tof0_vert = tof0->daughter(0);
    const MiceModule* tof0_horiz = tof0->daughter(1);
    const MiceModule* tof1 = tof_modules[1];
    const MiceModule* tof1_vert = tof1->daughter(0);
    const MiceModule* tof1_horiz = tof1->daughter(1);
    const MiceModule* tof2 = tof_modules[2];
    const MiceModule* tof2_vert = tof2->daughter(0);
    const MiceModule* tof2_horiz = tof2->daughter(1);

    size_t max_i;
    std::vector<TOFSpacePoint> tofarray;
    double t;
    double z;
    double x;
    double x_err;
    double y;
    double y_err;
    MAUS::DataStructure::Global::DetectorPoint detector;
    int x_slab;
    int y_slab;

    for (int n = 0; n < 3; n++) {
      switch (n) {
      case 0:
        max_i = tofEventSpacepoint.GetTOF0SpacePointArraySize();
        tofarray = tofEventSpacepoint.GetTOF0SpacePointArray();
        z = tof0->globalPosition().getZ();
        x_err = 20.0;
        y_err = 20.0;
        detector = MAUS::DataStructure::Global::kTOF0;
	for (unsigned int i = 0; i < max_i; ++i) {
	  x_slab = tofarray[i].GetSlaby();
	  y_slab = tofarray[i].GetSlabx();
	  switch (x_slab) {
	  case 0:
	    x = tof0_vert->daughter(0)->globalPosition().getX();
	    break;
	  case 1:
	    x = tof0_vert->daughter(1)->globalPosition().getX();
	    break;
	  case 2:
	    x = tof0_vert->daughter(2)->globalPosition().getX();
	    break;
	  case 3:
	    x = tof0_vert->daughter(3)->globalPosition().getX();
	    break;
	  case 4:
	    x = tof0_vert->daughter(4)->globalPosition().getX();
	    break;
	  case 5:
	    x = tof0_vert->daughter(5)->globalPosition().getX();
	    break;
	  case 6:
	    x = tof0_vert->daughter(6)->globalPosition().getX();
	    break;
	  case 7:
	    x = tof0_vert->daughter(7)->globalPosition().getX();
	    break;
	  case 8:
	    x = tof0_vert->daughter(8)->globalPosition().getX();
	    break;
	  case 9:
	    x = tof0_vert->daughter(9)->globalPosition().getX();
	    break;
	  }
	  switch (y_slab) {
	  case 0:
	    y = tof0_horiz->daughter(0)->globalPosition().getY();
	    break;
	  case 1:
	    y = tof0_horiz->daughter(1)->globalPosition().getY();
	    break;
	  case 2:
	    y = tof0_horiz->daughter(2)->globalPosition().getY();
	    break;
	  case 3:
	    y = tof0_horiz->daughter(3)->globalPosition().getY();
	    break;
	  case 4:
	    y = tof0_horiz->daughter(4)->globalPosition().getY();
	    break;
	  case 5:
	    y = tof0_horiz->daughter(5)->globalPosition().getY();
	    break;
	  case 6:
	    y = tof0_horiz->daughter(6)->globalPosition().getY();
	    break;
	  case 7:
	    y = tof0_horiz->daughter(7)->globalPosition().getY();
	    break;
	  case 8:
	    y = tof0_horiz->daughter(8)->globalPosition().getY();
	    break;
	  case 9:
	    y = tof0_horiz->daughter(9)->globalPosition().getY();
	    break;
	  }
	  t = tofarray[i].GetTime();
	  MAUS::DataStructure::Global::SpacePoint* spoint =
	    new MAUS::DataStructure::Global::SpacePoint();

	  TLorentzVector pos(x, y, z, t);

	  // z position is given as the centre of the TOF, so set error to half the
	  // TOF depth i.e. the depth of a single bar
	  double z_err = 2.5;
	  double t_err = tofarray[i].GetDt();
	  TLorentzVector pos_err(x_err, y_err, z_err, t_err);

	  spoint->set_detector(detector);
	  spoint->set_position(pos);
	  spoint->set_position_error(pos_err);

	  global_event->add_space_point(spoint);
	}
        break;

      case 1:
        max_i = tofEventSpacepoint.GetTOF1SpacePointArraySize();
        tofarray = tofEventSpacepoint.GetTOF1SpacePointArray();
        z = tof1->globalPosition().getZ();
        x_err = 30.0;
        y_err = 30.0;
        detector = MAUS::DataStructure::Global::kTOF1;
	for (unsigned int i = 0; i < max_i; ++i) {
	  x_slab = tofarray[i].GetSlaby();
	  y_slab = tofarray[i].GetSlabx();
	  switch (x_slab) {
	  case 0:
	    x = tof1_vert->daughter(0)->globalPosition().getX();
	    break;
	  case 1:
	    x = tof1_vert->daughter(1)->globalPosition().getX();
	    break;
	  case 2:
	    x = tof1_vert->daughter(2)->globalPosition().getX();
	    break;
	  case 3:
	    x = tof1_vert->daughter(3)->globalPosition().getX();
	    break;
	  case 4:
	    x = tof1_vert->daughter(4)->globalPosition().getX();
	    break;
	  case 5:
	    x = tof1_vert->daughter(5)->globalPosition().getX();
	    break;
	  case 6:
	    x = tof1_vert->daughter(6)->globalPosition().getX();
	    break;
	  }
	  switch (y_slab) {
	  case 0:
	    y = tof1_horiz->daughter(0)->globalPosition().getY();
	    break;
	  case 1:
	    y = tof1_horiz->daughter(1)->globalPosition().getY();
	    break;
	  case 2:
	    y = tof1_horiz->daughter(2)->globalPosition().getY();
	    break;
	  case 3:
	    y = tof1_horiz->daughter(3)->globalPosition().getY();
	    break;
	  case 4:
	    y = tof1_horiz->daughter(4)->globalPosition().getY();
	    break;
	  case 5:
	    y = tof1_horiz->daughter(5)->globalPosition().getY();
	    break;
	  case 6:
	    y = tof1_horiz->daughter(6)->globalPosition().getY();
	    break;
	  }
	  t = tofarray[i].GetTime();
	  MAUS::DataStructure::Global::SpacePoint* spoint =
	    new MAUS::DataStructure::Global::SpacePoint();

	  TLorentzVector pos(x, y, z, t);

	  // z position is given as the centre of the TOF, so set error to half the
	  // TOF depth i.e. the depth of a single bar
	  double z_err = 2.5;
	  double t_err = tofarray[i].GetDt();
	  TLorentzVector pos_err(x_err, y_err, z_err, t_err);

	  spoint->set_detector(detector);
	  spoint->set_position(pos);
	  spoint->set_position_error(pos_err);

	  global_event->add_space_point(spoint);
	}
        break;

      case 2:
        max_i = tofEventSpacepoint.GetTOF2SpacePointArraySize();
        tofarray = tofEventSpacepoint.GetTOF2SpacePointArray();
        z = tof2->globalPosition().getZ();
        x_err = 30.0;
        y_err = 30.0;
        detector = MAUS::DataStructure::Global::kTOF2;
	for (unsigned int i = 0; i < max_i; ++i) {
	  x_slab = tofarray[i].GetSlaby();
	  y_slab = tofarray[i].GetSlabx();
	  switch (x_slab) {
	  case 0:
	    x = tof2_vert->daughter(0)->globalPosition().getX();
	    break;
	  case 1:
	    x = tof2_vert->daughter(1)->globalPosition().getX();
	    break;
	  case 2:
	    x = tof2_vert->daughter(2)->globalPosition().getX();
	    break;
	  case 3:
	    x = tof2_vert->daughter(3)->globalPosition().getX();
	    break;
	  case 4:
	    x = tof2_vert->daughter(4)->globalPosition().getX();
	    break;
	  case 5:
	    x = tof2_vert->daughter(5)->globalPosition().getX();
	    break;
	  case 6:
	    x = tof2_vert->daughter(6)->globalPosition().getX();
	    break;
	  case 7:
	    x = tof2_vert->daughter(7)->globalPosition().getX();
	    break;
	  case 8:
	    x = tof2_vert->daughter(8)->globalPosition().getX();
	    break;
	  case 9:
	    x = tof2_vert->daughter(9)->globalPosition().getX();
	    break;
	  }
	  switch (y_slab) {
	  case 0:
	    y = tof2_horiz->daughter(0)->globalPosition().getY();
	    break;
	  case 1:
	    y = tof2_horiz->daughter(1)->globalPosition().getY();
	    break;
	  case 2:
	    y = tof2_horiz->daughter(2)->globalPosition().getY();
	    break;
	  case 3:
	    y = tof2_horiz->daughter(3)->globalPosition().getY();
	    break;
	  case 4:
	    y = tof2_horiz->daughter(4)->globalPosition().getY();
	    break;
	  case 5:
	    y = tof2_horiz->daughter(5)->globalPosition().getY();
	    break;
	  case 6:
	    y = tof2_horiz->daughter(6)->globalPosition().getY();
	    break;
	  case 7:
	    y = tof2_horiz->daughter(7)->globalPosition().getY();
	    break;
	  case 8:
	    y = tof2_horiz->daughter(8)->globalPosition().getY();
	    break;
	  case 9:
	    y = tof2_horiz->daughter(9)->globalPosition().getY();
	    break;
	  }
	  t = tofarray[i].GetTime();
	  MAUS::DataStructure::Global::SpacePoint* spoint =
	    new MAUS::DataStructure::Global::SpacePoint();

	  TLorentzVector pos(x, y, z, t);

	  // z position is given as the centre of the TOF, so set error to half the
	  // TOF depth i.e. the depth of a single bar
	  double z_err = 2.5;
	  double t_err = tofarray[i].GetDt();
	  TLorentzVector pos_err(x_err, y_err, z_err, t_err);

	  spoint->set_detector(detector);
	  spoint->set_position(pos);
	  spoint->set_position_error(pos_err);

	  global_event->add_space_point(spoint);
	}
        break;
      }
    }
  }
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
