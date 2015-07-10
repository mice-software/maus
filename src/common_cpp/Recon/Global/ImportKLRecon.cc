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

#include "Recon/Global/ImportKLRecon.hh"

namespace MAUS {
namespace recon {
namespace global {

  void ImportKLRecon::process(const MAUS::KLEvent &kl_event,
			       MAUS::GlobalEvent* global_event,
			       std::string mapper_name) {

    KLEventCellHit KLEventCellHit = kl_event.GetKLEventCellHit();

    for (size_t i = 0; i < KLEventCellHit.GetKLCellHitArraySize(); ++i) {
      KLCellHit KLCellHit = KLEventCellHit.GetKLCellHitArrayElement(i);
      MAUS::DataStructure::Global::SpacePoint* GlobalKLSpacePoint =
	new MAUS::DataStructure::Global::SpacePoint();
      GlobalKLSpacePoint->set_ADC_charge(KLCellHit.GetCharge());
      GlobalKLSpacePoint->set_ADC_charge_product(KLCellHit.GetChargeProduct());
      GlobalKLSpacePoint->set_detector(MAUS::DataStructure::Global::kCalorimeter);
      double x = KLCellHit.GetGlobalPosX();
      double x_err = KLCellHit.GetErrorX();
      double y = KLCellHit.GetGlobalPosY();
      double y_err = KLCellHit.GetErrorY();
      double z = KLCellHit.GetGlobalPosZ();
      double z_err = KLCellHit.GetErrorZ();
      double t = -1000000;
      double t_err = 1000000;
      TLorentzVector pos(x,y,z,t);
      TLorentzVector pos_err(x_err, y_err, z_err, t_err);
      GlobalKLSpacePoint->set_position(pos);
      GlobalKLSpacePoint->set_position_error(pos_err);
      global_event->add_space_point(GlobalKLSpacePoint);
    }
  }

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
