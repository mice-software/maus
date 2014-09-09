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

#include "src/map/MapCppKLMCDigitizer/MapCppKLMCDigitizer.hh"
#include <string>
#include "src/common_cpp/Utils/KLChannelMap.hh"
#include "Config/MiceModule.hh"
#include "Utils/Exception.hh"
#include "API/PyWrapMapBase.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppKLMCDigitizer(void) {
  PyWrapMapBase<MAUS::MapCppKLMCDigitizer>::PyWrapMapBaseModInit
                                        ("MapCppKLMCDigitizer", "", "", "", "");
}

MapCppKLMCDigitizer::MapCppKLMCDigitizer()
  : MapBase<Json::Value>("MapCppKLMCDigitizer") {
}

//////////////////////////////////////////////////////////////////////
void MapCppKLMCDigitizer::_birth(const std::string& argJsonConfigDocument) {
  // print level
  fDebug = false;


  // Check if the JSON document can be parsed, else return error only
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    throw MAUS::Exception(Exception::recoverable,
                          "Failed to parse Json Configuration",
                          "MapCppTOFMCDigitizer::_birth");
  }
  // get the geometry
  if (!_configJSON.isMember("simulation_geometry_filename"))
      throw(Exception(Exception::recoverable,
                   "Could not find geometry file",
                   "MapCppKLMCDigitizer::birth"));
  std::string filename;
  filename = _configJSON["simulation_geometry_filename"].asString();
  // get the kl geometry modules
  geo_module = new MiceModule(filename);
  kl_modules = geo_module->findModulesByPropertyString("SensitiveDetector", "KL");
}

//////////////////////////////////////////////////////////////////////
void MapCppKLMCDigitizer::_death() {
}

//////////////////////////////////////////////////////////////////////
void MapCppKLMCDigitizer::_process(Json::Value* data) const {
  Json::Value& root = *data;
  Json::Value mc = check_sanity_mc(root);

  // all_kl_digits store all the kl hits
  // then we'll weed out multiple hits, add up yields, and store the event
  std::vector<Json::Value> all_kl_digits;
  all_kl_digits.push_back(Json::Value());
  Json::Value kl_evt;

  // loop over events
  if (fDebug) std::cout << "mc numevts = " << mc.size() << std::endl;
  for ( unsigned int i = 0; i < mc.size(); i++ ) {
    Json::Value particle = mc[i];
    if (fDebug) std::cout << "evt: " << i << " particle= " << particle << std::endl;

    // hits for this event
    Json::Value _hits = particle["kl_hits"];
    // if _hits.size() = 0, the make_digits and fill_kl_evt functions will
    // return null.
    all_kl_digits.clear();

    // pick out kl hits, digitize and store
    all_kl_digits = make_kl_digits(_hits, root);


    // fill kl events
    kl_evt.clear();
    kl_evt[i] = fill_kl_evt(i, all_kl_digits);
    if (fDebug) {
        std::cout << "mcevt: " << i << " kl " << _hits.size()
                  << " hits, " << all_kl_digits.size() << std::endl;
    }
    Json::Value kl_digs = fill_kl_evt(i, all_kl_digits);
    root["recon_events"][i]["kl_event"]["kl_digits"]["kl"]  = kl_digs;
  } // end loop over events
}

