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


/** @class 
 *
 */

#ifndef _REDUCECPPTOFCALIB_H
#define _REDUCECPPTOFCALIB_H

#include <string>
#include <vector>
#include <map>
#include "json/json.h"
#include "TTree.h"
#include "TH1I.h"
#include "TFile.h"
#include "TStyle.h"
#include "TROOT.h"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "Utils/TOFChannelMap.hh"

namespace MAUS {
class ReduceCppTofCalib {

 public:

 /** @brief Sets up the worker
  *
  *  @param argJsonConfigDocument a JSON document with
  *         the configuration.
  */
  bool birth(std::string argJsonConfigDocument);

 /** @brief Shutdowns the worker
  *
  *  This takes no arguments and does nothing.
  */
  bool death();

 /** @brief process JSON document
  *
  *  @param document Receive a document with slab hits and return
  *  a document with space points.
  */
  std::string process(std::string document);

  void process(MAUS::Data *data);

  MAUS::Spill get_spill() { return _spill; }

  void setFilePath(std::string p) {_filepath = p;}
  void setFileName(std::string n) {_filename = n;}

  // save the output root tree
  void Save();
  void Save(std::string n);

 private:

  /** @brief Check if this particle event is useful or not. 
   *  @param xPartEvent Json document containing a particle event.
   */

  Json::Value root;
  // initialize the root tree for storing slab hits
  bool MakeTree();

  // load json
  bool loadSpill(std::string jsonDoc);

  void processSpill();

  std::string _classname;
  // output root file name
  std::string _filename;
  std::string _filepath;

  /// Vector to hold the names of all detectors to be processed.
  std::vector<std::string> _stationKeys;

  // to hold the spill document
  Spill _spill;
  // output root tree
  TTree dataTree;
  // tree members
  int slabA;
  int slabB;
  double t0;
  double t1;
  double t2;
  double t3;
  int adc0;
  int adc1;
  int adc2;
  int adc3;

  int slabC;
  int slabD;
  double t4;
  double t5;
  double t6;
  double t7;
  int adc4;
  int adc5;
  int adc6;
  int adc7;

  int slabE;
  int slabF;
  double t8;
  double t9;
  double t10;
  double t11;
  int adc8;
  int adc9;
  int adc10;
  int adc11;
};
} // end namespace
#endif

