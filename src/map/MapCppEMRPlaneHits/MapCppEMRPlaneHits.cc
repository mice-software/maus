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
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"

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
                      "MapCppEMRHits::birth");
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
                          "Failed to load EMR Map File",
                          "MapCppEMRHits::birth");

  // Enable fADC unpacking
  xEnable_V1731_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_V1731_Unpacking",
                                                     JsonWrapper::booleanValue);
  if (!xEnable_V1731_Unpacking.asBool()) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppKLDigits::birth. The unpacking of the flashADC V1724 is disabled!!!"
    << " Are you shure you want this?"
    << std::endl;
  }

  // Enable DBB unpacking
  xEnable_DBB_Unpacking   = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_DBB_Unpacking",
                                                     JsonWrapper::booleanValue);
  if (!xEnable_DBB_Unpacking.asBool()) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppKLDigits::birth. The unpacking of the flashADC V1724 is disabled!!!"
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
//   cout << "nPartEvts: " << nPartEvents << endl;

  // Create DBB and fADC arrays with n+2 events (1 per trigger + noise + decays)
  EMRDBBEventVector emr_dbb_events_tmp = get_dbb_data_tmp(nPartEvents+2);
  EMRfADCEventVector emr_fadc_events_tmp = get_fadc_data_tmp(nPartEvents+2);

  // Fill the fADC and DBB array with DAQ information
  processFADC(emr_data, nPartEvents, emr_fadc_events_tmp);
  processDBB(emr_data, nPartEvents, emr_dbb_events_tmp, emr_fadc_events_tmp);

  // Fill the Recon event array with Spill information (1 per trigger + noise + decays)
  fill(spill, nPartEvents+2, emr_dbb_events_tmp, emr_fadc_events_tmp);
}

void MapCppEMRPlaneHits::processDBB(MAUS::EMRDaq EMRdaq,
				    int nPartTrigger,
				    EMRDBBEventVector emr_dbb_events_tmp,
				    EMRfADCEventVector emr_fadc_events_tmp) const {
//   cout << "DBBArraySize: " << EMRdaq.GetDBBArraySize() << endl;
  int nDBBs = EMRdaq.GetDBBArraySize();
  for (int idbb = 0; idbb < nDBBs; idbb++) {

    DBBSpillData dbb = EMRdaq.GetDBBArrayElement(idbb);
    if ( dbb.GetTriggerCount() != nPartTrigger ) {
      Squeak::mout(Squeak::error)
      << "ERROR in  MapCppEMRPlaneHits::processDBB: number of triggers mismatch ("
      << dbb.GetTriggerCount() << "!=" << nPartTrigger << ")" << std::endl;
      return;
    }

    int xLDC    = dbb.GetLdcId();
    int xSpill  = dbb.GetSpillNumber();
    int xGeo    = dbb.GetDBBId();
    int nHits   = dbb.GetDBBHitsArraySize();
//    int nTr     = dbb.GetDBBTriggersArraySize();

    for (int ihit = 0; ihit < nHits; ihit++) {
      DBBHit this_hit = dbb.GetDBBHitsArrayElement(ihit);
      int xCh = this_hit.GetChannel();
      int lt  = this_hit.GetLTime();
      int tt  = this_hit.GetTTime();

      DAQChannelKey daq_key(xLDC, xGeo, xCh, 141, "emr");
//       cout << daq_key << endl;
      EMRChannelKey *emr_key = _emrMap.find(&daq_key);
      if (emr_key) {
        int xPlane = emr_key->plane();
//         int xOri   = emr_key->orientation();
        int xBar   = emr_key->bar();
//         cout << *emr_key << " --> lt: " << lt << "  tt: " << tt << endl;

        // Set the spill number for the noise and decay events
        if (ihit == 0) { // Set the spill information only when processing the very first hit.
          emr_fadc_events_tmp[nPartTrigger][xPlane]._spill = xSpill;
          emr_fadc_events_tmp[nPartTrigger+1][xPlane]._spill = xSpill;
        }

	// Loop over the trigger and tries to associate the hit to one of them
	bool matched = false;
        for (int ipe = 0; ipe < nPartTrigger; ipe++) {
          DBBHit this_trigger = dbb.GetDBBTriggersArrayElement(ipe);
          int tr_lt = this_trigger.GetLTime();
//           int tr_tt = this_trigger.GetTTime();
//           int xCh   = this_trigger.GetChannel();

          if (ihit == 0) { // Set the plane information only when processing the very first hit.
            emr_fadc_events_tmp[ipe][xPlane]._time  = tr_lt;
            emr_fadc_events_tmp[ipe][xPlane]._spill = xSpill;
          }

          int delta_t = lt - tr_lt;
          int tot = tt - lt;

	  // set bar hit
          EMRBarHit bHit;
          bHit.SetTot(tot);

	  // discriminate noise and decays from signal events
          if (delta_t > _deltat_signal_low && delta_t < _deltat_signal_up) {
            bHit.SetDeltaT(delta_t - _deltat_signal_low);
//             cout << "*---> " << *emr_key << " --> trigger_Id: " << ipe
//                  << "  tot: " << tt-lt
//                  << "  delta: " << delta_t - deltat_signal_low
//                  << "(" << delta_t << ")" << endl;
            emr_dbb_events_tmp[ipe][xPlane][xBar].push_back(bHit);
	    matched = true;
          } else if (delta_t > _deltat_noise_low && delta_t < _deltat_noise_up &&
		   tot > _tot_noise_low && tot < _tot_noise_up ) {
            bHit.SetDeltaT(delta_t - _deltat_signal_low);
	    emr_dbb_events_tmp[nPartTrigger][xPlane][xBar].push_back(bHit);
	    matched = true;
	  } else if (ipe == nPartTrigger-1 && !matched) {
	    bHit.SetDeltaT(0); // tt -lt is irrelevant for decay products
	    emr_dbb_events_tmp[nPartTrigger+1][xPlane][xBar].push_back(bHit);
	  }
        }
      }/* else {cout << "WARNING!!! unknow EMR DBB channel " << daq_key << endl;}*/
    }
  }
}

