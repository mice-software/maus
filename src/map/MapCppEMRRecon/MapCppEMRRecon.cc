/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Utils/CppErrorHandler.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/DAQChannelMap.hh"
#include "Interface/dataCards.hh"
#include "API/PyWrapMapBase.hh"
#include "Converter/DataConverters/CppJsonSpillConverter.hh"
#include "Converter/DataConverters/JsonCppSpillConverter.hh"

#include "src/map/MapCppEMRRecon/MapCppEMRRecon.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppEMRRecon(void) {
  PyWrapMapBase<MAUS::MapCppEMRRecon>::PyWrapMapBaseModInit
                                                ("MapCppEMRRecon", "", "", "", "");
}

MapCppEMRRecon::MapCppEMRRecon()
    : MapBase<MAUS::Data>("MapCppEMRRecon") {
}

void MapCppEMRRecon::_birth(const std::string& argJsonConfigDocument) {
  _classname = "MapCppEMRRecon";
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    throw MAUS::Exception(Exception::recoverable,
                      "Could not resolve ${MAUS_ROOT_DIR} environment variable",
                      "MapCppEMRRecon::birth");
  }

  //  JsonCpp setup
  Json::Value configJSON;

  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();
  _bar_width = configJSON["EMRbarWidth"].asDouble();
  _bar_height = configJSON["EMRbarHeight"].asDouble();
  _gap = configJSON["EMRgap"].asDouble();

  _secondary_hits_bunching_distance = configJSON["EMRsecondaryHitsBunchingDistance"].asInt();
  _secondary_hits_bunching_width = configJSON["EMRsecondaryHitsBunchingWidth"].asInt();

  _primary_trigger_minXhits = configJSON["EMRprimaryTriggerMinXhits"].asInt();
  _primary_trigger_minYhits = configJSON["EMRprimaryTriggerMinYhits"].asInt();
  _primary_trigger_minNhits = configJSON["EMRprimaryTriggerMinNhits"].asInt();
  _secondary_trigger_minXhits = configJSON["EMRsecondaryTriggerMinXhits"].asInt();
  _secondary_trigger_minYhits = configJSON["EMRsecondaryTriggerMinYhits"].asInt();
  _secondary_trigger_minNhits = configJSON["EMRsecondaryTriggerMinNhits"].asInt();
  _secondary_trigger_minTot = configJSON["EMRsecondaryTriggerMinTot"].asInt();

  _max_secondary_to_primary_track_distance
	= configJSON["EMRmaxSecondaryToPrimaryTrackDistance"].asInt();

  _tot_func_p1 = configJSON["EMRtotFuncP1"].asDouble();
  _tot_func_p2 = configJSON["EMRtotFuncP2"].asDouble();
  _tot_func_p3 = configJSON["EMRtotFuncP3"].asDouble();
  _tot_func_p4 = configJSON["EMRtotFuncP4"].asDouble();

  // Load the EMR calibration map
  bool loaded = _calibMap.InitializeFromCards(configJSON);
  if (!loaded)
    throw(Exception(Exception::recoverable,
          "Could not find EMR calibration map",
          "MapCppEMRMCDigitizer::birth"));

  // Load the EMR attenuation map
  loaded = _attenMap.InitializeFromCards(configJSON);
  if (!loaded)
    throw(Exception(Exception::recoverable,
          "Could not find EMR attenuation map",
          "MapCppEMRMCDigitizer::birth"));
}

void MapCppEMRRecon::_death() {
}

void MapCppEMRRecon::_process(Data *data) const {

  // Get spill, look into reconEvents, break if there's no EMR data
  Spill *spill = data->GetSpill();

  // At this stage, the tree needs to contain 1 event per trigger + noise + decays
  int nPartEvents = spill->GetReconEventSize();
  if (!nPartEvents)
      return;

  bool emrdata = false;
  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    EMREvent *evt = spill->GetReconEvents()->at(iPe)->GetEMREvent();
    int nPlHits = evt->GetEMRPlaneHitArray().size();

    if (nPlHits) {
      emrdata = true;
      break;
    }
  }
  if (!emrdata)
      return;

  // Create DBB and fADC arrays with n+2 events (1 per trigger + noise + decays)
  EMRDBBEventVector emr_dbb_events_tmp = get_dbb_data_tmp(nPartEvents);
  EMRfADCEventVector_er emr_fadc_events_tmp = get_fadc_data_tmp(nPartEvents);

  // Create DBB and fADC arrays to future host n+1+n' events (1 per trigger + noise + 1 per decay)
  EMRDBBEventVector emr_dbb_events[3]; // preselected, primary, secondary
  EMRfADCEventVector_er emr_fadc_events;
  EMRTrackEventVector emr_track_events;

  // Fill temporary array with preselected events
  process_preselected_events(spill, emr_dbb_events_tmp, emr_fadc_events_tmp);

  // Bunch secondary hits and place them in new reconEvents
  process_secondary_events(emr_dbb_events_tmp, emr_fadc_events_tmp,
			   emr_dbb_events, emr_fadc_events, emr_track_events);

  // Fill primary and secondary events array with the most significant bar of each plane
  tot_cleaning(nPartEvents, emr_dbb_events, emr_fadc_events, emr_track_events);

  // Reconstruct the main PID variables for muon tagging
  pid_variables(nPartEvents, emr_dbb_events, emr_track_events);

  // Reconstruct the coordinates of each Hit
  coordinates_reconstruction(nPartEvents, emr_dbb_events, emr_fadc_events);

  // Correct the ToT and charge using the calibration constants
  energy_correction(nPartEvents, emr_dbb_events, emr_fadc_events);

  // Match the primary tracks with their decay
  track_matching(nPartEvents, emr_dbb_events, emr_fadc_events, emr_track_events);

  // Calculate the total corrected charge the energy loss pattern
  event_charge_calculation(nPartEvents, emr_dbb_events, emr_fadc_events, emr_track_events);

  // Fill the Recon event array with Spill information (/!\ only 1 per trigger /!\)
  fill(spill, nPartEvents - 2, emr_dbb_events, emr_fadc_events, emr_track_events);
}

