#!/usr/bin/env python
# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#
"""
  This script loads tracker Recon and MC data and compares the two to produce
  plots of the reconstruction resolution and residuals.


  Script Aglorithm :

  - Create Virtual Plane - Tracker Plane lookup
    - Load Recon and MC Event
    - Find trackpoints in each tracker plane
    - Look for the nearest virtual planes in Z
    - Create lookup dictionary
  - Analyse all events
    - Bin Recon histograms
    - Bin Residual histograms
    - Bin residuals in bins of Pt
    - Bin residuals in bins of Pz
  - Calculate resolutions from histograms
  - Save all plots to a single root file (options PDF output)
"""

# pylint: disable = W0311, E1101, W0613, W0621, C0103, C0111, W0702, W0611
# pylint: disable = R0914, R0912, R0915, W0603, W0612, C0302

# Import MAUS Framework (Required!)
import MAUS

# Generic Python imports
import sys
import os
import argparse
import math
from math import sqrt
import array

# Third Party library import statements
import json
import event_loader
import analysis
from analysis import tools
from analysis import covariances
from analysis import hit_types
import ROOT


# Useful Constants and configuration
RECON_STATION = 1
RECON_PLANE = 0
SEED_STATION = 5
SEED_PLANE = 2
EXPECTED_STRAIGHT_TRACKPOINTS = 9
EXPECTED_HELIX_TRACKPOINTS = 12
REQUIRE_DATA = True
P_VALUE_CUT = 0.0
MUON_PID = [13, -13]

RECON_TRACKERS = [0, 1]

REQUIRE_ALL_PLANES = True

P_MIN = 0.0
P_MAX = 1000.0

MAX_GRADIENT = 2.0

PT_MIN = 0.0
PT_MAX = 100.0
PT_BIN = 10
PT_BIN_WIDTH = 10.0

PZ_MIN = 120.0
PZ_MAX = 260.0
PZ_BIN = 14
PZ_BIN_WIDTH = 10.0

ALIGNMENT_TOLERANCE = 0.01
RESOLUTION_BINS = 10
EFFICIENCY_BINS = 10

TRACK_ALGORITHM = 1

ENSEMBLE_SIZE = 2000

#UP_COV_MC = covariances.CovarianceMatrix()
#DOWN_COV_MC = covariances.CovarianceMatrix() 
#UP_COV_RECON = covariances.CovarianceMatrix()
#DOWN_COV_RECON = covariances.CovarianceMatrix() 

UP_COV_MC = []
DOWN_COV_MC = []
UP_COV_RECON = []
DOWN_COV_RECON = []
UP_CORRECTION = covariances.CorrectionMatrix()
DOWN_CORRECTION = covariances.CorrectionMatrix()

VIRTUAL_PLANE_DICT = None
INVERSE_PLANE_DICT = {}
TRACKER_PLANE_RADIUS = 150.0

SELECT_EVENTS = False
GOOD_EVENTS = None

 
def get_pz_bin(pz) :
  offset = pz - PZ_MIN
  return int(offset/PZ_BIN_WIDTH)


