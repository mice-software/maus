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
    : MapBase<Data>("MapCppCkovMCDigitizer") {
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

  // initialize thesholds and conversion factors
  // Ckov A
  muon_thrhold[0] = 275.0;
  charge_per_pe[0] = 17.3;
  // Ckov B
  muon_thrhold[1] = 210.0;
  charge_per_pe[1] = 26;

  // scaling factor to data
  scaling = 0.935;

  // npe->adc conversion factor
  ckovNpeToAdc = 23.0;
}

//////////////////////////////////////////////////////////////////////
void MapCppCkovMCDigitizer::_death() {
}

//////////////////////////////////////////////////////////////////////
void MapCppCkovMCDigitizer::_process(MAUS::Data* data) const {


  // --- Below are from TOF:
  // all_tof_digits store all the tof hits
  // then we'll weed out multiple hits, add up yields, and store the event
  // ---
  // For the Ckov, it's similar. Just change TOF to Ckov.
  //

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

  if (fDebug) std::cerr << "mc numevt = i" << mcEvts->size() << std::endl;

  // loop over events
  for ( unsigned int i = 0; i < mcEvts->size(); i++ ) {
    // Get Ckov hits for this event
    CkovHitArray *hits = spill->GetAnMCEvent(i).GetCkovHits();

    double gentime = spill->GetAnMCEvent(i).GetPrimary()->GetTime();
    CkovEvent *evt = new CkovEvent();
    (*recEvts)[i]->SetCkovEvent(evt);

    // process only if there are hits
    // if _hits.size() = 0, the make_digits and fill_ckov_evt functions will
    // return null. The Ckov recon expects something - either null or data, can't just skip
    if (hits) {

      // pick out ckov hits, digitize and store
      CkovTmpDigits tmpAllDigits = make_ckov_digits(hits, gentime);

      CkovDigitArray* CkovDigArray = evt->GetCkovDigitArrayPtr();
      fill_ckov_evt(i, tmpAllDigits, CkovDigArray);

      // DR 2015-08-31 -- the loops below are now done within fill_ckov_evt
      // ------>NOW,  go through the stations and fill Ckov events
      // loop over ckov stations
      /*
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
       */
    } // end check-if-hits
  } // end loop over events
}

