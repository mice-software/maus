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
#include "Utils/TOFChannelMap.hh"
#include "Utils/DAQChannelMap.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "API/PyWrapMapBase.hh"

#include "src/map/MapCppTOFDigits/MapCppTOFDigits.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTOFDigits(void) {
  PyWrapMapBase<MAUS::MapCppTOFDigits>::PyWrapMapBaseModInit
                                            ("MapCppTOFDigits", "", "", "", "");
}

////////////////////////////////////////////////////////////
MapCppTOFDigits::MapCppTOFDigits()
  : MapBase<MAUS::Data>("MapCppTOFDigits") {
    map_init = false;
    runNumberSave = -1;
}

////////////////////////////////////////////////////////////
void MapCppTOFDigits::_birth(const std::string& argJsonConfigDocument) {
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR)
    throw Exception(Exception::recoverable,
                    "Could not find the $MAUS_ROOT_DIR environmental variable",
                    "MapCppTOFDigits::_birth");

  // Check if the JSON document can be parsed, else return error only
  //  JsonCpp setup
  Json::Value xEnable_V1290_Unpacking;
  Json::Value xEnable_V1724_Unpacking;
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
  //  this will contain the configuration

  xEnable_V1290_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_V1290_Unpacking",
                                                     JsonWrapper::booleanValue);
  xEnable_V1724_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_V1724_Unpacking",
                                                     JsonWrapper::booleanValue);

  if (!xEnable_V1290_Unpacking.asBool()) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppTOFDigits::birth. The unpacking of the TDC V1290 is disabled!!!"
    << " Are you shure you want this?"
    << std::endl;
  }
  if (!xEnable_V1724_Unpacking.asBool()) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppTOFDigits::birth. The unpacking of the flashADC V1724 is disabled!!!"
    << " Are you shure you want this?"
    << std::endl;
  }
}


////////////////////////////////////////////////////////////
void MapCppTOFDigits::_death()  {}

