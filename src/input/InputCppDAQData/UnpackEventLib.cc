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

#include "src/input/InputCppDAQData/UnpackEventLib.hh"

namespace MAUS {

int V1290DataProcessor::Process(MDdataContainer* aPartEventPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentV1290 argument
  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1290) )  return CastError;
  MDpartEventV1290* xV1290Evnt = static_cast<MDpartEventV1290*>(aPartEventPtr);

  Json::Value pBoardDoc;
  Json::Value pTdcHit;

  string xDetector = "unknown";
  if ( !xV1290Evnt->IsValid() )
    return GenericError;

  int xLdc, xGeo, xEquip, xPartEv;

  // Put static data into the Json
  pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
  pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
  pBoardDoc["time_stamp"]          = this->GetTimeStamp();
  pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
  pBoardDoc["part_event_number"] = xPartEv = this->GetPartEventNumber();
  pBoardDoc["geo"]          = xGeo = xV1290Evnt->GetGeo();
  pBoardDoc["trigger_time_tag"]    = xV1290Evnt->GetTriggerTimeTag();
  // Loop over all the channels
  for (unsigned int xCh = 0; xCh < V1290_NCHANNELS; xCh++) {
    pTdcHit = pBoardDoc;
    // Get the number of leading edge hits.
    int xHitsL = xV1290Evnt->GetNHits(xCh, 'l');
    // Get the number of trailing edge hits.
    int xHitsT = xV1290Evnt->GetNHits(xCh, 't');

    // By definition the number of the leading edge hits has to be equal
    // to the number of the trailing edge hits but some times this is not true
    int xMaxHits = MAX(xHitsL, xHitsT);

    // Loop over all leading and trailing hits in this channel.
    int xLT, xTT;  // Lead and Trail times
    for (int j = 0; j < xMaxHits; j++) {
      if (j < xHitsL) {
        xLT = xV1290Evnt->GetHitMeasurement(j,  // Hit ID
                                            xCh,  // Channel ID
                                            'l');
      } else {
        // If no leading edge hit set the value og time to -99.
        xLT = -99;
      }

      if (j < xHitsT) {
        xTT = xV1290Evnt->GetHitMeasurement(j,  // Hit ID
                                            xCh,  // Channel ID
                                           't');
      } else {
        // If no trailing edge hit set the value og time to -99.
        xTT = -99;
      }

      int xBunchID = xV1290Evnt->GetBunchID(xCh/8);
      Json::Value xTdcHit;
      pTdcHit["bunch_id"]      = xBunchID;
      pTdcHit["channel"]       = xCh;
      pTdcHit["leading_time"]  = xLT;
      pTdcHit["trailing_time"] = xTT;
      DAQChannelKey* xKey = _chMap->find(xLdc, xGeo, xCh, xEquip);
      if (xKey) {
        pTdcHit["channel_key"] = xKey->str();
        xDetector = xKey->detector();
        pTdcHit["detector"] = xDetector;
      } else {
        pTdcHit["detector"] = xDetector = "unknown";
      }
      ( *_docSpill )[xDetector][ xPartEv ][_equipment.c_str()].append(pTdcHit);
    }
  }

  return OK;
}

int V1290CppDataProcessor::Process(MDdataContainer* aPartEventPtr) {
  /**
   * Cast the argument to structure it points to.
   * This process should be called only with MDfragmentV1290 argument.
   */
  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1290) )  return CastError;
  MDpartEventV1290* xV1290Evnt = static_cast<MDpartEventV1290*>(aPartEventPtr);
//   xV1290Evnt->Dump();

//   string xDetector = "emr";
  if ( !xV1290Evnt->IsValid() )
    return GenericError;

  unsigned int xPartEv = this->GetPartEventNumber();
//   cerr << "This is V1290CppDataProcessor::Process " << xPartEv << endl;
  if (xPartEv+1 > _tof0_spill.size()) {
    _tof0_spill.resize(xPartEv+1);
    _tof1_spill.resize(xPartEv+1);
    _tof2_spill.resize(xPartEv+1);
    _tr_spill.resize(xPartEv+1);
    _tr_req_spill.resize(xPartEv+1);
    _unknown_spill.resize(xPartEv+1);
  }
  // Put static data into the Data Structure.

  MAUS::V1290 xV1290hit;
  int xLdc, xGeo, xEquip;

  // Put static data
  xV1290hit.SetLdcId(xLdc = this->GetLdcId());
  xV1290hit.SetEquipType(xEquip = this->GetEquipmentType());
  xV1290hit.SetTimeStamp(this->GetTimeStamp());
  xV1290hit.SetPhysEventNumber(this->GetPhysEventNumber());
  xV1290hit.SetPartEventNumber(xPartEv);
  xV1290hit.SetGeo(xGeo = xV1290Evnt->GetGeo());
  xV1290hit.SetTriggerTimeTag(xV1290Evnt->GetTriggerTimeTag());

  // Loop over all the channels
  for (unsigned int xCh = 0; xCh < V1290_NCHANNELS; xCh++) {

    // Get the number of leading edge hits.
    int xHitsL = xV1290Evnt->GetNHits(xCh, 'l');
    // Get the number of trailing edge hits.
    int xHitsT = xV1290Evnt->GetNHits(xCh, 't');

    // By definition the number of the leading edge hits has to be equal
    // to the number of the trailing edge hits but some times this is not true
    int xMaxHits = MAX(xHitsL, xHitsT);

    // Loop over all leading and trailing hits in this channel.
    int xLT, xTT;  // Lead and Trail times
    for (int j = 0; j < xMaxHits; j++) {
      if (j < xHitsL) {
        xLT = xV1290Evnt->GetHitMeasurement(j,  // Hit ID
                                            xCh,  // Channel ID
                                            'l');
      } else {
        // If no leading edge hit set the value og time to -99.
        xLT = -99;
      }

      if (j < xHitsT) {
        xTT = xV1290Evnt->GetHitMeasurement(j,  // Hit ID
                                            xCh,  // Channel ID
                                           't');
      } else {
        // If no trailing edge hit set the value og time to -99.
        xTT = -99;
      }

      int xBunchID = xV1290Evnt->GetBunchID(xCh/8);
      xV1290hit.SetBunchId(xBunchID);
      xV1290hit.SetChannel(xCh);
      xV1290hit.SetLeadingTime(xLT);
      xV1290hit.SetTrailingTime(xTT);

      string xDetector;
      DAQChannelKey* xKey = _chMap->find(xLdc, xGeo, xCh, xEquip);
      if (xKey) {
        xV1290hit.SetChannelKey(xKey->str());
        xDetector = xKey->detector();
      } else {
        xV1290hit.SetChannelKey("unknown");
        xDetector = "unknown";
      }
      xV1290hit.SetDetector(xDetector);

      if (xDetector == "tof0")
       _tof0_spill[xPartEv].push_back(xV1290hit);
      else if (xDetector == "tof1")
       _tof1_spill[xPartEv].push_back(xV1290hit);
      else if (xDetector == "tof2")
       _tof2_spill[xPartEv].push_back(xV1290hit);
      else if (xDetector == "trigger")
        _tr_spill[xPartEv].push_back(xV1290hit);
      else if (xDetector == "trigger_request")
        _tr_req_spill[xPartEv].push_back(xV1290hit);
      else
       _unknown_spill[xPartEv].push_back(xV1290hit);
    }
  }

  return OK;
}

