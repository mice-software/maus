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
#ifndef _REDUCECPPTRACKER_H
#define _REDUCECPPTRACKER_H

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

class ReduceCppTracker {

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

  void show_light_yield(Json::Value const &root);

  void show_efficiency(Json::Value const &root);

 private:
  std::string _classname;

  int _nSpills;

  TTree _digits;
  double _npe;
  int _tracker_dig;

  TTree _spacepoints;
  int _tracker;
  int _station;
  int _type;
  double _x;
  double _y;
  double _z;

  TTree _events;
  int _tracker_event;
  int _station_hits;
  double _mx;
  double _my;
  double _x0;
  double _y0;

  // int _number_spacepoints;

  double tracker2;
};

#endif

