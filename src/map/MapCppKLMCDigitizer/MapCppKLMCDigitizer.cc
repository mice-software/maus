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
  : MapBase<Data>("MapCppKLMCDigitizer") {
}

//////////////////////////////////////////////////////////////////////
void MapCppKLMCDigitizer::_birth(const std::string& argJsonConfigDocument) {
  // print level
  fDebug = false;

  // Check if the JSON document can be parsed, else return error only
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, _configJSON);
  if (!parsingSuccessful) {
    throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                          "Failed to parse Json Configuration",
                          "MapCppKLMCDigitizer::_birth");
  }
  // get the geometry
  if (!_configJSON.isMember("simulation_geometry_filename"))
      throw(Exceptions::Exception(Exceptions::recoverable,
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
void MapCppKLMCDigitizer::_process(MAUS::Data* data) const {
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

  // all_kl_digits store all the kl hits
  // then we'll weed out multiple hits, add up yields, and store the event

  // loop over events
  if (fDebug) std::cout << "mc numevts = " << mcEvts->size() << std::endl;
  // Construct digits from hits for each MC event
  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    // Get kl hits for this event
    KLHitArray *hits = spill->GetAnMCEvent(iPe).GetKLHits();

    KLEvent *evt = new KLEvent();
    (*recEvts)[iPe]->SetKLEvent(evt);
    // process only if there are kl hits
    if (hits) {

      // pick out kl hits, digitize and store
      KLTmpDigits tmpDigits = make_kl_digits(hits);


      // fill kl events
      if (fDebug) {
        std::cout << "mcevt: " << iPe << " kl " << hits->size()
                  << " hits, " << tmpDigits.size() << std::endl;
      }

      KLDigitArray* kl_digits = evt->GetKLEventDigitPtr()->GetKLDigitArrayPtr();
      fill_kl_evt(iPe, tmpDigits, kl_digits);
    } // end if-hits
  } // end loop over events
}

//////////////////////////////////////////////////////////////////////
KLTmpDigits MapCppKLMCDigitizer::make_kl_digits(KLHitArray* hits) const {

  KLTmpDigits tmpDigits(0);
  if (hits->size() == 0) return tmpDigits;

  for ( unsigned int j = 0; j < hits->size(); j++ ) {  //  j-th hit
      KLHit hit = hits->at(j);
      // if (fDebug) std::cout << "hit# " << j << hit << std::endl;

      // make sure we can get the cell info
      if (!hit.GetChannelId())
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "No channel_id in hit",
                       "MapCppKLMCDigitizer::make_kl_digits"));
      double edep = hit.GetEnergyDeposited();

      if (fDebug) {
         std::cout << "klhit: " << hit.GetChannelId()->GetCell() << " "
                   << hit.GetPosition().x() << " " << hit.GetMomentum().x()
                   << " " << hit.GetEnergyDeposited() << std::endl;
      }

      int cll = hit.GetChannelId()->GetCell();

      /*
      if (!root.isMember("daq_event_type")) {
        root["daq_event_type"] = "physics_event";
      }
      */
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
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "No KL module for hit",
                       "MapCppKLMCDigitizer::make_kl_digits"));

      // now get the position of the hit
      ThreeVector tpos = hit.GetPosition();
      Hep3Vector hpos(tpos.x(), tpos.y(), tpos.z());

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


      tmpThisDigit thisDigit;
      // set the hits for PMTs at both ends
      for (int p = 0; p < 2; ++p) {
        thisDigit.fChannelId = hit.GetChannelId();
        thisDigit.fMom = hit.GetMomentum();
        thisDigit.fTime = hit.GetTime();
        thisDigit.fPos = hpos;
        thisDigit.fIsUsed = false;

	// set the key for this channel
        std::stringstream ss;
        ss << "KLChannelKey " << cll << " " << p << " kl";
        thisDigit.fKLKey = ss.str();
        thisDigit.fCell = cll;
        thisDigit.fPmt = p;
        if (p == 0) {
	  thisDigit.fNpe = npe1;
        } else if (p == 1) {
	  thisDigit.fNpe = npe2;
        }

	if (fDebug)
          std::cout << "digit " << j << " key: " << ss.str() << std::endl;

        tmpDigits.push_back(thisDigit);
      } // end loop over pmts
    }  //  ends 'for' loop over hits

    return tmpDigits;
}

