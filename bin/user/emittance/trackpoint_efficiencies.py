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


import io   #  generic python library for I/O
import math
import sys
import os
import itertools
import gc
import copy
import argparse
import traceback

import MAUS
import ROOT
import xboa
import numpy
from array import array

import operator

# definitions of MAUS data structure for PyROOT
import libMausCpp #pylint: disable = W0611

import emittance_analysis
from emittance_analysis import covariances
from emittance_analysis import duplicate_print
from emittance_analysis import extraction
from emittance_analysis import plotter
from emittance_analysis import muon_mass
from emittance_analysis import stats
from emittance_analysis import common



####################################################################################################


## Other Stuff
associate_max_attempts = 100
associate_tolerance = 1.0


####################################################################################################





def run( maus_root_files, recon_station, recon_plane, cut_p_value, cut_pz, cut_pt, cut_pid, bin_pz, bin_pt, output_directory,\
      max_pz, min_pz, max_pt, min_pt, cut_straight_tracks, cut_residual, cut_radius, require_clean_tracks ) :


  virtual_plane_dict = []
  upstream_virtual = -1
  downstream_virtual = -1


####################################################################################################


## Histgrams Go Here!

  upstream_mc_pid = ROOT.TH1F( "upstream_mc_pid", "Particle ID Values for the upstream refernce plane", 100, -50.0, 50.0 )
  downstream_mc_pid = ROOT.TH1F( "downstream_mc_pid", "Particle ID Values for the downstream refernce plane", 100, -50.0, 50.0 )

  upstream_mc_count_pz = [ 0 for i in range( bin_pz ) ]
  upstream_recon_count_pz = [ 0 for i in range( bin_pz ) ]
  upstream_mismatched_count_pz = [ 0 for i in range( bin_pz ) ]
  upstream_recon_tracks_pz = [ 0 for i in range( bin_pz ) ]
  upstream_mc_tracks_pz = [ 0 for i in range( bin_pz ) ]

  downstream_mc_count_pz = [ 0 for i in range( bin_pz ) ]
  downstream_recon_count_pz = [ 0 for i in range( bin_pz ) ]
  downstream_mismatched_count_pz = [ 0 for i in range( bin_pz ) ]
  downstream_recon_tracks_pz = [ 0 for i in range( bin_pz ) ]
  downstream_mc_tracks_pz = [ 0 for i in range( bin_pz ) ]


  upstream_mc_count_pt = [ 0 for i in range( bin_pt ) ]
  upstream_recon_count_pt = [ 0 for i in range( bin_pt ) ]
  upstream_mismatched_count_pt = [ 0 for i in range( bin_pt ) ]
  upstream_recon_tracks_pt = [ 0 for i in range( bin_pt ) ]
  upstream_mc_tracks_pt = [ 0 for i in range( bin_pt ) ]

  downstream_mc_count_pt = [ 0 for i in range( bin_pt ) ]
  downstream_recon_count_pt = [ 0 for i in range( bin_pt ) ]
  downstream_mismatched_count_pt = [ 0 for i in range( bin_pt ) ]
  downstream_recon_tracks_pt = [ 0 for i in range( bin_pt ) ]
  downstream_mc_tracks_pt = [ 0 for i in range( bin_pt ) ]


  pz_binwidth = float( ( max_pz - min_pz ) / bin_pz )
  pt_binwidth = float( ( max_pt - min_pt ) / bin_pt )
  pz_bins = array( 'd' )
  pt_bins = array( 'd' )
  for i in range( bin_pz ) : pz_bins.append( min_pz + ( float( i ) + 0.5 )*pz_binwidth )
  for i in range( bin_pt ) : pt_bins.append( min_pt + ( float( i ) + 0.5 )*pt_binwidth )

  errors_pz = array( 'd' )
  errors_pt = array( 'd' )
  zeros_pz = array( 'd' )
  zeros_pt = array( 'd' )
  for i in range( bin_pz ) :
    errors_pz.append( pz_binwidth * 0.5 )
    zeros_pz.append( 0.0 )
  for i in range( bin_pt ) :
    errors_pt.append( pt_binwidth * 0.5 )
    zeros_pt.append( 0.0 )


####################################################################################################

  
  print "Locating Virtual Planes"
  print "Please Wait..."

  filename = maus_root_files[0]

  for i in range( 2 ) :
    stations = []
    for j in range( 5 ) :
      planes = []
      for k in range( 3 ) :
        planes.append( ( -1, associate_tolerance ) )
      stations.append( planes )
    virtual_plane_dict.append( stations )

  reader = extraction.maus_reader( filename )

  attempt_count = 0
  trackers_list = []
  while attempt_count < associate_max_attempts and reader.next_event() :

    scifi_event = reader.get_event( 'scifi' )
    mc_event = reader.get_event( 'mc' )

    for track in scifi_event.scifitracks() :
      if not track.tracker() in trackers_list :
        trackers_list.append( track.tracker() )


    if common.associate_virtuals( virtual_plane_dict, scifi_event.scifitracks(), mc_event.GetVirtualHits(), trackers=trackers_list ) is True :
      break
    attempt_count += 1

  else :
    if attempt_count >= associate_max_attempts :
      print
      print "ERROR Could not locate all virtual planes."
      print
    else:
      print
      print "ERROR Ran out of events to locate virtual planes."
      print
    sys.exit( 0 )

  print "Found Trackers:", trackers_list

  upstream_virtual = virtual_plane_dict[0][recon_station - 1][recon_plane][0]
  upstream_virtual_outer = virtual_plane_dict[0][4][2][0]
  downstream_virtual = virtual_plane_dict[1][recon_station - 1][recon_plane][0]
  downstream_virtual_outer = virtual_plane_dict[1][4][2][0]

  print
  gc.collect()


