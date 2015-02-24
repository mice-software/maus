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
      int cell = KLCellHit.GetCell();
      double x = 0.0;
      double x_err = 466.0;
      double y;
      double y_err = 22.0;
      double z = 18052.0;
      double z_err = 20.0;
      double t = -1000000;
      double t_err = 1000000;
      switch (cell) {
      case 0:
	y = -441.5;
	break;
      case 1:
	y = -397.5;
	break;
      case 2:
	y = -353.5;
	break;
      case 3:
	y = -309.0;
	break;
      case 4:
	y = -265.0;
	break;
      case 5:
	y = -221.0;
	break;
      case 6:
	y = -176.5;
	break;
      case 7:
	y = -132.5;
	break;
      case 8:
	y = -88.5;
	break;
      case 9:
	y = -44.0;
	break;
      case 10:
	y = 0.0;
	break;
      case 11:
	y = 44.0;
	break;
      case 12:
	y = 88.5;
	break;
      case 13:
	y = 132.5;
	break;
      case 14:
        y = 176.5;
	break;
      case 15:
	y = 221.0;
	break;
      case 16:
	y = 265.0;
	break;
      case 17:
	y = 309.0;
	break;
      case 18:
	y = 353.5;
	break;
      case 19:
	y = 397.5;
	break;
      case 20:
	y = 441.5;
	break;
      }

      TLorentzVector pos(x,y,z,t);
      TLorentzVector pos_err(x_err, y_err, z_err, t_err);
      GlobalKLSpacePoint->set_position(pos);
      GlobalKLSpacePoint->set_position_error(pos_err);
      GlobalKLSpacePoint->set_ADC_charge(KLCellHit.GetCharge());
      GlobalKLSpacePoint->set_ADC_charge_product(KLCellHit.GetChargeProduct());
      GlobalKLSpacePoint->set_detector(MAUS::DataStructure::Global::kCalorimeter);
      global_event->add_space_point(GlobalKLSpacePoint);
    }
  }

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
