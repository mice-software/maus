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
# pylint: disable = R0914, R0912, R0915

# Import MAUS Framework (Required!)
import MAUS

# Generic Python imports
import sys
import os
import argparse
import math
import array

# Third Party library import statements
import event_loader
import analysis
from analysis import tools
from analysis import covariances
from analysis import hit_types
import ROOT


# Useful Constants and configuration
RECON_STATION = 1
RECON_PLANE = 0
MIN_NUMBER_TRACKPOINTS = 0
MUON_PID = 13

PT_MIN = 0.0
PT_MAX = 200.0
PT_BIN = 200

PZ_MIN = 120.0
PZ_MAX = 260.0
PZ_BIN = 100

ALIGNMENT_TOLERANCE = 0.001
RESOLUTION_BINS = 10
EFFICIENCY_BINS = 10

TRACK_ALGORITHM = 1

ENSEMBLE_SIZE = 2000

UP_COV_MC = covariances.CovarianceMatrix()
DOWN_COV_MC = covariances.CovarianceMatrix() 
UP_COV_RECON = covariances.CovarianceMatrix()
DOWN_COV_RECON = covariances.CovarianceMatrix() 


def init_plots_data() :
  """
    Initialised all the plots in a dictionary to pass around to the other 
    functions.
  """
  plot_dict = {}

  plot_dict['upstream_ntp'] = ROOT.TH1F('upstream_ntp', \
                                    'Upstream No. TrackPoints', 15, 0.5, 15.5 )
  plot_dict['downstream_ntp'] = ROOT.TH1F('downstream_ntp', \
                                  'Downstream No. TrackPoints', 15, 0.5, 15.5 )


  plot_dict['upstream_xy'] = ROOT.TH2F( 'upstream_xy', \
                  'Upstream Position', 500, -200.0, 200.0, 500, -200.0, 200.0 )
  plot_dict['downstream_xy'] = ROOT.TH2F( 'downstream_xy', \
                'Downstream Position', 500, -200.0, 200.0, 500, -200.0, 200.0 )

  plot_dict['upstream_pxpy'] = ROOT.TH2F('upstream_pxpy', \
                  'Upstream Momentum', 500, -200.0, 200.0, 500, -200.0, 200.0 )
  plot_dict['downstream_pxpy'] = ROOT.TH2F('downstream_pxpy', \
                'Downstream Momentum', 500, -200.0, 200.0, 500, -200.0, 200.0 )

  plot_dict['upstream_pt'] = ROOT.TH1F( 'upstream_pt', \
                              'Upstream Transvere Momentum', 500, -0.0, 200.0 )
  plot_dict['downstream_pt'] = ROOT.TH1F( 'downstream_pt', \
                            'Downstream Transvere Momentum', 500, -0.0, 200.0 )

  plot_dict['upstream_pz'] = ROOT.TH1F( 'upstream_pz', \
                          'Upstream Longitudinal Momentum', 500, 100.0, 300.0 )
  plot_dict['downstream_pz'] = ROOT.TH1F( 'downstream_pz', \
                        'Downstream Longitudinal Momentum', 500, 100.0, 300.0 )



  plot_dict['upstream_mc_xy'] = ROOT.TH2F( 'upstream_mc_xy', \
               'Upstream MC Position', 500, -200.0, 200.0, 500, -200.0, 200.0 )
  plot_dict['downstream_mc_xy'] = ROOT.TH2F( 'downstream_mc_xy', \
             'Downstream MC Position', 500, -200.0, 200.0, 500, -200.0, 200.0 )

  plot_dict['upstream_mc_pxpy'] = ROOT.TH2F( 'upstream_mc_pxpy', \
               'Upstream MC Momentum', 500, -200.0, 200.0, 500, -200.0, 200.0 )
  plot_dict['downstream_mc_pxpy'] = ROOT.TH2F( 'downstream_mc_pxpy', \
             'Downstream MC Momentum', 500, -200.0, 200.0, 500, -200.0, 200.0 )

  plot_dict['upstream_mc_pt'] = ROOT.TH1F( 'upstream_mc_pt', \
                           'Upstream MC Transvere Momentum', 500, -0.0, 200.0 )
  plot_dict['downstream_mc_pt'] = ROOT.TH1F( 'downstream_mc_pt', \
                         'Downstream MC Transvere Momentum', 500, -0.0, 200.0 )

  plot_dict['upstream_mc_pz'] = ROOT.TH1F( 'upstream_mc_pz', \
                       'Upstream MC Longitudinal Momentum', 500, 100.0, 300.0 )
  plot_dict['downstream_mc_pz'] = ROOT.TH1F( 'downstream_mc_pz', \
                     'Downstream MC Longitudinal Momentum', 500, 100.0, 300.0 )



  plot_dict['upstream_residual_xy'] = ROOT.TH2F( 'upstream_residual_xy', \
             'Upstream Residual Position', 500, -50.0, 50.0, 500, -50.0, 50.0 )
  plot_dict['downstream_residual_xy'] = ROOT.TH2F( 'downstream_residual_xy', \
           'Downstream Residual Position', 500, -50.0, 50.0, 500, -50.0, 50.0 )

  plot_dict['upstream_residual_pxpy'] = ROOT.TH2F( 'upstream_residual_pxpy', \
             'Upstream Residual Momentum', 500, -50.0, 50.0, 500, -50.0, 50.0 )
  plot_dict['downstream_residual_pxpy'] = ROOT.TH2F( \
                  'downstream_residual_pxpy', 'Downstream Residual Momentum', \
                                           500, -50.0, 50.0, 500, -50.0, 50.0 )

  plot_dict['upstream_residual_pt'] = ROOT.TH1F( 'upstream_residual_pt', \
                                 "Upstream p_{t} Residuals", 500, -50.0, 50.0 )
  plot_dict['downstream_residual_pt'] = ROOT.TH1F( 'downstream_residual_pt', \
                               "Downstream p_{t} Residuals", 500, -50.0, 50.0 )
  plot_dict['upstream_residual_pz'] = ROOT.TH1F( 'upstream_residual_pz', \
                                 "Upstream p_{z} Residuals", 500, -50.0, 50.0 )
  plot_dict['downstream_residual_pz'] = ROOT.TH1F( 'downstream_residual_pz', \
                               "Downstream p_{z} Residuals", 500, -50.0, 50.0 )



  plot_dict['upstream_ntp_pt'] = ROOT.TH2F( \
                      'upstream_ntp_pt', "Upstream No. Trackpoints in P_{t}", \
                                        PT_BIN, PT_MIN, PT_MAX, 15, 0.5, 15.5 )
  plot_dict['downstream_ntp_pt'] = ROOT.TH2F( \
                  'downstream_ntp_pt', "Downstream No. Trackpoints in P_{t}", \
                                        PT_BIN, PT_MIN, PT_MAX, 15, 0.5, 15.5 )


  plot_dict['upstream_x_residual_pt'] = ROOT.TH2F( \
               'upstream_x_residual_pt', "Upstream X Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )
  plot_dict['upstream_y_residual_pt'] = ROOT.TH2F( \
               'upstream_y_residual_pt', "Upstream Y Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )

  plot_dict['upstream_px_residual_pt'] = ROOT.TH2F( \
              'upstream_px_residual_pt', "Upstream p_{x} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )
  plot_dict['upstream_py_residual_pt'] = ROOT.TH2F( \
              'upstream_py_residual_pt', "Upstream p_{y} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )

  plot_dict['upstream_pt_residual_pt'] = ROOT.TH2F( \
              'upstream_pt_residual_pt', "Upstream p_{t} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )
  plot_dict['upstream_pz_residual_pt'] = ROOT.TH2F( \
              'upstream_pz_residual_pt', "Upstream p_{z} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )


  plot_dict['downstream_x_residual_pt'] = ROOT.TH2F( \
               'downstream_x_residual_pt', "Downstream X Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )
  plot_dict['downstream_y_residual_pt'] = ROOT.TH2F( \
               'downstream_y_residual_pt', "Downstream Y Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )

  plot_dict['downstream_px_residual_pt'] = ROOT.TH2F( \
          'downstream_px_residual_pt', "Downstream p_{x} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )
  plot_dict['downstream_py_residual_pt'] = ROOT.TH2F( \
          'downstream_py_residual_pt', "Downstream p_{y} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )

  plot_dict['downstream_pt_residual_pt'] = ROOT.TH2F( \
          'downstream_pt_residual_pt', "Downstream p_{t} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )
  plot_dict['downstream_pz_residual_pt'] = ROOT.TH2F( \
          'downstream_pz_residual_pt', "Downstream p_{z} Residuals in p_{t}", \
                                     PT_BIN, PT_MIN, PT_MAX, 500, -50.0, 50.0 )



  plot_dict['upstream_ntp_pz'] = ROOT.TH2F( \
                      'upstream_ntp_pz', "Upstream No. Trackpoints in P_{z}", \
                                        PZ_BIN, PZ_MIN, PZ_MAX, 15, 0.5, 15.5 )
  plot_dict['downstream_ntp_pz'] = ROOT.TH2F( \
                  'downstream_ntp_pz', "Downstream No. Trackpoints in P_{z}", \
                                        PZ_BIN, PZ_MIN, PZ_MAX, 15, 0.5, 15.5 )


  plot_dict['upstream_x_residual_pz'] = ROOT.TH2F( \
               'upstream_x_residual_pz', "Upstream X Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
  plot_dict['upstream_y_residual_pz'] = ROOT.TH2F( \
               'upstream_y_residual_pz', "Upstream Y Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )

  plot_dict['upstream_px_residual_pz'] = ROOT.TH2F( \
              'upstream_px_residual_pz', "Upstream p_{x} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
  plot_dict['upstream_py_residual_pz'] = ROOT.TH2F( \
              'upstream_py_residual_pz', "Upstream p_{y} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )

  plot_dict['upstream_pt_residual_pz'] = ROOT.TH2F( \
              'upstream_pt_residual_pz', "Upstream p_{t} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
  plot_dict['upstream_pz_residual_pz'] = ROOT.TH2F( \
              'upstream_pz_residual_pz', "Upstream p_{z} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )


  plot_dict['downstream_x_residual_pz'] = ROOT.TH2F( \
               'downstream_x_residual_pz', "Downstream X Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
  plot_dict['downstream_y_residual_pz'] = ROOT.TH2F( \
               'downstream_y_residual_pz', "Downstream Y Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )

  plot_dict['downstream_px_residual_pz'] = ROOT.TH2F( \
          'downstream_px_residual_pz', "Downstream p_{x} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
  plot_dict['downstream_py_residual_pz'] = ROOT.TH2F( \
          'downstream_py_residual_pz', "Downstream p_{y} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )

  plot_dict['downstream_pt_residual_pz'] = ROOT.TH2F( \
          'downstream_pt_residual_pz', "Downstream p_{t} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )
  plot_dict['downstream_pz_residual_pz'] = ROOT.TH2F( \
          'downstream_pz_residual_pz', "Downstream p_{z} Residuals in p_{z}", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 500, -50.0, 50.0 )

  for pl_id in range( -15, 0 ) + range( 1, 16 ) :
    pull_plot_name = 'kalman_pulls_{0:02d}'.format(pl_id)
    plot_dict[pull_plot_name] = ROOT.TH1F( pull_plot_name, "Kalman Pulls", \
                                                             101, -5.05, 5.05 )
#   residual_plot_name = 'plane_residual_{0:02d}_xy'.format(pl_id)
#   plot_dict[residual_plot_name] = ROOT.TH2F(residual_plot_name, \
#                                  "Plane {0:02d} XY Residual".format(pl_id), \
#                                           200, -20.0, 20.0, 200, -20.0, 20.0)
#   residual_plot_name = 'plane_residual_{0:02d}_pxpy'.format(pl_id)
#   plot_dict[residual_plot_name] = ROOT.TH2F(residual_plot_name, \
#                                "Plane {0:02d} PxPy Residual".format(pl_id), \
#                                           200, -20.0, 20.0, 200, -20.0, 20.0)



  plot_dict['upstream_mc_alpha'] = ROOT.TH2F( 'upstream_mc_alpha', \
               "Upstream MC Alpha Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                               200, -2.0, 2.0 )
  plot_dict['upstream_mc_beta'] = ROOT.TH2F( 'upstream_mc_beta', \
                "Upstream MC Beta Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                            1000, 0.0, 2500.0 )
  plot_dict['upstream_mc_emittance'] = ROOT.TH2F( 'upstream_mc_emittance', \
           "Upstream MC Emittance Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                               500, 0.0, 20.0 )

  plot_dict['upstream_recon_alpha'] = ROOT.TH2F( 'upstream_recon_alpha', \
                  "Upstream Alpha Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                               200, -2.0, 2.0 )
  plot_dict['upstream_recon_beta'] = ROOT.TH2F( 'upstream_recon_beta', \
                   "Upstream Beta Reconstruction Pz", PZ_BIN, PZ_MIN, PZ_MAX, \
                                                            1000, 0.0, 2500.0 )
  plot_dict['upstream_recon_emittance'] = ROOT.TH2F( \
          'upstream_recon_emittance', "Upstream Emittance Reconstruction Pz", \
                                       PZ_BIN, PZ_MIN, PZ_MAX, 500, 0.0, 20.0 )

  plot_dict['upstream_residual_alpha'] = ROOT.TH2F( \
             'upstream_residual_alpha', "Upstream Alpha Residual Pz", PZ_BIN, \
                                               PZ_MIN, PZ_MAX, 200, -1.0, 1.0 )
  plot_dict['upstream_residual_beta'] = ROOT.TH2F( \
                       'upstream_residual_beta', "Upstream Beta Residual Pz", \
                                   PZ_BIN, PZ_MIN, PZ_MAX, 200, -100.0, 100.0 )
  plot_dict['upstream_residual_emittance'] = ROOT.TH2F( \
             'upstream_residual_emittance', "Upstream Emittance Residual Pz", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 200, -10.0, 10.0 )




  plot_dict['downstream_mc_alpha'] = ROOT.TH2F( \
              'downstream_mc_alpha', "Downstream MC Alpha Reconstruction Pz", \
                                       PZ_BIN, PZ_MIN, PZ_MAX, 200, -2.0, 2.0 )
  plot_dict['downstream_mc_beta'] = ROOT.TH2F( \
                'downstream_mc_beta', "Downstream MC Beta Reconstruction Pz", \
                                    PZ_BIN, PZ_MIN, PZ_MAX, 1000, 0.0, 2500.0 )
  plot_dict['downstream_mc_emittance'] = ROOT.TH2F( \
      'downstream_mc_emittance', "Downstream MC Emittance Reconstruction Pz", \
                                       PZ_BIN, PZ_MIN, PZ_MAX, 500, 0.0, 20.0 )

  plot_dict['downstream_recon_alpha'] = ROOT.TH2F( \
              'downstream_recon_alpha', "Downstream Alpha Reconstruction Pz", \
                                       PZ_BIN, PZ_MIN, PZ_MAX, 200, -2.0, 2.0 )
  plot_dict['downstream_recon_beta'] = ROOT.TH2F( \
                'downstream_recon_beta', "Downstream Beta Reconstruction Pz", \
                                    PZ_BIN, PZ_MIN, PZ_MAX, 1000, 0.0, 2500.0 )
  plot_dict['downstream_recon_emittance'] = ROOT.TH2F( \
      'downstream_recon_emittance', "Downstream Emittance Reconstruction Pz", \
                                       PZ_BIN, PZ_MIN, PZ_MAX, 500, 0.0, 20.0 )

  plot_dict['downstream_residual_alpha'] = ROOT.TH2F( \
                 'downstream_residual_alpha', "Downstream Alpha Residual Pz", \
                                       PZ_BIN, PZ_MIN, PZ_MAX, 200, -1.0, 1.0 )
  plot_dict['downstream_residual_beta'] = ROOT.TH2F( \
                   'downstream_residual_beta', "Downstream Beta Residual Pz", \
                                   PZ_BIN, PZ_MIN, PZ_MAX, 200, -100.0, 100.0 )
  plot_dict['downstream_residual_emittance'] = ROOT.TH2F( \
         'downstream_residual_emittance', "Downstream Emittance Residual Pz", \
                                     PZ_BIN, PZ_MIN, PZ_MAX, 200, -10.0, 10.0 )



  data_dict = { 'counters' : {}, 'data' : {} }
  data_dict['counters']['missing_virtual_hits'] = 0
  data_dict['counters']['missing_reference_hits'] = 0
  data_dict['counters']['missing_tracks_up'] = 0
  data_dict['counters']['missing_tracks_down'] = 0
  data_dict['counters']['found_tracks_up'] = 0
  data_dict['counters']['found_tracks_down'] = 0
  data_dict['counters']['found_pairs'] = 0

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
    for plane in virtual_plane_dict :
      if virtual_plane_dict[plane][1] > ALIGNMENT_TOLERANCE :
#        print plane, virtual_plane_dict[plane]
        done = False
    if done :
      break
  else :
    raise ValueError("Could not locate all virtuals planes")

  file_reader.reset()
  return virtual_plane_dict


def get_expected_tracks(mc_event, virtual_plane_dict) :
  upstream_planes = [ virtual_plane_dict[i][0] for i in range(-15, 0)]
  downstream_planes = [ virtual_plane_dict[i][0] for i in range(1, 16)]

  upstream_hits = 0
  downstream_hits = 0

  upstream_tracks = 0
  downstream_tracks = 0

  for vhit_num in xrange(mc_event.GetVirtualHitsSize()) :
    vhit = mc_event.GetAVirtualHit(vhit_num)
    station_id = vhit.GetStationId()

    if station_id in upstream_planes :
      upstream_hits += 1
    if station_id in downstream_planes :
      downstream_hits += 1

  if upstream_hits > 10 :
    upstream_tracks += 1
  if downstream_hits > 10 :
    downstream_tracks += 1

  return upstream_tracks, downstream_tracks



def make_scifi_mc_pairs(plot_dict, data_dict, virtual_plane_dict, \
                                                        scif_event, mc_event) :
  """
    Make pairs of SciFiTrackpoints and MC VirtualHits
  """
  paired_hits = []

  expected_up, expected_down = get_expected_tracks(mc_event, virtual_plane_dict)

  tracks = scifi_event.scifitracks()
  for track in tracks :
    if track.GetAlgorithmUsed() != TRACK_ALGORITHM :
      continue
    trackpoints = track.scifitrackpoints()
    trackpoint = None

# Find a reference trackpoint
    for trkpt in trackpoints :
      pl_id = analysis.tools.calculate_plane_id(\
                               trkpt.tracker(), trkpt.station(), trkpt.plane())
      plot_name = 'kalman_pulls_{0:02d}'.format(pl_id)
      plot_dict[plot_name].Fill( trkpt.pull() )

      if trkpt.station() == RECON_STATION and trkpt.plane() == RECON_PLANE :
        trackpoint = trkpt

# Histogram the number of trackpoints
    tracker = track.tracker()
    pt = math.sqrt( trackpoint.mom().x()**2 + trackpoint.mom().y()**2 )
    pz = trackpoint.mom().z()
    tp_counter = 0
    for trkpt in trackpoints :
      if trkpt.has_data() :
        tp_counter += 1

    if tracker == 0 :
      plot_dict['upstream_ntp'].Fill( tp_counter )
      plot_dict['upstream_ntp_pt'].Fill( pt, tp_counter )
      plot_dict['upstream_ntp_pz'].Fill( pz, tp_counter )
    else :
      plot_dict['downstream_ntp'].Fill( tp_counter )
      plot_dict['downstream_ntp_pt'].Fill( pt, tp_counter )
      plot_dict['downstream_ntp_pz'].Fill( pz, tp_counter )

# If no data then give up!
    if not trackpoint.has_data() :
      data_dict['counters']['missing_reference_hits'] += 1
      continue

    expected_virtual_plane = virtual_plane_dict[\
            analysis.tools.calculate_plane_id(\
            trackpoint.tracker(), trackpoint.station(), trackpoint.plane())][0]


# Find the virtual hit
    for vhit_num in xrange(mc_event.GetVirtualHitsSize()) :
      vhit = mc_event.GetAVirtualHit(vhit_num)
      if vhit.GetStationId() == expected_virtual_plane and \
                                        abs(vhit.GetParticleId()) == MUON_PID :
        virtual_hit = vhit
        break
    else :
      data_dict['counters']['missing_virtual_hits'] += 1
      continue

# Add to list
    paired_hits.append((trackpoint, virtual_hit))
    data_dict['counters']['found_pairs'] += 1

  found_up = 0
  found_down = 0

  for scifi_hit, _ in paired_hits :
    if scifi_hit.tracker() == 0 :
      found_up += 1
    else :
      found_down += 1
    
  data_dict['counters']['missing_tracks_up'] += (expected_up - found_up)
  data_dict['counters']['missing_tracks_down'] += (expected_down - found_down)
  data_dict['counters']['found_tracks_up'] += found_up
  data_dict['counters']['found_tracks_down'] += found_down

  return paired_hits


def fill_plots(plot_dict, data_dict, hit_pairs) :
  """
    Fill Plots with Track and Residual Data
  """
  for scifi_hit, virt_hit in hit_pairs :
    tracker = scifi_hit.tracker()
    if tracker == 0 :
      prefix = 'upstream_'
      UP_COV_MC.add_hit(hit_types.AnalysisHit(virtual_track_point=virt_hit))
      UP_COV_RECON.add_hit(hit_types.AnalysisHit(scifi_track_point=scifi_hit))
    else :
      prefix = 'downstream_'
      DOWN_COV_MC.add_hit(hit_types.AnalysisHit(virtual_track_point=virt_hit))
      DOWN_COV_RECON.add_hit(hit_types.AnalysisHit(scifi_track_point=scifi_hit))

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

    Pt_mc = math.sqrt( virt_mom[0] ** 2 + virt_mom[1] ** 2 )
    Pz_mc = virt_mom[2]

    Pt_recon = math.sqrt( scifi_mom[0] ** 2 + scifi_mom[1] ** 2 )
    Pt_res = Pt_recon - Pt_mc

    plot_dict[prefix+'xy'].Fill(scifi_pos[0], scifi_pos[1])
    plot_dict[prefix+'pxpy'].Fill(scifi_mom[0], scifi_mom[1])
    plot_dict[prefix+'pt'].Fill(Pt_recon)
    plot_dict[prefix+'pz'].Fill(scifi_mom[2])

    plot_dict[prefix+'mc_xy'].Fill(virt_pos[0], virt_pos[1])
    plot_dict[prefix+'mc_pxpy'].Fill(virt_mom[0], virt_mom[1])
    plot_dict[prefix+'mc_pt'].Fill(Pt_mc)
    plot_dict[prefix+'mc_pz'].Fill(Pz_mc)

    plot_dict[prefix+'residual_xy'].Fill(res_pos[0], res_pos[1])
    plot_dict[prefix+'residual_pxpy'].Fill(res_mom[0], res_mom[1])
    plot_dict[prefix+'residual_pt'].Fill(Pt_res)
    plot_dict[prefix+'residual_pz'].Fill(res_mom[2])

    plot_dict[prefix+'x_residual_pt'].Fill( Pt_mc, res_pos[0] )
    plot_dict[prefix+'y_residual_pt'].Fill( Pt_mc, res_pos[1] )
    plot_dict[prefix+'px_residual_pt'].Fill( Pt_mc, res_mom[0] )
    plot_dict[prefix+'py_residual_pt'].Fill( Pt_mc, res_mom[1] )
    plot_dict[prefix+'pt_residual_pt'].Fill( Pt_mc, Pt_res )
    plot_dict[prefix+'pz_residual_pt'].Fill( Pt_mc, res_mom[2] )

    plot_dict[prefix+'x_residual_pz'].Fill( Pz_mc, res_pos[0] )
    plot_dict[prefix+'y_residual_pz'].Fill( Pz_mc, res_pos[1] )
    plot_dict[prefix+'px_residual_pz'].Fill( Pz_mc, res_mom[0] )
    plot_dict[prefix+'py_residual_pz'].Fill( Pz_mc, res_mom[1] )
    plot_dict[prefix+'pt_residual_pz'].Fill( Pz_mc, Pt_res )
    plot_dict[prefix+'pz_residual_pz'].Fill( Pz_mc, res_mom[2] )

    if UP_COV_MC.length() == ENSEMBLE_SIZE :
      up_pz = UP_COV_MC.get_mean('pz')
      plot_dict['upstream_mc_alpha'].Fill(up_pz, UP_COV_MC.get_alpha(['x','y']))
      plot_dict['upstream_mc_beta'].Fill(up_pz, UP_COV_MC.get_beta(['x','y']))
      plot_dict['upstream_mc_emittance'].Fill(up_pz, UP_COV_MC.get_emittance(\
                                                          ['x','px','y','py']))

      plot_dict['upstream_recon_alpha'].Fill(up_pz, UP_COV_RECON.get_alpha(\
                                                                    ['x','y']))
      plot_dict['upstream_recon_beta'].Fill(up_pz, UP_COV_RECON.get_beta(\
                                                                    ['x','y']))
      plot_dict['upstream_recon_emittance'].Fill(up_pz, \
                               UP_COV_RECON.get_emittance(['x','px','y','py']))

      plot_dict['upstream_residual_alpha'].Fill(up_pz, \
            UP_COV_RECON.get_alpha(['x','y']) - UP_COV_MC.get_alpha(['x','y']))
      plot_dict['upstream_residual_beta'].Fill(up_pz, \
              UP_COV_RECON.get_beta(['x','y']) - UP_COV_MC.get_beta(['x','y']))
      plot_dict['upstream_residual_emittance'].Fill(up_pz, \
                            UP_COV_RECON.get_emittance(['x','px','y','py']) - \
                                  UP_COV_MC.get_emittance(['x','px','y','py']))

      UP_COV_MC.clear()
      UP_COV_RECON.clear()


    if DOWN_COV_MC.length() == ENSEMBLE_SIZE :
      down_pz = UP_COV_MC.get_mean('pz')
      plot_dict['downstream_mc_alpha'].Fill(down_pz, \
                                              DOWN_COV_MC.get_alpha(['x','y']))
      plot_dict['downstream_mc_beta'].Fill(down_pz, \
                                               DOWN_COV_MC.get_beta(['x','y']))
      plot_dict['downstream_mc_emittance'].Fill(down_pz, \
                                DOWN_COV_MC.get_emittance(['x','px','y','py']))

      plot_dict['downstream_recon_alpha'].Fill(down_pz, \
                                           DOWN_COV_RECON.get_alpha(['x','y']))
      plot_dict['downstream_recon_beta'].Fill(down_pz, \
                                            DOWN_COV_RECON.get_beta(['x','y']))
      plot_dict['downstream_recon_emittance'].Fill(down_pz, \
                             DOWN_COV_RECON.get_emittance(['x','px','y','py']))

      plot_dict['downstream_residual_alpha'].Fill(down_pz, \
        DOWN_COV_RECON.get_alpha(['x','y']) - DOWN_COV_MC.get_alpha(['x','y']))
      plot_dict['downstream_residual_beta'].Fill(down_pz, \
          DOWN_COV_RECON.get_beta(['x','y']) - DOWN_COV_MC.get_beta(['x','y']))
      plot_dict['downstream_residual_emittance'].Fill(down_pz, \
                          DOWN_COV_RECON.get_emittance(['x','px','y','py']) - \
                                DOWN_COV_MC.get_emittance(['x','px','y','py']))

      DOWN_COV_MC.clear()
      DOWN_COV_RECON.clear()


def analyse_plots(plot_dict, data_dict) :
  """
    Use existing plots to perform some useful analysis
  """
# Print out some simple stats
  print
  print
  print "Missed {0:0.0f} Virtual Hits".format( \
                                data_dict['counters']['missing_virtual_hits'] )
  print "Missed {0:0.0f} Reference Plane Hits".format( \
                              data_dict['counters']['missing_reference_hits'] )
  print "Missed {0:0.0f} Upstream Tracks".format( \
                              data_dict['counters']['missing_tracks_up'] )
  print "Missed {0:0.0f} Downstream Tracks".format( \
                              data_dict['counters']['missing_tracks_down'] )
  print "Found {0:0.0f} Upstream Tracks".format( \
                                     data_dict['counters']['found_tracks_up'] )
  print "Found {0:0.0f} Downstream Tracks".format( \
                                   data_dict['counters']['found_tracks_down'] )
  print

# Make the pretty plots

  for tracker_prefix in [ "upstream_", "downstream_" ] :
    for component in [ "x_", "y_", "px_", "py_", "pt_", "pz_" ] :
      for plot_axis in [ "residual_pt", "residual_pz" ] :
        plot = plot_dict[tracker_prefix+component+plot_axis]

        errors = array.array( 'd' )
        bin_size = array.array( 'd' )
        bins = array.array( 'd' )
        rms = array.array( 'd' )

        width = int( plot.GetNbinsX() / RESOLUTION_BINS )
        for i in range( 0, RESOLUTION_BINS-1 ) :
          projection = plot.ProjectionY( \
      tracker_prefix+component+plot_axis+'_pro_'+str(i), i*width, (i+1)*width )

          plot_mean = plot.GetXaxis().GetBinCenter( i*width ) + width*0.5
#          pro_mean, pro_mean_err, pro_std, pro_std_err = \
          _, _, pro_std, pro_std_err = \
                                        analysis.tools.fit_gaussian(projection)

#          plot.GetXaxis().SetRange( i*width, (i+1)*width )
#          plot_rms = plot.GetRMS( 2 )

          bin_size.append( width*0.5 )
          errors.append( pro_std_err )
          bins.append( plot_mean )
          rms.append( pro_std )

        resolution_graph = ROOT.TGraphErrors( RESOLUTION_BINS-1, \
                                                  bins, rms, bin_size, errors )

        plot_dict[tracker_prefix+component+plot_axis+'_resolution'] = \
                                                               resolution_graph

      for plot_axis in [ "pt", "pz" ] :
        plot = plot_dict[tracker_prefix+"ntp_"+plot_axis]

        errors = array.array( 'd' )
        bin_size = array.array( 'd' )
        bins = array.array( 'd' )
        eff = array.array( 'd' )

        width = int( plot.GetNbinsX() / EFFICIENCY_BINS )
        for i in range( 0, EFFICIENCY_BINS-1 ) :
          projection = plot.ProjectionY( \
      tracker_prefix+component+plot_axis+'_pro_'+str(i), i*width, (i+1)*width )

          plot_mean = plot.GetXaxis().GetBinCenter( i*width ) + width*0.5
          plot_entries = projection.GetEntries()

          if plot_entries == 0 :
            continue

          integral = 0.0
          expected = plot_entries * 15.0
          for bin_num in range( 1, 16 ) :
            val = bin_num * projection.GetBinContent( bin_num )
            integral += val

          bin_size.append( width*0.5 )
          errors.append( 100.0 / math.sqrt( plot_entries ) )
          bins.append( plot_mean )
          eff.append( 100.0 * integral / expected )

        resolution_graph = ROOT.TGraphErrors( EFFICIENCY_BINS-1, \
                                                  bins, eff, bin_size, errors )

        plot_dict[tracker_prefix+plot_axis+'_efficiency'] = resolution_graph

  return data_dict


def save_plots(plot_dict, directory, filename, print_plots=False) :
  """
    Save all the plots to file
  """
  filename = os.path.join(directory, filename+".root")
  if print_plots :
    outfile = ROOT.TFile(filename, "RECREATE")
    for plot in sorted(plot_dict) :
      plot_dict[plot].Write()
      name = plot_dict[plot].GetName()
      canvas = ROOT.TCanvas(name+"_canvas")
      plot_dict[plot].Draw()
      canvas.SaveAs( os.path.join( directory, name+".pdf" ) )
    outfile.Close()
  else :
    outfile = ROOT.TFile(filename, "RECREATE")
    for plot in sorted(plot_dict) :
      plot_dict[plot].Write( plot )
    outfile.Close()

def save_pretty(plot_dict, output_directory) :
  """
    Save the required plots after some beautification
  """
  for tracker_prefix in [ "upstream_", "downstream_" ] :
    for component, comp_title in [ ("pt_", "#sigma_{p_{#perp}}  [MeV/c]"), \
                                         ("pz_", "#sigma_{p_{z}}  [MeV/c]") ] :
      for plot_axis, axis_title in [ ("residual_pt", "p_{#perp}  [MeV/c]"), \
                                          ("residual_pz", "p_{z}  [MeV/c]") ] :
        plot = plot_dict[tracker_prefix+component+plot_axis+'_resolution']
        canvas = ROOT.TCanvas( tracker_prefix+component+plot_axis+'_canvas' )

        plot.Draw()

        plot.GetXaxis().SetTitle(axis_title)
        plot.GetYaxis().SetTitle(comp_title)
      
        canvas.Update()

        out_file_name = os.path.join( output_directory, \
                         tracker_prefix+component+plot_axis+'_resolution.pdf' )
        canvas.SaveAs( out_file_name, 'pdf' )
        

      for tracker_prefix in [ "upstream_", "downstream_" ] :
        for plot_axis, axis_title in [ ("pt", "p_{#perp}  [MeV/c]"), \
                                                   ("pz", "p_{z}  [MeV/c]") ] :
          plot = plot_dict[tracker_prefix+plot_axis+'_efficiency']
          canvas = ROOT.TCanvas( tracker_prefix+plot_axis+'_canvas' )

          plot.Draw()

          plot.GetXaxis().SetTitle(axis_title)
          plot.GetYaxis().SetTitle(comp_title)
        
          canvas.Update()

          out_file_name = os.path.join( output_directory, \
                                   tracker_prefix+plot_axis+'_efficiency.pdf' )
          canvas.SaveAs( out_file_name, 'pdf' )


  ROOT.gStyle.SetOptStat(0)
  for tracker, mult in [ ("upstream", -1.0), ("downstream", 1.0) ] :
    canvas = ROOT.TCanvas( tracker+"_pulls" )
    canvas.Divide(3, 5, 0, 0)
    for pl_num in range( 1, 16 ) :
      pl_id = int(mult*pl_num)
      plot_name = 'kalman_pulls_{0:02d}'.format(pl_id)
      plot = plot_dict[plot_name]

      canvas.cd(pl_num)
      plot.Draw()
      canvas.GetPad(pl_num).SetLogy()

    out_file_name = os.path.join( output_directory, \
                                                  tracker+'_kalman_pulls.pdf' )
    canvas.SaveAs(out_file_name, 'pdf')

  for tracker in [ "upstream", "downstream" ] :
    for component, axis_title in [ ("pt", "p_{#perp}  [MeV/c]"), \
                                                   ("pz", "p_{z}  [MeV/c]") ] :
      plot = plot_dict[tracker+'_residual_'+component]

      canvas = ROOT.TCanvas(tracker+'_residual_'+component+'_canvas')
      plot.Draw()

      plot.GetXaxis().SetTitle(axis_title)
      plot.GetYaxis().SetTitle("Frequency")
      canvas.Update()

      out_file_name = os.path.join( output_directory, \
                                        tracker+'_residual_'+component+'.pdf' )
      canvas.SaveAs( out_file_name, 'pdf' )



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

  parser.add_argument( '-P', '--print_plots', action='store_true', \
                        help="Flag to save the plots as individual pdf files" )

  parser.add_argument( '--cut_number_trackpoints', type=int, default=0, \
          help="Specify the minumum number of trackpoints required per track" )

  parser.add_argument( '--track_algorithm', type=int, default=1, \
                          help="Specify the track reconstruction algorithm. "+\
                             "1 for Helical Tracks and 0 for Straight Tracks" )

  parser.add_argument( '--ensemble_size', type=int, default=2000, \
                        help="Specify the size of the ensemble of particles "+\
                                     "to consider per emittance measurement." )

#  parser.add_argument( '-C', '--configuration_file', help='Configuration '+\
#      'file for the reconstruction. I need the geometry information' )

  try :
    namespace = parser.parse_args()

    MIN_NUMBER_TRACKPOINTS = namespace.cut_number_trackpoints
    TRACK_ALGORITHM = namespace.track_algorithm
    ENSEMBLE_SIZE = namespace.ensemble_size
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
    sys.stdout.write( "\n- Finding Virtual Planes : Running\r" )
    sys.stdout.flush()
    virtual_plane_dictionary = create_virtual_plane_dict(file_reader)
    sys.stdout.write(   "- Finding Virtual Planes : Done   \n" )

##### 4. Load Events ##########################################################
    print "\n- Loading Spills...\n"
    try :
      while file_reader.next_event() and \
               file_reader.get_total_num_events() != namespace.max_num_events :
        try :
          sys.stdout.write( 
              '  Spill ' + str(file_reader.get_current_spill_number()) + \
              ' of ' + str(file_reader.get_current_number_spills()) + \
              ' in File ' + str(file_reader.get_current_filenumber()) + \
              ' of ' + str(file_reader.get_number_files()) + '             \r')
          sys.stdout.flush()

          scifi_event = file_reader.get_event( 'scifi' )
          mc_event = file_reader.get_event( 'mc' )

##### 5. Extract tracks and Fill Plots ########################################

          paired_hits = make_scifi_mc_pairs(plot_dict, data_dict, \
                               virtual_plane_dictionary, scifi_event, mc_event)
          fill_plots(plot_dict, data_dict, paired_hits)

        except ValueError :
          print "An Error Occured. Skipping Spill: " + \
                str(file_reader.get_current_spill_number()) + \
                " In File: " + str(file_reader.get_current_filenumber()) + "\n"
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
    save_pretty(plot_dict, namespace.output_directory )

    save_plots(plot_dict, namespace.output_directory, \
                              namespace.output_filename, namespace.print_plots)
    sys.stdout.write(   "- Saving Plots and Data : Done   \n" )

  print 
  print "Complete."
  print