####################################################################################################

  file_reader = extraction.maus_reader( maus_root_files )

  number_files = file_reader.get_number_files()

  print "Loading", number_files, "File(s)"
  print

  scifi_extractor = extraction.scifi_tracks()
  scifi_extractor.add_reference_plane( 0, recon_station, recon_plane, 'upstream' )
  scifi_extractor.add_reference_plane( 1, recon_station, recon_plane, 'downstream' )
  scifi_extractor.add_cut( 'pvalue', cut_p_value, operator.lt )
  scifi_extractor.set_allow_straight_tracks( not cut_straight_tracks )
#  scifi_extractor.set_min_num_trackpoints( 5 ) # Not yet imlemented

  virtual_extractor = extraction.virtual_hits()
#  virtual_extractor.add_cut( 'pid', cut_pid, operator.ne )
  virtual_extractor.add_cut( 'r', cut_radius, operator.gt )
  if 0 in trackers_list :
    virtual_extractor.add_reference_plane( upstream_virtual, 'upstream' )
    virtual_extractor.add_reference_plane( upstream_virtual_outer, 'upstream_outer' )
  if 1 in trackers_list :
    virtual_extractor.add_reference_plane( downstream_virtual, 'downstream' )
    virtual_extractor.add_reference_plane( downstream_virtual_outer, 'downstream_outer' )

  while file_reader.next_event() :

    sys.stdout.write( ' Spill ' + str( file_reader.get_current_spill_number() ) + ' of ' + str( file_reader.get_current_number_spills() ) + ' in File ' + str( file_reader.get_current_filenumber() ) + ' of ' + str( file_reader.get_number_files() ) + '   \r' )
    sys.stdout.flush()


    scifi_event = file_reader.get_event( 'scifi' )
    mc_event = file_reader.get_event( 'mc' )
    
    virtual_extractor.load_tracks( mc_event )
    scifi_extractor.load_tracks( scifi_event )

    contain_up = True
    contain_down = True
    if 0 in trackers_list :
      contain_up = is_contained( virtual_extractor.get_current_trackpoints(), upstream_virtual_outer, upstream_virtual, cut_radius )
    if 1 in trackers_list :
      contain_down = is_contained( virtual_extractor.get_current_trackpoints(), downstream_virtual, downstream_virtual_outer, cut_radius )

    # Check to see if any events fail the cuts, if the user wants clean tracks only
#    if require_clean_tracks and ( scifi_extractor.is_cut() or virtual_extractor.is_cut() ) :
    if virtual_extractor.is_cut() and ( not contain_up ) and ( not contain_down ) : continue

    upstream_mc_tracks = 0
    downstream_mc_tracks = 0
    upstream_recon_tracks = 0
    downstream_recon_tracks = 0

    upstream_pz_bin = 0
    upstream_pt_bin = 0
    downstream_pz_bin = 0
    downstream_pt_bin = 0



    for hit in virtual_extractor.get_reference_trackpoints() :
      if hit.get_reference() == "upstream" :
        upstream_mc_pid.Fill( hit.get_pid() )
        if hit.get_pid() == cut_pid :
          upstream_pz_bin = get_bin_num( hit.get_pz(), min_pz, max_pz, bin_pz )
          upstream_pt_bin = get_bin_num( hit.get_pt(), min_pt, max_pt, bin_pt )
#          print
#          print hit.get_pz(), hit.get_pt(), upstream_pz_bin, upstream_pt_bin
          upstream_mc_tracks += 1
      if hit.get_reference() == "downstream" :
        downstream_mc_pid.Fill( hit.get_pid() )
        if hit.get_pid() == cut_pid :
          downstream_pz_bin = get_bin_num( hit.get_pz(), min_pz, max_pz, bin_pz )
          downstream_pt_bin = get_bin_num( hit.get_pt(), min_pt, max_pt, bin_pt )
#          print
#          print hit.get_pz(), hit.get_pt(), downstream_pz_bin, downstream_pt_bin
          downstream_mc_tracks += 1

    
####################################################################################################

# Find the upstream and downstream MC trackpoints

    mc_trackpoints_raw = virtual_extractor.get_current_trackpoints()

    upstream_mc_trackpoints = []
    downstream_mc_trackpoints = []
    for loc, point in mc_trackpoints_raw :
      for j in range( 5 ) :
        for k in range( 3 ) :
          if virtual_plane_dict[0][j][k][0] == loc :
            upstream_mc_trackpoints.append( point )
      for j in range( 5 ) :
        for k in range( 3 ) :
          if virtual_plane_dict[1][j][k][0] == loc :
            downstream_mc_trackpoints.append( point )


####################################################################################################