//////////////////////////////////////////////////////////////////////
CkovTmpDigits MapCppCkovMCDigitizer::make_ckov_digits(CkovHitArray* hits,
                                                      double gentime) const {

  CkovTmpDigits tmpDigits(0);
  if (hits->size() == 0) return tmpDigits;

  for (unsigned int j = 0; j < hits->size(); ++j) {  //  j-th hit
      CkovHit hit = hits->at(j);
      if (fDebug) std::cout << "=================== hit# " << j << std::endl;

      // make sure we can get the station number
      if (!hit.GetChannelId())
          throw(Exception(Exception::recoverable,
                       "No channel_id in hit",
                       "MapCppCkovMCDigitizer::make_ckov_digits"));

      double edep = hit.GetEnergyDeposited();

      // Ckov hits in the CkovChannelIdProcessor of Json Parser registers a property
      // "station_number" in channel_id
      int stn = hit.GetChannelId()->GetStation();

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
      ThreeVector tpos = hit.GetPosition();
      Hep3Vector hpos(tpos.x(), tpos.y(), tpos.z());

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
      pmtName = "PMT3Pos";
      Hep3Vector pmt3Pos = hit_module->propertyHep3Vector(pmtName);
      //

      // find distances from hit to pmt geom
      double dist0 = (hpos - (pmt0Pos)).mag();
      double dist1 = (hpos - (pmt1Pos)).mag();
      double dist2 = (hpos - (pmt2Pos)).mag();
      double dist3 = (hpos - (pmt3Pos)).mag();

      // Here we uniformly distribute all the pe to the 4 pmts;
      // The distance is used to calculate the time of the signal.
      if (fDebug) printf("Passing hit number %i", j);
      double npe = get_npe(hit);
      double npe0, npe1, npe2, npe3;
      npe0 = npe1 = npe2 = npe3 = npe/4;

      // get the hit time, speed of c in the ckov, and define the pmt resolution;
      double htime = hit.GetTime() - gentime;
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
      // I believe the ckov only has caen v1731 flash adc's
      int tdc0 = static_cast<int>(time0 / tdc_factor);
      int tdc1 = static_cast<int>(time1 / tdc_factor);
      int tdc2 = static_cast<int>(time2 / tdc_factor);
      int tdc3 = static_cast<int>(time3 / tdc_factor);
      if (fDebug) {
         std::cout << "tdc: " << tdc0 << " " << tdc1
                   << " " << tdc2 << " " << tdc3 << std::endl;
      }

      aTmpCkovDigit aTmpDigit;
      aTmpDigit.fChannelId = hit.GetChannelId();
      aTmpDigit.fMom = hit.GetMomentum();
      aTmpDigit.fTime = hit.GetTime();
      aTmpDigit.fPos = hit.GetPosition();
      aTmpDigit.fIsUsed = 0;
      aTmpDigit.fNpe = npe;

      // set the key for this channel
      aTmpDigit.fStation = stn;
      aTmpDigit.fNpe0 = npe0;
      aTmpDigit.fLeadingTime0 = tdc0;
      aTmpDigit.fRawTime0 = time0;
      aTmpDigit.fNpe1 = npe1;
      aTmpDigit.fLeadingTime1 = tdc1;
      aTmpDigit.fRawTime1 = time1;
      aTmpDigit.fNpe2 = npe2;
      aTmpDigit.fLeadingTime2 = tdc2;
      aTmpDigit.fRawTime2 = time2;
      aTmpDigit.fNpe3 = npe3;
      aTmpDigit.fLeadingTime3 = tdc3;
      aTmpDigit.fRawTime3 = time3;
      tmpDigits.push_back(aTmpDigit);
    }  // for (unsigned int j = 0; j < hits.size(); ++j)

    return tmpDigits;
}
//////////////////////////////////////////////////////////////////////
double MapCppCkovMCDigitizer::get_npe(CkovHit& hit) const {

  double nphot = 0.;
  double nptmp = 0.;
  /* these are defined in the header & initialized at birth - DR 2015-08-31
  double muon_thrhold = 0.;
  double charge_per_pe = 0.;
  // scaling factor to data
  double scaling = 0.935;
  */
  // mass of the hit particle;
  double particle_mass = hit.GetMass();
  // momentum of the hit particle;
  double px = hit.GetMomentum().x();
  double py = hit.GetMomentum().y();
  double pz = hit.GetMomentum().z();
  double p_tot = sqrt(px*px + py*py + pz*pz);

  int hitStation = hit.GetChannelId()->GetStation();
  int hitPid = hit.GetParticleId();
  /* the initialization of these is now done in birth so as to keep 
   * all hardcoded values in one place  - DR 2015-08-31
  if (hitStation == 0) {
    muon_thrhold = 275.0;
    charge_per_pe = 17.3;
  } else if (hitStation == 1) {
    muon_thrhold = 210.0;
    charge_per_pe = 26;
  }
  */

  // momentum threshold, see Lucien's Python code.
  double pp_threshold = muon_thrhold[hitStation] * particle_mass / 105.6;

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
    double pred = scaling * charge_per_pe[hitStation] *
                  (1.0 - (pp_threshold/p_tot)*(pp_threshold/p_tot)) + 0.5;
    double pred_single = pred/4;

    npssn_sum = 0;
    rms_sum = 0;

    // Electron shower:
    if (hitPid == 11 || hitPid == -11) {

      double rnd_number = rnd->Rndm();
      if (hitStation == 0) {
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
    if (hitPid == 13 || hitPid == -13 ||
        hitPid == 211 || hitPid == -211) {
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
void MapCppCkovMCDigitizer::fill_ckov_evt(int evnum,
                                          CkovTmpDigits& tmpAllDigits,
                                          CkovDigitArray* CkovDigArray) const {
  // return null if this evt had no ckov hits
  if (tmpAllDigits.size() == 0) return;

  double npe;
  int ndigs = 0;

  for (int snum = 0; snum < 2; ++snum) {
    for (unsigned int kk = 0; kk < tmpAllDigits.size(); ++kk) {
      // check that this hit has not already been used/filled
      if (tmpAllDigits[kk].fStation != snum) continue;
      CkovDigit aDigit;
      if (tmpAllDigits[kk].fIsUsed == 0) {
          ndigs++;
          npe = tmpAllDigits[kk].fNpe;
          // 23 is the ADC factor.
          // the factor - ckovNpeToAdc is set at birth, defined in header
          // keeping for-now-hardcoded definitions in one place
          // -- DR 2015-08-31
          int adc = static_cast<int>(npe * ckovNpeToAdc);

          CkovA digitA;
          CkovB digitB;
          if (snum == 0) {
              digitA.SetArrivalTime0(tmpAllDigits[kk].fLeadingTime0);
              digitA.SetArrivalTime1(tmpAllDigits[kk].fLeadingTime1);
              digitA.SetArrivalTime2(tmpAllDigits[kk].fLeadingTime2);
              digitA.SetArrivalTime3(tmpAllDigits[kk].fLeadingTime3);
              digitA.SetTotalCharge(adc);
              digitA.SetPartEventNumber(evnum);
              digitA.SetNumberOfPes(npe);

              // Does not matter too much but..
              //   I think the pulse0/1/2/3 should be adc/4 -- DR 2015-08-31
              digitA.SetPositionMin0(0);
              digitA.SetPositionMin1(0);
              digitA.SetPositionMin2(0);
              digitA.SetPositionMin3(0);
              digitA.SetPulse0(0);
              digitA.SetPulse1(0);
              digitA.SetPulse2(0);
              digitA.SetPulse3(0);
              digitA.SetCoincidences(0);
              aDigit.SetCkovA(digitA);
          } else {
              digitB.SetArrivalTime4(tmpAllDigits[kk].fLeadingTime0);
              digitB.SetArrivalTime5(tmpAllDigits[kk].fLeadingTime1);
              digitB.SetArrivalTime6(tmpAllDigits[kk].fLeadingTime2);
              digitB.SetArrivalTime7(tmpAllDigits[kk].fLeadingTime3);
              digitB.SetTotalCharge(adc);
              digitB.SetPartEventNumber(evnum);
              digitB.SetNumberOfPes(npe);

              // Does not matter too much but..
              //   I think the pulse4/5/6/7 should be adc/4 -- DR 2015-08-31
              digitB.SetPositionMin4(0);
              digitB.SetPositionMin5(0);
              digitB.SetPositionMin6(0);
              digitB.SetPositionMin7(0);
              digitB.SetPulse4(0);
              digitB.SetPulse5(0);
              digitB.SetPulse6(0);
              digitB.SetPulse7(0);
              digitB.SetCoincidences(0);
              aDigit.SetCkovB(digitB);
          }
          tmpAllDigits[kk].fIsUsed = true;
      } // end check if-digit-used
      CkovDigArray->push_back(aDigit);
    } // end loop over tmp digits
  } // end loop over stations
}
//=====================================================================
}
