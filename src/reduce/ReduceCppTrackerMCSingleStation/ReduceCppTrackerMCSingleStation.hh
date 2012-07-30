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
#ifndef _REDUCECPPTRACKERMCSINGLESTATION_H
#define _REDUCECPPTRACKERMCSINGLESTATION_H

#include <string>
#include <vector>
#include <map>
#include "json/json.h"
#include "TTree.h"
#include "TH1I.h"
#include "TFile.h"

// Root
#include "TAxis.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TStyle.h"
#include "TPad.h"
#include "TROOT.h"
#include "TColor.h"
#include "TGFrame.h"
#include "TVirtualPad.h"
#include "TFrame.h"

class ReduceCppTrackerMCSingleStation {

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


 private:

  std::string _classname;
  std::string _filename;
  int _nSpills;

  TTree _mctrue;
  TTree _sci_fi;
  TTree _virt;
  TTree _sp;
  TTree _digits;
  TTree _clusters;
  TTree _other;

  double _x;
  double _y;
  double _z;

  double _px;
  double _py;
  double _pz;
  double _pT;

  double _bx;
  double _by;
  double _bz;

  double _energy;
  double _de;

  int _tracker;
  int _station;
  int _plane;
  int _type;
  double _npe;
  int _part_event;

  double cluster_eff1;
  double cluster_eff2;
  double space_eff1;
  double space_eff2;

  void Save();
  bool Trigger(Json::Value path);

  double Tspace_eff;
  double Tcluster_eff;
};

#endif