# Find the upstream and downstream reconstructed trackpoints

    scifi_trackpoints_raw = scifi_extractor.get_current_trackpoints()

    upstream_scifi_trackpoints = []
    downstream_scifi_trackpoints = []

    for loc, point in scifi_trackpoints_raw :
      tracker, station, plane = loc
      if tracker == 0 : upstream_scifi_trackpoints.append( point )
      elif tracker == 1 : downstream_scifi_trackpoints.append( point )


####################################################################################################

# See how the MC and reconstructed trackpoints match up!
    upstream_pairs = []
    downstream_pairs = []

    if cut_residual is None :
      upstream_pairs = extraction.make_virt_recon_pairs( upstream_mc_trackpoints, upstream_scifi_trackpoints, check_pid = True, check_length = False )
    else :
      upstream_pairs = extraction.make_virt_recon_pairs( upstream_mc_trackpoints, upstream_scifi_trackpoints, check_pid = True, check_length = False, max_separation = cut_residual )

    if upstream_pz_bin >= 0 :
      upstream_recon_count_pz[ upstream_pz_bin ] += len( upstream_pairs )
      upstream_mismatched_count_pz[ upstream_pz_bin ] += abs( len( upstream_pairs ) - len( upstream_scifi_trackpoints ) )
      upstream_mc_count_pz[ upstream_pz_bin ] += len( upstream_mc_trackpoints )
      if len( upstream_pairs ) > 0 : upstream_recon_tracks_pz[ upstream_pz_bin ] += 1
      upstream_mc_tracks_pz[ upstream_pz_bin ] += 1
    if upstream_pt_bin >= 0 :
      upstream_recon_count_pt[ upstream_pt_bin ] += len( upstream_pairs )
      upstream_mismatched_count_pt[ upstream_pt_bin ] += abs( len( upstream_pairs ) - len( upstream_scifi_trackpoints ) )
      upstream_mc_count_pt[ upstream_pt_bin ] += len( upstream_mc_trackpoints )
      if len( upstream_pairs ) > 0 : upstream_recon_tracks_pt[ upstream_pt_bin ] += 1
      upstream_mc_tracks_pt[ upstream_pt_bin ] += 1



    if cut_residual is None :
      downstream_pairs = extraction.make_virt_recon_pairs( downstream_mc_trackpoints, downstream_scifi_trackpoints, check_pid = True, check_length = False )
    else :
      downstream_pairs = extraction.make_virt_recon_pairs( downstream_mc_trackpoints, downstream_scifi_trackpoints, check_pid = True, check_length = False, max_separation = cut_residual )

    if downstream_pz_bin >= 0 :
      downstream_recon_count_pz[ downstream_pz_bin ] += len( downstream_pairs )
      downstream_mismatched_count_pz[ downstream_pz_bin ] += abs( len( downstream_pairs ) - len( downstream_scifi_trackpoints ) )
      downstream_mc_count_pz[ downstream_pz_bin ] += len( downstream_mc_trackpoints )
      if len( downstream_pairs ) > 0 : downstream_recon_tracks_pz[ downstream_pz_bin ] += 1
      downstream_mc_tracks_pz[ downstream_pz_bin ] += 1
    if downstream_pt_bin >= 0 :
      downstream_mismatched_count_pt[ downstream_pt_bin ] += abs( len( downstream_pairs ) - len( downstream_scifi_trackpoints ) )
      downstream_recon_count_pt[ downstream_pt_bin ] += len( downstream_pairs )
      downstream_mc_count_pt[ downstream_pt_bin ] += len( downstream_mc_trackpoints )
      if len( downstream_pairs ) > 0 : downstream_recon_tracks_pt[ downstream_pt_bin ] += 1
      downstream_mc_tracks_pt[ downstream_pt_bin ] += 1


#    print
#    print len( upstream_mc_trackpoints ), len( upstream_scifi_trackpoints ), len( upstream_pairs ), len( downstream_mc_trackpoints ), len( downstream_scifi_trackpoints ), len( downstream_pairs )
 
 
 

  gc.collect()

  sys.stdout.write( '\rAll ' + str( file_reader.get_number_files() ) + ' File(s) Loaded                            \n' )
  sys.stdout.flush()

  print
  print "Saving Results"
  print



####################################################################################################



  upstream_number_recon_pz = array( 'd' )
  upstream_number_recon_pt = array( 'd' )
  upstream_number_mc_pz = array( 'd' )
  upstream_number_mc_pt = array( 'd' )
  upstream_number_recon_tracks_pz = array( 'd' )
  upstream_number_recon_tracks_pt = array( 'd' )
  upstream_number_mc_tracks_pz = array( 'd' )
  upstream_number_mc_tracks_pt = array( 'd' )
  upstream_eff_pz = array( 'd' )
  upstream_eff_pz_err = array( 'd' )
  upstream_eff_pt = array( 'd' )
  upstream_eff_pt_err = array( 'd' )
  upstream_track_eff_pz = array( 'd' )
  upstream_track_eff_pz_err = array( 'd' )
  upstream_track_eff_pt = array( 'd' )
  upstream_track_eff_pt_err = array( 'd' )
  upstream_purity_pz = array( 'd' )
  upstream_purity_pz_err = array( 'd' )
  upstream_purity_pt = array( 'd' )
  upstream_purity_pt_err = array( 'd' )

  downstream_number_recon_pz = array( 'd' )
  downstream_number_recon_pt = array( 'd' )
  downstream_number_mc_pz = array( 'd' )
  downstream_number_mc_pt = array( 'd' )
  downstream_number_recon_tracks_pz = array( 'd' )
  downstream_number_recon_tracks_pt = array( 'd' )
  downstream_number_mc_tracks_pz = array( 'd' )
  downstream_number_mc_tracks_pt = array( 'd' )
  downstream_eff_pz = array( 'd' )
  downstream_eff_pz_err = array( 'd' )
  downstream_eff_pt = array( 'd' )
  downstream_eff_pt_err = array( 'd' )
  downstream_track_eff_pz = array( 'd' )
  downstream_track_eff_pz_err = array( 'd' )
  downstream_track_eff_pt = array( 'd' )
  downstream_track_eff_pt_err = array( 'd' )
  downstream_purity_pz = array( 'd' )
  downstream_purity_pz_err = array( 'd' )
  downstream_purity_pt = array( 'd' )
  downstream_purity_pt_err = array( 'd' )


