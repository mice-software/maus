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
import MAUS
import ROOT
import xboa
import numpy
import operator

# definitions of MAUS data structure for PyROOT
import libMausCpp #pylint: disable = W0611

# Local emittance analysis library to accompany xboa
import emittance_analysis
from emittance_analysis import covariances
from emittance_analysis import duplicate_print
from emittance_analysis import extraction
from emittance_analysis import plotter
from emittance_analysis import muon_mass
from emittance_analysis import stats
from emittance_analysis import common



####################################################################################################
## Define all the histograms that could possibly want to produce.
####################################################################################################


residual_x_up = ROOT.TH1F( "residual_x_up", "Residual in Upstream X-Positions", 200, -10.0, 10.0 )
residual_y_up = ROOT.TH1F( "residual_y_up", "Residual in Upstream Y-Positions", 200, -10.0, 10.0 )
residual_z_up = ROOT.TH1F( "residual_z_up", "Residual in Upstream Z-Positions", 200, -10.0, 10.0 )
residual_x_down = ROOT.TH1F( "residual_x_down", "Residual in Downstream X-Positions", 200, -10.0, 10.0 )
residual_y_down = ROOT.TH1F( "residual_y_down", "Residual in Downstream Y-Positions", 200, -10.0, 10.0 )
residual_z_down = ROOT.TH1F( "residual_z_down", "Residual in Downstream Z-Positions", 200, -10.0, 10.0 )
residual_px_up = ROOT.TH1F( "residual_px_up", "Residual in Upstream X-Momentum", 500, -25.0, 25.0 )
residual_py_up = ROOT.TH1F( "residual_py_up", "Residual in Upstream Y-Momentum", 500, -25.0, 25.0 )
residual_px_down = ROOT.TH1F( "residual_px_down", "Residual in Downstream X-Momentum", 500, -25.0, 25.0 )
residual_py_down = ROOT.TH1F( "residual_py_down", "Residual in Downstream Y-Momentum", 500, -25.0, 25.0 )
residual_pt_up = ROOT.TH1F( "residual_pt_up", "Residual in Upstream Transverse Momentum", 500, -50.0, 50.0 )
residual_pz_up = ROOT.TH1F( "residual_pz_up", "Residual in Upstream Longitudinal Momentum", 500, -50.0, 50.0 )
residual_pt_down = ROOT.TH1F( "residual_pt_down", "Residual in Downstream Transverse Momentum", 500, -50.0, 50.0 )
residual_pz_down = ROOT.TH1F( "residual_pz_down", "Residual in Downstream Longitudinal Momentum", 500, -50.0, 50.0 )
residual_phi_up = ROOT.TH1F( "residual_phi_up", "Residual in Upstream Phi-Positions", 200, -6.3, 6.3 )
residual_phi_down = ROOT.TH1F( "residual_phi_down", "Residual in Downstream Phi-Positions", 200, -6.3, 6.3 )

residual_pt_pz_up = ROOT.TH2F( "residual_pt_pz_up", "Residuals in Upstream Transverse Momentum Against the MC Longitudinal Momentum", 500, -50.0, 50.0, 500, 100.0, 300.0 )
residual_pt_pz_down = ROOT.TH2F( "residual_pt_pz_down", "Residuals in Downstream Transverse Momentum Against the MC Longitudinal Momentum", 500, -50.0, 50.0, 500, 100.0, 300.0 )
residual_pz_pz_up = ROOT.TH2F( "residual_pz_pz_up", "Residuals in Upstream Longitudinal Momentum Against the MC Longitudinal Momentum", 500, -50.0, 50.0, 500, 100.0, 300.0 )
residual_pz_pz_down = ROOT.TH2F( "residual_pz_pz_down", "Residuals in Downstream Longitudinal Momentum Against the MC Longitudinal Momentum", 500, -50.0, 50.0, 500, 100.0, 300.0 )

p_value_up = ROOT.TH1F( "p_value_up", "Distribution of Track P-Values in Upstream Tracker", 200, 0.0, 1.0 )
p_value_down = ROOT.TH1F( "p_value_down", "Distribution of Track P-Values in Downstream Tracker", 200, 0.0, 1.0 )

virt_SPA_up = ROOT.TH1F( "virt_single_particle_amplitude_up", "Amplitude in Upstream Reference Plane", 500, 0.0, 200.0 )
virt_SPA_down = ROOT.TH1F( "virt_single_particle_amplitude_down", "Amplitude in Downstream Reference Plane", 500, 0.0, 200.0 )
kalman_SPA_up = ROOT.TH1F( "kalman_single_particle_amplitude_up", "Amplitude in Upstream Reference Plane", 500, 0.0, 200.0 )
kalman_SPA_down = ROOT.TH1F( "kalman_single_particle_amplitude_down", "Amplitude in Downstream Reference Plane", 500, 0.0, 200.0 )

virt_SPA_change = ROOT.TH1F( "virt_single_particle_amplitude_change", "Amplitude Change", 1000, -100.0, 100.0 )
kalman_SPA_change = ROOT.TH1F( "kalman_single_particle_amplitude_change", "Amplitude Change", 1000, -100.0, 100.0 )

upstream_emittance_error = ROOT.TH1F( "emittance_mc_upstream", "Histogram of Upstream Emittance", 500, 0.0, 20.0 )
downstream_emittance_error = ROOT.TH1F( "emittance_mc_downstream", "Histogram of Downstream Emittance", 500, 0.0, 20.0 )
fractional_emittance_error = ROOT.TH1F( "emittance_mc_fractional_change", "Histogram of Fractional Emittance Change", 200, -1.0, 1.0 )
delta_emittance_error = ROOT.TH1F( "delta_mc_emittance_change", "Histogram of the Emittance Change", 200, -1.0, 1.0 )

upstream_emittance_error_corrected = ROOT.TH1F( "emittance_mc_upstream_corrected", "Histogram of Upstream Corrected Emittance", 500, 0.0, 20.0 )
downstream_emittance_error_corrected = ROOT.TH1F( "emittance_mc_downstream_corrected", "Histogram of Downstream Corrected Emittance", 500, 0.0, 20.0 )
fractional_emittance_error_corrected = ROOT.TH1F( "emittance_mc_fractional_change_corrected", "Histogram of Fractional Corrected Emittance Change", 200, -1.0, 1.0 )
delta_emittance_error_corrected = ROOT.TH1F( "delta_mc_emittance_change_corrected", "Histogram of the Corrected Emittance Change", 200, -1.0, 1.0 )



