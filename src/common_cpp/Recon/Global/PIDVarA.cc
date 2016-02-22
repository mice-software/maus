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
#include "Recon/Global/PIDVarA.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string PIDVarA::VARIABLE = "diffTOF1TOF0";

  PIDVarA::PIDVarA(std::string hypothesis, std::string unique_identifier)
    : PIDBase1D(VARIABLE, hypothesis, unique_identifier, minBinA, maxBinA,
                numBins) {
    _nonZeroHistEntries = true;
  }

  PIDVarA::PIDVarA(TFile* file, std::string hypothesis, int minA, int maxA)
    : PIDBase1D(file, VARIABLE, hypothesis, minA, maxA, minBinA, maxBinA) {
  }

  PIDVarA::~PIDVarA() {}

  std::pair<double, double> PIDVarA::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    double TOF0_t = 0;
    double TOF1_t = 0;
    int checkCount0 = 0;
    int checkCount1 = 0;
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
      track_points = track->GetTrackPoints();
    // TODO(Pidcott) check that right detector enums are used
    MAUS::DataStructure::Global::DetectorPoint TOF0_DP =
      MAUS::DataStructure::Global::kTOF0;
    MAUS::DataStructure::Global::DetectorPoint TOF1_DP =
      MAUS::DataStructure::Global::kTOF1;
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
      ::iterator eachTP;
    for (eachTP = track_points.begin(); eachTP != track_points.end();
	 ++eachTP) {
      if (!(*eachTP)) continue;
      if ((*eachTP)->get_detector() == TOF0_DP) {
	TOF0_t = (*eachTP)->get_position().T();
	++checkCount0;
      } else if ((*eachTP)->get_detector() == TOF1_DP) {
	TOF1_t = (*eachTP)->get_position().T();
	++checkCount1;
      }
    }
    if (checkCount0 > 1 || checkCount1 > 1) {
      Squeak::mout(Squeak::debug) << "Multiple measurements for TOF0/TOF1" <<
	" times, Recon::Global::PIDVarA::Calc_Var()" << std::endl;
      return std::make_pair(-1, 0);
    } else if (checkCount0 == 0 ||
	       checkCount1 == 0 || (TOF1_t -TOF0_t) <= 0) {
      Squeak::mout(Squeak::debug) << "Missing/invalid measurements for " <<
	"TOF0/TOF1 times, Recon::Global::PIDVarA::Calc_Var()" << std::endl;
      return std::make_pair(-1, 0);
    } else if ( minBinA > (TOF1_t - TOF0_t) || (TOF1_t - TOF0_t) > maxBinA ) {
      Squeak::mout(Squeak::debug) << "Difference between TOF0 and TOF1 times" <<
	" outside of range, Recon::Global::PIDVarA::Calc_Var()" << std::endl;
      return std::make_pair((TOF1_t - TOF0_t), 0);
    } else {
      return std::make_pair((TOF1_t - TOF0_t), 0);
    }
  }
}
}
}

