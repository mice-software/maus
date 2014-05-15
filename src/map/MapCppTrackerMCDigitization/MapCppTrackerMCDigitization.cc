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
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/map/MapCppTrackerMCDigitization/MapCppTrackerMCDigitization.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTrackerMCDigitization(void) {
  PyWrapMapBase<MAUS::MapCppTrackerMCDigitization>::PyWrapMapBaseModInit
                                                ("", "", "", "");
}

MapCppTrackerMCDigitization::MapCppTrackerMCDigitization()
    : MapBase<Data>("MapCppTrackerMCDigitization") {
}

MapCppTrackerMCDigitization::~MapCppTrackerMCDigitization() {
}

void MapCppTrackerMCDigitization::_birth(const std::string& argJsonConfigDocument) {
  if (!Globals::HasInstance()) {
    GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
  }
  static MiceModule* mice_modules = Globals::GetMonteCarloMiceModules();
  modules = mice_modules->findModulesByPropertyString("SensitiveDetector", "SciFi");
  Json::Value *json = Globals::GetConfigurationCards();
  // Load constants.
  _SciFiNPECut        = (*json)["SciFiNoiseNPECut"].asDouble();
  _SciFivlpcEnergyRes = (*json)["SciFivlpcEnergyRes"].asDouble();
  _SciFiadcFactor     = (*json)["SciFiadcFactor"].asDouble();
  _SciFitdcBits       = (*json)["SciFitdcBits"].asDouble();
  _SciFivlpcTimeRes   = (*json)["SciFivlpcTimeRes"].asDouble();
  _SciFitdcFactor     = (*json)["SciFitdcFactor"].asDouble();
  _SciFiFiberConvFactor  = (*json)["SciFiFiberConvFactor"].asDouble();
  _SciFiFiberTrappingEff = (*json)["SciFiFiberTrappingEff"].asDouble();
  _SciFiFiberMirrorEff   = (*json)["SciFiFiberMirrorEff"].asDouble();
  _SciFivlpcQE           = (*json)["SciFivlpcQE"].asDouble();
  _SciFiFiberTransmissionEff = (*json)["SciFiFiberTransmissionEff"].asDouble();
  _SciFiMUXTransmissionEff   = (*json)["SciFiMUXTransmissionEff"].asDouble();
  _eV_to_phe = _SciFiFiberConvFactor *
               _SciFiFiberTrappingEff *
               ( 1.0 + _SciFiFiberMirrorEff ) *
               _SciFiFiberTransmissionEff *
               _SciFiMUXTransmissionEff *
               _SciFivlpcQE;
}

void MapCppTrackerMCDigitization::_death() {
}

void MapCppTrackerMCDigitization::_process(MAUS::Data* data) const {
  Spill& spill = *(data->GetSpill());

  if (!spill.GetMCEvents() ) {
    throw MAUS::Exception(Exception::recoverable,
            "MC event array not initialised.",
            "MapCppTrackerMCDigitization::process");
  }

  // ================= Reconstruction =========================

  // Check the Recon event array is initialised, and if not make it so
  if ( !spill.GetReconEvents() ) {
    ReconEventPArray* revts = new ReconEventPArray();
    spill.SetReconEvents(revts);
  }
  // Construct digits from hits for each MC event
  for ( size_t event_i = 0; event_i < spill.GetMCEventSize(); event_i++ ) {
    MCEvent *mc_evt = spill.GetMCEvents()->at(event_i);
    SciFiDigitPArray digits;
    if ( mc_evt->GetSciFiHits() ) {
      construct_digits(mc_evt->GetSciFiHits(), spill.GetSpillNumber(), event_i, digits);
    }
    if ( mc_evt->GetSciFiNoiseHits() ) {
      add_noise(mc_evt->GetSciFiNoiseHits(), digits);
    }

    // Make a SciFiEvent to hold the digits produced
    SciFiEvent *sf_evt = new SciFiEvent();
    sf_evt->set_digits(digits);

    // If there is already a Recon event associated with this MC event, add the SciFiEvent to it,
    // otherwise make a new Recon event to hold the SciFiEvent
    if ( spill.GetReconEvents()->size() > event_i ) {
      spill.GetReconEvents()->at(event_i)->SetSciFiEvent(sf_evt);
    } else {
      ReconEvent *revt = new ReconEvent();
      revt->SetPartEventNumber(event_i);
      revt->SetSciFiEvent(sf_evt);
      spill.GetReconEvents()->push_back(revt);
    }
  }
}

