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

#include "src/map/MapCppEMRSpacePoints/MapCppEMRSpacePoints.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppEMRSpacePoints(void) {
  PyWrapMapBase<MAUS::MapCppEMRSpacePoints>::PyWrapMapBaseModInit
                                                ("MapCppEMRSpacePoints", "", "", "", "");
}

MapCppEMRSpacePoints::MapCppEMRSpacePoints()
    : MapBase<MAUS::Data>("MapCppEMRSpacePoints") {
}

void MapCppEMRSpacePoints::_birth(const std::string& argJsonConfigDocument) {
  _classname = "MapCppEMRSpacePoints";
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                      "Could not resolve ${MAUS_ROOT_DIR} environment variable",
                      "MapCppEMRSpacePoints::birth");
  }

  //  JsonCpp setup
  Json::Value configJSON;
  Json::Value map_file_name;
  Json::Value xEnable_V1731_Unpacking;
  Json::Value xEnable_DBB_Unpacking;

  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();

  _tot_func_p1 = configJSON["EMRtotFuncP1"].asDouble();
  _tot_func_p2 = configJSON["EMRtotFuncP2"].asDouble();
  _tot_func_p3 = configJSON["EMRtotFuncP3"].asDouble();

  // Load the EMR calibration map
  bool loaded = _calibMap.InitializeFromCards(configJSON);
  if ( !loaded )
    throw(Exceptions::Exception(Exceptions::recoverable,
          "Could not find EMR calibration map",
          "MapCppEMRRecon::birth"));

  // Load the EMR attenuation map
  loaded = _attenMap.InitializeFromCards(configJSON);
  if ( !loaded )
    throw(Exceptions::Exception(Exceptions::recoverable,
          "Could not find EMR attenuation map",
          "MapCppEMRReccon::birth"));

  // Load the EMR geometry map
  loaded = _geoMap.InitializeFromCards(configJSON);
  if ( !loaded )
    throw(Exceptions::Exception(Exceptions::recoverable,
          "Could not find EMR geometry map",
          "MapCppEMRReccon::birth"));
}

void MapCppEMRSpacePoints::_death() {
}

void MapCppEMRSpacePoints::_process(Data *data) const {

  // Routine data checks before processing it
  if ( !data )
      throw Exceptions::Exception(Exceptions::recoverable, "Data was NULL",
                    "MapCppEMRSpacePoints::_process");

  Spill* spill = data->GetSpill();
  if ( !spill )
      throw Exceptions::Exception(Exceptions::recoverable, "Spill was NULL",
                    "MapCppEMRSpacePoints::_process");

  if ( spill->GetDaqEventType() != "physics_event" )
      return;

  size_t nPartEvents = spill->GetReconEventSize();
  if ( !nPartEvents )
      return;

  if ( !spill->GetEMRSpillData() )
      return;

  bool emrData = false;
  for (size_t iPe = 0; iPe < nPartEvents; iPe++) {
    EMREvent *evt = spill->GetReconEvents()->at(iPe)->GetEMREvent();
    if ( evt->GetMotherPtr() ) {
      if ( evt->GetMotherPtr()->GetEMRPlaneHitArraySize() ) {
        emrData = true;
        break;
      }
    }
  }
  if ( !emrData )
      return;

  // Create a temporary array containing n+n' plane hit arrays (1 per trigger + spill data)
  size_t nSeconPartEvents = spill->GetEMRSpillData()->GetEMREventTrackArraySize();
  EMREventVector emr_events_tmp(nPartEvents+nSeconPartEvents);

  // Remove the crosstalk hits from each plane hit
  clean_crosstalk(spill, emr_events_tmp);

  // Reconstruct the coordinates of the spacepoint for each plane hit
  reconstruct_coordinates(emr_events_tmp);

  // Correct the SAPMT charge and reconstruct the MAPMT charge for the fibre attenuation
  correct_charge(emr_events_tmp, nPartEvents);

  // Fill the Recon event array with spill information (1 per trigger + daughter candidates)
  fill(spill, emr_events_tmp, nPartEvents);
}