####################################################################################################
## Run the program
####################################################################################################

def run( maus_root_files, calculate_full, calculate_errors, calculate_spa, calculate_corrections, save_summary, save_variables, save_data, \
    save_residuals, save_emittance, save_graphs, output_directory, manual_associate_planes, associate_tolerance, associate_max_attempts, \
    recon_plane, recon_station, correction_upstream, correction_downstream, emittance_bin_size, cut_p_value, cut_pt, cut_low_pt, cut_pz, \
    cut_radius, cut_pid, cut_virtuals, cut_4point_tracks, upstream_virtual, downstream_virtual, no_virtuals, max_num_events, correction_directory, \
    inspect_planes ) :


# Useful Later - where we store the virtual plane / tracker plane associations
  virtual_plane_dict = []


# Copy the stdout to a text file if asked
  logging = None
  if save_summary :
    logging = duplicate_print.Tee( os.path.join( output_directory, 'measure_emittance_logging.txt' ) )



####################################################################################################
## Find the virtual planes associated with the tracker planes using the data
####################################################################################################

  if not manual_associate_planes is True and no_virtuals is False :
    print "Locating Virtual Planes"
    print "Please Wait..."

    filename = maus_root_files[0]

# Initialise the dictionary
    for i in range( 2 ) :
      stations = []
      for j in range( 5 ) :
        planes = []
        for k in range( 3 ) :
          planes.append( ( -1, associate_tolerance ) )
        stations.append( planes )
      virtual_plane_dict.append( stations )


# Initialise the file reader
    reader = extraction.maus_reader( filename )

    attempt_count = 0
    while attempt_count < associate_max_attempts and reader.next_event() :

      scifi_event = reader.get_event( 'scifi' )
      mc_event = reader.get_event( 'mc' )

# See if we can associate ALL tracker planes with a virtual plane
      if common.associate_virtuals( virtual_plane_dict, scifi_event.scifitracks(), mc_event.GetVirtualHits() ) is True :
        break
      attempt_count += 1

    else :
      if attempt_count >= associate_max_attempts :
        print
        print "ERROR Could not successfully associate all virtual planes after", associate_max_attempts, "attempts"
        print
      else : 
        print
        print "ERROR Ran out of events to locate virtual planes."
        print
      print virtual_plane_dict
      print
      sys.exit( 0 )

    if upstream_virtual < 0 :
      upstream_virtual = virtual_plane_dict[0][recon_station - 1][recon_plane][0]
    if downstream_virtual < 0 :
      downstream_virtual = virtual_plane_dict[1][recon_station - 1][recon_plane][0]

    print
    print "Found reference planes. Numbers:", upstream_virtual, "and", downstream_virtual
    print
    print "ALL VIRTUAL PLANES ALLOCATIONS:"
    print 
    print virtual_plane_dict
    print

    gc.collect()


####################################################################################################
## Initialse the structures to hold various hit and bunch information
####################################################################################################
# Covariance Matrix Corrections from text files
  correction_upstream_matrix = None
  correction_downstream_matrix = None
  if correction_upstream != None :
    correction_upstream_matrix = numpy.loadtxt( correction_upstream )
    print "Loaded Upstream Correction Matrix"
  if correction_downstream != None :
    correction_downstream_matrix = numpy.loadtxt( correction_downstream )
    print "Loaded Downstream Correction Matrix"

# Covariance matrix corrections from this dataset
  correction_up = covariances.correction_matrix()
  correction_down = covariances.correction_matrix()

# Particle Bunches at reference planes
  kalman1_bunch = xboa.Bunch.Bunch()
  kalman2_bunch = xboa.Bunch.Bunch()
  virtual1_bunch = xboa.Bunch.Bunch()
  virtual2_bunch = xboa.Bunch.Bunch()

# Temporary bunches for histogramming measurements
  current_upstream = xboa.Bunch.Bunch()
  current_downstream = xboa.Bunch.Bunch()

# Some counters
  current_num = 0
  spill_counter = 0

# Store information from ALL virtual planes to make the MC graphs later
  virtual_bunches = []


####################################################################################################
## Initialise the file reader with ALL the files.
####################################################################################################
  file_reader = extraction.maus_reader( maus_root_files )

  number_files = file_reader.get_number_files()

  print "Loading", number_files, "File(s)"
  print


####################################################################################################
# Prepare the user defined cuts to be applied later
####################################################################################################
# Extract reconstucted trackpoints
  scifi_extractor = extraction.scifi_tracks()
  scifi_extractor.add_reference_plane( 0, recon_station, recon_plane, 'upstream' )
  scifi_extractor.add_reference_plane( 1, recon_station, recon_plane, 'downstream' )
  scifi_extractor.add_cut( 'pvalue', cut_p_value, operator.lt )
  scifi_extractor.add_cut( 'pt', cut_pt, operator.gt )
  scifi_extractor.add_cut( 'pz', cut_pz, operator.gt )
  scifi_extractor.add_cut( 'pt', cut_low_pt, operator.lt )
  scifi_extractor.add_cut( 'pid', cut_pid, operator.ne )
#  scifi_extractor.set_cut_missing( False )
  if cut_4point_tracks :
    scifi_extractor.set_min_num_trackpoints( 5 )

# Extract the MC information to compare against
  mc_extractor = None
  if no_virtuals :
    print "Using MC Data from SciFi Hits objects"
    mc_extractor = extraction.scifi_hits()
    mc_extractor.add_reference_plane( 0, recon_station, recon_plane, 'upstream' )
    mc_extractor.add_reference_plane( 1, recon_station, recon_plane, 'downstream' )
  else :
    print "Using MC Data from Virtual Hits objects"
    mc_extractor = extraction.virtual_hits()
    mc_extractor.add_reference_plane( upstream_virtual, 'upstream' )
    mc_extractor.add_reference_plane( downstream_virtual, 'downstream' )
  mc_extractor.add_cut( 'pid', cut_pid, operator.ne )

# Extract the pure virtual plane data
  virtual_extractor = extraction.virtual_hits()
  virtual_extractor.add_cut( 'pid', cut_pid, operator.ne )
  virtual_extractor.add_cut( 'r', cut_radius, operator.ge )

# Count when the trackers and MC don't agree
  mismatch_counter = 0

####################################################################################################
# Iterate through all events of all files and analyse what we find
####################################################################################################
  while file_reader.next_event() and file_reader.get_total_num_events() != max_num_events :
    try :