void MapCppEMRRecon::process_preselected_events(MAUS::Spill *spill,
						EMRDBBEventVector& emr_dbb_events_tmp,
						EMRfADCEventVector_er& emr_fadc_events_tmp) const {

  int nPartEvents = spill->GetReconEvents()->size();

  for (int iPe = 0; iPe < nPartEvents; iPe++) {

    EMREvent *evt = spill->GetReconEvents()->at(iPe)->GetEMREvent();

    int nPlHits = evt->GetEMRPlaneHitArray().size();

    if ( !nPlHits ) continue;

    // Total amount of bars hit in one trigger
    int nBarsTotal = 0;
    for (int iPlane = 0; iPlane < nPlHits; iPlane++) {
      EMRPlaneHit *plHit = evt->GetEMRPlaneHitArray().at(iPlane);
      nBarsTotal += plHit->GetEMRBarArray().size();
    }

    // Fill collection of pre-selected events
    for (int iPlane = 0; iPlane < nPlHits; iPlane++) {

      EMRPlaneHit *plHit = evt->GetEMRPlaneHitArray().at(iPlane);
      int xPlane = plHit->GetPlane();
      int nBars = plHit->GetEMRBarArray().size();

      for (int iBar = 0; iBar < nBars; iBar++) {

	EMRBar *bar = plHit->GetEMRBarArray().at(iBar);
	int xBar = bar->GetBar();

	// Skip calibration channel
	if (xBar == 0) continue;

	int nBarHits = bar->GetEMRBarHitArray().size();

	for (int iBarHit = 0; iBarHit < nBarHits; iBarHit++) {

	  EMRBarHit barHit = bar->GetEMRBarHitArray().at(iBarHit);
	  int xTot  = barHit.GetTot();
	  int xDeltaT = barHit.GetDeltaT();
	  int xHitTime = barHit.GetHitTime();
	  double x = -1.0;
	  double y = -1.0;
	  double z = -1.0;

	  EMRBarHit bHit;
	  bHit.SetTot(xTot);
	  bHit.SetDeltaT(xDeltaT);
	  bHit.SetHitTime(xHitTime);
	  bHit.SetX(x);
	  bHit.SetY(y);
	  bHit.SetZ(z);

	  emr_dbb_events_tmp[iPe][xPlane][xBar].push_back(bHit);
	}
      }

      int xOri = plHit->GetOrientation();
      int xArrivalTime = plHit->GetDeltaT();
      double xCharge = plHit->GetCharge();
      double xPedestalArea = plHit->GetPedestalArea();
      std::vector<int> xSamples = plHit->GetSamples();

      fADCdata_er data;
      data._orientation = xOri;
      data._charge = xCharge;
      data._pedestal_area = xPedestalArea;
      data._time = xArrivalTime;
      data._samples = xSamples;
      emr_fadc_events_tmp[iPe][xPlane] = data;
    }
  }
}

void MapCppEMRRecon::process_secondary_events(EMRDBBEventVector emr_dbb_events_tmp,
					     EMRfADCEventVector_er emr_fadc_events_tmp,
					     EMRDBBEventVector *emr_dbb_events,
					     EMRfADCEventVector_er& emr_fadc_events,
					     EMRTrackEventVector& emr_track_events) const {

  int nPartEvents = emr_fadc_events_tmp.size();

  // The last trigger corresponds to the secondary events
  int secondaryEventId = nPartEvents-1;

  // Save hit time of each secondary trigger in a vector
  std::vector<int> hitTimeVector;
  for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
    for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
      int nHits = emr_dbb_events_tmp[secondaryEventId][iPlane][iBar].size();

      for (int iBarHit = 0; iBarHit < nHits; iBarHit++) {
  	EMRBarHit bHit = emr_dbb_events_tmp[secondaryEventId][iPlane][iBar][iBarHit];
	int xTot = bHit.GetTot(); // DBB counts
  	int hitTime = bHit.GetHitTime(); // DBB counts
	if (xTot > _secondary_trigger_minTot) hitTimeVector.push_back(hitTime);
      }
    }
  }

  // Sort the vector in ascending order
  sort(hitTimeVector.begin(), hitTimeVector.end());
  int hitTimeVectorSize = hitTimeVector.size();

  // Find groups of hits defined by _secondary_hits_bunching_width
  std::vector<int> hitTimeGroup;
  int nHitsInGroup = 0;

  for (int iHit = 1; iHit < hitTimeVectorSize+1; iHit++) {
    int deltat;
    if (iHit == hitTimeVectorSize) deltat = _secondary_hits_bunching_distance+1;
    else
      deltat = hitTimeVector.at(iHit) - hitTimeVector.at(iHit-1);

    if (nHitsInGroup >= _secondary_trigger_minNhits &&
	deltat > _secondary_hits_bunching_distance ) {
      hitTimeGroup.push_back(hitTimeVector.at(iHit-1));
      nHitsInGroup = 0;
    } else if (deltat <= _secondary_hits_bunching_width) {
      nHitsInGroup++;
    }
  }

  // Number of the secondary trigger
  int nSeconPartEvents = hitTimeGroup.size();

  // Resize the event arrays to accomodate one extra event per secondary track (n')
  for (int iArray = 0; iArray < 3; iArray++)
    emr_dbb_events[iArray] = get_dbb_data_tmp(nPartEvents+nSeconPartEvents);
  emr_fadc_events = get_fadc_data_tmp(nPartEvents+nSeconPartEvents);
  emr_track_events = get_track_data_tmp(nPartEvents+nSeconPartEvents);

  // Fill collection of events (pre-selected)
  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
	int nHits = emr_dbb_events_tmp[iPe][iPlane][iBar].size();

	for (int iBarHit = 0; iBarHit < nHits; iBarHit++) {
	  EMRBarHit bHit = emr_dbb_events_tmp[iPe][iPlane][iBar][iBarHit];
	  emr_dbb_events[0][iPe][iPlane][iBar].push_back(bHit);
	}
      }

      if (iPe < nPartEvents-2) { // All triggers except the noise and secondary
	fADCdata_er data;
	data._orientation = emr_fadc_events_tmp[iPe][iPlane]._orientation;
	data._charge = emr_fadc_events_tmp[iPe][iPlane]._charge;
	data._pedestal_area = emr_fadc_events_tmp[iPe][iPlane]._pedestal_area;
	data._time = emr_fadc_events_tmp[iPe][iPlane]._time;
	data._samples = emr_fadc_events_tmp[iPe][iPlane]._samples;
	emr_fadc_events[iPe][iPlane] = data;
      }
    }
  }

  // Associate each bunch to a secondary trigger
  for (int iHitTimeGroup = 0; iHitTimeGroup < nSeconPartEvents; iHitTimeGroup++) {
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
	int nHits = emr_dbb_events_tmp[secondaryEventId][iPlane][iBar].size();

	for (int iBarHit = 0; iBarHit < nHits; iBarHit++) {
	  EMRBarHit bHit = emr_dbb_events_tmp[secondaryEventId][iPlane][iBar][iBarHit];
	  int hitTime = bHit.GetHitTime(); // DBB counts
	  int deltat = hitTimeGroup.at(iHitTimeGroup) - hitTime;

	  if (deltat >= 0 && deltat < _secondary_hits_bunching_width) {
	    emr_dbb_events[0][secondaryEventId+1+iHitTimeGroup]
			  [iPlane][iBar].push_back(bHit);
	  }
	}
      }
    }
  }
}