void MapCppEMRSpacePoints::clean_crosstalk(MAUS::Spill* spill,
					   EMREventVector& emr_events_tmp) const {

  size_t nPartEvents = spill->GetReconEvents()->size();
  size_t nSeconPartEvents = spill->GetEMRSpillData()->GetEMREventTrackArraySize();

  for (size_t iPe = 0; iPe < nPartEvents+nSeconPartEvents; iPe++) {
    // Skip events without an EventTrack (mother or candidate)
    EMREventTrack* evtTrack;
    if ( iPe < nPartEvents ) {
      evtTrack = spill->GetReconEvents()->at(iPe)->GetEMREvent()->GetMotherPtr();
    } else {
      evtTrack = spill->GetEMRSpillData()->GetEMREventTrackArray()[iPe-nPartEvents];
    }
    if ( !evtTrack )
	continue;

    // Reject arrays that do not contain both projections (cannot reconstruct a SP)
    EMRPlaneHitArray plArray = evtTrack->GetEMRPlaneHitArray();
    bool HitXY[2] = {false, false};
    for (size_t iPlane = 0; iPlane < plArray.size(); iPlane++) {
      int xOri = plArray[iPlane]->GetOrientation();
      if ( !HitXY[xOri] && plArray[iPlane]->GetEMRBarHitArraySize() )
	  HitXY[xOri] = true;
      if ( HitXY[0] && HitXY[1] )
	  break;
    }
    if ( !HitXY[0] || !HitXY[1] )
        continue;

    // Reject the crosstalk in each plane
    for (size_t iPlane = 0; iPlane < plArray.size(); iPlane++) {
      EMRBarHitArray barHitArray = plArray[iPlane]->GetEMRBarHitArray();
      std::vector<EMRBarHitArray> barHitGroupVector;

      if ( !barHitArray.size() ) { // Skip the plane if there is no bar
	continue;
      } else if ( barHitArray.size() == 1 ) { // Select automatically if there is only 1 hit
        barHitGroupVector.push_back(EMRBarHitArray());
	barHitGroupVector[0].push_back(barHitArray[0]);
      } else { // Keep only the most energetic bunch if there is >= 2 hits

        // Sort the vector in with respect to the bar channel ID
	sort(barHitArray.begin(), barHitArray.end(),
             [] (const EMRBarHit& a, const EMRBarHit& b) {
		 return a.GetChannel() < b.GetChannel();
	     });

        // Create groups of adjacent hits
        barHitGroupVector.push_back(EMRBarHitArray());
	barHitGroupVector[0].push_back(barHitArray[0]);
        for (size_t iHit = 1; iHit < barHitArray.size(); iHit++) {
	  int aBar = barHitArray[iHit-1].GetChannel()%_number_of_bars;
	  int bBar = barHitArray[iHit].GetChannel()%_number_of_bars;
	  if ( abs(bBar-aBar) == 1 ) {
	    barHitGroupVector.back().push_back(barHitArray[iHit]);
	  } else {
	    barHitGroupVector.push_back(EMRBarHitArray());
	    barHitGroupVector.back().push_back(barHitArray[iHit]);
	  }
        }
      }

      // Only keep the group with the highest energy deposition (gets rid of XT)
      size_t mGroup(0);
      double mCharge(0.);
      for (size_t iGroup = 0; iGroup < barHitGroupVector.size(); iGroup++) {
	double aCharge(0);
	for (size_t iHit = 0; iHit < barHitGroupVector[iGroup].size(); iHit++) {
	  double xTot = barHitGroupVector[iGroup][iHit].GetTot();
	  double xCharge = exp(xTot/_tot_func_p1-log(_tot_func_p2))
			   - _tot_func_p3/_tot_func_p2;
	  aCharge += xCharge;
	}
	if ( aCharge > mCharge ) {
	  mGroup = iGroup;
	  mCharge = aCharge;
	}
      }

      // Fill the temporary array with the selected hits (XT cleaned) and plane information
      EMRPlaneTmp plane;
      for (size_t iHit = 0; iHit < barHitGroupVector[mGroup].size(); iHit++)
  	plane._barhits.push_back(barHitGroupVector[mGroup][iHit]);
      plane._plane = plArray[iPlane]->GetPlane();
      plane._charge = plArray[iPlane]->GetCharge();
      emr_events_tmp[iPe].push_back(plane);
    }
  }
}