# Print current position - it looks nice!
      duplicate_print.actual_stdout.write( ' Spill ' + str( file_reader.get_current_spill_number() ) + ' of ' + str( file_reader.get_current_number_spills() ) + ' in File ' + str( file_reader.get_current_filenumber() ) + ' of ' + str( file_reader.get_number_files() ) + '   \r' )
      duplicate_print.actual_stdout.flush()


# Load Scifi and MC events
      scifi_event = file_reader.get_event( 'scifi' )
      mc_event = file_reader.get_event( 'mc' )

# Load the scifi data
      scifi_extractor.load_tracks( scifi_event )
      ext_cut = scifi_extractor.is_cut()

# Load the MC data
      mc_extractor.load_tracks( mc_event )
      mc_cut = ( cut_virtuals and mc_extractor.is_cut() )

# Check the cuts
      if not ( ext_cut or mc_cut ) :

        pairs = []

# Make pairs of MC and Recon trackpoints to compare
        if no_virtuals :
          pairs = extraction.make_virt_recon_pairs( mc_extractor.get_reference_trackpoints(), scifi_extractor.get_reference_trackpoints(), average_mc = True, max_separation = 2.0 )
        else :
          pairs = extraction.make_virt_recon_pairs( mc_extractor.get_reference_trackpoints(), scifi_extractor.get_reference_trackpoints() )

        if len( pairs ) != 2 :
          print
          print "ERROR : We found", len( pairs ), "Reference Hits..."
          print
          mismatch_counter += 1
          continue

# By the power of deduction, we have upstream and downstream points!
        recon_up, mc_up = pairs[0]
        recon_down, mc_down = pairs[1]


####################################################################################################
## Perform actions depending on the flags from the user
####################################################################################################
# Full reconstruction analysis
        if calculate_full or calculate_spa :
          kalman1_bunch.append( make_xboa_hit( recon_up ) )
          kalman2_bunch.append( make_xboa_hit( recon_down ) )
          virtual1_bunch.append( make_xboa_hit( mc_up ) )
          virtual2_bunch.append( make_xboa_hit( mc_down ) )

# Calculate correction matrices
        if calculate_corrections :
          correction_up.add_hit( make_xboa_hit( recon_up ), make_xboa_hit( mc_up ) )
          correction_down.add_hit( make_xboa_hit( recon_down ), make_xboa_hit( mc_down ) )

# Histogram emittances for an error estimation
        if calculate_errors :
          current_num += 1
          current_upstream.append( make_xboa_hit( recon_up ) )
          current_downstream.append( make_xboa_hit( recon_down ) )

          if current_num == emittance_bin_size :
            up_em = current_upstream.get_emittance( ['x', 'y'] )
            cov_up = current_upstream.covariance_matrix( ['x', 'px', 'y', 'py'] )
            down_em = current_downstream.get_emittance( ['x', 'y'] )
            cov_down = current_downstream.covariance_matrix( ['x', 'px', 'y', 'py'] )

            upstream_emittance_error.Fill( up_em )
            downstream_emittance_error.Fill( down_em )
            fractional_emittance_error.Fill( ( down_em / up_em ) - 1.0 )
            delta_emittance_error.Fill( ( down_em - up_em ) )


            if correction_upstream_matrix != None  and correction_downstream_matrix != None :
              correct_up = cov_up + correction_upstream_matrix
              correct_down = cov_down + correction_downstream_matrix
              up_em = math.sqrt( math.sqrt( numpy.linalg.det( correct_up ) ) ) / muon_mass
              down_em = math.sqrt( math.sqrt( numpy.linalg.det( correct_down ) ) ) / muon_mass
            
              upstream_emittance_error_corrected.Fill( up_em )
              downstream_emittance_error_corrected.Fill( down_em )
              fractional_emittance_error_corrected.Fill( ( down_em / up_em ) - 1.0 )
              delta_emittance_error_corrected.Fill( ( down_em - up_em ) )


            current_upstream = xboa.Bunch.Bunch()
            current_downstream = xboa.Bunch.Bunch()
            current_num = 0

# Save the MC-Recon residuals
        if save_residuals :
          residual_x_up.Fill( recon_up.get_x() - mc_up.get_x() )
          residual_y_up.Fill( recon_up.get_y() - mc_up.get_y() )
          residual_z_up.Fill( recon_up.get_z() - mc_up.get_z() )
          residual_px_up.Fill( recon_up.get_px() - mc_up.get_px() )
          residual_py_up.Fill( recon_up.get_py() - mc_up.get_py() )
#          residual_px_up.Fill( ( recon_up.get_px() / recon_up.get_pz() ) - ( mc_up.get_px() / mc_up.get_pz() ) )
#          residual_py_up.Fill( ( recon_up.get_py() / recon_up.get_pz() ) - ( mc_up.get_py() / mc_up.get_pz() ) )
          residual_pt_up.Fill( recon_up.get_pt() - mc_up.get_pt() )
          residual_pz_up.Fill( recon_up.get_pz() - mc_up.get_pz() )
          residual_phi_up.Fill( math.atan2( recon_up.get_y(), recon_up.get_x() ) -\
                                math.atan2( mc_up.get_y(), mc_up.get_x() ) )
          residual_pt_pz_up.Fill( recon_up.get_pt() - mc_up.get_pt(), mc_up.get_pz() )
          residual_pz_pz_up.Fill( recon_up.get_pz() - mc_up.get_pz(), mc_up.get_pz() )

          residual_x_down.Fill( recon_down.get_x() - mc_down.get_x() )
          residual_y_down.Fill( recon_down.get_y() - mc_down.get_y() )
          residual_z_down.Fill( recon_down.get_z() - mc_down.get_z() )
          residual_px_down.Fill( recon_down.get_px() - mc_down.get_px() )
          residual_py_down.Fill( recon_down.get_py() - mc_down.get_py() )
#          residual_px_down.Fill( ( recon_down.get_px() / recon_down.get_pz() ) - ( mc_down.get_px() / mc_down.get_pz() ) )
#          residual_py_down.Fill( ( recon_down.get_py() / recon_down.get_pz() ) - ( mc_down.get_py() / mc_down.get_pz() ) )
          residual_pt_down.Fill( recon_down.get_pt() - mc_down.get_pt() )
          residual_pz_down.Fill( recon_down.get_pz() - mc_down.get_pz() )
          residual_phi_down.Fill( math.atan2( recon_down.get_y(), recon_down.get_x() ) -\
                                math.atan2( mc_down.get_y(), mc_down.get_x() ) )
          residual_pt_pz_down.Fill( recon_down.get_pt() - mc_down.get_pt(), mc_down.get_pz() )
          residual_pz_pz_down.Fill( recon_down.get_pz() - mc_down.get_pz(), mc_down.get_pz() )

          p_value_up.Fill( recon_up.get_p_value() )
          p_value_down.Fill( recon_down.get_p_value() )
            

