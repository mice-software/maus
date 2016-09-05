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

#include <Python.h>
#include <stdlib.h>

#include <vector>
#include <iostream>
#include <string>

#include "TCanvas.h"
#include "TH1D.h"
#include "TMinuit.h"
#include "TFitResult.h"
#include "TF1.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TPaveStats.h"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/TOFEvent.hh"
#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFSpacePoint.hh"

#include "src/legacy/Interface/STLUtils.hh"
#include "Utils/Squeak.hh"

#include "src/reduce/ReduceCppTofCalib/ReduceCppTofCalib.hh"

namespace MAUS {

PyMODINIT_FUNC init_ReduceCppTofCalib(void) {
    PyWrapReduceBase<ReduceCppTofCalib>::PyWrapReduceBaseModInit(
                                  "ReduceCppTofCalib", "", "", "", "");
}

ReduceCppTofCalib::ReduceCppTofCalib()
  : ReduceBase<Data, Data>("ReduceCppTofCalib") {}

ReduceCppTofCalib::~ReduceCppTofCalib() {
  // Everything will be deleted by the ImageData destructor.
}

void ReduceCppTofCalib::_birth(const std::string& argJsonConfigDocument) {

  if (!_output) {
    throw MAUS::Exceptions::Exception(Exceptions::nonRecoverable,
                          "The output is disconnected.",
                          "ReduceCppTofCalib::_birth");
  }

  if (!MakeTree())
      std::cerr << "Failed to make TTree" << std::endl;

  // JsonCpp setup - check file parses correctly, if not return false
  Json::Value configJSON;
  try {
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
    // this will contain the configuration
  } catch (Exceptions::Exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, _classname);
  } catch (std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
}

void ReduceCppTofCalib::_death() {
  Save();
}

void ReduceCppTofCalib::_process(MAUS::Data* data) {
  if (data == NULL)
    throw Exceptions::Exception(Exceptions::recoverable, "Data was NULL",
                    "ReduceCppTofCalib::_process");

  if (data->GetSpill() == NULL)
    throw Exceptions::Exception(Exceptions::recoverable, "Spill was NULL",
                    "ReduceCppTofCalib::_process");

  std::string ev_type = data->GetSpill()->GetDaqEventType();

  if (ev_type != "physics_event")
     return;

  if (data->GetSpill()->GetReconEvents() == NULL)
     throw Exceptions::Exception(Exceptions::recoverable, "ReconEvents were NULL",
                        "ReduceCppTofCalib::_process");


  Spill *spillPtr = data->GetSpill();
  _spill = *spillPtr;
  this->processSpill();
}

void ReduceCppTofCalib::processSpill() {
  if (_spill.GetDaqEventType() == "physics_event") {
    int runNum = _spill.GetRunNumber();
    std::stringstream fname;
    fname << "TofCalibRun" << runNum << ".root";
    _filename = fname.str();

    int n_part_events = _spill.GetReconEventSize();
//       std::cerr << n_part_events << std::endl;
    for (int PartEvent = 0; PartEvent < n_part_events; PartEvent++) {
//       std::cerr << PartEvent << std::endl;
      // Get this particle event in TOF0, TOF1 and TOF2.
      ReconEvent revt = _spill.GetAReconEvent(PartEvent);
      TOFEvent* tof_event = revt.GetTOFEvent();
     if (!tof_event)
       continue;

      TOFEventSlabHit slbhits = tof_event->GetTOFEventSlabHit();

      int ntof0_sh = slbhits.GetTOF0SlabHitArraySize();
      int ntof1_sh = slbhits.GetTOF1SlabHitArraySize();
      int ntof2_sh = slbhits.GetTOF2SlabHitArraySize();

      if (!(ntof0_sh == 2 && (ntof1_sh == 2 || ntof2_sh == 2)))
        continue;

      // Make sure that all times and charges are set to zero and
      // that all slab numbers are set to -99.
      slabA = -99;
      slabB = -99;
      slabC = -99;
      slabD = -99;
      slabE = -99;
      slabF = -99;
      adc0 = adc1 = adc2 =adc3 = 0;
      adc4 = adc5 = adc6 = adc7 = 0;
      adc8 = adc9 = adc10 = adc11 =0;
      t0 = t1 = t2 = t3 = 0.;
      t4 = t5 = t6 = t7 = 0.;
      t8 = t9 = t10 = t11 = 0.;

      int tpln, tslb, ch0, ch1;
      double rt0, rt1;
      Pmt0 pm0;
      Pmt1 pm1;
      for (int h = 0; h < ntof0_sh; ++h) {
        TOFSlabHit tof0_sh = slbhits.GetTOF0SlabHitArrayElement(h);
        tpln = tof0_sh.GetPlane();
        tslb = tof0_sh.GetSlab();
        pm0 = tof0_sh.GetPmt0();
        pm1 = tof0_sh.GetPmt1();
        rt0 = pm0.GetRawTime();
        ch0 = pm0.GetCharge();
        rt1 = pm1.GetRawTime();
        ch1 = pm1.GetCharge();
        if (tpln == 0) {
          slabA = tslb;
          adc0 = ch0;
          adc1 = ch1;
          t0 = rt0*1000;
          t1 = rt1*1000;
        } else if (tpln == 1) {
          slabB = tslb;
          adc2 = ch0;
          adc3 = ch1;
          t2 = rt0*1000;
          t3 = rt1*1000;
        }
        if (ntof1_sh == 2) {
          TOFSlabHit tof1_sh = slbhits.GetTOF1SlabHitArrayElement(h);
          tpln = tof1_sh.GetPlane();
          tslb = tof1_sh.GetSlab();
          pm0 = tof1_sh.GetPmt0();
          pm1 = tof1_sh.GetPmt1();
          rt0 = pm0.GetRawTime();
          ch0 = pm0.GetCharge();
          rt1 = pm1.GetRawTime();
          ch1 = pm1.GetCharge();
          if (tpln == 0) {
            slabC = tslb;
            adc4 = ch0;
            adc5 = ch1;
            t4 = rt0*1000;
            t5 = rt1*1000;
          } else if (tpln == 1) {
            slabD = tslb;
            adc6 = ch0;
            adc7 = ch1;
            t6 = rt0*1000;
            t7 = rt1*1000;
          }
        }
        if (ntof2_sh == 2) {
          TOFSlabHit tof2_sh = slbhits.GetTOF2SlabHitArrayElement(h);
          tpln = tof2_sh.GetPlane();
          tslb = tof2_sh.GetSlab();
          pm0 = tof2_sh.GetPmt0();
          pm1 = tof2_sh.GetPmt1();
          rt0 = pm0.GetRawTime();
          ch0 = pm0.GetCharge();
          rt1 = pm1.GetRawTime();
          ch1 = pm1.GetCharge();
          if (tpln == 0) {
            slabE = tslb;
            adc8 = ch0;
            adc9 = ch1;
            t8 = rt0*1000;
            t9 = rt1*1000;
          } else if (tpln == 1) {
            slabF = tslb;
            adc10 = ch0;
            adc11 = ch1;
            t10 = rt0*1000;
            t11 = rt1*1000;
          }
        }
      }
      dataTree.Fill();
    }
  }
}


void ReduceCppTofCalib::reset() {
}

void ReduceCppTofCalib::update() {
}
void ReduceCppTofCalib::Save() {
  this->Save(_filepath + _filename);
}

void ReduceCppTofCalib::Save(std::string fname) {
  if (!dataTree.GetEntries())
    return;

  TFile datafile(fname.c_str(), "recreate" );
  datafile.cd();

  // Get the data tree.
  dataTree.Write();

//   datafile.ls();
  datafile.Close();
  Squeak::mout(Squeak::info) << _filename << " is updated." << std::endl;
}
bool ReduceCppTofCalib::MakeTree() {
  dataTree.SetNameTitle("dataTree", "tofdata");
  dataTree.Branch("slabA", &slabA, "slabA/I");
  dataTree.Branch("slabB", &slabB, "slabB/I");
  dataTree.Branch("t0", &t0, "t0/D");
  dataTree.Branch("t1", &t1, "t1/D");
  dataTree.Branch("t2", &t2, "t2/D");
  dataTree.Branch("t3", &t3, "t3/D");
  dataTree.Branch("adc0", &adc0, "adc0/I");
  dataTree.Branch("adc1", &adc1, "adc1/I");
  dataTree.Branch("adc2", &adc2, "adc2/I");
  dataTree.Branch("adc3", &adc3, "adc3/I");

  dataTree.Branch("slabC", &slabC, "slabC/I");
  dataTree.Branch("slabD", &slabD, "slabD/I");
  dataTree.Branch("t4", &t4, "t4/D");
  dataTree.Branch("t5", &t5, "t5/D");
  dataTree.Branch("t6", &t6, "t6/D");
  dataTree.Branch("t7", &t7, "t7/D");
  dataTree.Branch("adc4", &adc4, "adc4/I");
  dataTree.Branch("adc5", &adc5, "adc5/I");
  dataTree.Branch("adc6", &adc6, "adc6/I");
  dataTree.Branch("adc7", &adc7, "adc7/I");

  dataTree.Branch("slabE", &slabE, "slabE/I");
  dataTree.Branch("slabF", &slabF, "slabF/I");
  dataTree.Branch("t8", &t8, "t8/D");
  dataTree.Branch("t9", &t9, "t9/D");
  dataTree.Branch("t10", &t10, "t10/D");
  dataTree.Branch("t11", &t11, "t11/D");
  dataTree.Branch("adc8", &adc8, "adc8/I");
  dataTree.Branch("adc9", &adc9, "adc9/I");
  dataTree.Branch("adc10", &adc10, "adc10/I");
  dataTree.Branch("adc11", &adc11, "adc11/I");

  return true;
}
} // MAUS
