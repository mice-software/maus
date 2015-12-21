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

  _dbb_count = configJSON["EMRdbbCount"].asDouble();
  _charge_threshold = configJSON["EMRchargeThreshold"].asDouble();
  _polynomial_order = configJSON["EMRpolynomialOrder"].asInt();
  _max_time = configJSON["EMRmaxMotherDaughterTime"].asDouble();
  _max_distance = configJSON["EMRmaxMotherDaughterDistance"].asDouble();
  _hole_fraction = configJSON["EMRholeFraction"].asDouble();

  // Load the EMR calibration map
  bool loaded = _calibMap.InitializeFromCards(configJSON);
  if (!loaded)
    throw(Exception(Exception::recoverable,
          "Could not find EMR calibration map",
          "MapCppEMRRecon::birth"));

  // Load the EMR attenuation map
  loaded = _attenMap.InitializeFromCards(configJSON);
  if (!loaded)
    throw(Exception(Exception::recoverable,
          "Could not find EMR attenuation map",
          "MapCppEMRReccon::birth"));

  // Load the EMR geometry map
  loaded = _geoMap.InitializeFromCards(configJSON);
  if (!loaded)
    throw(Exception(Exception::recoverable,
          "Could not find EMR geometry map",
          "MapCppEMRReccon::birth"));
}

void MapCppEMRRecon::_death() {
}

void MapCppEMRRecon::_process(Data *data) const {

  // Routine data checks before processing it
  if ( !data )
      throw Exception(Exception::recoverable, "Data was NULL",
                     "MapCppEMRRecon::_process");

  Spill* spill = data->GetSpill();
  if ( !spill )
      throw Exception(Exception::recoverable, "Spill was NULL",
                     "MapCppEMRRecon::_process");

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

  // Reconstruct the plane density from plane hits
  reconstruct_plane_density(spill, nPartEvents);

  // Calculate the total corrected charge and the energy loss pattern from space points
  reconstruct_event_charge(spill, nPartEvents);

  // Fit the space points with an EMRTrack
  reconstruct_tracks(spill, nPartEvents);

  // Try to match the mother particles with their decay daughter
  match_daughters(spill, nPartEvents);
}

void MapCppEMRRecon::reconstruct_plane_density(MAUS::Spill* spill,
					       size_t nPartEvents) const {

  // Loop over mothers and candidates to reconstruct their plane density
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

    // Sort the planes by ascending order of plane ID
    EMRPlaneHitArray planeHitArray = evtTrack->GetEMRPlaneHitArray();
    sort(planeHitArray.begin(), planeHitArray.end(),
         [] (const EMRPlaneHit* a, const EMRPlaneHit* b) {
	     return a->GetPlane() < b->GetPlane();
         });

    // Find the first, last and total amount of plane hit by the particle
    int fPlaneMA(47), lPlaneMA(0), nPlaneMA(0);
    int fPlaneSA(47), lPlaneSA(0), nPlaneSA(0);
    for (size_t iPlane = 0; iPlane < planeHitArray.size(); iPlane++) {
      int xPlane = planeHitArray[iPlane]->GetPlane();
      if ( planeHitArray[iPlane]->GetEMRBarHitArraySize() ) {
	nPlaneMA++;
	if ( xPlane < fPlaneMA )
	    fPlaneMA = xPlane;
        if ( xPlane > lPlaneMA )
	    lPlaneMA = xPlane;
      }
      if ( planeHitArray[iPlane]->GetCharge() > _charge_threshold ) {
	nPlaneSA++;
	if ( xPlane < fPlaneSA )
	    fPlaneSA = xPlane;
        if ( xPlane > lPlaneSA )
	    lPlaneSA = xPlane;
      }
    }

    // Mothers should alway hit the first plane
    if ( iPe < nPartEvents ) {
      fPlaneMA = 0;
      fPlaneSA = 0;
    }

    if ( nPlaneMA )
        evtTrack->SetPlaneDensityMA(static_cast<double>
					(nPlaneMA)/(lPlaneMA-fPlaneMA+1));
    if ( iPe < nPartEvents && nPlaneSA )
        evtTrack->SetPlaneDensitySA(static_cast<double>
					(nPlaneSA)/(lPlaneSA-fPlaneSA+1));
  }
}