# Check if we need to produce specific files
# Load the virtual plane hits data
        if save_graphs or save_data or len( inspect_planes ) > 0 :
          virtual_extractor.load_tracks( mc_event )
          virtuals = virtual_extractor.get_current_trackpoints()
          for plane, hit in virtuals :
            if hit.get_pid() != cut_pid :
              continue
            if len( virtual_bunches ) <= plane :
              for i in range( len( virtual_bunches ), plane ) :
                virtual_bunches.append( xboa.Bunch.Bunch() )
            virtual_bunches[ plane - 1 ].append( make_xboa_hit( hit ) )


    except ValueError as ex :
      print 'Caught an exception. Details :', ex
      print 'Continuing Analysis.'
      gc.collect()
      continue

  gc.collect()


####################################################################################################
## Finish up the while loop
####################################################################################################

  duplicate_print.actual_stdout.write( '\rAll ' + str( len( sys.argv ) - 1 ) + ' File(s) Loaded                            \n' )
  duplicate_print.actual_stdout.flush()

# Print information concerning the cuts applied
  print
  print "SCIFI_EXTRACTOR:", scifi_extractor.get_total_number_tracks(), "total"
  print "SCIFI_EXTRACTOR:", scifi_extractor.get_total_cut(), "cut"
  print "SCIFI_EXTRACTOR:", scifi_extractor.get_total_passed(), "passed"
  print
  for i in range( scifi_extractor.get_number_cuts() ) :
    print "SCIFI_EXTRACTOR:", scifi_extractor.print_cut( i )
  print "SCIFI_EXTRACTOR: Number Straight Track Events:", scifi_extractor.get_total_straight_tracks()
  print "SCIFI_EXTRACTOR: Missing Ref Point:", scifi_extractor.get_number_missing()
  print

  print
  print "VIRT_EXTRACTOR:", mc_extractor.get_total_number_trackpoints(), "total"
  print "VIRT_EXTRACTOR:", mc_extractor.get_total_cut(), "cut"
  print "VIRT_EXTRACTOR:", mc_extractor.get_total_passed(), "passed"
  print
  for i in range( mc_extractor.get_number_cuts() ) :
    print "VIRT_EXTRACTOR:", mc_extractor.print_cut( i )
  print "VIRT_EXTRACTOR: Missing Ref Point:", mc_extractor.get_number_missing()
  print
  print
  print "Number Mismatched Hits =", mismatch_counter
  print


  duplicate_print.actual_stdout.flush()

####################################################################################################
## Produce variaous plots and calculate usful numbers for the Full Analysis option
####################################################################################################

# Calculate the Single Particle Amplitude histograms
  if calculate_spa is True :
    cov_mat_1 = virtual1_bunch.covariance_matrix( [ 'x', 'px', 'y', 'py' ] )
    cov_mat_2 = virtual2_bunch.covariance_matrix( [ 'x', 'px', 'y', 'py' ] )
    for hit_1, hit_2 in itertools.izip( virtual1_bunch, virtual2_bunch ) :
      amp_1 = xboa.Bunch.Bunch.get_amplitude( virtual1_bunch, hit_1, ['x', 'y' ], cov_mat_1 )
      amp_2 = xboa.Bunch.Bunch.get_amplitude( virtual2_bunch, hit_2, ['x', 'y' ], cov_mat_2 )
      virt_SPA_up.Fill( amp_1 )
      virt_SPA_down.Fill( amp_2 )
      virt_SPA_change.Fill( amp_2 - amp_1 )

    cov_mat_1 = kalman1_bunch.covariance_matrix( [ 'x', 'px', 'y', 'py' ] )
    cov_mat_2 = kalman2_bunch.covariance_matrix( [ 'x', 'px', 'y', 'py' ] )
    for hit_1, hit_2 in itertools.izip( kalman1_bunch, kalman2_bunch ) :
      amp_1 = xboa.Bunch.Bunch.get_amplitude( kalman1_bunch, hit_1, ['x', 'y' ], cov_mat_1 )
      amp_2 = xboa.Bunch.Bunch.get_amplitude( kalman2_bunch, hit_2, ['x', 'y' ], cov_mat_2 )
      kalman_SPA_up.Fill( amp_1 )
      kalman_SPA_down.Fill( amp_2 )
      kalman_SPA_change.Fill( amp_2 - amp_1 )


  if calculate_full :

# Find some useful numbers
    virtual1_means = virtual1_bunch.mean( ['x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy', 'bz' ] )
    virtual2_means = virtual2_bunch.mean( ['x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy', 'bz' ] )
    kalman1_means = kalman1_bunch.mean( ['x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy', 'bz' ] )
    kalman2_means = kalman2_bunch.mean( ['x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy', 'bz' ] )

    virtual1_emittance = virtual1_bunch.get_emittance( ['x', 'y'] )
    virtual1_weight = virtual1_bunch.bunch_weight()
    virtual1_beta = virtual1_bunch.get_beta( ['x', 'y'] )

    virtual2_emittance = virtual2_bunch.get_emittance( ['x', 'y'] )
    virtual2_weight = virtual2_bunch.bunch_weight()
    virtual2_beta = virtual2_bunch.get_beta( ['x', 'y'] )