#  print
#  print upstream_mc_count_pz
#  print upstream_recon_count_pz
#  print upstream_mismatched_count_pz
#  print
#  print upstream_mc_count_pt
#  print upstream_recon_count_pt
#  print upstream_mismatched_count_pt
#  print
#  print downstream_mc_count_pz
#  print downstream_recon_count_pz
#  print downstream_mismatched_count_pz
#  print
#  print downstream_mc_count_pt
#  print downstream_recon_count_pt
#  print downstream_mismatched_count_pt
#  print



  upstream_eff_tot = 0.0
  upstream_track_eff_tot = 0.0
  upstream_pur_tot = 0.0
  upstream_eff_pur_tot = 0
  upstream_eff = 0.0
  upstream_track_eff = 0.0
  upstream_pur = 0.0

  if 0 in trackers_list :
    for i in range( bin_pz ) :
      a = float( upstream_recon_count_pz[i] )
      b = float( upstream_mismatched_count_pz[i] )
      c = float( upstream_mc_count_pz[i] )
      d = float( upstream_recon_tracks_pz[i] )
      e = float( upstream_mc_tracks_pz[i] )

      if upstream_mc_count_pz[i] == 0 : 
        upstream_eff_pz.append( 0.0 )
        upstream_eff_pz_err.append( 0.0 )
        upstream_track_eff_pz.append( 0.0 )
        upstream_track_eff_pz_err.append( 0.0 )
        upstream_purity_pz.append( 0.0 )
        upstream_purity_pz_err.append( 0.0 )
      else :
        upstream_eff_tot += ( a / c ) * 100.0
        upstream_track_eff_tot += ( d / e ) * 100.0
        upstream_pur_tot += ( 1.0 - ( b / c ) ) * 100.0
        upstream_eff_pur_tot += 1
        upstream_number_recon_pz.append( a )
        upstream_number_mc_pz.append( c )
        upstream_number_recon_tracks_pz.append( d )
        upstream_number_mc_tracks_pz.append( e )
        upstream_eff_pz.append( ( a / c ) * 100.0 )
#        upstream_eff_pz_err.append( 100.0 * math.sqrt( ( a / c**2 ) + ( a**2 / c**3 ) )  )
        upstream_eff_pz_err.append( 100.0 * math.sqrt( a ) / c )
        upstream_track_eff_pz.append( ( d / e ) * 100.0 )
#        upstream_track_eff_pz_err.append( 100.0 * math.sqrt( ( d / e**2.0 ) + ( d**2.0 / e**3.0 ) )  )
        upstream_track_eff_pz_err.append( 100.0 * math.sqrt( d ) / e )
        upstream_purity_pz.append( ( 1.0 - ( b / c ) ) * 100.0 )
#        upstream_purity_pz_err.append( 100.0 * math.sqrt( ( b / c**2 ) + ( b**2 / c**3 ) )  )
        upstream_purity_pz_err.append( 100.0 * math.sqrt( b ) / c )

    for i in range( bin_pt ) :
      a = float( upstream_recon_count_pt[i] )
      b = float( upstream_mismatched_count_pt[i] )
      c = float( upstream_mc_count_pt[i] )
      d = float( upstream_recon_tracks_pt[i] )
      e = float( upstream_mc_tracks_pt[i] )

      if upstream_mc_count_pt[i] == 0 : 
        upstream_eff_pt.append( 0.0 )
        upstream_eff_pt_err.append( 0.0 )
        upstream_purity_pt.append( 0.0 )
        upstream_purity_pt_err.append( 0.0 )
      else :
        upstream_number_recon_pt.append( a )
        upstream_number_mc_pt.append( c )
        upstream_number_recon_tracks_pt.append( d )
        upstream_number_mc_tracks_pt.append( e )
        upstream_eff_pt.append( ( a / c ) * 100.0 )
#        upstream_eff_pt_err.append( 100.0 * math.sqrt( ( a / c**2 ) + ( a**2 / c**3 ) )  )
        upstream_eff_pt_err.append( 100.0 * math.sqrt( a ) / c )
        upstream_track_eff_pt.append( ( d / e ) * 100.0 )
#        upstream_track_eff_pt_err.append( 100.0 * math.sqrt( ( d / e**2.0 ) + ( d**2.0 / e**3.0 ) )  )
        upstream_track_eff_pt_err.append( 100.0 * math.sqrt( d ) / e )
        upstream_purity_pt.append( ( 1.0 - ( b / c ) ) * 100.0 )