void MapCppEMRRecon::reconstruct_event_charge(MAUS::Spill* spill,
					      size_t nPartEvents) const {

  // Loop over mothers and candidates to reconstruct their charge
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
    if ( !evtTrack->GetEMRSpacePointArraySize() )
	continue;

    // Add up the charge from all the spacepoints
    EMRSpacePointArray spacePointArray = evtTrack->GetEMRSpacePointArray();
    double xTotalChargeMA(0.), xTotalChargeSA(0.);
    for (size_t iSP = 0; iSP < spacePointArray.size(); iSP++) {
      xTotalChargeMA += spacePointArray[iSP]->GetChargeMA();
      xTotalChargeSA += spacePointArray[iSP]->GetChargeSA();
    }
    evtTrack->SetTotalChargeMA(xTotalChargeMA);
    evtTrack->SetTotalChargeSA(xTotalChargeSA);

    // Only reconstruct the charge ratio for mothers (muon/pion), irrelevant for candidates
    if ( iPe < nPartEvents ) {

      // Calculate the total charge in the last 1/5th of the track
      size_t nSP = spacePointArray.size();
      size_t tSP = nSP*4./5; // Threshold space point ID
      double xTailChargeMA(0.), xTailChargeSA(0.);
      for (size_t iSP = tSP; iSP < nSP; iSP++) {
        xTailChargeMA += spacePointArray[iSP]->GetChargeMA();
        xTailChargeSA += spacePointArray[iSP]->GetChargeSA();
      }

      double xFraction = static_cast<double>(nSP-tSP)/nSP; // Actual proportion in the tail
      evtTrack->SetChargeRatioMA(xFraction*(xTotalChargeMA-xTailChargeMA)/xTailChargeMA);
      evtTrack->SetChargeRatioSA(xFraction*(xTotalChargeSA-xTailChargeSA)/xTailChargeSA);
    }

    // Reconstruct the global time of the event (end time for mothers, begin time for daughters)
    std::vector<EMRSpacePoint*>::iterator minIterator =
	min_element(spacePointArray.begin(), spacePointArray.end(),
     		    [] (const EMRSpacePoint* a, const EMRSpacePoint* b) {
			return a->GetTime() < b->GetTime();
		    });

    double mTime(0.);
    if ( iPe < nPartEvents ) {
      mTime = spacePointArray[distance(spacePointArray.begin(), minIterator)]->GetTime();
    } else {
      mTime = spacePointArray[distance(spacePointArray.begin(), minIterator)]->GetTime();
    }
    evtTrack->SetTime(mTime*_dbb_count);	// [ns]
  }
}