void V1290CppDataProcessor::fill_daq_data() {

  unsigned int npe = _tof1_spill.size();

  if (_daq_data->GetTOF0DaqArraySize() != npe)
    _daq_data->GetTOF0DaqArrayPtr()->resize(npe);

  if (_daq_data->GetTOF1DaqArraySize() != npe)
    _daq_data->GetTOF1DaqArrayPtr()->resize(npe);

  if (_daq_data->GetTOF2DaqArraySize() != npe)
    _daq_data->GetTOF2DaqArrayPtr()->resize(npe);

  if (_daq_data->GetTriggerRequestArraySize() != npe)
    _daq_data->GetTriggerRequestArrayPtr()->resize(npe);

  if (_daq_data->GetTriggerArraySize() != npe)
    _daq_data->GetTriggerArrayPtr()->resize(npe);

  if (_daq_data->GetUnknownArraySize() != npe)
    _daq_data->GetUnknownArrayPtr()->resize(npe);


  for (unsigned int ipe = 0; ipe < npe; ipe++) {
    MAUS::TOFDaq *tof0_daq = _daq_data->GetTOF0DaqArrayElement(ipe);
    if (!tof0_daq)
      (*_daq_data->GetTOF0DaqArrayPtr())[ipe] = new MAUS::TOFDaq;

    _daq_data->GetTOF0DaqArrayElement(ipe)->SetV1290Array(_tof0_spill[ipe]);

    MAUS::TOFDaq *tof1_daq = _daq_data->GetTOF1DaqArrayElement(ipe);
    if (!tof1_daq)
      (*_daq_data->GetTOF1DaqArrayPtr())[ipe] = new MAUS::TOFDaq;

    _daq_data->GetTOF1DaqArrayElement(ipe)->SetV1290Array(_tof1_spill[ipe]);

    MAUS::TOFDaq *tof2_daq = _daq_data->GetTOF2DaqArrayElement(ipe);
    if (!tof2_daq)
      (*_daq_data->GetTOF2DaqArrayPtr())[ipe] = new MAUS::TOFDaq;

    _daq_data->GetTOF2DaqArrayElement(ipe)->SetV1290Array(_tof2_spill[ipe]);

    MAUS::Trigger *tr_daq = _daq_data->GetTriggerArrayElement(ipe);
    if (!tr_daq)
      (*_daq_data->GetTriggerArrayPtr())[ipe] = new MAUS::Trigger;

    _daq_data->GetTriggerArrayElement(ipe)->SetV1290Array(_tr_spill[ipe]);

    MAUS::TriggerRequest *tr_req_daq = _daq_data->GetTriggerRequestArrayElement(ipe);
    if (!tr_req_daq)
      (*_daq_data->GetTriggerRequestArrayPtr())[ipe] = new MAUS::TriggerRequest;

    _daq_data->GetTriggerRequestArrayElement(ipe)->SetV1290Array(_tr_req_spill[ipe]);

    MAUS::Unknown *unknown = _daq_data->GetUnknownArrayElement(ipe);
    if (!unknown)
      (*_daq_data->GetUnknownArrayPtr())[ipe] = new MAUS::Unknown;
    _daq_data->GetUnknownArrayElement(ipe)->SetV1290Array(_unknown_spill[ipe]);
  }
}

void V1290CppDataProcessor::reset() {
  _tof0_spill.clear();
  _tof1_spill.clear();
  _tof2_spill.clear();
  _tr_spill.clear();
  _tr_req_spill.clear();
  _unknown_spill.clear();
}

////////////////////////////////////////////////////////////////////////////////

int V1724DataProcessor::Process(MDdataContainer* aPartEventPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentV1724 argument.
  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1724) )  return CastError;
  MDpartEventV1724* xV1724Evnt = static_cast<MDpartEventV1724*>(aPartEventPtr);

  Json::Value pBoardDoc;
  Json::Value xfAdcHit;
  int xLdc, xGeo, xEquip, xPartEv;
  string xDetector = "unknown";
  if ( !xV1724Evnt->IsValid() )
    return GenericError;

  // Put static data into the Json
  pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
  pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
  pBoardDoc["time_stamp"]          = this->GetTimeStamp();
  pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
  pBoardDoc["part_event_number"] = xPartEv = this->GetPartEventNumber();
  pBoardDoc["geo"]          = xGeo = xV1724Evnt->GetGeo();
  pBoardDoc["trigger_time_tag"]    = xV1724Evnt->GetTriggerTimeTag();
  // Loop over all the channels
  for (unsigned int xCh = 0; xCh < V1724_NCHANNELS; xCh++) {
    unsigned int xSamples = ( xV1724Evnt->GetLength(xCh) )*V1724_SAMPLES_PER_WORD;
    for (unsigned int j = 0; j < xSamples; j++) {
      int sample = xV1724Evnt->GetSampleData(xCh,  // Channel ID
                                             j);  // Sample ID
      _data.push_back(sample);
    }
    xfAdcHit = pBoardDoc;
    this->set_pedestal();
    int charge_mm = this->get_charge(ceaMinMax);
    if ( !_zero_suppression ||
         (_zero_suppression && charge_mm > _zs_threshold) ) {
      xfAdcHit["charge_mm"]    = charge_mm;
      xfAdcHit["charge_pm"]    = this->get_charge(ceaPedMax);
      xfAdcHit["position_max"] = this->get_max_position();
      xfAdcHit["pedestal"]     = this->get_pedestal();
      DAQChannelKey* xKey = _chMap->find(xLdc, xGeo, xCh, xEquip);
      if (xKey) {
        xDetector = xKey->detector();
        xfAdcHit["channel_key"]   = xKey->str();
        xfAdcHit["detector"]      = xDetector;
      } else {
        xfAdcHit["detector"] = xDetector = "unknown";
      }
      xfAdcHit["channel"]        = xCh;

      ( *_docSpill )[xDetector][ xPartEv ][_equipment].append(xfAdcHit);
    }
    _data.resize(0);
  }

  return OK;
}