def init_plots_data() :
  """
    Initialised all the plots in a dictionary to pass around to the other 
    functions.
  """
  global UP_COV_MC
  global DOWN_COV_MC 
  global UP_COV_RECON
  global DOWN_COV_RECON
  global PZ_BIN
  global PT_BIN

  PZ_BIN = int(((PZ_MAX-PZ_MIN) / PZ_BIN_WIDTH) + 0.5)
  PT_BIN = int(((PT_MAX-PT_MIN) / PT_BIN_WIDTH) + 0.5)
  UP_COV_MC = [ covariances.CovarianceMatrix() for _ in range(PZ_BIN) ]
  DOWN_COV_MC = [ covariances.CovarianceMatrix() for _ in range(PZ_BIN) ]
  UP_COV_RECON = [ covariances.CovarianceMatrix() for _ in range(PZ_BIN) ]
  DOWN_COV_RECON = [ covariances.CovarianceMatrix() for _ in range(PZ_BIN) ]

  plot_dict = {'upstream' : {}, 'downstream' : {}, \
                                           'missing_tracks' : {}, 'pulls' : {}}
  for tracker in [ 'upstream', 'downstream' ] :
    tracker_dict = {}

    tracker_dict['ntp'] = ROOT.TH1F(tracker+'_ntp', \
                                    'No. TrackPoints', 15, 0.5, 15.5 )


    tracker_dict['xy'] = ROOT.TH2F( tracker+'_xy', \
                  'Position', 500, -200.0, 200.0, 500, -200.0, 200.0 )

    tracker_dict['pxpy'] = ROOT.TH2F(tracker+'_pxpy', \
                  'Momentum', 500, -200.0, 200.0, 500, -200.0, 200.0 )

    tracker_dict['pt'] = ROOT.TH1F( tracker+'_pt', \
                              'Transvere Momentum', 500, -0.0, 200.0 )

    tracker_dict['pz'] = ROOT.TH1F( tracker+'_pz', \
                          'Longitudinal Momentum', 500, 100.0, 300.0 )



    tracker_dict['mc_xy'] = ROOT.TH2F( tracker+'_mc_xy', \
               'MC Position', 500, -200.0, 200.0, 500, -200.0, 200.0 )

    tracker_dict['mc_pxpy'] = ROOT.TH2F( tracker+'_mc_pxpy', \
               'MC Momentum', 500, -200.0, 200.0, 500, -200.0, 200.0 )

    tracker_dict['mc_pt'] = ROOT.TH1F( tracker+'_mc_pt', \
                           'MC Transvere Momentum', 500, -0.0, 200.0 )

    tracker_dict['mc_pz'] = ROOT.TH1F( tracker+'_mc_pz', \
                       'MC Longitudinal Momentum', 500, 100.0, 300.0 )



    tracker_dict['residual_xy'] = ROOT.TH2F( tracker+'_residual_xy', \
             'Residual Position', 800, -20.0, 20.0, 800, -20.0, 20.0 )

    tracker_dict['residual_mxmy'] = ROOT.TH2F( tracker+'_residual_mxmy', \
             'Residual Gradient', 500, -0.5, 0.5, 500, -0.5, 0.5 )

    tracker_dict['residual_pxpy'] = ROOT.TH2F( tracker+'_residual_pxpy', \
             'Residual Momentum', 500, -50.0, 50.0, 500, -50.0, 50.0 )

    tracker_dict['residual_pt'] = ROOT.TH1F( tracker+'_residual_pt', \
                                 "p_{t} Residuals", 500, -50.0, 50.0 )
    tracker_dict['residual_pz'] = ROOT.TH1F( tracker+'_residual_pz', \
                                 "p_{z} Residuals", 500, -50.0, 50.0 )



    tracker_dict['ntp_pt'] = ROOT.TH2F( \
                      tracker+'_ntp_pt', "No. Trackpoints in P_{t}", \
                                        PT_BIN, PT_MIN, PT_MAX, 15, 0.5, 15.5 )
    tracker_dict['ntp_mc_pt'] = ROOT.TH2F( \
                tracker+'_ntp_mc_pt', "No. MC Trackpoints in P_{t}", \
                                        PT_BIN, PT_MIN, PT_MAX, 15, 0.5, 15.5 )

    tracker_dict['ntp_pz'] = ROOT.TH2F( \
                      tracker+'_ntp_pz', "No. Trackpoints in P_{z}", \
                                        PZ_BIN, PZ_MIN, PZ_MAX, 15, 0.5, 15.5 )
    tracker_dict['ntp_mc_pz'] = ROOT.TH2F( \
                tracker+'_ntp_mc_pz', "No. MC Trackpoints in P_{z}", \
                                        PZ_BIN, PZ_MIN, PZ_MAX, 15, 0.5, 15.5 )


    tracker_dict['trackpoint_efficiency'] = ROOT.TEfficiency( \
                                         tracker+'_trackpoint_efficiency', \
                             "Track Point Efficiency in P_{z} and P_{#perp}", \
                               PZ_BIN, PZ_MIN, PZ_MAX, PT_BIN, PT_MIN, PT_MAX )
    tracker_dict['trackpoint_efficiency_pt'] = ROOT.TEfficiency( \
                                         tracker+'_trackpoint_efficiency_pt', \
                              "Track Point Efficiency in P_{#perp}", \
                                                       PT_BIN, PT_MIN, PT_MAX )
    tracker_dict['trackpoint_efficiency_pz'] = ROOT.TEfficiency( \
                                         tracker+'_trackpoint_efficiency_pz', \
                                    "Track Point Efficiency in P_z", \
                                                       PZ_BIN, PZ_MIN, PZ_MAX )


    tracker_dict['ntracks_pt'] = ROOT.TH1F( \
                   tracker+'_ntracks_pt', "No. Tracks in P_{#perp}", \
                                                       PT_BIN, PT_MIN, PT_MAX )
    tracker_dict['ntracks_mc_pt'] = ROOT.TH1F( \
             tracker+'_ntracks_mc_pt', "No. MC Tracks in P_{#perp}", \
                                                       PT_BIN, PT_MIN, PT_MAX )

    tracker_dict['ntracks_pz'] = ROOT.TH1F( \
                   tracker+'_ntracks_pz', "No. Tracks in P_{z}", \
                                                       PZ_BIN, PZ_MIN, PZ_MAX )
    tracker_dict['ntracks_mc_pz'] = ROOT.TH1F( \
             tracker+'_ntracks_mc_pz', "No. MC Tracks in P_{z}", \
                                                       PZ_BIN, PZ_MIN, PZ_MAX )

    tracker_dict['track_efficiency'] = ROOT.TEfficiency( \
        tracker+'_track_efficiency', "Track Efficiency in P_z and P_{#perp}", \
                               PZ_BIN, PZ_MIN, PZ_MAX, PT_BIN, PT_MIN, PT_MAX )
    tracker_dict['track_efficiency_pt'] = ROOT.TEfficiency( \
    tracker+'_track_efficiency_pt', "Track Efficiency in P_{#perp}", \
                                                       PT_BIN, PT_MIN, PT_MAX )
    tracker_dict['track_efficiency_pz'] = ROOT.TEfficiency( \
          tracker+'_track_efficiency_pz', "Track Efficiency in P_z", \
                                                       PZ_BIN, PZ_MIN, PZ_MAX )




    tracker_dict['x_residual_p'] = ROOT.TH2F( \
               tracker+'_x_residual_p', "X Residuals in p", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -20.0, 20.0 )
    tracker_dict['y_residual_p'] = ROOT.TH2F( \
               tracker+'_y_residual_p', "Y Residuals in p", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -20.0, 20.0 )
    tracker_dict['r_residual_p'] = ROOT.TH2F( \
               tracker+'_r_residual_p', "Radius Residuals in p", \
                                       PZ_BIN, PZ_MIN, PZ_MAX, 500, 0.0, 50.0 )

    tracker_dict['px_residual_p'] = ROOT.TH2F( \
              tracker+'_px_residual_p', "p_{x} Residuals in p", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
    tracker_dict['py_residual_p'] = ROOT.TH2F( \
              tracker+'_py_residual_p', "p_{y} Residuals in p", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )

    tracker_dict['pt_residual_p'] = ROOT.TH2F( \
              tracker+'_p_residual_p', "p_{t} Residuals in p", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
    tracker_dict['pz_residual_p'] = ROOT.TH2F( \
              tracker+'_pz_residual_p', "p_{z} Residuals in p", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )

    tracker_dict['p_residual_p'] = ROOT.TH2F( \
              tracker+'_p_residual_p', "p Residuals in p", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )


    tracker_dict['x_residual_pt'] = ROOT.TH2F( \
               tracker+'_x_residual_pt', "X Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -20.0, 20.0 )
    tracker_dict['y_residual_pt'] = ROOT.TH2F( \
               tracker+'_y_residual_pt', "Y Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -20.0, 20.0 )
    tracker_dict['r_residual_pt'] = ROOT.TH2F( \
               tracker+'_r_residual_pt', "Radius Residuals in p_{t}", \
                                       PT_BIN, PT_MIN, PT_MAX, 500, 0.0, 50.0 )

    tracker_dict['px_residual_pt'] = ROOT.TH2F( \
              tracker+'_px_residual_pt', "p_{x} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )
    tracker_dict['py_residual_pt'] = ROOT.TH2F( \
              tracker+'_py_residual_pt', "p_{y} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )

    tracker_dict['pt_residual_pt'] = ROOT.TH2F( \
              tracker+'_pt_residual_pt', "p_{t} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )
    tracker_dict['pz_residual_pt'] = ROOT.TH2F( \
              tracker+'_pz_residual_pt', "p_{z} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )
    tracker_dict['p_residual_pt'] = ROOT.TH2F( \
              tracker+'_p_residual_pt', "p Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )



    tracker_dict['x_residual_pz'] = ROOT.TH2F( \
               tracker+'_x_residual_pz', "X Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -20.0, 20.0 )
    tracker_dict['y_residual_pz'] = ROOT.TH2F( \
               tracker+'_y_residual_pz', "Y Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -20.0, 20.0 )
    tracker_dict['r_residual_pz'] = ROOT.TH2F( \
               tracker+'_r_residual_pz', "Radius Residuals in p_{z}", \
                                       PZ_BIN, PZ_MIN, PZ_MAX, 500, 0.0, 50.0 )

    tracker_dict['mx_residual_pz'] = ROOT.TH2F( \
               tracker+'_mx_residual_pz', "m_{x} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -0.5, 0.5 )
    tracker_dict['my_residual_pz'] = ROOT.TH2F( \
               tracker+'_my_residual_pz', "m_{y} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -0.5, 0.5 )

    tracker_dict['px_residual_pz'] = ROOT.TH2F( \
              tracker+'_px_residual_pz', "p_{x} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
    tracker_dict['py_residual_pz'] = ROOT.TH2F( \
              tracker+'_py_residual_pz', "p_{y} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )

    tracker_dict['pt_residual_pz'] = ROOT.TH2F( \
              tracker+'_pt_residual_pz', "p_{t} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
    tracker_dict['pz_residual_pz'] = ROOT.TH2F( \
              tracker+'_pz_residual_pz', "p_{z} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )

    tracker_dict['p_residual_pz'] = ROOT.TH2F( \
              tracker+'_p_residual_pz', "p Residuals in pz", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )




    tracker_dict['mc_alpha'] = ROOT.TH2F( tracker+'_mc_alpha', \
               "MC Alpha Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                               200, -2.0, 2.0 )
    tracker_dict['mc_beta'] = ROOT.TH2F( tracker+'_mc_beta', \
                "MC Beta Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                            1000, 0.0, 2500.0 )
    tracker_dict['mc_emittance'] = ROOT.TH2F( tracker+'_mc_emittance', \
           "MC Emittance Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                               500, 0.0, 20.0 )
    tracker_dict['mc_momentum'] = ROOT.TH2F( \
                           tracker+'_mc_momentum', "MC Momentum Pz", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 200, -10.0, 10.0 )

    tracker_dict['recon_alpha'] = ROOT.TH2F( tracker+'_recon_alpha', \
                  "Alpha Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                               200, -2.0, 2.0 )
    tracker_dict['recon_beta'] = ROOT.TH2F( tracker+'_recon_beta', \
                   "Beta Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                            1000, 0.0, 2500.0 )
    tracker_dict['recon_emittance'] = ROOT.TH2F( \
          tracker+'_recon_emittance', "Emittance Reconstruction Pz", \
                                       PZ_BIN, PZ_MIN, PZ_MAX, 500, 0.0, 20.0 )
    tracker_dict['recon_momentum'] = ROOT.TH2F( \
                     tracker+'_recon_momentum', "Recon Momentum Pz", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 200, -10.0, 10.0 )

    tracker_dict['residual_alpha'] = ROOT.TH2F( \
             tracker+'_residual_alpha', "Alpha Residual Pz", PZ_BIN, \
                                               PZ_MIN, PZ_MAX, 200, -1.0, 1.0 )
    tracker_dict['residual_beta'] = ROOT.TH2F( \
                       tracker+'_residual_beta', "Beta Residual Pz", \
                                   PZ_BIN, PZ_MIN, PZ_MAX, 200, -100.0, 100.0 )
    tracker_dict['residual_emittance'] = ROOT.TH2F( \
             tracker+'_residual_emittance', "Emittance Residual Pz", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 200, -10.0, 10.0 )
    tracker_dict['residual_momentum'] = ROOT.TH2F( \
             tracker+'_residual_momentum', "Momentum Residual Pz", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 200, -10.0, 10.0 )


    for component in ['x', 'y', 'px', 'py', 'pt'] :
      tracker_dict['seed_'+component+'_residual'] = \
                    ROOT.TH1F( tracker+'_patrec_seed_'+component+'_residual', \
                                   "Residual: "+component, 201, -10.05, 10.05 )

    tracker_dict['seed_mx_residual'] = ROOT.TH1F( \
        tracker+'_patrec_seed_mx_residual', "Residual: m_{x}", 501, -0.5, 0.5 )
    tracker_dict['seed_my_residual'] = ROOT.TH1F( \
        tracker+'_patrec_seed_my_residual', "Residual: m_{y}", 501, -0.5, 0.5 )

    tracker_dict['seed_pz_residual'] = ROOT.TH1F( \
         tracker+'_patrec_seed_pz_residual', "Residual: pz", 501, -50.1, 50.1 )
    tracker_dict['seed_p_residual'] = ROOT.TH1F( \
           tracker+'_patrec_seed_p_residual', "Residual: p", 501, -50.1, 50.1 )

    tracker_dict['seed_pz_residual_pz'] = ROOT.TH2F( \
                     tracker+'_patrec_seed_pz-pz', "True p_{z} - Seed p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 200, -50.0, 50.0 )
    tracker_dict['seed_pt_residual_pt'] = ROOT.TH2F( \
             tracker+'_patrec_seed_pt-pt', "True p_{#perp} - Seed p_{#perp}", \
                                     PT_BIN, PT_MIN, PT_MAX, 200, -50.0, 50.0 )
    tracker_dict['seed_pz_residual_pt'] = ROOT.TH2F( \
                 tracker+'_patrec_seed_pz-pt', "True p_{z} - Seed p_{#perp}", \
                                     PT_BIN, PT_MIN, PT_MAX, 200, -50.0, 50.0 )
    tracker_dict['seed_pt_residual_pz'] = ROOT.TH2F( \
                 tracker+'_patrec_seed_pt-pz', "True p_{#perp} - Seed p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 200, -50.0, 50.0 )
    tracker_dict['seed_p_residual_p'] = ROOT.TH2F( \
                               tracker+'_patrec_seed_p-p', "True p - Seed p", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 200, -50.0, 50.0 )


    plot_dict[tracker] = tracker_dict


  missing_tracks = {}
  for tracker in [ 'upstream', 'downstream' ] :
    missing_tracker = {}

    missing_tracker['x_y'] = ROOT.TH2F(tracker+'_x_y_missing', \
        "Missing Tracks x:y", 400, -200.0, 200.0, 400, -200.0, 200.0 )
    missing_tracker['px_py'] = ROOT.TH2F(tracker+'_px_py_missing', \
        "Missing Tracks p_{x}:p_{y}", 400, -200.0, 200.0, 400, -200.0, 200.0 )
    missing_tracker['x_px'] = ROOT.TH2F(tracker+'_x_px_missing', \
        "Missing Tracks x:p_{x}", 400, -200.0, 200.0, 400, -200.0, 200.0 )
    missing_tracker['y_py'] = ROOT.TH2F(tracker+'_y_py_missing', \
        "Missing Tracks y:p_{y}", 400, -200.0, 200.0, 400, -200.0, 200.0 )
    missing_tracker['x_py'] = ROOT.TH2F(tracker+'_x_py_missing', \
        "Missing Tracks x:p_{y}", 400, -200.0, 200.0, 400, -200.0, 200.0 )
    missing_tracker['y_px'] = ROOT.TH2F(tracker+'_y_px_missing', \
        "Missing Tracks y:p_{x}", 400, -200.0, 200.0, 400, -200.0, 200.0 )
    missing_tracker['pt'] = ROOT.TH1F(tracker+'_pt_missing', \
                           "Missing Tracks pt", PT_BIN, PT_MIN, PT_MAX )
    missing_tracker['pz'] = ROOT.TH1F(tracker+'_pz_missing', \
                           "Missing Tracks pz", PZ_BIN, PZ_MIN, PZ_MAX )
    missing_tracker['pz_pt'] = ROOT.TH2F(tracker+'_pz_pt_missing', \
    "Missing Tracks pz", PZ_BIN, PZ_MIN, PZ_MAX, PT_BIN, PT_MIN, PT_MAX )

    missing_tracks[tracker] = missing_tracker

  plot_dict['missing_tracks'] = missing_tracks


  for pl_id in range( -15, 0 ) + range( 1, 16 ) :
    pull_plot_name = 'kalman_pulls_{0:02d}'.format(pl_id)
    plot_dict['pulls'][pull_plot_name] = ROOT.TH1F( \
                             pull_plot_name, "Kalman Pulls", 101, -5.05, 5.05 )





  data_dict = { 'counters' : {'upstream' : {}, 'downstream' : {} }, \
                                                                  'data' : {} }
  data_dict['counters']['number_events'] = 0

  for tracker in ['upstream', 'downstream'] :
    data_dict['counters'][tracker]['number_virtual'] = 0
    data_dict['counters'][tracker]['missing_virtuals'] = 0

    data_dict['counters'][tracker]['number_tracks'] = 0
    data_dict['counters'][tracker]['number_candidates'] = 0
    data_dict['counters'][tracker]['found_tracks'] = 0
    data_dict['counters'][tracker]['wrong_track_type'] = 0
    data_dict['counters'][tracker]['p_value_cut'] = 0
    data_dict['counters'][tracker]['superfluous_track_events'] = 0
    data_dict['counters'][tracker]['missing_tracks'] = 0
    data_dict['counters'][tracker]['missing_reference_hits'] = 0

    data_dict['counters'][tracker]['momentum_cut'] = 0
    data_dict['counters'][tracker]['gradient_cut'] = 0

    data_dict['counters'][tracker]['found_pairs'] = 0

  return plot_dict, data_dict


def create_virtual_plane_dict(file_reader) :
  """
    Matches up scifitrackpoints to virtual planes to make a lookup dictionary 
  """
  virtual_plane_dict = {}
  for num in range( -15, 0, 1 ) :
    virtual_plane_dict[ num ] = ( -1, (ALIGNMENT_TOLERANCE * 100.0) )
  for num in range( 1, 16, 1 ) :
    virtual_plane_dict[ num ] = ( -1, (ALIGNMENT_TOLERANCE * 100.0) )

  while file_reader.next_event() :
    scifi_event = file_reader.get_event( 'scifi' )
    mc_event = file_reader.get_event( 'mc' )

    tracks = scifi_event.scifitracks()
    for track in tracks :
      trackpoints = track.scifitrackpoints()
      for trkpt in trackpoints :
        z_pos = trkpt.pos().z()
        plane_id = analysis.tools.calculate_plane_id(\
                               trkpt.tracker(), trkpt.station(), trkpt.plane())

        for vhit_num in xrange(mc_event.GetVirtualHitsSize()) :
          vhit = mc_event.GetAVirtualHit(vhit_num)
          diff = math.fabs(vhit.GetPosition().z() - z_pos)

          if diff < virtual_plane_dict[ plane_id ][1] :
            virtual_plane_dict[ plane_id ] = ( vhit.GetStationId(), diff )

    done = True
    for tracker in RECON_TRACKERS :
      for station in [1, 2, 3, 4, 5] :
        for plane in [0, 1, 2] :
          plane_id = analysis.tools.calculate_plane_id( \
                                                      tracker, station, plane )
          if virtual_plane_dict[plane_id][1] > ALIGNMENT_TOLERANCE :
#            print plane_id, virtual_plane_dict[plane]
            done = False
    if done :
      break
  else :
    if REQUIRE_ALL_PLANES :
      print
      print virtual_plane_dict
      raise ValueError("Could not locate all virtuals planes")

  file_reader.reset()
  return virtual_plane_dict


def inverse_virtual_plane_dict(virtual_plane_dict) :
  """
    Create the inverse lookup.
  """
  inverse_dict = {}
  for num in range( -15, 0, 1 ) :
    inverse_dict[virtual_plane_dict[num][0]] = num
  for num in range( 1, 16, 1 ) :
    inverse_dict[virtual_plane_dict[num][0]] = num

  return inverse_dict


def get_expected_tracks(mc_event, virtual_plane_dict) :
  upstream_planes = [ virtual_plane_dict[i][0] for i in range(-15, 0)]
  downstream_planes = [ virtual_plane_dict[i][0] for i in range(1, 16)]

  upstream_track = None
  downstream_track = None

  upstream_hits = {}
  downstream_hits = {}

  for vhit_num in xrange(mc_event.GetVirtualHitsSize()) :
    vhit = mc_event.GetAVirtualHit(vhit_num)
    if vhit.GetParticleId() not in MUON_PID :
      continue

    station_id = vhit.GetStationId()
    radius = math.sqrt( vhit.GetPosition().x()**2 + vhit.GetPosition().y()**2 )
    if radius > TRACKER_PLANE_RADIUS : 
      continue

    if station_id in upstream_planes :
      plane_id = INVERSE_PLANE_DICT[station_id]
      upstream_hits[plane_id] = vhit
    if station_id in downstream_planes :
      plane_id = INVERSE_PLANE_DICT[station_id]
      downstream_hits[plane_id] = vhit

  if TRACK_ALGORITHM == 1 :
    if len(upstream_hits) > EXPECTED_HELIX_TRACKPOINTS :
      upstream_track = upstream_hits
    if len(downstream_hits) > EXPECTED_HELIX_TRACKPOINTS :
      downstream_track = downstream_hits
  elif TRACK_ALGORITHM == 0 :
    if len(upstream_hits) > EXPECTED_STRAIGHT_TRACKPOINTS :
      upstream_track = upstream_hits
    if len(downstream_hits) > EXPECTED_STRAIGHT_TRACKPOINTS :
      downstream_track = downstream_hits
  else:
    raise ValueError("Unknown track algorithm found!")

  return upstream_track, downstream_track


def get_found_tracks(scifi_event, plot_dict, data_dict) :
  """
    Find all the single tracks that pass the cuts.
  """
  upstream_tracks = []
  downstream_tracks = []

  tracks = scifi_event.scifitracks()
  for track in tracks :
    if track.tracker() == 0 :
      tracker = "upstream"
    else :
      tracker = "downstream"

    data_dict['counters'][tracker]['number_tracks'] += 1

    if track.GetAlgorithmUsed() != TRACK_ALGORITHM :
      data_dict['counters'][tracker]['wrong_track_type'] += 1
      continue

    if track.P_value() < P_VALUE_CUT :
      data_dict['counters'][tracker]['p_value_cut'] += 1
      continue

    data_dict['counters'][tracker]['number_candidates'] += 1


    if track.tracker() == 0 :
      upstream_tracks.append(track)
    if track.tracker() == 1 :
      downstream_tracks.append(track)

  if len(upstream_tracks) > 1 :
    data_dict['counters']['upstream']['superfluous_track_events'] += 1
  if len(downstream_tracks) > 1 :
    data_dict['counters']['downstream']['superfluous_track_events'] += 1

  if len(upstream_tracks) == 1 :
    upstream_track = upstream_tracks[0]
    data_dict['counters']['upstream']['found_tracks'] += 1
  else :
    upstream_track = None

  if len(downstream_tracks) == 1 :
    downstream_track = downstream_tracks[0]
    data_dict['counters']['downstream']['found_tracks'] += 1
  else :
    downstream_track = None

  return upstream_track, downstream_track



def make_scifi_mc_pairs(plot_dict, data_dict, virtual_plane_dict, \
                                                        scif_event, mc_event) :
  """
    Make pairs of SciFiTrackpoints and MC VirtualHits
  """
  paired_hits = []
  paired_seeds = []

  expected_up, expected_down = get_expected_tracks(mc_event, virtual_plane_dict)
  found_up, found_down = get_found_tracks(scifi_event, plot_dict, data_dict)

  downstream_pt = 0.0
  downstream_pz = 0.0

  data_dict['counters']['number_events'] += 1

  for tracker_num, tracker, scifi_track, virtual_track in \
                            [ (0, "upstream", found_up, expected_up), \
                              (1, "downstream", found_down, expected_down) ] :
    if virtual_track is None :
      continue

    ref_plane = tools.calculate_plane_id(tracker_num,
                                                    RECON_STATION, RECON_PLANE)
    seed_plane = tools.calculate_plane_id(tracker_num,
                                                    SEED_STATION, SEED_PLANE)
    virtual_pt = 0.0
    virtual_pz = 0.0
    virtual_hits = 0
    scifi_hits = 0

    seed_virt = None
    reference_virt = None
    reference_scifi = None

    for plane in virtual_track :
      if virtual_track[plane] is not None :
        hit = virtual_track[plane]
        virtual_pt += math.sqrt(hit.GetMomentum().x()**2 + \
                                                      hit.GetMomentum().y()**2)
        virtual_pz += hit.GetMomentum().z()
        virtual_hits += 1
        if plane == ref_plane :
          reference_virt = virtual_track[plane]
        if plane == seed_plane :
          seed_virt = virtual_track[plane]

    virtual_pt /= virtual_hits
    virtual_pz /= virtual_hits
    virtual_p = math.sqrt( virtual_pt**2 + virtual_pz**2 )
    if virtual_p > P_MAX or virtual_p < P_MIN :
      data_dict['counters'][tracker]['momentum_cut'] += 1
      continue
    elif virtual_pt / virtual_p > MAX_GRADIENT :
      data_dict['counters'][tracker]['gradient_cut'] += 1
      continue
    else :
      data_dict['counters'][tracker]['number_virtual'] += 1

    plot_dict[tracker]['ntracks_mc_pt'].Fill( virtual_pt )
    plot_dict[tracker]['ntracks_mc_pz'].Fill( virtual_pz )
    plot_dict[tracker]['ntp_mc_pt'].Fill( virtual_pt, virtual_hits )
    plot_dict[tracker]['ntp_mc_pz'].Fill( virtual_pz,  virtual_hits )

    if scifi_track is None :
      plot_dict[tracker]['track_efficiency'].Fill(False, virtual_pz, virtual_pt)
      plot_dict[tracker]['track_efficiency_pt'].Fill(False, virtual_pt)
      plot_dict[tracker]['track_efficiency_pz'].Fill(False, virtual_pz)
      data_dict['counters'][tracker]['missing_tracks'] += 1
#      for i in range(virtual_hits) :
#        plot_dict[tracker]['trackpoint_efficiency'].Fill(False, virtual_pz,\
#                                                                   virtual_pt)
#        plot_dict[tracker]['trackpoint_efficiency_pt'].Fill(False, virtual_pt)
#        plot_dict[tracker]['trackpoint_efficiency_pz'].Fill(False, virtual_pz)

      if reference_virt is not None :
        plot_dict['missing_tracks'][tracker]['x_y'].Fill( \
            reference_virt.GetPosition().x(), reference_virt.GetPosition().y())
        plot_dict['missing_tracks'][tracker]['px_py'].Fill( \
            reference_virt.GetMomentum().x(), reference_virt.GetMomentum().y())
        plot_dict['missing_tracks'][tracker]['x_px'].Fill( \
            reference_virt.GetPosition().x(), reference_virt.GetMomentum().x())
        plot_dict['missing_tracks'][tracker]['y_py'].Fill( \
            reference_virt.GetPosition().y(), reference_virt.GetMomentum().y())
        plot_dict['missing_tracks'][tracker]['x_py'].Fill( \
            reference_virt.GetPosition().x(), reference_virt.GetMomentum().y())
        plot_dict['missing_tracks'][tracker]['y_px'].Fill( \
            reference_virt.GetPosition().y(), reference_virt.GetMomentum().x())
        plot_dict['missing_tracks'][tracker]['pz'].Fill( virtual_pz )
        plot_dict['missing_tracks'][tracker]['pt'].Fill( virtual_pt )
        plot_dict['missing_tracks'][tracker]['pz_pt'].Fill( \
                                                       virtual_pz, virtual_pt )

      continue # Can't do anything else without a scifi track


    for scifi_hit in scifi_track.scifitrackpoints() :
      if scifi_hit.has_data() :
        scifi_hits += 1

        pl_id = analysis.tools.calculate_plane_id(scifi_hit.tracker(), \
                                        scifi_hit.station(), scifi_hit.plane())
        plot_name = 'kalman_pulls_{0:02d}'.format(pl_id)
        plot_dict['pulls'][plot_name].Fill( scifi_hit.pull() )

        if scifi_hit.station() == RECON_STATION and \
                                             scifi_hit.plane() == RECON_PLANE :
          reference_scifi = scifi_hit


    plot_dict[tracker]['track_efficiency'].Fill(True, virtual_pz, virtual_pt)
    plot_dict[tracker]['track_efficiency_pt'].Fill(True, virtual_pt)
    plot_dict[tracker]['track_efficiency_pz'].Fill(True, virtual_pz)

    plot_dict[tracker]['ntracks_pt'].Fill( virtual_pt )
    plot_dict[tracker]['ntracks_pz'].Fill( virtual_pz )

    plot_dict[tracker]['ntp'].Fill( scifi_hits )
    plot_dict[tracker]['ntp_pt'].Fill( virtual_pt, scifi_hits )
    plot_dict[tracker]['ntp_pz'].Fill( virtual_pz, scifi_hits )

    if scifi_hits >= virtual_hits :
      for i in range(virtual_hits) :
        plot_dict[tracker]['trackpoint_efficiency'].Fill(True, \
                                                        virtual_pz, virtual_pt)
        plot_dict[tracker]['trackpoint_efficiency_pt'].Fill(True, virtual_pt)
        plot_dict[tracker]['trackpoint_efficiency_pz'].Fill(True, virtual_pz)
    else :
      for i in range( virtual_hits - scifi_hits ) :
        plot_dict[tracker]['trackpoint_efficiency'].Fill(False, \
                                                        virtual_pz, virtual_pt)
        plot_dict[tracker]['trackpoint_efficiency_pt'].Fill(False, virtual_pt)
        plot_dict[tracker]['trackpoint_efficiency_pz'].Fill(False, virtual_pz)
      for i in range( scifi_hits ) :
        plot_dict[tracker]['trackpoint_efficiency'].Fill(True, \
                                                        virtual_pz, virtual_pt)
        plot_dict[tracker]['trackpoint_efficiency_pt'].Fill(True, virtual_pt)
        plot_dict[tracker]['trackpoint_efficiency_pz'].Fill(True, virtual_pz)


    if reference_virt is None :
      data_dict['counters'][tracker]['missing_virtuals'] += 1

    if reference_scifi is None :
      data_dict['counters'][tracker]['missing_reference_hits'] += 1

    if reference_virt is not None and reference_scifi is not None :
      paired_hits.append( (reference_scifi, reference_virt) )
      data_dict['counters'][tracker]['found_pairs'] += 1

    if seed_virt is not None and scifi_track is not None :
      paired_seeds.append( (scifi_track, seed_virt))

  return paired_hits, paired_seeds



def fill_plots(plot_dict, data_dict, hit_pairs) :
  """
    Fill Plots with Track and Residual Data
  """
  for scifi_hit, virt_hit in hit_pairs :
    tracker_num = scifi_hit.tracker()
    pz_bin = get_pz_bin( virt_hit.GetMomentum().z() )
    if pz_bin >= PZ_BIN or pz_bin < 0 :
      continue

    mc_cov = None
    recon_cov = None
    correction_matrix = None
    if tracker_num == 0 :
      tracker = 'upstream'
      mc_cov = UP_COV_MC[pz_bin]
      recon_cov = UP_COV_RECON[pz_bin]
      correction_matrix = UP_CORRECTION
    else :
      tracker = 'downstream'
      mc_cov = DOWN_COV_MC[pz_bin]
      recon_cov = DOWN_COV_RECON[pz_bin]
      correction_matrix = DOWN_CORRECTION

    tracker_plots = plot_dict[tracker]
    mc_cov.add_hit(hit_types.AnalysisHit(virtual_track_point=virt_hit))
    recon_cov.add_hit(hit_types.AnalysisHit(scifi_track_point=scifi_hit))
    correction_matrix.add_hit(\
                          hit_types.AnalysisHit(scifi_track_point=scifi_hit), \
                          hit_types.AnalysisHit(virtual_track_point=virt_hit))

    scifi_pos = [scifi_hit.pos().x(), scifi_hit.pos().y(), scifi_hit.pos().z()]
    scifi_mom = [scifi_hit.mom().x(), scifi_hit.mom().y(), scifi_hit.mom().z()]
    virt_pos = [virt_hit.GetPosition().x(), \
                        virt_hit.GetPosition().y(), virt_hit.GetPosition().z()]
    virt_mom = [virt_hit.GetMomentum().x(), \
                        virt_hit.GetMomentum().y(), virt_hit.GetMomentum().z()]

    res_pos = [ scifi_pos[0] - virt_pos[0], \
                scifi_pos[1] - virt_pos[1], \
                scifi_pos[2] - virt_pos[2] ]
    res_mom = [ scifi_mom[0] - virt_mom[0], \
                scifi_mom[1] - virt_mom[1], \
                scifi_mom[2] - virt_mom[2] ]
    res_gra = [ scifi_mom[0]/scifi_mom[2] - virt_mom[0]/virt_mom[2], \
                scifi_mom[1]/scifi_mom[2] - virt_mom[1]/virt_mom[2] ]

    Pt_mc = math.sqrt( virt_mom[0] ** 2 + virt_mom[1] ** 2 )
    Pz_mc = virt_mom[2]
    P_mc = math.sqrt(Pz_mc**2 +Pt_mc**2)

    Pt_recon = math.sqrt( scifi_mom[0] ** 2 + scifi_mom[1] ** 2 )
    P_recon = math.sqrt(Pt_recon**2 + scifi_mom[2]**2)

    Pt_res = Pt_recon - Pt_mc
    P_res = P_recon - P_mc

    tracker_plots['xy'].Fill(scifi_pos[0], scifi_pos[1])
    tracker_plots['pxpy'].Fill(scifi_mom[0], scifi_mom[1])
    tracker_plots['pt'].Fill(Pt_recon)
    tracker_plots['pz'].Fill(scifi_mom[2])

    tracker_plots['mc_xy'].Fill(virt_pos[0], virt_pos[1])
    tracker_plots['mc_pxpy'].Fill(virt_mom[0], virt_mom[1])
    tracker_plots['mc_pt'].Fill(Pt_mc)
    tracker_plots['mc_pz'].Fill(Pz_mc)

    tracker_plots['residual_xy'].Fill(res_pos[0], res_pos[1])
    tracker_plots['residual_pxpy'].Fill(res_mom[0], res_mom[1])
    tracker_plots['residual_mxmy'].Fill(res_gra[0], res_gra[1])
    tracker_plots['residual_pt'].Fill(Pt_res)
    tracker_plots['residual_pz'].Fill(res_mom[2])


    tracker_plots['x_residual_pt'].Fill( Pt_mc, res_pos[0] )
    tracker_plots['y_residual_pt'].Fill( Pt_mc, res_pos[1] )
    tracker_plots['r_residual_pt'].Fill( Pt_mc, \
                                          sqrt(res_pos[1]**2 + res_pos[2]**2) )
    tracker_plots['px_residual_pt'].Fill( Pt_mc, res_mom[0] )
    tracker_plots['py_residual_pt'].Fill( Pt_mc, res_mom[1] )
    tracker_plots['pt_residual_pt'].Fill( Pt_mc, Pt_res )
    tracker_plots['pz_residual_pt'].Fill( Pt_mc, res_mom[2] )
    tracker_plots['p_residual_pt'].Fill( Pt_mc, P_res )

    tracker_plots['x_residual_p'].Fill( P_mc, res_pos[0] )
    tracker_plots['y_residual_p'].Fill( P_mc, res_pos[1] )
    tracker_plots['r_residual_p'].Fill( P_mc, \
                                          sqrt(res_pos[1]**2 + res_pos[2]**2) )
    tracker_plots['px_residual_p'].Fill( P_mc, res_mom[0] )
    tracker_plots['py_residual_p'].Fill( P_mc, res_mom[1] )
    tracker_plots['pt_residual_p'].Fill( P_mc, Pt_res )
    tracker_plots['pz_residual_p'].Fill( P_mc, res_mom[2] )
    tracker_plots['p_residual_p'].Fill( P_mc, P_res )


    tracker_plots['x_residual_pz'].Fill( Pz_mc, res_pos[0] )
    tracker_plots['y_residual_pz'].Fill( Pz_mc, res_pos[1] )
    tracker_plots['r_residual_pz'].Fill( Pz_mc, \
                                          sqrt(res_pos[1]**2 + res_pos[2]**2) )
    tracker_plots['mx_residual_pz'].Fill( Pz_mc, res_gra[0] )
    tracker_plots['my_residual_pz'].Fill( Pz_mc, res_gra[1] )
    tracker_plots['px_residual_pz'].Fill( Pz_mc, res_mom[0] )
    tracker_plots['py_residual_pz'].Fill( Pz_mc, res_mom[1] )
    tracker_plots['pt_residual_pz'].Fill( Pz_mc, Pt_res )
    tracker_plots['pz_residual_pz'].Fill( Pz_mc, res_mom[2] )
    tracker_plots['p_residual_pz'].Fill( Pz_mc, P_res )

    if mc_cov.length() == ENSEMBLE_SIZE :
      pz = mc_cov.get_mean('pz')
      tracker_plots['mc_alpha'].Fill(pz, mc_cov.get_alpha(['x','y']))
      tracker_plots['mc_beta'].Fill(pz, mc_cov.get_beta(['x','y']))
      tracker_plots['mc_emittance'].Fill(pz, mc_cov.get_emittance(\
                                                 ['x','px','y','py']))
      tracker_plots['mc_momentum'].Fill(pz, mc_cov.get_momentum())

      tracker_plots['recon_alpha'].Fill(pz, recon_cov.get_alpha(\
                                                           ['x','y']))
      tracker_plots['recon_beta'].Fill(pz, recon_cov.get_beta(\
                                                           ['x','y']))
      tracker_plots['recon_emittance'].Fill(pz, \
                      recon_cov.get_emittance(['x','px','y','py']))
      tracker_plots['recon_momentum'].Fill(pz, \
                                                   recon_cov.get_momentum())

      tracker_plots['residual_alpha'].Fill(pz, \
            recon_cov.get_alpha(['x','y']) - mc_cov.get_alpha(['x','y']))
      tracker_plots['residual_beta'].Fill(pz, \
              recon_cov.get_beta(['x','y']) - mc_cov.get_beta(['x','y']))
      tracker_plots['residual_emittance'].Fill(pz, \
                            recon_cov.get_emittance(['x','px','y','py']) - \
                                  mc_cov.get_emittance(['x','px','y','py']))
      tracker_plots['residual_momentum'].Fill(pz, \
                        recon_cov.get_momentum() - mc_cov.get_momentum())

      mc_cov.clear()
      recon_cov.clear()




def fill_plots_seeds(plot_dict, data_dict, hit_pairs) :
  """
    Fill Plots with Track and Residual Data
  """
  for scifi_track, virt_hit in hit_pairs :
    tracker_num = scifi_track.tracker()
    pz_bin = get_pz_bin( virt_hit.GetMomentum().z() )
    if pz_bin >= PZ_BIN or pz_bin < 0 :
      continue

    if tracker_num == 0 :
      tracker = 'upstream'
    else :
      tracker = 'downstream'

    tracker_plots = plot_dict[tracker]

    scifi_pos = [scifi_track.GetSeedPosition().x(), \
          scifi_track.GetSeedPosition().y(), scifi_track.GetSeedPosition().z()]
    scifi_mom = [scifi_track.GetSeedMomentum().x(), \
          scifi_track.GetSeedMomentum().y(), scifi_track.GetSeedMomentum().z()]
    virt_pos = [virt_hit.GetPosition().x(), \
                        virt_hit.GetPosition().y(), virt_hit.GetPosition().z()]
    virt_mom = [virt_hit.GetMomentum().x(), \
                        virt_hit.GetMomentum().y(), virt_hit.GetMomentum().z()]

    res_pos = [ scifi_pos[0] - virt_pos[0], \
                scifi_pos[1] - virt_pos[1], \
                scifi_pos[2] - virt_pos[2] ]
    res_mom = [ scifi_mom[0] - virt_mom[0], \
                scifi_mom[1] - virt_mom[1], \
                scifi_mom[2] - virt_mom[2] ]
    res_gra = [ scifi_mom[0]/scifi_mom[2] - virt_mom[0]/virt_mom[2], \
                scifi_mom[1]/scifi_mom[2] - virt_mom[1]/virt_mom[2] ]

    Pt_mc = math.sqrt( virt_mom[0] ** 2 + virt_mom[1] ** 2 )
    P_mc = math.sqrt( virt_mom[0] ** 2 + virt_mom[1] ** 2 + virt_mom[2] ** 2 )
    Pz_mc = virt_mom[2]

    Pt_recon = math.sqrt( scifi_mom[0] ** 2 + scifi_mom[1] ** 2 )
    P_recon = math.sqrt( scifi_mom[0] ** 2 + scifi_mom[1] ** 2 + \
                                                            scifi_mom[2] ** 2 )

    Pt_res = Pt_recon - Pt_mc
    P_res = P_recon - P_mc

    tracker_plots['seed_x_residual'].Fill(res_pos[0])
    tracker_plots['seed_y_residual'].Fill(res_pos[1])
    tracker_plots['seed_px_residual'].Fill(res_mom[0])
    tracker_plots['seed_py_residual'].Fill(res_mom[1])
    tracker_plots['seed_pz_residual'].Fill(res_mom[2])
    tracker_plots['seed_mx_residual'].Fill(res_gra[0])
    tracker_plots['seed_my_residual'].Fill(res_gra[1])
    tracker_plots['seed_pt_residual'].Fill(Pt_res)
    tracker_plots['seed_p_residual'].Fill(P_res)

    tracker_plots['seed_pz_residual_pz'].Fill(Pz_mc, res_mom[2])
    tracker_plots['seed_pt_residual_pt'].Fill(Pt_mc, Pt_res)
    tracker_plots['seed_pz_residual_pt'].Fill(Pt_mc, res_mom[2])
    tracker_plots['seed_pt_residual_pz'].Fill(Pz_mc, Pt_res)
    tracker_plots['seed_p_residual_p'].Fill(P_mc, P_res)


def analyse_plots(plot_dict, data_dict) :
  """
    Use existing plots to perform some useful analysis
  """
# Print out some simple stats
  print
  print "There were:"
  print "  {0:0.0f} Events".format( data_dict['counters']['number_events'] )
  print "  {0:0.0f} Upstream Tracks".format( \
                           data_dict['counters']['upstream']['number_tracks'] )
  print "  {0:0.0f} Downstream Tracks".format( \
                         data_dict['counters']['downstream']['number_tracks'] )
  print "  {0:0.0f} Upstream Vitual Tracks".format( \
                          data_dict['counters']['upstream']['number_virtual'] )
  print "  {0:0.0f} Downstream Virtual Tracks".format( \
                          data_dict['counters']['upstream']['number_virtual'] )
  print "  Excluded {0:0.0f} Upstream Tracks outside momentum window".format( \
                            data_dict['counters']['upstream']['momentum_cut'] )
  print "  Excluded {0:0.0f} Downstream Tracks outside momentum window".format(\
                            data_dict['counters']['upstream']['momentum_cut'] )
  print
  print "Found {0:0.0f} Upstream Tracks of the wrong type".format( \
                        data_dict['counters']['upstream']['wrong_track_type'] )
  print "Found {0:0.0f} Downstream Tracks of the wrong type".format( \
                      data_dict['counters']['downstream']['wrong_track_type'] )
  print "Cut {0:0.0f} Upstream Tracks (P-Value Cut)".format( \
                             data_dict['counters']['upstream']['p_value_cut'] )
  print "Cut {0:0.0f} Downstream Tracks (P-Value Cut)".format( \
                           data_dict['counters']['downstream']['p_value_cut'] )
  print
  print "{0:0.0f} Upstream Tracks for analysis".format( \
                       data_dict['counters']['upstream']['number_candidates'] )
  print "{0:0.0f} Downstream Tracks for analysis".format( \
                     data_dict['counters']['downstream']['number_candidates'] )
  print
  print "Missed {0:0.0f} Upstream Virtual Hits".format( \
                        data_dict['counters']['upstream']['missing_virtuals'] )
  print "Missed {0:0.0f} Downstream Virtual Hits".format( \
                      data_dict['counters']['downstream']['missing_virtuals'] )
  print "Missed {0:0.0f} Upstream Reference Plane Hits".format( \
                  data_dict['counters']['upstream']['missing_reference_hits'] )
  print "Missed {0:0.0f} Downstream Reference Plane Hits".format( \
                data_dict['counters']['downstream']['missing_reference_hits'] )
  print "Missed {0:0.0f} Upstream Tracks".format( \
                          data_dict['counters']['upstream']['missing_tracks'] )
  print "Missed {0:0.0f} Downstream Tracks".format( \
                        data_dict['counters']['downstream']['missing_tracks'] )
  print
  print "Matched {0:0.0f} Upstream Tracks".format( \
                            data_dict['counters']['upstream']['found_tracks'] )
  print "Matched {0:0.0f} Downstream Tracks".format( \
                          data_dict['counters']['downstream']['found_tracks'] )

  print
  print "Found {0:0.0f} Upstream Superfluous Track Events".format( \
                data_dict['counters']['upstream']['superfluous_track_events'] )
  print "Found {0:0.0f} Downstream Superfluous Track Events".format( \
              data_dict['counters']['downstream']['superfluous_track_events'] )
  print

# Make the pretty plots

  for tracker in [ "upstream", "downstream" ] :
    for component in [ "x_", "y_", "r_", "px_", "py_", "pt_", "pz_", "p_" ] :
      for plot_axis in [ "residual_pt", "residual_pz", "residual_p" ] :
        plot = plot_dict[tracker][component+plot_axis]

        rms_error = array.array( 'd' )
        bin_size = array.array( 'd' )
        bins = array.array( 'd' )
        rms = array.array( 'd' )
        mean = array.array( 'd' )
        mean_error = array.array( 'd' )

        width = plot.GetXaxis().GetBinWidth(1)
        for i in range( 0, plot.GetXaxis().GetNbins() ) :
          projection = plot.ProjectionY( \
      tracker+component+plot_axis+'_pro_'+str(i), i, (i+1) )

          plot_mean = plot.GetXaxis().GetBinCenter( i ) + width
          pro_mean, pro_mean_err, pro_std, pro_std_err = \
                                        analysis.tools.fit_gaussian(projection)

          bin_size.append( width*0.5 )
          bins.append( plot_mean )
          rms.append( pro_std )
          rms_error.append( pro_std_err )
          mean.append( pro_mean )
          mean_error.append( pro_mean_err )

        if len(bins) != 0 :
          resolution_graph = ROOT.TGraphErrors( len(bins), \
                                               bins, rms, bin_size, rms_error )
          bias_graph = ROOT.TGraphErrors( len(bins), \
                                             bins, mean, bin_size, mean_error )
        else :
          resolution_graph = None
          bias_graph = None

        plot_dict[tracker][component+plot_axis+'_resolution'] = \
                                                               resolution_graph
        plot_dict[tracker][component+plot_axis+'_bias'] = bias_graph



  for tracker in [ "upstream", "downstream" ] :
#    for component in [ "pt_", "pz_", ] :
#      for plot_axis in [ "residual_pt", "residual_pz" ] :
    for plot_name in [ "pt_residual_pt", "pt_residual_pz", "pz_residual_pt", \
                                           "pz_residual_pz", "p_residual_p" ] :
      plot = plot_dict[tracker]['seed_'+plot_name]

      rms_error = array.array( 'd' )
      bin_size = array.array( 'd' )
      bins = array.array( 'd' )
      rms = array.array( 'd' )
      mean = array.array( 'd' )
      mean_error = array.array( 'd' )

      width = plot.GetXaxis().GetBinWidth(1)
      for i in range( 0, plot.GetXaxis().GetNbins() ) :
        projection = plot.ProjectionY( \
              tracker+plot_name+'_pro_'+str(i), i, (i+1) )

        plot_mean = plot.GetXaxis().GetBinCenter( i ) + width
        pro_mean, pro_mean_err, pro_std, pro_std_err = \
                                      analysis.tools.fit_gaussian(projection)

        bin_size.append( width*0.5 )
        bins.append( plot_mean )
        rms.append( pro_std )
        rms_error.append( pro_std_err )
        mean.append( pro_mean )
        mean_error.append( pro_mean_err )

      if len(bins) != 0 :
        resolution_graph = ROOT.TGraphErrors( len(bins), \
                                             bins, rms, bin_size, rms_error )
        bias_graph = ROOT.TGraphErrors( len(bins), \
                                           bins, mean, bin_size, mean_error )
      else :
        resolution_graph = None
        bias_graph = None

      plot_dict[tracker]['seed_'+plot_name+'_resolution'] = resolution_graph
      plot_dict[tracker]['seed_'+plot_name+'_bias'] = bias_graph

  return data_dict


if __name__ == "__main__" : 
  ROOT.gROOT.SetBatch( True )
  ROOT.gErrorIgnoreLevel = ROOT.kError

  parser = argparse.ArgumentParser( description='An example script showing '+\
      'some basic data extraction and analysis routines' )

  parser.add_argument( 'maus_root_files', nargs='+', help='List of MAUS '+\
                  'output root files containing reconstructed straight tracks')

  parser.add_argument( '-N', '--max_num_events', type=int, \
                                   help='Maximum number of events to analyse.')

  parser.add_argument( '-O', '--output_filename', \
            default='tracker_resolution_plots', help='Set the output filename')

  parser.add_argument( '-D', '--output_directory', \
                                 default='./', help='Set the output directory')

  parser.add_argument( '-V', '--virtual_plane_dictionary', default=None, \
                   help='Specify a json file containing a dictionary of the '+\
                                                       'virtual plane lookup' )

  parser.add_argument( '-P', '--print_plots', action='store_true', \
                        help="Flag to save the plots as individual pdf files" )

  parser.add_argument( '--cut_number_trackpoints', type=int, default=0, \
          help="Specify the minumum number of trackpoints required per track" )

  parser.add_argument( '--cut_p_value', type=float, default=0.0, \
  help="Specify the P-Value below which tracks are removed from the analysis" )

  parser.add_argument( '--track_algorithm', type=int, default=1, \
                          help="Specify the track reconstruction algorithm. "+\
                             "1 for Helical Tracks and 0 for Straight Tracks" )

  parser.add_argument( '--ensemble_size', type=int, default=2000, \
                        help="Specify the size of the ensemble of particles "+\
                                     "to consider per emittance measurement." )

  parser.add_argument( '--pz_bin', type=float, default=PZ_BIN_WIDTH, \
             help="Specify the size of the Pz bins which are used to select "+\
                     "particles for the reconstruction of optical functions." )
  parser.add_argument( '--pz_window', type=float, nargs=2, \
        default=[PZ_MIN, PZ_MAX], help="Specify the range of Pz to consider "+\
                               "for the reconstruction of optical functions." )

  parser.add_argument( '--pt_bin', type=float, default=PT_BIN_WIDTH, \
             help="Specify the size of the Pt bins which are used to select "+\
                     "particles for the reconstruction of optical functions." )
  parser.add_argument( '--pt_window', type=float, nargs=2, \
        default=[PT_MIN, PT_MAX], help="Specify the range of Pt to consider "+\
                               "for the reconstruction of optical functions." )

  parser.add_argument( '--trackers', type=int, default=RECON_TRACKERS, \
                          nargs='+', help="Specifies the trackers to analyse" )

  parser.add_argument( '--p_window', type=float, nargs=2, \
             default=[P_MIN, P_MAX], help="Specify the range of the total " + \
                                         "momentum to consider for analysis." )

  parser.add_argument( '--max_gradient', type=float, default=MAX_GRADIENT, \
                            help='Specify the maximum gradient to analyse.' + \
                                        ' This eliminates non-physical muons' )

  parser.add_argument( '-C', '--save_corrections', action='store_true', \
                            help="Flag to create the correction matrix files" )


  parser.add_argument( '--selection_file', default=None, \
                 help='Name of a JSON file containing the events to analyses' )



  parser.add_argument( '--not_require_cluster', action="store_true", \
                        help="Don't require a cluster in the reference plane" )

#  parser.add_argument( '-C', '--configuration_file', help='Configuration '+\
#      'file for the reconstruction. I need the geometry information' )

  try :
    namespace = parser.parse_args()

    EXPECTED_HELIX_TRACKPOINTS = namespace.cut_number_trackpoints
    EXPECTED_STRAIGHT_TRACKPOINTS = namespace.cut_number_trackpoints
    P_VALUE_CUT = namespace.cut_p_value
    TRACK_ALGORITHM = namespace.track_algorithm
    ENSEMBLE_SIZE = namespace.ensemble_size
    if namespace.not_require_cluster :
      REQUIRE_DATA = False

    RECON_TRACKERS = namespace.trackers

    P_MIN = namespace.p_window[0]
    P_MAX = namespace.p_window[1]

    MAX_GRADIENT = namespace.max_gradient

    PZ_MIN = namespace.pz_window[0]
    PZ_MAX = namespace.pz_window[1]
    PZ_BIN_WIDTH = namespace.pz_bin

    PT_MIN = namespace.pt_window[0]
    PT_MAX = namespace.pt_window[1]
    PT_BIN_WIDTH = namespace.pt_bin

    if namespace.selection_file is not None :
      SELECT_EVENTS = True
      with open(namespace.selection_file, 'r') as infile :
        GOOD_EVENTS = json.load(infile)
    else :
      SELECT_EVENTS = False

    if namespace.virtual_plane_dictionary is not None :
      VIRTUAL_PLANE_DICT = analysis.tools.load_virtual_plane_dict( \
                                           namespace.virtual_plane_dictionary )
  except BaseException as ex:
    raise
  else :
##### 1. Load MAUS globals and geometry. - NOT NECESSARY AT PRESENT
    # geom = load_tracker_geometry(namespace.configuration_file)

##### 2. Intialise plots ######################################################
    print
    sys.stdout.write( "\n- Initialising Plots : Running\r" )
    sys.stdout.flush()
    plot_dict, data_dict = init_plots_data()
    sys.stdout.write(   "- Initialising Plots : Done   \n" )

    file_reader = event_loader.maus_reader(namespace.maus_root_files)
##### 3. Initialise Plane Dictionary ##########################################
    if VIRTUAL_PLANE_DICT is None :
      sys.stdout.write( "\n- Finding Virtual Planes : Running\r" )
      sys.stdout.flush()
      virtual_plane_dictionary = create_virtual_plane_dict(file_reader)
      VIRTUAL_PLANE_DICT = virtual_plane_dictionary
      sys.stdout.write(   "- Finding Virtual Planes : Done   \n" )

    INVERSE_PLANE_DICT = inverse_virtual_plane_dict(VIRTUAL_PLANE_DICT)
    file_reader.select_events(GOOD_EVENTS)
    file_reader.set_max_num_events(namespace.max_num_events)
    file_reader.set_print_progress('spill')

##### 4. Load Events ##########################################################
    print "\n- Loading Spills...\n"
    try :
      while file_reader.next_selected_event() :

        try :
          scifi_event = file_reader.get_event( 'scifi' )
          mc_event = file_reader.get_event( 'mc' )

##### 5. Extract tracks and Fill Plots ########################################

          paired_hits, seed_pairs = make_scifi_mc_pairs(plot_dict, data_dict, \
                                     VIRTUAL_PLANE_DICT, scifi_event, mc_event)
          fill_plots(plot_dict, data_dict, paired_hits)
          fill_plots_seeds(plot_dict, data_dict, seed_pairs)

        except ValueError as ex :
          print "An Error Occured: " + str(ex)
          print "Skipping Event: " +\
                str(file_reader.get_current_event_number()) + " In Spill: " + \
                str(file_reader.get_current_spill_number()) + " In File: " + \
                str(file_reader.get_current_filenumber()) + "\n"
          continue

    except KeyboardInterrupt :
      print
      print " ###  Keyboard Interrupt  ###"
      print
    print "- {0:0.0f} Spills Loaded                                 ".format( \
                                            file_reader.get_total_num_spills())
##### 6. Analysing Plots ######################################################
    print"\n- Analysing Data...\n"

    analyse_plots(plot_dict, data_dict)

##### 7. Saving Plots and Data ################################################

    sys.stdout.write( "\n- Saving Plots and Data : Running\r" )
    sys.stdout.flush()
#    save_pretty(plot_dict, namespace.output_directory )

#    save_plots(plot_dict, namespace.output_directory, \
#                              namespace.output_filename, namespace.print_plots)
    filename = os.path.join(namespace.output_directory, \
                                                     namespace.output_filename)
    analysis.tools.save_plots(plot_dict, filename+'.root')
    if namespace.save_corrections :
      UP_CORRECTION.save_full_correction(filename+'_up_correction.txt')
      DOWN_CORRECTION.save_full_correction(filename+'_down_correction.txt')

      UP_CORRECTION.save_R_matrix(filename+'_up_correction-R.txt')
      UP_CORRECTION.save_C_matrix(filename+'_up_correction-C.txt')
      DOWN_CORRECTION.save_R_matrix(filename+'_down_correction-R.txt')
      DOWN_CORRECTION.save_C_matrix(filename+'_down_correction-C.txt')
    sys.stdout.write(   "- Saving Plots and Data : Done   \n" )

  print 
  print "Complete."
  print