//////////////////////////////////////////////////////////////////////
int MapCppKLMCDigitizer::calculate_nphe_at_pmt(double dist, double edep) const {
  if (fDebug) std::cout << "edep= " << edep << std::endl;

  if (!_configJSON.isMember("KLattLengthLong"))
          throw(Exceptions::Exception(Exceptions::recoverable,
		       "Could not find KLattLengthLong in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLattLengthShort"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "Could not find KLattLengthShort in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLattLengthLongNorm"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "Could not find KLattLengthLongNorm in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLattLengthShortNorm"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "Could not find KLattLengthShortNorm in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLconversionFactor"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "Could not find KLconversionFactor in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLlightCollectionEff"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "Could not find KLlightCollectionEff in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLquantumEff"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "Could not find KLquantumEff in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLlightGuideEff"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "Could not find KLlightGuideEff in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLpmtGain"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                       "Could not find KLpmtGain in config",
                       "MapCppKLMCDigitizer::calculate_nphe_at_pmt"));
  if (!_configJSON.isMember("KLpmtSigmaGain"))
          throw(Exceptions::Exception(Exceptions::recoverable,
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
  // int peAmplified = static_cast<int> (pe*gain);

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
void MapCppKLMCDigitizer::fill_kl_evt(int evnum,
                                      KLTmpDigits& tmpDigits,
                                      KLDigitArray* klDigits) const {
  if (!_configJSON.isMember("Do_V1724_Zero_Suppression"))
          throw(Exceptions::Exception(Exceptions::recoverable,
                "Could not find Do_V1724_Zero_Suppression",
                "MapCppKLMCDigitizer::fill_kl_evt"));
  if (!_configJSON.isMember("V1724_Zero_Suppression_Threshold"))
          throw(Exceptions::Exception(Exceptions::recoverable,
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
  if (tmpDigits.size() == 0) return;
  int npe;
  int ndigs = 0;

  // throw out multihits and add up the light yields from multihits in slabs
  for (unsigned int i = 0; i < tmpDigits.size(); ++i) {
    // check if the digit has been used
    if ( !tmpDigits[i].fIsUsed ) {
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
      int adc = static_cast<int>(npe / (_configJSON["KLadcConversionFactor"].asDouble()));

      if (fDebug) std::cout << "npe-adc: " << npe << " " << adc << std::endl;
      // now set the digit
      KLDigit theDigit;
      if (adc > adc_thresh) {
	theDigit.SetChargePm(adc);
	theDigit.SetChargeMm(adc);
	theDigit.SetKlKey(tmpDigits[i].fKLKey);
	theDigit.SetCell(tmpDigits[i].fCell);
	theDigit.SetPmt(tmpDigits[i].fPmt);

	// store event number
	theDigit.SetPhysEventNumber(evnum);
	theDigit.SetPartEventNumber(evnum);

	if (fDebug) std::cout << "adc over thresh: "<< adc << std::endl;

	klDigits->push_back(theDigit);
      }
      if (fDebug)
          std::cout << "digit #" << ndigs << " " <<  theDigit.GetKlKey() << std::endl;
      tmpDigits[i].fIsUsed = true;
    } // ends if-statement
  } // ends k-loop
}

//////////////////////////////////////////////////////////////////////
bool MapCppKLMCDigitizer::check_param(tmpThisDigit& hit1, tmpThisDigit& hit2) const {
  int cell1 = hit1.fCell;
  int cell2 = hit2.fCell;
  int pmt1     = hit1.fPmt;
  int pmt2     = hit2.fPmt;
  int hit_is_used = hit2.fIsUsed;

  if ( cell1 == cell2 && pmt1 == pmt2 && !hit_is_used ) {
    return true;
  } else {
    return false;
  }
}


//=====================================================================
}