////////////////////////////////////////////////////////////
void MapCppTOFDigits::_process(MAUS::Data *data) const {
  // Get spill, break if there's no DAQ data
  Spill *spill = data->GetSpill();

  if (spill->GetDAQData() == NULL)
    return;

  if (spill->GetDaqEventType() != "physics_event")
    return;

  int runNumber = 0;
  runNumber = spill->GetRunNumber();
  std::cerr << "RunNum = " << runNumber << std::endl;

  if (!map_init || runNumber != runNumberSave) {
      const_cast<MapCppTOFDigits*>(this)->getTofCabling(runNumber);
  }
  TriggerRequestArray *tr_req_data = spill->GetDAQData()->GetTriggerRequestArrayPtr();
  TriggerArray        *tr_data     = spill->GetDAQData()->GetTriggerArrayPtr();

  TOF0DaqArray *tof0_data = spill->GetDAQData()->GetTOF0DaqArrayPtr();
  TOF1DaqArray *tof1_data = spill->GetDAQData()->GetTOF1DaqArrayPtr();
  TOF2DaqArray *tof2_data = spill->GetDAQData()->GetTOF2DaqArrayPtr();

  int nPartEvents = tof0_data->size();
  int recPartEvents = spill->GetReconEventSize();
  ReconEventPArray *recEvts =  spill->GetReconEvents();

  // Resize the recon event to harbour all the EMR noise+decays
  if (recPartEvents == 0) { // No recEvts yet
      for (int iPe = 0; iPe < nPartEvents; iPe++) {
        recEvts->push_back(new ReconEvent);
    }
  }

//   std::cerr << "nPartEvts: " << nPartEvents << std::endl;
  for (int xPE = 0; xPE < nPartEvents; xPE++) {
    TOFEvent *evt = new TOFEvent();
    (*recEvts)[xPE]->SetTOFEvent(evt);
    TOF0DigitArray *tof0_digits = evt->GetTOFEventDigitPtr()->GetTOF0DigitArrayPtr();
    TOF1DigitArray *tof1_digits = evt->GetTOFEventDigitPtr()->GetTOF1DigitArrayPtr();
    TOF2DigitArray *tof2_digits = evt->GetTOFEventDigitPtr()->GetTOF2DigitArrayPtr();

    // Get the trigger and the trigger requests.
    V1290Array tr_hits     = (*tr_data)[xPE]->GetV1290Array();
    V1290Array tr_req_hits = (*tr_req_data)[xPE]->GetV1290Array();

    /** Create the digits in TOF0;
     */
    V1724Array tof0_adc_hits = (*tof0_data)[xPE]->GetV1724Array();
    V1290Array tof0_tdc_hits = (*tof0_data)[xPE]->GetV1290Array();

    // Loop over the tdc hits in TOF0.
    int nHits = tof0_tdc_hits.size();
    if (tof0_adc_hits.size())
    for (int xHit = 0; xHit < nHits; xHit++) {
      TOFDigit the_digit;
      this->setTdc(&the_digit, tof0_tdc_hits[xHit]);
      if (!this->findAdc(&the_digit, tof0_adc_hits)) {
        the_digit.SetChargeMm(0);
        the_digit.SetChargePm(0);
      }
      if (this->findTrigger(&the_digit, tof0_tdc_hits[xHit], tr_hits) &&
          this->findTriggerReq(&the_digit, tof0_tdc_hits[xHit], tr_req_hits))
        tof0_digits->push_back(the_digit);
    }

    /** Create the digits in TOF1;
     */
    V1724Array tof1_adc_hits = (*tof1_data)[xPE]->GetV1724Array();
    V1290Array tof1_tdc_hits = (*tof1_data)[xPE]->GetV1290Array();

    // Loop over the tdc hits in TOF1.
    nHits = tof1_tdc_hits.size();
    if (tof1_adc_hits.size())
    for (int xHit = 0; xHit < nHits; xHit++) {
      TOFDigit the_digit;
      this->setTdc(&the_digit, tof1_tdc_hits[xHit]);
      if (!this->findAdc(&the_digit, tof1_adc_hits)) {
        the_digit.SetChargeMm(0);
        the_digit.SetChargePm(0);
      }
      if (this->findTrigger(&the_digit, tof1_tdc_hits[xHit], tr_hits) &&
          this->findTriggerReq(&the_digit, tof1_tdc_hits[xHit], tr_req_hits))
        tof1_digits->push_back(the_digit);
    }

    /** Create the digits in TOF2;
     */
    V1724Array tof2_adc_hits = (*tof2_data)[xPE]->GetV1724Array();
    V1290Array tof2_tdc_hits = (*tof2_data)[xPE]->GetV1290Array();

    // Loop over the tdc hits in TOF2.
    nHits = tof2_tdc_hits.size();
    if (tof2_adc_hits.size())
    for (int xHit = 0; xHit < nHits; xHit++) {
      TOFDigit the_digit;
      this->setTdc(&the_digit, tof2_tdc_hits[xHit]);
      if (!this->findAdc(&the_digit, tof2_adc_hits)) {
        the_digit.SetChargeMm(0);
        the_digit.SetChargePm(0);
      }
      if (this->findTrigger(&the_digit, tof2_tdc_hits[xHit], tr_hits) &&
          this->findTriggerReq(&the_digit, tof2_tdc_hits[xHit], tr_req_hits))
        tof2_digits->push_back(the_digit);
    }
  }
}

////////////////////////////////////////////////////////////
void MapCppTOFDigits::setTdc(MAUS::TOFDigit *digit, MAUS::V1290 &tdc) const {
  std::stringstream xConv;
  DAQChannelKey xTdcDaqKey;
  std::string xDaqKey_tdc_str = tdc.GetChannelKey();
  xConv << xDaqKey_tdc_str;
  xConv >> xTdcDaqKey;
  TOFChannelKey* xTofTdcKey = _map.find(&xTdcDaqKey);

  digit->SetPmt(xTofTdcKey->pmt());
  digit->SetSlab(xTofTdcKey->slab());
  digit->SetPlane(xTofTdcKey->plane());
  digit->SetStation(xTofTdcKey->station());
  digit->SetTofKey(xTofTdcKey->str());

  digit->SetLeadingTime(tdc.GetLeadingTime());
  digit->SetTrailingTime(tdc.GetTrailingTime());
  digit->SetTimeStamp(tdc.GetTimeStamp());
  digit->SetTriggerTimeTag(tdc.GetTriggerTimeTag());

  digit->SetPhysEventNumber(tdc.GetPhysEventNumber());
  digit->SetPartEventNumber(tdc.GetPartEventNumber());
}

