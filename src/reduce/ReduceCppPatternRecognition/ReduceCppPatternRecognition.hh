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
#ifndef _REDUCECPPPATTERNRECOGNITION_H
#define _REDUCECPPPATTERNRECOGNITION_H

#include <string>
#include <vector>
#include <map>

#include "json/json.h"

// Root
#include "TTree.h"
#include "TH1I.h"
#include "TFile.h"
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
#include "TF1.h"
#include "TPaveText.h"

class ReduceCppPatternRecognition {

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

  int const get_num_tracks() { return _tracks.GetEntries(); }

 private:

  std::string _classname;
  std::string _filename;

  int _nSpills;
  // int _number_spacepoints;
  static const int _trk_lower_bound = -1300;
  static const int _trk_upper_bound = 0;
  double tracker2;

  TTree _digits;
  double _npe;
  int _tracker_dig;

  TTree _spacepoints;
  TTree _spacepoints_1spill;
  int _tracker;
  int _station;
  int _type;
  double _x;
  double _y;
  double _z;

  TTree _tracks;
  int _tracker_event;
  int _num_points;
  // int _station_hits
  double _mx;
  double _my;
  double _x0;
  double _y0;

  // Vectors to hold the tracks in each projection in each tracker
  std::vector<TF1> _trks_zx_trkr0;
  std::vector<TF1> _trks_zy_trkr0;
  std::vector<TF1> _trks_zx_trkr1;
  std::vector<TF1> _trks_zy_trkr1;

  void draw_histos(TTree * t1, TCanvas * c1);
  void draw_graphs(TTree * t1, TCanvas * c1);
  void draw_tracks(TCanvas * c1);
  TF1 make_track(double c, double m);
  void clear_tracks();

  void Save();
  void update_info(TCanvas * c1, TPaveText *pt);
};

#endif

