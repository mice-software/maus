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
#include "Recon/Global/ComPIDVarI.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string ComPIDVarI::VARIABLE =
    "CkovBvsDiffTOF1TOF2";

  ComPIDVarI::ComPIDVarI(std::string hypothesis, std::string unique_identifier)
    : PIDBase2D(VARIABLE, hypothesis, unique_identifier, XminBinComI,
		XmaxBinComI, XnumBins, YminBinComI, YmaxBinComI, YnumBins) {
    _nonZeroHistEntries = true;
  }

  ComPIDVarI::ComPIDVarI(TFile* file, std::string hypothesis, int XminComI,
			 int XmaxComI, int YminComI, int YmaxComI)
    : PIDBase2D(file, VARIABLE, hypothesis, XminComI, XmaxComI, YminComI,
		YmaxComI, XminBinComI, XmaxBinComI, YminBinComI, YmaxBinComI) {
  }

  ComPIDVarI::~ComPIDVarI() {}

  std::pair<double, double> ComPIDVarI::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    CkovB_pes = 0;
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
    MAUS::DataStructure::Global::DetectorPoint CKOVB_DP =
      MAUS::DataStructure::Global::kCherenkovB;
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
      ::iterator eachTP;
    for (eachTP = track_points.begin(); eachTP != track_points.end();
	 ++eachTP) {
      if (!(*eachTP)) continue;
      if ((*eachTP)->get_mapper_name() == "MapCppGlobalTrackMatching-Through") {
	if ((*eachTP)->get_detector() == CKOVB_DP) {
	  CkovB_pes = (*eachTP)->get_num_photoelectrons();
	} else if ((*eachTP)->get_detector() == TOF2_DP) {
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
    if (CkovB_pes == 0) {
      Squeak::mout(Squeak::debug) << "Global track was passed an Ckov event " <<
	" with no PES in CkovB, " <<
	"Recon::Global::ComPIDVarI::Calc_Var()" << std::endl;
      return std::make_pair(0, -1);
    } else if ( YminBinComI > CkovB_pes || CkovB_pes > YmaxBinComI ) {
      Squeak::mout(Squeak::debug) << "# of PES in CkovB " <<
	"outside of PDF range, Recon::Global::ComPIDVarI::Calc_Var()" <<
	std::endl;
      return std::make_pair(0, -1);
    }
    if (checkCount2 > 1 || checkCount1 > 1) {
      Squeak::mout(Squeak::debug) << "Multiple measurements for TOF2/TOF1" <<
	" times, Recon::Global::ComPIDVarI::Calc_Var()" << std::endl;
      return std::make_pair(-1, CkovB_pes);
    } else if (checkCount2 == 0 ||
	       checkCount1 == 0 || (TOF2_t -TOF1_t) <= 0) {
      Squeak::mout(Squeak::debug) << "Missing/invalid measurements for " <<
	"TOF0/TOF1 times, Recon::Global::ComPIDVarI::Calc_Var()" << std::endl;
      return std::make_pair(-1, CkovB_pes);
    } else if ( XminBinComI > (TOF2_t - TOF1_t) ||
		(TOF2_t - TOF1_t) > XmaxBinComI ) {
      Squeak::mout(Squeak::debug) << "Difference between TOF1 & TOF2 times" <<
	" out of PDF range, Recon::Global::ComPIDVarI::Calc_Var()" << std::endl;
      return std::make_pair(-1, CkovB_pes);
    } else {
      return std::make_pair((TOF2_t - TOF1_t), CkovB_pes);
    }
  }
}
}
}