void MapCppEMRPlaneHits::processFADC(MAUS::EMRDaq EMRdaq,
				     int nPartTrigger,
				     EMRfADCEventVector emr_fadc_events_tmp) const {
//   cout << "GetV1731NumPartEvents: " << EMRdaq.GetV1731NumPartEvents() << endl;

  for (int ipe = 0; ipe < nPartTrigger; ipe++) {

    V1731HitArray fADChits = EMRdaq.GetV1731PartEvent(ipe);
    int nHits = fADChits.size();
//     cout << "PartEvent " << ipe << "  --> " << nHits << " hits\n" << endl;

    for (int ihit = 0; ihit < nHits; ihit++) {
      V1731 fADChit = fADChits[ihit];
      int xLDC  = fADChit.GetLdcId();
      int xGeo  = fADChit.GetGeo();
      int xCh   = fADChit.GetChannel();
      int xArea = fADChit.GetPulseArea();
      int xPos  = fADChit.GetPositionMin();

      DAQChannelKey daq_key(xLDC, xGeo, xCh, 121, "emr");
//       cout << daq_key << endl;
      EMRChannelKey *emr_key = _emrMap.find(&daq_key);
      if (emr_key) {
        int xPlane = emr_key->plane();
        int xOri   = emr_key->orientation();
//         cout << ipe << "  " << *emr_key << " --> pos: "
//              << xPos << "  area: " << xArea << endl << endl;
        emr_fadc_events_tmp[ipe][xPlane]._orientation  = xOri;
        emr_fadc_events_tmp[ipe][xPlane]._charge       = xArea;
        emr_fadc_events_tmp[ipe][xPlane]._deltat       = xPos;
      }/* else {cout << "WARNING!!! unknow EMR fADC channel " << daq_key << endl;}*/
    }
  }
}

void MapCppEMRPlaneHits::fill(MAUS::Spill *spill,
			      int nPartTrigger,
			      EMRDBBEventVector emr_dbb_events_tmp,
			      EMRfADCEventVector emr_fadc_events_tmp) const {

  int recPartEvents = spill->GetReconEventSize();
  ReconEventPArray *recEvts =  spill->GetReconEvents();

  // Resize the recon event to harbour all the EMR noise+decays
  if (recPartEvents == 0) { // No recEvts yet
      for (int ipe = 0; ipe < nPartTrigger; ipe++) {
        recEvts->push_back(new ReconEvent);
    }
  }

  if (nPartTrigger == recPartEvents+2) { // Regular sized recEvts already created
    for (int ipe = 0; ipe < 2; ipe++) {
      recEvts->push_back(new ReconEvent);
    }
  }

  for (int ipe = 0; ipe < nPartTrigger; ipe++) {
    EMREvent *evt = new EMREvent;
    EMRPlaneHitArray plArray;

    for (int ipl = 0; ipl < _number_of_planes; ipl++) {
      EMRPlaneHit *plHit = new EMRPlaneHit;
      plHit->SetPlane(ipl);
      plHit->SetTrigger(ipe);

      fADCdata xPlData = emr_fadc_events_tmp[ipe][ipl];
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

      for (int ibar = 0; ibar < _number_of_bars; ibar++) {
        int nHits = emr_dbb_events_tmp[ipe][ipl][ibar].size();
        if ( nHits ) {
          EMRBar *bar = new EMRBar;
          bar->SetBar(ibar);
          bar->SetEMRBarHitArray(emr_dbb_events_tmp[ipe][ipl][ibar]);
          barArray.push_back(bar);
//           cout << "Ev: " << ipe << "  Pl: " << ipl
//                << "   Bar: " << ibar << "  Hits: " <<  nHits << endl;
        }
      }

      plHit->SetEMRBarArray(barArray);
      if ( barArray.size() || xCharge ) {
        plArray.push_back(plHit);
//         cout << "Ev: " << ipe << "  pl: " << ipl << "  --> pos: " << xPos << "  area: " << xArea
//              << "  nBarHits: " << barArray.size() << endl;
      } else {
        delete plHit;
      }
    }

    evt->SetEMRPlaneHitArray(plArray);
    recEvts->at(ipe)->SetEMREvent(evt);
    recEvts->at(ipe)->SetPartEventNumber(ipe);
  }
  spill->SetReconEvents(recEvts);
}

EMRDBBEventVector MapCppEMRPlaneHits::get_dbb_data_tmp(int nPartEvts) const {
  EMRDBBEventVector emr_dbb_events_tmp;
  emr_dbb_events_tmp.resize(nPartEvts);
  for (int ipe = 0; ipe < nPartEvts; ipe++) {
    emr_dbb_events_tmp[ipe].resize(_number_of_planes);  // number of planes
    for (int iplane = 0; iplane < _number_of_planes; iplane++) {
      emr_dbb_events_tmp[ipe][iplane].resize(_number_of_bars); // number of bars in a plane
    }
  }
  return emr_dbb_events_tmp;
}

EMRfADCEventVector MapCppEMRPlaneHits::get_fadc_data_tmp(int nPartEvts) const {
  EMRfADCEventVector emr_fadc_events_tmp;
  emr_fadc_events_tmp.resize(nPartEvts);
  for (int ipe = 0; ipe < nPartEvts ;ipe++) {
    emr_fadc_events_tmp[ipe].resize(_number_of_planes);
  }
  return emr_fadc_events_tmp;
}
}
