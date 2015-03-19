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

////////////////////////////////////////////////////////////////////////////
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
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRRecon::_death() {
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRRecon::_process(Data *data) const {

  // Get spill, break if there's no recon data
  Spill *spill = data->GetSpill();

  int nPartEvents = spill->GetReconEventSize();
  if (!nPartEvents)
      return;

  // Create DBB and fADC arrays with n+2 events (1 per trigger + noise + decays)
  EMRDBBEventVector emr_dbb_events_tmp = get_dbb_data_tmp(nPartEvents);
  EMRfADCEventVector emr_fadc_events_tmp = get_fadc_data_tmp(nPartEvents);

  // Create DBB and fADC arrays to future host n+1+n' events (1 per trigger + noise + 1 per decay)
  EMRDBBEventVector emr_dbb_events[3]; // preselected, primary, secondary
  EMRfADCEventVector emr_fadc_events;
  EMRTrackEventVector emr_track_events;

  // Fill temporary array with preselected events
  process_preselected_events(spill, emr_dbb_events_tmp, emr_fadc_events_tmp);

  // Bunch secondary hits and place them in new reconEvents
  process_secondary_events(emr_dbb_events_tmp, emr_fadc_events_tmp,
			   emr_dbb_events, emr_fadc_events, emr_track_events);

  // Fill primary and secondary events array with the most significant bar of each plane
  tot_cleaning(nPartEvents, emr_dbb_events, emr_fadc_events, emr_track_events);

  // Reconstruct the coordinates of each Hit
  coordinates_reconstruction(nPartEvents, emr_dbb_events, emr_fadc_events, emr_track_events);

  // Match the primary tracks with their decay
  track_matching(nPartEvents, emr_dbb_events, emr_fadc_events, emr_track_events);

  // Fill the Recon event array with Spill information (1 per trigger + noise + decays)
  fill(spill, nPartEvents, emr_dbb_events, emr_fadc_events, emr_track_events);
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRRecon::process_preselected_events(MAUS::Spill *spill,
						EMRDBBEventVector& emr_dbb_events_tmp,
						EMRfADCEventVector& emr_fadc_events_tmp) const {

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
	  double x = 0.0;
	  double y = 0.0;
	  double z = 0.0;

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

      fADCdata data;
      data._orientation = xOri;
      data._charge = xCharge;
      data._pedestal_area = xPedestalArea;
      data._time = xArrivalTime;
      data._samples = xSamples;
      emr_fadc_events_tmp[iPe][xPlane] = data;
    }
  }
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRRecon::process_secondary_events(EMRDBBEventVector emr_dbb_events_tmp,
					     EMRfADCEventVector emr_fadc_events_tmp,
					     EMRDBBEventVector *emr_dbb_events,
					     EMRfADCEventVector& emr_fadc_events,
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
  for (int i = 0; i < 3; i++)
    emr_dbb_events[i] = get_dbb_data_tmp(nPartEvents+nSeconPartEvents);
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
	fADCdata data;
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

////////////////////////////////////////////////////////////////////////////
void MapCppEMRRecon::tot_cleaning(int nPartEvents,
				  EMRDBBEventVector *emr_dbb_events,
				  EMRfADCEventVector& emr_fadc_events,
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
  	nHits = nHits + emr_dbb_events[0][iPe][iPlane][iBar].size();
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

////////////////////////////////////////////////////////////////////////////
void MapCppEMRRecon::coordinates_reconstruction(int nPartEvents,
						EMRDBBEventVector *emr_dbb_events,
						EMRfADCEventVector& emr_fadc_events,
						EMRTrackEventVector& emr_track_events) const {

  int nTotalPartEvents = emr_fadc_events.size();

  for (int iPe = 0; iPe < nTotalPartEvents; iPe++) {

    // Skip noise and secondary triggers
    if (iPe == nPartEvents-1 || iPe == nPartEvents-2) continue;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {

      EMRBarHit Hit0;
      bool Hit0Found = false;
      bool Hit1Found = false;
      bool Hit2Found = false;

      int barid = -1;

      int x0(-1), x1(-1), x2(-1), y1(-1), y2(-1);
      double y0(-1.0), a(-1.0), b(-1.0);

      // Find Primary hit
      for (int iBar = 1; iBar < _number_of_bars; iBar++) { // Skip test channel
	if (Hit0Found) break;
  	if (emr_dbb_events[1][iPe][iPlane][iBar].size()) {
  	  Hit0 = emr_dbb_events[1][iPe][iPlane][iBar][0];
	  Hit0Found = true;
	  x0 = iPlane;
	  barid = iBar;
	}
      }

      if (!Hit0Found) continue;

      // Look backwards for hits
      for (int aPlane = iPlane-1; aPlane >= 0; aPlane = aPlane-2) {
	if (Hit1Found) break;
	for (int aBar = 1; aBar < _number_of_bars; aBar++) { // Skip test channel
	  if (emr_dbb_events[1][iPe][aPlane][aBar].size()) {
	    if (!Hit1Found) {
	      Hit1Found = true;
	      x1 = aPlane;
	      y1 = aBar;
	    }
	  }
	}
      }

      // Look forward for hits
      for (int bPlane = iPlane+1; bPlane < _number_of_planes; bPlane = bPlane+2) {
	if (Hit1Found && Hit2Found) break;
	for (int bBar = 1; bBar < _number_of_bars; bBar++) { // Skip test channel
	  if (emr_dbb_events[1][iPe][bPlane][bBar].size()) {
	    if (Hit2Found && !Hit1Found) {
	      Hit1Found = true;
	      x1 = bPlane;
	      y1 = bBar;
	    }
	    if (!Hit2Found) {
	      Hit2Found = true;
	      x2 = bPlane;
	      y2 = bBar;
	    }
	  }
	}
      }

      // Look backwards for the second hit if nothing found in the forward direction
      for (int aPlane = iPlane-1; aPlane >= 0; aPlane = aPlane-2) {
	if (Hit1Found && Hit2Found) break;
	if (aPlane == x1) continue;
	for (int aBar = 1; aBar < _number_of_bars; aBar++) { // Skip test channel
	  if (emr_dbb_events[1][iPe][aPlane][aBar].size()) {
	    if (Hit1Found && !Hit2Found) {
	      Hit2Found = true;
	      x2 = aPlane;
	      y2 = aBar;
	    }
	  }
	}
      }

      // Calculate the coordinates of the hit in each plane
      if (Hit1Found && Hit2Found) {
	a = (static_cast<double>(y2)-static_cast<double>(y1))
	    / (static_cast<double>(x2)-static_cast<double>(x1));
	b = static_cast<double>(y1) - a*static_cast<double>(x1);
	y0 = a*static_cast<double>(x0) + b;
      }

      if (Hit1Found && !Hit2Found) {
	y0 = y1;
      }

      if (!Hit1Found && Hit2Found) {
	y0 = y2;
      }

      if (iPlane % 2 == 0) {
	emr_dbb_events[1][iPe][x0][barid][0].SetX(barid);
	emr_dbb_events[1][iPe][x0][barid][0].SetY(y0);
      }
      if (iPlane % 2 == 1) {
	emr_dbb_events[1][iPe][x0][barid][0].SetX(y0);
	emr_dbb_events[1][iPe][x0][barid][0].SetY(barid);
      }
      emr_dbb_events[1][iPe][x0][barid][0].SetZ(x0);
    }
  }
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRRecon::track_matching(int nPartEvents,
				    EMRDBBEventVector *emr_dbb_events,
				    EMRfADCEventVector& emr_fadc_events,
				    EMRTrackEventVector& emr_track_events) const {

  int nTotalPartEvents = emr_fadc_events.size();

  for (int iPe = 0; iPe < nPartEvents; iPe++) {
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
	  if (primHitsFound) {
	    primEventRange = primEventRange + sqrt(pow(x1-x2, 2)+pow(y1-y2, 2)+pow(z1-z2, 2));
	  }
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

////////////////////////////////////////////////////////////////////////////
void MapCppEMRRecon::fill(Spill *spill,
			  int nPartEvents,
			  EMRDBBEventVector *emr_dbb_events,
			  EMRfADCEventVector& emr_fadc_events,
			  EMRTrackEventVector& emr_track_events) const {

  int nTotalPartEvents = emr_fadc_events.size();
  int recPartEvents = spill->GetReconEventSize();
  int xRun = spill->GetRunNumber();
  int xSpill = spill->GetSpillNumber();

  ReconEventPArray *recEvts =  spill->GetReconEvents();

  if (recPartEvents < nTotalPartEvents) {
    for (int iPe = recPartEvents; iPe < nTotalPartEvents; iPe++)
      recEvts->push_back(new ReconEvent);
  }

  for (int iPe = 0; iPe < nTotalPartEvents; iPe++) {
    EMREvent *evt = new EMREvent;
    EMRPlaneHitArray plArray;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      int xOri  = emr_fadc_events[iPe][iPlane]._orientation;
      double xCharge = emr_fadc_events[iPe][iPlane]._charge;
      int xArrivalTime = emr_fadc_events[iPe][iPlane]._time;
      double xPedestalArea = emr_fadc_events[iPe][iPlane]._pedestal_area;
      std::vector<int> xSamples = emr_fadc_events[iPe][iPlane]._samples;

      EMRPlaneHit *plHit = new EMRPlaneHit;
      plHit->SetPlane(iPlane);
      plHit->SetTrigger(iPe);
      plHit->SetRun(xRun);
      plHit->SetOrientation(xOri);
      plHit->SetCharge(xCharge);
      plHit->SetDeltaT(xArrivalTime);
      plHit->SetSpill(xSpill);
      plHit->SetPedestalArea(xPedestalArea);
      plHit->SetSamples(xSamples);

      EMRBarArray barArray;
      EMRBarArray barArrayPrimary;
      EMRBarArray barArraySecondary;
      for (int i = 0; i < 3; i++) {
	for (int iBar = 1; iBar < _number_of_bars; iBar++) {
	  int nHits = emr_dbb_events[i][iPe][iPlane][iBar].size();
	  if (nHits) {
	    EMRBar *bar = new EMRBar;
	    bar->SetBar(iBar);
	    bar->SetEMRBarHitArray(emr_dbb_events[i][iPe][iPlane][iBar]);
	    if (i == 0) barArray.push_back(bar);
	    if (i == 1) barArrayPrimary.push_back(bar);
	    if (i == 2) barArraySecondary.push_back(bar);
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

    if (iPe < nPartEvents-2) evt->SetInitialTrigger(true);
    else
      evt->SetInitialTrigger(false);

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

EMRfADCEventVector MapCppEMRRecon::get_fadc_data_tmp(int nPartEvts) const {
  EMRfADCEventVector emr_fadc_events_tmp;
  emr_fadc_events_tmp.resize(nPartEvts);
  for (int iPe = 0; iPe < nPartEvts ;iPe++) {
    emr_fadc_events_tmp[iPe].resize(_number_of_planes);
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      fADCdata data;
      data._orientation = iPlane%2;
      data._charge = 0.0;
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
    data._range_primary = 0.0;
    data._range_secondary = 0.0;
    data._secondary_to_primary_track_distance = 0.0;
    data._has_primary = false;
    data._has_secondary = false;
    emr_track_events_tmp[iPe] = data;
  }
  return emr_track_events_tmp;
}
}
