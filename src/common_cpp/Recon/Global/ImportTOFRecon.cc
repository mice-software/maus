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

    char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
    if (!pMAUS_ROOT_DIR) {
      throw MAUS::Exception(Exception::recoverable,
          std::string("Could not find the $MAUS_ROOT_DIR env variable. ")+\
          std::string("Did you try running: source env.sh?"),
          "MapCppGlobalPID::_birth");
    }

    MiceModule* geo_module = new MiceModule(geo_filename);
    std::vector<const MiceModule*> tof_modules = geo_module->findModulesByPropertyString("Detector", "TOF");

    //if legacy 1 or 2
    std::string stage1_legacy = std::string(pMAUS_ROOT_DIR) + "/src/legacy/FILES/Models/Configurations/Stage1.dat";
    std::string stage2_legacy = std::string(pMAUS_ROOT_DIR) + "/src/legacy/FILES/Models/Configurations/Stage2.dat";

    if (geo_filename == stage1_legacy || geo_filename == stage2_legacy) {
      const MiceModule* tof2 = tof_modules[0];
      ImportTOF2SpacePoints(tofEventSpacepoint, global_event, mapper_name, tof2);
    } else {
      if (tof_modules.size() != 3) {
  throw MAUS::Exception(Exception::recoverable,
            std::string("Didn't find all TOFs in geometry. ")+\
            std::string("Skipping TOF importing to global event."),
            "ImportTOFRecon::ImportTOFSpacePoints");
      } else {      
  const MiceModule* tof0 = tof_modules[0];
  const MiceModule* tof1 = tof_modules[1];
  const MiceModule* tof2 = tof_modules[2];

  for (int n = 0; n < 3; n++) {
    switch (n) {
    case 0:
      ImportTOF0SpacePoints(tofEventSpacepoint, global_event, mapper_name, tof0);
      break;

    case 1:
      ImportTOF1SpacePoints(tofEventSpacepoint, global_event, mapper_name, tof1);
      break;

    case 2:
      ImportTOF2SpacePoints(tofEventSpacepoint, global_event, mapper_name, tof2);
      break;
    }
  }
      }
    }
    delete geo_module;
  }

  void ImportTOFRecon::ImportTOF0SpacePoints(
               const MAUS::TOFEventSpacePoint tofEventSpacepoint,
               MAUS::GlobalEvent* global_event,
               std::string mapper_name,
               const MiceModule* tof0_geom) {
    const MiceModule* tof0_vert = tof0_geom->daughter(0);
    const MiceModule* tof0_horiz = tof0_geom->daughter(1);
    double t;
    double x;
    double y;

    size_t max_i = tofEventSpacepoint.GetTOF0SpacePointArraySize();
    std::vector<TOFSpacePoint> tofarray = tofEventSpacepoint.GetTOF0SpacePointArray();
    double z = tof0_geom->globalPosition().getZ();
    double x_err = 20.0;
    double y_err = 20.0;
    MAUS::DataStructure::Global::DetectorPoint detector = MAUS::DataStructure::Global::kTOF0;
    for (unsigned int i = 0; i < max_i; ++i) {
      int x_slab = tofarray[i].GetSlaby();
      int y_slab = tofarray[i].GetSlabx();
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
  }

  void ImportTOFRecon::ImportTOF1SpacePoints(
               const MAUS::TOFEventSpacePoint tofEventSpacepoint,
               MAUS::GlobalEvent* global_event,
               std::string mapper_name,
               const MiceModule* tof1_geom) {
    const MiceModule* tof1_vert = tof1_geom->daughter(0);
    const MiceModule* tof1_horiz = tof1_geom->daughter(1);
    double t;
    double x;
    double y;
    size_t max_i = tofEventSpacepoint.GetTOF1SpacePointArraySize();
    std::vector<TOFSpacePoint> tofarray = tofEventSpacepoint.GetTOF1SpacePointArray();
    double z = tof1_geom->globalPosition().getZ();
    double x_err = 30.0;
    double y_err = 30.0;
    MAUS::DataStructure::Global::DetectorPoint detector = MAUS::DataStructure::Global::kTOF1;
    for (unsigned int i = 0; i < max_i; ++i) {
      int x_slab = tofarray[i].GetSlaby();
      int y_slab = tofarray[i].GetSlabx();
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
  }

  void ImportTOFRecon::ImportTOF2SpacePoints(
               const MAUS::TOFEventSpacePoint tofEventSpacepoint,
               MAUS::GlobalEvent* global_event,
               std::string mapper_name,
               const MiceModule* tof2_geom) {
    const MiceModule* tof2_vert = tof2_geom->daughter(0);
    const MiceModule* tof2_horiz = tof2_geom->daughter(1);
    double t;
    double x;
    double y;
    size_t max_i = tofEventSpacepoint.GetTOF2SpacePointArraySize();
    std::vector<TOFSpacePoint> tofarray = tofEventSpacepoint.GetTOF2SpacePointArray();
    double z = tof2_geom->globalPosition().getZ();
    double x_err = 30.0;
    double y_err = 30.0;
    MAUS::DataStructure::Global::DetectorPoint detector = MAUS::DataStructure::Global::kTOF2;
    for (unsigned int i = 0; i < max_i; ++i) {
      int x_slab = tofarray[i].GetSlaby();
      int y_slab = tofarray[i].GetSlabx();
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
  }
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