////////////////////////////////////////////////////////////
bool MapCppTOFDigits::findAdc(MAUS::TOFDigit *digit,
                              MAUS::V1724Array &adc_hits) const throw(Exception) {
  int n_Adc_hits = adc_hits.size();

  std::string xTofKey_str = digit->GetTofKey();
  TOFChannelKey xTdcTofKey(xTofKey_str);

  for (int AdcHitCount = 0; AdcHitCount < n_Adc_hits; AdcHitCount++) {
    std::string xDaqKey_adc_str = adc_hits[AdcHitCount].GetChannelKey();

    DAQChannelKey xAdcDaqKey(xDaqKey_adc_str);
    TOFChannelKey* xAdcTofKey = _map.find(&xAdcDaqKey);

    if (xTdcTofKey == *xAdcTofKey) {
      digit->SetChargeMm(adc_hits[AdcHitCount].GetChargeMm());
      digit->SetChargePm(adc_hits[AdcHitCount].GetChargePm());

      if (digit->GetPartEventNumber() != adc_hits[AdcHitCount].GetPartEventNumber()) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong part_event_number!"),
              "MapCppTOFDigits::findAdc"));
      }
      if (digit->GetPhysEventNumber() != adc_hits[AdcHitCount].GetPhysEventNumber()) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong phys_event_number!"),
              "MapCppTOFDigits::findAdc"));
      }

      return true;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////
bool MapCppTOFDigits::findTrigger(MAUS::TOFDigit *digit, MAUS::V1290 &tdc,
                                  MAUS::V1290Array &tr_hits) const  throw(Exception) {
  int n_tr_hits = tr_hits.size();

  int xGeo = tdc.GetGeo();

  for (int TrHitCount = 0; TrHitCount < n_tr_hits; TrHitCount++) {
    int tr_Geo = tr_hits[TrHitCount].GetGeo();
    if (xGeo == tr_Geo) {
      digit->SetTriggerLeadingTime(tr_hits[TrHitCount].GetLeadingTime());
      digit->SetTriggerTrailingTime(tr_hits[TrHitCount].GetTrailingTime());

      if (digit->GetPartEventNumber() != tr_hits[TrHitCount].GetPartEventNumber()) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong part_event_number!"),
              "MapCppTOFDigits::findTrigger"));
      }
      if (digit->GetPhysEventNumber() != tr_hits[TrHitCount].GetPhysEventNumber()) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong phys_event_number!"),
              "MapCppTOFDigits::findTrigger"));
      }

      return true;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////
bool MapCppTOFDigits::findTriggerReq(MAUS::TOFDigit *digit, MAUS::V1290 &tdc,
                                  MAUS::V1290Array &tr_req_hits) const  throw(Exception) {
  int n_tr_hits = tr_req_hits.size();

  int xGeo = tdc.GetGeo();

  for (int TrReqHitCount = 0; TrReqHitCount < n_tr_hits; TrReqHitCount++) {
    int tr_Geo = tr_req_hits[TrReqHitCount].GetGeo();
    if (xGeo == tr_Geo) {
      digit->SetTriggerRequestLeadingTime(tr_req_hits[TrReqHitCount].GetLeadingTime());
      digit->SetTriggerRequestTrailingTime(tr_req_hits[TrReqHitCount].GetTrailingTime());

      if (digit->GetPartEventNumber() != tr_req_hits[TrReqHitCount].GetPartEventNumber()) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong part_event_number!"),
              "MapCppTOFDigits::findTriggerReq"));
      }
      if (digit->GetPhysEventNumber() != tr_req_hits[TrReqHitCount].GetPhysEventNumber()) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong phys_event_number!"),
              "MapCppTOFDigits::findTriggerReq"));
      }

      return true;
    }
  }

  return false;
}
////////////////////////////////////////////////////////////
void MapCppTOFDigits::getTofCabling(int runNumber) {
  // Load the cabling map.
  runNumberSave = runNumber;
  map_init = _map.InitializeCards(configJSON, runNumber);
  if (!map_init) {
      throw MAUS::Exception(Exception::recoverable,
                          "Failed to initialize cabling map",
                          "MapCppTOFDigits::_process");
  }
}
////////////////////////////////////////////////////////////
} // end namespace MAUS
