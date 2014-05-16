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

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/DAQChannelMap.hh"
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

  map_file_name = JsonWrapper::GetProperty(configJSON,
                                           "EMR_cabling_file",
                                           JsonWrapper::stringValue);

  std::string xMapFile = std::string(pMAUS_ROOT_DIR) + map_file_name.asString();
  bool loaded = _emrMap.InitFromFile(xMapFile);
  if (!loaded)
    throw MAUS::Exception(Exception::recoverable,
                          "Failed to load EMR Map File",
                          "MapCppEMRHits::birth");

  // !!!!!!!!!!!!!!!!!!!!!!
  _trigger_window_lower = -400;
  _trigger_window_upper = 1000;
  // !!!!!!!!!!!!!!!!!!!!!!

  xEnable_V1731_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_V1731_Unpacking",
                                                     JsonWrapper::booleanValue);
  if (!xEnable_V1731_Unpacking.asBool()) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppKLDigits::birth. The unpacking of the flashADC V1724 is disabled!!!"
    << " Are you shure you want this?"
    << std::endl;
  }

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

  Spill *spill    = data->GetSpill();
  if (spill->GetDAQData() == NULL)
      return;
  EMRDaq emr_data = spill->GetDAQData()->GetEMRDaq();
  int nPartEvents = emr_data.GetV1731NumPartEvents();
//   cout << "nPartEvts: " << nPartEvents << endl;
  EMREventVector_2 emr_events_tmp2 = get_data_tmp2(nPartEvents);
  EMREventVector_4 emr_events_tmp4 = get_data_tmp4(nPartEvents);

  processFADC(emr_data, nPartEvents, emr_events_tmp2);
  processDBB(emr_data, nPartEvents, emr_events_tmp2, emr_events_tmp4);

//   arrange(emr_data);
  fill(spill, nPartEvents, emr_events_tmp2, emr_events_tmp4);
}

void MapCppEMRPlaneHits::processDBB(EMRDaq EMRdaq,
                                    int nPartTrigger,
                                    EMREventVector_2& emr_events_tmp2,
                                    EMREventVector_4& emr_events_tmp4) const {
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
            emr_events_tmp2[ipe][xPlane]._time  = tr_lt;
            emr_events_tmp2[ipe][xPlane]._spill = xSpill;
          }

          int delta_t = lt - tr_lt;
          if (delta_t > _trigger_window_lower && delta_t < _trigger_window_upper) {
            EMRBarHit bHit;
            bHit.SetTot(tt-lt);
            bHit.SetDeltaT(delta_t - _trigger_window_lower);
//             cout << "*---> " << *emr_key << " --> trigger_Id: " << ipe
//                  << "  tot: " << tt-lt
//                  << "  delta: " << delta_t - _trigger_window_lower
//                  << "(" << delta_t << ")" << endl;
            emr_events_tmp4[ipe][xPlane][xBar].push_back(bHit);
          }
        }
      }/* else {cout << "WARNING!!! unknow EMR DBB channel " << daq_key << endl;}*/
    }
  }
}

void MapCppEMRPlaneHits::processFADC(EMRDaq EMRdaq,
                                     int nPartTrigger,
                                     EMREventVector_2& emr_events_tmp2) const {
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
        emr_events_tmp2[ipe][xPlane]._orientation = xOri;
        emr_events_tmp2[ipe][xPlane]._charge      = xArea;
        emr_events_tmp2[ipe][xPlane]._deltat      = xPos;
      }/* else {cout << "WARNING!!! unknow EMR fADC channel " << daq_key << endl;}*/
    }
  }
}

void MapCppEMRPlaneHits::fill(Spill *spill,
                              int nPartTrigger,
                              EMREventVector_2& emr_events_tmp2,
                              EMREventVector_4& emr_events_tmp4) const {
  ReconEventPArray *recEvts =  spill->GetReconEvents();
  if (recEvts->size() == 0) {
    for (int ipe = 0; ipe < nPartTrigger; ipe++)
      recEvts->push_back(new ReconEvent);
  }

  for (int ipe = 0; ipe < nPartTrigger; ipe++) {
    EMREvent *evt = new EMREvent;
    EMRPlaneHitArray plArray;

    for (int ipl = 0; ipl < NUM_DBB_PLANES; ipl++) {
      EMRPlaneHit *plHit = new EMRPlaneHit;
      plHit->SetPlane(ipl);
      plHit->SetTrigger(ipe);

      EMRPlaneData xPlData = emr_events_tmp2[ipe][ipl];
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

      for (int ibar = 0; ibar < NUM_DBB_CHANNELS; ibar++) {
        int nHits = emr_events_tmp4[ipe][ipl][ibar].size();
        if ( nHits ) {
          EMRBar *bar = new EMRBar;
          bar->SetBar(ibar);
          bar->SetEMRBarHitArray(emr_events_tmp4[ipe][ipl][ibar]);
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

EMREventVector_4 MapCppEMRPlaneHits::get_data_tmp4(int nPartEvts) const {
  EMREventVector_4 emr_events_tmp4;
  emr_events_tmp4.resize(nPartEvts);
  for (int ipe = 0; ipe < nPartEvts ;ipe++) {
    emr_events_tmp4[ipe].resize(NUM_DBB_PLANES);  // number of planes
    for (int iplane = 0; iplane < NUM_DBB_PLANES; iplane++) {
      emr_events_tmp4[ipe][iplane].resize(NUM_DBB_CHANNELS); // number of bars in a plane
    }
  }
  return emr_events_tmp4;
}

EMREventVector_2 MapCppEMRPlaneHits::get_data_tmp2(int nPartEvts) const {
  EMREventVector_2 emr_events_tmp2;
  emr_events_tmp2.resize(nPartEvts);
  for (int ipe = 0; ipe < nPartEvts ;ipe++) {
    emr_events_tmp2[ipe].resize(NUM_DBB_PLANES);
  }
  return emr_events_tmp2;
}
}
