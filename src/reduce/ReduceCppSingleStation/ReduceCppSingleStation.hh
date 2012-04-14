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
#ifndef _ReduceCppSingleStation_H
#define _ReduceCppSingleStation_H

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
#include "TH2F.h"

#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"

class ReduceCppSingleStation {

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

  void unpacked_data_histograms(Json::Value root);

  void digits_histograms(Json::Value root);

  void draw_spacepoints(Json::Value root);

  bool is_physics_daq_event(Json::Value root);

  void count_particle_events(Json::Value root);

 private:
  TGraph *_graph;
  int _spill_counter;

  TH2F *triplets;

  TH2F *duplets;

  TH2F *triplets_copy;

  TH2F *duplets_copy;

  //TH2F *_trig_efficiency;

  std::string _classname;

  std::string _filename;

  int _nSpills;

  void Save();

  TTree _unpacked;
  int _adc, _tdc, _bank, _chan, _activebank;

  TTree _digits;
  int _plane;
  double _npe, _channel;

  TTree _spacepoints;
  TTree _spacepointscopy;
  double _x, _y, _z, _pe;
  int _type;
};

#endif