# Print the useful numbers
    print "=========================================="
    print " ---   USING VIRTUAL POINTS   --- "
    print "=========================================="
    print

    print
    print " ---  Tracker 1  --- "
    print 
    print "Weight       =  " + str( virtual1_weight )
    print "Beta         =  " + str( virtual1_beta )
    print "Mean x       =  " + str( virtual1_means[ 'x' ] )
    print "Mean y       =  " + str( virtual1_means[ 'y' ] )
    print "Mean z       =  " + str( virtual1_means[ 'z' ] )
    print "Mean pz      =  " + str( virtual1_means[ 'pz' ] )
    print "Mean p       =  " + str( virtual1_means[ 'p' ] )
    print "Mean Energy  =  " + str( virtual1_means[ 'energy' ] )
    print "Mean Bz      =  " + str( virtual1_means[ 'bz' ] )
    print "Emittance    =  " + str( virtual1_emittance )
    print
    print

    print " ---  Tracker 2  --- "
    print 
    print "Weight       =  " + str( virtual2_weight )
    print "Beta         =  " + str( virtual2_beta )
    print "Mean x       =  " + str( virtual2_means[ 'x' ] )
    print "Mean y       =  " + str( virtual2_means[ 'y' ] )
    print "Mean z       =  " + str( virtual2_means[ 'z' ] )
    print "Mean pz      =  " + str( virtual2_means[ 'pz' ] )
    print "Mean p       =  " + str( virtual2_means[ 'p' ] )
    print "Mean Energy  =  " + str( virtual2_means[ 'energy' ] )
    print "Mean Bz      =  " + str( virtual2_means[ 'bz' ] )
    print "Emittance    =  " + str( virtual2_emittance )
    print


    kalman1_emittance = kalman1_bunch.get_emittance( ['x', 'y'] )
    kalman1_weight = kalman1_bunch.bunch_weight()
    kalman1_beta = kalman1_bunch.get_beta( ['x', 'y'] )

    kalman2_emittance = kalman2_bunch.get_emittance( ['x', 'y'] )
    kalman2_weight = kalman2_bunch.bunch_weight()
    kalman2_beta = kalman2_bunch.get_beta( ['x', 'y'] )

    print
    print "=========================================="
    print " --- USING KALMAN FITTED TRACK POINTS --- "
    print "=========================================="
    print

    print
    print " ---  Tracker 1  --- "
    print 
    print "Weight       =  " + str( kalman1_weight )
    print "Beta         =  " + str( kalman1_beta )
    print "Mean x       =  " + str( kalman1_means[ 'x' ] )
    print "Mean y       =  " + str( kalman1_means[ 'y' ] )
    print "Mean z       =  " + str( kalman1_means[ 'z' ] )
    print "Mean pz      =  " + str( kalman1_means[ 'pz' ] )
    print "Mean p       =  " + str( kalman1_means[ 'p' ] )
    print "Mean Energy  =  " + str( kalman1_means[ 'energy' ] )
    print "Mean Bz      =  " + str( kalman1_means[ 'bz' ] )
    print "Emittance    =  " + str( kalman1_emittance )
    print
    print

    print " ---  Tracker 2  --- "
    print 
    print "Weight       =  " + str( kalman2_weight )
    print "Beta         =  " + str( kalman2_beta )
    print "Mean x       =  " + str( kalman2_means[ 'x' ] )
    print "Mean y       =  " + str( kalman2_means[ 'y' ] )
    print "Mean z       =  " + str( kalman2_means[ 'z' ] )
    print "Mean pz      =  " + str( kalman2_means[ 'pz' ] )
    print "Mean p       =  " + str( kalman2_means[ 'p' ] )
    print "Mean Energy  =  " + str( kalman2_means[ 'energy' ] )
    print "Mean Bz      =  " + str( kalman2_means[ 'bz' ] )
    print "Emittance    =  " + str( kalman2_emittance )
    print
    print "Creating plots. Please Wait."
    print

    duplicate_print.actual_stdout.flush()

# What files are we also creating?
    if save_data or save_graphs or len( inspect_planes ) > 0 :

# Print out plots relating to a specfic virtual plane on the order of the user
      if len( inspect_planes ) > 0 :
        print "Attempting to save", len( inspect_planes ), "virtual planes."
        outfile = ROOT.TFile.Open( os.path.join( output_directory, "measure_emittance_inspection_planes.root" ), "RECREATE" )
        for num in inspect_planes :
          if num >= len( virtual_bunches ) or len( virtual_bunches[num] ) == 0 or virtual_bunches[num].bunch_weight < 1.0 : 
            print "ERROR: Could not inspect Virtual Planes:", num
            continue
          print_plots( virtual_bunches[num], "inspect_plane_" + str( num ) + "_" )
        outfile.Close()
        

# Ignore bunches that aren't possible to plot. ROOT can be quite picky about it's food...
      print_bunches = [ virtual_bunches[ i ] for i in range( len( virtual_bunches ) ) if virtual_bunches[i].bunch_weight() > 1.0 and len( virtual_bunches[i] ) > 1 ]

# Produce a text file of the virtual plane information
      if save_data :
        with open( os.path.join( output_directory, "measure_emittance_virtual_plane_data.dat" ), 'w' ) as outfile :
          outfile.write( '# Position (x,y,z)   -   Momentum(x,y,z,tot)  -  Energy  -  Emittance  -  Beta  -  Alpha  -  Buch Weight\n' )
          for bunch in print_bunches :
            means = bunch.mean( [ 'x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy' ] )
            outfile.write( str( means['x'] ) + ' ' + str( means['y'] ) + ' ' + str( means['z'] ) + ' ' +
                           str( means['px'] ) + ' ' + str( means['py'] ) + ' ' + str( means['pz'] ) + ' ' +
                           str( means['p'] ) + ' ' + str( means['energy'] ) + ' ' +
                           str( bunch.get_emittance( ['x','y'] ) ) + ' ' + str( bunch.get_beta( ['x','y'] ) ) + ' ' + str( bunch.get_alpha( ['x','y'] ) ) + ' ' +
                           str( bunch.bunch_weight() ) + '\n' )


# Produce a Root file with MC information in graphs
      if save_graphs :
        outfile = ROOT.TFile.Open( os.path.join( output_directory, "measure_emittance_graphs.root" ), "RECREATE" )
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'bunch_weight', [''], 'm', '' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'emittance', ['x','y'], 'm', 'mm' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'emittance', ['t'], 'm', 'mm' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'emittance', ['x','y','t'], 'm', 'mm' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'beta', ['x','y'], 'm', 'mm' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'beta', ['t'], 'm', 'ns' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'alpha', ['x','y'], 'm', 'mm' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'alpha', ['t'], 'm', 'ns' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'mean', ['energy'], 'm', 'MeV' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'mean', ['p'], 'm', 'MeV' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'mean', ['pz'], 'm', 'MeV' )
        graph.Write()
        canvas.Close()
# This doesn't yet work properly...
#      canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'standard_deviation', ['pz'], 'm', 'MeV' )
#      graph.Write()
#      canvas.Close()  
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'mean', ['r'], 'm', 'm' )
        graph.Write()
        canvas.Close()
        outfile.Close()


# Save emittance plots at the reference planes
    if save_emittance :
      outfile = ROOT.TFile.Open( os.path.join( output_directory, "measure_emittance_emittance.root" ), "RECREATE" )
      print_plots( virtual1_bunch, "virtual_1_" )
      print_plots( virtual2_bunch, "virtual_2_" )
      print_plots( kalman1_bunch, "kalman_1_" )
      print_plots( kalman2_bunch, "kalman_2_" )
      outfile.Close()



