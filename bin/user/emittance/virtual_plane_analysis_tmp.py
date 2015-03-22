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


import xboa.Bunch
from xboa.Bunch import Bunch

import ROOT
import math

import traceback
import operator
import json
import copy
import sys
import os
import gc


radial_cut = 190.0
cut_cylinder = False
pid_cut = -13
upstream_transverse_amplitude_cut = 72.0
downstream_transverse_amplitude_cut = 72.0

#inspection_positions = [ -3440.3, -1900.0, 0.0, 1900.0, 3440.3 ] # Reference
#inspection_positions = [ -3578.0, -1647.0, 0.0, 1647.0, 3578.0 ] # Alternative
#inspection_positions = [ -1831.7313, 0.0, 1831.6589 ] # Step 4 Recent
#inspection_positions = [ -1000.0, -500.0, 0.0, 500.0, 1000.0 ] 
inspection_positions = []
#reference_positions = [ -3440.3, 3440.3 ] # Reference
#reference_positions = [ -3578.0, 3578.0 ] # Alternative
#reference_positions = [ -1831.7313, 1831.6589 ] # Step 4 Recent
reference_positions = [ 15135.5087, 19877.0811 ] # Full geoemtry
print_phasespace = False
print_all_planes = True
cut_amplitudes = False
data_only = False

MAUS_root = os.environ.get( 'MAUS_ROOT_DIR' )
output_directory = os.path.join( MAUS_root, "./" )


def analyse_file( filename, directory ):
  bunch_list_tmp = []
  canvas_list = [ None for i in range( len( reference_positions ) ) ]
  DATA = { "emitt_trans_in" : 0.0,  "emitt_trans_out" : 0.0,
           "emitt_long_in" : 0.0,   "emitt_long_out" : 0.0,
           "emitt_6d_in" : 0.0,     "emitt_6d_out" : 0.0,
           "number_in" : 0,         "number_out" : 0 }

  print "Loading", filename
  print

  bunch_list_tmp = Bunch.new_list_from_read_builtin('maus_root_virtual_hit', filename)
  for bunch in bunch_list_tmp : bunch.cut( {'pid' : -13}, operator.ne, global_cut = True )

  if len( reference_positions ) > 1 :
    reference_plane_up = find_virtual_plane( bunch_list_tmp, reference_positions[0] )
    reference_plane_down = find_virtual_plane( bunch_list_tmp, reference_positions[-1] )
  else :
    reference_plane_up = None
    reference_plane_down = None

  if reference_plane_up is None or reference_plane_down is None :
    print "WARN:  Could not locate all reference planes."
    print "WARN:  Continuing anyway."
    print 

  print "Creating Transmission Plot"
  print

  outfile = ROOT.TFile( os.path.join( directory, "virtual_plane_results.root" ), "RECREATE" )
  canvas, hist, graph = Bunch.root_graph(bunch_list_tmp, 'mean', ['z'], 'bunch_weight', [''], 'mm')
  graph.Write()
  canvas.Close()
  outfile.Close()

  if reference_plane_up is not None and reference_plane_down is not None :
    DATA[ "number_in" ] = bunch_list_tmp[ reference_plane_up ].bunch_weight()
    DATA[ "number_out" ] = bunch_list_tmp[ reference_plane_down ].bunch_weight()

  print "Applying Cuts"
  print

  end_bunch = bunch_list_tmp[-1]
  for bunch in bunch_list_tmp :
    bunch.transmission_cut( end_bunch )

  bunch_list = [ bun for bun in bunch_list_tmp if len( bun ) > 1 and bun.bunch_weight() > 1.0e-9 ]

  del bunch_list_tmp
  gc.collect()

  start_bunch = bunch_list[0]

  if cut_cylinder :
    for bun in bunch_list :
      bun.cut( {'r' : radial_cut}, operator.ge, global_cut = True )
  else :
    start_bunch.cut( {'r' : radial_cut}, operator.ge, global_cut = True )
    end_bunch.cut( {'r' : radial_cut}, operator.ge, global_cut = True )

  if len( reference_positions ) >= 0 :
    print "Saving Before Distributions for Reference Planes"
    for num, pos in enumerate( reference_positions ) :
      plane = find_virtual_plane( bunch_list, pos )
      if plane is None :
        print "ERROR : Could not locate a virtual plane near z =", pos
        continue
      canvas_list[num] = print_distributions( bunch_list[plane], 2 )

  if cut_amplitudes :
    start_bunch.cut( {'amplitude x y' : upstream_transverse_amplitude_cut}, operator.ge, global_cut = True )
    end_bunch.cut( {'amplitude x y' : downstream_transverse_amplitude_cut}, operator.ge, global_cut = True )

  if len( reference_positions ) >= 0 :
    print "Saving After Distributions for Reference Planes"
    for num, pos in enumerate( reference_positions ) :
      plane = find_virtual_plane( bunch_list, pos )
      if plane is None :
        continue
      canvas_list[num] = print_distributions( bunch_list[plane], 1, canvas_list[num] )

  for i in range( len( canvas_list ) ) :
    outfile = ROOT.TFile( os.path.join( directory, "virtual_plane_reference_" + str( reference_positions[i] ) + ".root" ), "RECREATE" )
    if canvas_list[i] is None :
      continue
    for j in range( len( canvas_list[i] ) ) :
      canvas_list[i][j].Write()
    outfile.Close()


  if reference_plane_up is not None and reference_plane_down is not None :
    DATA[ "emitt_trans_in" ] = bunch_list[ reference_plane_up ].get_emittance( [ 'x', 'y' ] )
    DATA[ "emitt_trans_out" ] = bunch_list[ reference_plane_down ].get_emittance( [ 'x', 'y' ] )
    DATA[ "emitt_long_in" ] = bunch_list[ reference_plane_up ].get_emittance( [ 't' ] )
    DATA[ "emitt_long_out" ] = bunch_list[ reference_plane_down ].get_emittance( [ 't' ] )
    DATA[ "emitt_6d_in" ] = bunch_list[ reference_plane_up ].get_emittance( [ 't', 'x', 'y' ] )
    DATA[ "emitt_6d_out" ] = bunch_list[ reference_plane_down ].get_emittance( [ 't', 'x', 'y' ] )

  print "Found :", len( bunch_list ), "Bunches"
  print

  if data_only :
    return DATA

  outfile = ROOT.TFile( os.path.join( directory, "virtual_plane_results.root" ), "UPDATE" )

  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'emittance', ['x', 'y'], 'mm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'emittance', ['t'], 'mm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'emittance', ['x','y','t'], 'mm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'beta', ['x', 'y'], 'mm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'beta', ['t'], 'mm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'alpha', ['x', 'y'], 'mm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'alpha', ['t'], 'mm', 'mm' )
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'mean', ['energy'], 'mm', 'MeV')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'mean', ['bz'], 'mm', 'T')
  graph.Write()
  canvas.Close()
