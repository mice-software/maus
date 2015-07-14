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


# Generic Python imports
import io 
import math
import sys
import os
import itertools
import gc
import copy
import argparse

# Third Party library import statements
import event_loader
import ROOT
import xboa
import numpy
import json
import array
import operator
import random


"""
  This is an example script to show how to read in some events and setup some
  analysis.


  Script Aglorithm :

  <Insert some useful description here>

  
"""


# Useful Constants and configuration
STRAIGHT_ALGORITHM_ID = 0
RECON_STATION = 1
RECON_PLANE = 0
MIN_NUMBER_TRACKPOINTS = 12


def init_plots() :
  plot_dict = {}

  plot_dict['upstream_xy'] = ROOT.TH2F('upstream_xy', \
           'Upstream Beam Position', 1000, -200.0, 200.0, 1000, -200.0, 200.0 )
  plot_dict['downstream_xy'] = ROOT.TH2F('downstream_xy', \
         'Downstream Beam Position', 1000, -200.0, 200.0, 1000, -200.0, 200.0 )


  plot_dict['upstream_mxmy'] = ROOT.TH2F('upstream_mxmy', \
                   'Upstream Beam Gradient', 1000, -1.0, 1.0, 1000, -1.0, 1.0 )
  plot_dict['downstream_mxmy'] = ROOT.TH2F('downstream_mxmy', \
                 'Downstream Beam Gradient', 1000, -1.0, 1.0, 1000, -1.0, 1.0 )


  plot_dict['tof_0_1'] = ROOT.TH1F( 'tof_0_1', 'Time TOF0 - TOF1', \
                                                             1000, 0.0, 100.0 )
  plot_dict['tof_1_2'] = ROOT.TH1F( 'tof_1_2', 'Time TOF1 - TOF2', \
                                                             1000, 0.0, 100.0 )

  return plot_dict


def find_straight_tracks(scifi_event) :
  scifi_tracks = scifi_event.scifitracks()
  upstream_tracks = []
  downstream_tracks = []
  for track in scifi_tracks :
    if track.GetAlgorithmUsed() != STRAIGHT_ALGORITHM_ID :
      continue

    if track.tracker() == 0 :
      upstream_tracks.append(track)
    elif track.tracker() == 1 :
      downstream_tracks.append(track)

# Only looking for single track events at present implementation
  track_list = []
  if len( upstream_tracks ) != 1 :
    return track_list
  if len( downstream_tracks) != 1 :
    return track_list

  track_list.append((upstream_tracks[0], downstream_tracks[0]))

  return track_list


def cut_scifi_event( event ) :
  digits = event.digits()
  saturation_counter = 0

  for digit in digits :
    if digit.get_adc() == 255 :
      saturation_counter += 1

  if saturation_counter > 1000 : return True

  return False



def cut_tof_event( plot_dict, event ) :
  event_spacepoints = event.GetTOFEventSpacePoint()

  tof0_sp_size = event_spacepoints.GetTOF0SpacePointArraySize()
  tof1_sp_size = event_spacepoints.GetTOF1SpacePointArraySize()
  tof2_sp_size = event_spacepoints.GetTOF2SpacePointArraySize()

  if tof0_sp_size < 1 or tof1_sp_size != 1 or tof2_sp_size != 1 :
    return True

  return False


def cut_tracks(up_trk, down_trk) :
  up_counter = 0
  down_counter = 0

  for tp in up_trk.scifitrackpoints() :
    if tp.has_data() : up_counter += 1
  for tp in down_trk.scifitrackpoints() :
    if tp.has_data() : down_counter += 1

  if up_counter < MIN_NUMBER_TRACKPOINTS :
    return True
  if down_counter < MIN_NUMBER_TRACKPOINTS :
    return True

  return False



def fill_plots_tof(plot_dict, tof_event) :
  event_spacepoints = tof_event.GetTOFEventSpacePoint()

  tof0_sp = event_spacepoints.GetTOF0SpacePointArrayElement(0)
  tof1_sp = event_spacepoints.GetTOF1SpacePointArrayElement(0)
  tof2_sp = event_spacepoints.GetTOF2SpacePointArrayElement(0)
    
  plot_dict['tof_0_1'].Fill( tof1_sp.GetTime() - tof0_sp.GetTime() )
  plot_dict['tof_1_2'].Fill( tof2_sp.GetTime() - tof1_sp.GetTime() )