void MapCppEMRRecon::tot_cleaning(int nPartEvents,
				  EMRDBBEventVector *emr_dbb_events,
				  EMRfADCEventVector_er& emr_fadc_events,
				  EMRTrackEventVector& emr_track_events) const {

  int nTotalPartEvents = emr_fadc_events.size();

  for (int iPe = 0; iPe < nTotalPartEvents; iPe++) {
    // Skip noise and secondary triggers
    if (iPe == nPartEvents-2 || iPe == nPartEvents-1) continue;

    // Count number of X and Y planes hit
    int xPlaneHits = 0;
    int yPlaneHits = 0;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      int nHits = 0;

      for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
  	nHits += emr_dbb_events[0][iPe][iPlane][iBar].size();
      }
      if (nHits && iPlane % 2 == 0) xPlaneHits++;
      if (nHits && iPlane % 2 == 1) yPlaneHits++;
    }

    // Reject triggers with too small amount of hits
    if (iPe < nPartEvents-2 &&
	(xPlaneHits < _primary_trigger_minXhits ||
	 yPlaneHits < _primary_trigger_minYhits ||
	 (xPlaneHits + yPlaneHits) < _primary_trigger_minNhits)) continue;
    if (iPe >= nPartEvents &&
	(xPlaneHits < _secondary_trigger_minXhits ||
	 yPlaneHits < _secondary_trigger_minYhits ||
	 (xPlaneHits + yPlaneHits) < _secondary_trigger_minNhits)) continue;

    emr_track_events[iPe]._has_primary = true;

    // Select the channel with the highest ToT in each plane
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      int xTotMax  = 0;
      int xBarMax = 0;
      int xHitMax = 0;

      for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
  	int nHits = emr_dbb_events[0][iPe][iPlane][iBar].size();

  	for (int iBarHit = 0; iBarHit < nHits; iBarHit++) {
  	  EMRBarHit bHit = emr_dbb_events[0][iPe][iPlane][iBar][iBarHit];
  	  int xTot = bHit.GetTot();

  	  if (xTot > xTotMax) {
  	    xTotMax = xTot;
  	    xHitMax = iBarHit;
	    xBarMax = iBar;
  	  }
  	}
      }
      for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
      	int nHits = emr_dbb_events[0][iPe][iPlane][iBar].size();

	// Fill primary events array
      	for (int iBarHit = 0; iBarHit < nHits; iBarHit++) {
      	  EMRBarHit bHit = emr_dbb_events[0][iPe][iPlane][iBar][iBarHit];

      	  if (iBar == xBarMax && iBarHit == xHitMax &&
	      bHit.GetTot()>_secondary_trigger_minTot) {
      	    emr_dbb_events[1][iPe][iPlane][iBar].push_back(bHit);
      	  }
      	}
      }
    }
  }
}

