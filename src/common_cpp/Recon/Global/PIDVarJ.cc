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
#include "Recon/Global/PIDVarJ.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string PIDVarJ::VARIABLE = "CkovBvsUSTrackerMom";

  PIDVarJ::PIDVarJ(std::string hypothesis, std::string unique_identifier)
    : PIDBase2D(VARIABLE, hypothesis, unique_identifier,
		XminBinJ, XmaxBinJ, XnumBins, YminBinJ, YmaxBinJ, YnumBins) {
    _nonZeroHistEntries = true;
  }

  PIDVarJ::PIDVarJ(TFile* file, std::string hypothesis, int XminJ, int XmaxJ,
		   int YminJ, int YmaxJ)
    : PIDBase2D(file, VARIABLE, hypothesis, XminJ, XmaxJ, YminJ, YmaxJ,
		XminBinJ, XmaxBinJ, YminBinJ, YmaxBinJ) {
  }

  PIDVarJ::~PIDVarJ() {}

  std::pair<double, double> PIDVarJ::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    CkovB_pes = 0;
    MAUS::DataStructure::Global::DetectorPoint CKOVB_DP =
      MAUS::DataStructure::Global::kCherenkovB;
    // Get trackpoint array from track
    global_track_points = track->GetTrackPoints();
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
      ::iterator eachTP;
    for (eachTP = global_track_points.begin();
	 eachTP != global_track_points.end(); ++eachTP) {
      if (!(*eachTP)) continue;
	if ((*eachTP)->get_detector() == CKOVB_DP) {
	  CkovB_pes = (*eachTP)->get_num_photoelectrons();
	} else if ((*eachTP)->get_detector() >=
	    MAUS::DataStructure::Global::kTracker0_1 &&
	    (*eachTP)->get_detector() <=
	    MAUS::DataStructure::Global::kTracker0_5) {
	  tracker0_track_points.push_back(*eachTP);
	} else {
	  continue;
	}
    }
    if (CkovB_pes == 0) {
      Squeak::mout(Squeak::debug) << "Global track was passed an Ckov event " <<
	" with no PES in CkovB, " <<
	"Recon::Global::PIDVarJ::Calc_Var()" << std::endl;
      return std::make_pair(0, -1);
    } else if ( YminBinJ > CkovB_pes || CkovB_pes > YmaxBinJ ) {
      Squeak::mout(Squeak::debug) << "# of PES in CkovB " <<
	"outside of PDF range, Recon::Global::PIDVarJ::Calc_Var()" <<
	std::endl;
      return std::make_pair(0, -1);
    }
    if (tracker0_track_points.size() < 1) {
      Squeak::mout(Squeak::debug) << "Global track contained no downstream" <<
	" tracker trackpoints, Recon::Global::PIDVarJ::Calc_Var()" << std::endl;
      tracker0_track_points.clear();
      return std::make_pair(-1, 0);
    } else {
      double tracker0_trackpoint_mom = 0;
      int tracker0_tp_count = 0;
      double tracker0_momentum = 0;
      const MAUS::DataStructure::Global::TrackPoint* tracker0_trackpoint;
      for (size_t i = 0; i < tracker0_track_points.size(); i++) {
	tracker0_trackpoint = tracker0_track_points[i];
	if (tracker0_trackpoint) {
	  TLorentzVector trackpoint_4mom = tracker0_trackpoint->get_momentum();
	  tracker0_trackpoint_mom +=
	    sqrt(trackpoint_4mom.Px()*trackpoint_4mom.Px() +
		 trackpoint_4mom.Py()*trackpoint_4mom.Py() +
		 trackpoint_4mom.Pz()*trackpoint_4mom.Pz());
	  tracker0_tp_count++;
	} else {
	  continue;
	}
      }
      tracker0_track_points.clear();
      tracker0_momentum = tracker0_trackpoint_mom/tracker0_tp_count;
      if ( XminBinJ > tracker0_momentum || tracker0_momentum > XmaxBinJ ) {
	Squeak::mout(Squeak::debug) << "Momentum for tracker 0 is outside " <<
	  "of range, Recon::Global::PIDVarJ::Calc_Var()" << std::endl;
	return std::make_pair(0, -1);
      } else {
	return std::make_pair(tracker0_momentum, CkovB_pes);
      }
    }
  }
}
}
}


