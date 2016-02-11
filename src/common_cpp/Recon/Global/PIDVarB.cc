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
#include "Recon/Global/PIDVarB.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string PIDVarB::VARIABLE = "diffTOF0TOF1vsTrackerMom";

  PIDVarB::PIDVarB(std::string hypothesis, std::string unique_identifier)
    : PIDBase2D(VARIABLE, hypothesis, unique_identifier,
                 XminBinB, XmaxBinB, XnumBins, YminBinB, YmaxBinB, YnumBins) {
    _nonZeroHistEntries = true;
  }

  PIDVarB::PIDVarB(TFile* file, std::string hypothesis, int XminB, int XmaxB,
		   int YminB, int YmaxB)
    : PIDBase2D(file, VARIABLE, hypothesis, XminB, XmaxB, YminB, YmaxB,
		XminBinB, XmaxBinB, YminBinB, YmaxBinB) {
  }

  PIDVarB::~PIDVarB() {}

  std::pair<double, double> PIDVarB::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    double TOF0_t = 0;
    double TOF1_t = 0;
    MAUS::DataStructure::Global::DetectorPoint TOF0_DP =
      MAUS::DataStructure::Global::kTOF0;
    MAUS::DataStructure::Global::DetectorPoint TOF1_DP =
      MAUS::DataStructure::Global::kTOF1;
    // Get trackpoint array from track
    global_track_points = track->GetTrackPoints();
    std::vector<const MAUS::DataStructure::Global::TrackPoint*>
      ::iterator eachTP;
    for (eachTP = global_track_points.begin();
	 eachTP != global_track_points.end(); ++eachTP) {
      if (!(*eachTP)) continue;
      if ((*eachTP)->get_mapper_name() == "MapCppGlobalTrackMatching-Through") {
	if ((*eachTP)->get_detector() == TOF0_DP) {
	  tof0_track_points.push_back(*eachTP);
	} else if ((*eachTP)->get_detector() == TOF1_DP) {
	  tof1_track_points.push_back(*eachTP);
	} else if ((*eachTP)->get_detector() >=
		   MAUS::DataStructure::Global::kTracker0_1 &&
		   (*eachTP)->get_detector() <=
		   MAUS::DataStructure::Global::kTracker0_5) {
	  tracker0_track_points.push_back(*eachTP);
	} else {
	  continue;
	}
      }
    }
    if (tof0_track_points.size() > 1 || tof1_track_points.size() > 1) {
      Squeak::mout(Squeak::debug) << "Multiple measurements for TOF0/TOF1" <<
	" times, Recon::Global::PIDVarB::Calc_Var()" << std::endl;
      tof0_track_points.clear();
      tof1_track_points.clear();
      return std::make_pair(-1, 0);
    } else if ( tof0_track_points.size() == 0 ||
		tof1_track_points.size() == 0 ) {
      Squeak::mout(Squeak::debug) << "Missing measurements for " <<
	"TOF0/TOF1 times, Recon::Global::PIDVarB::Calc_Var()" << std::endl;
      tof0_track_points.clear();
      tof1_track_points.clear();
      return std::make_pair(-1, 0);
    } else if (tracker0_track_points.size() < 1) {
      Squeak::mout(Squeak::debug) << "Global track contained no scifi" <<
	" trackpoints, Recon::Global::PIDVarB::Calc_Var()" << std::endl;
      tracker0_track_points.clear();
      return std::make_pair(-1, 0);
    } else {
      TOF0_t = (tof0_track_points[0])->get_position().T();
      tof0_track_points.clear();
      TOF1_t = (tof1_track_points[0])->get_position().T();
      tof1_track_points.clear();
      if ( YminBinB > (TOF1_t - TOF0_t) || (TOF1_t - TOF0_t) > YmaxBinB ) {
	Squeak::mout(Squeak::debug) << "Difference between TOF0 and TOF1 " <<
	  "times outside of range, Recon::Global::PIDVarB::Calc_Var()" <<
	  std::endl;
      }
      double tracker0_trackpoint_mom = 0;
      int tracker0_tp_count = 0;
      double tracker0_momentum = 0;
      const MAUS::DataStructure::Global::TrackPoint* tracker0_trackpoint;
      if (tracker0_track_points.size() < 1) {
      }
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
      if ( XminBinB > tracker0_momentum || tracker0_momentum > XmaxBinB ) {
	Squeak::mout(Squeak::debug) << "Momentum for tracker 0 is outside " <<
	  "of range, Recon::Global::PIDVarB::Calc_Var()" << std::endl;
      }
      return std::make_pair(tracker0_momentum, (TOF1_t - TOF0_t));
    }
  }
}
}
}