int V1724CppDataProcessor::Process(MDdataContainer* aPartEventPtr) {
  /**
   * Cast the argument to structure it points to.
   * This process should be called only with MDfragmentV1724 argument.
   */

  // cout << "This is V1724CppDataProcessor::Process" << endl;

  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1724) )  return CastError;
  MDpartEventV1724* xV1724Evnt = static_cast<MDpartEventV1724*>(aPartEventPtr);
//   xV1724Evnt->Dump();

  if ( !xV1724Evnt->IsValid() )
    return GenericError;

  unsigned int xEvCounter = xV1724Evnt->GetEventCounter(); // starts from 1.
  if (xEvCounter > _tof0_spill.size()) {
    _tof0_spill.resize(xEvCounter);
    _tof1_spill.resize(xEvCounter);
    _tof2_spill.resize(xEvCounter);
    _kl_spill.resize(xEvCounter);
    _tag_spill.resize(xEvCounter);
    _unknown_spill.resize(xEvCounter);
}
  // Put static data into the Data Structure.

  MAUS::V1724 xV1724hit;
  int xLdc   = this->GetLdcId();
  int xGeo   = xV1724Evnt->GetGeo();
  int xEquip = this->GetEquipmentType();
  xV1724hit.SetLdcId(xLdc);
  xV1724hit.SetGeo(xGeo);
  xV1724hit.SetEquipType(xEquip);

  // Loop over all the channels
  for (unsigned int xCh = 0; xCh < V1724_NCHANNELS; xCh++) {

    if (xV1724Evnt->GetLength(xCh) == 0) continue;

    unsigned int xSamples = ( xV1724Evnt->GetLength(xCh) )*V1724_SAMPLES_PER_WORD;
    for (unsigned int j = 0; j < xSamples; j++) {
      int sample = xV1724Evnt->GetSampleData(xCh,  // Channel ID
                                             j);  // Sample ID
      _data.push_back(sample);
    }
    xV1724hit.SetSampleArray(_data);
    this->set_pedestal();
    int charge_mm = this->get_charge(ceaMinMax);

    if ( !_zero_suppression ||
       (_zero_suppression && charge_mm > _zs_threshold) ) {
      xV1724hit.SetChannel(xCh);
      string xDetector;
      DAQChannelKey* xKey = _chMap->find(xLdc, xGeo, xCh, xEquip);
      if (xKey) {
        xV1724hit.SetChannelKey(xKey->str());
        xDetector = xKey->detector();
      } else {
        xDetector = "unknown";
      }

      xV1724hit.SetDetector(xDetector);
      xV1724hit.SetPartEventNumber(xV1724Evnt->GetEventCount());
      xV1724hit.SetPhysEventNumber(this->GetPhysEventNumber());
      xV1724hit.SetTimeStamp(this->GetTimeStamp());
      xV1724hit.SetChargeMm(this->get_charge(ceaMinMax));
      xV1724hit.SetChargePm(this->get_charge(ceaPedMax));
      int max_position = 0;
      xV1724hit.SetPulseArea(this->get_pos_signal_area(max_position));
      xV1724hit.SetPositionMin(this->get_min_position());
      xV1724hit.SetPositionMax(max_position);
      xV1724hit.SetArrivalTime(this->get_arrival_time());
      xV1724hit.SetTriggerTimeTag(xV1724Evnt->GetTriggerTimeTag());
      xV1724hit.SetPedestal(this->get_pedestal());
      xV1724hit.SetSampleArray(_data);

      if (xDetector == "tof0")
       _tof0_spill[xEvCounter-1].push_back(xV1724hit);
      else if (xDetector == "tof1")
       _tof1_spill[xEvCounter-1].push_back(xV1724hit);
      else if (xDetector == "tof2")
       _tof2_spill[xEvCounter-1].push_back(xV1724hit);
      else if (xDetector == "kl")
       _kl_spill[xEvCounter-1].push_back(xV1724hit);
      else if (xDetector == "tag")
       _tag_spill[xEvCounter-1].push_back(xV1724hit);
      else
        _unknown_spill[xEvCounter-1].push_back(xV1724hit);
    }

    _data.resize(0);
  }

  return OK;
}

