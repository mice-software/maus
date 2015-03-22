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

import operator
import sys
import array
import gc

import emittance_analysis.extraction as extraction
import emittance_analysis.covariances as covariances
import emittance_analysis.stats as stats
from emittance_analysis import muon_mass



radial_cut = 200.0
plane_number_cut = 125
number_planes = 150

inspection_plane = []
#inspection_plane = -1 # Negative means don't write a file

print_all_planes = True
apply_cuts = True


def main():
  if len( sys.argv ) <= 1 :
    print "No File Supplied."
    sys.exit( 0 )


  file_reader = extraction.maus_reader( sys.argv[1:] )

  bunch_list_tmp = []

  print "Loading", file_reader.get_number_files(), "Files"
  print
  print "Please Wait..."
  print


  covariance_matrices = [ covariances.covariance_matrix() for i in range( number_planes ) ]
  positions = [ stats.stats() for i in range( number_planes ) ]
  energies = [ stats.stats() for i in range( number_planes ) ]
  momenta = [ stats.stats() for i in range( number_planes ) ]


  
  counter = 0
  while file_reader.next_event() :
#    sys.stdout.write( '\r+Spill ' + str( file_reader.get_current_spill_number() ) + ', File: ' + str( file_reader.get_current_filenumber() ) )
    mc_event = file_reader.get_event( 'mc' )
    num_virtual_hits = mc_event.GetVirtualHitsSize()

    is_cut = False
    if apply_cuts :
      muon_counter = 0
      for i in range( num_virtual_hits ) :
        virt = mc_event.GetAVirtualHit( i )

        if abs( virt.GetParticleId() ) == 13 :
          muon_counter += 1
        if math.sqrt( virt.GetPosition()[0]**2 + virt.GetPosition()[1]**2 ) > radial_cut :
          is_cut = True
          break
#              ALL CUTS GO HERE!

      if plane_number_cut != 0 and muon_counter < plane_number_cut :
        is_cut = True

    if is_cut :
      continue

    for i in range( num_virtual_hits ) :
      virt = mc_event.GetAVirtualHit( i )
      if abs( virt.GetParticleId() ) != 13 :
        continue
      plane = virt.GetStationId()

      mom = virt.GetMomentum().mag()

      covariance_matrices[ plane ].add_hit( make_xboa_hit( virt, counter ) )
      positions[ plane ].record( virt.GetPosition()[2] )
      energies[ plane ].record( math.sqrt( mom**2 + muon_mass**2 ) )
      momenta[ plane ].record( mom )

    gc.collect()
    counter += 1

  print

  emittance_values = array.array( 'd' )
  beta_values = array.array( 'd' )
  position_values = array.array( 'd' )
  weight_values = array.array( 'd' )
  momentum_values = array.array( 'd' )
  energy_values = array.array( 'd' )

  for i in range( number_planes ) :
    if covariance_matrices[i].length() < 2 : continue
    emittance_values.append( covariance_matrices[i].get_emittance() )
    position_values.append( positions[i].get_mean() )
    weight_values.append( covariance_matrices[i].length() )
    momentum_values.append( momenta[i].get_mean() )
    energy_values.append( energies[i].get_mean() )
    beta_values.append( covariance_matrices[i].get_beta( momentum=momenta[i].get_mean() ) )

#  print position_values
#  print emittance_values

  canvas = ROOT.TCanvas( "emittancecanvas" )
  graph = ROOT.TGraph( len( position_values ), position_values, emittance_values )
  graph.SetLineWidth( 2 )
  graph.GetXaxis().SetTitle( "Position [m]" )
  graph.GetYaxis().SetTitle( "Emittance [mm]" )

  weight_graph = ROOT.TGraph( len( position_values ), position_values, weight_values )
  energy_graph = ROOT.TGraph( len( position_values ), position_values, energy_values )
  momentum_graph = ROOT.TGraph( len( position_values ), position_values, momentum_values )
  beta_graph = ROOT.TGraph( len( position_values ), position_values, beta_values )


  graph.Draw( "AL" )
  canvas.SaveAs( "test.png" )


  outfile = ROOT.TFile( "test.root", "RECREATE" )
  graph.Write( "emittance_graph" )
  weight_graph.Write( "weight_graph" )
  energy_graph.Write( "energy_graph" )
  momentum_graph.Write( "momentum_graph" )
  beta_graph.Write( "beta_graph" )
  outfile.Close()

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




if __name__ == "__main__":
    main()


