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

#include "src/map/MapCppTOFMCDigitizer/MapCppTOFMCDigitizer.hh"
#include <string>
#include "src/common_cpp/Utils/TOFChannelMap.hh"
#include "Config/MiceModule.hh"
#include "DataStructure/MCEvent.hh"
#include "DataStructure/Spill.hh"
#include "Utils/Exception.hh"
#include "API/PyWrapMapBase.hh"
#include "src/common_cpp/DataStructure/RunHeaderData.hh"
#include "src/common_cpp/DataStructure/RunHeader.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTOFMCDigitizer(void) {
  PyWrapMapBase<MAUS::MapCppTOFMCDigitizer>::PyWrapMapBaseModInit
                                      ("MapCppTOFMCDigitizer", "", "", "", "");
}

MapCppTOFMCDigitizer::MapCppTOFMCDigitizer()
    : MapBase<Data>("MapCppTOFMCDigitizer") {
}

//////////////////////////////////////////////////////////////////////
void MapCppTOFMCDigitizer::_birth(const std::string& argJsonConfigDocument) {
  // print level
  fDebug = false;

  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                          "Failed to parse Json Configuration",
                          "MapCppTOFMCDigitizer::_birth");
  }

  // get the geometry
  if (!_configJSON.isMember("reconstruction_geometry_filename"))
      throw(Exceptions::Exception(Exceptions::recoverable,
                   "Could not find geometry file",
                   "MapCppTOFMCDigitizer::birth"));
  std::string filename;
  filename = _configJSON["reconstruction_geometry_filename"].asString();
  // get the tof geometry modules
  geo_module = new MiceModule(filename);
  tof_modules = geo_module->findModulesByPropertyString("SensitiveDetector", "TOF");

  // load the TOF calibration map
  bool loaded = _map.InitializeFromCards(_configJSON, 0);
  if (!loaded)
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Could not find TOF calibration map",
                 "MapCppTOFMCDigitizer::birth"));
}

//////////////////////////////////////////////////////////////////////
void MapCppTOFMCDigitizer::_death() {
}

//////////////////////////////////////////////////////////////////////
void MapCppTOFMCDigitizer::_process(MAUS::Data* data) const {

  // Get spill, break if there's no DAQ data
  Spill *spill = data->GetSpill();
  if (spill->GetMCEvents() == NULL)
      return;

  MCEventPArray *mcEvts = spill->GetMCEvents();
  int nPartEvents = spill->GetMCEventSize();
  if (nPartEvents == 0)
      return;

  ReconEventPArray *recEvts =  spill->GetReconEvents();

  // Resize the recon event to hold mc events
  if (spill->GetReconEventSize() == 0) { // No recEvts yet
      for (int iPe = 0; iPe < nPartEvents; iPe++) {
          recEvts->push_back(new ReconEvent);
      }
  }

  // all_tof_digits store all the tof hits
  // then we'll weed out multiple hits, add up yields, and store the event

  // loop over events
  if (fDebug) std::cout << "mc numevts = " << mcEvts->size() << std::endl;

  // Construct digits from hits for each MC event
  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    // Get tof hits for this event
    TOFHitArray *hits = spill->GetAnMCEvent(iPe).GetTOFHits();

    // process only if there are tof hits
    if (hits) {
      // Get the time of the primary
      // for digitizing, hit time will be taken as relative to the primary
      Primary *primary = spill->GetAnMCEvent(iPe).GetPrimary();
      double gentime = primary->GetTime(); // ns

      // pick out tof hits, digitize and store
      TofTmpDigits tmpDigits = make_tof_digits(hits, gentime);

      // go through tof hits and find the hit in the trig station (TOF1)
      std::string strig = findTriggerPixel(tmpDigits);

      // now go through the stations and fill tof events
      // real data tof digits look like so:
      // "digits":{"tof1":[ [evt0..{pmt0},{pmt1}..],[evt1..]],"tof0":[[evt0]..],tof2..}
      TOFEvent *evt = new TOFEvent();
      (*recEvts)[iPe]->SetTOFEvent(evt);

      fDigsArrayPtr digitArrayPtr(0);
      TOF0DigitArray *tof0_digits = evt->GetTOFEventDigitPtr()->GetTOF0DigitArrayPtr();
      TOF1DigitArray *tof1_digits = evt->GetTOFEventDigitPtr()->GetTOF1DigitArrayPtr();
      TOF2DigitArray *tof2_digits = evt->GetTOFEventDigitPtr()->GetTOF2DigitArrayPtr();
      digitArrayPtr.push_back(tof0_digits);
      digitArrayPtr.push_back(tof1_digits);
      digitArrayPtr.push_back(tof2_digits);
      fill_tof_evt(iPe, 0, tmpDigits, digitArrayPtr, strig);
    }
  } // end loop over events
}

