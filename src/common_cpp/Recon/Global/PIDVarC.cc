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
#include "Recon/Global/PIDVarC.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  const std::string PIDVarC::VARIABLE = "KLChargeProdvsDSTrackerMom";

  PIDVarC::PIDVarC(std::string hypothesis, std::string unique_identifier)
    : PIDBase2D(VARIABLE, hypothesis, unique_identifier,
                 XminBinC, XmaxBinC, XnumBins, YminBinC, YmaxBinC, YnumBins) {
    _nonZeroHistEntries = true;
  }

  PIDVarC::PIDVarC(TFile* file, std::string hypothesis, int XminC, int XmaxC,
		   int YminC, int YmaxC)
    : PIDBase2D(file, VARIABLE, hypothesis, XminC, XmaxC, YminC, YmaxC,
		XminBinC, XmaxBinC, YminBinC, YmaxBinC) {
  }

  PIDVarC::~PIDVarC() {}

  std::pair<double, double> PIDVarC::Calc_Var(MAUS::DataStructure::Global::Track* track) {
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
      if ((*eachTP)->get_mapper_name() == "MapCppGlobalTrackMatching-Through") {
	if ((*eachTP)->get_detector() == KL_DP) {
	  kl_track_points.push_back(*eachTP);
	} else if ((*eachTP)->get_detector() >=
		   MAUS::DataStructure::Global::kTracker1_1 &&
		   (*eachTP)->get_detector() <=
		   MAUS::DataStructure::Global::kTracker1_5) {
	  tracker1_track_points.push_back(*eachTP);
	} else {
	  continue;
	}
      }
    }
    if (kl_track_points.size() < 1) {
      Squeak::mout(Squeak::debug) << "Global track contained no KL" <<
	" trackpoints, Recon::Global::PIDVarC::Calc_Var()" << std::endl;
      kl_track_points.clear();
      return std::make_pair(0, -1);
    } else if (tracker1_track_points.size() < 1) {
      Squeak::mout(Squeak::debug) << "Global track contained no downstream" <<
	" tracker trackpoints, Recon::Global::PIDVarC::Calc_Var()" << std::endl;
      tracker1_track_points.clear();
      kl_track_points.clear();
      return std::make_pair(-1, 0);
    } else {
      for (size_t i = 0; i < kl_track_points.size(); i++) {
	total_ADC_charge_product += kl_track_points[i]->get_ADC_charge_product();
      }
      if ( YminBinC > (total_ADC_charge_product) || (total_ADC_charge_product) > YmaxBinC ) {
	Squeak::mout(Squeak::debug) << "KL ADC charge product " <<
	  "outside of allowed range, Recon::Global::PIDVarC::Calc_Var()" <<
	  std::endl;
	kl_track_points.clear();
	tracker1_track_points.clear();
	return std::make_pair(0, -1);
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
	kl_track_points.clear();
	tracker1_track_points.clear();
	tracker1_momentum = tracker1_trackpoint_mom/tracker1_tp_count;
	if ( XminBinC > tracker1_momentum || tracker1_momentum > XmaxBinC ) {
	  Squeak::mout(Squeak::debug) << "Momentum for tracker 1 is outside " <<
	    "of range, Recon::Global::PIDVarC::Calc_Var()" << std::endl;
	  return std::make_pair(0, -1);
	} else {
	  return std::make_pair(tracker1_momentum, (total_ADC_charge_product));
	}
      }
    }
  }
}
}
}

