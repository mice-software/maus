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
import json
import array
import operator
import random

# definitions of MAUS data structure for PyROOT
import libMausCpp #pylint: disable = W0611

# Local emittance analysis library to accompany xboa
import emittance_analysis as em_ana


####################################################################################################
## HISTOGRAMS AND GRAPHS
####################################################################################################

def make_histograms( namespace ) :

  plot_dict = {}
  data_dict = {}

  plot_dict["beamline"] = {}
  plot_dict["distributions"] = {}
  plot_dict["phasespace"] = {}
  plot_dict["spa"] = {}
  plot_dict["errors"] = {}

  data_dict["covariances"] = {}
  data_dict["counters"] = {}

# Beamline

# Distributions
  for tr in namespace.recon_trackers :
    station_dict = {}
    for st in namespace.recon_stations :
      plane_dict = {}
      for pl in namespace.recon_planes :
        hists = {}
        hists["x"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_X", "X Positions", 200, -200.0, 200.0 )
        hists["y"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_Y", "Y Positions", 200, -200.0, 200.0 )
        hists["z"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_Z", "Z Positions", 200, -10000.0, 10000.0 )
        hists["r"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_R", "Radial Positions", 200, 0.0, 200.0 )
        hists["px"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_Px", "X Momentum", 200, -200.0, 200.0 )
        hists["py"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_Py", "Y Momentum", 200, -200.0, 200.0 )
        hists["pz"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_Pz", "Z Momentum", 200, 0.0, 500.0 )
        hists["pt"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_Pt", "Transverse Momentum", 200, 0.0, 200.0 )
        hists["p"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_P", "Total Momentum", 200, 0.0, 500.0 )
        hists["emittance"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_emittance", "Emittance", 100, 0.0, 20.0 )
        hists["beta"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_beta", "Beta", 200, 0.0, 2000.0 )
        hists["energy"] = ROOT.TH1F( str(tr)+"_"+str(st)+"_"+str(pl)+"_energy", "Energy", 200, 0.0, 500.0 )

        plane_dict[pl] = hists
      station_dict[st] = plane_dict
    plot_dict["distributions"][tr] = station_dict

# Phase Space
  for tr in namespace.recon_trackers :
    station_dict = {}
    for st in namespace.recon_stations :
      plane_dict = {}
      for pl in namespace.recon_planes :
        hists = {}
        hists["x_px"] = ROOT.TH2F( str(tr)+"_"+str(st)+"_"+str(pl)+"_x-px", "X-Px Phase Space", 100, -200.0, 200.0, 100, -200.0, 200.0 )
        hists["y_py"] = ROOT.TH2F( str(tr)+"_"+str(st)+"_"+str(pl)+"_y-py", "Y-Py Phase Space", 100, -200.0, 200.0, 100, -200.0, 200.0 )
        hists["t_E"] = ROOT.TH2F( str(tr)+"_"+str(st)+"_"+str(pl)+"_t-E", "T-E Phase Space", 100, -200.0, 200.0, 100, 0.0, 400.0 )
        hists["x_y"] = ROOT.TH2F( str(tr)+"_"+str(st)+"_"+str(pl)+"_x-y", "X-Y Phase Space", 100, -200.0, 200.0, 100, -200.0, 200.0 )

        plane_dict[pl] = hists
      station_dict[st] = plane_dict
    plot_dict["phasespace"][tr] = station_dict

# SPA

# Errors

# Data Setup
  for tr in namespace.recon_trackers :
    station_dict = {}
    for st in namespace.recon_stations :
      plane_dict = {}
      for pl in namespace.recon_planes :
        plane_dict[pl] = em_ana.covariances.covariance_matrix()
      station_dict[st] = plane_dict
    data_dict["covariances"][tr] = station_dict

  return plot_dict, data_dict


####################################################################################################
## DO THE WORK
####################################################################################################

def run_analysis( namespace, plot_dict, data_dict, file_list ) :

  correction_upstream_matrix = None
  correction_downstream_matrix = None
  if namespace.correction_upstream != None :
    correction_upstream_matrix = numpy.loadtxt( namespace.correction_upstream )
    print "Loaded Upstream Correction Matrix"
  if namespace.correction_downstream != None :
    correction_downstream_matrix = numpy.loadtxt( namespace.correction_downstream )
    print "Loaded Downstream Correction Matrix"

## Initialise the file reader with ALL the files.
  file_reader = em_ana.extraction.maus_reader( file_list )
  number_files = file_reader.get_number_files()
  print "Loading", number_files, "File(s)"
  print


# Prepare the user defined cuts to be applied later
  scifi_extractor = em_ana.extraction.scifi_tracks()
  for tr in namespace.recon_trackers :
    for st in namespace.recon_stations :
      for pl in namespace.recon_planes :
        scifi_extractor.add_reference_plane( tr, st, pl, 'ref_'+str(tr)+"_"+str(st)+"_"+str(pl) )
  scifi_extractor.add_cut( 'pvalue', namespace.cut_p_value, operator.lt )
  scifi_extractor.add_cut( 'pt', namespace.cut_pt, operator.gt )
  scifi_extractor.add_cut( 'pz', namespace.cut_pz, operator.gt )
  scifi_extractor.add_cut( 'pt', namespace.cut_low_pt, operator.lt )
  scifi_extractor.add_cut( 'pid', namespace.cut_pid, operator.ne )
  if namespace.cut_4point_tracks :
    scifi_extractor.set_min_num_trackpoints( 5 )

  random_plane = 0
  random_station = 1

# Iterate through all events of all files and analyse what we find
# and catch the keyboard interrupts
  try :

    while file_reader.next_event() and file_reader.get_total_num_events() != namespace.max_num_events :
      try :

# Print current position - it looks nice!
        em_ana.duplicate_print.actual_stdout.write( ' Spill ' + str( file_reader.get_current_spill_number() ) + ' of ' + str( file_reader.get_current_number_spills() ) + ' in File ' + str( file_reader.get_current_filenumber() ) + ' of ' + str( file_reader.get_number_files() ) + '   \r' )
        em_ana.duplicate_print.actual_stdout.flush()

# Load Scifi
        scifi_event = file_reader.get_event( 'scifi' )
        scifi_extractor.load_tracks( scifi_event )
        ext_cut = scifi_extractor.is_cut()

# Check the cuts
        if ext_cut :
          continue

        trackpoints = scifi_extractor.get_current_trackpoints()

# Randomly select stations and planes to measure on - reducing bias!
        if namespace.unbias_results :
          random_plane = random.choice( namespace.recon_planes )
          random_station = random.choice( namespace.recon_stations )

        for location, tp in trackpoints :
          tracker, station, plane = location

          if namespace.unbias_results :
            if station != random_station or plane != random_plane :
              continue
          elif tracker not in namespace.recon_trackers or \
               station not in namespace.recon_stations or \
               plane   not in namespace.recon_planes :
             continue

          dist_plots = plot_dict["distributions"][tracker][station][plane]
          dist_plots["x"].Fill( tp.get_x() )
          dist_plots["y"].Fill( tp.get_y() )
          dist_plots["z"].Fill( tp.get_z() )
          dist_plots["r"].Fill( tp.get_r() )
          dist_plots["px"].Fill( tp.get_px() )
          dist_plots["py"].Fill( tp.get_py() )
          dist_plots["pz"].Fill( tp.get_pz() )
          dist_plots["pt"].Fill( tp.get_pt() )
          dist_plots["p"].Fill( tp.get_p() )
          dist_plots["energy"].Fill( tp.get_energy() )

          phas_plots = plot_dict["phasespace"][tracker][station][plane]
          phas_plots["x_px"].Fill( tp.get_x(), tp.get_px() )
          phas_plots["y_py"].Fill( tp.get_y(), tp.get_py() )
          phas_plots["t_E"].Fill( tp.get_time(), tp.get_energy() )
          phas_plots["x_y"].Fill( tp.get_x(), tp.get_y() )

          cov_mat = data_dict["covariances"][tracker][station][plane]
          cov_mat.add_hit( tp )

          if namespace.ensemble_size > 1 and cov_mat.length() > namespace.ensemble_size :
            dist_plots["emittance"].Fill( cov_mat.get_emittance( ['x','px','y','py'] ) )
            dist_plots["beta"].Fill( cov_mat.get_beta( ['x','y'], momentum=tp.get_p() ) )

            cov_mat.clear()

      except ValueError as ex :
        print 'Caught an exception. Details :', ex
        print 'Continuing Analysis.'
        gc.collect()
        continue

  except KeyboardInterrupt :
    print 'Caught Exception.'
    print 'Ending analysis gracefully, please wait...'
    print
    

# Clear up the remnants
  for tr in namespace.recon_trackers :
    for st in namespace.recon_stations :
      for pl in namespace.recon_planes :
        cov_mat = data_dict['covariances'][tr][st][pl]
        dist_plots = plot_dict['distributions'][tr][st][pl]

        dist_plots['emittance'].Fill( cov_mat.get_emittance( ['x','px','y','py'] ) )
        dist_plots['beta'].Fill( cov_mat.get_beta( ['x','y'], momentum=tp.get_p() ) )
        cov_mat.clear()

  print "Loaded All Spills                                                  "
  gc.collect()
  print



####################################################################################################
## PLOT THE PLOTS
####################################################################################################

def plot_all( namesapce, plot_dict, data_dict, file_list, series=None ) :

  dist_dict = plot_dict["distributions"]
  phas_dict = plot_dict["phasespace"]
  cova_dict = data_dict["covariances"]

  filename_root = "data_analysis_"
  if series is not None :
    filename_root += "series_" + str( series ) + "_"
    

# Distributions
  outfile = ROOT.TFile( os.path.join( namespace.output_directory, filename_root+"distributions.root" ), "RECREATE" )
  for tr in dist_dict.keys() :
    for st in dist_dict[tr].keys() :
      for pl in dist_dict[tr][st].keys() :
        for hist in dist_dict[tr][st][pl].keys() :
          dist_dict[tr][st][pl][hist].Write()
  outfile.Close()

# Phase Space
  outfile = ROOT.TFile( os.path.join( namespace.output_directory, filename_root+"phasespace.root" ), "RECREATE" )
  for tr in phas_dict.keys() :
    for st in phas_dict[tr].keys() :
      for pl in phas_dict[tr][st].keys() :
        for hist in phas_dict[tr][st][pl].keys() :
          phas_dict[tr][st][pl][hist].Write()
  outfile.Close()

# Covariances
  outfile = ROOT.TFile( os.path.join( namespace.output_directory, filename_root+"covariances.root" ), "RECREATE" )
  z_points = array.array( 'd' )

  x_points = array.array( 'd' )
  rms_x_points = array.array( 'd' )
  y_points = array.array( 'd' )
  rms_y_points = array.array( 'd' )
  r_points = array.array( 'd' )
  rms_r_points = array.array( 'd' )
  pt_points = array.array( 'd' )
  rms_pt_points = array.array( 'd' )
  pz_points = array.array( 'd' )
  rms_pz_points = array.array( 'd' )
  p_points = array.array( 'd' )
  rms_p_points = array.array( 'd' )
  emittance_points = array.array( 'd' )
  beta_points = array.array( 'd' )

  zeros = array.array( 'd' )

  data = {}

  for tr in cova_dict.keys() :
    station_data = {}
    for st in cova_dict[tr].keys() :
      plane_data = {}
      for pl in cova_dict[tr][st].keys() :
        recon_data = {}
#        cov_mat = cova_dict[tr][st][pl]
        hists = dist_dict[tr][st][pl]

#        number_particles = cov_mat.length()
#        error_factor = 1.0 / math.sqrt( number_particles )

        x, s_x = em_ana.common.fit_gaussian( hists[ 'x' ] )
        y, s_y = em_ana.common.fit_gaussian( hists[ 'y' ] )
        z, s_z = em_ana.common.fit_gaussian( hists[ 'z' ] )
        r, s_r = em_ana.common.fit_gaussian( hists[ 'r' ] )
        px, s_px = em_ana.common.fit_gaussian( hists[ 'px' ] )
        py, s_py = em_ana.common.fit_gaussian( hists[ 'py' ] )
        pz, s_pz = em_ana.common.fit_gaussian( hists[ 'pz' ] )
        pt, s_pt = em_ana.common.fit_gaussian( hists[ 'pt' ] )
        p, s_p = em_ana.common.fit_gaussian( hists[ 'p' ] )
        energy, s_energy = em_ana.common.fit_gaussian( hists[ 'energy' ] )

        emittance, s_emittance = em_ana.common.fit_gaussian( hists[ 'emittance' ] )
        beta, s_beta = em_ana.common.fit_gaussian( hists[ 'beta' ] )

        recon_data['x'] = x
        recon_data['y'] = y
        recon_data['z'] = z
        recon_data['r'] = r
        recon_data['px'] = px
        recon_data['py'] = py
        recon_data['pz'] = pz
        recon_data['pt'] = pt
        recon_data['p'] = p
        recon_data['energy'] = energy
        recon_data['emittance'] = emittance
        recon_data['beta'] = beta
        recon_data['s_x'] = s_x
        recon_data['s_y'] = s_y
        recon_data['s_z'] = 0.0
        recon_data['s_r'] = s_r
        recon_data['s_px'] = s_px
        recon_data['s_py'] = s_py
        recon_data['s_pz'] = s_pz
        recon_data['s_pt'] = s_pt
        recon_data['s_p'] = s_p
        recon_data['s_energy'] = s_energy
        recon_data['s_emittance'] = s_emittance
        recon_data['s_beta'] = s_beta
        recon_data['weight'] = hists[ 'energy' ].GetEntries()

        z_points.append( z )
        zeros.append( 0.0 )

        x_points.append( x )
        rms_x_points.append( s_x )
        y_points.append( y )
        rms_y_points.append( s_y )
        r_points.append( r )
        rms_r_points.append( s_r )
        pt_points.append( pt ) 
        rms_pt_points.append( s_pt )
        pz_points.append( pz )
        rms_pz_points.append( s_pz )
        p_points.append( p )
        rms_p_points.append( s_p )

#        x_points.append( x )
#        rms_x_points.append( error_factor * math.sqrt( s_x ) )
#        y_points.append( y )
#        rms_y_points.append( error_factor * math.sqrt( s_y ) )
#        r_points.append( r )
#        rms_r_points.append( error_factor * math.sqrt( s_r ) )
#        pt_points.append( pt ) 
#        rms_pt_points.append( error_factor * math.sqrt( s_pt ) )
#        pz_points.append( pz )
#        rms_pz_points.append( error_factor * math.sqrt( s_pz ) )
#        p_points.append( p )
#        rms_p_points.append( error_factor * math.sqrt( s_p ) )

        emittance_points.append( emittance )
        beta_points.append( beta )
        plane_data[pl] = recon_data
      station_data[st] = plane_data
    data[tr] = station_data

  mean_x_graph = ROOT.TGraphErrors( len(z_points), z_points, x_points, zeros, rms_x_points )
  mean_x_graph.Write( "mean_x" )
  mean_y_graph = ROOT.TGraphErrors( len(z_points), z_points, y_points, zeros, rms_y_points )
  mean_y_graph.Write( "mean_y" )
  mean_r_graph = ROOT.TGraph( len(z_points), z_points, r_points )
  mean_r_graph.Write( "mean_r" )
  rms_r_graph = ROOT.TGraph( len(z_points), z_points, rms_r_points )
  rms_r_graph.Write( "rms_r" )
  mean_pt_graph = ROOT.TGraph( len(z_points), z_points, pt_points )
  mean_pt_graph.Write( "mean_pt" )
  rms_pt_graph = ROOT.TGraph( len(z_points), z_points, rms_pt_points )
  rms_pt_graph.Write( "rms_pt" )
  mean_pz_graph = ROOT.TGraph( len(z_points), z_points, pz_points )
  mean_pz_graph.Write( "mean_pz" )
  rms_pz_graph = ROOT.TGraph( len(z_points), z_points, rms_pz_points )
  rms_pz_graph.Write( "rms_pz" )
  mean_p_graph = ROOT.TGraph( len(z_points), z_points, p_points )
  mean_p_graph.Write( "mean_p" )
  rms_p_graph = ROOT.TGraph( len(z_points), z_points, rms_p_points )
  rms_p_graph.Write( "rms_p" )
  emittance_graph = ROOT.TGraph( len(z_points), z_points, emittance_points )
  emittance_graph.Write( "emittance" )
  beta_graph = ROOT.TGraph( len(z_points), z_points, beta_points )
  beta_graph.Write( "beta" )

  outfile.Close()

  with open( os.path.join( namespace.output_directory, filename_root+"raw_data.json" ), 'wb' ) as fp :
    json.dump( data, fp )


####################################################################################################
## OTHER FUNCTIONS
####################################################################################################

def file_list_split( list_of_lists ) :
  """
    Split list of files by the '--' delimeter
  """
  temp_list = []
  for item in list_of_lists :
    if item == "--" :
      if len( temp_list ) == 0 :
        continue
      yield temp_list
      temp_list = []
    else :
      temp_list.append( item )

  yield temp_list


def file_list_dir_split( list_of_lists ) :
  """
    Split list of files by the last directory they are in
  """
  temp_list = []
  last_dir = None
  for item in list_of_lists :
    current_dir = os.path.dirname( os.path.realpath( item ) )
    if current_dir != last_dir  :
      if last_dir is None or len( temp_list ) == 0 :
        last_dir = current_dir
        temp_list.append( item )
        continue
      else :
        yield temp_list
        temp_list = []
        last_dir = current_dir
    else :
      temp_list.append( item )

  yield temp_list


def multi_sort_lists( lists, key_list=0, descending=False ) :
  return izip( *sorted( izip( *lists ), reverse=descending, key=lambda x:x[0] ) )


####################################################################################################
## INITIALISE AND RUN
####################################################################################################

if __name__ == "__main__":
  ROOT.gROOT.SetBatch( True )

  parser = argparse.ArgumentParser( description='Performs emittance and beta function calculations on MAUS data files.\n\nMany of the functions require the simualation to be perfomed with virtual planes located within the tracker planes within the geometry file. If this is not the case no Monte Carlo information will be available for analysis.' )

# MAIN PARSER

  parser.add_argument( '-S', '--spa', action='store_true', help='Creates histograms of the Single Particle Amplitudes for the data. (Slow!)' )
  parser.add_argument( '-s', '--save_summary', action='store_true', help='Save a file containing the output from the analsys script.' )
  parser.add_argument( '-v', '--save_variables', action='store_true', help='Save a python script containing all the variables used or calculated during the running of the analysis script.' )
  parser.add_argument( '--output_directory', default='./', help='Change the location of the output directory in which to store the produced files.', metavar='DIRECTORY' )
  parser.add_argument( '--max_num_events',type=int, default=-1, help='Specify the maxmum number of events to analyse. Useful if you only have large datasets.' )
  parser.add_argument( '--ensemble_size', type=int, default=0, help='Set the size of the ensemble of particles to calculate emittance from. Zero uses the entire dataset.' )
  parser.add_argument( '-D', '--split_by_directory', action='store_true', help='Tells the script to split the supplied list of root files into different data series, depending on the directory they are in.' )
  parser.add_argument( 'maus_root_files', nargs=argparse.REMAINDER, help='List of Root files, generated by MAUS to analyse' )

# CUTS
  cuts = parser.add_argument_group( title='Cuts', description='Control cuts that can be applied the data being loaded.' )
  cuts.add_argument( '--cut_p_value', type=float, default=0.05, help='Cut *reconstructed* trackpoints with a P-Value lower than the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_pt', type=float, default=150.0, help='Cut *reconstructed* trackpoints with a transverse momentum above the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_low_pt', type=float, default=0.0, help='Cut *reconstructed* trackpoints with a transverse momentum below the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_pz', type=float, default=300.0, help='Cut *reconstructed* trackpoints with a longitudinal momenutm greater than the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_4point_tracks', action='store_true', help='Prevent *reconstructed* tracks with only 4 points from being used.' )
  cuts.add_argument( '--cut_radius', type=float, default=200.0, help='Cut *virtual* trackpoints with a radius greater than the value specified.', metavar='VAL' )
  cuts.add_argument( '--cut_pid', type=int, default=-13, help='Cut *virtual* trackpoints with an Particle ID not equal to the value specified.', metavar='VAL' )

# OTHER OPTIONS
  options = parser.add_argument_group( title='Options', description='Fine tune the behaviour of the script.' )

  options.add_argument( '--recon_planes', type=int, nargs='+', default=[0], help='Choose the reconstruction plane in the trackers. This is the location that the emittance will be reconstructed for cooling calculations.', metavar='PLANE' )
  options.add_argument( '--recon_stations', type=int, nargs='+', default=[1], help='Choose the reconstruction station in the trackers. This is the location that the emittance will be reconstructed for cooling calculations.', metavar='STATION' )
  options.add_argument( '--recon_trackers', type=int, nargs='+', default=[0,1], help='Choose the reconstruction tracker. This is the location that the emittance will be reconstructed for cooling calculations.', metavar='STATION' )
  options.add_argument( '--series_start', type=int, default=0, help='Choose the starting number of the series to be analysed. Useful if you don\'t want to overwrite previous data sets' )
  options.add_argument( '--unbias_results', action='store_true', help='Only measure one plane for each particle track. Means the number of particles is reduced by the number of measurement planes, but protectes the results from unintentional bias' )

  options.add_argument( '--correction_upstream', help='Enter the location of the upstream correction matrix to use in emittance calculations.', metavar='FILENAME' )
  options.add_argument( '--correction_downstream', help='Enter the location of the downstream correction matrix to use in emittance calculations.', metavar='FILENAME' )
  options.add_argument( '--correction_directory', default=None, help='Specify a directory in which the up- and downstream correction matrices can be found. They are expected to be named "measure_emittance_correction_matrix_up.dat, etc.' )

####################################################################################################

  try :
    namespace = parser.parse_args()
  except :
    pass
  else :

################################################################################

    try :

      if not os.path.isdir( namespace.output_directory ) :
        raise ValueError( str( namespace.output_directory ) + ' not found' )

# CUTS
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

# OPTIONS
      for plane_num in namespace.recon_planes :
        if plane_num > 2 or plane_num < 0 :
          raise ValueError( 'Reference plane number outside allowed range' )
      for station_num in namespace.recon_stations :
        if station_num > 5 or station_num < 1 :
          raise ValueError( 'Reference station number outside allowed range' )
      if namespace.save_variables is True :
        print
        print 'WARNING: Parameter "--save_variables" not yet implemented.'
        print

# OTHER FLAGS
      if namespace.correction_directory is not None :
        namespace.correction_upstream = os.path.join( namespace.correction_directory, "data_analysis_correction_matrix_up.dat" )
      if namespace.correction_directory is not None :
        namespace.correction_downstream = os.path.join( namespace.correction_directory, "data_analysis_correction_matrix_down.dat" )
      if namespace.correction_upstream is not None and not os.path.isfile( namespace.correction_upstream ) :
        raise ValueError( str( namespace.correction_upstream ) + ' not found' )
      if namespace.correction_downstream is not None and not os.path.isfile( namespace.correction_downstream ) :
        raise ValueError( str( namespace.correction_downstream ) + ' not found' )
#      if namespace.emittance_bin_size < 0 :
#        raise ValueError( 'Negative emittance bin size not allowed' )

    except :
      print "An error was found with your arguments."
      raise

# Copy the stdout to a text file if asked
    logging = None
    if namespace.save_summary :
      logging = em_ana.duplicate_print.Tee( os.path.join( output_directory, 'data_analysis_logging.txt' ) )

    splitter = None
    if namespace.split_by_directory :
      splitter = file_list_dir_split
    else :
      splitter = file_list_split

    for list_num, file_list in enumerate( splitter( namespace.maus_root_files ) ) :
      plot_dict, data_dict = make_histograms( namespace )
      list_num += namespace.series_start

      print
      print "Data Series:", list_num
      print

      run_analysis( namespace, plot_dict, data_dict, file_list )

      plot_all( namespace, plot_dict, data_dict, file_list, series=list_num )

      del plot_dict
      del data_dict
      gc.collect()
      ROOT.gROOT.Reset()


    print
    print "Complete."
    print


