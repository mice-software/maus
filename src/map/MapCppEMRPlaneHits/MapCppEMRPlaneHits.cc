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
#include "Utils/DAQChannelMap.hh"
#include "Interface/dataCards.hh"
#include "API/PyWrapMapBase.hh"
#include "Converter/DataConverters/CppJsonSpillConverter.hh"
#include "Converter/DataConverters/JsonCppSpillConverter.hh"

#include "src/map/MapCppEMRPlaneHits/MapCppEMRPlaneHits.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppEMRPlaneHits(void) {
  PyWrapMapBase<MAUS::MapCppEMRPlaneHits>::PyWrapMapBaseModInit
                                                ("MapCppEMRPlaneHits", "", "", "", "");
}

MapCppEMRPlaneHits::MapCppEMRPlaneHits()
    : MapBase<MAUS::Data>("MapCppEMRPlaneHits") {
}

void MapCppEMRPlaneHits::_birth(const std::string& argJsonConfigDocument) {
  _classname = "MapCppEMRPlaneHits";
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    throw MAUS::Exception(Exception::recoverable,
                      "Could not resolve ${MAUS_ROOT_DIR} environment variable",
                      "MapCppEMRPlaneHits::birth");
  }

  //  JsonCpp setup
  Json::Value configJSON;
  Json::Value map_file_name;
  Json::Value xEnable_V1731_Unpacking;
  Json::Value xEnable_DBB_Unpacking;

  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  // Fetch variables
  _number_of_planes = configJSON["EMRnumberOfPlanes"].asInt();
  _number_of_bars = configJSON["EMRnumberOfBars"].asInt();

  _tot_noise_low = configJSON["EMRtotNoiseLow"].asInt();
  _tot_noise_up = configJSON["EMRtotNoiseUp"].asInt();
  _deltat_signal_low = configJSON["EMRdeltatSignalLow"].asInt();
  _deltat_signal_up = configJSON["EMRdeltatSignalUp"].asInt();
  _deltat_noise_low = configJSON["EMRdeltatNoiseLow"].asInt();
  _deltat_noise_up = configJSON["EMRdeltatNoiseUp"].asInt();

  // Load EMR channel map
  map_file_name = JsonWrapper::GetProperty(configJSON,
                                           "EMR_cabling_file",
                                           JsonWrapper::stringValue);

  std::string xMapFile = std::string(pMAUS_ROOT_DIR) + map_file_name.asString();
  bool loaded = _emrMap.InitFromFile(xMapFile);
  if (!loaded)
    throw MAUS::Exception(Exception::recoverable,
                          "Failed to load EMR Channel Map File",
                          "MapCppEMRPlaneHits::birth");

  // Enable fADC unpacking
  xEnable_V1731_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_V1731_Unpacking",
                                                     JsonWrapper::booleanValue);
  if (!xEnable_V1731_Unpacking.asBool()) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppEMRPlaneHits::birth. The unpacking of the flashADC V1731 is disabled!!!"
    << " Are you shure you want this?"
    << std::endl;
  }

  // Enable DBB unpacking
  xEnable_DBB_Unpacking   = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_DBB_Unpacking",
                                                     JsonWrapper::booleanValue);
  if (!xEnable_DBB_Unpacking.asBool()) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppEMRPlaneHits::birth. The unpacking of the VRBs is disabled!!!"
    << " Are you shure you want this?"
    << std::endl;
  }
}

void MapCppEMRPlaneHits::_death() {
}

void MapCppEMRPlaneHits::_process(Data *data) const {

  // Get spill, break if there's no DAQ data
  Spill *spill = data->GetSpill();

  if (spill->GetDAQData() == NULL)
      return;

  EMRDaq emr_data = spill->GetDAQData()->GetEMRDaq();
  int nPartEvents = emr_data.GetV1731NumPartEvents();
//  std::cerr << "nPartEvts: " << nPartEvents << std::endl;

  // Create DBB and fADC arrays with n+2 events (1 per trigger + noise + decays)
  EMRDBBEventVector emr_dbb_events_tmp = get_dbb_data_tmp(nPartEvents+2);
  EMRfADCEventVector emr_fadc_events_tmp = get_fadc_data_tmp(nPartEvents+2);

  // Fill the fADC and DBB array with DAQ information
  processDBB(emr_data, nPartEvents, emr_dbb_events_tmp, emr_fadc_events_tmp);
  processFADC(emr_data, nPartEvents, emr_fadc_events_tmp);

  // Fill the Recon event array with Spill information (1 per trigger + noise + decays)
  fill(spill, nPartEvents+2, emr_dbb_events_tmp, emr_fadc_events_tmp);
}