void MapCppEMRRecon::pid_variables(int nPartEvents,
				  EMRDBBEventVector *emr_dbb_events,
				  EMRTrackEventVector& emr_track_events) const {

  // Loop over the primary events only
  for (int iPe = 0; iPe < nPartEvents - 2; iPe++) {

    // Skip the events without a primary track
    if ( !emr_track_events[iPe]._has_primary ) continue;

    int nPlane(0), lPlaneX(0), lPlaneY(0);
    vector<double> x[2], y[2];

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      for (int iBar = 1; iBar < _number_of_bars; iBar++) {
	if ( emr_dbb_events[0][iPe][iPlane][iBar].size() ) {
	  if (iPlane%2 == 0 && iPlane/2+1 > lPlaneX) lPlaneX = iPlane/2+1;
	  else if (iPlane%2 != 0 && iPlane/2+1 > lPlaneY) lPlaneY = iPlane/2+1;
	}
	if ( emr_dbb_events[1][iPe][iPlane][iBar].size() ) {
	  nPlane++;
	  if (iPlane%2 == 0) {
	    if (iBar%2 == 0) {
  	      x[0].push_back(iPlane + 1./3);
	      y[0].push_back(iBar);
	    } else {
  	      x[0].push_back(iPlane + 2./3);
	      y[0].push_back(iBar);
	    }
	  } else {
	    if (iBar%2 == 0) {
  	      x[1].push_back(iPlane + 1./3);
	      y[1].push_back(iBar);
	    } else {
  	      x[1].push_back(iPlane + 2./3);
	      y[1].push_back(iBar);
	    }
	  }
	}
      }
    }

    // Definition of the plane density, lPlaneX+lPlaneY > 0 as _has_primary = true
    emr_track_events[iPe]._plane_density = static_cast<double>(nPlane/(lPlaneX+lPlaneY));

    // Compute the normalised chi^2 in the two projections
    for (int iArray = 0; iArray < 2; iArray++) {
      int n = x[iArray].size();
      double xmean(0.0), ymean(0.0);
      for (int i = 0; i < n; i++) {
        xmean += x[iArray][i]/n;
        ymean += y[iArray][i]/n;
      }

      double xymean(0.0), x2mean(0.0), y2mean(0.0);
      for (int i = 0; i < n; i++) {
        xymean += x[iArray][i]*y[iArray][i]/n;
        x2mean += x[iArray][i]*x[iArray][i]/n;
        y2mean += y[iArray][i]*y[iArray][i]/n;
      }

      double xvar = x2mean - xmean*xmean;
      double yvar = y2mean - ymean*ymean;
      double xyvar = xymean - xmean*ymean;
      double xycorr(0);
      if (xvar > 0 && yvar > 0) xycorr = xyvar/sqrt(xvar*yvar);
      else
	xycorr = 1;

      double chi2 = yvar*(1-pow(xycorr, 2));
      if (chi2 < 0) chi2 = 0.;

      if (iArray == 0) emr_track_events[iPe]._chi2_x = chi2;
      else
	emr_track_events[iPe]._chi2_y = chi2;
    }
  }
}

void MapCppEMRRecon::coordinates_reconstruction(int nPartEvents,
						EMRDBBEventVector *emr_dbb_events,
						EMRfADCEventVector_er& emr_fadc_events) const {

  int nTotalPartEvents = emr_fadc_events.size();

  for (int iPe = 0; iPe < nTotalPartEvents; iPe++) {

    // Skip noise and secondary triggers
    if (iPe == nPartEvents-1 || iPe == nPartEvents-2) continue;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {

      bool Hit0Found = false;
      bool Hit1Found = false;
      bool Hit2Found = false;

      double x0(-1.0), y0(-1.0), z0(-1.0), x1(-1.0), x2(-1.0), y1(-1.0), y2(-1.0);
      double a(-1.0), b(-1.0), xi(-1.0);

      // Find Primary hit, carry on if there's none
      int iBar;
      for (iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
  	if (emr_dbb_events[1][iPe][iPlane][iBar].size()) {
	  x0 = iPlane * (_bar_height + _gap) + (1. + iBar%2) * _bar_height * 1./3;
	  z0 = (iBar - _number_of_bars/2) * (_bar_width/2 + _gap);
	  Hit0Found = true;
	  break;
	}
      }
      if (!Hit0Found) continue;

      // Look backwards for hits
      for (int aPlane = iPlane-1; aPlane >= 0; aPlane = aPlane-2) {
	for (int aBar = 1; aBar < _number_of_bars; aBar++) { // Skip test channel
	  if (emr_dbb_events[1][iPe][aPlane][aBar].size()) {
	    x1 = aPlane * (_bar_height + _gap) + (1. + aBar%2) * _bar_height * 1./3;
	    y1 = (aBar - _number_of_bars/2) * (_bar_width/2 + _gap);
	    Hit1Found = true;
	    break;
	  }
	}
	if (Hit1Found) break;
      }

      // Look forward for hits
      for (int bPlane = iPlane+1; bPlane < _number_of_planes; bPlane = bPlane+2) {
	for (int bBar = 1; bBar < _number_of_bars; bBar++) { // Skip test channel
	  if (emr_dbb_events[1][iPe][bPlane][bBar].size()) {
	    if (Hit2Found && !Hit1Found) {
	      x1 = bPlane * (_bar_height + _gap) + (1. + bBar%2) * _bar_height * 1./3;
	      y1 = (bBar - _number_of_bars/2) * (_bar_width/2 + _gap);
	      Hit1Found = true;
	      break;
	    }
	    if (!Hit2Found) {
	      x2 = bPlane * (_bar_height + _gap) + (1. + bBar%2) * _bar_height * 1./3;
	      y2 = (bBar - _number_of_bars/2) * (_bar_width/2 + _gap);
	      Hit2Found = true;
	    }
	  }
	}
	if (Hit1Found && Hit2Found) break;
      }

      // Look backwards for the second hit if nothing found in the forward direction
      for (int aPlane = iPlane-1; aPlane >= 0; aPlane = aPlane-2) {
	if ((aPlane + 1) * (_bar_height + _gap) > x1) continue;
	for (int aBar = 1; aBar < _number_of_bars; aBar++) { // Skip test channel
	  if (emr_dbb_events[1][iPe][aPlane][aBar].size()) {
	    if (Hit1Found && !Hit2Found) {
	      x2 = aPlane * (_bar_height + _gap) + (1. + aBar%2) * _bar_height * 1./3;
	      y2 = (aBar - _number_of_bars/2) * (_bar_width/2 + _gap);
	      Hit2Found = true;
	      break;
	    }
	  }
	}
	if (Hit1Found && Hit2Found) break;
      }

      // Calculate the missing coordinate (y0) of the primary hit in this plane
      if (Hit1Found && Hit2Found) {
	a = (y2 - y1)/(x2 - x1);
	b = y1 - a * x1;
	y0 = a * x0 + b;
	xi = (x0 - x1)/(x2 - x1);
      }

      if (Hit1Found && !Hit2Found) {
	y0 = y1;
	a = 0.0;
	xi = 0.0;
      }

      if (!Hit1Found && Hit2Found) {
	y0 = y2;
	a = 0.0;
	xi = 0.0;
      }

      // Return the coordinates in metric, see EMR.dat for geometry
      double etrans = _bar_width/(2*sqrt(6)); // Transverse uncertainty
      double elong =  _bar_height/(3*sqrt(2)); // Longitudinal uncertainty
      double erecon = sqrt((pow(xi, 2) + pow(1-xi, 2)) * pow(etrans, 2)
		  + pow(a, 2) * (pow(xi, 2) + pow(1-xi, 2)) * pow(elong, 2)); // Recon uncertainty

      if (iPlane % 2 == 0) {
	emr_dbb_events[1][iPe][iPlane][iBar][0].SetX(z0);
	emr_dbb_events[1][iPe][iPlane][iBar][0].SetErrorX(etrans);
	emr_dbb_events[1][iPe][iPlane][iBar][0].SetY(y0);
	emr_dbb_events[1][iPe][iPlane][iBar][0].SetErrorY(erecon);
      } else {
	emr_dbb_events[1][iPe][iPlane][iBar][0].SetX(y0);
	emr_dbb_events[1][iPe][iPlane][iBar][0].SetErrorX(erecon);
	emr_dbb_events[1][iPe][iPlane][iBar][0].SetY(z0);
	emr_dbb_events[1][iPe][iPlane][iBar][0].SetErrorY(etrans);
      }

      emr_dbb_events[1][iPe][iPlane][iBar][0].SetZ(x0);
      emr_dbb_events[1][iPe][iPlane][iBar][0].SetErrorZ(elong);
    }
  }
}