//////////////////////////////////////////////////////////////////////
TofTmpDigits MapCppTOFMCDigitizer::make_tof_digits(TOFHitArray* hits,
                                                   double gentime) const {
  TofTmpDigits tmpDigits(0);
  if (hits->size() == 0) return tmpDigits;

  for ( unsigned int j = 0; j < hits->size(); j++ ) {  //  j-th hit
      TOFHit hit = hits->at(j);
      if (fDebug) std::cout << "=================== hit# " << j << std::endl;

      // make sure we can get the station/slab info
      if (!hit.GetChannelId())
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "No channel_id in hit",
                       "MapCppTOFMCDigitizer::make_tof_digits"));
      ThreeVector mom = hit.GetMomentum();
      double hTime = hit.GetTime();
      TOFChannelId* channel_id = hit.GetChannelId();

      double edep = hit.GetEnergyDeposited();

      if (fDebug) {
         std::cout << "tofhit: stn: " << channel_id->GetStation() << " slab: "
                   << channel_id->GetSlab() << " plane: " << channel_id->GetPlane()
                   << " xyz: " << hit.GetPosition().x() << " "
                   << hit.GetPosition().y() << " " << hit.GetPosition().z()
                   << " px,py,pz: " << hit.GetMomentum().x() << " "
                   << hit.GetMomentum().y() << " " << hit.GetMomentum().z()
                   << " time: " << hit.GetTime() << std::endl;
      }

      int stn = channel_id->GetStation();
      int pln = channel_id->GetPlane();
      int slb = channel_id->GetSlab();

      // set daq_event_type. TofSlabHits expects this.
      // NOTE: In principle, should be done globally
