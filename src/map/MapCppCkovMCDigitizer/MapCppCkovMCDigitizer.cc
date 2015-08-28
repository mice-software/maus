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

#include "src/map/MapCppCkovMCDigitizer/MapCppCkovMCDigitizer.hh"
#include <string>
#include "Config/MiceModule.hh"
#include "Utils/Exception.hh"
#include "API/PyWrapMapBase.hh"
#include "src/common_cpp/DataStructure/RunHeaderData.hh"
#include "src/common_cpp/DataStructure/RunHeader.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppCkovMCDigitizer(void) {
  PyWrapMapBase<MAUS::MapCppCkovMCDigitizer>::PyWrapMapBaseModInit
                                      ("MapCppCkovMCDigitizer", "", "", "", "");
}

MapCppCkovMCDigitizer::MapCppCkovMCDigitizer()
    : MapBase<Json::Value>("MapCppCkovMCDigitizer") {
}

//////////////////////////////////////////////////////////////////////
void MapCppCkovMCDigitizer::_birth(const std::string& argJsonConfigDocument) {

  // Before we go on we generate a new TRandom3;
  rnd = new TRandom3;
  // print level
  fDebug = false;

  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    throw MAUS::Exception(Exception::recoverable,
                          "Failed to parse Json Configuration",
                          "MapCppCkovMCDigitizer::_birth");
  }

  // get the geometry
  // Default value of the reconstruction_geometry_filename is an empty string;
  // The simulation_geometry_filename will be copied to it.
  // So this is normally the simulation geometry.
  if (!_configJSON.isMember("reconstruction_geometry_filename"))
      throw(Exception(Exception::recoverable,
                   "Could not find geometry file",
                   "MapCppCkovMCDigitizer::birth"));
  std::string filename;
  filename = _configJSON["reconstruction_geometry_filename"].asString();

  // get the ckov geometry modules
  geo_module = new MiceModule(filename);
  ckov_modules = geo_module->findModulesByPropertyString("SensitiveDetector", "CKOV");

  _station_index.push_back("A");
  _station_index.push_back("B");
}

//////////////////////////////////////////////////////////////////////
void MapCppCkovMCDigitizer::_death() {
}

//////////////////////////////////////////////////////////////////////
void MapCppCkovMCDigitizer::_process(Json::Value* document) const {

  Json::Value& root = *document;
  // check sanity of json input file and mc brach
  Json::Value mc = check_sanity_mc(*document);

  // --- Below are from TOF:
  // all_tof_digits store all the tof hits
  // then we'll weed out multiple hits, add up yields, and store the event
  // ---
  // For the Ckov, it's similar. Just change TOF to Ckov.
  //
  std::vector<Json::Value> all_ckov_digits;
  all_ckov_digits.push_back(Json::Value());

  // loop over events
  if (fDebug) printf("mc numevt = %i", mc.size());
  for ( unsigned int i = 0; i < mc.size(); i++ ) {
    Json::Value particle = mc[i];
    double gentime = particle["primary"]["time"].asDouble();
    if (fDebug) std::cout << "evt: " << i << " pcle= " << particle << std::endl;

    // hits for this event
    Json::Value _hits = particle["ckov_hits"];
    // if _hits.size() = 0, the make_digits and fill_ckov_evt functions will
    // return null. The Ckov recon expects something - either null or data, can't just skip
    all_ckov_digits.clear();

    // pick out tof hits, digitize and store
    all_ckov_digits = make_ckov_digits(_hits, gentime, *document);

    // Does Ckov need this pixel thing? Maybe not.
    // std::string strig = findTriggerPixel(all_tof_digits);

    // ------>NOW,  go through the stations and fill Ckov events
    // real data tof digits look like so:
    // "digits":{"tof1":[ [evt0..{pmt0},{pmt1}..],[evt1..]],"tof0":[[evt0]..],tof2..}
    // loop over ckov stations
    Json::Value ckov_digitAB(Json::arrayValue);
    ckov_digitAB.clear();
    for (int snum = 0; snum < 2; ++snum) {
       for (unsigned int kk = 0; kk < all_ckov_digits.size(); ++kk) {
         // check that this digit belongs to the station we are trying to fill
         if (all_ckov_digits[kk]["station"].asInt() != snum) continue;
         ckov_digitAB.append(fill_ckov_evt(i, snum, all_ckov_digits, kk));
       }
     } // end loop over stations
     root["recon_events"][i]["ckov_event"]["ckov_digits"] = ckov_digitAB;
  } // end loop over events
}

