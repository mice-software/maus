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
#include "Interface/Squeak.hh"

//////////////////////////////////////////////////////////////////////
bool MapCppTOFMCDigitizer::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTOFMCDigitizer";

  // print level
  fDebug = false;

  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  // get the geometry
  assert(_configJSON.isMember("reconstruction_geometry_filename"));
  std::string filename;
  filename = _configJSON["reconstruction_geometry_filename"].asString();
  // get the tof geometry modules
  geo_module = new MiceModule(filename);
  tof_modules = geo_module->findModulesByPropertyString("SensitiveDetector", "TOF");

  _stationKeys.push_back("tof0");
  _stationKeys.push_back("tof1");
  _stationKeys.push_back("tof2");

  // load the TOF calibration map
  bool loaded = _map.InitializeFromCards(_configJSON);
  if (!loaded)
     return false;

  return true;
}

//////////////////////////////////////////////////////////////////////
bool MapCppTOFMCDigitizer::death() {
  return true;
}

//////////////////////////////////////////////////////////////////////
std::string MapCppTOFMCDigitizer::process(std::string document) {

  Json::FastWriter writer;

  // check sanity of json input file and mc brach
  if ( !check_sanity_mc(document) ) {
    // if bad, write error file
    return writer.write(root);
  }

  // all_tof_digits store all the tof hits
  // then we'll weed out multiple hits, add up yields, and store the event
  std::vector<Json::Value> all_tof_digits;
  all_tof_digits.push_back(Json::Value());
  Json::Value tof_evt;

  // loop over events
  if (fDebug) std::cout << "mc numevts = " << mc.size() << std::endl;
  for ( unsigned int i = 0; i < mc.size(); i++ ) {
    Json::Value particle = mc[i];
    gentime = particle["primary"]["time"].asDouble();
    if (fDebug) std::cout << "evt: " << i << " pcle= " << particle << std::endl;

    // hits for this event
    Json::Value _hits = particle["tof_hits"];
    // if _hits.size() = 0, the make_digits and fill_tof_evt functions will
    // return null. The TOF recon expects something - either null or data, can't just skip
    all_tof_digits.clear();
    strig.str(std::string());

    // pick out tof hits, digitize and store
    all_tof_digits = make_tof_digits(_hits);

    // go through tof hits and find the hit in the trig station (TOF1)
    // if (all_tof_digits.size() != 0) findTriggerPixel(all_tof_digits);
    findTriggerPixel(all_tof_digits);

    // now go through the stations and fill tof events
    // real data tof digits look like so:
    // "digits":{"tof1":[ [evt0..{pmt0},{pmt1}..],[evt1..]],"tof0":[[evt0]..],tof2..}
    // loop over tof stations
    for (int snum = 0; snum < 3; ++snum) {
       tof_evt.clear();
       tof_evt[i] = fill_tof_evt(i, snum, all_tof_digits);
       if (fDebug) {
          std::cout << "mcevt: " << i << " tof" << snum << " " << _hits.size()
                    << " hits, " << all_tof_digits.size() << " digits" << std::endl;
       }
       //root["digits"][_stationKeys[snum]].append(tof_evt[i]);
       Json::Value tof_digs = fill_tof_evt(i, snum, all_tof_digits);
       root["recon_events"][i]["tof_event"]["tof_digits"][_stationKeys[snum]]
                                                                     = tof_digs;
     } // end loop over stations
  } // end loop over events
  // write it out
  return writer.write(root);
}