/*
      if (!root.isMember("daq_event_type")) {
        root["daq_event_type"] = "physics_event";
      }
*/
      // find the geo module corresponding to this hit
      const MiceModule* hit_module = NULL;
      for ( unsigned int jj = 0; !hit_module && jj < tof_modules.size(); ++jj ) {
        if ( tof_modules[jj]->propertyExists("Station", "int") &&
             tof_modules[jj]->propertyInt("Station") == stn &&
             tof_modules[jj]->propertyExists("Slab", "int") &&
             tof_modules[jj]->propertyInt("Slab") == slb &&
             tof_modules[jj]->propertyExists("Plane", "int") &&
             tof_modules[jj]->propertyInt("Plane") == pln ) {
               // got it
               hit_module = tof_modules[jj];
               if (fDebug) {
                 std::cout << "mod: " << jj << " " << tof_modules[jj]->propertyInt("Station")
	                   << " " << tof_modules[jj]->propertyInt("Plane") << " "
			   << tof_modules[jj]->propertyInt("Slab") << std::endl;
               }
        } // end check on module
      } // end loop over tof_modules

      // make sure we actually found a tof module corresponding to this hit
      if (hit_module == NULL)
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "No TOF module for hit",
                       "MapCppTOFMCDigitizer::make_tof_digits"));

      // now get the position of the hit
      ThreeVector tpos = hit.GetPosition();
      Hep3Vector hpos(tpos.x(), tpos.y(), tpos.z());

      // get the slab and pmt positions from the geometry
      // slab pos
      Hep3Vector slabPos = hit_module->globalPosition();
      // pmt positions
      std::string pmtName;
      // pmt 1
      pmtName = "Pmt1Pos";
      Hep3Vector pmt1Pos = hit_module->propertyHep3Vector(pmtName);
      // pmt 2
      pmtName = "Pmt2Pos";
      Hep3Vector pmt2Pos = hit_module->propertyHep3Vector(pmtName);
      if (fDebug) {
        std::cout << "hitpos: " << hpos << std::endl;
        std::cout << "pmt1pos: " << pmt1Pos << " slabPos: " << slabPos << std::endl;
        std::cout << "pmt2pos: " << pmt2Pos << " slabPos: " << slabPos << std::endl;
      }

      // find distances from hit to pmt geom
      double dist1 = ( hpos - (slabPos + pmt1Pos) ).mag();
      double dist2 = ( hpos - (slabPos + pmt2Pos) ).mag();

      // convert edep to photoelectrons for this slab/pmt
      // can't convert to adc yet since we need to add up ph.el's
      //   from other hits if any
      if (fDebug) std::cout << "edep= " << edep << std::endl;
      double npe1 = get_npe(dist1, edep);
      double npe2 = get_npe(dist2, edep);
      if (fDebug) printf("npe# %3.15f %3.4f %3.4f\n", edep, npe1, npe2);

      // get the hit time
      double csp = _configJSON["TOFscintLightSpeed"].asDouble();
      double tres = _configJSON["TOFpmtTimeResolution"].asDouble();
      double htime = hit.GetTime() - gentime;

      // propagate time to pmt & smear by the resolution
      double time1 = CLHEP::RandGauss::shoot((htime + dist1/csp) , tres);
      double time2 = CLHEP::RandGauss::shoot((htime + dist2/csp) , tres);
      double tdc2time = _configJSON["TOFtdcConversionFactor"].asDouble();

      // convert to tdc
      int tdc1 = static_cast<int>(time1 / tdc2time);
      int tdc2 = static_cast<int>(time2 / tdc2time);
      if (fDebug) {
         std::cout << "time: " << hit.GetTime() << " now: " <<
	           time1 << " " << time2 << " " << (time1+time2)/2 << std::endl;
         std::cout << "dist: " << dist1 << " " << dist2 << std::endl;
         std::cout << "tdc: " << tdc1 << " " << tdc2 << std::endl;
      }

      tmpThisDigit thisDigit;
      // set the hits for PMTs at both ends
      for (int p = 0; p < 2; ++p) {
        thisDigit.fChannelId = channel_id;
        thisDigit.fMom = mom;
        thisDigit.fTime = hTime;
        thisDigit.fPos = hpos;
        thisDigit.fIsUsed = 0;

	// set the key for this channel
        std::stringstream ss;
        ss << "TOFChannelKey " << stn << " " << pln << " " << slb << " " << p << " tof" << stn;
        thisDigit.fTofKey = ss.str();
        thisDigit.fStation = stn;
        thisDigit.fPlane = pln;
        thisDigit.fSlab = slb;
        thisDigit.fPmt = p;
        if (p == 0) {
          thisDigit.fNpe = npe1;
          thisDigit.fLeadingTime = tdc1;
          thisDigit.fRawTime = time1;
        } else if (p == 1) {
          thisDigit.fNpe = npe2;
          thisDigit.fLeadingTime = tdc2;
          thisDigit.fRawTime = time2;
        }
	// this stuff needs tweaking
        // need to make sure the trigger leading time is reasonable
        // as is this requires a mod --
        // --- ConfigDefaults triggerpixelcut has to be modified to
        //     allow for the trigger pixel to be found
        // ideally if all was well the triggerpixel after calib
        // will give a time ~= 0.
        // but without the correct trigger leading time, this will no
        // longer be the case. hence the pixelcut mod.
        // need a more elegant way to handle this -- DR 3/15
        thisDigit.fTrailingTime = 0;
        thisDigit.fTriggerRequestLeadingTime = 0;
        thisDigit.fTriggerTimeTag = 0;
        thisDigit.fTimeStamp = 0;
	if (fDebug)
          std::cout << "digit " << j << " key: " << ss.str() << std::endl;

        tmpDigits.push_back(thisDigit);
      } // end loop over pmts
    }  //  ends 'for' loop over hits

    return tmpDigits;
}