def fill_plots_track(plot_dict, up_trk, down_trk) :
  up_z = None
  down_z = None

  up_pos = None
  up_gra = None
  down_pos = None
  down_gra = None

  for tp in up_trk.scifitrackpoints() :
    if tp.station() == RECON_STATION and tp.plane() == RECON_PLANE :
      up_z = tp.pos().z()
      up_pos = [ tp.pos().x(), tp.pos().y() ]
      up_gra = [ tp.mom().x()/tp.mom().z(), tp.mom().y()/tp.mom().z() ]
      break
  if up_z is None :
    raise ValueError("Could not find a trackpoint in Tracker 0, " + \
               "Station " + str(RECON_STATION) + ", Plane " + str(RECON_PLANE))

  for tp in down_trk.scifitrackpoints() :
    if tp.station() == RECON_STATION and tp.plane() == RECON_PLANE :
      down_z = tp.pos().z()
      down_pos = [ tp.pos().x(), tp.pos().y() ]
      down_gra = [ tp.mom().x()/tp.mom().z(), tp.mom().y()/tp.mom().z() ]
      break
  if down_z is None :
    raise ValueError("Could not find a trackpoint in Tracker 1, " + \
               "Station " + str(RECON_STATION) + ", Plane " + str(RECON_PLANE))


  plot_dict['upstream_xy'].Fill(up_pos[0], up_pos[1])
  plot_dict['upstream_mxmy'].Fill(up_gra[0], up_gra[1])

  plot_dict['downstream_xy'].Fill(down_pos[0], down_pos[1])
  plot_dict['downstream_mxmy'].Fill(down_gra[0], down_gra[1])




def analyse_plots(plot_dict) :
  print "Found", plot_dict['tof_1_2'].GetEntries(), "Events"

# MORE ANALYSIS CODE HERE



def save_plots(plot_dict, directory, filename, print_plots=False) :
  filename = os.path.join(directory, filename)
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
      plot_dict[plot].Write()
    outfile.Close()





if __name__ == "__main__" : 
  ROOT.gROOT.SetBatch( True )

  parser = argparse.ArgumentParser( description='An example script showing '+\
      'some basic data extraction and analysis routines' )

  parser.add_argument( 'maus_root_files', nargs='+', help='List of MAUS '+\
                  'output root files containing reconstructed straight tracks')

  parser.add_argument( '-N', '--max_num_events', type=int, \
                                   help='Maximum number of events to analyse.')

  parser.add_argument( '-O', '--output_filename', \
               default='example_analysis.root', help='Set the output filename')

  parser.add_argument( '-D', '--output_directory', \
                                 default='./', help='Set the output directory')

  parser.add_argument( '-P', '--print_plots', action='store_true', \
                        help="Flag to save the plots as individual pdf files" )

#  parser.add_argument( '-C', '--configuration_file', help='Configuration '+\
#      'file for the reconstruction. I need the geometry information' )

  try :
    namespace = parser.parse_args()
  except :
    pass
  else :
##### 1. Load MAUS globals and geometry. - NOT NECESSARY AT PRESENT
    # geom = load_tracker_geometry(namespace.configuration_file)

##### 2. Intialise plots ######################################################
    print "\nInitialising Plots"
    plot_dict = init_plots()

##### 3. Load Events ##########################################################
    print "\nLoading Spills...\n"
    file_reader = event_loader.maus_reader(namespace.maus_root_files)

    try :
      while file_reader.next_event() and \
               file_reader.get_total_num_events() != namespace.max_num_events :
        try :
          sys.stdout.write( 
              ' Spill ' + str(file_reader.get_current_spill_number()) + \
              ' of ' + str(file_reader.get_current_number_spills()) + \
              ' in File ' + str(file_reader.get_current_filenumber()) + \
              ' of ' + str(file_reader.get_number_files()) + '             \r')
          sys.stdout.flush()

          scifi_event = file_reader.get_event( 'scifi' )
          tof_event = file_reader.get_event( 'tof' )

##### 4. Extract potential tracks #############################################
          straights = find_straight_tracks(scifi_event)

##### 5. Apply Cuts ###########################################################
          if cut_scifi_event(scifi_event) :
            continue

          if cut_tof_event(plot_dict, tof_event) :
            continue
          
          for up_str, down_str in straights :
            if cut_tracks(up_str, down_str) :
              break

##### 6. Fill plots ###########################################################
            else :
              fill_plots_track(plot_dict, up_str, down_str)
              fill_plots_tof(plot_dict, tof_event)

        except ValueError :
          print "An Error Occured. Skipping Spill: " + \
                str(file_reader.get_current_spill_number()) + \
                " In File: " + str(file_reader.get_current_filenumber()) + "\n"
          continue

##### 7. Analysis Plots #######################################################
    except KeyboardInterrupt :
      pass
    print "All Spills Loaded                                                  "
    print "\nStarting Analysis"
    data_dict = analyse_plots(plot_dict)

##### 8. Save plots and data ##################################################
    print "\nSaving Plots and Data"
    save_plots(plot_dict, namespace.output_directory, \
                              namespace.output_filename, namespace.print_plots)


  print 
  print "Complete."
  print