####################################################################################################
## Print out the differences between the MC and the Recon
####################################################################################################
    print
    print "=========================================="
    print " ---  KALMAN-VIRTUALS ERRORS SUMMARY  --- "
    print "=========================================="
    print
    print " ---  Tracker 1  --- "
    print
    print "Emittance      : ", ( kalman1_emittance - virtual1_emittance ), " mm (", ( kalman1_emittance - virtual1_emittance )/( 0.01*virtual1_emittance ), "% )"
    print "Beta Function  : ", ( kalman1_beta - virtual1_beta ), " mm (", ( kalman1_beta - virtual1_beta )/( 0.01*virtual1_beta ), "% )"
    print "Energy         : ", ( kalman1_means[ 'energy' ] - virtual1_means[ 'energy' ] ), " MeV (", ( kalman1_means[ 'energy' ] - virtual1_means[ 'energy' ] )/( 0.01*virtual1_means[ 'energy' ] ), "% )"
    print "Z Momentum     : ", ( kalman1_means[ 'pz' ] - virtual1_means[ 'pz' ] ), " MeV (", ( kalman1_means[ 'pz' ] - virtual1_means[ 'pz' ] )/( 0.01*virtual1_means[ 'pz' ] ), "% )"
    print "Z Position     : ", ( kalman1_means[ 'z' ] - virtual1_means[ 'z' ] )
    print
    print " ---  Tracker 2  --- "
    print
    print "Emittance      : ", ( kalman2_emittance - virtual2_emittance ), " mm (", ( kalman2_emittance - virtual2_emittance )/( 0.01*virtual2_emittance ), "% )"
    print "Beta Function  : ", ( kalman2_beta - virtual2_beta ), " mm (", ( kalman2_beta - virtual2_beta )/( 0.01*virtual2_beta ), "% )"
    print "Energy         : ", ( kalman2_means[ 'energy' ] - virtual2_means[ 'energy' ] ), " MeV (", ( kalman2_means[ 'energy' ] - virtual2_means[ 'energy' ] )/( 0.01*virtual2_means[ 'energy' ] ), "% )"
    print "Z Momentum     : ", ( kalman2_means[ 'pz' ] - virtual2_means[ 'pz' ] ), " MeV (", ( kalman2_means[ 'pz' ] - virtual2_means[ 'pz' ] )/( 0.01*virtual2_means[ 'pz' ] ), "% )"
    print "Z Position     : ", ( kalman2_means[ 'z' ] - virtual2_means[ 'z' ] )
    print
    print
    print "Saving Residuals Root File"
    print

    duplicate_print.actual_stdout.flush()


####################################################################################################
## Save the residuals
####################################################################################################
  if save_residuals :
    rootOutFile = ROOT.TFile.Open( os.path.join( output_directory, "measure_emittance_residuals.root" ), "RECREATE" )
    residual_x_up.Write()
    residual_y_up.Write()
    residual_z_up.Write()
    residual_px_up.Write()
    residual_py_up.Write()
    residual_pt_up.Write()
    residual_pz_up.Write()
    residual_phi_up.Write()
    residual_pt_pz_up.Write()
    residual_pz_pz_up.Write()

    residual_x_down.Write()
    residual_y_down.Write()
    residual_z_down.Write()
    residual_px_down.Write()
    residual_py_down.Write()
    residual_pt_down.Write()
    residual_pz_down.Write()
    residual_phi_down.Write()
    residual_pt_pz_down.Write()
    residual_pz_pz_down.Write()

    p_value_up.Write()
    p_value_down.Write()
    rootOutFile.Close()


####################################################################################################
## Save the single particle amplitude histograms
####################################################################################################
  if calculate_spa :
    rootOutFile = ROOT.TFile.Open( os.path.join( output_directory, "measure_emittance_single_particle_amplitudes.root" ), "RECREATE" )
    virt_SPA_up.Write()
    virt_SPA_down.Write()
    kalman_SPA_up.Write()
    kalman_SPA_down.Write()
    virt_SPA_change.Write()
    kalman_SPA_change.Write()
    rootOutFile.Close()


####################################################################################################
## Covariance Matrix Corrections
####################################################################################################
#    print "=========================================="
#    print "    ---    COVARIANCE MATRICES    ---     "
#    print "=========================================="
#    print
#    print "Calculating Covariance Matrices"
#    print "Please Wait."
#    print 
#  
#    cov_up = kalman1_bunch.covariance_matrix( ['x','px','y','py'] )
#    cov_down = kalman2_bunch.covariance_matrix( ['x','px','y','py'] )
#  
#    true_up = virtual1_bunch.covariance_matrix( ['x','px','y','py'] )
#    true_down = virtual2_bunch.covariance_matrix( ['x','px','y','py'] )
#  
#    correct_up = None
#    correct_down = None
#  
#  # If we have already loaded them from a file - use those! Otherwise use the ones from this dataset
#    if correction_upstream_matrix != None :
#      correct_up = cov_up + correction_upstream_matrix
#    else :
#      correct_up = correction_up.correct_covariance( cov_up )
#  
#    if correction_downstream_matrix != None :
#      correct_down = cov_down + correction_downstream_matrix
#    else :
#      correct_down = correction_down.correct_covariance( cov_down )
#  
#    print
#    print "Uncorrected Upstream Emittance    = ", covariances.emittance_from_matrix( cov_up  )
#    print "True Upstream Emittance           = ", covariances.emittance_from_matrix( true_up )
#    print "Corrected Upstream Emittance      = ", covariances.emittance_from_matrix( correct_up )
#    print
#  
#    print
#    print "Uncorrected Downstream Emittance  = ", covariances.emittance_from_matrix( cov_down )
#    print "True Downstream Emittance         = ", covariances.emittance_from_matrix( true_down )
#    print "Corrected Downstream Emittance    = ", covariances.emittance_from_matrix( correct_down )
#    print


####################################################################################################
## Covariance matrix corrections for this dataset
####################################################################################################
  if calculate_corrections is True :
    correction_up.save_R_matrix( os.path.join( output_directory, 'measure_emittance_R_matrix_up.dat' ) )
    correction_down.save_R_matrix( os.path.join( output_directory, 'measure_emittance_R_matrix_down.dat' ) )
    correction_up.save_C_matrix( os.path.join( output_directory, 'measure_emittance_C_matrix_up.dat' ) )
    correction_down.save_C_matrix( os.path.join( output_directory, 'measure_emittance_C_matrix_down.dat' ) )
    correction_up.save_full_correction( os.path.join( output_directory, 'measure_emittance_correction_matrix_up.dat' ) )
    correction_down.save_full_correction( os.path.join( output_directory, 'measure_emittance_correction_matrix_down.dat' ) )