#  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'standard_deviation', ['r'], 'mm', 'mm')
#  graph.Write()
#  canvas.Close()
  graph1, graph2, graph3 = make_3sigma_graph( bunch_list )
  graph1.Write()
  graph2.Write()
  graph3.Write()

  outfile.Close()


  if print_phasespace :

    outfile = ROOT.TFile( os.path.join( "virtual_plane_phasespace.root" ), "RECREATE" )

    for i in range( len( bunch_list ) / 5 ) :
      plane_number = 5*i
      pos = bunch_list[plane_number][0].get( 'z' )

      canvas, hist = bunch_list[plane_number].root_histogram( 'x', 'mm', 'px', 'MeV/c' )
      hist.SetName( 'X-Px:' + str( pos ) )
      hist.Write()
      canvas.Close()
      canvas, hist = bunch_list[plane_number].root_histogram( 'y', 'mm', 'py', 'MeV/c' )
      hist.SetName( 'Y-Py:' + str( pos ) )
      hist.Write()
      canvas.Close()
      canvas, hist = bunch_list[plane_number].root_histogram( 't', 'ns', 'energy', 'MeV' )
      hist.SetName( 'T-Energy:' + str( pos ) )
      hist.Write()
      canvas.Close()

    outfile.Close()

  if len( reference_positions ) > 0 :
    print "Saving Reference Plane Data"
    filename = os.path.join( directory, 'reference_plane_data.txt' )
    propertyList = [ 't', 'x', 'y', 'z', 'px', 'py', 'pz', 'pt', 'energy' ]

    with open( filename, 'w' ) as outfile :
      outfile.write( '#  T sigmaT  X  sigmaX  Y  sigmaY  Z  sigmaZ  Px  sigmaPx  Py  sigmaPy  Pz  sigmaPz  Pt  sigmaPt  Energy  sigmaE  EmittanceTrans  EmittanceLong  Emittance6D  BetaTrans  BetaLong  AlphaTrans  AlphaLong\n' )
      for pos in reference_positions :
        plane = find_virtual_plane( bunch_list, pos )
        if plane is None :
          print "ERROR : Could not locate a virtual plane near z =", pos
          continue
        bunch = bunch_list[plane]
        means = bunch.mean( propertyList )
        sigmas = {}
        for prop in propertyList :
          sigmas[ prop ] = math.sqrt( abs( bunch.moment( [prop, prop] ) ) )
        
        for prop in propertyList :
          outfile.write( str( means[ prop ] ) + ' ' + str( sigmas[ prop ] ) + ' ' )

        outfile.write( str( bunch.get_emittance( ['x','y'] ) ) + ' ' )
        outfile.write( str( bunch.get_emittance( ['ct'] ) ) + ' ' )
        outfile.write( str( bunch.get_emittance( ['ct', 'x', 'y'] ) ) + ' ' )
        outfile.write( str( bunch.get_beta( ['x','y'] ) ) + ' ' )
        outfile.write( str( bunch.get_beta( ['ct'] ) ) + ' ' )
        outfile.write( str( bunch.get_alpha( ['x','y'] ) ) + ' ' )
        outfile.write( str( bunch.get_alpha( ['ct'] ) ) + '\n' )


  if len( inspection_positions ) > 0 :
    print "Saving Inspection Position(s)"
    for pos in inspection_positions :
      plane = find_virtual_plane( bunch_list, pos )
      if plane is None :
        print "ERROR : Could not locate a virtual plane near z =", pos
        continue
      filename = os.path.join( directory, 'inspection_position_'+str( pos )+'.txt' )
      print_bunch( filename, bunch_list[plane] )


  if print_all_planes is True :
    print "Saving Plane Data"
    with open( os.path.join( directory, 'plane_data.txt' ), 'w' ) as outfile :
      outfile.write( '# Mean X    Mean Y   Mean Z    Mean Px   Mean Py   Mean Pz   Mean Pt   Mean Bz   Energy   Emittance   Beta   Alpha  EmittanceLong BetaLong AlphaLong Emittance6D  Weight \n' )
      bunch_count = 0
      for bun in bunch_list :
        means = bun.mean( ['x','y','z','px','py','pz','pt','energy','bz'] )
        outfile.write( str( bunch_count ) + ' ' +
                       str( means[ 'x' ] ) + ' ' +
                       str( means[ 'y' ] ) + ' ' +
                       str( means[ 'z' ] ) + ' ' +
                       str( means[ 'px' ] ) + ' ' +
                       str( means[ 'py' ] ) + ' ' +
                       str( means[ 'pz' ] ) + ' ' +
                       str( means[ 'pt' ] ) + ' ' +
                       str( means[ 'bz' ] ) + ' ' +
                       str( means[ 'energy' ] ) + ' ' +
                       str( bun.get_emittance( [ 'x','y' ] ) ) + ' ' +
                       str( bun.get_beta( ['x','y'] ) ) + ' ' +
                       str( bun.get_alpha( ['x','y'] ) ) + ' ' +
                       str( bun.get_emittance( ['t'] ) ) + ' ' +
                       str( bun.get_beta( ['t'] ) ) + ' ' +
                       str( bun.get_alpha( ['t'] ) ) + ' ' +
                       str( bun.get_emittance( [ 'x', 'y', 't' ] ) ) + ' ' +
                       str( bun.bunch_weight() ) + "\n" )
        bunch_count += 1

  return DATA