void MapCppEMRRecon::energy_correction(int nPartEvents,
				       EMRDBBEventVector *emr_dbb_events,
				       EMRfADCEventVector_er& emr_fadc_events) const {

  int nTotalPartEvents = emr_fadc_events.size();

  for (int iPe = 0; iPe < nTotalPartEvents; iPe++) {

    int nPrimPartEvents = 0;

    // Skip noise and secondary triggers
    if (iPe == nPartEvents-1 || iPe == nPartEvents-2) continue;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      // Fetch the attenuation parameters from the map
      double alpha_MA = 1.0;
      double alpha_SA = 1.0; // Default parameters, no corrections

      int nBars = 0;
      for (int iBar = 0; iBar < _number_of_bars; iBar++)
	if (emr_dbb_events[1][iPe][iPlane][iBar].size())
	  nBars++;

      if (nBars) {
        for (int iBar = 0; iBar < _number_of_bars; iBar++) {
          if (emr_dbb_events[1][iPe][iPlane][iBar].size()) {
	    EMRBarHit barHit = emr_dbb_events[1][iPe][iPlane][iBar].at(0);
	    double x = barHit.GetX(); // mm
	    double y = barHit.GetY(); // mm

	    EMRChannelKey xKey(iPlane, iPlane%2, iBar, "emr");

	    alpha_MA = _attenMap.fibreAtten(xKey, x, y, "MA");
	    alpha_SA = _attenMap.fibreAtten(xKey, x, y, "SA");

            nPrimPartEvents++;
          }
	}

        // Reconstruct the corrected MAPMT charge
        double Q_MA = 0.0;
        int xPrimBar = 0;

        for (int iArray = 0; iArray < 2; iArray++) {
	  for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
	  int nBarHits = emr_dbb_events[iArray][iPe][iPlane][iBar].size();
            if (nBarHits) {

	      EMRChannelKey xKey(iPlane, iPlane%2, iBar, "emr");

	      for (int iBarHit = 0; iBarHit < nBarHits; iBarHit++) {

	        EMRBarHit barHit = emr_dbb_events[iArray][iPe][iPlane][iBar].at(iBarHit);
	        int xTot  = barHit.GetTot();

	        // Correct single MAPMT signals
	        if (nPrimPartEvents) {
	          double epsilon_MA_i = _calibMap.Eps(xKey, "MA");
	          double Q_MA_meas_i = _tot_func_p4
				       * (exp((static_cast<double>(xTot) - _tot_func_p1)
				   	      / _tot_func_p2) - _tot_func_p3);
	          if (Q_MA_meas_i < 0) Q_MA_meas_i = 0.0;

	          double Q_MA_i = Q_MA_meas_i/(alpha_MA*epsilon_MA_i);
	          emr_dbb_events[iArray][iPe][iPlane][iBar][iBarHit].SetChargeCorrected(Q_MA_i);

	          if (iArray == 0) Q_MA += Q_MA_i;
	          else
		    xPrimBar = iBar;
	        }
	      }
	    }
	  }
        }

        // Total MAPMT charge, all bars combined
        if (nPrimPartEvents)
	  emr_dbb_events[1][iPe][iPlane][xPrimBar][0].SetTotalChargeCorrected(Q_MA);

        // Reconstruct the corrected SAPMT charge
        double Q_SA_meas_over_Q_SA = 0.0; // Ratio of the measured SAPMT over the real one

        if (nPrimPartEvents) {
      	  for (int iBar = 1; iBar < _number_of_bars; iBar++) {
	  int nBarHits = emr_dbb_events[0][iPe][iPlane][iBar].size();

	  if (nBarHits) {

	      EMRChannelKey xKey(iPlane, iPlane%2, iBar, "emr");
	      double epsilon_SA_i = _calibMap.Eps(xKey, "SA");

      	      for (int iBarHit = 0; iBarHit < nBarHits; iBarHit++) {

      	        double Q_MA_i = emr_dbb_events[0][iPe][iPlane][iBar][iBarHit].GetChargeCorrected();
      	        double phi_MA_i = Q_MA_i/Q_MA; // Fraction of the light in hit iBarHit
      	        double phi_SA_i = phi_MA_i; // Same fraction interpolated for the SAPMT
      	        Q_SA_meas_over_Q_SA += phi_SA_i*alpha_SA*epsilon_SA_i; // Contribution to the ratio
      	      }
	    }
      	  }

	  if (Q_SA_meas_over_Q_SA) {
	    double Q_SA_meas = emr_fadc_events[iPe][iPlane]._charge;
	    // If the charge is lost, assume equivalence on both sides
	    if (Q_SA_meas == 0) {
	      emr_fadc_events[iPe][iPlane]._charge_corrected = Q_MA;
	    } else {
	      double Q_SA = Q_SA_meas/Q_SA_meas_over_Q_SA;
	      // Correct for the disparity between the MA charge and the SA charge (fitting)
	      EMRChannelKey xGlobalAverageKey(-1, -1, -1, "emr");
	      double global_ma = _calibMap.Eps(xGlobalAverageKey, "MA");
	      double global_sa = _calibMap.Eps(xGlobalAverageKey, "SA");

	      emr_fadc_events[iPe][iPlane]._charge_corrected = Q_SA*global_ma/global_sa;
	    }
	  }
        }
      }
    }
  }
}