#    numpy.savetxt( os.path.join( output_directory, 'measure_emittance_upstream_covariance.dat' ), true_up  )
#    numpy.savetxt( os.path.join( output_directory, 'measure_emittance_downstream_covariance.dat' ), true_down  )


####################################################################################################
## Save the histogrammed emittance measurements
####################################################################################################
  if calculate_errors :
    rootOutFile = ROOT.TFile.Open( os.path.join( output_directory, "measure_emittance_errors.root" ), "RECREATE" )
    upstream_emittance_error.Write()
    downstream_emittance_error.Write()
    fractional_emittance_error.Write()
    delta_emittance_error.Write()
    upstream_emittance_error_corrected.Write()
    downstream_emittance_error_corrected.Write()
    fractional_emittance_error_corrected.Write()
    delta_emittance_error_corrected.Write()
    rootOutFile.Close()


  print "Complete."
  print




####################################################################################################
####################################################################################################
# A couple of useful functions

def make_xboa_hit( hit ) :
  hit = xboa.Hit.Hit.new_from_dict(
      { 'x' : hit.get_x(),
        'y' : hit.get_y(),
        'z' : hit.get_z(),
        'px' : hit.get_px(),
        'py' : hit.get_py(),
        'pz' : hit.get_pz(),
        't' : hit.get_time(),
        'pid' : abs( hit.get_pid() ),
        'mass' : xboa.Common.pdg_pid_to_mass[ abs( hit.get_pid() ) ],
        'event_number' : make_xboa_hit.counter }, 'energy' )
  make_xboa_hit.counter += 1

  return hit

make_xboa_hit.counter = 0


def print_plots( bunch, prefix ) :
  canvas, hist = bunch.root_histogram( 'x', 'm', 'px', 'MeV/c', nx_bins = 1000, ny_bins = 1000 )
  hist.Write( prefix + "x-px_phasespace" )
  canvas.Close()
  canvas, hist = bunch.root_histogram( 'y', 'm', 'py', 'MeV/c', nx_bins = 1000, ny_bins = 1000 )
  hist.Write( prefix + "y-py_phasespace")
  canvas.Close()
  canvas, hist = bunch.root_histogram( 't', 'ns', 'energy', 'MeV', nx_bins = 1000, ny_bins = 1000 )
  hist.Write( prefix + "t-E_phasespace" )
  canvas.Close()
  canvas, hist = bunch.root_histogram( 'pt', 'MeV/c' )
  hist.Write( prefix + "pt_hist" )
  canvas.Close()
  canvas, hist = bunch.root_histogram( 'pz', 'MeV/c' )
  hist.Write( prefix + "pz_hist" )
  canvas.Close()


#  filename = prefix+"hitList.txt"
#
#  with open( filename, 'w' ) as outfile :
#    outfile.write( '#  Position (x,y,z)  |  Momentum (px,py,pz)  |  Magnetic Field (bz) \n\n' )
#
#    for hit in bunch :
#      outfile.write( str(hit.get('x')) + ' '
#                     + str(hit.get( 'y' )) + ' '
#                     + str(hit.get( 'z' )) + ' '
#                     + str(hit.get( 'px' )) + ' '
#                     + str(hit.get( 'py' )) + ' '
#                     + str(hit.get( 'pz' )) + ' '
#                     + str(hit.get( 'bz' )) + '\n' )




####################################################################################################
####################################################################################################



#try :

if __name__ == "__main__":

  ROOT.gROOT.SetBatch( True )

  parser = argparse.ArgumentParser( description='Performs emittance and beta function calculations on MAUS data files.\n\nMany of the functions require the simualation to be perfomed with virtual planes located within the tracker planes within the geometry file. If this is not the case no Monte Carlo information will be available for analysis.' )

  parser.add_argument( 'maus_root_files', nargs='+', help='List of Root files, generated by MAUS to analyse' )

  operations = parser.add_argument_group( title='Operations', description='Controls the main modes of operation for the analysis.' )
  operations.add_argument( '-F', '--calculate_full', action='store_true', help='Run full emittance calculation routines. Most options require this mode of operation.' )
  operations.add_argument( '-E', '--calculate_errors', action='store_true', help='Histogram emittance measurements for small ensembles of particles, allowing an error to be determined.' )
  operations.add_argument( '-S', '--calculate_spa', action='store_true', help='Calculate the Single Particle Amplitude for every trackpoint and histogram the values.' )
  operations.add_argument( '-C', '--calculate_corrections', action='store_true', help='Calculate the covariance matrix corrections for the emittance measurements.' )


  output = parser.add_argument_group( title='Output', description='Change the output from the script.' )
  output.add_argument( '-s', '--save_summary', action='store_true', help='Save a file containing the output from the analsys script.' )
  output.add_argument( '-v', '--save_variables', action='store_true', help='Save a python script containing all the variables used or calculated during the running of the analysis script.' )
  output.add_argument( '-d', '--save_data', action='store_true', help='Save a data file storing information from the virtual planes for later analysis.' )
  output.add_argument( '-r', '--save_residuals', action='store_true', help='Save plots of position and momentum residuals for the reconstructed trackpoints.' )
  output.add_argument( '-e', '--save_emittance', action='store_true', help='Save emittance plots for the reference planes.' )
