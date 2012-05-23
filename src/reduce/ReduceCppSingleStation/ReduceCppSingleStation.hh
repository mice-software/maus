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
#include "TH1F.h"

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

  void doublet_clusters_histograms(Json::Value root);

  void draw_spacepoints(Json::Value root);

  bool is_physics_daq_event(Json::Value root);

  void count_particle_events(Json::Value root);

  void compute_station_efficiencies(Json::Value root);
 private:
  TGraph *_graph;
  TGraph *_station;
  TGraph *_plane0;
  TGraph *_plane1;
  TGraph *_plane2;

  int _spill_counter;

  TH2F *triplets;

  TH2F *duplets;

  TH2F *triplets_copy;

  TH2F *duplets_copy;

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

  std::string _classname;

  std::string _filename;

  int _nSpills;

  void Save();

  TTree _unpacked;
  int _adc, _tdc, _bank, _chan, _activebank;

  TTree _digits;
  int _plane_dig, _adc_dig;
  double _npe_dig, _channel_dig;

  TTree _doublet_clusters;
  int _plane;
  double _npe, _channel;

  TTree _spacepoints;
  TTree _spacepointscopy;
  double _x, _y, _z, _pe;
  int _type;

  // Efficiencies.
  double _plane_array[3];
  double _station_eff[3];

  int _plane_0_counter;
  double _plane_0_map[214][2];
  //double _plane0_eff[214];
  int _plane_0_hits;

  int _plane_1_counter;
  double _plane_1_map[214][2];
  //double _plane1_eff[214];
  int _plane_1_hits;

  int _plane_2_counter;
  double _plane_2_map[214][2];
  //double _plane2_eff[214];
  int _plane_2_hits;

  double _channel_array[214];
};

#endif

