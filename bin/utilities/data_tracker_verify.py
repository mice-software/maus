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
  This analysis script will perform the first offline verification of the 
  tracker data. Simple plots and analysis are made/performed to examine the
  performance during the run.

  It is critical to have a fast and simple way to check the data "looks" good
  before we start running more detailed analyses.
"""

# pylint: disable = W0311, E1101, W0613, C0111, W0621, C0103, W0702, W0611
# pylint: disable = R0912, R0914, R0915

import MAUS

# Generic Python imports
import sys
import os
import math
import argparse
import types

# Third Party library import statements
import event_loader
from analysis import tools
import ROOT


# Useful Constants and configuration
REFERENCE_PLANE = 0
REFERENCE_STATION = 1
TOF_CUT_LOW = 0.0
TOF_CUT_HIGH = 1000.0
P_VALUE_CUT = 0.05
GRAD_CUT = 1.05
MIN_NUM_TRACKPOINTS = 0
IGNORE_PLANES = [ -7, -8, -9 ]

PLOT_OPTIONS = { 'npe' : [ 'logy' ], \
                 'npe_cluster' : [ 'logy' ], \
                 'adc' : [ 'logy' ], \
                 'adc_channel' : [ 'logy', 'colz' ], \
                 'npe_channel' : [ 'logz', 'colz' ], \
                 'plane_pulls' : [ 'colz' ], \
                 'plane_residuals' : [ 'colz' ], \
                 'plane_s_residuals' : [ 'colz' ], \
                 'xy' : [ 'colz' ], \
                 'xmx' : [ 'colz' ], \
                 'ymy' : [ 'colz' ], \
                 'mxmy' : [ 'colz' ], \
                 'xpx' : [ 'colz' ], \
                 'ypy' : [ 'colz' ], \
                 'pxpy' : [ 'colz' ], \
                 'spacepoint_road' : [ 'colz' ],
                 'spacepoints' : [ 'colz'],
                 '2_hit_spacepoints' : [ 'colz' ],
                 '3_hit_spacepoints' : [ 'colz' ] }


def init_plots_data() :
  """
    Initialise a dictionary of plots to fill with data
  """
  plot_dict = {}

  track_plots = {}
  track_plots['tracks_event'] = ROOT.TH1F( "tracks_event", \
                                "Number of Tracks per Event", 100, 0.0, 100.0 )
  track_plots['trackpoints_track'] = ROOT.TH1F( "trackpoints_track", \
                           "Number of Trackpoints per Track", 100, 0.0, 100.0 )
  track_plots['tracks_tracker'] = ROOT.TH1F( "tracks_tracker", \
                                  "Number of Tracks per Tracker", 2, 0.0, 2.0 )

  for shape in [ "helical", "straight" ] :
    shape_plots = {}

    shape_plots['plane_pulls'] = ROOT.TH2F(shape+"_plane_pulls", \
                         "Pulls per Plane", 31, -15.5, 15.5, 500, -20.0, 20.0 )
    shape_plots['plane_residuals'] = ROOT.TH2F(shape+"_plane_residuals",
            "Filtered Residuals per Plane", 31, -15.5, 15.5, 500, -20.0, 20.0 )
    shape_plots['plane_s_residuals'] = ROOT.TH2F(shape+"_plane_s_residuals",
            "Smoothed Residuals per Plane", 31, -15.5, 15.5, 500, -20.0, 20.0 )

    shape_plots['tracks_event'] = ROOT.TH1F( shape+"_tracks_event", \
                                "Number of Tracks per Event", 100, 0.0, 100.0 )
    shape_plots['trackpoints_track'] = ROOT.TH1F( shape+"_trackpoints_track", \
                           "Number of Trackpoints per Track", 100, 0.0, 100.0 )
    shape_plots['tracks_tracker'] = ROOT.TH1F( shape+"_tracks_tracker", \
                                  "Number of Tracks per Tracker", 2, 0.0, 2.0 )
    shape_plots['chi_squared'] = ROOT.TH1F( shape+'_tracks_chi_squared', \
                                 "Chi Squared Distribution", 5000, 0.0, 100.0 )
    shape_plots['chi_squared_up'] = ROOT.TH1F( shape+'_tracks_chi_squared_up', \
                        "Upstream Chi Squared Distribution", 5000, 0.0, 100.0 )
    shape_plots['chi_squared_down'] = ROOT.TH1F( \
                                            shape+'_tracks_chi_squared_down', \
                      "Downstream Chi Squared Distribution", 5000, 0.0, 100.0 )
    shape_plots['chi_squared_ndf'] = ROOT.TH1F( \
                                             shape+'_tracks_chi_squared_ndf', \
           "Chi Squared per Degree of Freedom Distribution", 5000, 0.0, 100.0 )
    shape_plots['chi_squared_ndf_up'] = ROOT.TH1F( \
                                         shape+'_tracks_chi_squared_ndf_up',  \
                   "Upstream Chi Squared per Degree of Freedom Distribution", \
                                                             5000, 0.0, 100.0 )
    shape_plots['chi_squared_ndf_down'] = ROOT.TH1F( \
                                        shape+'_tracks_chi_squared_ndf_down', \
                 "Downstream Chi Squared per Degree of Freedom Distribution", \
                                                             5000, 0.0, 100.0 )
    shape_plots['p_value'] = ROOT.TH1F( shape+'_tracks_p_value', \
                                       "P-Value Distribution", 1000, 0.0, 1.0 )
    shape_plots['ndf'] = ROOT.TH1F( shape+'_tracks_ndf', \
                     "Number Degrees of Freedom Distribution", 21, -0.5, 20.5 )
    shape_plots['p_value_up'] = ROOT.TH1F( shape+'_tracks_p_value_up', \
                              "Upstream P-Value Distribution", 1000, 0.0, 1.0 )
    shape_plots['ndf_up'] = ROOT.TH1F( shape+'_tracks_ndf_up', \
            "Upstream Number Degrees of Freedom Distribution", 21, -0.5, 20.5 )
    shape_plots['p_value_down'] = ROOT.TH1F( shape+'_tracks_p_value_down', \
                              "Upstream P-Value Distribution", 1000, 0.0, 1.0 )
    shape_plots['ndf_down'] = ROOT.TH1F( shape+'_tracks_ndf_down', \
            "Upstream Number Degrees of Freedom Distribution", 21, -0.5, 20.5 )


    track_plots[shape] = shape_plots


  reco_plots = {}
  reco_plots['tof_0_1'] = ROOT.TH1F( 'tof_0_1', 'Time TOF0 - TOF1', \
                                                             1000, 0.0, 100.0 )
  reco_plots['tof_1_2'] = ROOT.TH1F( 'tof_1_2', 'Time TOF1 - TOF2', \
                                                             1000, 0.0, 100.0 )
  reco_plots['tof_0_1_cut'] = ROOT.TH1F( 'tof_0_1_cut', 'Time TOF0 - TOF1', \
                                                             1000, 0.0, 100.0 )
  reco_plots['tof_1_2_cut'] = ROOT.TH1F( 'tof_1_2_cut', 'Time TOF1 - TOF2', \
                                                             1000, 0.0, 100.0 )

  reco_plots['npe'] = ROOT.TH1F( 'npe', "Number of Photo Electrons", \
                                                            100, 0.0, 100.0 )
  reco_plots['adc'] = ROOT.TH1F( 'adc', "ADC Count", 260, 0.0, 260.0 )
  reco_plots['plane_hits'] = ROOT.TH1F("plane_hits", "Hits per Plane", 31, \
                                                                  -15.0, 16.0 )
  reco_plots['channels_cluster'] = ROOT.TH1F( 'channels_cluster', \
                                        "Channels per Cluster", 50, 0.0, 50.0 )
  reco_plots['npe_cluster'] = ROOT.TH1F( 'npe_cluster', \
                                             "NPE per Cluster", 75, 0.0, 75.0 )
  reco_plots['spacepoints_station'] = ROOT.TH1F( "spacepoints_station", \
                           "Spacepoints Found in Each Station", 11, -5.0, 6.0 )
  reco_plots['2_hit_spacepoints_station'] = ROOT.TH1F( \
                                                 "2_hit_spacepoints_station", \
                           "Spacepoints Found in Each Station", 11, -5.0, 6.0 )
  reco_plots['3_hit_spacepoints_station'] = ROOT.TH1F( \
                                                 "3_hit_spacepoints_station", \
                           "Spacepoints Found in Each Station", 11, -5.0, 6.0 )
  reco_plots['clusters_plane'] = ROOT.TH1F( "clusters_plane", \
                              "Clusters Found in Each Plane", 31, -15.0, 16.0 )
  reco_plots['spacepoints'] = ROOT.TH2F( 'spacepoints', \
               "Spacepoint Positions", 200, -200.0, 200.0, 200, -200.0, 200.0 )
  reco_plots['saturations_event'] = ROOT.TH1F( 'saturations_event', \
                    "Number of Saturated Digits per Event", 8192, 0.0, 8192.0 )



  patrec_plots = {}

  patrec_plots['chi_squared_circle'] = ROOT.TH1F( 'chi_squared_circle', \
                                   "Circle Fit Chi Squared", 1000, 0.0, 100.0 )
  patrec_plots['chi_squared_sz'] = ROOT.TH1F( 'chi_squared_sz', \
                                      "S-Z Fit Chi Squared", 1000, 0.0, 100.0 )
  patrec_plots['chi_squared_x'] = ROOT.TH1F( 'chi_squared_x', \
                                        "X Fit Chi Squared", 1000, 0.0, 100.0 )
  patrec_plots['chi_squared_y'] = ROOT.TH1F( 'chi_squared_y', \
                                        "Y Fit Chi Squared", 1000, 0.0, 100.0 )
  patrec_plots['chi_squared_straight'] = ROOT.TH1F( 'chi_squared_straight', \
                                 "Straight Fit Chi Squared", 1000, 0.0, 100.0 )
  patrec_plots['chi_squared_straight_ndf'] = ROOT.TH1F( \
                'chi_squared_straight_ndf', "Straight Fit Chi Squared / NDF", \
                                                             1000, 0.0, 100.0 )
  patrec_plots['chi_squared_helical'] = ROOT.TH1F( 'chi_squared_helical', \
                                  "Helical Fit Chi Squared", 1000, 0.0, 100.0 )
  patrec_plots['chi_squared_helical_ndf'] = ROOT.TH1F( \
                  'chi_squared_helical_ndf', "Helical Fit Chi Squared / NDF", \
                                                             1000, 0.0, 100.0 )

  patrec_plots['up_patrec_xy'] = ROOT.TH2F( "up_patrec_xy", \
      "XY Positions of Pat Rec Track", 100, -200.0, 200.0, 100, -200.0, 200.0 )
  patrec_plots['up_patrec_mxmy'] = ROOT.TH2F( "up_patrec_mxmy", \
              "XY Gradients of Pat Rec Track", 100, -0.5, 0.5, 100, -0.5, 0.5 )
  patrec_plots['up_patrec_pxpy'] = ROOT.TH2F( "up_patrec_pxpy", \
        "XY Momenta of Pat Rec Track", 100, -200.0, 200.0, 100, -200.0, 200.0 )

  patrec_plots['down_patrec_xy'] = ROOT.TH2F( "down_patrec_xy", \
      "XY Positions of Pat Rec Track", 100, -200.0, 200.0, 100, -200.0, 200.0 )
  patrec_plots['down_patrec_mxmy'] = ROOT.TH2F( "down_patrec_mxmy", \
              "XY Gradients of Pat Rec Track", 100, -0.5, 0.5, 100, -0.5, 0.5 )
  patrec_plots['down_patrec_pxpy'] = ROOT.TH2F( "down_patrec_pxpy", \
        "XY Momenta of Pat Rec Track", 100, -200.0, 200.0, 100, -200.0, 200.0 )

  comp_plots = {}

  comp_plots['up_patrec_kalman_xy'] = ROOT.TH2F( \
                               "up_patrec_kalman_xy", "XY Position Residual", \
                                               100, -5.0, 5.0, 100, -5.0, 5.0 )
  comp_plots['up_patrec_kalman_mxmy'] = ROOT.TH2F(
                             "up_patrec_kalman_mxmy", "XY Gradient Residual", \
                                           100, -0.05, 0.05, 100, -0.05, 0.05 )
  comp_plots['up_patrec_kalman_pxpy'] = ROOT.TH2F( \
                              "up_patrec_kalman_pxpy", "XY Momenta Residual", \
                                               100, -5.0, 5.0, 100, -5.0, 5.0 )

  comp_plots['down_patrec_kalman_xy'] = ROOT.TH2F( \
                             "down_patrec_kalman_xy", "XY Position Residual", \
                                               100, -5.0, 5.0, 100, -5.0, 5.0 )
  comp_plots['down_patrec_kalman_mxmy'] = ROOT.TH2F( \
                           "down_patrec_kalman_mxmy", "XY Gradient Residual", \
                                           100, -0.05, 0.05, 100, -0.05, 0.05 )
  comp_plots['down_patrec_kalman_pxpy'] = ROOT.TH2F( \
                            "down_patrec_kalman_pxpy", "XY Momenta Residual", \
                                               100, -5.0, 5.0, 100, -5.0, 5.0 )

  station_plots = {}

  for tracker in [ 0, 1 ] :
    for station in [ 1, 2, 3, 4, 5 ] :
      dir_name = str(tracker) + "." + str(station)
      station_plots[dir_name] = {}

      station_plots[dir_name]['spacepoints'] = ROOT.TH2F(
                                                     dir_name+'_spacepoints', \
              "Spacepoints: Tracker "+str(tracker)+", Station "+str(station), \
                                       200, -200.0, 200.0, 200, -200.0, 200.0 )
      station_plots[dir_name]['spacepoint_road'] = ROOT.TH2F(
                                                 dir_name+'_spacepoint_road', \
                              "Spacepoint distance from fit: "+str(dir_name), \
                                           200, -10.0, 10.0, 200, -10.0, 10.0 )
      station_plots[dir_name]['2_hit_spacepoints'] = ROOT.TH2F(
                                               dir_name+'_2_hit_spacepoints', \
    "2-Cluster Spacepoints: Tracker "+str(tracker)+", Station "+str(station), \
                                       200, -200.0, 200.0, 200, -200.0, 200.0 )
      station_plots[dir_name]['3_hit_spacepoints'] = ROOT.TH2F(
                                               dir_name+'_3_hit_spacepoints', \
    "3-Cluster Spacepoints: Tracker "+str(tracker)+", Station "+str(station), \
                                       200, -200.0, 200.0, 200, -200.0, 200.0 )
      station_plots[dir_name]['kuno_plot'] = ROOT.TH1F( dir_name+'_kuno', \
                                      "Kuno Plot: "+dir_name, 1001, -0.25, 500.25 )


  plane_plots = {}

  for tracker in [ 0, 1 ] :
    for station in [ 1, 2, 3, 4, 5 ] :
      for plane in [ 0, 1, 2 ] :
        dir_name = str(tracker) + "." + str(station) + "." + str(plane)
        plane_plots[dir_name] = {}

        for com in ['xy', 'pxpy', 'xpx', 'ypy' ] :
          plane_plots[dir_name][com] = ROOT.TH2F(dir_name+'_'+com, \
                 "Phase Space : "+com, 200, -200.0, 200.0, 200, -200.0, 200.0 )
        plane_plots[dir_name]['xmx'] = ROOT.TH2F(dir_name+'_xmx', \
                   "Phase Space : "+'xmx', 200, -200.0, 200.0, 200, -1.0, 1.0 )
        plane_plots[dir_name]['ymy'] = ROOT.TH2F(dir_name+'_ymy', \
                   "Phase Space : "+'ymy', 200, -200.0, 200.0, 200, -1.0, 1.0 )
        plane_plots[dir_name]['mxmy'] = ROOT.TH2F(dir_name+'_mxmy', \
                      "Phase Space : "+'mxmy', 200, -1.0, 1.0, 200, -1.0, 1.0 )

        plane_plots[dir_name]['channels_cluster'] = ROOT.TH1F( \
          dir_name+'_channels_cluster', "Channels per Cluster", 50, 0.0, 50.0 )
        plane_plots[dir_name]['npe_cluster'] = ROOT.TH1F( \
                    dir_name+'_npe_cluster', "NPE per Cluster", 75, 0.0, 75.0 )

        plane_plots[dir_name]['cluster_occup'] = ROOT.TH1F( \
                                                   dir_name+'_cluster_occup', \
                               "Cluster Occupancy per Plane", 500, 0.0, 250.0 )
        plane_plots[dir_name]['cluster_positions'] = ROOT.TH1F( \
                                               dir_name+'_cluster_positions', \
                           "Cluster Positions per Plane", 400, -200.0, 200.0 )
        plane_plots[dir_name]['single_cluster_positions'] = ROOT.TH1F( \
                                        dir_name+'_single_cluster_positions', \
                    "Single Cluster Positions per Plane", 400, -200.0, 200.0 )
        plane_plots[dir_name]['double_cluster_positions'] = ROOT.TH1F( \
                                        dir_name+'_double_cluster_positions', \
                    "Double Cluster Positions per Plane", 400, -200.0, 200.0 )

        plane_plots[dir_name]['channel_occup'] = ROOT.TH1F( \
                                                   dir_name+'_channel_occup', \
                               "Channel Occupancy per Plane", 250, 0.0, 250.0 )

        plane_plots[dir_name]['npe_channel'] = ROOT.TH2F(\
                       dir_name+'_npe_channel', "NPE Per Channel: "+dir_name, \
                                             250, 0.0, 250.0, 75, 0.0, 75.0 )
        plane_plots[dir_name]['adc_channel'] = ROOT.TH2F(\
                       dir_name+'_adc_channel', "ADC Per Channel: "+dir_name, \
                                             250, 0.0, 250.0, 256, 0.0, 256.0 )


  plot_dict['track_plots'] = track_plots
  plot_dict['recon_plots'] = reco_plots
  plot_dict['patrec_plots'] = patrec_plots
  plot_dict['comparison_plots'] = comp_plots
  plot_dict['plane_plots'] = plane_plots
  plot_dict['station_plots'] = station_plots


  data_dict = {}
  data_dict['counters'] = {}
  data_dict['analysis'] = {}

  data_dict['counters']['N_zero_clusters'] = 0
  data_dict['counters']['N_fat_clusters'] = 0
  data_dict['counters']['N_events'] = 0
  data_dict['counters']['N_tracks'] = 0
  data_dict['counters']['N_tracks_up'] = 0
  data_dict['counters']['N_tracks_down'] = 0
  data_dict['counters']['N_track_pairs'] = 0


  return plot_dict, data_dict


def cut_tof_event( plot_dict, event ) :
  """
    Examine a TOF event to see if it should be vetoed
  """
  event_spacepoints = event.GetTOFEventSpacePoint()

  tof0_sp_size = event_spacepoints.GetTOF0SpacePointArraySize()
  tof1_sp_size = event_spacepoints.GetTOF1SpacePointArraySize()
  tof2_sp_size = event_spacepoints.GetTOF2SpacePointArraySize()

  if tof0_sp_size < 1 or tof1_sp_size < 1 or tof2_sp_size < 1 :
    return True

  tof0_sp = event_spacepoints.GetTOF0SpacePointArrayElement(0)
  tof1_sp = event_spacepoints.GetTOF1SpacePointArrayElement(0)
  tof2_sp = event_spacepoints.GetTOF2SpacePointArrayElement(0)

  if tof1_sp_size != 1 or tof2_sp_size != 1 :
    return True

  diff_0_1 = tof1_sp.GetTime() - tof0_sp.GetTime()
  diff_1_2 = tof2_sp.GetTime() - tof1_sp.GetTime()

  plot_dict['recon_plots']['tof_0_1'].Fill( diff_0_1 )
  plot_dict['recon_plots']['tof_1_2'].Fill( diff_1_2 )

  if diff_1_2 < TOF_CUT_LOW or diff_1_2 > TOF_CUT_HIGH :
    return True

  plot_dict['recon_plots']['tof_0_1_cut'].Fill(\
                                        tof1_sp.GetTime() - tof0_sp.GetTime() )
  plot_dict['recon_plots']['tof_1_2_cut'].Fill(\
                                        tof2_sp.GetTime() - tof1_sp.GetTime() )

  return False

 
def cut_scifi_event( plot_dict, event ) :
  """
    Examine a SciFi Event to see if it should be vetoed
  """
  digits = event.digits()
  saturation_counter = 0

  for digit in digits :
    if digit.get_adc() == 255 :
      saturation_counter += 1

  plot_dict['recon_plots']['saturations_event'].Fill( saturation_counter )
  if saturation_counter > 1000 :
    return True
  else :
    return False


def fill_plots_data(plot_dict, data_dict, event) :
  """
    Fill the plots in the plot dictionary with data
  """
  scifi_digits = event.digits()
  if not scifi_digits :
    return 

  scifi_clusters = event.clusters()
  scifi_spacepoints = event.spacepoints()
  fill_plots_recon(plot_dict, data_dict, scifi_digits, \
                                            scifi_clusters, scifi_spacepoints )

  scifi_helicals = event.helicalprtracks()
  scifi_straights = event.straightprtracks()
  fill_plots_patrec(plot_dict, data_dict, scifi_helicals, scifi_straights )

  scifi_tracks = event.scifitracks()
  fill_plots_tracks(plot_dict, data_dict, scifi_tracks)


def fill_plots_recon(plot_dict, data_dict, digits, clusters, spacepoints) :
  """
    Fill the Recon specific plots
  """
  plane_plots = plot_dict['plane_plots']
  station_plots = plot_dict['station_plots']
  reco_plots = plot_dict['recon_plots']

  for digit in digits :
    tracker = digit.get_tracker()
    station = digit.get_station()
    plane = digit.get_plane()

    dir_name = str(tracker)+'.'+str(station)+'.'+str(plane)
    plane_plots[dir_name]['channel_occup'].Fill( digit.get_channel() )
    reco_plots['adc'].Fill( digit.get_adc() )
    plane_plots[dir_name]['adc_channel'].Fill( digit.get_channel(), \
                                                              digit.get_adc() )
    if digit.get_adc() != 255 :
      reco_plots['npe'].Fill( digit.get_npe() )
      plane_plots[dir_name]['npe_channel'].Fill( digit.get_channel(), \
                                                              digit.get_npe() )

  kuno_sums = {}
  for tracker in [ 0, 1 ] :
    kuno_sums_stations = {}
    for station in [ 1, 2, 3, 4, 5 ] :
      kuno_sums_stations[ station ] = 0.0
    kuno_sums[tracker] = kuno_sums_stations


  for cluster in clusters :
    tracker = cluster.get_tracker()
    station = cluster.get_station()
    plane = cluster.get_plane()
    dir_name = str(tracker) + "." + str(station) + "." + str(plane)
    kuno_sums[tracker][station] += cluster.get_channel()

    plane_plots[dir_name]['cluster_occup'].Fill( cluster.get_channel() + 0.1 )
    plane_plots[dir_name]['cluster_positions'].Fill( cluster.get_alpha() )

    plane_id = ( 1 + plane + ( station - 1) * 3 ) * \
                                              ( -1.0 if tracker == 0 else 1.0 )
    clus_digits = cluster.get_digits_pointers()
    reco_plots['clusters_plane'].Fill(plane_id)
    reco_plots['channels_cluster'].Fill( len(clus_digits) )
    plane_plots[dir_name]['channels_cluster'].Fill( len(clus_digits) )

    if len( clus_digits ) == 0 :
      data_dict['counters']['N_zero_clusters'] += 1
    elif len( clus_digits ) == 1 :
      plane_plots[dir_name]['single_cluster_positions'].Fill(\
                                                          cluster.get_alpha() )
    elif len( clus_digits ) == 2 :
      plane_plots[dir_name]['double_cluster_positions'].Fill(\
                                                          cluster.get_alpha() )
    else :
      data_dict['counters']['N_fat_clusters'] += 1


    for digit in clus_digits :
      if digit.get_adc() == 255 :
        break
    else :
      reco_plots['npe_cluster'].Fill( cluster.get_npe() )
      plane_plots[dir_name]['npe_cluster'].Fill( cluster.get_npe() )


  for tracker in [ 0, 1 ] :
    for station in [ 1, 2, 3, 4, 5 ] :
      dir_name = str(tracker)+'.'+str(station)
      station_plots[dir_name]['kuno_plot'].Fill( kuno_sums[tracker][station] )


  for spacepoint in spacepoints :
    tracker = spacepoint.get_tracker()
    station = spacepoint.get_station()

    station_id = station * ( -1.0 if tracker == 0 else 1.0 )
    reco_plots['spacepoints_station'].Fill(station_id)
    reco_plots['spacepoints'].Fill( spacepoint.get_position().x(), \
                                                spacepoint.get_position().y() )

    dir_name = str(tracker)+'.'+str(station)
    station_plots[dir_name]['spacepoints'].Fill( \
                 spacepoint.get_position().x(), spacepoint.get_position().y() )

    if len( spacepoint.get_channels_pointers() ) == 2 :
      reco_plots['2_hit_spacepoints_station'].Fill( station_id )
      station_plots[dir_name]['2_hit_spacepoints'].Fill( \
                 spacepoint.get_position().x(), spacepoint.get_position().y() )
    elif len( spacepoint.get_channels_pointers() ) == 3 :
      reco_plots['3_hit_spacepoints_station'].Fill( station_id )
      station_plots[dir_name]['3_hit_spacepoints'].Fill( \
                 spacepoint.get_position().x(), spacepoint.get_position().y() )
    else :
      raise ValueError( "Unknown Number of spacepoints: " + \
                               str( len(spacepoint.get_channels_pointers()) ) )


  return True


def fill_plots_patrec(plot_dict, data_dict, helicals, straights) :
  """
    Fille the Pattern Recognition specific plots
  """
  patrec_plots = plot_dict['patrec_plots']

  for tracklist in [ helicals, straights ] :
    for track in tracklist :
      tracker = track.get_tracker()
      if track.get_type() == 0 :
        patrec_plots['chi_squared_x'].Fill( track.get_x_chisq() )
        patrec_plots['chi_squared_y'].Fill( track.get_y_chisq() )
        patrec_plots['chi_squared_straight'].Fill( track.get_chi_squared() )
        patrec_plots['chi_squared_straight_ndf'].Fill( \
                                    track.get_chi_squared() / track.get_ndf() )
        straight_road_plot(plot_dict, track)
      elif track.get_type() == 1 :
        patrec_plots['chi_squared_sz'].Fill( track.get_line_sz_chisq() )
        patrec_plots['chi_squared_circle'].Fill( track.get_circle_chisq() )
        patrec_plots['chi_squared_helical'].Fill( track.get_chi_squared() )
        patrec_plots['chi_squared_helical_ndf'].Fill( \
                                    track.get_chi_squared() / track.get_ndf() )

      prefix = ""
      if tracker == 0 :
        prefix = 'up_'
      elif tracker == 1 :
        prefix = 'down_'
      pr_pos = track.get_reference_position()
      pr_mom = track.get_reference_momentum()
      if tracker == 0 :
        pr_mom.setX( -pr_mom.x() )
        pr_mom.setY( -pr_mom.y() )

      patrec_plots[prefix+'patrec_xy'].Fill( pr_pos.x(), pr_pos.y() )
      patrec_plots[prefix+'patrec_mxmy'].Fill( pr_mom.x() / pr_mom.z(), \
                                                      pr_mom.y() / pr_mom.z() )
      patrec_plots[prefix+'patrec_pxpy'].Fill( pr_mom.x(), pr_mom.y() )


def straight_road_plot(plot_dict, track) :
  """
    Do the calculation to examine the straight tracks road cut
  """
  station_plots = plot_dict['station_plots']

  spacepoints = track.get_spacepoints_pointers()

  pos = track.get_reference_position()
  mom = track.get_reference_momentum()
  mom.SetX( mom.x()/mom.z() )
  mom.SetY( mom.y()/mom.z() )

  tracker = track.get_tracker()

  for sp in spacepoints :
    station = sp.get_station()
  
    sp_pos = sp.get_position()
    diff_z = sp_pos.z()

    ex_x = pos.x() + diff_z * mom.x()
    ex_y = pos.y() + diff_z * mom.y()

    if tracker == 0 :
      ex_x = -ex_x

    diff_x = ex_x - sp_pos.x()
    diff_y = ex_y - sp_pos.y()

    stat_id = str(tracker)+'.'+str(station)
    station_plots[stat_id]['spacepoint_road'].Fill(diff_x, diff_y)


def fill_plots_tracks(plot_dict, data_dict, tracks) :
  """
    Fill the track specific plots
  """
  track_plots = plot_dict['track_plots']
  reco_plots = plot_dict['recon_plots']
  comp_plots = plot_dict['comparison_plots']

  track_plots['tracks_event'].Fill( len(tracks) )
  upstream_good = 0
  downstream_good = 0
  straight_counter = 0
  helical_counter = 0

  for track in tracks :
    trackpoints = track.scifitrackpoints()

    if track.P_value() < P_VALUE_CUT : 
      continue

    count_trackpoints = 0
    for tp in trackpoints :
      if tp.has_data() :
        count_trackpoints += 1
      elif tools.calculate_plane_id( tp.tracker(), tp.station(), tp.plane() ) in IGNORE_PLANES :
        count_trackpoints += 1
    if count_trackpoints < MIN_NUM_TRACKPOINTS :
      continue

    tracker = track.tracker()
    track_plots['tracks_tracker'].Fill( track.tracker() )

    shape = track.GetAlgorithmUsed()
    shape_plots = None
    pr_track = None

    if shape == 0 :
      shape_plots = track_plots['straight']
      pr_track = track.pr_track_pointer_straight()
      straight_counter += 1
    elif shape == 1 :
      shape_plots = track_plots['helical']
      pr_track = track.pr_track_pointer_helical()
      helical_counter += 1

    data_dict['counters']['N_tracks'] += 1 

    shape_plots['tracks_tracker'].Fill( track.tracker() )
    shape_plots['chi_squared'].Fill( track.chi2() )
    shape_plots['chi_squared_ndf'].Fill( track.chi2() / track.ndf() )
    shape_plots['p_value'].Fill( track.P_value() )
    shape_plots['ndf'].Fill( track.ndf() )

    if tracker == 0 :
      shape_plots['chi_squared_up'].Fill( track.chi2() )
      shape_plots['chi_squared_ndf_up'].Fill( track.chi2() / track.ndf() )
      shape_plots['p_value_up'].Fill( track.P_value() )
      shape_plots['ndf_up'].Fill( track.ndf() )
      data_dict['counters']['N_tracks_up'] += 1 
      upstream_good += 1
    elif tracker == 1 :
      shape_plots['chi_squared_down'].Fill( track.chi2() )
      shape_plots['chi_squared_ndf_down'].Fill( track.chi2() / track.ndf() )
      shape_plots['p_value_down'].Fill( track.P_value() )
      shape_plots['ndf_down'].Fill( track.ndf() )
      data_dict['counters']['N_tracks_down'] += 1 
      downstream_good += 1
    
    pr_track = None

    if track.GetAlgorithmUsed() == 0 :
      pr_track = track.pr_track_pointer_straight()
    elif track.GetAlgorithmUsed() == 1 :
      pr_track = track.pr_track_pointer_helical()

    for tp in trackpoints :

      station = tp.station()
      plane = tp.plane()
      dir_name = str(tracker) + "." + str(station) + "." + str(plane)
      plane_plots = plot_dict['plane_plots']

      plane_id = ( 1 + plane + ( station - 1) * 3 ) * \
                                              ( -1.0 if tracker == 0 else 1.0 )
      reco_plots['plane_hits'].Fill( plane_id )

      pull = tp.pull()
      residual = tp.residual()
      s_residual = tp.smoothed_residual()

      shape_plots['plane_pulls'].Fill( plane_id, pull )
      shape_plots['plane_residuals'].Fill( plane_id, residual )
      shape_plots['plane_s_residuals'].Fill( plane_id, s_residual )

      pos = tp.pos()
      mom = tp.mom()
      plane_plots[dir_name]['xy'].Fill( pos.x(), pos.y() )
      plane_plots[dir_name]['pxpy'].Fill( mom.x(), mom.y() )
      plane_plots[dir_name]['xpx'].Fill( pos.x(), mom.x() )
      plane_plots[dir_name]['ypy'].Fill( pos.y(), mom.y() )
      plane_plots[dir_name]['mxmy'].Fill( mom.x() / mom.z(), mom.y() / mom.z() )
      plane_plots[dir_name]['xmx'].Fill( pos.x(), mom.x() / mom.z() )
      plane_plots[dir_name]['ymy'].Fill( pos.y(), mom.y() / mom.z() )

      if station == REFERENCE_STATION and plane == REFERENCE_PLANE :
        prefix = ""
        if tracker == 0 :
          prefix = 'up_'
        elif tracker == 1 :
          prefix = 'down_'
        pr_pos = pr_track.get_reference_position()
        pr_mom = pr_track.get_reference_momentum()

        diff_pos = [ pos.x() - pr_pos.x(), pos.y() - pr_pos.y() ]
        diff_mom = [ mom.x() - pr_mom.x(), mom.y() - pr_mom.y() ]
 
        comp_plots[prefix+'patrec_kalman_xy'].Fill( \
                                                     diff_pos[0], diff_pos[1] )
        comp_plots[prefix+'patrec_kalman_mxmy'].Fill( \
                                 diff_mom[0] / mom.z(), diff_mom[1] / mom.z() )
        comp_plots[prefix+'patrec_kalman_pxpy'].Fill( \
                                                     diff_mom[0], diff_mom[1] )


    if upstream_good == 1 and downstream_good == 1 :
      data_dict['counters']['N_track_pairs'] += 1
    track_plots['trackpoints_track'].Fill( count_trackpoints )
    shape_plots['trackpoints_track'].Fill( count_trackpoints )

  track_plots['straight']['tracks_event'].Fill(straight_counter)
  track_plots['helical']['tracks_event'].Fill(helical_counter)



def analyse_plots(plot_dict, data_dict) :
  """
    Perform some analysis on the plots and data stored in the repective
    dictionaries
  """

  track_plots = plot_dict['track_plots']

  print "Analysing Data"
  print
  print "No. of Files                   = ", data_dict['counters']['N_files']
  print "No. of Spills                  = ", data_dict['counters']['N_spills']
  print "No. of Events                  = ", data_dict['counters']['N_events']
  print
  print "No. of Tracks                  = ", data_dict['counters']['N_tracks']
  print "No. of Upstream Tracks         = ", \
                                           data_dict['counters']['N_tracks_up']
  print "No. of Downstream Tracks       = ", \
                                         data_dict['counters']['N_tracks_down']
  print "No. of Track Pairs             = ", \
                                         data_dict['counters']['N_track_pairs']
  print
  print "Mean No. Tracks per Event      = ", \
                                          track_plots['tracks_event'].GetMean()
  print "Mean No. Trackpoints per Track = ", \
                                     track_plots['trackpoints_track'].GetMean()
  print
  print "No. Events with 0 Tracks       = ", \
                                   track_plots['tracks_event'].GetBinContent(1)
  print "No. Events with 1 Tracks       = ", \
                                   track_plots['tracks_event'].GetBinContent(2)
  print "No. Events with 2 Tracks       = ", \
                                   track_plots['tracks_event'].GetBinContent(3)
  print
  print "  Straight Tracks :"
  print
  print "Mean Smoothed Residual         = ", \
                        track_plots['straight']['plane_s_residuals'].GetMean(2)
  print "RMS  Smoothed Residual         = ", \
                         track_plots['straight']['plane_s_residuals'].GetRMS(2)

  print
  print "  Helical Tracks :"
  print
  print "Mean Smoothed Residual         = ", \
                         track_plots['helical']['plane_s_residuals'].GetMean(2)
  print "RMS  Smoothed Residual         = ", \
                          track_plots['helical']['plane_s_residuals'].GetRMS(2)

def save_plots(plot_dict, filename) :
  """
    Save all the plots to file. Assumes a directory like strucutre of plots to
    recursively save them.
  """
  outfile = ROOT.TFile(filename, "RECREATE")

  for key in sorted(plot_dict) :
    if type( plot_dict[key] ) is types.DictType :
      directory = outfile.mkdir( key )
      directory.cd()
      save_plot( plot_dict[key], directory )
      outfile.cd()
    else :
      plot_dict[key].Write()
      
  outfile.Close()


def save_plot(plot_dict, outfile) :
  """
    The recursive saving function for the plot dictionary
  """
  for key in sorted(plot_dict) :
    if type( plot_dict[key] ) is types.DictType :
      directory = outfile.mkdir( key )
      directory.cd()
      save_plot( plot_dict[key], directory )
      outfile.cd()
    else :
      plot_dict[key].Write()
      

def print_plots(plot_dict, location) :
  """
    Print the plots to PDF files rather than a root file. Assumes a recursive
    structure of directories and plots to save then to file
  """
  if not os.path.exists( location ) :
    os.makedirs( location )

  for key in sorted(plot_dict) :
    if type( plot_dict[key] ) is types.DictType :
      new_location = os.path.join( location, key )
      print_plots( plot_dict[key], new_location )
      
    else :
      canvas = ROOT.TCanvas( key+'_canvas' )
      plot_dict[key].Draw()
      if key in PLOT_OPTIONS :
        apply_options( canvas, plot_dict[key], PLOT_OPTIONS[key] )
      canvas.SaveAs( os.path.join(location, key ) + ".pdf", "pdf" )


def apply_options( canvas, hist, options ) :
  """
    Change some simple plot options using a dictionary of options.
  """
  for opt in options :
    if opt == "logy" :
      canvas.SetLogy()
    elif opt == "logz" :
      canvas.SetLogz()
    elif opt == "colz" :
      canvas.SetDrawOption( 'colz' )
      hist.Draw("colz")
      canvas.Update()
    else :
      print "ERROR : Unknown Canvas Option:", opt


if __name__ == "__main__" : 
  ROOT.gROOT.SetBatch( True )

  parser = argparse.ArgumentParser( description='Performs a quick ' +\
                'verification routine for the tracks storing the data files.' )

  parser.add_argument( 'maus_root_files', nargs='+', help='List of MAUS '+\
      'output root files containing reconstructed straight tracks')

  parser.add_argument( '-N', '--max_num_events', type=int, \
                                   help='Maximum number of events to analyse.')

  parser.add_argument( '-O', '--output_filename', \
             default='track_verification', help='Set the output filename')

  parser.add_argument( '-P', '--print_plots', action='store_true', \
                        default=False, help='Set flag to print plots to file' )

  parser.add_argument( '-D', '--output_directory', \
             default='./', help='Set the output directory for plots')

  parser.add_argument( '-T', '--cut_tof', action='store_true', \
                                           help='Set flag to enable TOF Cuts' )
  parser.add_argument( '-S', '--cut_scifi', action='store_true', \
                                   help='Set flag to enable SciFi Event Cuts' )

  parser.add_argument( '--tof_cut_low', type=float, default=0.0, \
                                                help='Lower limit of TOF cut' )
  parser.add_argument( '--tof_cut_high', type=float, default=100.0, \
                                                help='Upper limit of TOF cut' )

  parser.add_argument( '--p_value_cut', type=float, default=0.0, \
                         help='Cut on P-Values less than the specified value' )
  parser.add_argument( '--gradient_cut', type=float, default=1.0, \
                                     help='Set the cut on the track gradient' )
  parser.add_argument( '--number_trackpoints_cut', type=int, default=0, \
                         help='Cut on the Number of Trackpoints in the track' )

  try :
    namespace = parser.parse_args()

    TOF_CUT_LOW = namespace.tof_cut_low
    TOF_CUT_HIGH = namespace.tof_cut_high

    P_VALUE_CUT = namespace.p_value_cut

    GRAD_CUT = math.fabs( namespace.gradient_cut )

    MIN_NUM_TRACKPOINTS = namespace.number_trackpoints_cut

  except :
    raise
  else :
##### 1. Intialise plots ######################################################
    print "\nInitialising Plots"
    plot_dict, data_dict = init_plots_data()

##### 2. Load SciFi Events ####################################################
    print "\nLoading Spills...\n"
    file_reader = event_loader.maus_reader(namespace.maus_root_files)

    try :
      while file_reader.next_event() and \
               file_reader.get_total_num_events() != namespace.max_num_events :
        sys.stdout.write( 
            ' File ' + str(file_reader.get_current_filenumber()) + \
            ' of ' + str(file_reader.get_number_files()) + '             \r')
        sys.stdout.flush()

        try :
          scifi_event = file_reader.get_event( 'scifi' )
          tof_event = file_reader.get_event( 'tof' )
          data_dict['counters']['N_events'] += 1

##### 3. Fill plots ###########################################################
          if namespace.cut_tof and cut_tof_event( plot_dict, tof_event ) :
            continue
          if namespace.cut_scifi and \
                                    cut_scifi_event( plot_dict, scifi_event ) :
            continue

          fill_plots_data(plot_dict, data_dict, scifi_event)

        except ValueError :
          print "An Error Occured. Skipping Spill: " + \
                str(file_reader.get_current_spill_number()) + \
                " In File: " + str(file_reader.get_current_filenumber()) + "\n"
          continue

##### 4. Analysis Plots #######################################################
    except KeyboardInterrupt :
      print
      print "Keyboard Interrupt"
      print
    data_dict['counters']['N_files'] = file_reader.get_current_filenumber()
    data_dict['counters']['N_spills'] = file_reader.get_total_num_spills()
    print "All Spills Loaded                                                  "
    print "\nStarting Analysis"
    analyse_plots(plot_dict, data_dict)

##### 5. Save plots and data ##################################################
    print "\nSaving Plots and Data"
    outdir = namespace.output_directory
    outfile = os.path.join( outdir, namespace.output_filename+".root" )
    save_plots(plot_dict, outfile)
    if namespace.print_plots :
      print_plots(plot_dict, outdir)


  print 
  print "Complete."
  print