#        upstream_purity_pt_err.append( 100.0 * math.sqrt( ( b / c**2 ) + ( b**2 / c**3 ) )  )
        upstream_purity_pt_err.append( 100.0 * math.sqrt( b ) / c )

    upstream_eff = upstream_eff_tot / upstream_eff_pur_tot
    upstream_track_eff = upstream_track_eff_tot / upstream_eff_pur_tot
    upstream_pur = upstream_pur_tot / upstream_eff_pur_tot

  downstream_eff_tot = 0.0
  downstream_track_eff_tot = 0.0
  downstream_pur_tot = 0.0
  downstream_eff_pur_tot = 0
  downstream_eff = 0.0
  downstream_track_eff = 0.0
  downstream_pur = 0.0

  if 1 in trackers_list :
    for i in range( bin_pz ) :
      a = float( downstream_recon_count_pz[i] )
      b = float( downstream_mismatched_count_pz[i] )
      c = float( downstream_mc_count_pz[i] )
      d = float( downstream_recon_tracks_pz[i] )
      e = float( downstream_mc_tracks_pz[i] )

      if downstream_mc_count_pz[i] == 0 : 
        downstream_eff_pz.append( 0.0 )
        downstream_eff_pz_err.append( 0.0 )
        downstream_purity_pz.append( 0.0 )
        downstream_purity_pz_err.append( 0.0 )
      else :
        downstream_eff_tot += ( a / c ) * 100.0
        downstream_track_eff_tot += ( d / e ) * 100.0
        downstream_pur_tot += ( 1 - ( b / c ) ) * 100.0
        downstream_eff_pur_tot += 1
        downstream_number_recon_pz.append( a )
        downstream_number_mc_pz.append( c )
        downstream_number_recon_tracks_pz.append( d )
        downstream_number_mc_tracks_pz.append( e )
        downstream_eff_pz.append( ( a / c ) * 100.0 )
#        downstream_eff_pz_err.append( 100.0 * math.sqrt( ( a / c**2 ) + ( a**2 / c**3 ) )  )
        downstream_eff_pz_err.append( 100.0 * math.sqrt( a ) / c )
        downstream_track_eff_pz.append( ( d / e ) * 100.0 )
#        downstream_track_eff_pz_err.append( 100.0 * math.sqrt( ( d / e**2.0 ) + ( d**2.0 / e**3.0 ) )  )
        downstream_track_eff_pz_err.append( 100.0 * math.sqrt( d ) / e )
        downstream_purity_pz.append( ( 1.0 - ( b / c ) ) * 100.0 )
#        downstream_purity_pz_err.append( 100.0 * math.sqrt( ( b / c**2 ) + ( b**2 / c**3 ) )  )
        downstream_purity_pz_err.append( 100.0 * math.sqrt( b ) / c )

    for i in range( bin_pt ) :
      a = float( downstream_recon_count_pt[i] )
      b = float( downstream_mismatched_count_pt[i] )
      c = float( downstream_mc_count_pt[i] )
      d = float( downstream_recon_tracks_pt[i] )
      e = float( downstream_mc_tracks_pt[i] )

      if downstream_mc_count_pt[i] == 0 : 
        downstream_eff_pt.append( 0.0 )
        downstream_eff_pt_err.append( 0.0 )
        downstream_purity_pt.append( 0.0 )
        downstream_purity_pt_err.append( 0.0 )
      else :
        downstream_number_recon_pt.append( a )
        downstream_number_mc_pt.append( c )
        downstream_number_recon_tracks_pt.append( d )
        downstream_number_mc_tracks_pt.append( e )
        downstream_eff_pt.append( ( a / c ) * 100.0 )
#        downstream_eff_pt_err.append( 100.0 * math.sqrt( ( a / c**2 ) + ( a**2 / c**3 ) )  )
        downstream_eff_pt_err.append( 100.0 * math.sqrt( a ) / c )
        downstream_track_eff_pt.append( ( d / e ) * 100.0 )
#        downstream_track_eff_pt_err.append( 100.0 * math.sqrt( ( d / e**2.0 ) + ( d**2.0 / e**3.0 ) )  )
        downstream_track_eff_pt_err.append( 100.0 * math.sqrt( d ) / e )
        downstream_purity_pt.append( ( 1.0 - ( b / c ) ) * 100.0 )
#        downstream_purity_pt_err.append( 100.0 * math.sqrt( ( b / c**2 ) + ( b**2 / c**3 ) )  )
        downstream_purity_pt_err.append( 100.0 * math.sqrt( b ) / c )


    downstream_eff = downstream_eff_tot / downstream_eff_pur_tot
    downstream_track_eff = downstream_track_eff_tot / downstream_eff_pur_tot
    downstream_pur = downstream_pur_tot / downstream_eff_pur_tot
    
    