//////////////////////////////////////////////////////////////////////
double MapCppTOFMCDigitizer::get_npe(double dist, double edep) const {
      double nphot = 0;
      double nptmp = 0.;
      if (fDebug) std::cout << "get_npe::edep= " << edep << std::endl;

      if (!_configJSON.isMember("TOFattenuationLength"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                "Could not find TOFattenauationLength in config",
                "MapCppTOFMCDigitizer::get_npe"));
      if (!_configJSON.isMember("reconstruction_geometry_filename"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                "Could not find TOFpmtQuantumEfficiency in config",
                "MapCppTOFMCDigitizer::get_npe"));
      if (!_configJSON.isMember("TOFconversionFactor"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "Could not find TOFconversionFactor in config",
                       "MapCppTOFMCDigitizer::birth"));
      if (fDebug)
          printf("nphot0: %3.12f %3.12f\n", edep, _configJSON["TOFconversionFactor"].asDouble());

      // convert energy deposited to number of photoelectrons
      nphot = edep / (_configJSON["TOFconversionFactor"].asDouble());

      TRandom* rnd = new TRandom();
      rnd = new TRandom();
      nptmp = rnd->Poisson(nphot);
      nphot = nptmp;
      // attenuate the yield
      nphot *= exp(-dist / (_configJSON["TOFattenuationLength"].asDouble()));
      rnd = new TRandom();
      nptmp = rnd->Poisson(nphot);
      nphot = nptmp;
      // correct for phototube quantum efficiency
      nphot *= (_configJSON["TOFpmtQuantumEfficiency"].asDouble());
      rnd = new TRandom();
      nptmp = rnd->Poisson(nphot);
      nphot = nptmp;

      return nphot;
}

//////////////////////////////////////////////////////////////////////
std::string MapCppTOFMCDigitizer::findTriggerPixel(TofTmpDigits& tmpDigits) const {
  int tstn = 1;
  int tsx = 99, tsy = 99;

  for (unsigned int digit_i = 0; digit_i < tmpDigits.size(); digit_i++) {
    if (tmpDigits[digit_i].fStation == tstn) {
      if (tsx == 99 || tsy == 99) {
        if (tmpDigits[digit_i].fPlane == 0)
           tsx = tmpDigits[digit_i].fSlab;
        else
           tsy = tmpDigits[digit_i].fSlab;
      }
    }
  }

  std::stringstream strig_str("");
  // set trigger to be "unknown" if we could not find the hit in TOF1
  if (tsx == 99 || tsy == 99)
     strig_str << "unknown";
  else
     strig_str << "TOFPixelKey " << tstn << " " << tsx << " " << tsy << " tof" << tstn;
  if (fDebug)
     std::cout << "pixelKey: " << strig_str.str() << std::endl;
  return strig_str.str();
}