void V1724CppDataProcessor::fill_daq_data() {

  unsigned int npe = _tof1_spill.size();

  if (_daq_data->GetTOF0DaqArraySize() != npe)
    _daq_data->GetTOF0DaqArrayPtr()->resize(npe);

  if (_daq_data->GetTOF1DaqArraySize() != npe)
    _daq_data->GetTOF1DaqArrayPtr()->resize(npe);

  if (_daq_data->GetTOF2DaqArraySize() != npe)
    _daq_data->GetTOF2DaqArrayPtr()->resize(npe);

  if (_daq_data->GetKLArraySize() != npe)
    _daq_data->GetKLArrayPtr()->resize(npe);

  if (_daq_data->GetTagArraySize() != npe)
    _daq_data->GetTagArrayPtr()->resize(npe);

  if (_daq_data->GetUnknownArraySize() != npe)
    _daq_data->GetUnknownArrayPtr()->resize(npe);


  for (unsigned int ipe = 0; ipe < npe; ipe++) {
    MAUS::TOFDaq *tof0_daq = _daq_data->GetTOF0DaqArrayElement(ipe);
    if (!tof0_daq)
      (*_daq_data->GetTOF0DaqArrayPtr())[ipe] = new MAUS::TOFDaq;

    _daq_data->GetTOF0DaqArrayElement(ipe)->SetV1724Array(_tof0_spill[ipe]);

    MAUS::TOFDaq *tof1_daq = _daq_data->GetTOF1DaqArrayElement(ipe);
    if (!tof1_daq)
      (*_daq_data->GetTOF1DaqArrayPtr())[ipe] = new MAUS::TOFDaq;

    _daq_data->GetTOF1DaqArrayElement(ipe)->SetV1724Array(_tof1_spill[ipe]);

    MAUS::TOFDaq *tof2_daq = _daq_data->GetTOF2DaqArrayElement(ipe);
    if (!tof2_daq)
      (*_daq_data->GetTOF2DaqArrayPtr())[ipe] = new MAUS::TOFDaq;

    _daq_data->GetTOF2DaqArrayElement(ipe)->SetV1724Array(_tof2_spill[ipe]);

    MAUS::KLDaq *kl_daq = _daq_data->GetKLArrayElement(ipe);
    if (!kl_daq)
      (*_daq_data->GetKLArrayPtr())[ipe] = new MAUS::KLDaq;

    _daq_data->GetKLArrayElement(ipe)->SetV1724Array(_kl_spill[ipe]);

    MAUS::Tag *tag_daq = _daq_data->GetTagArrayElement(ipe);
    if (!tag_daq)
      (*_daq_data->GetTagArrayPtr())[ipe] = new MAUS::Tag;

    _daq_data->GetTagArrayElement(ipe)->SetV1724Array(_tag_spill[ipe]);

    MAUS::Unknown *unknown = _daq_data->GetUnknownArrayElement(ipe);
    if (!unknown)
      (*_daq_data->GetUnknownArrayPtr())[ipe] = new MAUS::Unknown;
    _daq_data->GetUnknownArrayElement(ipe)->SetV1724Array(_unknown_spill[ipe]);
  }
}

void V1724CppDataProcessor::reset() {
  _tof0_spill.clear();
  _tof1_spill.clear();
  _tof2_spill.clear();
  _kl_spill.clear();
  _tag_spill.clear();
  _unknown_spill.clear();
}

////////////////////////////////////////////////////////////////////////////////

int V1731DataProcessor::Process(MDdataContainer* aPartEventPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentV1731 argument.
  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1731) )  return CastError;
  MDpartEventV1731* xV1731Evnt = static_cast<MDpartEventV1731*>(aPartEventPtr);

  Json::Value pBoardDoc;
  Json::Value xfAdcHit;
  int xLdc, xGeo, xEquip, xPartEv;
  string xDetector = "unknown";
  if ( !xV1731Evnt->IsValid() )
    return GenericError;

  // Put static data into the Json
  pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
  pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
  pBoardDoc["time_stamp"]          = this->GetTimeStamp();
  pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
  pBoardDoc["part_event_number"] = xPartEv = this->GetPartEventNumber();
  pBoardDoc["geo"]          = xGeo = xV1731Evnt->GetGeo();
  pBoardDoc["trigger_time_tag"]    = xV1731Evnt->GetTriggerTimeTag();
  // Loop over all the channels
  for (unsigned int xCh = 0; xCh < V1731_NCHANNELS; xCh++) {
    unsigned int xSamples = ( xV1731Evnt->GetLength(xCh) )*V1731_SAMPLES_PER_WORD;
    for (unsigned int j = 0; j < xSamples; j++) {
      int sample = xV1731Evnt->GetSampleData(xCh,  // Channel ID
                                             j);  // Sample ID
      _data.push_back(sample);
    }
    xfAdcHit = pBoardDoc;
    this->set_pedestal();
    int charge_mm = this->get_charge(ceaMinMax);
    if ( !_zero_suppression ||
        (_zero_suppression && charge_mm > _zs_threshold) ) {
      xfAdcHit["charge_mm"]    = charge_mm;
      xfAdcHit["charge_pm"]    = this->get_charge(ceaPedMin);
      int position_min = 0;
      xfAdcHit["pulse_area"]   = this->get_neg_signal_area(position_min);
      xfAdcHit["position_min"] = position_min;
      xfAdcHit["position_max"] = this->get_max_position();
      xfAdcHit["arrival_time"] = this->get_arrival_time();
      xfAdcHit["pedestal"]     = this->get_pedestal();
      xfAdcHit["samples"]      = this ->get_samples();
      DAQChannelKey* xKey      = _chMap->find(xLdc, xGeo, xCh, xEquip);
      if (xKey) {
        xDetector = xKey->detector();
        xfAdcHit["channel_key"]   = xKey->str();
        xfAdcHit["detector"]      = xDetector;
      } else {
        xfAdcHit["detector"] = xDetector = "unknown";
        xfAdcHit["channel_key"]   = "unknown";
      }
      xfAdcHit["channel"]        = xCh;

      ( *_docSpill )[xDetector][ xPartEv ][_equipment].append(xfAdcHit);
    }
    _data.resize(0);
  }

  return OK;
}

int V1731CppDataProcessor::Process(MDdataContainer* aPartEventPtr) {
  /**
   * Cast the argument to structure it points to.
   * This process should be called only with MDfragmentV1731 argument.
   */

  // cout << "This is V1731CppDataProcessor::Process" << endl;

  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1731) )  return CastError;
  MDpartEventV1731* xV1731Evnt = static_cast<MDpartEventV1731*>(aPartEventPtr);
//   xV1731Evnt->Dump();

