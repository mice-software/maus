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
#include "Recon/Global/ComPIDVarE.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string ComPIDVarE::VARIABLE = "CommissioningEMRrangevsDiffTOF1TOF2";

  ComPIDVarE::ComPIDVarE(std::string hypothesis, std::string unique_identifier)
    : PIDBase2D(VARIABLE, hypothesis, unique_identifier, XminBinComE,
		XmaxBinComE, XnumBins, YminBinComE, YmaxBinComE, YnumBins) {
    _nonZeroHistEntries = true;
  }

  ComPIDVarE::ComPIDVarE(TFile* file, std::string hypothesis, int XminComE,
			 int XmaxComE, int YminComE, int YmaxComE)
    : PIDBase2D(file, VARIABLE, hypothesis, XminComE, XmaxComE, YminComE,
		YmaxComE, XminBinComE, XmaxBinComE, YminBinComE, YmaxBinComE) {
  }

  ComPIDVarE::~ComPIDVarE() {}

  std::pair<double, double> ComPIDVarE::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    EMR_range = track->get_emr_range_primary();
    if (EMR_range == 0.0) {
      Squeak::mout(Squeak::debug) << "Global track records no range " <<
	" of particle in EMR, Recon::Global::ComPIDVarE::Calc_Var()" << std::endl;
      return std::make_pair(0, -1);
    } else if ( YminBinComE > EMR_range || EMR_range > YmaxBinComE ) {
      Squeak::mout(Squeak::debug) << "Range of particle in EMR " <<
	"outside of PDF range, Recon::Global::ComPIDVarE::Calc_Var()" <<
	std::endl;
      return std::make_pair(0, -1);
    } else {
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
	if ((*eachTP)->get_mapper_name() == "MapCppGlobalTrackMatching") {
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
	  " times, Recon::Global::ComPIDVarE::Calc_Var()" << std::endl;
	return std::make_pair(-1, EMR_range);
      } else if (checkCount2 == 0 ||
		 checkCount1 == 0 || (TOF2_t -TOF1_t) <= 0) {
	Squeak::mout(Squeak::debug) << "Missing/invalid measurements for " <<
	  "TOF0/TOF1 times, Recon::Global::ComPIDVarE::Calc_Var()" << std::endl;
	return std::make_pair(-1, EMR_range);
      } else if ( XminBinComE > (TOF2_t - TOF1_t) ||
		  (TOF2_t - TOF1_t) > XmaxBinComE ) {
	Squeak::mout(Squeak::debug) << "Difference between TOF1 & TOF2 times" <<
	  " out of PDF range, Recon::Global::ComPIDVarE::Calc_Var()" << std::endl;
	return std::make_pair(-1, EMR_range);
      } else {
	return std::make_pair((TOF2_t - TOF1_t), EMR_range);
      }
    }
  }
}
}
}