####################################################################################################

def analyse_details( data, directory ) :
  emittance_trans_in = []
  emittance_trans_out = []
  emittance_trans_change = []
  emittance_trans_frac = []
  emittance_trans_perc = []
  emittance_long_in = []
  emittance_long_out = []
  emittance_long_change = []
  emittance_long_frac = []
  emittance_long_perc = []
  emittance_6d_in = []
  emittance_6d_out = []
  emittance_6d_change = []
  emittance_6d_frac = []
  emittance_6d_perc = []
  number_in = []
  number_out = []
  number_frac = []
  number_perc = []

  for datum in data :
    emittance_trans_in.append( datum[ "emitt_trans_in" ] )
    emittance_trans_out.append( datum[ "emitt_trans_out" ] )
    emittance_trans_change.append( datum[ "emitt_trans_out"] - datum[ "emitt_trans_in" ] )
    emittance_trans_frac.append( ( datum[ "emitt_trans_out"] - datum[ "emitt_trans_in" ] ) / datum[ "emitt_trans_in" ] )
    emittance_trans_perc.append( 100.0 * ( ( datum[ "emitt_trans_out"] - datum[ "emitt_trans_in" ] ) / datum[ "emitt_trans_in" ] ) )
    emittance_long_in.append( datum[ "emitt_long_in" ] )
    emittance_long_out.append( datum[ "emitt_long_out" ] )
    emittance_long_change.append( datum[ "emitt_long_out"] - datum[ "emitt_long_in" ] )
    emittance_long_frac.append( ( datum[ "emitt_long_out"] - datum[ "emitt_long_in" ] ) / datum[ "emitt_long_in" ] )
    emittance_long_perc.append( 100.0 * ( ( datum[ "emitt_long_out"] - datum[ "emitt_long_in" ] ) / datum[ "emitt_long_in" ] ) )
    emittance_6d_in.append( datum[ "emitt_6d_in" ] )
    emittance_6d_out.append( datum[ "emitt_6d_out" ] )
    emittance_6d_change.append( datum[ "emitt_6d_out"] - datum[ "emitt_6d_in" ] )
    emittance_6d_frac.append( ( datum[ "emitt_6d_out"] - datum[ "emitt_6d_in" ] ) / datum[ "emitt_6d_in" ] )
    emittance_6d_perc.append( 100.0 * ( ( datum[ "emitt_6d_out"] - datum[ "emitt_6d_in" ] ) / datum[ "emitt_6d_in" ] ) )
    number_in.append( datum[ "number_in" ] )
    number_out.append( datum[ "number_out" ] )
    number_frac.append( datum[ "number_out" ] / datum[ "number_in" ] )
    number_perc.append( 100.0 * ( datum[ "number_out" ] / datum[ "number_in" ] ) )

  outfile = ROOT.TFile( os.path.join( directory, "virtual_analysis_comparisons.root" ), "RECREATE" )

  hist, graph = xboa.Common.make_root_graph( "trans_emittance_change", emittance_trans_in, "Input Emittance [mm]", emittance_trans_change, "Emittance Change", True, xmin=1.0, xmax=10.0 )
  graph.Write()
  hist, graph = xboa.Common.make_root_graph( "fractional_trans_emittance_change", emittance_trans_in, "Input Emittance [mm]", emittance_trans_frac, "Fractional Emittance Change", True )
  graph.Write()
  hist, graph = xboa.Common.make_root_graph( "percentage_trans_emittance_change", emittance_trans_in, "Input Emittance [mm]", emittance_trans_perc, "Percentage Emittance Change", True )
  graph.Write()
  hist, graph = xboa.Common.make_root_graph( "long_emittance_change", emittance_long_in, "Input Emittance [mm]", emittance_long_change, "Emittance Change", True )
  graph.Write()
  hist, graph = xboa.Common.make_root_graph( "fractional_long_emittance_change", emittance_long_in, "Input Emittance [mm]", emittance_long_frac, "Fractional Emittance Change", True )
  graph.Write()
  hist, graph = xboa.Common.make_root_graph( "percentage_long_emittance_change", emittance_long_in, "Input Emittance [mm]", emittance_long_perc, "Percentage Emittance Change", True )
  graph.Write()
  hist, graph = xboa.Common.make_root_graph( "6d_emittance_change", emittance_6d_in, "Input Emittance [mm]", emittance_6d_change, "Emittance Change", True )
  graph.Write()
  hist, graph = xboa.Common.make_root_graph( "fractional_6d_emittance_change", emittance_6d_in, "Input Emittance [mm]", emittance_6d_frac, "Fractional Emittance Change", True )
  graph.Write()
  hist, graph = xboa.Common.make_root_graph( "percentage_6d_emittance_change", emittance_6d_in, "Input Emittance [mm]", emittance_6d_perc, "Percentage Emittance Change", True )
  graph.Write()

  hist, graph = xboa.Common.make_root_graph( "fractional_transmission", emittance_trans_in, "Input Emittance [mm]", number_frac, "Fractional Transmission", True )
  graph.Write()
  hist, graph = xboa.Common.make_root_graph( "percentage_transmission", emittance_trans_in, "Input Emittance [mm]", number_perc, "Percentage Transmission", True )
  graph.Write()

  outfile.Close()