//////////////////////////////////////////////////////////////////////
std::vector<Json::Value>
MapCppCkovMCDigitizer::make_ckov_digits(Json::Value hits, double gentime,
                                        Json::Value& root) const {
  std::vector<Json::Value> ckov_digits;
  ckov_digits.clear();
  if (hits.size() == 0) return ckov_digits;

  for (unsigned int j = 0; j < hits.size(); ++j) {  //  j-th hit
      Json::Value hit = hits[j];

      if (fDebug) std::cout << "=================== hit# " << j << std::endl;

      // make sure we can get the station/slab info
      if (!hit.isMember("channel_id"))
          throw(Exception(Exception::recoverable,
                       "No channel_id in hit",
                       "MapCppCkovMCDigitizer::make_ckov_digits"));
      if (!hit.isMember("momentum"))
          throw(Exception(Exception::recoverable,
                       "No momentum in hit",
                       "MapCppCkovMCDigitizer::make_ckov_digits"));
      if (!hit.isMember("time"))
          throw(Exception(Exception::recoverable,
                       "No time in hit",
                       "MapCppCkovMCDigitizer::make_ckov_digits"));
      Json::Value channel_id = hit["channel_id"];

      if (!hit.isMember("energy_deposited"))
          throw(Exception(Exception::recoverable,
                       "No energy_deposited in hit",
                       "MapCppCkovMCDigitizer::make_ckov_digits"));
      double edep = hit["energy_deposited"].asDouble();

      // Ckov hits in the CkovChannelIdProcessor of Json Parser registers a property
      // "station_number" in channel_id
      int stn = hit["channel_id"]["station_number"].asInt();

      // find the geo module corresponding to this hit
      const MiceModule* hit_module = NULL;
      for ( unsigned int jj = 0; !hit_module && jj < ckov_modules.size(); ++jj ) {
        if (ckov_modules[jj]->propertyExists("CkovStation", "int") &&
            ckov_modules[jj]->propertyInt("CkovStation") == stn)
          hit_module = ckov_modules[jj];
      } // end loop over tof_modules

      // make sure we actually found a tof module corresponding to this hit
      if (hit_module == NULL)
          throw(Exception(Exception::recoverable,
                       "No Ckov module for hit",
                       "MapCppCkovMCDigitizer::make_ckov_digits"));

      // now get the position of the hit
      Hep3Vector hpos = JsonWrapper::JsonToThreeVector(hit["position"]);

      // get the pmt positions from the geometry
      // pmt positions
      std::string pmtName;
      // pmt 0
      pmtName = "PMT0Pos";
      Hep3Vector pmt0Pos = hit_module->propertyHep3Vector(pmtName);
      // pmt 1
      pmtName = "PMT1Pos";
      Hep3Vector pmt1Pos = hit_module->propertyHep3Vector(pmtName);
      // pmt 2
      pmtName = "PMT2Pos";
      Hep3Vector pmt2Pos = hit_module->propertyHep3Vector(pmtName);
      // pmt 3
      pmtName = "PMT2Pos";
      Hep3Vector pmt3Pos = hit_module->propertyHep3Vector(pmtName);
      //

      // find distances from hit to pmt geom
      double dist0 = (hpos - (pmt0Pos)).mag();
      double dist1 = (hpos - (pmt1Pos)).mag();
      double dist2 = (hpos - (pmt2Pos)).mag();
      double dist3 = (hpos - (pmt3Pos)).mag();

      // Here we uniformly distribute all the pe to the 4 pmts;
      // The distance is used to calculate the time of the signal.
      printf("Passing hit number %i", j);
      double npe = get_npe(hit);
      double npe0, npe1, npe2, npe3;
      npe0 = npe1 = npe2 = npe3 = npe/4;

      // get the hit time, speed of c in the ckov, and define the pmt resolution;
      double htime = hit["time"].asDouble() - gentime;
      double csp = (stn == 0 ? 3.0e8/1.069 : 3.0e8/1.112);
      double pmt_res = 0.1;

      // propagate time to pmt & smear by the resolution
      double time0 = rnd->Gaus((htime + dist0/csp) , pmt_res);
      double time1 = rnd->Gaus((htime + dist1/csp) , pmt_res);
      double time2 = rnd->Gaus((htime + dist2/csp) , pmt_res);
      double time3 = rnd->Gaus((htime + dist3/csp) , pmt_res);
      // TDC conversion factor:
      double tdc_factor = 0.025;

      // convert to tdc
      int tdc0 = static_cast<int>(time0 / tdc_factor);
      int tdc1 = static_cast<int>(time1 / tdc_factor);
      int tdc2 = static_cast<int>(time2 / tdc_factor);
      int tdc3 = static_cast<int>(time3 / tdc_factor);
      if (fDebug) {
         std::cout << "tdc: " << tdc0 << " " << tdc1
                   << " " << tdc2 << " " << tdc3 << std::endl;
      }

      Json::Value tmpDigit;
      tmpDigit.clear();
      tmpDigit["channel_id"] = hit["channel_id"];
      tmpDigit["momentum"] = hit["momentum"];
      tmpDigit["time"] = hit["time"];
      tmpDigit["position"]=hit["position"];
      tmpDigit["isUsed"] = 0;
      tmpDigit["npe"] = npe;

	  // set the key for this channel
      tmpDigit["station"] = stn;
	  tmpDigit["npe0"] = npe0;
      tmpDigit["leading_time0"] = tdc0;
      tmpDigit["raw_time0"] = time0;
	  tmpDigit["npe1"] = npe1;
      tmpDigit["leading_time1"] = tdc1;
      tmpDigit["raw_time1"] = time1;
      tmpDigit["npe2"] = npe2;
      tmpDigit["leading_time2"] = tdc2;
      tmpDigit["raw_time2"] = time2;
      tmpDigit["npe3"] = npe3;
      tmpDigit["leading_time3"] = tdc3;
      tmpDigit["raw_time3"] = time3;
      ckov_digits.push_back(tmpDigit);
    }  // for (unsigned int j = 0; j < hits.size(); ++j)

    return ckov_digits;
}

