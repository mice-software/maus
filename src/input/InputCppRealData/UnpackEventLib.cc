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

#include "src/input/InputCppRealData/UnpackEventLib.hh"

int V1290DataProcessor::Process(MDdataContainer* aPartEventPtr){
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentV1290 argument
  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1290) )  return CastError;
  MDpartEventV1290* xV1290Evnt=static_cast<MDpartEventV1290*>(aPartEventPtr);

  Json::Value pBoardDoc;
  Json::Value pTdcHit;
  int xLdc, xGeo, xEquip, xPartEv;
  string xDetector = "unknown";
  if (xV1290Evnt->IsValid()) {
    // Put static data into the Json
    pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
		pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
    pBoardDoc["time_stamp"]          = this->GetTimeStamp();
    pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
    pBoardDoc["part_event_number"] = xPartEv = xV1290Evnt->GetEventCount();
    pBoardDoc["geo"]          = xGeo = xV1290Evnt->GetGeo();
    pBoardDoc["trigger_time_tag"]    = xV1290Evnt->GetTriggerTimeTag();
    // Loop over all the channels
    for (unsigned int xCh = 0; xCh < V1290_NCHANNELS; xCh++) {
			pTdcHit = pBoardDoc;
      int xHitsL = xV1290Evnt->GetNHits(xCh, 'l');
      int xHitsT = xV1290Evnt->GetNHits(xCh, 't');
      int xMaxHits = MAX(xHitsL, xHitsT);

      // Loop over each possible hit
      int xLT, xTT;  // Lead and Trail times?
      for (unsigned int j = 0; j < xMaxHits; j++) {
        if (j < xHitsL)
          xLT = xV1290Evnt->GetHitMeasurement(j,  // Hit ID
                                             xCh,  // Channel ID
                                             'l');
        else
          xLT = -99;

        if (j < xHitsT) xTT = xV1290Evnt->GetHitMeasurement(j,  // Hit ID
                                                           xCh,  // Channel ID
                                                           't');
        else
          xTT = -99;

        int xBunchID = xV1290Evnt->GetBunchID( xCh/8 );

        Json::Value xTdcHit;
        pTdcHit["bunch_id"]      = xBunchID;
        pTdcHit["channel"]       = xCh;
        pTdcHit["leading_time"]  = xLT;
        pTdcHit["trailing_time"] = xTT;
        DAQChannelKey* xKey = _chMap->find(xLdc,xGeo,xCh,xEquip);
        if(xKey){
          pTdcHit["channel_key"] = xKey->str();
          xDetector = xKey->detector();
          pTdcHit["detector"] = xDetector;
        } else pTdcHit["detector"] = xDetector = "unknown";

        ( *_docSpill )[xDetector][ xPartEv ][_equipment.c_str()].append(pTdcHit);
      }
    }
  }else cout<<"MDpartEventV1290 is not valid"<<endl;

  return OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////

int V1724DataProcessor::Process(MDdataContainer* aPartEventPtr){
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentV1724 argument.
  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1724) )  return CastError;
  MDpartEventV1724* xV1724Evnt=static_cast<MDpartEventV1724*>(aPartEventPtr);

  Json::Value pBoardDoc;
	Json::Value xfAdcHit;
  int xLdc, xGeo, xEquip, xPartEv;
  string xDetector = "unknown";
  if (xV1724Evnt->IsValid()) {
    // Put static data into the Json
    pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
    pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
    pBoardDoc["time_stamp"]          = this->GetTimeStamp();
    pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
    pBoardDoc["part_event_number"] = xPartEv = xV1724Evnt->GetEventCount();
    pBoardDoc["geo"]          = xGeo = xV1724Evnt->GetGeo();
    pBoardDoc["trigger_time_tag"]    = xV1724Evnt->GetTriggerTimeTag();
    // Loop over all the channels
    for (unsigned int xCh = 0; xCh < V1724_NCHANNELS; xCh++) {
      unsigned int xSamples = ( xV1724Evnt->GetLength(xCh) )*V1724_SAMPLES_PER_WORD;
      for (unsigned int j = 0; j < xSamples; j++) {
        int sample = xV1724Evnt->GetSampleData(xCh,  // Channel ID
                                               j);  // Sample ID
        data.push_back( sample );
      }
      xfAdcHit = pBoardDoc;
      this->SetPedestal();
      int charge_mm = this->GetCharge(ceaMinMax);
      if(!_zero_suppression || charge_mm>100){
        xfAdcHit["charge_mm"]    = charge_mm;
        xfAdcHit["charge_pm"]    = this->GetCharge(ceaPedMax);
        xfAdcHit["position_max"] = this->GetMaxPosition();
        xfAdcHit["pedestals"]    = this->GetPedestal();
        DAQChannelKey* xKey = _chMap->find(xLdc,xGeo,xCh,xEquip);
        if(xKey){
          xDetector = xKey->detector();
          xfAdcHit["channel_key"]   = xKey->str();
          xfAdcHit["detector"]      = xDetector;
        } else xfAdcHit["detector"] = xDetector = "unknown";
        xfAdcHit["channel"]        = xCh;

        ( *_docSpill )[xDetector][ xPartEv ][_equipment].append(xfAdcHit);
      }
      data.resize(0);
    }
  }

  return OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////

