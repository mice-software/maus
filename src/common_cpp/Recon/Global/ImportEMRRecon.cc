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

#include "Recon/Global/ImportEMRRecon.hh"

namespace MAUS {
namespace recon {
namespace global {

  void ImportEMRRecon::process(const MAUS::EMREvent &emr_event,
             MAUS::GlobalEvent* global_event,
             std::string mapper_name) {

    std::cerr << "We made it into the import" << std::endl;

    double z_ref = 18556.38;
    double x_ref = -500.0;
    double y_ref = -500.0;

    double x;
    double y;
    double z;
    double t = -1000000;
    TLorentzVector pos_err(8.5, 8.5, 8.5, 0.0);

    // Plane hit array
    MAUS::EMRPlaneHitArray plane_hit_array = emr_event.GetEMRPlaneHitArray();
    if (plane_hit_array.size() > 0) {
    MAUS::EMRPlaneHitArray::iterator plane_hit_iter;
    for (plane_hit_iter = plane_hit_array.begin();
         plane_hit_iter != plane_hit_array.end();
         ++plane_hit_iter) {
      // Plane hit (sets z)
      MAUS::EMRPlaneHit* plane_hit = (*plane_hit_iter);
      // Bar array
      MAUS::EMRBarArray bar_array =  plane_hit->GetEMRBarArray();
      MAUS::EMRBarArray::iterator bar_iter;
      for (bar_iter = bar_array.begin();
           bar_iter != bar_array.end();
           ++bar_iter) {
        MAUS::EMRBar* emr_bar = (*bar_iter);
        // Bar hit array
        MAUS::EMRBarHitArray bar_hit_array =  emr_bar->GetEMRBarHitArray();
        MAUS::EMRBarHitArray::iterator bar_hit_iter;
        for (bar_hit_iter = bar_hit_array.begin();
             bar_hit_iter != bar_hit_array.end();
             ++bar_hit_iter) {
          // Bar hit (get x and y)
          MAUS::EMRBarHit bar_hit = (*bar_hit_iter);
          // According to FD, x is the bar number in x, y is the average of bar numbers
          // based on papers and posters, the EMR coord system appears to switch x and y
          // with the MICE coordinate system.
          x = (bar_hit.GetX() + 0.5)*17 + x_ref;
          y = (bar_hit.GetY() + 0.5)*17 + y_ref;
          z = (bar_hit.GetZ() + 0.5)*17 + z_ref;
          TLorentzVector pos(x, y, z, t);
          std::cerr << "before creating the spacepoint" << std::endl;
          MAUS::DataStructure::Global::SpacePoint* spoint =
            new MAUS::DataStructure::Global::SpacePoint();
          spoint->set_detector(MAUS::DataStructure::Global::kEMR);
          std::cerr << "detector : " << spoint->get_detector() << std::endl;
          spoint->set_position(pos);
          std::cerr << "x pos : " << spoint->get_position().X() << std::endl;
          std::cerr << "y pos : " << spoint->get_position().Y() << std::endl;
          std::cerr << "z pos : " << spoint->get_position().Z() << std::endl;
          spoint->set_position_error(pos_err);
          std::cerr << "before adding the spacepoint to the global event" << std::endl;

          global_event->add_space_point(spoint);
          std::cerr << "after adding the spacepoint to the global event" << std::endl;
        }
      }
    }
  }
  }
} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
