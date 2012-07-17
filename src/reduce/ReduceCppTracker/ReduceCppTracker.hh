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
#include "TH2F.h"
#include "TH1F.h"

// #include "src/reduce/ReduceCppTracker/RootEvent.hh"

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

  bool is_physics_daq_event(Json::Value root);

  bool is_good_event();

  void unpacked_data_tree(Json::Value root);

  void draw_spacepoints(Json::Value root);

  void doublet_clusters_tree();

  void digits_tree();

  void spacepoints_tree();
  // void save_light_yield(Json::Value const &root);

  // void save_efficiency(Json::Value const &root);

  void show_efficiency();

  void compute_stations_efficiencies();

  void display_histograms();

  void save();

  void merge_tracker_events(Json::Value &root);

  // void perform_alignment_study();
 private:
  bool _this_is_tracker0_event;

  bool _this_is_tracker1_event;

  Json::Value json_daq, json_digits, json_clusters, json_spacepoints;

  std::string _classname;

  int _nSpills;

  int _map_planes_total[30][214];

  int _map_planes_hit[30][214];

  int map_stations_total[2][6];

  int map_stations_hits[2][6];

  TTree _events;
  int _tracker_event;
  int _station_hits;
  double _mx;
  double _my;
  double _x0;
  double _y0;

  // int _number_spacepoints;

  double tracker2;

///////////////////////////////
  int _spill_counter;

  TH2F *triplets;

  TH2F *duplets;

  TH2F *triplets_tracker2;

  TH2F *duplets_tracker2;

  TH2F *triplets_copy;

  TH2F *duplets_copy;

  TH2F *triplets_tracker2_copy;

  TH2F *duplets_tracker2_copy;

  TH1F *_hist_plane0;

  TH1F *_hist_plane1;

  TH1F *_hist_plane2;

  TH1F *_chan_sum;

  TH1F *_npe_plane0;

  TH1F *_npe_plane1;

  TH1F *_npe_plane2;

  TH1F *_adc_plane0;

  TH1F *_adc_plane1;

  TH1F *_adc_plane2;

  TH1F *_dig_npe_plane0;

  TH1F *_dig_npe_plane1;

  TH1F *_dig_npe_plane2;

  std::string _filename;

  TTree _unpacked;
  int _adc, _tdc, _bank, _chan, _board;

  TTree _digits;
  int _tracker_dig, _station_dig, _plane_dig, _adc_dig;
  double _npe_dig, _channel_dig, _time;

  TTree _doublet_clusters;
  int _tracker, _station, _plane;
  double _channel, _npe;

  TTree _spacepoints;
  TTree _spacepointscopy;
  double _x, _y, _z, _pe;
  int _type, _tracker_sp, _station_sp;

  // Efficiencies.
  double _plane_array[3];
  double _station_eff[3];

  int _plane_0_counter;
  double _plane_0_map[214][2];
  int _plane_0_hits;

  int _plane_1_counter;
  double _plane_1_map[214][2];
  int _plane_1_hits;

  int _plane_2_counter;
  double _plane_2_map[214][2];
  int _plane_2_hits;

  double _channel_array[214];
};

#endif