//   string xDetector = "emr";
  if ( !xV1731Evnt->IsValid() )
    return GenericError;

  unsigned int xEvCounter = xV1731Evnt->GetEventCounter(); // starts from 1.
  if (xEvCounter > _emr_spill.size()) {
    _emr_spill.resize(xEvCounter);
    _ckov_spill.resize(xEvCounter);
    _unknown_spill.resize(xEvCounter);
  }
  // Put static data into the Data Structure.

  MAUS::V1731 xV1731hit;
  int xLdc   = this->GetLdcId();
  int xGeo   = xV1731Evnt->GetGeo();
  int xEquip = this->GetEquipmentType();
  xV1731hit.SetLdcId(xLdc);
  xV1731hit.SetGeo(xGeo);
  xV1731hit.SetEquipType(xEquip);

  // Loop over all the channels

  for (unsigned int xCh = 0; xCh < V1731_NCHANNELS; xCh++) {
    if ( xV1731Evnt->GetLength(xCh) ) {
      unsigned int xSamples = ( xV1731Evnt->GetLength(xCh) )*V1731_SAMPLES_PER_WORD;
      for (unsigned int j = 0; j < xSamples; j++) {
        int sample = xV1731Evnt->GetSampleData(xCh,  // Channel ID
                                               j);  // Sample ID
        _data.push_back(sample);
      }
      xV1731hit.SetSampleArray(_data);
      this->set_pedestal();

      xV1731hit.SetChannel(xCh);
      string xDetector;
      DAQChannelKey* xKey = _chMap->find(xLdc, xGeo, xCh, xEquip);
      if (xKey) {
        xV1731hit.SetChannelKey(xKey->str());
        xDetector = xKey->detector();
      } else {
        xDetector = "unknown";
      }

      xV1731hit.SetDetector(xDetector);
      xV1731hit.SetPartEventNumber(xV1731Evnt->GetEventCount());
      xV1731hit.SetPhysEventNumber(this->GetPhysEventNumber());
      xV1731hit.SetTimeStamp(this->GetTimeStamp());
      xV1731hit.SetChargeMm(this->get_charge(ceaMinMax));
      xV1731hit.SetChargePm(this->get_charge(ceaPedMin));
      int min_position = 0;
      xV1731hit.SetPulseArea(this->get_neg_signal_area(min_position));
      xV1731hit.SetPositionMin(min_position);
      xV1731hit.SetPositionMax(this->get_max_position());
      xV1731hit.SetArrivalTime(this->get_arrival_time());
      xV1731hit.SetTriggerTimeTag(xV1731Evnt->GetTriggerTimeTag());
      xV1731hit.SetPedestal(this->get_pedestal());
      xV1731hit.SetSampleArray(_data);

      if (xDetector == "emr")
        _emr_spill[xEvCounter-1].push_back(xV1731hit);
      else if (xDetector == "ckov")
        _ckov_spill[xEvCounter-1].push_back(xV1731hit);
      else
        _unknown_spill[xEvCounter-1].push_back(xV1731hit);
    }

    _data.resize(0);
  }

  return OK;
}

void V1731CppDataProcessor::fill_daq_data() {
  unsigned int npe = _emr_spill.size();
  _daq_data->GetEMRDaqPtr()->SetV1731PartEventArray(_emr_spill);

  if (_daq_data->GetCkovArraySize() != npe)
    _daq_data->GetCkovArrayPtr()->resize(npe);

  if (_daq_data->GetUnknownArraySize() != npe)
    _daq_data->GetUnknownArrayPtr()->resize(npe);

  for (unsigned int ipe = 0; ipe < npe; ipe++) {
    MAUS::CkovDaq *ckov_daq = _daq_data->GetCkovArrayElement(ipe);
    if (!ckov_daq)
      (*_daq_data->GetCkovArrayPtr())[ipe] = new MAUS::CkovDaq;

    _daq_data->GetCkovArrayElement(ipe)->SetV1731Array(_ckov_spill[ipe]);

    MAUS::Unknown *unknown = _daq_data->GetUnknownArrayElement(ipe);
    if (!unknown)
      (*_daq_data->GetUnknownArrayPtr())[ipe] = new MAUS::Unknown;
    _daq_data->GetUnknownArrayElement(ipe)->SetV1731Array(_unknown_spill[ipe]);
  }
}

void V1731CppDataProcessor::reset() {
  _emr_spill.clear();
  _ckov_spill.clear();
  _unknown_spill.clear();
}

////////////////////////////////////////////////////////////////////////////////

int V830DataProcessor::Process(MDdataContainer* aFragPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentV830 argument.
  if ( typeid(*aFragPtr) != typeid(MDfragmentV830) ) return CastError;
  MDfragmentV830* xV830Fragment = static_cast<MDfragmentV830*>(aFragPtr);

  Json::Value pBoardDoc;
  if ( !xV830Fragment->IsValid() )
    return GenericError;

  pBoardDoc["ldc_id"]              = this->GetLdcId();
  pBoardDoc["equip_type"]          = this->GetEquipmentType();
  pBoardDoc["time_stamp"]          = this->GetTimeStamp();
  pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
  pBoardDoc["channels"] = Json::Value();

  uint32_t * ptr = xV830Fragment->Get32bWordPtr(0);
  MDdataWordV830 xDataWord(ptr);
  unsigned int xWordCount(0);

  // Loop over all the channels
  while ( xWordCount < xV830Fragment->GetWordCount() ) {
    uint32_t dt= xDataWord.GetDataType();
    switch ( dt ) {
      case DWV830_Measurement:
      {
        // Convert the channel number into a channel number string!
        stringstream xConv;
        xConv <<  "ch" << xDataWord.GetChannel();
        unsigned int xValue = xDataWord.GetMeasurement();
        pBoardDoc["channels"][xConv.str()] = Json::Value(xValue);
        break;
      }
      case DWV830_Header:
      {
        pBoardDoc["geo"] = Json::Value(xDataWord.GetGeo());
        break;
      }
    }
    xWordCount++;
    xDataWord.SetDataPtr(++ptr);
  }

  (*_docSpill)[_equipment] = pBoardDoc;
  return OK;
}

#define V830_NCHANNELS   32

int V830CppDataProcessor::Process(MDdataContainer* aFragPtr) {
  /**
   * Cast the argument to structure it points to.
   * This process should be called only with MDfragmentV830 argument.
   */
//   cerr << "This is V830CppDataProcessor::Process " << endl;

  if ( typeid(*aFragPtr) != typeid(MDfragmentV830) ) return CastError;
  MDfragmentV830* xV830Fragment = static_cast<MDfragmentV830*>(aFragPtr);

  if ( !xV830Fragment->IsValid() )
    return GenericError;

  _v830_spill.SetLdcId(this->GetLdcId());
  _v830_spill.SetEquipType(this->GetEquipmentType());
  _v830_spill.SetTimeStamp(this->GetTimeStamp());
  _v830_spill.SetPhysEventNumber(this->GetPhysEventNumber());

  MAUS::Channels xChannels;

  uint32_t * ptr = xV830Fragment->Get32bWordPtr(0);
  MDdataWordV830 xDataWord(ptr);
  unsigned int xWordCount(0);

  // Loop over all the channels
  while ( xWordCount < xV830Fragment->GetWordCount() ) {
    uint32_t dt= xDataWord.GetDataType();
    switch ( dt ) {
      case DWV830_Measurement:
      {
        int xCh = xDataWord.GetChannel();
        int xValue = xDataWord.GetMeasurement();
        xChannels.SetCh(xCh, xValue);
        break;
      }
      case DWV830_Header:
      {
        _v830_spill.SetGeo(xDataWord.GetGeo());
        break;
      }
    }
    xWordCount++;
    xDataWord.SetDataPtr(++ptr);
  }
  _v830_spill.SetChannels(xChannels);
  return OK;
}