//////////////////////////////////////////////////////////////////////
void MapCppTOFMCDigitizer::fill_tof_evt(int evnum, int snum,
                                             TofTmpDigits& tmpDigits,
                                             fDigsArrayPtr& tofDigits,
                                             std::string pixStr) const {
  // return null if this evt had no tof hits
  if (tmpDigits.size() == 0) return;
  double npe;
  int ndigs = 0;

  // throw out multihits and add up the light yields from multihits in slabs
  for (unsigned int i = 0; i < tmpDigits.size(); ++i) {
    int thisStation = tmpDigits[i].fStation;
    // check if the digit has been used
    if (!tmpDigits[i].fIsUsed) {

      ndigs++;

      npe = tmpDigits[i].fNpe;
      // loop over all the other digits to find multihit slabs
      for ( unsigned int j = i+1; j < tmpDigits.size(); j++ ) {
        if ( check_param(tmpDigits[i], tmpDigits[j]) ) {
            // add up light yields if same bar was hit
            npe += tmpDigits[j].fNpe;
            // mark this hit as used so we don't multicount
            tmpDigits[j].fIsUsed = true;
        } // end check for used
      } // end loop over secondary digits

      // convert light yield to adc & set the charge
      int adc = static_cast<int>(npe / (_configJSON["TOFadcConversionFactor"].asDouble()));
/*    
      double adcs = npe / (_configJSON["TOFadcConversionFactor"].asDouble());
      double adcw = 0.5;
      TRandom* rnd = new TRandom();
      double adcsm = rnd->Landau(adcs,adcw);
      int adc = static_cast<int>(adcsm);
*/    
      if (fDebug) std::cout << "npe-adc: " << npe << " " << adc << std::endl;

      // now set the digit
      TOFDigit theDigit;

      theDigit.SetChargePm(adc);
      // NOTE: needs tweaking/verifying -- DR 3/15
      theDigit.SetChargeMm(adc);
      theDigit.SetTofKey(tmpDigits[i].fTofKey);
      theDigit.SetStation(tmpDigits[i].fStation);
      theDigit.SetPlane(tmpDigits[i].fPlane);
      theDigit.SetSlab(tmpDigits[i].fSlab);
      theDigit.SetPmt(tmpDigits[i].fPmt);

      // add the calibration corrections to the time
      std::string keyStr = tmpDigits[i].fTofKey;
      TOFChannelKey xChannelKey(keyStr);
      double dT = 0.;
      if (pixStr != "unknown") {
        if (fDebug) std::cout << "MapCppTOFMCDigitizer::Correcting pixel: " << keyStr << std::endl;
        TOFPixelKey xTriggerPixelKey(pixStr);
        dT = _map.dT(xChannelKey, xTriggerPixelKey, adc);
        if (fDebug) std::cout << "dT= " << dT << std::endl;
      }
      if (fDebug)
         std::cout << "calibCorr: " << dT << std::endl;
      double raw = tmpDigits[i].fRawTime;
      double rt = raw + dT;

      // convert to tdc
      int tdc = static_cast<int>(rt/(_configJSON["TOFtdcConversionFactor"].asDouble()));
      if (fDebug) {
         std::cout << "times: raw=  " << raw << " dT= " << dT
                   << " corr= " << rt << " tdc= " << tdc << std::endl;
      }

      // store the digitized times
      // NOTE: trigger_req... has to be tweaked -- see earlier note -- DR/march15
      theDigit.SetLeadingTime(tdc);
      theDigit.SetTriggerRequestLeadingTime(tmpDigits[i].fTriggerRequestLeadingTime);
      // ROGERS addition to maintain data integrity 03-May-2012
      //  - trigger_leading_time
      //  - trigger_trailing_time
      //  - trigger_request_trailing_timr
      theDigit.SetTriggerLeadingTime(0);
      theDigit.SetTriggerTrailingTime(0);
      theDigit.SetTriggerRequestTrailingTime(0);
      theDigit.SetTrailingTime(0);
      theDigit.SetTimeStamp(0);
      theDigit.SetTriggerTimeTag(0);

      // store event number
      theDigit.SetPhysEventNumber(evnum);
      theDigit.SetPartEventNumber(evnum);

      // store this digit in the digits array for this station
      tofDigits[thisStation]->push_back(theDigit);

      if (fDebug)
          std::cout << "digit #" << ndigs << " " <<  theDigit.GetTofKey() << std::endl;

      // done - mark digit as used
      tmpDigits[i].fIsUsed = true;
    } // ends check of used hit
  } // ends loop over all digits
}

//////////////////////////////////////////////////////////////////////
bool MapCppTOFMCDigitizer::check_param(tmpThisDigit& hit1, tmpThisDigit& hit2) const {
  int station1   = hit1.fStation;
  int station2   = hit2.fStation;
  int plane1 = hit1.fPlane;
  int plane2 = hit2.fPlane;
  int slab1 = hit1.fSlab;
  int slab2 = hit2.fSlab;
  int pmt1     = hit1.fPmt;
  int pmt2     = hit2.fPmt;
  bool hit_is_used = hit2.fIsUsed;

  if ( station1 == station2 && plane1 == plane2 &&
       slab1 == slab2 && pmt1 == pmt2 && !hit_is_used ) {
    return true;
  } else {
    return false;
  }
}
//=====================================================================
}
