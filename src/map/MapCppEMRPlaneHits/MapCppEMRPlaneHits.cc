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

#include "map/MapCppEMRPlaneHits/MapCppEMRPlaneHits.hh"

// using namespace std;
using namespace MAUS;

// bool MapCppEMRPlaneHits::birth(std::string argJsonConfigDocument) {
bool MapCppEMRPlaneHits::birth() {
  _classname = "MapCppEMRPlaneHits";
  _emrMap.InitFromFile("../../maus-data/src/toolslib/EMRChannelMap.txt");
  _trigger_window_lower = -400;
  _trigger_window_upper = 1000;
}

bool MapCppEMRPlaneHits::death() {}

string MapCppEMRPlaneHits::process(string document) {}

// void MapCppEMRPlaneHits::process(Spill *spill) {
void MapCppEMRPlaneHits::process(Data *data) {

  Spill *spill    = data->GetSpill();
  EMRDaq emr_data = spill->GetDAQData()->GetEMRDaq();
  int nPartEvents = emr_data.GetV1731NumPartEvents();
//   cout << "nPartEvts: " << nPartEvents << endl;
  reset_data_tmp(nPartEvents);

  processFADC(emr_data, nPartEvents);
  processDBB(emr_data, nPartEvents);

//   arrange(emr_data);
  fill(spill, nPartEvents);
}

void MapCppEMRPlaneHits::processDBB(EMRDaq EMRdaq, int nPartTrigger) {
//   cout << "DBBArraySize: " << EMRdaq.GetDBBArraySize() << endl;
  int nDBBs = EMRdaq.GetDBBArraySize();
  for (int idbb = 0; idbb < nDBBs; idbb++) {

    DBBSpillData dbb = EMRdaq.GetDBBArrayElement(idbb);
    if ( dbb.GetTriggerCount() != nPartTrigger ) {
      stringstream ss;
      ss << "ERROR in  MapCppEMRPlaneHits::processDBB: trigger count is wrong ("
         << dbb.GetTriggerCount() << "!=" << nPartTrigger << ")";
      throw MDexception(ss.str());
    }

    int xLDC    = dbb.GetLdcId();
    int xSpill = dbb.GetSpillNumber();
    int xGeo    = dbb.GetDBBId();
    int nHits   = dbb.GetDBBHitsArraySize();
    int nTr     = dbb.GetDBBTriggersArraySize();

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
        int xOri   = emr_key->orientation();
        int xBar   = emr_key->bar();
//         cout << *emr_key << " --> lt: " << lt << "  tt: " << tt << endl;

        for (int ipe = 0; ipe < nPartTrigger; ipe++) {
          DBBHit this_trigger = dbb.GetDBBTriggersArrayElement(ipe);
          int tr_lt = this_trigger.GetLTime();
          int tr_tt = this_trigger.GetTTime();
          int xCh   = this_trigger.GetChannel();

          if (ihit == 0) { // Set the plane information only when processing the very first hit.
            _emr_events_tmp2[ipe][xPlane]._time  = tr_lt;
            _emr_events_tmp2[ipe][xPlane]._spill = xSpill;
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
            _emr_events_tmp4[ipe][xPlane][xBar].push_back(bHit);
          }
        }
      }/* else {cout << "WARNING!!! unknow EMR DBB channel " << daq_key << endl;}*/
    }
  }
}

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
      MAUS::EMRChannelKey *emr_key = _emrMap.find(&daq_key);
      if (emr_key) {
        int xPlane = emr_key->plane();
        int xOri   = emr_key->orientation();
//         cout << ipe << "  " << *emr_key << " --> pos: "
//              << xPos << "  area: " << xArea << endl << endl;
        _emr_events_tmp2[ipe][xPlane]._orientation = xOri;
        _emr_events_tmp2[ipe][xPlane]._charge      = xArea;
        _emr_events_tmp2[ipe][xPlane]._deltat      = xPos;
      }/* else {cout << "WARNING!!! unknow EMR fADC channel " << daq_key << endl;}*/
    }
  }
}

void MapCppEMRPlaneHits::fill(Spill *spill, int nPartTrigger) {
  ReconEventArray *recEvts =  spill->GetReconEvents();
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

      EMRPlaneData xPlData = _emr_events_tmp2[ipe][ipl];
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
        int nHits = _emr_events_tmp4[ipe][ipl][ibar].size();
        if ( nHits ) {
          EMRBar *bar = new EMRBar;
          bar->SetBar(ibar);
          bar->SetEMRBarHitArray(_emr_events_tmp4[ipe][ipl][ibar]);
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

void MapCppEMRPlaneHits::reset_data_tmp(int nPartEvts) {
  _emr_events_tmp4.resize(0);
  _emr_events_tmp4.resize(nPartEvts);
  for (int ipe = 0; ipe < nPartEvts ;ipe++) {
    _emr_events_tmp4[ipe].resize(NUM_DBB_PLANES);  // number of planes
    for (int iplane = 0; iplane < NUM_DBB_PLANES; iplane++) {
      _emr_events_tmp4[ipe][iplane].resize(NUM_DBB_CHANNELS); // number of bars in a plane
    }
  }

  _emr_events_tmp2.resize(0);
  _emr_events_tmp2.resize(nPartEvts);
  for (int ipe = 0; ipe < nPartEvts ;ipe++) {
    _emr_events_tmp2[ipe].resize(NUM_DBB_PLANES);
  }
}
