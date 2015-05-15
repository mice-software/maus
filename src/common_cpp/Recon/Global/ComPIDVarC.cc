/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

#include <string>
#include <vector>
#include "Recon/Global/ComPIDVarC.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string ComPIDVarC::VARIABLE = "KLADCChargeProduct";

  ComPIDVarC::ComPIDVarC(std::string hypothesis, std::string unique_identifier)
    : PIDBase1D(VARIABLE, hypothesis, unique_identifier, minBinComC, maxBinComC,
                numBins) {
    _nonZeroHistEntries = true;
  }

  ComPIDVarC::ComPIDVarC(TFile* file, std::string hypothesis, int minComC, int maxComC)
    : PIDBase1D(file, VARIABLE, hypothesis, minComC, maxComC) {
  }

  ComPIDVarC::~ComPIDVarC() {}

  std::pair<double, double> ComPIDVarC::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    int total_ADC_charge_product = 0;
    MAUS::DataStructure::Global::DetectorPoint KL_DP =
      MAUS::DataStructure::Global::kCalorimeter;
    // Get trackpoint array from track
    global_track_points = track->GetTrackPoints();
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
      ::iterator eachTP;
    for (eachTP = global_track_points.begin();
	 eachTP != global_track_points.end(); ++eachTP) {
      if (!(*eachTP)) continue;
      if ((*eachTP)->get_mapper_name() == "MapCppGlobalTrackMatching") {
	if ((*eachTP)->get_detector() == KL_DP) {
	  kl_track_points.push_back(*eachTP);
	} else {
	  continue;
	}
      }
    }
    if (kl_track_points.size() < 1) {
      Squeak::mout(Squeak::debug) << "Global track contained no KL" <<
	" trackpoints, Recon::Global::ComPIDVarC::Calc_Var()" << std::endl;
      kl_track_points.clear();
      return std::make_pair(-1, 0);
    } else {
      for (size_t i = 0; i < kl_track_points.size(); i++) {
	total_ADC_charge_product += kl_track_points[i]->get_ADC_charge_product();
      }
      kl_track_points.clear();
      if ( minBinComC > (total_ADC_charge_product) || (total_ADC_charge_product) > maxBinComC ) {
	Squeak::mout(Squeak::debug) << "KL ADC charge product " <<
	  "outside of PDF range, Recon::Global::ComPIDVarC::Calc_Var()" <<
	  std::endl;
	return std::make_pair(total_ADC_charge_product, 0);
      } else {
	return std::make_pair(total_ADC_charge_product, 0);
      }
    }
  }
}
}
}


