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

#include "TFile.h"
#include "TTree.h"

#include "src/common_cpp/Utils/JsonWrapper.hh"

#include "src/input/InputCppRoot/InputCppRoot.hh"

#include "src/common_cpp/JsonCppStreamer/JsonCppConverter.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

namespace MAUS {

InputCppRoot::InputCppRoot(std::string filename) : _infile(NULL),
              _jsonCppConverter(NULL),  _spill(NULL), _filename(filename) {
}

InputCppRoot::~InputCppRoot() {
  death();
}

bool InputCppRoot::test_script() {
  TFile* f = new TFile("TestFile.root", "RECREATE");
  TTree* t = new TTree("Data", "Spills");
  TestSpill* spill = new TestSpill();
  t->Branch("spill", spill, sizeof(*spill), 1);
  spill->SetMyInt(1);
  t->Fill();
  spill->SetMyInt(2);
  t->Fill();
  spill->SetMyInt(3);
  t->Fill();
  t->Write();
  f->Close();

  TFile* fin = new TFile("TestFile.root", "READ");
  TTree* tin = (TTree*)fin->Get("Data");
  TestSpill* spillin = new TestSpill();
  std::cerr << "New TestSpill " << spillin->GetMyInt() << std::endl;
  tin->SetBranchAddress("spill", &spillin);
  std::cerr << tin->GetEntry(0);
  std::cerr << "GetEntry TestSpill " << spillin << " " << spillin->GetMyInt() << std::endl;
  std::cerr << tin->GetEntry(1);
  std::cerr << "GetEntry TestSpill " << spillin << " "  << spillin->GetMyInt() << std::endl;
  std::cerr << tin->GetEntry(2);
  std::cerr << "GetEntry TestSpill " << spillin << " "  << spillin->GetMyInt() << std::endl;
  std::cerr << sizeof(*tin) << std::endl;
  fin->Close();
  return false;
}

bool InputCppRoot::birth(std::string json_datacards) {
  Json::Value json_dc = JsonWrapper::StringToJson(json_datacards);
  if (_filename == "") {
    _filename = JsonWrapper::GetProperty(json_dc,
                   "root_input_filename", JsonWrapper::stringValue).asString();
  }
  _infile = new irstream(_filename.c_str());

  _spill = new TestSpill();

  (*_infile) >> branchName("spill") >> _spill;

  _jsonCppConverter = new JsonCppConverter();
  return true;
}

bool InputCppRoot::death() {
  // if _infile != NULL, _infile will delete spill
  if (_infile == NULL && _spill != NULL) {  
    delete _spill;
    _spill = NULL;
  }

  if (_infile != NULL) {
    delete _infile;
    _infile = NULL;
  }

  if (_jsonCppConverter != NULL) {
    delete _jsonCppConverter;
    _jsonCppConverter = NULL;
  }
  return true;
}

std::string InputCppRoot::getNextEvent() {
  if (_jsonCppConverter == NULL || _infile == NULL) {
    throw(Squeal(
      Squeal::recoverable,
      "InputCppRoot was not initialised properly",
      "InputCppRoot::getNextEvent"
    ) );
  }
  std::cerr << "_spill originally " << _spill << std::endl;
  if ((*_infile) >> readEvent == NULL) {
    return "";
  }
  std::cerr << "_spill conversion " << _spill << std::endl;
  std::cerr << "spill int " << _spill->GetMyInt() << std::endl;
  Json::Value* value = (*_jsonCppConverter)(*_spill);
  Json::FastWriter writer;
  return writer.write(*value);
}

}

