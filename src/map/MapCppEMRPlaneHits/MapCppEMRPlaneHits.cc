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
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"

#include "src/map/MapCppEMRPlaneHits/MapCppEMRPlaneHits.hh"

namespace MAUS {

////////////////////////////////////////////////////////////////////////////
bool MapCppEMRPlaneHits::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppEMRPlaneHits";
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }

  try {
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
      return false;

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

    return true;
  } catch (Exception exc) {
    CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, _classname);
  } catch (std::exception exc) {
    CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////
bool MapCppEMRPlaneHits::death() {
  return true;
}

////////////////////////////////////////////////////////////////////////////
string MapCppEMRPlaneHits::process(string document) {

  Json::Value spill_json;
  try {
    spill_json = JsonWrapper::StringToJson(document);
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    spill_json["errors"] = errors;
    return JsonWrapper::JsonToString(spill_json);
  }

  Data* spill_cpp;
  try {
    spill_cpp = JsonCppSpillConverter().convert(&spill_json);
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to convert the input document into Data";
    errors["bad_json_document"] = ss.str();
    spill_json["errors"] = errors;
    return JsonWrapper::JsonToString(spill_json);
  }

  process(spill_cpp);

  Json::Value* spill_json_out;
  try {
    spill_json_out = CppJsonSpillConverter().convert(spill_cpp);
  } catch (...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to convert the output Data into Json";
    errors["bad_cpp_data"] = ss.str();
    spill_json["errors"] = errors;
    return JsonWrapper::JsonToString(spill_json);
  }

//   std::cerr << (*spill_json_out)["recon_events"] << std::endl;
  document = JsonWrapper::JsonToString(*spill_json_out);

  delete spill_cpp;
  delete spill_json_out;
  return document;
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRPlaneHits::process(Data *data) {

  // Get spill, break if there's no DAQ data
  Spill *spill = data->GetSpill();

  if (spill->GetDAQData() == NULL)
      return;

  EMRDaq emr_data = spill->GetDAQData()->GetEMRDaq();
  int nPartEvents = emr_data.GetV1731NumPartEvents();
//   cout << "nPartEvts: " << nPartEvents << endl;

  // Reset/Resize DBB and fADC arrays with n+2 events (1 per trigger + noise + decays)
  reset_data_tmp(nPartEvents+2);

  // Fill the fADC and DBB array with DAQ information
  processFADC(emr_data, nPartEvents);
  processDBB(emr_data, nPartEvents);

  // Fill the Recon event array with Spill information (1 per trigger + noise + decays)
  fill(spill, nPartEvents+2);
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRPlaneHits::processDBB(EMRDaq EMRdaq, int nPartTrigger) {
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
//     int nTr     = dbb.GetDBBTriggersArraySize();

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

        for (int ipe = 0; ipe < nPartTrigger; ipe++) {
          DBBHit this_trigger = dbb.GetDBBTriggersArrayElement(ipe);
          int tr_lt = this_trigger.GetLTime();
//           int tr_tt = this_trigger.GetTTime();
//           int xCh   = this_trigger.GetChannel();

          if (ihit == 0) { // Set the plane information only when processing the very first hit.
            _emr_fadc_events_tmp[ipe][xPlane]._time  = tr_lt;
            _emr_fadc_events_tmp[ipe][xPlane]._spill = xSpill;
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
//                  << "  delta: " << delta_t - _trigger_window_lower
//                  << "(" << delta_t << ")" << endl;
            _emr_dbb_events_tmp[ipe][xPlane][xBar].push_back(bHit);
          } else if (delta_t > _deltat_noise_low && delta_t < _deltat_noise_up &&
		   tot > _tot_noise_low && tot < _tot_noise_up ) {
            bHit.SetDeltaT(delta_t - _deltat_signal_low);
	    _emr_dbb_events_tmp[nPartTrigger][xPlane][xBar].push_back(bHit);
	  } else {
	    bHit.SetDeltaT(0);
	    _emr_dbb_events_tmp[nPartTrigger+1][xPlane][xBar].push_back(bHit);
	  }
        }
      }/* else {cout << "WARNING!!! unknow EMR DBB channel " << daq_key << endl;}*/
    }
  }
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRPlaneHits::processFADC(EMRDaq EMRdaq, int nPartTrigger) {
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
        _emr_fadc_events_tmp[ipe][xPlane]._orientation  = xOri;
        _emr_fadc_events_tmp[ipe][xPlane]._charge       = xArea;
        _emr_fadc_events_tmp[ipe][xPlane]._deltat       = xPos;
      }/* else {cout << "WARNING!!! unknow EMR fADC channel " << daq_key << endl;}*/
    }
  }
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRPlaneHits::fill(Spill *spill, int nPartTrigger) {

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

      fADCdata xPlData = _emr_fadc_events_tmp[ipe][ipl];
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
        int nHits = _emr_dbb_events_tmp[ipe][ipl][ibar].size();
        if ( nHits ) {
          EMRBar *bar = new EMRBar;
          bar->SetBar(ibar);
          bar->SetEMRBarHitArray(_emr_dbb_events_tmp[ipe][ipl][ibar]);
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
  }
  spill->SetReconEvents(recEvts);
}

////////////////////////////////////////////////////////////////////////////
void MapCppEMRPlaneHits::reset_data_tmp(int nPartEvts) {

  _emr_dbb_events_tmp.resize(0);
  _emr_dbb_events_tmp.resize(nPartEvts);

  for (int ipe = 0; ipe < nPartEvts; ipe++) {
    _emr_dbb_events_tmp[ipe].resize(_number_of_planes);  // number of planes
    for (int iplane = 0; iplane < _number_of_planes; iplane++) {
      _emr_dbb_events_tmp[ipe][iplane].resize(_number_of_bars); // number of bars in a plane
    }
  }

  _emr_fadc_events_tmp.resize(0);
  _emr_fadc_events_tmp.resize(nPartEvts);

  for (int ipe = 0; ipe < nPartEvts; ipe++) {
    _emr_fadc_events_tmp[ipe].resize(_number_of_planes);
    for (int iplane = 0; iplane < _number_of_planes; iplane++) {
      fADCdata data;
      data._orientation = iplane % 2;
      data._charge = 0;
      data._time = 0;
      data._deltat = 0;
      _emr_fadc_events_tmp[ipe][iplane] = data;
    }
  }
}
}