void MapCppTrackerMCDigitization::construct_digits(SciFiHitArray *hits, int spill_num,
                                                   int event_num, SciFiDigitPArray &digits) const {
  SciFiLookup lookup;
  for ( unsigned int hit_i = 0; hit_i < hits->size(); hit_i++ ) {
    if ( !hits->at(hit_i).GetChannelId()->GetUsed() ) {

      SciFiHit *a_hit = &hits->at(hit_i);
      a_hit->GetChannelId()->SetUsed(true);

      int chanNo = compute_chan_no(a_hit);

       // Get nPE from this hit.
      double edep = a_hit->GetEnergyDeposited();
      double nPE  = edep*_eV_to_phe;

      // Compute tdc count.
      double time1   = a_hit->GetTime();
      // int tdcCounts = compute_tdc_counts(time1);

      // Pull tracker/station/plane information from geometry
      int tracker = a_hit->GetChannelId()->GetTrackerNumber();
      int station = a_hit->GetChannelId()->GetStationNumber();
      int plane   = a_hit->GetChannelId()->GetPlaneNumber();

      // Create Digit for use with MC lookup, will update NPE later
      SciFiDigit *a_digit = new SciFiDigit(spill_num, event_num,
                                           tracker, station, plane, chanNo, nPE, time1);

      a_hit->GetChannelId()->SetID(lookup.get_digit_id(a_digit));

      // Loop over all the other hits.
      for ( unsigned int hit_j = hit_i+1; hit_j < hits->size(); hit_j++ ) {
        if ( check_param(&(*hits)[hit_i], &(*hits)[hit_j]) ) {
          MAUS::SciFiHit *same_digit = &(*hits)[hit_j];
          double edep_j = same_digit->GetEnergyDeposited();
          nPE += edep_j*_eV_to_phe;
          same_digit->GetChannelId()->SetUsed(true);
          same_digit->GetChannelId()->SetID(lookup.get_digit_id(a_digit));
        }
      } // ends loop over all the array

      a_digit->set_npe(nPE);

      // .start. TO BE REMOVED .start.//
      ThreeVector position = a_hit->GetPosition();
      ThreeVector momentum = a_hit->GetMomentum();
      a_digit->set_true_position(position);
      a_digit->set_true_momentum(momentum);
      // .end. TO BE REMOVED .end.//

      digits.push_back(a_digit);
    }
  } // ends 'for' loop over hits
}

void MapCppTrackerMCDigitization::add_noise(SciFiNoiseHitArray *noises,
                                            SciFiDigitPArray &digits) const {

    /**************************************************************************
    *  Function checks which channel has noise against which channel has a
    *  digit.  If noise is in the same channel as a digit then the noise is 
    *  added to the digit.  If the noise is removed from the digit by one 
    *  channel, then a new digit is created.
    *  Regardless, if noise is over the NPE cut off, 2 NPE, then a new digit
    *  is created.
    **************************************************************************/

  SciFiLookup lookup;

  for (unsigned int j = 0; j < noises->size(); j++) {
    for (unsigned int i = 0; i < digits.size(); i++) {

      // Checks if noise is in the same channel as a digit
      if (digits[i]->get_tracker() == noises->at(j).GetTracker() &&
          digits[i]->get_station() == noises->at(j).GetStation() &&
          digits[i]->get_plane()   == noises->at(j).GetPlane()   &&
          digits[i]->get_channel() == noises->at(j).GetChannel() &&
          noises->at(j).GetUsed() == false) {
        double digit_npe = digits[i]->get_npe();
        double noise_npe = noises->at(j).GetNPE();
        digits[i]->set_npe(digit_npe + noise_npe);
        noises->at(j).SetUsed(true);
        noises->at(j).SetID(static_cast<double>(lookup.get_digit_id(digits[i])));
        continue;

        // Checks if noise is one channel removed from a digit
      } else if (digits[i]->get_tracker() == noises->at(j).GetTracker() &&
                 digits[i]->get_station() == noises->at(j).GetStation() &&
                 digits[i]->get_plane()   == noises->at(j).GetPlane()   &&
                 abs(digits[i]->get_channel() - noises->at(j).GetChannel()) <= 1.0 &&
                 noises->at(j).GetUsed() == false) {
        SciFiNoiseHit a_noise = noises->at(j);
        SciFiDigit* a_digit = new SciFiDigit(a_noise.GetSpill(), a_noise.GetEvent(),
                           a_noise.GetTracker(), a_noise.GetStation(), a_noise.GetPlane(),
                           a_noise.GetChannel(), a_noise.GetNPE(), a_noise.GetTime());
        digits.push_back(a_digit);
        noises->at(j).SetUsed(true);
        noises->at(j).SetID(static_cast<double>(lookup.get_digit_id(a_digit)));
        continue;
      }
    }

    // Checks if noise is above NPE cutoff
    if (noises->at(j).GetNPE() >= _SciFiNPECut &&
        noises->at(j).GetUsed() == false) {
      SciFiNoiseHit a_noise = noises->at(j);
      SciFiDigit* a_digit = new SciFiDigit(a_noise.GetSpill(), a_noise.GetEvent(),
                                           a_noise.GetTracker(), a_noise.GetStation(),
                                           a_noise.GetPlane(), a_noise.GetChannel(),
                                           a_noise.GetNPE(), a_noise.GetTime());
      digits.push_back(a_digit);
      noises->at(j).SetUsed(true);
      noises->at(j).SetID(static_cast<double>(lookup.get_digit_id(a_digit)));
    }
  }
}

