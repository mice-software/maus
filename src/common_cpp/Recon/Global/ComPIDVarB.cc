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
#include <utility>
#include "Recon/Global/ComPIDVarB.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string ComPIDVarB::VARIABLE = "KLChargeProdvsDiffTOF1TOF2";

  ComPIDVarB::ComPIDVarB(std::string hypothesis, std::string unique_identifier)
    : PIDBase2D(VARIABLE, hypothesis, unique_identifier, XminBinComB,
		XmaxBinComB, XnumBins, YminBinComB, YmaxBinComB, YnumBins) {
    _nonZeroHistEntries = true;
  }

  ComPIDVarB::ComPIDVarB(TFile* file, std::string hypothesis, int XminComB,
			 int XmaxComB, int YminComB, int YmaxComB)
    : PIDBase2D(file, VARIABLE, hypothesis, XminComB, XmaxComB, YminComB,
		YmaxComB, XminBinComB, XmaxBinComB, YminBinComB, YmaxBinComB) {
  }

  ComPIDVarB::~ComPIDVarB() {}

  std::pair<double, double> ComPIDVarB::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    int total_ADC_charge_product = 0;
    MAUS::DataStructure::Global::DetectorPoint KL_DP =
      MAUS::DataStructure::Global::kCalorimeter;
    double TOF1_t = 0;
    double TOF2_t = 0;
    MAUS::DataStructure::Global::DetectorPoint TOF1_DP =
      MAUS::DataStructure::Global::kTOF1;
    MAUS::DataStructure::Global::DetectorPoint TOF2_DP =
      MAUS::DataStructure::Global::kTOF2;
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
	} else if ((*eachTP)->get_detector() == TOF1_DP) {
	  tof1_track_points.push_back(*eachTP);
	} else if ((*eachTP)->get_detector() == TOF2_DP) {
	  tof2_track_points.push_back(*eachTP);
	} else {
	  continue;
	}
      }
    }
    if (kl_track_points.size() < 1) {
      Squeak::mout(Squeak::debug) << "Global track contained no KL" <<
	" trackpoints, Recon::Global::ComPIDVarB::Calc_Var()" << std::endl;
      kl_track_points.clear();
      tof2_track_points.clear();
      tof1_track_points.clear();
      return std::make_pair(-1, 0);
    } else if (tof2_track_points.size() > 1 || tof1_track_points.size() > 1) {
      Squeak::mout(Squeak::debug) << "Multiple measurements for TOF2/TOF1" <<
	" times, Recon::Global::ComPIDVarB::Calc_Var()" << std::endl;
      tof2_track_points.clear();
      tof1_track_points.clear();
      kl_track_points.clear();
      return std::make_pair(-1, 0);
    } else if ( tof2_track_points.size() == 0 ||
		tof1_track_points.size() == 0 ) {
      Squeak::mout(Squeak::debug) << "Missing measurements for " <<
	"TOF2/TOF1 times, Recon::Global::ComPIDVarB::Calc_Var()" << std::endl;
      tof2_track_points.clear();
      tof1_track_points.clear();
      kl_track_points.clear();
      return std::make_pair(-1, 0);
    } else {
      for (size_t i = 0; i < kl_track_points.size(); i++) {
	total_ADC_charge_product += kl_track_points[i]->get_ADC_charge_product();
      }
      kl_track_points.clear();
      if ( YminBinComB > (total_ADC_charge_product) || (total_ADC_charge_product) > YmaxBinComB ) {
	Squeak::mout(Squeak::debug) << "KL ADC charge product " <<
	  "outside of allowed range, Recon::Global::ComPIDVarB::Calc_Var()" <<
	  std::endl;
      }
      TOF2_t = (tof2_track_points[0])->get_position().T();
      tof2_track_points.clear();
      TOF1_t = (tof1_track_points[0])->get_position().T();
      tof1_track_points.clear();
      if ( XminBinComB > (TOF2_t - TOF1_t) || (TOF2_t - TOF1_t) > XmaxBinComB ) {
	Squeak::mout(Squeak::debug) << "Difference between TOF2 and TOF1 " <<
	  "times out of PDF range, Recon::Global::ComPIDVarB::Calc_Var()" <<
	  std::endl;
      }
      return std::make_pair((TOF2_t - TOF1_t), (total_ADC_charge_product));
    }
  }
}
}
}