####################################################################################################


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

def print_bunch( filename, bunch ) :
  with open( filename, 'w' ) as outfile :
    outfile.write( '#  T   X   Y   Z   r   Px   Py   Pz   Pt   P   E\n' )
    for hit in bunch :
      if hit.get_weight() < 0.001 :
        continue
      outfile.write( str( hit.get( 't' ) ) + ' ' +
                     str( hit.get( 'x' ) ) + ' ' + 
                     str( hit.get( 'y' ) ) + ' ' +
                     str( hit.get( 'z' ) ) + ' ' +
                     str( hit.get( 'r' ) ) + ' ' +
                     str( hit.get( 'px' ) ) + ' ' +
                     str( hit.get( 'py' ) ) + ' ' +
                     str( hit.get( 'pz' ) ) + ' ' +
                     str( hit.get( 'pt' ) ) + ' ' +
                     str( hit.get( 'p' ) ) + ' ' +
                     str( hit.get( 'energy' ) ) + ' ' +
                     str( hit.get( 'pid' ) ) + '\n' )


def print_distributions( bunch, colour, canvas_list=None ) :
  number = 5
  if canvas_list is None :
    canvas_list = [ None for i in range( number ) ]
  graphs = [ None for i in range( number ) ]

  canvas_list[0], hist, graphs[0] = bunch.root_scatter_graph( 'x', 'px', 'mm', 'MeV/c', include_weightless=False, canvas=canvas_list[0] )
  canvas_list[1], hist, graphs[1] = bunch.root_scatter_graph( 'y', 'py', 'mm', 'MeV/c', include_weightless=False, canvas=canvas_list[1] )
  canvas_list[2], hist, graphs[2] = bunch.root_scatter_graph( 't', 'energy', 'ns', 'MeV', include_weightless=False, canvas=canvas_list[2] )
  canvas_list[3], hist, graphs[3] = bunch.root_scatter_graph( 'r', 'pt', 'mm', 'MeV/c', include_weightless=False, canvas=canvas_list[3] )
  canvas_list[4], hist, graphs[4] = bunch.root_scatter_graph( 'x', 'y', 'mm', 'mm', include_weightless=False, canvas=canvas_list[4] )

  for i in range( number ) :
    canvas_list[i].cd()
    graphs[i].SetMarkerStyle( 6 )
    graphs[i].SetMarkerColor( colour )
    graphs[i].Draw("p")
    canvas_list[i].Update()
    
  return canvas_list