//////////////////////////////////////////////////////////////////////
std::vector<Json::Value> MapCppTOFMCDigitizer::make_tof_digits(Json::Value hits) {
  std::vector<Json::Value> tof_digits;
  tof_digits.clear();
  if (hits.size() == 0) return tof_digits;

  for ( unsigned int j = 0; j < hits.size(); j++ ) {  //  j-th hit
      Json::Value hit = hits[j];
      if (fDebug) std::cout << "hit# " << j << hit << std::endl;

      // make sure we can get the station/slab info
      assert(hit.isMember("channel_id"));
      assert(hit.isMember("momentum"));
      assert(hit.isMember("time"));
      Json::Value channel_id = hit["channel_id"];

      assert(hit.isMember("energy_deposited"));
      double edep = hit["energy_deposited"].asDouble();

      if (fDebug) {
         std::cout << "tofhit: " << hit["channel_id"] << " "
                   << hit["position"] << " " << hit["momentum"] << " " << hit["time"] << std::endl;
      }

      int stn = hit["channel_id"]["station_number"].asInt();
      int pln = hit["channel_id"]["plane"].asInt();
      int slb = hit["channel_id"]["slab"].asInt();

      // set daq_event_type. TofSlabHits expects this.
      // NOTE: In principle, should be done globally
      if (!root.isMember("daq_event_type")) {
        root["daq_event_type"] = "physics_event";
      }

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
      assert(hit_module != NULL);

      // now get the position of the hit
      Hep3Vector hpos = JsonWrapper::JsonToThreeVector(hit["position"]);

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
      double npe1 = get_npe(dist1, edep);
      double npe2 = get_npe(dist2, edep);

      // get the hit time
      double csp = _configJSON["TOFscintLightSpeed"].asDouble();
      double tres = _configJSON["TOFpmtTimeResolution"].asDouble();
      double htime = hit["time"].asDouble() - gentime;

      // propagate time to pmt & smear by the resolution
      double time1 = CLHEP::RandGauss::shoot((htime + dist1/csp) , tres);
      double time2 = CLHEP::RandGauss::shoot((htime + dist2/csp) , tres);
      double tdc2time = _configJSON["TOFtdcConversionFactor"].asDouble();

      // convert to tdc
      int tdc1 = static_cast<int>(time1 / tdc2time);
      int tdc2 = static_cast<int>(time2 / tdc2time);
      if (fDebug) {
         std::cout << "time: " << hit["time"].asDouble() << " now: " <<
	           time1 << " " << time2 << " " << (time1+time2)/2 << std::endl;
         std::cout << "dist: " << dist1 << " " << dist2 << std::endl;
         std::cout << "tdc: " << tdc1 << " " << tdc2 << std::endl;
      }

      Json::Value tmpDigit;
      // set the hits for PMTs at both ends
      for (int p = 0; p < 2; ++p) {
        tmpDigit.clear();
        tmpDigit["channel_id"] = hit["channel_id"];
        tmpDigit["mc_mom"] = hit["momentum"];
        tmpDigit["time"] = hit["time"];
        tmpDigit["mc_position"]=hit["position"];
        tmpDigit["isUsed"] = 0;

	// set the key for this channel
        std::stringstream ss;
        ss << "TOFChannelKey " << stn << " " << pln << " " << slb << " " << p << " tof" << stn;
        tmpDigit["tof_key"] = ss.str();
        tmpDigit["station"] = stn;
        tmpDigit["plane"] = pln;
        tmpDigit["slab"] = slb;
        tmpDigit["pmt"] = p;
        if (p == 0) {
	  tmpDigit["npe"] = npe1;
          tmpDigit["leading_time"] = tdc1;
          tmpDigit["raw_time"] = time1;
        } else if (p == 1) {
	  tmpDigit["npe"] = npe2;
          tmpDigit["leading_time"] = tdc2;
          tmpDigit["raw_time"] = time2;
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
        tmpDigit["trailing_time"] = 0;
        tmpDigit["trigger_request_leading_time"] = 0;
        tmpDigit["trigger_time_tag"] = 0;
	// can't put a time_t into json???
	// std::time_t tstamp = std::time(0);
        tmpDigit["time_stamp"] = 0;
	if (fDebug)
          std::cout << "digit " << j << " key: " << ss.str() << std::endl;

        tof_digits.push_back(tmpDigit);
      } // end loop over pmts
    }  //  ends 'for' loop over hits

    return tof_digits;
}

//////////////////////////////////////////////////////////////////////
bool MapCppTOFMCDigitizer::check_sanity_mc(std::string document) {
  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(document, root);
  if (!parsingSuccessful) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return false;
  }

  // Check if the JSON document has a 'mc' branch, else return error
  if (!root.isMember("mc_events")) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "I need an MC branch to simulate.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
    return false;
  }

  mc = root.get("mc_events", 0);
  // Check if JSON document is of the right type, else return error
  if (!mc.isArray()) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "MC branch isn't an array";
    errors["bad_type"] = ss.str();
    root["errors"] = errors;
    return false;
  }
  return true;
}