#    output.add_argument( '-c', '--save_corrections', action='store_true', help='Save the covariance correction matrices to be used later.' )
  output.add_argument( '-g', '--save_graphs', action='store_true', help='Save graphs plotting the energy, momentum, emittance and beta function of the beam through the cooling channel.' )
  output.add_argument( '-p', '--inspect_planes', default=[], type=int, nargs='+', help='Enter a list of integers. Virtual planes with these ID numbers will have a verbose selection of plots produced.' )

  options = parser.add_argument_group( title='Options', description='Control the behaviour of the script.' )
  options.add_argument( '--output_directory', default='./', help='Change the location of the output directory in which to store the produced files.', metavar='DIRECTORY' )
  options.add_argument( '--manual_associate_planes', action='store_true', help='Tells the analysis script to assume the specified virtual plane numbers correspond to the reference planes. (See "upstream_virtual" and "downstream_virtual").' )
  options.add_argument( '--associate_tolerance', type=float, default=1.0, help='Select how closely the virtuals must be to the tracker planes in order to be assiciated with them.', metavar='TOLERANCE' )
  options.add_argument( '--associate_max_attempts', type=int, default=100, help='Number of events to use to try to associate the virtual planes with the reconstruction planes before giving up' )
  options.add_argument( '--recon_plane', type=int, default=0, help='Choose the reconstruction plane in the trackers. This is the location that the emittance will be reconstructed for cooling calculations.', metavar='PLANE' )
  options.add_argument( '--recon_station', type=int, default=1, help='Choose the reconstruction station in the trackers. This is the location that the emittance will be reconstructed for coolign calculations.', metavar='STATION' )
  options.add_argument( '--emittance_bin_size', type=int, default=2000, help='Choose the number of muons in each ensemble for histogrammed emittance calculations.', metavar='SIZE' )
  options.add_argument( '--correction_upstream', help='Enter the location of the upstream correction matrix to use in emittance calculations.', metavar='FILENAME' )
  options.add_argument( '--correction_downstream', help='Enter the location of the downstream correction matrix to use in emittance calculations.', metavar='FILENAME' )
  options.add_argument( '--upstream_virtual', type=int, default=-1, help='Manually specify the virtual plane number that corresponds to the upstream reference plane' )
  options.add_argument( '--downstream_virtual', type=int, default=-1, help='Manually specify the virtual plane number that corresponds to the downstream reference plane' )
  options.add_argument( '--no_virtuals', action='store_true', help="Forces the script to perform Monte Carlo recostruction using the SciFi Hits objects, rather than Virtual Hits." )
  options.add_argument( '--max_num_events',type=int, default=-1, help='Specify the maxmum number of events to analyse. Useful if you only have large datasets.' )
  options.add_argument( '--correction_directory', default=None, help='Specify a directory in which the up- and downstream correction matrices can be found. They are expected to be names "measure_emittance_correction_matrix_up.dat, etc.' )


  cuts = parser.add_argument_group( title='Cuts', description='Control cuts that can be applied the data being loaded.' )
  cuts.add_argument( '--cut_p_value', type=float, default=0.05, help='Cut *reconstructed* trackpoints with a P-Value lower than the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_pt', type=float, default=150.0, help='Cut *reconstructed* trackpoints with a transverse momentum above the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_low_pt', type=float, default=0.0, help='Cut *reconstructed* trackpoints with a transverse momentum below the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_pz', type=float, default=300.0, help='Cut *reconstructed* trackpoints with a longitudinal momenutm greater than the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_4point_tracks', action='store_true', help='Prevent *reconstructed* tracks with only 4 points from being used.' )
  cuts.add_argument( '--cut_radius', type=float, default=200.0, help='Cut *virtual* trackpoints with a radius greater than the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_pid', type=int, default=-13, help='Cut *virtual* trackpoints with an Particle ID not equal to the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_virtuals', action='store_true', help='Allow *virtual* trackpoint cuts to affect a cut on the reconstructed trackpoints.' )


################################################################################


  try :
    namespace = parser.parse_args()
  except :
    pass
  else :



################################################################################

    try :

      if namespace.correction_directory is not None :
        namespace.correction_upstream = os.path.join( namespace.correction_directory, "measure_emittance_correction_matrix_up.dat" )
      if namespace.correction_directory is not None :
        namespace.correction_downstream = os.path.join( namespace.correction_directory, "measure_emittance_correction_matrix_down.dat" )

## Better check those arguments before we hand them over!
      if not os.path.isdir( namespace.output_directory ) :
        raise ValueError( str( namespace.output_directory ) + ' not found' )
      if namespace.recon_plane > 2 or namespace.recon_plane < 0 :
        raise ValueError( 'Reference plane number outside allowed range' )
      if namespace.recon_station > 5 or namespace.recon_station < 1 :
        raise ValueError( 'Reference station number outside allowed range' )
      if namespace.correction_upstream is not None and not os.path.isfile( namespace.correction_upstream ) :
        raise ValueError( str( namespace.correction_upstream ) + ' not found' )
      if namespace.correction_downstream is not None and not os.path.isfile( namespace.correction_downstream ) :
        raise ValueError( str( namespace.correction_downstream ) + ' not found' )
      if namespace.associate_tolerance < 0.0 :
        raise ValueError( 'Association tolerance must be positive' )
      if namespace.associate_max_attempts < 0 :
        raise ValueError( 'Number of association attempts must be a positive integer' )
      if namespace.emittance_bin_size < 0 :
        raise ValueError( 'Negative emittance bin size not allowed' )
      if namespace.cut_p_value < 0.0 or namespace.cut_p_value > 1.0 :
        raise ValueError( 'P Value cut outside allowed range of values' )
      if namespace.cut_pt < 0.0 :
        raise ValueError( 'Transverse Momentum cut cannot be negative' )
      if namespace.cut_low_pt < 0.0 :
        raise ValueError( 'Transverse Momentum cut cannot be negative' )
      if namespace.cut_low_pt > namespace.cut_pt :
        raise ValueError( 'Low PT cut cannot be greater than the PT cut' )
      if namespace.cut_pz < 0.0 :
        raise ValueError( 'Longitudinal Momentum cut cannot be negative' )
      if namespace.cut_radius < 0.0 :
        raise ValueError( 'Radial cut cannot be negative' )
#      if namespace.upstream_virtual < 0 :
#        raise ValueError( 'Virtual plane numbers must be positive' )
#      if namespace.downstream_virtual < 0 :
#        raise ValueError( 'Virtual plane numbers must be positive' )
      if ( namespace.manual_associate_planes is True ) and ( ( namespace.upstream_virtual == 0 ) or ( namespace.downstream_virtual == 0 ) ) :
        raise ValueError( 'Must specify upstream_virtual and downstream_virtual when using manual plane association' )
      if namespace.save_variables is True :
        print
        print 'WARNING: Parameter "--save_variables" not yet implemented.'
        print
      if namespace.no_virtuals is True :
        if namespace.save_data is True or namespace.save_graphs is True  or len( namespace.inspect_planes ) > 0 :
          raise ValueError( 'Cannot produce Data Files or Graphs if no_virtuals flag is set' )

    except :
      print "An error was found with your arguments."
      raise

    print
    run( **vars( namespace ) )
    print

#except BaseException as ex :
#  print
#  print "An error occured."
#  print "Details: " + str( ex )
#  print
#  print "Exiting"
#  print
#  raise