####################################################################################################



  outfile = ROOT.TFile( os.path.join( output_directory, 'trackpoint_efficiencies_results.root' ), 'RECREATE' )

  upstream_mc_pid.Write()
  downstream_mc_pid.Write()

  if 0 in trackers_list :
    up_number_pz_mc_graph = ROOT.TGraphErrors( bin_pz, pz_bins, upstream_number_mc_pz, errors_pz, zeros_pz )
    up_number_pz_recon_graph = ROOT.TGraphErrors( bin_pz, pz_bins, upstream_number_recon_pz, errors_pz, zeros_pz )
    up_number_pt_mc_graph = ROOT.TGraphErrors( bin_pt, pt_bins, upstream_number_mc_pt, errors_pt, zeros_pt )
    up_number_pt_recon_graph = ROOT.TGraphErrors( bin_pt, pt_bins, upstream_number_recon_pt, errors_pt, zeros_pt )

    up_number_track_pz_mc_graph = ROOT.TGraphErrors( bin_pz, pz_bins, upstream_number_mc_tracks_pz, errors_pz, zeros_pz )
    up_number_track_pz_recon_graph = ROOT.TGraphErrors( bin_pz, pz_bins, upstream_number_recon_tracks_pz, errors_pz, zeros_pz )
    up_number_track_pt_mc_graph = ROOT.TGraphErrors( bin_pt, pt_bins, upstream_number_mc_tracks_pt, errors_pt, zeros_pt )
    up_number_track_pt_recon_graph = ROOT.TGraphErrors( bin_pt, pt_bins, upstream_number_recon_tracks_pt, errors_pt, zeros_pt )

    up_eff_pz_graph = ROOT.TGraphErrors( bin_pz, pz_bins, upstream_eff_pz, errors_pz, upstream_eff_pz_err )
    up_eff_pt_graph = ROOT.TGraphErrors( bin_pt, pt_bins, upstream_eff_pt, errors_pt, upstream_eff_pt_err )

    up_track_eff_pz_graph = ROOT.TGraphErrors( bin_pz, pz_bins, upstream_track_eff_pz, errors_pz, upstream_track_eff_pz_err )
    up_track_eff_pt_graph = ROOT.TGraphErrors( bin_pt, pt_bins, upstream_track_eff_pt, errors_pt, upstream_track_eff_pt_err )

    up_purity_pz_graph = ROOT.TGraphErrors( bin_pz, pz_bins, upstream_purity_pz, errors_pz, upstream_purity_pz_err )
    up_purity_pt_graph = ROOT.TGraphErrors( bin_pt, pt_bins, upstream_purity_pt, errors_pt, upstream_purity_pt_err )

    up_number_pz_mc_graph.SetName( 'upstream_number_mc_pz' )
    up_number_pz_recon_graph.SetName( 'upstream_number_recon_pz' )
    up_number_pt_mc_graph.SetName( 'upstream_number_mc_pt' )
    up_number_pt_recon_graph.SetName( 'upstream_number_recon_pt' )

    up_number_track_pz_mc_graph.SetName( 'upstream_number_track_mc_pz' )
    up_number_track_pz_recon_graph.SetName( 'upstream_number_track_recon_pz' )
    up_number_track_pt_mc_graph.SetName( 'upstream_number_track_mc_pt' )
    up_number_track_pt_recon_graph.SetName( 'upstream_number_track_recon_pt' )

    up_eff_pz_graph.SetName( 'upstream_efficiency_pz' )
    up_eff_pt_graph.SetName( 'upstream_efficiency_pt' )

    up_track_eff_pz_graph.SetName( 'upstream_track_efficiency_pz' )
    up_track_eff_pt_graph.SetName( 'upstream_track_efficiency_pt' )

    up_purity_pz_graph.SetName( 'upstream_purity_pz' )
    up_purity_pt_graph.SetName( 'upstream_purity_pt' )

    up_number_pz_mc_graph.Write()
    up_number_pz_recon_graph.Write()
    up_number_pt_mc_graph.Write()
    up_number_pt_recon_graph.Write()

    up_number_track_pz_mc_graph.Write()
    up_number_track_pz_recon_graph.Write()
    up_number_track_pt_mc_graph.Write()
    up_number_track_pt_recon_graph.Write()

    up_eff_pz_graph.Write()
    up_eff_pt_graph.Write()

    up_track_eff_pz_graph.Write()
    up_track_eff_pt_graph.Write()

    up_purity_pz_graph.Write()
    up_purity_pt_graph.Write()

  if 1 in trackers_list :
    down_number_pz_mc_graph = ROOT.TGraphErrors( bin_pz, pz_bins, downstream_number_mc_pz, errors_pz, zeros_pz )
    down_number_pz_recon_graph = ROOT.TGraphErrors( bin_pz, pz_bins, downstream_number_recon_pz, errors_pz, zeros_pz )
    down_number_pt_mc_graph = ROOT.TGraphErrors( bin_pt, pt_bins, downstream_number_mc_pt, errors_pt, zeros_pt )
    down_number_pt_recon_graph = ROOT.TGraphErrors( bin_pt, pt_bins, downstream_number_recon_pt, errors_pt, zeros_pt )

    down_number_track_pz_mc_graph = ROOT.TGraphErrors( bin_pz, pz_bins, downstream_number_mc_tracks_pz, errors_pz, zeros_pz )
    down_number_track_pz_recon_graph = ROOT.TGraphErrors( bin_pz, pz_bins, downstream_number_recon_tracks_pz, errors_pz, zeros_pz )
    down_number_track_pt_mc_graph = ROOT.TGraphErrors( bin_pt, pt_bins, downstream_number_mc_tracks_pt, errors_pt, zeros_pt )
    down_number_track_pt_recon_graph = ROOT.TGraphErrors( bin_pt, pt_bins, downstream_number_recon_tracks_pt, errors_pt, zeros_pt )

    down_eff_pz_graph = ROOT.TGraphErrors( bin_pz, pz_bins, downstream_eff_pz, errors_pz, downstream_eff_pz_err )
    down_eff_pt_graph = ROOT.TGraphErrors( bin_pt, pt_bins, downstream_eff_pt, errors_pt, downstream_eff_pt_err )

    down_track_eff_pz_graph = ROOT.TGraphErrors( bin_pz, pz_bins, downstream_track_eff_pz, errors_pz, downstream_track_eff_pz_err )
    down_track_eff_pt_graph = ROOT.TGraphErrors( bin_pt, pt_bins, downstream_track_eff_pt, errors_pt, downstream_track_eff_pt_err )

    down_purity_pz_graph = ROOT.TGraphErrors( bin_pz, pz_bins, downstream_purity_pz, errors_pz, downstream_purity_pz_err )
    down_purity_pt_graph = ROOT.TGraphErrors( bin_pt, pt_bins, downstream_purity_pt, errors_pt, downstream_purity_pt_err )

    down_number_pz_mc_graph.SetName( 'downstream_number_mc_pz' )
    down_number_pz_recon_graph.SetName( 'downstream_number_recon_pz' )
    down_number_pt_mc_graph.SetName( 'downstream_number_mc_pt' )
    down_number_pt_recon_graph.SetName( 'downstream_number_recon_pt' )

    down_number_track_pz_mc_graph.SetName( 'downstream_number_track_mc_pz' )
    down_number_track_pz_recon_graph.SetName( 'downstream_number_track_recon_pz' )
    down_number_track_pt_mc_graph.SetName( 'downstream_number_track_mc_pt' )
    down_number_track_pt_recon_graph.SetName( 'downstream_number_track_recon_pt' )

    down_eff_pz_graph.SetName( 'downstream_efficiency_pz' )
    down_eff_pt_graph.SetName( 'downstream_efficiency_pt' )

    down_track_eff_pz_graph.SetName( 'downstream_track_efficiency_pz' )
    down_track_eff_pt_graph.SetName( 'downstream_track_efficiency_pt' )

    down_purity_pz_graph.SetName( 'downstream_purity_pz' )
    down_purity_pt_graph.SetName( 'downstream_purity_pt' )

    down_number_pz_mc_graph.Write()
    down_number_pz_recon_graph.Write()
    down_number_pt_mc_graph.Write()
    down_number_pt_recon_graph.Write()

    down_number_track_pz_mc_graph.Write()
    down_number_track_pz_recon_graph.Write()
    down_number_track_pt_mc_graph.Write()
    down_number_track_pt_recon_graph.Write()

    down_eff_pz_graph.Write()
    down_eff_pt_graph.Write()

    down_track_eff_pz_graph.Write()
    down_track_eff_pt_graph.Write()

    down_purity_pz_graph.Write()
    down_purity_pt_graph.Write()

  outfile.Close()