void V830CppDataProcessor::fill_daq_data() {
  _daq_data->SetV830(_v830_spill);
}

void V830CppDataProcessor::reset() {
  MAUS::Channels ch;
  _v830_spill.SetChannels(ch);
}

////////////////////////////////////////////////////////////////////////////////

int VLSBDataProcessor::Process(MDdataContainer* aFragPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentVLSB_C argument.
  if ( typeid(*aFragPtr) != typeid(MDfragmentVLSB) ) {
    std::cout << "CASTERROR: " << typeid(*aFragPtr).name() << " != " <<
                                  typeid(MDfragmentVLSB).name() << std::endl;
    return CastError;
  }
  std::cerr << "ED: VLSBDataProcessor::Process called" << std::endl;
  MDfragmentVLSB* xVLSBFragment = static_cast<MDfragmentVLSB*>(aFragPtr);

  Json::Value pBoardDoc, xVLSB_CHit;
  if ( !xVLSBFragment->IsValid() )
    return GenericError;

  int xLdc, xAdc, xPartEv;
  string xDetector;
  // Put static data into the Json
  pBoardDoc["ldc_id"]        = xLdc = this->GetLdcId();

  if (xLdc == 0)
    xDetector = "tracker1";
  if (xLdc == 2)
    xDetector = "tracker2";
  if (xLdc == 3)
    xDetector = "single_station";

  pBoardDoc["detector"]             = xDetector;
  pBoardDoc["equip_type"]           = this->GetEquipmentType();
  pBoardDoc["time_stamp"]           = this->GetTimeStamp();
  pBoardDoc["phys_event_number"]    = this->GetPhysEventNumber();
  pBoardDoc["bank_id"]              = xVLSBFragment->GetBoardID();
  // pBoardDoc["bank_size"]            = xVLSBFragment->GetPayLoadWordCount();

  // Get the number of data words.
  uint32_t nDataWords = xVLSBFragment->GetPayLoadWordCount();

  // Loop over the data.
  uint32_t xWordCount(0);
  while (xWordCount < nDataWords) {
    xVLSB_CHit = pBoardDoc;
    xVLSB_CHit["adc"] = xAdc = xVLSBFragment->GetAdc(xWordCount);
    if (!_zero_suppression ||
        (_zero_suppression && xAdc > _zs_threshold) ) {
      xVLSB_CHit["part_event_number"] = xPartEv = xVLSBFragment->GetEventNum(xWordCount)-1;
      xVLSB_CHit["channel"] = xVLSBFragment->GetChannel(xWordCount);
      xVLSB_CHit["tdc"] = xVLSBFragment->GetTdc(xWordCount);
      xVLSB_CHit["discriminator"] = xVLSBFragment->GetDiscriBit(xWordCount);

      ( *_docSpill )[xDetector][xPartEv][_equipment].append(xVLSB_CHit);
    }
    xWordCount++;
  }

  return OK;
}

int VLSBCppDataProcessor::Process(MDdataContainer* aFragPtr) {
  std::cerr << "INFO: VLSBCppDataProcessor::Process Processing data\n";
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentVLSB_C argument.
  if ( typeid(*aFragPtr) != typeid(MDfragmentVLSB) )
    return CastError;

//   std::cerr << "VLSBDataProcessor::Process" << std::endl;
  MDfragmentVLSB* xVLSBFragment = static_cast<MDfragmentVLSB*>(aFragPtr);

  if ( !xVLSBFragment->IsValid() )
    return GenericError;

  MAUS::VLSB xVLSBhit;
  int xLdc, xAdc, xTimeStamp, xPhysEvNum;
  unsigned int xPartEv;
  string xDetector;

  xLdc = this->GetLdcId();
  std::cerr << "INFO: VLSBCppDataProcessor::Process: Found xLdc = " << xLdc << std::endl;
  xTimeStamp = this->GetTimeStamp();
  xPhysEvNum = this->GetPhysEventNumber();
  // Get the number of data words.
  uint32_t nDataWords = xVLSBFragment->GetPayLoadWordCount();

//   std::cerr << "PhysEvNum: " << xPhysEvNum << " nDataWords: " << nDataWords << endl;
  // Loop over the data.
  uint32_t xWordCount(0);
  while (xWordCount < nDataWords) {
    xPartEv = xVLSBFragment->GetEventNum(xWordCount)-1;
//     std::cerr << xWordCount << " PartEv:" << xPartEv << "  LDC: " << xLdc << endl;
    if (xPartEv+1 > _tracker1_spill.size()) {
      _tracker1_spill.resize(xPartEv+1);
      _tracker0_spill.resize(xPartEv+1);
      _single_st_spill.resize(xPartEv+1);
    }

    xAdc = xVLSBFragment->GetAdc(xWordCount);
//     std::cerr << xWordCount << " adc: " << xAdc << endl;
    if (!_zero_suppression ||
        (_zero_suppression && xAdc > _zs_threshold) ) {
      xVLSBhit.SetLdcId(xLdc);
      xVLSBhit.SetEquipType(this->GetEquipmentType());
      xVLSBhit.SetTimeStamp(xTimeStamp);
      xVLSBhit.SetPhysEventNumber(xPhysEvNum);
      xVLSBhit.SetBankID(xVLSBFragment->GetBoardID());
      xVLSBhit.SetADC(xAdc);
      xVLSBhit.SetPartEventNumber(xPartEv);
      xVLSBhit.SetChannel(xVLSBFragment->GetChannel(xWordCount));
      xVLSBhit.SetTDC(xVLSBFragment->GetTdc(xWordCount));
      xVLSBhit.SetDiscriminator(xVLSBFragment->GetDiscriBit(xWordCount));
      if (xLdc == 3) {
        xDetector = "tracker0";
        xVLSBhit.SetDetector(xDetector);
        _tracker0_spill[xPartEv].push_back(xVLSBhit);

      } else if (xLdc == 4) {
        xDetector = "tracker1";
        xVLSBhit.SetDetector(xDetector);
        _tracker1_spill[xPartEv].push_back(xVLSBhit);
      }
//        else if (xLdc == 3) {
//         xDetector = "single_station";
//         xVLSBhit.SetDetector(xDetector);
//         _single_st_spill[xPartEv].push_back(xVLSBhit);
//       }
    }
    xWordCount++;
  }

  return OK;
}