void MapCppEMRRecon::reconstruct_tracks(MAUS::Spill* spill,
					size_t nPartEvents) const {

  // Loop over mothers and candidates to reconstruct their tracks
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
    if ( !evtTrack->GetEMRSpacePointArraySize() )
	continue;

    // Fit the space points with a polynomial of order n=1 and get the fit parameters
    EMRSpacePointArray spacePointArray = evtTrack->GetEMRSpacePointArray();
    EMRTrack track;
    TrackFitter::polynomial_fit(spacePointArray, track, 1);

    std::vector<double> ax = track.GetParametersX();
    std::vector<double> ay = track.GetParametersY();
    std::vector<double> eax = track.GetParametersErrorsX();
    std::vector<double> eay = track.GetParametersErrorsY();

    // Set the origin, inclination and azimuth of mothers only
    if ( iPe < nPartEvents ) {
      double z0 = _geoMap.LocalStart().z();
      double x0 = TrackFitter::pol(z0, ax);
      double y0 = TrackFitter::pol(z0, ay);

      ThreeVector xOrigin(x0, y0, z0); // Local origin
      track.SetOrigin(_geoMap.MakeGlobal(xOrigin));

      // Propagate the error on the origin from the fit parameters
      double sigma_x = TrackFitter::pol_error(z0, 0., ax, eax);
      double sigma_y = TrackFitter::pol_error(z0, 0., ay, eay);

      ThreeVector xOriginErrors(sigma_x, sigma_y, 0.);
      track.SetOriginErrors(xOriginErrors);

      // Add the origin to the list of trackpoints
      EMRTrackPoint trackPoint;
      trackPoint.SetPosition(xOrigin);
      trackPoint.SetGlobalPosition(_geoMap.MakeGlobal(xOrigin));
      trackPoint.SetPositionErrors(xOriginErrors);
      trackPoint.SetChannel(-1);
      trackPoint.SetResiduals(0., 0.);
      trackPoint.SetChi2(0.);

      track.AddEMRTrackPoint(trackPoint);

      // Reconstruct the inclination and azimuth at the origin
      double mx = TrackFitter::dnpol(z0, ax, 1);
      double emx = TrackFitter::dnpol_error(z0, 0, ax, eax, 1);
      double my = TrackFitter::dnpol(z0, ay, 1);
      double emy = TrackFitter::dnpol_error(z0, 0, ay, eay, 1);
      track.SetPolar(TrackFitter::polar(mx, my));
      track.SetPolarError(TrackFitter::polar_error(mx, my, emx, emy));
      track.SetAzimuth(TrackFitter::azimuth(mx, my));
      track.SetAzimuthError(TrackFitter::azimuth_error(mx, my, emx, emy));
    }

    // For each space point, create a track point on the track
    for (size_t iSP = 0; iSP < spacePointArray.size(); iSP++) {

      // Find the z of the point on the track closest to the spacepoint in the relevant projection
      int xPlane = spacePointArray[iSP]->GetChannel()/_number_of_bars;
      double zstart(_geoMap.LocalStart().z()), zend(_geoMap.LocalEnd().z());

      double z(0.);
      double zp = spacePointArray[iSP]->GetPosition().z();
      if ( !xPlane ) {
        double xp = spacePointArray[iSP]->GetPosition().x();
	z = TrackFitter::pol_closest(ax, zp, xp, zstart, zend);
      } else {
        double yp = spacePointArray[iSP]->GetPosition().y();
	z = TrackFitter::pol_closest(ay, zp, yp, zstart, zend);
      }

      // Evaluate the function in the two projections in the closest z and set the positon
      double x = TrackFitter::pol(z, ax);
      double y = TrackFitter::pol(z, ay);
      ThreeVector xPos(x, y, z);

      // Propagate the error on the track point position from the fit parameters
      double sigma_x = TrackFitter::pol_error(z, 0., ax, eax);
      double sigma_y = TrackFitter::pol_error(z, 0., ay, eay);
      ThreeVector xErrors(sigma_x, sigma_y, spacePointArray[iSP]->GetPositionErrors().z());

      // Calculate the residuals and chi squared between the corrected and orginial points
      double xRes = x - spacePointArray[iSP]->GetPosition().x();
      double yRes = y - spacePointArray[iSP]->GetPosition().y();
      double xErr = spacePointArray[iSP]->GetPositionErrors().x();
      double yErr = spacePointArray[iSP]->GetPositionErrors().y();
      double xChi2 = (pow(xRes, 2)+pow(yRes, 2))/(pow(xErr, 2)+pow(yErr, 2));

      // Set the track point
      EMRTrackPoint trackPoint;
      trackPoint.SetPosition(xPos);
      trackPoint.SetGlobalPosition(_geoMap.MakeGlobal(xPos));
      trackPoint.SetPositionErrors(xErrors);
      trackPoint.SetChannel(spacePointArray[iSP]->GetChannel());
      trackPoint.SetResiduals(xRes, yRes);
      trackPoint.SetChi2(xChi2);

      track.AddEMRTrackPoint(trackPoint);
    }

    // Sort the newly formed track points by ascending order of z
    EMRTrackPointArray trackPointArray = track.GetEMRTrackPointArray();
    sort(trackPointArray.begin(), trackPointArray.end(),
         [] (const EMRTrackPoint& a, const EMRTrackPoint& b) {
	     return a.GetPosition().z() < b.GetPosition().z();
	 });
    track.SetEMRTrackPointArray(trackPointArray);

    // Reconstruct the range of the particle in the EMR
    double zstart = track.GetEMRTrackPointArray().front().GetPosition().z();
    double ezstart = track.GetEMRTrackPointArray().front().GetPositionErrors().z();
    double zend = track.GetEMRTrackPointArray().back().GetPosition().z();
    double ezend = track.GetEMRTrackPointArray().back().GetPositionErrors().z();

    double xRange = TrackRange::range_integral(ax, ay, zstart, zend);
    double xRangeError = TrackRange::range_integral_error(ax, ay, eax, eay,
							  zstart, zend, ezstart, ezend);
    track.SetRange(xRange);
    track.SetRangeError(xRangeError);

    // Estimate the momentum of the mothers from the CSDA range (default=muon)
    if ( iPe < nPartEvents ) {
      // Correct the range to account for the holes and gaps (TODO better)
      int lPlane = track.GetEMRTrackPointArray().back().GetChannel()/_number_of_bars;
      xRange -= lPlane*_geoMap.Gap();
      xRange *= (1-_hole_fraction);

      double xMomentum = TrackMomentum::csda_momentum(xRange, "muon");
      double xMomentumError = TrackMomentum::csda_momentum_error(xMomentum, xRangeError, "muon");
      track.SetMomentum(xMomentum);
      track.SetMomentumError(xMomentumError);
    }

    evtTrack->SetEMRTrack(track);
  }
}