def make_3sigma_graph( bunches ) :
  positions = [ bunch[0].get( 'z' ) for bunch in bunches ]
  sigmas1 = [ bunch.standard_deviation( 'r' ) for bunch in bunches ]
  sigmas2 = [ 2.0*s for s in sigmas1 ]
  sigmas3 = [ 3.0*s for s in sigmas1 ]

  hist, sigma1_graph = xboa.Common.make_root_graph( '1sigma_beam_profile', positions, "Z Position [mm]", sigmas1, "Beam Profile [mm]", True )
  hist, sigma2_graph = xboa.Common.make_root_graph( '2sigma_beam_profile', positions, "Z Position [mm]", sigmas2, "Beam Profile [mm]", True )
  hist, sigma3_graph = xboa.Common.make_root_graph( '3sigma_beam_profile', positions, "Z Position [mm]", sigmas3, "Beam Profile [mm]", True )

  return sigma1_graph, sigma2_graph, sigma3_graph


if __name__ == "__main__":
  if MAUS_root is None :
    print "Could not find MAUS_ROOT_DIR"
    print "Did you source the env.sh script?"
    print
    sys.exit( 0 )

  ROOT.gROOT.SetBatch( True )

  if len( sys.argv ) <= 1 :
    print "No Files Supplied."
    sys.exit( 0 )
  
  filelist = sys.argv[1:]
  details = []


  print "Found", len( filelist ), "File(s)"
  print "Analysing..."

  try :

    for filename in filelist :
      print
      print '#################################################'
      print
      print "Analysing file,", filename
      print
      details.append( analyse_file( filename, os.path.dirname( filename ) ) )
      xboa.Common.clear_root()
      gc.collect()
    print
    print '#################################################'

  except BaseException as ex :
    print "Caught and exception."
    if len( filelist ) > 1 :
      print "Saving Details and then I'm going to die..."
      with open( os.path.join( output_directory, "virtual_analysis_comparison_data.json" ), 'w' ) as outfile :
        outfile.write( json.dumps( details ) )
      print traceback.format_exc()
    else :
      print "I'm going to die now..."
      print traceback.format_exc()
    raise ex

  if len( filelist ) > 1 :
    
    with open( os.path.join( output_directory, "virtual_analysis_comparison_data.json" ), 'w' ) as outfile :
      outfile.write( json.dumps( details ) )

    print
    print "Multiple files found."
    print "Running comparison analysis"
    print
    analyse_details( details, output_directory )

  print
  print "Complete!"
  print