void MapCppEMRSpacePoints::reconstruct_coordinates(EMREventVector& emr_events_tmp)
			   const {

  for (size_t iPe = 0; iPe < emr_events_tmp.size(); iPe++) {

    // Sort the temporary planes from upstreammost to the downstreammost
    sort(emr_events_tmp[iPe].begin(), emr_events_tmp[iPe].end(),
         [] (const EMRPlaneTmp& a, const EMRPlaneTmp& b) {
	     return a._plane < b._plane;
	 });

    // Look for hits in the other projection to reconstruct the missing coordinate
    for (size_t iPlane = 0; iPlane < emr_events_tmp[iPe].size(); iPlane++) {
      if ( !emr_events_tmp[iPe][iPlane]._barhits.size() )
	  continue;

      int xPlane = emr_events_tmp[iPe][iPlane]._plane;
      int xOri = xPlane%2;

      ThreeVector v0, v1, v2;
      bool Hit1(false), Hit2(false);
      double a(-1.), b(-1.), xi(-1.);

      // Look backwards for hits in the other projection
      int aPlane(0);
      for (aPlane = iPlane-1; aPlane >= 0; aPlane--) {
	if ( emr_events_tmp[iPe][aPlane]._barhits.size() &&
	     emr_events_tmp[iPe][aPlane]._plane%2 != xOri ) {
	  v1 = get_weighted_position(emr_events_tmp[iPe][aPlane]._barhits);
	  Hit1 = true;
	  break;
	}
      }

      // Look forwards for hits in the other projection
      for (size_t bPlane = iPlane+1; bPlane < emr_events_tmp[iPe].size(); bPlane++) {
	if ( emr_events_tmp[iPe][bPlane]._barhits.size() &&
	     emr_events_tmp[iPe][bPlane]._plane%2 != xOri ) {
	  if ( !Hit2 ) {
	    v2 = get_weighted_position(emr_events_tmp[iPe][bPlane]._barhits);
	    Hit2 = true;
	    if ( Hit1 )
		break;
	  } else if ( Hit2 && !Hit1 ) {
	    v1 = get_weighted_position(emr_events_tmp[iPe][bPlane]._barhits);
	    Hit1 = true;
	    break;
	  }
	}
      }

      // Look backwards for the second hit if nothing found in the forward direction
      if ( Hit1 && !Hit2 ) {
        for (int cPlane = aPlane-1; cPlane >= 0; cPlane--) {
	  if ( emr_events_tmp[iPe][cPlane]._barhits.size() &&
	       emr_events_tmp[iPe][cPlane]._plane%2 != xOri ) {
	    v2 = get_weighted_position(emr_events_tmp[iPe][cPlane]._barhits);
	    Hit2 = true;
	    break;
	  }
	}
      }

      // Calculate the parameters of the line between the two complementary planes
      if ( Hit1 && Hit2 ) {
        if ( !xOri ) {
	  a = (v2.y() - v1.y())/(v2.z() - v1.z());
	  b = v1.y() - a*v1.z();
	} else {
	  a = (v2.x() - v1.x())/(v2.z() - v1.z());
	  b = v1.x() - a*v1.z();
	}
      } else if ( Hit1 && !Hit2 ) {
        if ( !xOri ) {
	  b = v1.y();
	} else {
	  b = v1.x();
	}
	a = 0.;
      } else if ( !Hit1 && Hit2 ) {
        if ( !xOri ) {
	  b = v2.y();
	} else {
	  b = v2.x();
	}
	a = 0.;
      }

      // Set the transverse (x or y) and longitudinal (z) errors
      xi = (v0.z() - v1.z())/(v2.z() - v1.z());
      ThreeVector dim = _geoMap.Dimensions(); // (w, h, l) of an EMR bar
      double etrans = dim.y()/(2*sqrt(6)); // Transverse uncertainty
      double elong =  dim.z()/(3*sqrt(2)); // Longitudinal uncertainty

      for (size_t iHit = 0; iHit < emr_events_tmp[iPe][iPlane]._barhits.size(); iHit++) {
        // Find the position of the bar in local coordinates
	EMRBarHit barHit = emr_events_tmp[iPe][iPlane]._barhits[iHit];
	int xBar = barHit.GetChannel()%_number_of_bars;
        EMRChannelKey xKey(xPlane, xOri, xBar, "emr");
        v0 = _geoMap.LocalPosition(xKey);

        // Set the reconstructed (y or x) errors
        if ( Hit1 && Hit2 ) {
	  xi = (v0.z() - v1.z())/(v2.z() - v1.z());
	} else {
	  xi = 0.;
	}
        double erecon = sqrt((pow(xi, 2) // Recon uncertainty !!!TODO!!! (FIX)
			     + pow(1-xi, 2)) * pow(etrans, 2)
		  	     + pow(a, 2) * (pow(xi, 2) + pow(1-xi, 2)) * pow(elong, 2));

	// Add the missing coordinate and set the appropriate errors
        // Interpolate or extrapolate y for an x plane and x for a y plane
        ThreeVector errors;
	if ( !xOri ) {
	  v0.SetY(a*v0.z() + b);
	  errors = ThreeVector(etrans, erecon, elong);
	} else {
	  v0.SetX(a*v0.z() + b);
	  errors = ThreeVector(erecon, etrans, elong);
        }

        // Add a spacepoint to the array
	EMRSpacePointTmp spacePoint;
	spacePoint._pos = v0;
	spacePoint._errors = errors;
	spacePoint._ch = barHit.GetChannel();
	spacePoint._time = barHit.GetTime()-_attenMap.fibreDelay(xKey, v0.x(), v0.y(), "MA");
	spacePoint._deltat = barHit.GetDeltaT()-_attenMap.fibreDelay(xKey, v0.x(), v0.y(), "MA");
	spacePoint._chargema = -1;
	spacePoint._chargesa = -1;

        emr_events_tmp[iPe][iPlane]._spacepoints.push_back(spacePoint);
      }
    }
  }
}