void MapCppEMRRecon::match_daughters(MAUS::Spill* spill,
				     size_t nPartEvents) const {

  // Retain the index of the last candidate examinated to not go over them twice
  size_t cPe = 0;

  // Loop over the mothers and try to match them with their daughter
  size_t nSeconPartEvents = spill->GetEMRSpillData()->GetEMREventTrackArraySize();
  for (size_t aPe = 0; aPe < nPartEvents; aPe++) {

    // Skip events that did not reconstruct a track (no trackpoints)
    EMREvent* evt = spill->GetReconEvents()->at(aPe)->GetEMREvent();
    EMREventTrack* evtTrackM = evt->GetMotherPtr();
    if ( !evtTrackM )
	continue;
    if ( !evtTrackM->GetEMRTrack().GetEMRTrackPointArraySize() )
	continue;

    std::cerr << std::endl;
    std::cerr << "Mother in the EMR" << std::endl;

    double aTime = evtTrackM->GetEMRSpacePointArray().front()->GetTime();
    EMRTrack aTrack = evtTrackM->GetEMRTrack();
    ThreeVector xPoint = aTrack.GetEMRTrackPointArray().back().GetPosition();

    int xPe(-1);
    double xTime(-1.);
    double xDist(9999);
    for (size_t bPe = cPe; bPe < nSeconPartEvents; bPe++) {

      // Skip candidates that did not reconstruct a track (no trackpoints)
      EMREventTrack* evtTrackC = spill->GetEMRSpillData()->GetEMREventTrackArray()[bPe];
      if ( !evtTrackC->GetEMRTrack().GetEMRTrackPointArraySize() )
	  continue;

      double bTime = evtTrackC->GetEMRSpacePointArray().front()->GetTime();
      EMRTrack bTrack = evtTrackC->GetEMRTrack();

      if ( bTime < aTime ) 			// No decay before the mother particle
          continue;
      if ( (bTime - aTime) > _max_time )  {	// Skip unreasonable time difference
	cPe = bPe;
	break;
      }

      ThreeVector fPoint = bTrack.GetEMRTrackPointArray().front().GetPosition();
      ThreeVector lPoint = bTrack.GetEMRTrackPointArray().back().GetPosition();

      double fDist = sqrt(pow(fPoint.x()-xPoint.x(), 2)+
    			  pow(fPoint.y()-xPoint.y(), 2)+
			  pow(fPoint.z()-xPoint.z(), 2));
      double lDist = sqrt(pow(lPoint.x()-xPoint.x(), 2)+
    			  pow(lPoint.y()-xPoint.y(), 2)+
			  pow(lPoint.z()-xPoint.z(), 2));

      if ( fDist < xDist || lDist < xDist ) {
        xPe = bPe;
	xTime = (bTime - aTime)*_dbb_count;	// [ns]
        xDist = std::min(fDist, lDist);		// [mm]
      }
    }

    // If a daughter is found, deep copy the candidate into the EMREvent
    if ( xDist < _max_distance ) {
//      std::cerr << "Decay found" << std::endl;
//      std::cerr << "xPe: " << xPe << std::endl;
//      std::cerr << "xTime: " << xTime << " ns" << std::endl;
//      std::cerr << "xDist: " << xDist << " mm" << std::endl;

      EMREventTrack* evtTrackX = spill->GetEMRSpillData()->GetEMREventTrackArray()[xPe];
      EMREventTrack* evtTrackD = new EMREventTrack;

      // Deep copy all the plane hits
      for (size_t i = 0; i < evtTrackX->GetEMRPlaneHitArraySize(); i++) {
	EMRPlaneHit* planeHit = new EMRPlaneHit;
        planeHit->SetEMRBarHitArray(evtTrackX->GetEMRPlaneHitArray()[i]->GetEMRBarHitArray());
        planeHit->SetPlane(evtTrackX->GetEMRPlaneHitArray()[i]->GetPlane());
        planeHit->SetOrientation(evtTrackX->GetEMRPlaneHitArray()[i]->GetOrientation());
        planeHit->SetTime(evtTrackX->GetEMRPlaneHitArray()[i]->GetTime());
        planeHit->SetDeltaT(evtTrackX->GetEMRPlaneHitArray()[i]->GetDeltaT());
        planeHit->SetCharge(evtTrackX->GetEMRPlaneHitArray()[i]->GetCharge());
        planeHit->SetPedestalArea(evtTrackX->GetEMRPlaneHitArray()[i]->GetPedestalArea());
        planeHit->SetSampleArray(evtTrackX->GetEMRPlaneHitArray()[i]->GetSampleArray());
	evtTrackD->AddEMRPlaneHit(planeHit);
      }

      // Deep copy all the space points
      for (size_t i = 0; i < evtTrackX->GetEMRSpacePointArraySize(); i++) {
	EMRSpacePoint* spacePoint = new EMRSpacePoint;
	spacePoint->SetPosition(evtTrackX->GetEMRSpacePointArray()[i]->GetPosition());
	spacePoint->SetGlobalPosition(evtTrackX->GetEMRSpacePointArray()[i]->GetGlobalPosition());
	spacePoint->SetPositionErrors(evtTrackX->GetEMRSpacePointArray()[i]->GetPositionErrors());
	spacePoint->SetChannel(evtTrackX->GetEMRSpacePointArray()[i]->GetChannel());
	spacePoint->SetTime(evtTrackX->GetEMRSpacePointArray()[i]->GetTime());
	spacePoint->SetDeltaT(evtTrackX->GetEMRSpacePointArray()[i]->GetDeltaT());
	spacePoint->SetChargeMA(evtTrackX->GetEMRSpacePointArray()[i]->GetChargeMA());
	spacePoint->SetChargeSA(evtTrackX->GetEMRSpacePointArray()[i]->GetChargeSA());
	evtTrackD->AddEMRSpacePoint(spacePoint);
      }

      // Deep copy all the PID variables
      evtTrackD->SetType("daughter");
      evtTrackD->SetTrackId(0);
      evtTrackD->SetTime(evtTrackX->GetTime());
      evtTrackD->SetPlaneDensityMA(evtTrackX->GetPlaneDensityMA());
      evtTrackD->SetPlaneDensitySA(evtTrackX->GetPlaneDensitySA());
      evtTrackD->SetTotalChargeMA(evtTrackX->GetTotalChargeMA());
      evtTrackD->SetTotalChargeSA(evtTrackX->GetTotalChargeSA());
      evtTrackD->SetChargeRatioMA(evtTrackX->GetChargeRatioMA());
      evtTrackD->SetChargeRatioSA(evtTrackX->GetChargeRatioSA());

      // Reconstruct the Michel electron/positron momentum (TODO beam polarity)
      EMRTrack track = evtTrackX->GetEMRTrack();
      double xMomentum = TrackMomentum::csda_momentum(track.GetRange(), "electron");
      double xMomentumError = TrackMomentum::csda_momentum_error(track.GetRange(),
						  		 track.GetRangeError(),
						   		 "electron");
      track.SetMomentum(xMomentum);
      track.SetMomentumError(xMomentumError);
      evtTrackD->SetEMRTrack(track);

      evt->AddEMREventTrack(evtTrackD);
    } else {
//      std::cerr << "NO DECAY FOUND" << std::endl;
//      std::cerr << "Closest candidate" << std::endl;
//      std::cerr << "xPe: " << xPe << std::endl;
//      std::cerr << "xTime: " << xTime << " ns" << std::endl;
//      std::cerr << "xDist: " << xDist << " mm" << std::endl;
    }
  }
}
}
