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

#include "Recon/Global/ImportKLRecon.hh"

namespace MAUS {
namespace recon {
namespace global {

  void ImportKLRecon::process(const MAUS::KLEvent &kl_event, MAUS::GlobalEvent* global_event,
      std::string mapper_name, bool merge_cell_hits) {

    KLEventCellHit kl_event_cell_hit = kl_event.GetKLEventCellHit();
    std::vector<KLCellHit> kl_cell_hits = kl_event_cell_hit.GetKLCellHitArray();
    if (kl_cell_hits.size() < 1) {
      return;
    }
    std::sort(kl_cell_hits.begin(), kl_cell_hits.end(), KLCellHitSort);
    std::vector<std::vector<size_t> > cell_groupings = AdjacentCellGroupings(kl_cell_hits,
        merge_cell_hits);

    for (size_t i = 0; i < cell_groupings.size(); i++) {
      double chargesum = 0.0, chargeproductsum = 0.0;
      double x = 0.0, x_err = 0.0, y = 0.0, y_err = 0.0, z = 0.0, z_err = 0.0;
      double weighted_y2 = 0.0;
      double t = -1000000, t_err = 1000000;
      for (size_t j = 0; j < cell_groupings.at(i).size(); j++) {
        KLCellHit kl_cell_hit = kl_cell_hits.at(cell_groupings.at(i).at(j));
        double charge = kl_cell_hit.GetCharge();
        double chargeproduct = kl_cell_hit.GetChargeProduct();
        chargesum += charge;
        chargeproductsum += chargeproduct;
        x += kl_cell_hit.GetGlobalPosX()*charge;
        y += kl_cell_hit.GetGlobalPosY()*charge;
        z += kl_cell_hit.GetGlobalPosZ()*charge;
        weighted_y2 += kl_cell_hit.GetGlobalPosY()*kl_cell_hit.GetGlobalPosY()*charge;
        // Are the same for all, so this way we can easily just pick out the last one
        x_err = kl_cell_hit.GetErrorX();
        z_err = kl_cell_hit.GetErrorZ();
        if (cell_groupings.at(i).size() == 1) {
          y_err = kl_cell_hit.GetErrorY();
        }
      }
      x /= chargesum;
      y /= chargesum;
      z /= chargesum;
      if (cell_groupings.at(i).size() != 1) {
        weighted_y2 /= chargesum;
        y_err = std::sqrt(weighted_y2 - y*y);
      }
      TLorentzVector pos(x, y, z, t);
      TLorentzVector pos_err(x_err, y_err, z_err, t_err);
      DataStructure::Global::SpacePoint* global_space_point =
            new DataStructure::Global::SpacePoint();
      global_space_point->set_ADC_charge(chargesum);
      global_space_point->set_ADC_charge_product(chargeproductsum);
      global_space_point->set_detector(MAUS::DataStructure::Global::kCalorimeter);
      global_space_point->set_position(pos);
      global_space_point->set_position_error(pos_err);
      global_event->add_space_point(global_space_point);
    }
  }

  std::vector<std::vector<size_t> > ImportKLRecon::AdjacentCellGroupings(
      std::vector<KLCellHit> kl_cell_hits, bool merge_cell_hits) {
    std::vector<std::vector<size_t> > cell_groupings;
    std::vector<size_t> cell_hit_ids;
    if (kl_cell_hits.size() == 1) {
      cell_hit_ids.push_back(0);
      cell_groupings.push_back(cell_hit_ids);
    } else {
      for (size_t i = 0; i < kl_cell_hits.size() - 1; i++) {
        cell_hit_ids.push_back(i);
        if (kl_cell_hits.at(i+1).GetCell() != kl_cell_hits.at(i).GetCell() + 1
            or !merge_cell_hits) { // If configuration is set not to merge, one std::vector for each
          cell_groupings.push_back(cell_hit_ids);
          cell_hit_ids.clear();
        }
      }
      cell_hit_ids.push_back(kl_cell_hits.size() - 1);
      cell_groupings.push_back(cell_hit_ids);
    }
    return cell_groupings;
  }

  bool ImportKLRecon::KLCellHitSort(KLCellHit hit1, KLCellHit hit2) {
    return (hit1.GetCell() < hit2.GetCell());
  }

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