void MapCppEMRRecon::track_matching(int nPartEvents,
				    EMRDBBEventVector *emr_dbb_events,
				    EMRfADCEventVector_er& emr_fadc_events,
				    EMRTrackEventVector& emr_track_events) const {

  int nTotalPartEvents = emr_fadc_events.size();

  for (int iPe = 0; iPe < nPartEvents-2; iPe++) {

    // Skip the events without a primary track
    if (!emr_track_events[iPe]._has_primary) continue;

    // Range calculation of primary tracks
    double x1(-1.0), y1(-1.0), z1(-1.0);
    double x2(-1.0), y2(-1.0), z2(-1.0);
    double primEventRange = 0.0;
    bool primHitsFound = false;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
  	if (emr_dbb_events[1][iPe][iPlane][iBar].size()) {
  	  EMRBarHit bHit = emr_dbb_events[1][iPe][iPlane][iBar][0];
	  // Last point
	  x2 = bHit.GetX();
	  y2 = bHit.GetY();
	  z2 = bHit.GetZ();
	  if (primHitsFound)
	    primEventRange = primEventRange + sqrt(pow(x1-x2, 2)+pow(y1-y2, 2)+pow(z1-z2, 2));

	  // Previous point
	  x1 = bHit.GetX();
	  y1 = bHit.GetY();
	  z1 = bHit.GetZ();
	  primHitsFound = true;
	}
      }
    }

    // Range calculation of secondary tracks
    double minDist = 999.9;
    int seconEventId = -1;
    double seconEventRange = -1.0;

    for (int iiPe = nPartEvents; iiPe < nTotalPartEvents; iiPe++) {
      double x3(-1.0), y3(-1.0), z3(-1.0);
      double x4(-1.0), y4(-1.0), z4(-1.0);
      double x5(-1.0), y5(-1.0), z5(-1.0);
      double seconEventRangeTmp = 0.0;
      bool seconHitsFound = false;

      for (int iiPlane = 0; iiPlane < _number_of_planes; iiPlane++) {
	for (int iiBar = 1; iiBar < _number_of_bars; iiBar++) { // Skip test channel
	  if (emr_dbb_events[1][iiPe][iiPlane][iiBar].size()) {
	    EMRBarHit bHit = emr_dbb_events[1][iiPe][iiPlane][iiBar][0];

	    // Last point
	    x5 = bHit.GetX();
	    y5 = bHit.GetY();
	    z5 = bHit.GetZ();

	    if (!seconHitsFound) {
	      // First point
	      x3 = x5;
	      y3 = y5;
	      z3 = z5;
	    }
	    if (seconHitsFound) {
	      seconEventRangeTmp = seconEventRangeTmp
				 + sqrt(pow(x5-x4, 2)+pow(y5-y4, 2)+pow(z5-z4, 2));
	    }
	    // Previous point
	    x4 = bHit.GetX();
	    y4 = bHit.GetY();
	    z4 = bHit.GetZ();
	    seconHitsFound = true;
	  }
	}
      }

      // Look for a track close enough to the primary to match it
      if (seconHitsFound) {
	double dist23 = sqrt(pow(x2-x3, 2)+pow(y2-y3, 2)+pow(z2-z3, 2));
	double dist25 = sqrt(pow(x2-x5, 2)+pow(y2-y5, 2)+pow(z2-z5, 2));
	double dist = std::min(dist23, dist25);

	if (dist < minDist && dist < _max_secondary_to_primary_track_distance) {
	  minDist = dist;
	  seconEventId = iiPe;
	  seconEventRange = seconEventRangeTmp;
	}
      }
    }

//    std::cerr << "minDist: " << minDist << " seconEventId: " << seconEventId << std::endl;
//    std::cerr << "Rp=" << primEventRange << " Rs=" << seconEventRange << "   " << std::endl;

    // If a secondary is found, fill the corresponding secondary array
    if (seconEventId != -1) {
      for (int iiPlane = 0; iiPlane < _number_of_planes; iiPlane++) {
	for (int iiBar = 1; iiBar < _number_of_bars; iiBar++) { // Skip test channel
	  if (emr_dbb_events[1][seconEventId][iiPlane][iiBar].size()) {
	    EMRBarHit bHit = emr_dbb_events[1][seconEventId][iiPlane][iiBar][0];
	    emr_dbb_events[2][iPe][iiPlane][iiBar].push_back(bHit);
	  }
	}
      }
      emr_track_events[iPe]._range_secondary = seconEventRange;
      emr_track_events[iPe]._has_secondary = true;
      emr_track_events[iPe]._secondary_to_primary_track_distance = minDist;
    }
    emr_track_events[iPe]._range_primary = primEventRange;
  }
}