##################################################################################

# Calculate the total efficiency and purity for all the tracks

  print '############################################'
  print '   Calculated Efficiencies and Purities'
  print '############################################'
  print
  print ' Upstream Efficiency          = ', upstream_eff
  print ' Upstream Track Efficiency    = ', upstream_track_eff
  print ' Upstream Purity              = ', upstream_pur
  print
  print ' Downstream Efficiency        = ', downstream_eff
  print ' Downstream Track Efficiency  = ', downstream_track_eff
  print ' Downstream Purity            = ', downstream_pur
  print


##################################################################################



def get_bin_num( value, min, max, total ) :
  """
    Returns the bin number associated with the value, given the total number of bins,
    and the range over which they cover
  """
  if max < min :
    raise ValueError( 'Minimum cannot be greater than the maximum: ' + str( min ) + ' > ' + str( max ) )
  if value < min :
    print 'Cannot bin value less than minimum: ' + str( value ) + ' < ' + str( min )
    return -1
#    raise ValueError( 'Cannot bin value less than minimum: ' + str( value ) + ' < ' + str( min ) )
  if value > max :
    print 'Cannot bin value greater than maximum: ' + str( value ) + ' > ' + str( max )
    return -1
#    raise ValueError( 'Cannot bin value greater than maximum: ' + str( value ) + ' > ' + str( max ) )

  return int( ( value - min ) / ( ( max - min ) / total ) )


def bin_value( bins, value, min, max, total ) :
  """
    Returns the bin number associated with the value, given the total number of bins,
    and the range over which they cover
  """
  if max < min :
    raise ValueError( 'Minimum cannot be greater than the maximum: ' + str( min ) + ' > ' + str( max ) )
  if value < min :
    print 'Cannot bin value less than minimum: ' + str( value ) + ' < ' + str( min )
    return False
#    raise ValueError( 'Cannot bin value less than minimum: ' + str( value ) + ' < ' + str( min ) )
  if value > max :
    print 'Cannot bin value greater than maximum: ' + str( value ) + ' > ' + str( max )
    return False
#    raise ValueError( 'Cannot bin value greater than maximum: ' + str( value ) + ' > ' + str( max ) )

  bins[ int( ( value - min ) / ( ( max - min ) / total ) ) ] += value
  return True



##################################################################################