//////////////////////////////////////////////////////////////////////
std::vector<Json::Value> MapCppKLMCDigitizer::make_kl_digits(Json::Value hits,
                                                             Json::Value& root) const {
  std::vector<Json::Value> kl_digits;
  kl_digits.clear();

  if (hits.size() == 0) return kl_digits;

  for ( unsigned int j = 0; j < hits.size(); j++ ) {  //  j-th hit
      Json::Value hit = hits[j];
      if (fDebug) std::cout << "hit# " << j << hit << std::endl;

      // make sure we can get the cell info
      if (!hit.isMember("channel_id"))
          throw(Exception(Exception::recoverable,
                       "No channel_id in hit",
                       "MapCppKLMCDigitizer::make_kl_digits"));
      if (!hit.isMember("momentum"))
          throw(Exception(Exception::recoverable,
                       "No momentum in hit",
                       "MapCppKLMCDigitizer::make_kl_digits"));
      if (!hit.isMember("time"))
          throw(Exception(Exception::recoverable,
                       "No time in hit",
                       "MapCppKLMCDigitizer::make_kl_digits"));
      Json::Value channel_id = hit["channel_id"];

      if (!hit.isMember("energy_deposited"))
          throw(Exception(Exception::recoverable,
                       "No energy_deposited in hit",
                       "MapCppKLMCDigitizer::make_kl_digits"));
      double edep = hit["energy_deposited"].asDouble();

      if (fDebug) {
         std::cout << "klhit: " << hit["channel_id"] << " "
                   << hit["position"] << " " << hit["momentum"]
                   << " " << hit["time"] << " " << hit["energy_deposited"] << std::endl;
      }

      int cll = hit["channel_id"]["cell"].asInt();

      if (!root.isMember("daq_event_type")) {
        root["daq_event_type"] = "physics_event";
      }

      // find the geo module corresponding to this hit
      const MiceModule* hit_module = NULL;

      for ( unsigned int jj = 0; !hit_module && jj < kl_modules.size(); ++jj ) {
        if ( kl_modules[jj]->propertyExists("Cell", "int") &&
             kl_modules[jj]->propertyInt("Cell") == cll) {
               // got it
               hit_module = kl_modules[jj];
               if (fDebug) {
                 std::cout << kl_modules[jj]->propertyInt("Cell") << std::endl;
                 std::cout << kl_modules[jj]->fullName() << std::endl;
               }
        } // end check on module
      } // end loop over kl_modules

      // make sure we actually found a kl module corresponding to this hit
      if (hit_module == NULL)
          throw(Exception(Exception::recoverable,
                       "No KL module for hit",
                       "MapCppKLMCDigitizer::make_kl_digits"));

      // now get the position of the hit
      Hep3Vector hpos = JsonWrapper::JsonToThreeVector(hit["position"]);

      // get the cell and pmt positions from the geometry
      // cell pos
      Hep3Vector cellPos = hit_module->globalPosition();
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
        std::cout << "pmt1pos: " << pmt1Pos << " cellPos: " << cellPos << std::endl;
        std::cout << "pmt2pos: " << pmt2Pos << " cellPos: " << cellPos << std::endl;
      }

      // find distances from hit to pmt geom
      double dist1 = ( hpos - (cellPos + pmt1Pos) ).mag();
      double dist2 = ( hpos - (cellPos + pmt2Pos) ).mag();

      if (fDebug) std::cout << "distances to pmt: " << dist1 << "   " << dist2 << std::endl;

      // convert edep to photoelectrons for this cell/pmt
      // can't convert to adc yet since we need to add up ph.el's
      //   from other hits if any
      if (fDebug) std::cout << "edep= " << edep << std::endl;
      int npe1 = calculate_nphe_at_pmt(dist1, edep);
      int npe2 = calculate_nphe_at_pmt(dist2, edep);

      if (fDebug) std::cout << "npe1: "<< npe1 << ", npe2: "<< npe2 << std::endl;


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
        ss << "KLChannelKey " << cll << " " << p << " kl";
        tmpDigit["kl_key"] = ss.str();
        tmpDigit["cell"] = cll;
        tmpDigit["pmt"] = p;
        if (p == 0) {
	  tmpDigit["npe"] = npe1;
        } else if (p == 1) {
	  tmpDigit["npe"] = npe2;
        }

	if (fDebug)
          std::cout << "digit " << j << " key: " << ss.str() << std::endl;

        kl_digits.push_back(tmpDigit);
      } // end loop over pmts
    }  //  ends 'for' loop over hits

    return kl_digits;
}