void MapCppEMRRecon::event_charge_calculation(int nPartEvents,
					      EMRDBBEventVector *emr_dbb_events,
					      EMRfADCEventVector_er& emr_fadc_events,
					      EMRTrackEventVector& emr_track_events) const {

  for (int iPe = 0; iPe < nPartEvents-2; iPe++) {

    // Skip the events without a primary track
    if (!emr_track_events[iPe]._has_primary) continue;

    // Reconstrcut total charge deposited by the primary particle
    double total_charge_ma = 0.0;
    double total_charge_sa = 0.0;
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      total_charge_sa += emr_fadc_events[iPe][iPlane]._charge_corrected;
      for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
  	if (emr_dbb_events[1][iPe][iPlane][iBar].size()) {
  	  EMRBarHit barHit = emr_dbb_events[1][iPe][iPlane][iBar][0];
	  total_charge_ma += barHit.GetTotalChargeCorrected();
	  break;
	}
      }
    }

    emr_track_events[iPe]._total_charge_ma = total_charge_ma;
    emr_track_events[iPe]._total_charge_sa = total_charge_sa;

    // Find the end point of the track (last plane hit)
    int aPlane = -1;
    int bPlane = -1;
    for (int iPlane = _number_of_planes - 1; iPlane >= 0; iPlane--) {
      for (int iBar = 1; iBar < _number_of_bars; iBar++) {
  	if (emr_dbb_events[1][iPe][iPlane][iBar].size()) {
	  bPlane = iPlane + 1;
	  break;
	}
      }
      if (bPlane > 0) break;
    }

    // Location of the plane 4/5 along the track round up
    aPlane = bPlane*4/5;

    // Reconstruct the total charge before the aPlane
    int nsa1 = 0;
    int nma1 = 0;
    double Q_SA_1 = 0.0;
    double Q_MA_1 = 0.0;
    for (int iPlane = 0; iPlane < aPlane; iPlane++) {
      double qsa = emr_fadc_events[iPe][iPlane]._charge_corrected;
      if (qsa) {
	Q_SA_1 += qsa;
	nsa1++;
      }
      for (int iBar = 1; iBar < _number_of_bars; iBar++) {
  	if (emr_dbb_events[1][iPe][iPlane][iBar].size()) {
  	  EMRBarHit bHit = emr_dbb_events[1][iPe][iPlane][iBar][0];
	  double qma = bHit.GetTotalChargeCorrected();
	  if (qma) {
	    Q_MA_1 += qma;
	    nma1++;
	    break;
	  }
	}
      }
    }

    // Reconstruct the total charge after the aPlane
    int nsa2 = 0;
    int nma2 = 0;
    double Q_SA_2 = 0.0;
    double Q_MA_2 = 0.0;
    for (int iPlane = aPlane; iPlane < bPlane; iPlane++) {
      double qsa = emr_fadc_events[iPe][iPlane]._charge_corrected;
      if (qsa) {
	Q_SA_2 += qsa;
	nsa2++;
      }
      for (int iBar = 1; iBar < _number_of_bars; iBar++) {
  	if (emr_dbb_events[1][iPe][iPlane][iBar].size()) {
  	  EMRBarHit bHit = emr_dbb_events[1][iPe][iPlane][iBar][0];
	  double qma = bHit.GetTotalChargeCorrected();
	  if (qma) {
	    Q_MA_2 += qma;
	    nma2++;
	    break;
	  }
	}
      }
    }

    // Compute the charge ratio
    if (Q_SA_2 != 0 && nsa1 != 0)
      emr_track_events[iPe]._charge_ratio_sa = (Q_SA_1*static_cast<double>(nsa2))
					     / (Q_SA_2*static_cast<double>(nsa1));
    if (Q_MA_2 != 0 && nma1 != 0)
      emr_track_events[iPe]._charge_ratio_ma = (Q_MA_1*static_cast<double>(nma2))
					     / (Q_MA_2*static_cast<double>(nma1));
  }
}

