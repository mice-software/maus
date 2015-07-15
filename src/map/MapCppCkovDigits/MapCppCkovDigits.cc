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
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "API/PyWrapMapBase.hh"

#include "src/map/MapCppCkovDigits/MapCppCkovDigits.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppCkovDigits(void) {
  PyWrapMapBase<MAUS::MapCppCkovDigits>::PyWrapMapBaseModInit
                                            ("MapCppCkovDigits", "", "", "", "");
}

MapCppCkovDigits::MapCppCkovDigits() : MapBase<MAUS::Data>("MapCppCkovDigits") {
}

void MapCppCkovDigits::_birth(const std::string& argJsonConfigDocument) {

  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    throw MAUS::Exception(Exception::recoverable,
              "Could not find the $MAUS_ROOT_DIR environmental variable.",
              "MapCppCkovDigits::_birth");
  }

  // Check if the JSON document can be parsed, else return error only
  //  JsonCpp setup
  Json::Value configJSON;
  Json::Value map_file_name;
  Json::Value xEnable_V1731_Unpacking;
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  xEnable_V1731_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                     "Enable_V1731_Unpacking",
                                                     JsonWrapper::booleanValue);

  if (!xEnable_V1731_Unpacking.asBool()) {
    Squeak::mout(Squeak::warning)
    << "WARNING in MapCppCkovDigits::birth. The unpacking of the flashADC V1731 is disabled!!!"
    << " Are you sure you want this?"
    << std::endl;
  }
  _position_threshold = 40;
  _pulse_area_threshold = 0;
  _window_min = 19;
  _window_max = 80;
  _maxCoincidences = 4;
}


void MapCppCkovDigits::_death()  {}

///////////////////////////////////////////////////////////
void MapCppCkovDigits::_process(MAUS::Data* data) const {

    Spill *spill = data->GetSpill();
    if (spill->GetDAQData() == NULL)
      return;

    /* return if not physics spill */
    if (spill->GetDaqEventType() != "physics_event")
      return;

    CkovArray *ckov_data = spill->GetDAQData()->GetCkovArrayPtr();
    int n_part_events = ckov_data->size();

    int recPartEvents = spill->GetReconEventSize();
    ReconEventPArray *recEvts =  spill->GetReconEvents();

    // Check if recon events, if not initialize
    if (recPartEvents == 0) { // No recEvts yet
      for (int iPe = 0; iPe < n_part_events; iPe++) {
        recEvts->push_back(new ReconEvent);
      }
    }

    /* loop over events, and get adc, calculate signal, set digits */
    for (int xPE = 0; xPE < n_part_events; xPE++) {
      CkovEvent *evt = new CkovEvent();
      (*recEvts)[xPE]->SetCkovEvent(evt);
      CkovDigitArray *ckov_digits = evt->GetCkovDigitArrayPtr();

      /* return if no ckov daq data */
      if (ckov_data->at(xPE) == NULL)
        continue;

      V1731Array ckov_adc_hits = (ckov_data->at(xPE))->GetV1731Array();
      /* skip if no v1731 hits */
      if (ckov_adc_hits.size() == 0)
        continue;

      CkovTmpDigs ckov_digs_tmp = getAdc(ckov_adc_hits);

      this->setDigits(ckov_digs_tmp, ckov_digits);
    }
}

///////////////////////////////////////////////////////////
CkovTmpDigs MapCppCkovDigits::getAdc(V1731Array &adcHits) const {
      double _one_pe[8] = {23., 23., 23., 23., 23., 23., 23., 23.};
      int ckovid, pindex;

      CkovTmpDigs ckov_digs_tmp;
      ckov_digs_tmp.resize(2);

      for (int iPmt = 0; iPmt < adcHits.size(); iPmt++) {
          pindex = iPmt % 4;
          CkovAdcHits ckd;
          if (iPmt < 4)
              ckovid = 0;
          else
              ckovid = 1;

          ckd.pulse[pindex] = (adcHits[iPmt]).GetPulseArea();
          ckd.posmin[pindex] = (adcHits[iPmt]).GetPositionMin();
          ckd.arTime[pindex] = (adcHits[iPmt]).GetArrivalTime();

          if (ckd.pulse[pindex] >= _pulse_area_threshold &&
              ckd.posmin[pindex] > _window_min && ckd.posmin[pindex] <= _window_max) {
            ckd.totpulse[pindex] = ckd.pulse[pindex];
            ckd.pes[pindex] = (static_cast<double>(ckd.pulse[pindex])/_one_pe[iPmt]);
            ckd.position[pindex] = ckd.posmin[pindex];
          } else {
            ckd.totpulse[pindex] = 0;
            ckd.pes[pindex] = 0;
            ckd.position[pindex] = 300;
          }
          if (iPmt == 3 || iPmt == 7) {
            ckd.ckovindex = ckovid;
            ckd.partnum = adcHits[iPmt].GetPartEventNumber();
            getSignal(&ckd);
            ckov_digs_tmp[ckovid] = ckd;
          }
      }
      return ckov_digs_tmp;
}