//////////////////////////////////////////////////////////////////////
double MapCppTOFMCDigitizer::get_npe(double edep, double dist) {
      double peRes = 1e-4;
      double nphot = 0;
      assert(_configJSON.isMember("TOFattenuationLength"));
      assert(_configJSON.isMember("TOFpmtQuantumEfficiency"));

      assert(_configJSON.isMember("TOFconversionFactor"));
      nphot = edep / (_configJSON["TOFconversionFactor"].asDouble());

      nphot *= exp(-dist / (_configJSON["TOFattenuationLength"].asDouble()));
      nphot *= (_configJSON["TOFpmtQuantumEfficiency"].asDouble());
      nphot = CLHEP::RandGauss::shoot(nphot, peRes);
      // cannot convert to adc yet -- need to add up yields from multi hits in bar
      // take care of adc conversion after weeding out duplicate/multi hits
      // int adc = (int)( nPE / (_configJSON.["TOFadcConversionFactor"].asDouble()) );
      return nphot;
}

//////////////////////////////////////////////////////////////////////
void MapCppTOFMCDigitizer::findTriggerPixel(std::vector<Json::Value> all_tof_digits) {
  int tstn = 1;
  int tsx = 99, tsy = 99;

  if (fDebug) std::cout << "adigsize= " << all_tof_digits.size() << std::endl;
  for (unsigned int digit_i = 0; digit_i < all_tof_digits.size(); digit_i++) {
    if (all_tof_digits[digit_i]["station"] == tstn) {
      if (fDebug)
         std::cout << "found trig stn " << tstn << " " << tsx << " " << tsy << std::endl;
      if (tsx == 99 || tsy == 99) {
        if (all_tof_digits[digit_i]["plane"].asInt() == 0)
           tsx = all_tof_digits[digit_i]["slab"].asInt();
        else
           tsy = all_tof_digits[digit_i]["slab"].asInt();
      }
    }
  }
  // set trigger to be "unknown" if we could not find the hit in TOF1
  if (tsx == 99 || tsy == 99)
     strig << "unknown";
  else
     strig << "TOFPixelKey " << tstn << " " << tsx << " " << tsy << " tof" << tstn;
  if (fDebug)
     std::cout << "pixelKey: " << strig.str() << std::endl;
}