void MapCppEMRRecon::fill(Spill *spill,
			  int nPartEvents,
			  EMRDBBEventVector *emr_dbb_events,
			  EMRfADCEventVector_er& emr_fadc_events,
			  EMRTrackEventVector& emr_track_events) const {

  int xRun = spill->GetRunNumber();
  int xSpill = spill->GetSpillNumber();

  // Only save the primary triggers with their primary and seconday arrays (n - 2)
  ReconEventPArray *recEvts = spill->GetReconEvents();
  recEvts->resize(nPartEvents);

  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    EMREvent *evt = new EMREvent;
    EMRPlaneHitArray plArray;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      int xOri = emr_fadc_events[iPe][iPlane]._orientation;
      double xCharge = emr_fadc_events[iPe][iPlane]._charge;
      double xChargeCorrected = emr_fadc_events[iPe][iPlane]._charge_corrected;
      int xArrivalTime = emr_fadc_events[iPe][iPlane]._time;
      double xPedestalArea = emr_fadc_events[iPe][iPlane]._pedestal_area;
      std::vector<int> xSamples = emr_fadc_events[iPe][iPlane]._samples;

      EMRPlaneHit *plHit = new EMRPlaneHit;
      plHit->SetPlane(iPlane);
      plHit->SetTrigger(iPe);
      plHit->SetRun(xRun);
      plHit->SetOrientation(xOri);
      plHit->SetCharge(xCharge);
      plHit->SetChargeCorrected(xChargeCorrected);
      plHit->SetDeltaT(xArrivalTime);
      plHit->SetSpill(xSpill);
      plHit->SetPedestalArea(xPedestalArea);
      plHit->SetSamples(xSamples);

      EMRBarArray barArray;
      EMRBarArray barArrayPrimary;
      EMRBarArray barArraySecondary;
      for (int iArray = 0; iArray < 3; iArray++) {
	for (int iBar = 1; iBar < _number_of_bars; iBar++) {
	  int nHits = emr_dbb_events[iArray][iPe][iPlane][iBar].size();
	  if (nHits) {
	    EMRBar *bar = new EMRBar;
	    bar->SetBar(iBar);
	    bar->SetEMRBarHitArray(emr_dbb_events[iArray][iPe][iPlane][iBar]);
	    if (iArray == 0) barArray.push_back(bar);
	    if (iArray == 1) barArrayPrimary.push_back(bar);
	    if (iArray == 2) barArraySecondary.push_back(bar);
	  }
	}
      }

      plHit->SetEMRBarArray(barArray);
      plHit->SetEMRBarArrayPrimary(barArrayPrimary);
      plHit->SetEMRBarArraySecondary(barArraySecondary);
      if (barArray.size() || barArrayPrimary.size() || barArraySecondary.size() || xCharge) {
	plArray.push_back(plHit);
      } else {
        delete plHit;
      }
    }
    evt->SetEMRPlaneHitArray(plArray);
    evt->SetRangePrimary(emr_track_events[iPe]._range_primary);
    evt->SetRangeSecondary(emr_track_events[iPe]._range_secondary);
    evt->SetSecondaryToPrimaryTrackDistance
	 (emr_track_events[iPe]._secondary_to_primary_track_distance);
    evt->SetHasPrimary(emr_track_events[iPe]._has_primary);
    evt->SetHasSecondary(emr_track_events[iPe]._has_secondary);
    evt->SetTotalChargeMA(emr_track_events[iPe]._total_charge_ma);
    evt->SetTotalChargeSA(emr_track_events[iPe]._total_charge_sa);
    evt->SetChargeRatioMA(emr_track_events[iPe]._charge_ratio_ma);
    evt->SetChargeRatioSA(emr_track_events[iPe]._charge_ratio_sa);
    evt->SetPlaneDensity(emr_track_events[iPe]._plane_density);
    evt->SetChi2(emr_track_events[iPe]._chi2_x + emr_track_events[iPe]._chi2_y);

    // All the noise and unmatched secondaries are dumped, only initial triggers are kept
    evt->SetInitialTrigger(true);

    // std::cerr << "************************************************" << std::endl;
    // std::cerr << "range_primary = " << emr_track_events[iPe]._range_primary << std::endl;
    // std::cerr << "range_secondary = " << emr_track_events[iPe]._range_secondary << std::endl;
    // std::cerr << "secondary_to_primary_track_distance = "
    // 		 << emr_track_events[iPe]._secondary_to_primary_track_distance << std::endl;
    // std::cerr << "has_primary = " << emr_track_events[iPe]._has_primary << std::endl;
    // std::cerr << "has_secondary = " << emr_track_events[iPe]._has_secondary << std::endl;
    // std::cerr << "================================================" << std::endl;

    recEvts->at(iPe)->SetEMREvent(evt);
    recEvts->at(iPe)->SetPartEventNumber(iPe);
  }
  spill->SetReconEvents(recEvts);
}

EMRDBBEventVector MapCppEMRRecon::get_dbb_data_tmp(int nPartEvts) const {
  EMRDBBEventVector emr_dbb_events_tmp;
  emr_dbb_events_tmp.resize(nPartEvts);
  for (int iPe = 0; iPe < nPartEvts; iPe++) {
    emr_dbb_events_tmp[iPe].resize(_number_of_planes);  // number of planes
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      emr_dbb_events_tmp[iPe][iPlane].resize(_number_of_bars); // number of bars in a plane
    }
  }
  return emr_dbb_events_tmp;
}

EMRfADCEventVector_er MapCppEMRRecon::get_fadc_data_tmp(int nPartEvts) const {
  EMRfADCEventVector_er emr_fadc_events_tmp;
  emr_fadc_events_tmp.resize(nPartEvts);
  for (int iPe = 0; iPe < nPartEvts ;iPe++) {
    emr_fadc_events_tmp[iPe].resize(_number_of_planes);
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      fADCdata_er data;
      data._orientation = iPlane%2;
      data._charge = 0.0;
      data._charge_corrected = 0.0;
      data._pedestal_area = 0.0;
      data._time = 0;
      std::vector<int> xSamples;
      data._samples = xSamples;
      emr_fadc_events_tmp[iPe][iPlane] = data;
    }
  }
  return emr_fadc_events_tmp;
}

EMRTrackEventVector MapCppEMRRecon::get_track_data_tmp(int nPartEvts) const {
  EMRTrackEventVector emr_track_events_tmp;
  emr_track_events_tmp.resize(nPartEvts);
  for (int iPe = 0; iPe < nPartEvts ;iPe++) {
    TrackData data;
    data._range_primary = -1.0;
    data._range_secondary = -1.0;
    data._secondary_to_primary_track_distance = -1.0;
    data._has_primary = false;
    data._has_secondary = false;
    data._total_charge_ma = -1.0;
    data._total_charge_sa = -1.0;
    data._charge_ratio_ma = -1.0;
    data._charge_ratio_sa = -1.0;
    data._plane_density = -1.0;
    data._chi2_x = -1.0;
    data._chi2_y = -1.0;
    emr_track_events_tmp[iPe] = data;
  }
  return emr_track_events_tmp;
}
}