def find_virtual_plane( bunches, position, max_sep=10.0 ) :

  current_plane = -1
  current_sep = max_sep

  for i in range( len( bunches ) ) :
    bunch = bunches[i]
    sep = abs( bunch[0].get( 'z' ) - position )
    if  sep < current_sep :
      current_sep = sep
      current_plane = i

#  print "Position :", position, "-> Plane :", current_plane
  if current_plane < 0 :
    return None
  else :
    return current_plane



##################################################################################



def is_contained( virtual_hits, tracker_start, tracker_end, radius ) :
  for plane, hit in virtual_hits :
    if plane >= tracker_start and plane <= tracker_end :
      if math.sqrt( hit.get_x() ** 2 + hit.get_y() ** 2 ) > radius :
        return False
  return True




##################################################################################



try :

  if __name__ == "__main__":

    parser = argparse.ArgumentParser( description='Performs emittance and beta function calculations on MAUS data files.\n\nMany of the functions require the simualation to be perfomed with virtual planes located within the tracker planes within the geometry file. If this is not the case no Monte Carlo information will be available for analysis.' )

    parser.add_argument( 'maus_root_files', nargs='+', help='List of Root files, generated by MAUS to analyse' )

    parser.add_argument( '--recon_station', type=int, default=1, help='Station ID to make the reference plane in the analysis' )
    parser.add_argument( '--recon_plane', type=int, default=0, help='Plane ID to make the reference plane in the analysis' )
    parser.add_argument( '--cut_p_value', type=float, default=0.05, help='Cut to apply to the recon P Value' )
    parser.add_argument( '--cut_pt', type=float, default=None, help='Cut to apply to the transverse momentum of reconstructed trackpoints' )
    parser.add_argument( '--cut_pz', type=float, default=None, help='Cut to apply to the longintudinal momentum of reconstructed trackpoints' )
    parser.add_argument( '--cut_pid', type=int, default=-13, help='Cut to apply to the PID of reconstructed and Monte Carlo trackpoints' )
    parser.add_argument( '--bin_pz', type=int, default=10, help='Number of bins in longitudinal momentum, overwhich to run the analysis' )
    parser.add_argument( '--bin_pt', type=int, default=10, help='Number of bins in transverse momentum, overwhich to run the analysis' )
    parser.add_argument( '--max_pz', type=float, default=250.0, help='Max Value of longintudinal momentum to study' )
    parser.add_argument( '--min_pz', type=float, default=150.0, help='Min Value of longintudinal momentum to study' )
    parser.add_argument( '--max_pt', type=float, default=100.0, help='Max Value of transverse momentum to study' )
    parser.add_argument( '--min_pt', type=float, default=0.0, help='Min Value of transverse momentum to study' )
    parser.add_argument( '--output_directory', default='./', help='Directory to save the output from this analysis script' )
    parser.add_argument( '--cut_straight_tracks', action='store_true', help='Choose whether straight tracks should be cut during the analysis' )
    parser.add_argument( '--cut_residual', type=float, default=None, help='Set the distance between MC and Recon trackpoints where they are considered incorrect' )
    parser.add_argument( '--cut_radius', type=float, default=150.0, help='Set the radial cut in the MC to elimanate tracks that we couldn\'t reconstruct' )
    parser.add_argument( '--require_clean_tracks', action='store_true', help='Ensure that all trackpoints come from tracks that are perfectly reconstructable' )


################################################################################


    namespace = None

    try :
      namespace = parser.parse_args()
    except :
      pass
    else :

      if namespace.recon_station > 5 or namespace.recon_station < 1 :
        raise ValueError( 'Invalid reconstruction station: ' + str( namespace.recon_station ) )
      if namespace.recon_plane > 2 or namespace.recon_plane < 0 :
        raise ValueError( 'Invalid reconstruction plane: ' + str( namespace.recon_plane ) )
      if namespace.cut_p_value < 0.0 or namespace.cut_p_value > 1.0 :
        raise ValueError( 'P Values must be between 0.0 and 1.0' )
      if namespace.cut_pt is not None and namespace.cut_pt < 0.0 :
        raise ValueError( 'Transverse momentum must be positive' )
      if namespace.bin_pz < 0 or namespace.bin_pt < 0 :
        raise ValueError( 'Bin numbers must be positive integers' )
      if namespace.max_pz < namespace.min_pz :
        raise ValueError( 'Maximum longitudinal momentum must be larger than the minimum' )
      if namespace.max_pt < namespace.min_pt :
        raise ValueError( 'Maximum transverse momentum must be larger than the minimum' )
      if namespace.max_pt < 0.0 or namespace.min_pt < 0.0 :
        raise ValueError( 'Transverse momentum must be positive' )
      if not os.path.isdir( namespace.output_directory ) :
        raise ValueError( 'Could not find directory: ' + str( namespace.output_directory ) )
      if namespace.cut_residual is not None and namespace.cut_residual < 0.0 :
        raise ValueError( 'cut_residual must be a positive float' )
      if namespace.cut_radius is not None and namespace.cut_radius < 0.0 :
        raise ValueError( 'cut_radius must be a positive float' )



################################################################################

      print
      run( **vars( namespace ) )
      print


################################################################################



except Exception as ex :
  print
  print "An error occured."
  print
  print traceback.format_exc()
  print