void MapCppEMRPlaneHits::processDBB(MAUS::EMRDaq EMRdaq,
				    int nPartEvents,
				    EMRDBBEventVector& emr_dbb_events_tmp,
				    EMRfADCEventVector& emr_fadc_events_tmp) const {
//  std::cerr << "DBBArraySize: " << EMRdaq.GetDBBArraySize() << std::endl;
  int nDBBs = EMRdaq.GetDBBArraySize();
  for (int idbb = 0; idbb < nDBBs; idbb++) {

    DBBSpillData dbb = EMRdaq.GetDBBArrayElement(idbb);
    if ( dbb.GetTriggerCount() != nPartEvents ) {
      Squeak::mout(Squeak::error)
      << "ERROR in  MapCppEMRPlaneHits::processDBB: number of triggers mismatch ("
      << dbb.GetTriggerCount() << "!=" << nPartEvents << ")" << std::endl;
      return;
    }

    int xLDC    = dbb.GetLdcId();
    int xSpill  = dbb.GetSpillNumber();
    int xGeo    = dbb.GetDBBId();
    int nHits   = dbb.GetDBBHitsArraySize();
//    int nTr     = dbb.GetDBBTriggersArraySize();

    for (int iHit = 0; iHit < nHits; iHit++) {
      DBBHit this_hit = dbb.GetDBBHitsArrayElement(iHit);
      int xCh = this_hit.GetChannel();
      int lt  = this_hit.GetLTime();
      int tt  = this_hit.GetTTime();

      DAQChannelKey daq_key(xLDC, xGeo, xCh, 141, "emr");
//      std::cerr << daq_key << endl;
      EMRChannelKey *emr_key = _emrMap.find(&daq_key);
      if (emr_key) {
        int xPlane = emr_key->plane();
//        int xOri   = emr_key->orientation();
        int xBar   = emr_key->bar();
//        std::cerr << *emr_key << " --> lt: " << lt << "  tt: " << tt << endl;

        // Set the spill number for the noise and decay events
        if (iHit == 0) { // Set the spill information only when processing the very first hit.
          emr_fadc_events_tmp[nPartEvents][xPlane]._spill = xSpill;
          emr_fadc_events_tmp[nPartEvents+1][xPlane]._spill = xSpill;
        }

	// Loop over the trigger and tries to associate the hit to one of them
	bool matched = false;
        for (int iPe = 0; iPe < nPartEvents; iPe++) {
          DBBHit this_trigger = dbb.GetDBBTriggersArrayElement(iPe);
          int tr_lt = this_trigger.GetLTime();
//           int tr_tt = this_trigger.GetTTime();
//           int xCh   = this_trigger.GetChannel();

          if (iHit == 0) { // Set the plane information only when processing the very first hit.
            emr_fadc_events_tmp[iPe][xPlane]._time  = tr_lt;
            emr_fadc_events_tmp[iPe][xPlane]._spill = xSpill;
          }

          int delta_t = lt - tr_lt;
          int tot = tt - lt;

	  // Set bar hit
          EMRBarHit bHit;
          bHit.SetTot(tot);

	  // Discriminate noise and decays from events signal
          if (delta_t > _deltat_signal_low && delta_t < _deltat_signal_up) {
            bHit.SetDeltaT(delta_t - _deltat_signal_low);
//            std::cerr << "*---> " << *emr_key << " --> trigger_Id: " << iPe
//            	      << "  tot: " << tt-lt
//            	      << "  delta: " << delta_t - _deltat_signal_low
//            	      << "(" << delta_t << ")" << std::endl;
            emr_dbb_events_tmp[iPe][xPlane][xBar].push_back(bHit);
	    matched = true;
          } else if (delta_t > _deltat_noise_low && delta_t < _deltat_noise_up &&
		   tot > _tot_noise_low && tot < _tot_noise_up ) {
            bHit.SetDeltaT(delta_t - _deltat_signal_low);
	    emr_dbb_events_tmp[nPartEvents][xPlane][xBar].push_back(bHit);
	    matched = true;
	  } else if (iPe == nPartEvents-1 && !matched) {
	    bHit.SetDeltaT(0); // tt -lt is irrelevant for decay products
	    emr_dbb_events_tmp[nPartEvents+1][xPlane][xBar].push_back(bHit);
	  }
        }
      }/* else {std::cerr << "WARNING!!! unknow EMR DBB channel " << daq_key << std::endl;}*/
    }
  }
}