//////////////////////////////////////////////////////////////////////
Json::Value MapCppCkovMCDigitizer::check_sanity_mc(Json::Value& root) const {
  // Check if the JSON document can be parsed, else return error only
  // Check if the JSON document has a 'mc' branch, else return error
  if (!root.isMember("mc_events")) {
    throw MAUS::Exception(Exception::recoverable,
                          "Could not find an MC branch to simulate.",
                          "MapCppCkovMCDigitizer::check_sanity_mc");
  }

  Json::Value mc = root.get("mc_events", 0);
  // Check if JSON document is of the right type, else return error
  if (!mc.isArray()) {
    throw MAUS::Exception(Exception::recoverable,
                          "MC branch was not an array.",
                          "MapCppCkovMCDigitizer::check_sanity_mc");
  }
  return mc;
}

//////////////////////////////////////////////////////////////////////
double MapCppCkovMCDigitizer::get_npe(Json::Value hit) const {

  double nphot = 0.;
  double nptmp = 0.;
  double muon_thrhold = 0.;
  double charge_per_pe = 0.;
  // scaling factor to data
  double scaling = 0.935;
  // mass of the hit particle;
  double particle_mass = hit["mass"].asDouble();
  // momentum of the hit particle;
  double px = hit["momentum"]["x"].asDouble();
  double py = hit["momentum"]["y"].asDouble();
  double pz = hit["momentum"]["z"].asDouble();
  double p_tot = sqrt(px*px + py*py + pz*pz);

  if (hit["channel_id"]["station_number"].asInt() == 0) {
    muon_thrhold = 275.0;
    charge_per_pe = 17.3;
  } else if (hit["channel_id"]["station_number"].asInt() == 1) {
    muon_thrhold = 210.0;
    charge_per_pe = 26;
  }

  // momentum threshold, see Lucien's Python code.
  double pp_threshold = muon_thrhold * particle_mass / 105.6;

  // smearing constant
  const double smear_const = 0.5;

  // poisson generator
  double npssn_sum;
  double npssn;
  double rms_sum;
  double npssn_i;
  double rms_i;
  double rms;

  if (p_tot >= pp_threshold) {

    // Above threshold;
    double pred = scaling * charge_per_pe *
                  (1.0 - (pp_threshold/p_tot)*(pp_threshold/p_tot)) + 0.5;
    double pred_single = pred/4;

    npssn_sum = 0;
    rms_sum = 0;

    // Electron shower:
    if (hit["particle_id"].asInt() == 11 || hit["particle_id"].asInt() == -11) {

      double rnd_number = rnd->Rndm();
      if (hit["channel_id"]["station_number"].asInt() == 0) {
        if (rnd_number <= 0.03)
          pred *= 2;
        else if (rnd_number <= 0.039 && rnd_number > 0.03)
          pred *= 3;
        else if (rnd_number <= 0.046 && rnd_number > 0.039)
          pred = 0;
        else
          pred *= 1;
      } else {
        if (rnd_number <= 0.04) {
          pred *= 2;
        } else if (rnd_number <= 0.047 && rnd_number > 0.04) {
          pred *= 3;
        } else if (rnd_number <= 0.0475 && rnd_number > 0.047) {
          pred *= 4;
        }
      }
    }

    for (unsigned int ii = 0; ii < 4; ii++) {
      npssn_i = rnd->Poisson(pred_single);
      rms_i = (smear_const*sqrt(npssn_i)) * (smear_const*sqrt(npssn_i));
      npssn_sum += npssn_i;
      rms_sum += rms_i * rms_i;
    }
    rms = sqrt(rms_sum);
    rms = (rms < 0.2 ? 0.2 : rms);
    npssn = npssn_sum;
    if (hit["particle_id"].asInt() == 13 || hit["particle_id"].asInt() == -13 ||
        hit["particle_id"].asInt() == 211 || hit["particle_id"].asInt() == -211) {
      if (rnd->Rndm() < 0.06) {
        npssn -= log(rnd->Rndm())/0.2;
      }
    }
  } else {

    // below threshold;

    double pred = 0.5;
    double ped0 = (exp(-1*pred) >= 0.2298*exp(-0.34344*pred)
                   ? exp(-1*pred) : 0.2298*exp(-0.34344*pred));
    double ped1 = pred * ped0;
    double ped2 = 0.5*pred*pred*ped0;
    double npssn = 1.0;
    double rms = 0.35;
    double rnd_number = rnd->Rndm();
    if (rnd_number <= ped0) {
      npssn = 0;
      rms = 0.1;
    } else if (rnd_number <= ped1+ped0 && rnd_number > ped0) {
      npssn = 1;
    } else if (rnd_number <= ped2+ped1+ped0 && rnd_number > ped1+ped0) {
      npssn = 2;
    }
  }

  double gaussian = rnd->Gaus();
  double xnpe = ((npssn+rms*gaussian) > 0 ? npssn+rms*gaussian : 0);

  if (fDebug) {
    printf("Hit: Momentum %.3e, mass %.3e, generated Gaussian is %.3e, the resulting xnpe is %.3e",
           p_tot, particle_mass, gaussian, xnpe);
  }
  return xnpe;
}