int V1731DataProcessor::Process(MDdataContainer* aPartEventPtr){
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentV1731 argument.
  if ( typeid(*aPartEventPtr) != typeid(MDpartEventV1731) )  return CastError;
  MDpartEventV1731* xV1731Evnt=static_cast<MDpartEventV1731*>(aPartEventPtr);

  Json::Value pBoardDoc;
	Json::Value xfAdcHit;
  int xLdc, xGeo, xEquip, xPartEv;
  string xDetector = "unknown";
  if (xV1731Evnt->IsValid()) {
    // Put static data into the Json
    pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
    pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
    pBoardDoc["time_stamp"]          = this->GetTimeStamp();
    pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
    pBoardDoc["part_event_number"] = xPartEv = xV1731Evnt->GetEventCount();
    pBoardDoc["geo"]          = xGeo = xV1731Evnt->GetGeo();
    pBoardDoc["trigger_time_tag"]    = xV1731Evnt->GetTriggerTimeTag();
    // Loop over all the channels
    for (unsigned int xCh = 0; xCh < V1724_NCHANNELS; xCh++) {
      unsigned int xSamples = ( xV1731Evnt->GetLength(xCh) )*V1731_SAMPLES_PER_WORD;
      for (unsigned int j = 0; j < xSamples; j++) {
        int sample = xV1731Evnt->GetSampleData(xCh,  // Channel ID
                                               j);  // Sample ID
        data.push_back( sample );
      }
      xfAdcHit = pBoardDoc;
      this->SetPedestal();
      int charge_mm = this->GetCharge(ceaMinMax);
      if(!_zero_suppression || charge_mm>100){
        xfAdcHit["charge_mm"]    = charge_mm;
        xfAdcHit["charge_pm"]    = this->GetCharge(ceaPedMax);
        xfAdcHit["position_max"] = this->GetMaxPosition();
        xfAdcHit["pedestals"]    = this->GetPedestal();
        DAQChannelKey* xKey = _chMap->find(xLdc,xGeo,xCh,xEquip);
        if(xKey){
          xDetector = xKey->detector();
          xfAdcHit["channel_key"]   = xKey->str();
          xfAdcHit["detector"]      = xDetector;
        } else xfAdcHit["detector"] = xDetector = "unknown";
        xfAdcHit["channel"]        = xCh;

        ( *_docSpill )[xDetector][ xPartEv ][_equipment].append(xfAdcHit);
      }
      data.resize(0);
    }
  }

  return OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////

int V830DataProcessor::Process(MDdataContainer* aFragPtr){
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentV830 argument.
  if ( typeid(*aFragPtr) != typeid(MDfragmentV830) ) return CastError;
  MDfragmentV830* xV830Fragment = static_cast<MDfragmentV830*>(aFragPtr);

  Json::Value pBoardDoc;
  if (xV830Fragment->IsValid()) {
    pBoardDoc["ldc_id"]              = this->GetLdcId();
    pBoardDoc["equip_type"]          = this->GetEquipmentType();
    pBoardDoc["time_stamp"]          = this->GetTimeStamp();
    pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
    pBoardDoc["channels"] = Json::Value();

    unsigned int * ptr = xV830Fragment->Get32bWordPtr(0);
    MDdataWordV830 dw(ptr) ;
    unsigned int wc(0);

    while (wc < xV830Fragment->GetWordCount() ) {
      uint32_t dt= dw.GetDataType();
      switch ( dt ) {
        case DWV830_Measurement:
        {
		      // Convert the channel number into a channel number string!
          std::stringstream xConv;
          xConv <<  "ch" << dw.GetChannel();
					unsigned int xValue = dw.GetMeasurement();
					pBoardDoc["channels"][xConv.str()] = Json::Value(xValue);
	        break;
        }
        case DWV830_Header:
        {
           pBoardDoc["geo"] = Json::Value( dw.GetGeo() );
           break;
        }
      }
      wc++;
      dw.SetDataPtr( ++ptr );
    }
	}

	(*_docSpill)[_equipment] = pBoardDoc;

  return OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////

int VLSBDataProcessor::Process(MDdataContainer* aFragPtr){
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentVLSB_C argument.
  if ( typeid(*aFragPtr) != typeid(MDfragmentVLSB_C) ) return CastError;
  MDfragmentVLSB_C* xVLSBFragment = static_cast<MDfragmentVLSB_C*>(aFragPtr);

  Json::Value pBoardDoc;
  if( xVLSBFragment->IsValid()) {
    int xLdc, xGeo, xEquip;
    // Put static data into the Json
    pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
    pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
    pBoardDoc["time_stamp"]          = this->GetTimeStamp();
    pBoardDoc["phys_event_number"]   = this->GetPhysEventNumber();
    //pBoardDoc["part_event_number"]   = this->GetPartEventNumber();
    pBoardDoc["geo"]          = xGeo = xVLSBFragment->GetGeo();
    for (int ib=0; ib<4; ib++){
		// Convert the bank number into a bank number string!
      std::stringstream xConv;
      xConv << "bank_length_" << ib;
      pBoardDoc[ib] = xVLSBFragment->GetBankLength(ib);
    }
  }

  (*_docSpill)[_equipment].append(pBoardDoc);

  return OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////

int DBBDataProcessor::Process(MDdataContainer* aFragPtr){
  // Cast the argument to structure it points to.
  // This process should be called only with MDfragmentDBB argument.
  if ( typeid(*aFragPtr) != typeid(MDfragmentDBB) ) return CastError;
  MDfragmentDBB* xDBBFragment = static_cast<MDfragmentDBB*>(aFragPtr);

  Json::Value pBoardDoc;
  Json::Value xDBBHit;
  int xLdc, xGeo, xEquip, xPartEv;
  string xDetector = "emr";

  if (xDBBFragment->IsValid()) {
    // Put static data into the Json
    pBoardDoc["ldc_id"]       = xLdc = this->GetLdcId();
    pBoardDoc["equip_type"] = xEquip = this->GetEquipmentType();
    pBoardDoc["time_stamp"]          = this->GetTimeStamp();
    pBoardDoc["spill_number"]        = xDBBFragment->GetSpillNumber();
    pBoardDoc["spill_width"]         = xDBBFragment->GetSpillWidth();
    pBoardDoc["geo"]          = xGeo = xDBBFragment->GetGeo();
    pBoardDoc["trigger_count"]       = xDBBFragment->GetTriggerCount();
    pBoardDoc["hit_count"]           = xDBBFragment->GetHitCount();

    // Loop over all the channels
    for (unsigned int xCh = 0; xCh < V1290_NCHANNELS; xCh++) {
      int xHitsL = xDBBFragment->GetNLeadingEdgeHits(xCh);
      int xHitsT = xDBBFragment->GetNTrailingEdgeHits(xCh);
      int xMaxHits = MAX(xHitsL, xHitsT);

      // Loop over each possible hit
      int xLT, xTT;  // Lead and Trail times?
      for (unsigned int j = 0; j < xMaxHits; j++) {
        if (j < xHitsL)
          xLT = xDBBFragment->GetHitMeasurement(j,  // Hit ID
                                             xCh,  // Channel ID
                                             'l');
        else
          xLT = -99;

        if (j < xHitsT) xTT = xDBBFragment->GetHitMeasurement(j,  // Hit ID
                                                           xCh,  // Channel ID
                                                           't');
        else
          xTT = -99;

        xDBBHit["channel"]       = xCh;
        xDBBHit["leading_time"]  = xLT;
        xDBBHit["trailing_time"] = xTT;
        DAQChannelKey* xKey = _chMap->find(xLdc,xGeo,xCh,xEquip);
        if(xKey){
          xDetector = xKey->detector();
          xDBBHit["channel_key"]   = xKey->str();
          xDBBHit["detector"]      = xDetector;
        } else xDBBHit["detector"] = xDetector = "unknown";
        pBoardDoc["hits"].append(xDBBHit);
      }
    }
    ( *_docSpill )[xDetector][_equipment].append(pBoardDoc);
  }

  return OK;
}












