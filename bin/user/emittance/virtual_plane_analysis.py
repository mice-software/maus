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


radial_cut = 200.5
pid_cut = -13
upstream_transverse_amplitude_cut = 72.0
downstream_transverse_amplitude_cut = 72.0

inspection_positions = [ -1831.7313, 0.0, 1831.6589 ] # Step 4 Recent
#inspection_positions = [ -3440.3, -1900.0, 0.0, 1900.0, 3440.3 ] # Reference
#inspection_positions = [ -3578.0, -1647.0, 0.0, 1647.0, 3578.0 ] # Alternative
reference_positions = [ -1831.7313, 1831.6589 ] # Step 4 Recent
#reference_positions = [ -3440.3, 3440.3 ] # Reference
#reference_positions = [ -3578.0, 3578.0 ] # Alternative
print_phasespace = False
print_all_planes = True
cut_amplitudes = False
apply_cuts = True

MAUS_root = os.environ.get( 'MAUS_ROOT_DIR' )

def main():
  if len( sys.argv ) <= 1 :
    print "No File Supplied."
    sys.exit( 0 )


  file_reader = extraction.maus_reader( sys.argv[1:] )

  bunch_list_tmp = []

  print "Loading", file_reader.get_number_files(), "Files"
  print

  counter = 0
  while file_reader.next_event() :
    mc_event = file_reader.get_event( 'mc' )
    num_virtual_hits = mc_event.GetVirtualHitsSize()

    is_cut = False
    if apply_cuts :
      muon_counter = 0
      for i in range( num_virtual_hits ) :
        virt = mc_event.GetAVirtualHit( i )

        if abs( virt.GetParticleId() ) == 13 :
          muon_counter += 1
#        if math.sqrt( virt.GetPosition()[0]**2 + virt.GetPosition()[1]**2 ) > radial_cut :
#          is_cut = True
#          break
#              ALL CUTS GO HERE!

#      if plane_number_cut != 0 and muon_counter < plane_number_cut :
#        is_cut = True

    if is_cut :
      continue

    for i in range( num_virtual_hits ) :
      virt = mc_event.GetAVirtualHit( i )
      if abs( virt.GetParticleId() ) != 13 :
        continue
      plane = virt.GetStationId()

      xboa_hit = make_xboa_hit( virt, counter )
      if len( bunch_list_tmp ) <= plane :
        for i in range( len( bunch_list_tmp ), plane ) :
          bunch_list_tmp.append( xboa.Bunch.Bunch() )

      bunch_list_tmp[ plane - 1 ].append( xboa_hit )

    gc.collect()
    counter += 1

  end_bunch = bunch_list_tmp[-1]
  for bunch in bunch_list_tmp :
    bunch.transmission_cut( end_bunch )

  end_bunch.cut( {'r' : radial_cut}, operator.ge, global_cut = True )

  bunch_list = [ bun for bun in bunch_list_tmp if len( bun ) > 1 and bun.bunch_weight() > 1.0e-9 ]

  del bunch_list_tmp
  gc.collect()


  print "Found :", len( bunch_list ), "Bunches"


  outfile = ROOT.TFile( "virtual_plane_results.root", "RECREATE" )

  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'bunch_weight', [''], 'm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'emittance', ['x', 'y'], 'm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'emittance', ['t'], 'm', 'ns')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'emittance', ['x','y','t'], 'm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'beta', ['x', 'y'], 'm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'beta', ['t'], 'm', 'ns')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'alpha', ['x', 'y'], 'm', 'mm')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'alpha', ['t'], 'm', 'ns' )
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'mean', ['energy'], 'm', 'MeV')
  graph.Write()
  canvas.Close()
  canvas, hist, graph = Bunch.root_graph(bunch_list, 'mean', ['z'], 'mean', ['bz'], 'm', 'T')
  graph.Write()
  canvas.Close()

  outfile.Close()


  if print_phasespace :

    outfile = ROOT.TFile( "virtual_plane_phasespace.root", "RECREATE" )

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


  if len( inspection_positions ) >= 0 :
    print
    print "Saving Inspection Position(s)"
    print "Please Wait."
    print

    for pos in inspection_positions :
      plane = find_virtual_plane( bunch_list, pos )
      if plane is None :
        print "ERROR : Could not locate a virtual plane near z =", pos
        print
        continue
      filename = 'inspection_position_'+str( pos )+'.txt'
      with open( filename, 'w' ) as outfile :
        outfile.write( '#  T   X   Y   Z   r   Px   Py   Pz   Pt   P   E\n' )
        for hit in bunch_list[ plane ] :
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
                         str( hit.get( 'pid' ) ) + ' ' +
                         str( hit.get_weight() ) + '\n' )

      print filename, "Created"


  if len( inspection_plane ) >= 0 :
    print
    print "Saving Inspection Plane(s)"
    print "Please Wait."
    print
    for plane in inspection_plane :
      filename = 'inspection_plane_'+str( plane )+'.txt'
      with open( filename, 'w' ) as outfile :
        outfile.write( '#  T   X   Y   Z   r   Px   Py   Pz   Pt   P   E\n' )
        for hit in bunch_list[ plane ] :
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
                         str( hit.get( 'pid' ) ) + ' ' +
                         str( hit.get_weight() ) + '\n' )

      print filename, "Created"


  if print_all_planes is True :
    print
    print "Saving Plane Data"
    print "Please Wait."
    print
    with open( 'plane_data.txt', 'w' ) as outfile :
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
    print 'plane_data.txt Created'




def make_xboa_hit( hit, num ) :

  hit = xboa.Hit.Hit.new_from_dict(
      { 'x' : hit.GetPosition()[0],
        'y' : hit.GetPosition()[1],
        'z' : hit.GetPosition()[2],
        'px' : hit.GetMomentum()[0],
        'py' : hit.GetMomentum()[1],
        'pz' : hit.GetMomentum()[2],
        'pid' : hit.GetParticleId(),
        'mass' : hit.GetMass(),
        'bx' : hit.GetBField()[0],
        'by' : hit.GetBField()[1],
        'bz' : hit.GetBField()[2],
        't' : hit.GetTime(),
        'spill' : num,
        'particle_number' : num,
        'event_number' : num }, 'energy' )
  return hit


def find_virtual_plane( bunches, position, max_sep=10.0 ) :

  current_plane = -1
  current_sep = max_sep

  for i in range( len( bunches ) ) :
    bunch = bunches[i]
    sep = abs( bunch[0].get( 'z' ) - position )
    if  sep < current_sep :
      current_sep = sep
      current_plane = i

  print "Position :", position, "-> Plane :", current_plane
  if current_plane < 0 :
    return None
  else :
    return current_plane




if __name__ == "__main__":
  ROOT.gROOT.SetBatch( True )
  main()


