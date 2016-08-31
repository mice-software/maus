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
#include "Utils/KLChannelMap.hh"
#include "Utils/DAQChannelMap.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "API/PyWrapMapBase.hh"

#include "src/map/MapCppKLDigits/MapCppKLDigits.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppKLDigits(void) {
  PyWrapMapBase<MAUS::MapCppKLDigits>::PyWrapMapBaseModInit
                                            ("MapCppKLDigits", "", "", "", "");
}

MapCppKLDigits::MapCppKLDigits() : MapBase<MAUS::Data>("MapCppKLDigits") {
}

void MapCppKLDigits::_birth(const std::string& argJsonConfigDocument) {

  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    throw MAUS::Exceptions::Exception(Exceptions::recoverable,
              "Could not find the $MAUS_ROOT_DIR environmental variable.",
              "MapCppKLDigits::_birth");
  }

  // Check if the JSON document can be parsed, else return error only
  //  JsonCpp setup
  Json::Value configJSON;
  Json::Value map_file_name;
  Json::Value xEnable_V1724_Unpacking;
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
  //  this will contain the configuration

  map_file_name = JsonWrapper::GetProperty(configJSON,
                                           "KL_cabling_file",
                                           JsonWrapper::stringValue);

  std::string xMapFile = std::string(pMAUS_ROOT_DIR) + map_file_name.asString();
  bool loaded = _map.InitFromFile(xMapFile);
  if (!loaded)
    throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                          "Failed to initialise map KLChannelMap",
                          "MapCppKLDigits::_birth");

    // Load the calibration
    bool loaded_cal = _mapcal.InitializeFromCards(configJSON);
    if (!loaded_cal)
    throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                          "Failed to initialise map KLChannelMap",
                          "MapCppKLDigits::_birth");


  xEnable_V1724_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_V1724_Unpacking",
                                                     JsonWrapper::booleanValue);

  if (!xEnable_V1724_Unpacking.asBool()) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppKLDigits::birth. The unpacking of the flashADC V1724 is disabled!!!"
    << " Are you shure you want this?"
    << std::endl;
  }
}


void MapCppKLDigits::_death()  {}

void MapCppKLDigits::_process(MAUS::Data* data) const {

    Spill *spill = data->GetSpill();

    if (spill->GetDAQData() == NULL)
      return;

    if (spill->GetDaqEventType() != "physics_event")
      return;

    TriggerArray        *tr_data     = spill->GetDAQData()->GetTriggerArrayPtr();

    KLArray *kl_data = spill->GetDAQData()->GetKLArrayPtr();

    // Get number of Particle trigger.
    int n_part_event_triggers = tr_data->size();

    int n_part_events = kl_data->size();

    int recPartEvents = spill->GetReconEventSize();
    ReconEventPArray *recEvts =  spill->GetReconEvents();

    // Resize the recon event to harbour all the EMR noise+decays
    if (recPartEvents == 0) { // No recEvts yet
      for (int iPe = 0; iPe < n_part_events; iPe++) {
        recEvts->push_back(new ReconEvent);
      }
    }

  //   std::cerr << "nPartEvts: " << nPartEvents << std::endl;
    for (int xPE = 0; xPE < n_part_events; xPE++) {
      KLEvent *evt = new KLEvent();
      (*recEvts)[xPE]->SetKLEvent(evt);
      KLEventDigit *kl_ev_digit = evt->GetKLEventDigitPtr();
      if (kl_ev_digit == NULL)
          continue;
      KLDigitArray *kl_digits = kl_ev_digit->GetKLDigitArrayPtr();

      /** Create the digits in KL;
       */
      if (kl_data->at(xPE) == NULL)
        continue;

      V1724Array kl_adc_hits = (kl_data->at(xPE))->GetV1724Array();
      int n_adc_hits = kl_adc_hits.size();
      for (int xHit = 0; xHit < n_adc_hits; xHit++) {
        KLDigit the_digit;
        if (!this->getAdc(&the_digit, kl_adc_hits[xHit])) {
          the_digit.SetChargeMm(0);
          the_digit.SetChargePm(0);
        }
        kl_digits->push_back(the_digit);
      }
    }
}

bool MapCppKLDigits::SetConfiguration(std::string json_configuration) {
  return true;
}

bool MapCppKLDigits::getAdc(KLDigit* thisDigit, V1724 &adcHit) const {
  std::stringstream xConv;
  Json::Value xDocInfo;

  DAQChannelKey xAdcDaqKey;

  std::string xDaqKey_adc_str = adcHit.GetChannelKey();
  xConv << xDaqKey_adc_str;
  xConv >> xAdcDaqKey;
  KLChannelKey* xKlAdcKey = _map.find(&xAdcDaqKey);

  // Get the gain factors
  double gain = _mapcal.Gain(*xKlAdcKey);

  // Get the charge
  int char_mm = adcHit.GetChargeMm();
  int char_pm = adcHit.GetChargePm();
  // Equalize charges
  char_mm /= gain;
  char_pm /= gain;

  if (xKlAdcKey) {
    thisDigit->SetKlKey(xKlAdcKey->str());
    thisDigit->SetCell(xKlAdcKey->cell());
    thisDigit->SetPmt(xKlAdcKey->pmt());


    thisDigit->SetChargeMm(char_mm);
    thisDigit->SetChargePm(char_pm);
    thisDigit->SetPositionMax(adcHit.GetPositionMax());
    thisDigit->SetPartEventNumber(adcHit.GetPartEventNumber());
    thisDigit->SetPhysEventNumber(adcHit.GetPhysEventNumber());


     // std::cout << "phys_event_number= " << thisDigit->GetPhysEventNumber() << std::endl;
     // std::cout << "part_event_number= " << thisDigit->GetPartEventNumber() << std::endl;
     // std::cout << "adc before= " << thisDigit->GetChargeMm() << std::endl;
     // std::cout << "adc after= " << char_mm << std::endl;
     // std::cout << "gain= " << gain << std::endl;
     // std::cout << "kl_key= " << thisDigit->GetKlKey() << std::endl;
     // std::cout << "kl_cell= " << thisDigit->GetCell() << std::endl;
     // std::cout << "kl_pmt= " << thisDigit->GetPmt() << std::endl;
     return true;
  } else {
     return false;
  }
}
}