void MapCppEMRSpacePoints::correct_charge(EMREventVector& emr_events_tmp,
					  size_t nPartEvents) const {

  for (size_t iPe = 0; iPe < emr_events_tmp.size(); iPe++) {
    for (size_t iPlane = 0; iPlane < emr_events_tmp[iPe].size(); iPlane++) {

      int xPlane = emr_events_tmp[iPe][iPlane]._plane;
      EMRSpacePointVector spacePointVector = emr_events_tmp[iPe][iPlane]._spacepoints;

      // Reconstruct the MAPMT charge in each bar hit
      double xPlaneChargeMA(0.);
      for (size_t iSP = 0; iSP < spacePointVector.size(); iSP++) {
	int xBar = spacePointVector[iSP]._ch%_number_of_bars;
	EMRChannelKey xKey(xPlane, xPlane%2, xBar, "emr");
        double x = spacePointVector[iSP]._pos.x();		// [mm]
        double y = spacePointVector[iSP]._pos.y();		// [mm]

	double alphMA = _attenMap.fibreAtten(xKey, x, y, "MA");	// Fibre attenuation
        double epsMA = _calibMap.Eps(xKey, "MA");		// Calibration factor

	double xTot = emr_events_tmp[iPe][iPlane]._barhits[iSP].GetTot();
	double xCharge = exp(xTot/_tot_func_p1-log(_tot_func_p2))-_tot_func_p3/_tot_func_p2;
	xCharge /= alphMA*epsMA;

        spacePointVector[iSP]._chargema = xCharge;
        emr_events_tmp[iPe][iPlane]._spacepoints[iSP]._chargema = xCharge;
	xPlaneChargeMA += xCharge;
      }

      // Correct and split the SAPMT charge in each bar, reconstruct it for candidates
      double xPlaneChargeSA(0.), xFactorSA(0.), xFactorMA(0.);
      for (size_t iSP = 0; iSP < spacePointVector.size(); iSP++) {
	int xBar = spacePointVector[iSP]._ch%_number_of_bars;
	EMRChannelKey xKey(xPlane, xPlane%2, xBar, "emr");
        double x = spacePointVector[iSP]._pos.x();	// [mm]
        double y = spacePointVector[iSP]._pos.y();	// [mm]

	double alphSA = _attenMap.fibreAtten(xKey, x, y, "SA");
        double epsSA = _calibMap.Eps(xKey, "SA");
	double alphMA = _attenMap.fibreAtten(xKey, x, y, "MA");
        double epsMA = _calibMap.Eps(xKey, "MA");
        double phi = spacePointVector[iSP]._chargema/xPlaneChargeMA;	// Fraction of the total charge

        xFactorSA += alphSA*epsSA*phi;
	xFactorMA += alphMA*epsMA*phi;
      }
      if ( iPe < nPartEvents ) {
        xPlaneChargeSA = emr_events_tmp[iPe][iPlane]._charge/xFactorSA;
      } else {
	xPlaneChargeSA = xPlaneChargeMA*xFactorSA/xFactorMA;
      }

      for ( size_t iSP = 0; iSP < spacePointVector.size(); iSP++)
	  emr_events_tmp[iPe][iPlane]._spacepoints[iSP]._chargesa =
		xPlaneChargeSA*spacePointVector[iSP]._chargema/xPlaneChargeMA;

      // Correct the error on the spacepoint for its charge fraction,
      // A spacepoint that has comparatively less charge is less definite
      for (size_t iSP = 0; iSP < spacePointVector.size(); iSP++) {
	double xFactor = spacePointVector[iSP]._chargema/xPlaneChargeMA;
        ThreeVector xErrors = spacePointVector[iSP]._errors;
	if ( !(xPlane%2) ) {
	  xErrors.SetX(xErrors.x()/sqrt(fabs(xFactor)));
	} else {
	  xErrors.SetY(xErrors.y()/sqrt(fabs(xFactor)));
	}
	emr_events_tmp[iPe][iPlane]._spacepoints[iSP]._errors = xErrors;
      }
    }
  }
}

