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
#include "Recon/Global/PIDVarF.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string PIDVarF::VARIABLE = "EMRrangevsDSTrackerMom";

  PIDVarF::PIDVarF(std::string hypothesis, std::string unique_identifier)
    : PIDBase2D(VARIABLE, hypothesis, unique_identifier,
                 XminBinF, XmaxBinF, XnumBins, YminBinF, YmaxBinF, YnumBins) {
    _nonZeroHistEntries = true;
  }

  PIDVarF::PIDVarF(TFile* file, std::string hypothesis, int XminF, int XmaxF,
		   int YminF, int YmaxF)
    : PIDBase2D(file, VARIABLE, hypothesis, XminF, XmaxF, YminF, YmaxF,
		XminBinF, XmaxBinF, YminBinF, YmaxBinF) {
  }

  PIDVarF::~PIDVarF() {}

  std::pair<double, double> PIDVarF::Calc_Var(MAUS::DataStructure::Global::Track* track) {
    EMR_range = track->get_emr_range_primary();
    if (EMR_range == 0.0) {
      Squeak::mout(Squeak::debug) << "Global track records no range " <<
	" of particle in EMR, Recon::Global::PIDVarF::Calc_Var()" << std::endl;
      return std::make_pair(0, -1);
    } else if ( YminBinF > EMR_range || EMR_range > YmaxBinF ) {
      Squeak::mout(Squeak::debug) << "Range of particle in EMR " <<
	"outside of PDF range, Recon::Global::PIDVarF::Calc_Var()" <<
	std::endl;
      return std::make_pair(0, -1);
    } else {
      // Get trackpoint array from track
      global_track_points = track->GetTrackPoints();
      std::vector<const MAUS::DataStructure::Global::TrackPoint*>
	::iterator eachTP;
      for (eachTP = global_track_points.begin();
	   eachTP != global_track_points.end(); ++eachTP) {
	if (!(*eachTP)) continue;
	if ((*eachTP)->get_mapper_name() == "MapCppGlobalTrackMatching") {
	  if ((*eachTP)->get_detector() >=
		     MAUS::DataStructure::Global::kTracker1_1 &&
		     (*eachTP)->get_detector() <=
		     MAUS::DataStructure::Global::kTracker1_5) {
	    tracker1_track_points.push_back(*eachTP);
	  } else {
	    continue;
	  }
	}
      }
      if (tracker1_track_points.size() < 1) {
	Squeak::mout(Squeak::debug) << "Global track contained no downstream" <<
	  " tracker trackpoints, Recon::Global::PIDVarF::Calc_Var()" << std::endl;
	tracker1_track_points.clear();
	return std::make_pair(-1, 0);
      } else {
	double tracker1_trackpoint_mom = 0;
	int tracker1_tp_count = 0;
	double tracker1_momentum = 0;
	const MAUS::DataStructure::Global::TrackPoint* tracker1_trackpoint;
	for (size_t i = 0; i < tracker1_track_points.size(); i++) {
	  tracker1_trackpoint = tracker1_track_points[i];
	  if (tracker1_trackpoint) {
	    TLorentzVector trackpoint_4mom = tracker1_trackpoint->get_momentum();
	    tracker1_trackpoint_mom +=
	      sqrt(trackpoint_4mom.Px()*trackpoint_4mom.Px() +
		   trackpoint_4mom.Py()*trackpoint_4mom.Py() +
		   trackpoint_4mom.Pz()*trackpoint_4mom.Pz());
	    tracker1_tp_count++;
	  } else {
	    continue;
	  }
	}
	tracker1_track_points.clear();
	tracker1_momentum = tracker1_trackpoint_mom/tracker1_tp_count;
	if ( XminBinF > tracker1_momentum || tracker1_momentum > XmaxBinF ) {
	  Squeak::mout(Squeak::debug) << "Momentum for tracker 1 is outside " <<
	    "of range, Recon::Global::PIDVarF::Calc_Var()" << std::endl;
	  return std::make_pair(0, -1);
	} else {
	  return std::make_pair(tracker1_momentum, EMR_range);
	}
      }
    }
  }
}
}
}