///////////////////////////////////////////////////////////
void MapCppCkovDigits::getSignal(CkovAdcHits* ckdigs) const {
    int nzeros = std::count(ckdigs->totpulse, ckdigs->totpulse + 4, 0);
    ckdigs->ncoin = _maxCoincidences - nzeros;
    int max_index = std::distance(ckdigs->totpulse,
                                  std::max_element(ckdigs->totpulse, ckdigs->totpulse + 4));
    int dist;
    /* debug
    for (int kk=0; kk<4; ++kk) {
        std::cerr << "pulse: " << kk << " " << ckdigs->pulse[kk]
                  << " totpulse: " << kk << " " << ckdigs->totpulse[kk]
                  << "pos: " << kk << " " << ckdigs->position[kk
                  << "pes: " << kk << " " << ckdigs->pes[kk]
                  << std::endl;
    }
    */
    if (nzeros == 4) {
        ckdigs->totcharge = 0;
        ckdigs->totnpe = 0.0;
        ckdigs->ncoin = 0;
    } else if (nzeros == 3) {
        ckdigs->totcharge = std::accumulate(ckdigs->totpulse, ckdigs->totpulse + 4, 0);
        ckdigs->totnpe = std::accumulate(ckdigs->pes, ckdigs->pes + 4, 0.0);
    } else if (nzeros < 3) {
        for (int k = 0; k < 4; ++k) {
            dist = abs(ckdigs->position[k] - ckdigs->position[max_index]);
            if (dist > _position_threshold) {
                ckdigs->totpulse[k] = 0;
                ckdigs->pes[k] = 0;
            }
        }
        ckdigs->totcharge = std::accumulate(ckdigs->totpulse, ckdigs->totpulse + 4, 0);
        ckdigs->totnpe = std::accumulate(ckdigs->pes, ckdigs->pes + 4, 0.0);
    }
}

///////////////////////////////////////////////////////////
void MapCppCkovDigits::setDigits(CkovTmpDigs &tmpDigs, CkovDigitArray* theDigits) const {
      CkovDigit aDigit;
      CkovA ckov_a;
      CkovB ckov_b;

      // this is a little silly, the parameters should be set in arrays
      // rather than in noindexable string names
      // datastructure changes will have to wait for another day, DR 6/30/15
      // Set Ckov A
      ckov_a.SetArrivalTime0(tmpDigs[0].arTime[0]);
      ckov_a.SetArrivalTime1(tmpDigs[0].arTime[1]);
      ckov_a.SetArrivalTime2(tmpDigs[0].arTime[2]);
      ckov_a.SetArrivalTime3(tmpDigs[0].arTime[3]);

      ckov_a.SetPositionMin0(tmpDigs[0].posmin[0]);
      ckov_a.SetPositionMin1(tmpDigs[0].posmin[1]);
      ckov_a.SetPositionMin2(tmpDigs[0].posmin[2]);
      ckov_a.SetPositionMin3(tmpDigs[0].posmin[3]);

      ckov_a.SetPulse0(tmpDigs[0].pulse[0]);
      ckov_a.SetPulse1(tmpDigs[0].pulse[1]);
      ckov_a.SetPulse2(tmpDigs[0].pulse[2]);
      ckov_a.SetPulse3(tmpDigs[0].pulse[3]);

      ckov_a.SetPulse0(tmpDigs[0].pulse[0]);
      ckov_a.SetPulse1(tmpDigs[0].pulse[1]);
      ckov_a.SetPulse2(tmpDigs[0].pulse[2]);
      ckov_a.SetPulse3(tmpDigs[0].pulse[3]);

      ckov_a.SetTotalCharge(tmpDigs[0].totcharge);
      ckov_a.SetNumberOfPes(tmpDigs[0].totnpe);
      ckov_a.SetCoincidences(tmpDigs[0].ncoin);
      ckov_a.SetPartEventNumber(tmpDigs[0].partnum);

      // Set Ckov B
      ckov_b.SetArrivalTime4(tmpDigs[1].arTime[0]);
      ckov_b.SetArrivalTime5(tmpDigs[1].arTime[1]);
      ckov_b.SetArrivalTime6(tmpDigs[1].arTime[2]);
      ckov_b.SetArrivalTime7(tmpDigs[1].arTime[3]);

      ckov_b.SetPositionMin4(tmpDigs[1].posmin[0]);
      ckov_b.SetPositionMin5(tmpDigs[1].posmin[1]);
      ckov_b.SetPositionMin6(tmpDigs[1].posmin[2]);
      ckov_b.SetPositionMin7(tmpDigs[1].posmin[3]);

      ckov_b.SetPulse4(tmpDigs[1].pulse[0]);
      ckov_b.SetPulse5(tmpDigs[1].pulse[1]);
      ckov_b.SetPulse6(tmpDigs[1].pulse[2]);
      ckov_b.SetPulse7(tmpDigs[1].pulse[3]);

      ckov_b.SetTotalCharge(tmpDigs[1].totcharge);
      ckov_b.SetNumberOfPes(tmpDigs[1].totnpe);
      ckov_b.SetCoincidences(tmpDigs[1].ncoin);
      ckov_b.SetPartEventNumber(tmpDigs[1].partnum);

      aDigit.SetCkovA(ckov_a);
      aDigit.SetCkovB(ckov_b);
      theDigits->push_back(aDigit);
}
}