void MapCppEMRSpacePoints::fill(MAUS::Spill* spill,
		  	        EMREventVector emr_events_tmp,
		  	        size_t nPartEvents) const {

  // Get the EMR recon events and the spill data
  ReconEventPArray *recEvts =  spill->GetReconEvents();
  EMRSpillData *emrSpill = spill->GetEMRSpillData();

  for (size_t iPe = 0; iPe < emr_events_tmp.size(); iPe++) {

    EMRSpacePointArray spacePointArray;
    for (size_t iPlane = 0; iPlane < emr_events_tmp[iPe].size(); iPlane++) {

      EMRSpacePointVector spacePointVector = emr_events_tmp[iPe][iPlane]._spacepoints;
      for (size_t iSP = 0; iSP < spacePointVector.size(); iSP++) {

        EMRSpacePoint *spacePoint = new EMRSpacePoint;
        spacePoint->SetChannel(spacePointVector[iSP]._ch);
        spacePoint->SetPosition(spacePointVector[iSP]._pos);
        spacePoint->SetGlobalPosition(_geoMap.MakeGlobal(spacePointVector[iSP]._pos));
        spacePoint->SetPositionErrors(spacePointVector[iSP]._errors);
        spacePoint->SetTime(spacePointVector[iSP]._time);
        spacePoint->SetDeltaT(spacePointVector[iSP]._deltat);
        spacePoint->SetChargeMA(spacePointVector[iSP]._chargema);
        spacePoint->SetChargeSA(spacePointVector[iSP]._chargesa);

        spacePointArray.push_back(spacePoint);
      }
    }

    EMREventTrack *evtTrack;
    if ( iPe < nPartEvents ) {
      evtTrack = recEvts->at(iPe)->GetEMREvent()->GetMotherPtr();
    } else {
      evtTrack = emrSpill->GetEMREventTrackArray()[iPe-nPartEvents];
    }
    if ( evtTrack )
      evtTrack->SetEMRSpacePointArray(spacePointArray);
  }
}

ThreeVector MapCppEMRSpacePoints::get_weighted_position(EMRBarHitArray barHitArray) const {

  double wx(0.), wy(0.), wz(0.), w(0.);
  for (size_t iHit = 0; iHit < barHitArray.size(); iHit++) {
    int xPlane = barHitArray[iHit].GetChannel()/_number_of_bars;
    int xBar = barHitArray[iHit].GetChannel()%_number_of_bars;
    EMRChannelKey xKey(xPlane, xPlane%2, xBar, "emr");
    ThreeVector xPos = _geoMap.LocalPosition(xKey);

    double xTot = barHitArray[iHit].GetTot();
    double xCharge= exp(xTot/_tot_func_p1-log(_tot_func_p2))
		    - _tot_func_p3/_tot_func_p2;

    wx += xPos.x()*xCharge;
    wy += xPos.y()*xCharge;
    wz += xPos.z()*xCharge;
    w += xCharge;
  }

  if (w) {
    return ThreeVector(wx/w, wy/w, wz/w);
  } else {
    return ThreeVector(0., 0., 0.);
  }
}
} // namespace MAUS