//////////////////////////////////////////////////////////////////////
Json::Value MapCppKLMCDigitizer::check_sanity_mc(const Json::Value& root) const {
  // Check if the JSON document has a 'mc' branch, else return error
  if (!root.isMember("mc_events")) {
    throw MAUS::Exception(Exception::recoverable,
                          "Missing MC branch from data",
                          "MapCppKLDigitizer::check_sanity_mc");
  }

  Json::Value mc = root.get("mc_events", 0);
  // Check if JSON document is of the right type, else return error
  if (!mc.isArray()) {
    throw MAUS::Exception(Exception::recoverable,
                          "MC branch isn't an array",
                          "MapCppKLDigitizer::check_sanity_mc");
  }
  return mc;
}

//////////////////////////////////////////////////////////////////////
int MapCppKLMCDigitizer::calculate_nphe_at_pmt(double dist, double edep) const {
  if (fDebug) std::cout << "edep= " << edep << std::endl;

  if (!_configJSON.isMember("KLattLengthLong"))
          throw(Exception(Exception::recoverable,
		       "Could not find KLattLengthLong in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLattLengthShort"))
          throw(Exception(Exception::recoverable,
                       "Could not find KLattLengthShort in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLattLengthLongNorm"))
          throw(Exception(Exception::recoverable,
                       "Could not find KLattLengthLongNorm in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLattLengthShortNorm"))
          throw(Exception(Exception::recoverable,
                       "Could not find KLattLengthShortNorm in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLconversionFactor"))
          throw(Exception(Exception::recoverable,
                       "Could not find KLconversionFactor in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLlightCollectionEff"))
          throw(Exception(Exception::recoverable,
                       "Could not find KLlightCollectionEff in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLquantumEff"))
          throw(Exception(Exception::recoverable,
                       "Could not find KLquantumEff in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLlightGuideEff"))
          throw(Exception(Exception::recoverable,
                       "Could not find KLlightGuideEff in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLpmtGain"))
          throw(Exception(Exception::recoverable,
                       "Could not find KLpmtGain in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLpmtSigmaGain"))
          throw(Exception(Exception::recoverable,
                       "Could not find KLpmtSigmaGain in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));

  // convert energy deposited to number of photoelectrons
  double attLengthL   = (_configJSON["KLattLengthLong"].asDouble());
  double attLengthS   = (_configJSON["KLattLengthShort"].asDouble());
  double nL           = (_configJSON["KLattLengthLongNorm"].asDouble());
  double nS           = (_configJSON["KLattLengthShortNorm"].asDouble());
  double energyW      = (_configJSON["KLconversionFactor"].asDouble());
  double effColl      = (_configJSON["KLlightCollectionEff"].asDouble());
  double qEff         = (_configJSON["KLquantumEff"].asDouble());
  double lgEff        = (_configJSON["KLlightGuideEff"].asDouble());

  double meanPhN;
  int numberOfPh, collectedPh, pe;

  // Photons in hit
  meanPhN    = edep/energyW;
  numberOfPh = floor(RandPoisson::shoot(meanPhN));

  // At the photomultiplier, after attenuation. Formula is from KLOE
  double attenuation = nL*exp(-dist/attLengthL)+nS*exp(-dist/attLengthS);
  collectedPh = floor(numberOfPh*effColl*attenuation*lgEff+0.5);
  // Poisson smearing of photoelctrons at pmt cathode
  pe = floor(RandPoisson::shoot(collectedPh*qEff));

  // Now calculate amplified photoelectrons at the pmt anode and smear them with gaussian
  int gain   = (_configJSON["KLpmtGain"].asInt());
  int sigma_gain   = (_configJSON["KLpmtSigmaGain"].asInt());

  int peAmplified = static_cast<int> (floor(pe*RandGauss::shoot(gain, sigma_gain)));

  if (fDebug) {
    std::cout << "created photons: " << meanPhN << std::endl;
    std::cout << "smeared photons: " << numberOfPh << std::endl;
    std::cout << "attenuated photons at pmt: " << collectedPh << std::endl;
    std::cout << "photoelectrons at pmt cathode: " << collectedPh*qEff << std::endl;
    std::cout << "smeared photoelectrons at pmt cathode: " << pe << std::endl;
    std::cout << "amplified photoelectrons: " << peAmplified<< std::endl;
  }

  return peAmplified;
}


//////////////////////////////////////////////////////////////////////
Json::Value MapCppKLMCDigitizer::fill_kl_evt(int evnum,
                                             std::vector<Json::Value> all_kl_digits) const {
  if (!_configJSON.isMember("Do_V1724_Zero_Suppression"))
          throw(Exception(Exception::recoverable,
                "Could not find Do_V1724_Zero_Suppression",
                "MapCppKLMCDigitizer::fill_kl_evt"));
  if (!_configJSON.isMember("V1724_Zero_Suppression_Threshold"))
          throw(Exception(Exception::recoverable,
                "Could not find V1724_Zero_Suppression_Threshold",
                "MapCppKLMCDigitizer::fill_kl_evt"));

  bool zero_supp   = (_configJSON["Do_V1724_Zero_Suppression"].asBool());
  int adc_thresh;

  if (zero_supp) {
    adc_thresh   = (_configJSON["V1724_Zero_Suppression_Threshold"].asInt());
  } else {
    adc_thresh = 0;
  }

  Json::Value kl_digit(Json::arrayValue);
  // return null if this evt had no kl hits
  if (all_kl_digits.size() == 0) return kl_digit;
  int npe;
  int ndigs = 0;

  // throw out multihits and add up the light yields from multihits in slabs
  for (unsigned int i = 0; i < all_kl_digits.size(); ++i) {
    // check if the digit has been used
    if ( all_kl_digits[i]["isUsed"].asInt() == 0 ) {
      ndigs++;

      npe = all_kl_digits[i]["npe"].asInt();
      // loop over all the other digits to find multihit slabs
      for ( unsigned int j = i+1; j < all_kl_digits.size(); j++ ) {
        if ( check_param(&(all_kl_digits[i]), &(all_kl_digits[j])) ) {
            // add up light yields if same bar was hit
            npe += all_kl_digits[j]["npe"].asInt();
            // mark this hit as used so we don't multicount
            all_kl_digits[j]["isUsed"]=1;
        } // end check for used
      } // end loop over secondary digits
      // convert light yield to adc & set the charge
      int adc = static_cast<int>(npe / (_configJSON["KLadcConversionFactor"].asInt()));

      if (fDebug) std::cout << "npe-adc: " << npe << " " << adc << std::endl;
      Json::Value digit(Json::objectValue);
      if (adc > adc_thresh) {
	digit["charge_pm"] = adc;
	digit["charge_mm"] = adc;
	digit["kl_key"] = all_kl_digits[i]["kl_key"].asString();
	digit["cell"] = all_kl_digits[i]["cell"].asInt();
	digit["pmt"] = all_kl_digits[i]["pmt"].asInt();

	// store event number
	digit["phys_event_number"] = evnum;
	digit["part_event_number"] = evnum;

	if (fDebug) std::cout << "adc over thresh: "<< adc << std::endl;

	kl_digit.append(digit);
      }

      if (fDebug)
          std::cout << "digit #" << ndigs << " " <<  digit["kl_key"].asString() << std::endl;
      all_kl_digits[i]["isUsed"]=1;
    } // ends if-statement
  } // ends k-loop
  return kl_digit;
}

//////////////////////////////////////////////////////////////////////
bool MapCppKLMCDigitizer::check_param(Json::Value* hit1, Json::Value* hit2) const {
  int cell1 = (*hit1)["cell"].asInt();
  int cell2 = (*hit2)["cell"].asInt();
  int pmt1     = (*hit1)["pmt"].asInt();
  int pmt2     = (*hit2)["pmt"].asInt();
  int hit_is_used = (*hit2)["isUsed"].asInt();

  if ( cell1 == cell2 && pmt1 == pmt2 && !hit_is_used ) {
    return true;
  } else {
    return false;
  }
}


//=====================================================================
}