//////////////////////////////////////////////////////////////////////
Json::Value MapCppTOFMCDigitizer::fill_tof_evt(int evnum, int snum,
                                             std::vector<Json::Value> all_tof_digits) {
  Json::Value tof_digit(Json::arrayValue);
  // return null if this evt had no tof hits
  if (all_tof_digits.size() == 0) return tof_digit;
  double npe;
  int ndigs = 0;

  // throw out multihits and add up the light yields from multihits in slabs
  for (unsigned int i = 0; i < all_tof_digits.size(); ++i) {
    // check that this digit belongs to the station we are trying to fill
    if (all_tof_digits[i]["station"].asInt() != snum) continue;

    // check if the digit has been used
    if ( all_tof_digits[i]["isUsed"].asInt() == 0 ) {

      ndigs++;

      npe = all_tof_digits[i]["npe"].asDouble();
      // loop over all the other digits to find multihit slabs
      for ( unsigned int j = i; j < all_tof_digits.size(); j++ ) {
        if ( check_param(&(all_tof_digits[i]), &(all_tof_digits[j])) ) {
            // add up light yields if same bar was hit
            npe += all_tof_digits[j]["npe"].asDouble();
            // mark this hit as used so we don't multicount
            all_tof_digits[j]["isUsed"]=1;
        } // end check for used
      } // end loop over secondary digits

      // convert light yield to adc & set the charge
      int adc = static_cast<int>(npe / (_configJSON["TOFadcConversionFactor"].asDouble()));
      // ROGERS = changed from "charge" to "charge_pm" for data integrity
      digit["charge_pm"] = adc;
      // NOTE: needs tweaking/verifying -- DR 3/15
      digit["charge_mm"] = adc;
      digit["tof_key"] = all_tof_digits[i]["tof_key"].asString();
      digit["station"] = all_tof_digits[i]["station"].asInt();
      digit["plane"] = all_tof_digits[i]["plane"].asInt();
      digit["slab"] = all_tof_digits[i]["slab"].asInt();
      digit["pmt"] = all_tof_digits[i]["pmt"].asInt();

      // add the calibration corrections to the time
      std::string keyStr = all_tof_digits[i]["tof_key"].asString();
      TOFChannelKey xChannelKey(keyStr);
      double dT = 0.;
      std::string pixStr = strig.str();
      if (pixStr != "unknown") {
        TOFPixelKey xTriggerPixelKey(pixStr);
        dT = _map.dT(xChannelKey, xTriggerPixelKey, adc);
      }
      if (fDebug)
         std::cout << "calibCorr: " << dT << std::endl;
      double raw = all_tof_digits[i]["raw_time"].asDouble();
      double rt = raw + dT;

      // convert to tdc
      int tdc = static_cast<int>(rt/(_configJSON["TOFtdcConversionFactor"].asDouble()));
      if (fDebug) {
         std::cout << "times: raw=  " << raw << " dT= " << dT
                   << " corr= " << rt << " tdc= " << tdc << std::endl;
      }

      // store the digitized times
      // NOTE: trigger_req... has to be tweaked -- see earlier note -- DR/march15
      digit["leading_time"] = tdc;
      digit["trigger_request_leading_time"] =
                         all_tof_digits[i]["trigger_request_leading_time"].asInt();
      // ROGERS addition to maintain data integrity 03-May-2012
      //  - trigger_leading_time 
      //  - trigger_trailing_time 
      //  - trigger_request_trailing_timr
      digit["trigger_leading_time"] = 0;
      digit["trigger_trailing_time"] = 0;
      digit["trigger_request_trailing_time"] = 0;
      digit["trailing_time"] = 0;
      digit["time_stamp"] = 0;
      digit["trigger_time_tag"] = 0;

      // store event number
      digit["phys_event_number"] = evnum;
      digit["part_event_number"] = evnum;

      tof_digit.append(digit);
      if (fDebug)
          std::cout << "digit #" << ndigs << " " <<  digit["tof_key"].asString() << std::endl;
      all_tof_digits[i]["isUsed"]=1;
    } // ends if-statement
  } // ends k-loop
  return tof_digit;
}

//////////////////////////////////////////////////////////////////////
bool MapCppTOFMCDigitizer::check_param(Json::Value* hit1, Json::Value* hit2) {
  int station1   = (*hit1)["station"].asInt();
  int station2   = (*hit2)["station"].asInt();
  int plane1 = (*hit1)["plane"].asInt();
  int plane2 = (*hit2)["plane"].asInt();
  int slab1 = (*hit1)["slab"].asInt();
  int slab2 = (*hit2)["slab"].asInt();
  int pmt1     = (*hit1)["pmt"].asInt();
  int pmt2     = (*hit2)["pmt"].asInt();
  int hit_is_used = (*hit2)["isUsed"].asInt();

  if ( station1 == station2 && plane1 == plane2 &&
       slab1 == slab2 && pmt1 == pmt2 && !hit_is_used ) {
    return true;
  } else {
    return false;
  }
}
//=====================================================================