//////////////////////////////////////////////////////////////////////
Json::Value MapCppCkovMCDigitizer::fill_ckov_evt(int evnum,
                                                 int snum,
                                                 std::vector<Json::Value> all_ckov_digits,
                                                 int i)
const {
  Json::Value ckov_digits;
  ckov_digits.clear();

  // return null if this evt had no ckov hits
  if (all_ckov_digits.size() == 0) return ckov_digits;

  double npe;
  int ndigs = 0;

  Json::Value digitA;
  Json::Value digitB;
  digitA.clear();
  digitB.clear();

  // check if the digit has been used
  if (all_ckov_digits[i]["isUsed"].asInt() == 0) {

    ndigs++;

    npe = all_ckov_digits[i]["npe"].asDouble();
    // 23 is the ADC factor.
    int adc = static_cast<int>(npe * 23);

    // NOTE: needs tweaking/verifying -- DR 3/15
    // NOTE: if tof needs tweaking, Ckov also needs it. -- frankliuao 8/15
    // Initialize digits for both stations:
    digitA["position_min_0"] = 0;
    digitA["position_min_1"] = 0;
    digitA["position_min_2"] = 0;
    digitA["position_min_3"] = 0;
    digitA["pulse_0"] = 0;
    digitA["pulse_1"] = 0;
    digitA["pulse_2"] = 0;
    digitA["pulse_3"] = 0;
    digitA["coincidences"] = 0;
    digitA["total_charge"] = 0;
    digitA["arrival_time_0"] = 0;
    digitA["arrival_time_1"] = 0;
    digitA["arrival_time_2"] = 0;
    digitA["arrival_time_3"] = 0;
    digitA["part_event_number"] = 0;
    digitA["number_of_pes"] = 0;


    digitB["position_min_4"] = 0;
    digitB["position_min_5"] = 0;
    digitB["position_min_6"] = 0;
    digitB["position_min_7"] = 0;
    digitB["pulse_4"] = 0;
    digitB["pulse_5"] = 0;
    digitB["pulse_6"] = 0;
    digitB["pulse_7"] = 0;
    digitB["total_charge"] = 0;
    digitB["coincidences"] = 0;
    digitB["arrival_time_4"] = 0;
    digitB["arrival_time_5"] = 0;
    digitB["arrival_time_6"] = 0;
    digitB["arrival_time_7"] = 0;
    digitB["part_event_number"] = 0;
    digitB["number_of_pes"] = 0;


    if (snum == 0) {
      digitA["arrival_time_0"] = all_ckov_digits[i]["leading_time0"];
      digitA["arrival_time_1"] = all_ckov_digits[i]["leading_time1"];
      digitA["arrival_time_2"] = all_ckov_digits[i]["leading_time2"];
      digitA["arrival_time_3"] = all_ckov_digits[i]["leading_time3"];
      digitA["total_charge"] = adc;
      digitA["part_event_number"] = evnum;
      digitA["number_of_pes"] = npe;
    } else {
      digitB["arrival_time_4"] = all_ckov_digits[i]["leading_time0"];
      digitB["arrival_time_5"] = all_ckov_digits[i]["leading_time1"];
      digitB["arrival_time_6"] = all_ckov_digits[i]["leading_time2"];
      digitB["arrival_time_7"] = all_ckov_digits[i]["leading_time3"];
      digitB["total_charge"] = adc;
      digitB["part_event_number"] = evnum;
      digitB["number_of_pes"] = npe;
    }

    all_ckov_digits[i]["isUsed"] = 1;
  } // #if (all_ckov_digits[i]["isUsed"].asInt() == 0)
  ckov_digits["A"] = digitA;
  ckov_digits["B"] = digitB;
  return ckov_digits;
}
//=====================================================================
}