void VLSBCppDataProcessor::fill_daq_data() {

  unsigned int npe = _tracker1_spill.size();
  std::cerr << "INFO: VLSBCppDataProcessor::fill_daq_data: Tracker 1 npe = " << npe << std::endl;

  if (_daq_data->GetTracker0DaqArraySize() != npe)
    _daq_data->GetTracker0DaqArrayPtr()->resize(npe);

  if (_daq_data->GetTracker1DaqArraySize() != npe)
    _daq_data->GetTracker1DaqArrayPtr()->resize(npe);


  for (unsigned int ipe = 0; ipe < npe; ipe++) {
    MAUS::TrackerDaq *tracker0_daq = _daq_data->GetTracker0DaqArrayElement(ipe);
    if (!tracker0_daq)
      (*_daq_data->GetTracker0DaqArrayPtr())[ipe] = new MAUS::TrackerDaq;
    _daq_data->GetTracker0DaqArrayElement(ipe)->SetVLSBArray(_tracker0_spill[ipe]);

    MAUS::TrackerDaq *tracker1_daq = _daq_data->GetTracker1DaqArrayElement(ipe);
    if (!tracker1_daq)
      (*_daq_data->GetTracker1DaqArrayPtr())[ipe] = new MAUS::TrackerDaq;
    _daq_data->GetTracker1DaqArrayElement(ipe)->SetVLSBArray(_tracker1_spill[ipe]);
  }
}

void VLSBCppDataProcessor::reset() {
  _tracker1_spill.clear();
  _tracker0_spill.clear();
  _single_st_spill.clear();
}

////////////////////////////////////////////////////////////////////////////////

int VLSB_CDataProcessor::Process(MDdataContainer* aFragPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentVLSB_C argument.
  if ( typeid(*aFragPtr) != typeid(MDfragmentVLSB_C) )
    return CastError;

  MDfragmentVLSB_C* xVLSB_CFragment = static_cast<MDfragmentVLSB_C*>(aFragPtr);

  Json::Value pBoardDoc, xVLSB_CHit;
  if ( !xVLSB_CFragment->IsValid() )
    return GenericError;

  int xLdc, xAdc, xPartEv;
  string xDetector;
  // Put static data into the Json
  pBoardDoc["ldc_id"]     = xLdc = this->GetLdcId();

  if (xLdc == 0)
    xDetector = "tracker1";
  if (xLdc == 2)
    xDetector = "tracker2";

  pBoardDoc["detector"]          = xDetector;
  pBoardDoc["equip_type"]        = this->GetEquipmentType();
  pBoardDoc["time_stamp"]        = this->GetTimeStamp();
  pBoardDoc["phys_event_number"] = this->GetPhysEventNumber()-1;
  pBoardDoc["geo"]               = xVLSB_CFragment->GetGeo();

  MDdataWordVLSB xDataWord;
  uint32_t * dataPtr = xVLSB_CFragment->UserPayLoadPtr();
  for (unsigned int iban = 0; iban < 4; iban++) {
    for (unsigned int iw =0; iw < xVLSB_CFragment->GetBankLength(iban); iw++) {
      xVLSB_CHit = pBoardDoc;
      xDataWord.SetDataPtr(dataPtr);
      xVLSB_CHit["adc"] = xAdc = xDataWord.GetAdc();
      if ( !_zero_suppression ||
          (_zero_suppression && xAdc > _zs_threshold) ) {
        xVLSB_CHit["part_event_number"] = xPartEv = xDataWord.GetEventNum()-1;
        xVLSB_CHit["bank"] = iban;
        xVLSB_CHit["channel"] = xDataWord.GetChannel();
        xVLSB_CHit["tdc"] = xDataWord.GetTdc();
        xVLSB_CHit["discriminator"] = xDataWord.GetDiscriBit();

        ( *_docSpill )[xDetector][xPartEv][_equipment].append(xVLSB_CHit);
      }
      dataPtr++;
    }
  }
  return OK;
}

////////////////////////////////////////////////////////////////////////////////

int DBBDataProcessor::Process(MDdataContainer* aFragPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentDBB argument.
  if ( typeid(*aFragPtr) != typeid(MDfragmentDBB) )
    return CastError;

  MDfragmentDBB* xDBBFragment = static_cast<MDfragmentDBB*>(aFragPtr);

  Json::Value pBoardDoc;
  Json::Value xDBBHit;
  int xLdc, xGeo, xEquip;
  string xDetector = "emr";

  if ( !xDBBFragment->IsValid() )
    return GenericError;

  // Put static data into the Json
  pBoardDoc["detector"]      = xDetector;
  pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
  pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
  pBoardDoc["time_stamp"]          = this->GetTimeStamp();
  pBoardDoc["spill_number"]        = xDBBFragment->GetSpillNumber();
  pBoardDoc["spill_width"]         = xDBBFragment->GetSpillWidth();
  pBoardDoc["geo"]          = xGeo = xDBBFragment->GetGeo();
  pBoardDoc["trigger_count"]       = xDBBFragment->GetTriggerCount();
  pBoardDoc["hit_count"]           = xDBBFragment->GetHitCount();

  // Loop over all the channels
  for (unsigned int xCh = 0; xCh < DBB_NCHANNELS; xCh++) {
    // Get the number of leading edge hits.
    int xHitsL = xDBBFragment->GetNLeadingEdgeHits(xCh);
    // Get the number of trailing edge hits.
    int xHitsT = xDBBFragment->GetNTrailingEdgeHits(xCh);

    // By definition the number of the leading edge hits has to be equal
    // to the number of the trailing edge hits but some times this is not true.
    int xMaxHits = MAX(xHitsL, xHitsT);

    // Loop over all leading and trailing hits in this channel.
    int xLT, xTT;  // Lead and Trail times
    for (int j = 0; j < xMaxHits; j++) {
      if (j < xHitsL) {
        xLT = xDBBFragment->GetHitMeasurement(j,  // Hit ID
                                             xCh,  // Channel ID
                                             'l');
      } else {
        // If no trailing edge hit set the value og time to -99.
        xLT = -99;
      }

      if (j < xHitsT) {
        xTT = xDBBFragment->GetHitMeasurement(j,  // Hit ID
                                              xCh,  // Channel ID
                                              't');
      } else {
        // If no trailing edge hit set the value og time to -99.
        xTT = -99;
      }

      xDBBHit["channel"]       = xCh;
      xDBBHit["leading_time"]  = xLT;
      xDBBHit["trailing_time"] = xTT;
      DAQChannelKey xKey(xLdc, xGeo, xCh, xEquip, xDetector);
      xDBBHit["channel_key"]   = xKey.str();
      pBoardDoc["hits"].append(xDBBHit);
    }
  }
  ( *_docSpill )[xDetector][_equipment].append(pBoardDoc);

  return OK;
}

