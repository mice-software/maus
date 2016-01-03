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
#include "Recon/Global/ComPIDVarA.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string ComPIDVarA::VARIABLE = "diffTOF1TOF2";

  ComPIDVarA::ComPIDVarA(std::string hypothesis, std::string unique_identifier)
    : PIDBase1D(VARIABLE, hypothesis, unique_identifier, minBinComA, maxBinComA,
                numBins) {
    _nonZeroHistEntries = true;
  }

  ComPIDVarA::ComPIDVarA(TFile* file, std::string hypothesis, int minComA,
			 int maxComA)
    : PIDBase1D(file, VARIABLE, hypothesis, minComA, maxComA, minBinComA,
		maxBinComA) {
  }

  ComPIDVarA::~ComPIDVarA() {}

  std::pair<double, double> ComPIDVarA::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    double TOF2_t = 0;
    double TOF1_t = 0;
    int checkCount2 = 0;
    int checkCount1 = 0;
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
      track_points = track->GetTrackPoints();
    MAUS::DataStructure::Global::DetectorPoint TOF2_DP =
      MAUS::DataStructure::Global::kTOF2;
    MAUS::DataStructure::Global::DetectorPoint TOF1_DP =
      MAUS::DataStructure::Global::kTOF1;
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
      ::iterator eachTP;
    for (eachTP = track_points.begin(); eachTP != track_points.end();
	 ++eachTP) {
      if (!(*eachTP)) continue;
      if ((*eachTP)->get_mapper_name() == "MapCppGlobalTrackMatching-Through") {
	if ((*eachTP)->get_detector() == TOF2_DP) {
	  TOF2_t = (*eachTP)->get_position().T();
	  ++checkCount2;
	} else if ((*eachTP)->get_detector() == TOF1_DP) {
	  TOF1_t = (*eachTP)->get_position().T();
	  ++checkCount1;
	}
      } else {
	continue;
      }
    }
    if (checkCount2 > 1 || checkCount1 > 1) {
      Squeak::mout(Squeak::debug) << "Multiple measurements for TOF2/TOF1" <<
	" times, Recon::Global::ComPIDVarA::Calc_Var()" << std::endl;
      return std::make_pair(-1, 0);
    } else if (checkCount2 == 0 ||
	       checkCount1 == 0 || (TOF2_t -TOF1_t) <= 0) {
      Squeak::mout(Squeak::debug) << "Missing/invalid measurements for " <<
	"TOF0/TOF1 times, Recon::Global::ComPIDVarA::Calc_Var()" << std::endl;
      return std::make_pair(-1, 0);
    } else if ( minBinComA > (TOF2_t - TOF1_t) ||
              (TOF2_t - TOF1_t) > maxBinComA ) {
      Squeak::mout(Squeak::debug) << "Difference between TOF1 & TOF2 times" <<
	" out of PDF range, Recon::Global::ComPIDVarA::Calc_Var()" << std::endl;
      return std::make_pair(-1, 0);
    } else {
      return std::make_pair((TOF2_t - TOF1_t), 0);
    }
  }
}
}
}