void MapCppEMRPlaneHits::processFADC(MAUS::EMRDaq EMRdaq,
				     int nPartEvents,
				     EMRfADCEventVector& emr_fadc_events_tmp) const {
//  std::cerr << "GetV1731NumPartEvents: " << EMRdaq.GetV1731NumPartEvents() << std::endl;

  for (int iPe = 0; iPe < nPartEvents; iPe++) {

    V1731HitArray fADChits = EMRdaq.GetV1731PartEvent(iPe);
    int nHits = fADChits.size();
//    std::cerr << "PartEvent " << iPe << "  --> " << nHits << " hits\n" << std::endl;

    for (int iHit = 0; iHit < nHits; iHit++) {
      V1731 fADChit = fADChits[iHit];
      int xLDC  = fADChit.GetLdcId();
      int xGeo  = fADChit.GetGeo();
      int xCh   = fADChit.GetChannel();
      int xArea = fADChit.GetPulseArea();
      int xPos  = fADChit.GetPositionMin();

      DAQChannelKey daq_key(xLDC, xGeo, xCh, 121, "emr");
//      std::cerr << daq_key << std::endl;
      EMRChannelKey *emr_key = _emrMap.find(&daq_key);
      if (emr_key) {
        int xPlane = emr_key->plane();
        int xOri   = emr_key->orientation();
//        std::cerr << iPe << "  " << *emr_key << " --> pos: "
//                  << xPos << "  area: " << xArea << std::endl;
        emr_fadc_events_tmp[iPe][xPlane]._orientation  = xOri;
        emr_fadc_events_tmp[iPe][xPlane]._charge       = xArea;
        emr_fadc_events_tmp[iPe][xPlane]._deltat       = xPos;
      }// else {std::cerr << "WARNING!!! unknow EMR fADC channel " << daq_key << std::endl;}
    }
  }
}

void MapCppEMRPlaneHits::fill(MAUS::Spill *spill,
			      int nPartEvents,
			      EMRDBBEventVector emr_dbb_events_tmp,
			      EMRfADCEventVector emr_fadc_events_tmp) const {

  int recPartEvents = spill->GetReconEventSize();
  ReconEventPArray *recEvts =  spill->GetReconEvents();

  // Resize the recon event to harbour all the EMR noise+decays
  while (recPartEvents < nPartEvents) {
    recEvts->push_back(new ReconEvent);
    recPartEvents++;
  }

//  std::cerr << spill->GetReconEventSize() << std::endl;

  for (int iPe = 0; iPe < nPartEvents; iPe++) {
    EMREvent *evt = new EMREvent;
    EMRPlaneHitArray plArray;

    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      EMRPlaneHit *plHit = new EMRPlaneHit;
      plHit->SetPlane(iPlane);
      plHit->SetTrigger(iPe);

      fADCdata xPlData = emr_fadc_events_tmp[iPe][iPlane];
      int xOri     = xPlData._orientation;
      int xCharge  = xPlData._charge;
      int xTime    = xPlData._time;
      int xSpill   = xPlData._spill;
      int xDeltaT  = xPlData._deltat;

      plHit->SetOrientation(xOri);
      plHit->SetCharge(xCharge);
      plHit->SetTime(xTime);
      plHit->SetSpill(xSpill);
      plHit->SetDeltaT(xDeltaT);

      EMRBarArray barArray;

      for (int iBar = 0; iBar < _number_of_bars; iBar++) {
        int nHits = emr_dbb_events_tmp[iPe][iPlane][iBar].size();
        if ( nHits ) {
          EMRBar *bar = new EMRBar;
          bar->SetBar(iBar);
          bar->SetEMRBarHitArray(emr_dbb_events_tmp[iPe][iPlane][iBar]);
          barArray.push_back(bar);
//          std::cerr << "Ev: " << iPe << "  Pl: " << iPlane
//                    << "   Bar: " << iBar << "  Hits: " <<  nHits << std::endl;
        }
      }

      plHit->SetEMRBarArray(barArray);
      if ( barArray.size() || xCharge ) {
        plArray.push_back(plHit);
//        std::cerr << "Ev: " << iPe <<  --> pos: " << xTime << "  area: " << xCharge
//                  << "  nBarHits: " << barArray.size() << std::endl;
      } else {
        delete plHit;
      }
    }

    evt->SetEMRPlaneHitArray(plArray);
    recEvts->at(iPe)->SetEMREvent(evt);
    recEvts->at(iPe)->SetPartEventNumber(iPe);
  }
  spill->SetReconEvents(recEvts);
}

EMRDBBEventVector MapCppEMRPlaneHits::get_dbb_data_tmp(int nPartEvts) const {
  EMRDBBEventVector emr_dbb_events_tmp;
  emr_dbb_events_tmp.resize(nPartEvts);
  for (int iPe = 0; iPe < nPartEvts; iPe++) {
    emr_dbb_events_tmp[iPe].resize(_number_of_planes);  // number of planes
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      emr_dbb_events_tmp[iPe][iPlane].resize(_number_of_bars); // number of bars in a plane
    }
  }
  return emr_dbb_events_tmp;
}

EMRfADCEventVector MapCppEMRPlaneHits::get_fadc_data_tmp(int nPartEvts) const {
  EMRfADCEventVector emr_fadc_events_tmp;
  emr_fadc_events_tmp.resize(nPartEvts);
  for (int iPe = 0; iPe < nPartEvts ;iPe++) {
    emr_fadc_events_tmp[iPe].resize(_number_of_planes);  // number of planes
    for (int iPlane = 0; iPlane < _number_of_planes; iPlane++) {
      fADCdata data;
      data._orientation = iPlane%2;
      data._charge = 0;
      data._time = 0;
      data._deltat = 0;
      data._spill = 0;
      emr_fadc_events_tmp[iPe][iPlane] = data;
    }
  }
  return emr_fadc_events_tmp;
}
}