int DBBCppDataProcessor::Process(MDdataContainer* aFragPtr) {
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentDBB argument.

//   cout << "This is DBBCppDataProcessor::Process" << endl;
  if ( typeid(*aFragPtr) != typeid(MDfragmentDBB) ) return CastError;

  MDfragmentDBB* xDBBFragment = static_cast<MDfragmentDBB*>(aFragPtr);

  if ( !xDBBFragment->IsValid() )
    return GenericError;

//   xDBBFragment->Dump();
  MAUS::DBBSpillData xDBBSpill;
  xDBBSpill.SetLdcId(this->GetLdcId());
  xDBBSpill.SetTimeStamp(this->GetTimeStamp());
  set_spill_data(xDBBFragment, &xDBBSpill);

  _spill.push_back(xDBBSpill);

  return OK;
}

void DBBCppDataProcessor::set_spill_data(MDfragmentDBB *fragment, MAUS::DBBSpillData *dbb_spill) {
  int xLdc, xGeo;
  string xDetector = "emr";

  dbb_spill->SetDetector(xDetector);
  xGeo = fragment->GetGeo();
  xLdc = dbb_spill->GetLdcId();
  dbb_spill->SetDBBId(xGeo);
  dbb_spill->SetLdcId(xLdc);
  dbb_spill->SetSpillNumber(fragment->GetSpillNumber());
  dbb_spill->SetSpillWidth(fragment->GetSpillWidth());
  dbb_spill->SetTriggerCount(fragment->GetTriggerCount());
  dbb_spill->SetHitCount(fragment->GetHitCount());

  MAUS::DBBHitsArray hits;
  MAUS::DBBHitsArray triggers;

  // Loop over all the channels
  for (unsigned int xCh = 0; xCh < DBB_NCHANNELS; xCh++) {
    // Get the number of leading edge hits.
    int xHitsL = fragment->GetNLeadingEdgeHits(xCh);
    // Get the number of trailing edge hits.
    int xHitsT = fragment->GetNTrailingEdgeHits(xCh);

    // By definition the number of the leading edge hits has to be equal
    // to the number of the trailing edge hits but some times this is not true.
    int xMaxHits = MAX(xHitsL, xHitsT);

    int xLT, xTT;  // Lead and Trail times
    for (int xHit = 0; xHit < xMaxHits; xHit++) {

      MAUS::DBBHit hit;
      hit.SetLDC(xLdc);
      hit.SetGEO(xGeo);
      hit.SetChannel(xCh);

      if (xHit < xHitsL) {
        xLT = fragment->GetHitMeasurement(xHit,  // Hit ID
                                             xCh,  // Channel ID
                                             'l');
      } else {
        // If no trailing edge hit set the value og time to -99.
        xLT = 0;
      }

      if (xHit < xHitsT) {
        xTT = fragment->GetHitMeasurement(xHit,  // Hit ID
                                              xCh,  // Channel ID
                                              't');
      } else {
        // If no trailing edge hit set the value og time to -99.
        xTT = 0;
      }
      hit.SetLTime(xLT);
      hit.SetTTime(xTT);

      if (xCh == DBB_TRIGGER_CHANNEL)
        triggers.push_back(hit);
      else
        hits.push_back(hit);
    }
  }
  dbb_spill->SetDBBHitsArray(hits);
  dbb_spill->SetDBBTriggersArray(triggers);
}

void DBBCppDataProcessor::fill_daq_data() {
  _daq_data->GetEMRDaqPtr()->SetDBBArray(_spill);
}

#define DBB_TRIGGER_CHANNEL 4

int DBBChainCppDataProcessor::Process(MDdataContainer* aFragPtr) {
  /** Cast the argument to structure it points to.
      This process should be called only with MDfragmentDBB argument. */
//   cout << "This is DBBChainCppDataProcessor::Process" << endl;

  if ( typeid(*aFragPtr) != typeid(MDfragmentDBBChain) )
    return CastError;

  MDfragmentDBBChain* xDBBChainFragment = static_cast<MDfragmentDBBChain*>(aFragPtr);

  /** Loop over the elements of the chain */
  for (int i = 0; i < DBBS_IN_CHAIN; i++) {
    MDfragmentDBB* xDBBFragment =  xDBBChainFragment->getFragmentDBB(i);

    if ( !xDBBFragment->IsValid() )
      return GenericError;

//     xDBBFragment->Dump();
    MAUS::DBBSpillData xDBBSpill;

    /** Fill into the data structure */
    xDBBSpill.SetLdcId(this->GetLdcId());
    xDBBSpill.SetTimeStamp(this->GetTimeStamp());

    DBBCppDataProcessor::set_spill_data(xDBBFragment, &xDBBSpill);

    _spill.push_back(xDBBSpill);
  }
  return OK;
}

void DBBChainCppDataProcessor::fill_daq_data() {
  _daq_data->GetEMRDaqPtr()->SetDBBArray(_spill);
}
}
//////////////////////////////////////////////////////////////////////////////////////////////