int MapCppTrackerMCDigitization::compute_tdc_counts(double time1) const {
  double tmpcounts;

  tmpcounts = CLHEP::RandGauss::shoot(time1, _SciFivlpcTimeRes)*_SciFitdcFactor;

  // Check for saturation of TDCs
  if ( tmpcounts > pow(2.0, _SciFitdcBits) - 1.0 ) {
    tmpcounts = pow(2.0, _SciFitdcBits) - 1.0;
  }

  int tdcCounts = static_cast <int> (floor(tmpcounts));
  return tdcCounts;
}

int MapCppTrackerMCDigitization::compute_chan_no(MAUS::SciFiHit *ahit) const {
  // This is the channel number computed from the fibre number.
  int fiberNumber = ahit->GetChannelId()->GetFibreNumber();
  int chanNo      = static_cast<int> (floor(fiberNumber/7.0));

  return chanNo;
}

int MapCppTrackerMCDigitization::compute_adc_counts(double numb_pe) const {
  double tmpcounts;
  if ( numb_pe == 0 ) return 0;

  // Throw the dice and generate the ADC count
  // value for the energy summed for each channel.
  tmpcounts = CLHEP::RandGauss::shoot(numb_pe, _SciFivlpcEnergyRes) *
              _SciFiadcFactor;

  // Check for saturation of ADCs
  if ( tmpcounts > pow(2.0, _SciFitdcBits) - 1.0 ) {
    tmpcounts = pow(2.0, _SciFitdcBits) - 1.0;
  }

  int adcCounts = static_cast <int> (floor(tmpcounts));
  return adcCounts;
}

bool MapCppTrackerMCDigitization::check_param(MAUS::SciFiHit *hit1, MAUS::SciFiHit *hit2) const {
  if ( hit2->GetChannelId()->GetUsed() ) {
    return false;
  } else {
    // two hits passed to the check_param function
    int tracker1 = hit1->GetChannelId()->GetTrackerNumber();
    int tracker2 = hit2->GetChannelId()->GetTrackerNumber();
    int station1 = hit1->GetChannelId()->GetStationNumber();
    int station2 = hit2->GetChannelId()->GetStationNumber();
    int plane1   = hit1->GetChannelId()->GetPlaneNumber();
    int plane2   = hit2->GetChannelId()->GetPlaneNumber();
    double chan1 = compute_chan_no(hit1);
    double chan2 = compute_chan_no(hit2);

    // check whether the hits belong to the same tracker, station, plane and channel
    if ( tracker1 == tracker2 && station1 == station2 &&
         plane1 == plane2 && chan1 == chan2 ) {
      return true;
    } else {
      return false;
    }
  }
}
} // ~namespace MAUS

