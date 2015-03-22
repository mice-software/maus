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



associate_tolerance = 1.0
associate_max_attempts = 100
recon_station = 1
recon_plane = 0


def run() :

  maus_root_files = sys.argv[1:]

  residual_x = ROOT.TH1F( "residual_x", "Residuals in X", 1000, -10.0, 10.0 )
  residual_y = ROOT.TH1F( "residual_y", "Residuals in Y", 1000, -10.0, 10.0 )
  residual_z = ROOT.TH1F( "residual_z", "Residuals in Z", 1000, -10.0, 10.0 )
  residual_px = ROOT.TH1F( "residual_px", "Residuals in PX", 1000, -10.0, 10.0 )
  residual_py = ROOT.TH1F( "residual_py", "Residuals in PY", 1000, -10.0, 10.0 )
  residual_pz = ROOT.TH1F( "residual_pz", "Residuals in PZ", 1000, -10.0, 10.0 )

  residual_phi = ROOT.TH1F( "residual_phi", "Residuals in Phi", 500, -6.3, 6.3 )

  virtual_plane_dict = []

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
    sys.exit( 0 )

  upstream_virtual = virtual_plane_dict[0][recon_station - 1][recon_plane][0]
  downstream_virtual = virtual_plane_dict[1][recon_station - 1][recon_plane][0]

  print
  print "Found reference planes. Numbers:", upstream_virtual, "and", downstream_virtual
  print

  gc.collect()


  file_reader = extraction.maus_reader( maus_root_files )

  number_files = file_reader.get_number_files()

  print "Loading", number_files, "File(s)"
  print


  virt_counter = 0
  hit_counter = 0


  while file_reader.next_event() :
    try :

# Print current position - it looks nice!
#      sys.stdout.write( ' Spill ' + str( file_reader.get_current_spill_number() ) + ' of ' + str( file_reader.get_current_number_spills() ) + ' in File ' + str( file_reader.get_current_filenumber() ) + ' of ' + str( file_reader.get_number_files() ) + '   \r' )
      sys.stdout.flush()


      mc_event = file_reader.get_event( 'mc' )

      raw_virtuals = mc_event.GetVirtualHits()
      raw_scifis = mc_event.GetSciFiHits()


      tracker_virtuals = []
      for hit in raw_virtuals :
        if hit.GetParticleId() != -13 :
          continue
        for t in range( 2 ) :
          for s in range( 5 ) :
            for p in range( 3 ) :
              if hit.GetStationId() == virtual_plane_dict[t][s][p][0] :
                tracker_virtuals.append( ( t, s, p, hit ) )



      for tr, st, pl, virt in tracker_virtuals :
        st = st + 1
        counter = 0
        mean_x = 0.0
        mean_y = 0.0
        mean_z = 0.0
        mean_px = 0.0
        mean_py = 0.0
        mean_pz = 0.0
        track_id = virt.GetTrackId()
        for hit in raw_scifis :
          channelId = hit.GetChannelId()
          if channelId.GetTrackerNumber() == tr and channelId.GetStationNumber() == st and channelId.GetPlaneNumber() == pl and hit.GetTrackId() == track_id :
#            hit_counter += 1
#            residual_x.Fill( hit.GetPosition()[0] - virt.GetPosition()[0] )
#            residual_y.Fill( hit.GetPosition()[1] - virt.GetPosition()[1] )
#            residual_z.Fill( hit.GetPosition()[2] - virt.GetPosition()[2] )
#            residual_px.Fill( hit.GetMomentum()[0] - virt.GetMomentum()[0] )
#            residual_py.Fill( hit.GetMomentum()[1] - virt.GetMomentum()[1] )
#            residual_pz.Fill( hit.GetMomentum()[2] - virt.GetMomentum()[2] )
#            break

            counter += 1
            mean_x += hit.GetPosition()[0]
            mean_y += hit.GetPosition()[1]
            mean_z += hit.GetPosition()[2]
            mean_px += hit.GetMomentum()[0]
            mean_py += hit.GetMomentum()[1]
            mean_pz += hit.GetMomentum()[2]

#            print hit.GetPosition()[0], hit.GetPosition()[1], hit.GetPosition()[2], hit.GetMomentum()[0], hit.GetMomentum()[1], hit.GetMomentum()[2]
#            print virt.GetPosition()[0], virt.GetPosition()[1], virt.GetPosition()[2], virt.GetMomentum()[0], virt.GetMomentum()[1], virt.GetMomentum()[2]
#            print

        if counter > 0 :
          hit_counter += 1
          residual_x.Fill( ( mean_x / counter ) - virt.GetPosition()[0] )
          residual_y.Fill( ( mean_y / counter ) - virt.GetPosition()[1] )
          residual_z.Fill( ( mean_z / counter ) - virt.GetPosition()[2] )
          residual_px.Fill( ( mean_px / counter ) - virt.GetMomentum()[0] )
          residual_py.Fill( ( mean_py / counter ) - virt.GetMomentum()[1] )
          residual_pz.Fill( ( mean_pz / counter ) - virt.GetMomentum()[2] )
    
      virt_counter += len( tracker_virtuals )


    except ValueError as ex :
      print 'Caught an exception. Details :', ex
      print 'Continuing Analysis.'
      gc.collect()
      continue


  print "VIRTS : ", virt_counter
  print "HITS  : ", hit_counter

  outfile = ROOT.TFile( "check_scifi_residuals.root", "RECREATE" )

  residual_x.Write()
  residual_y.Write()
  residual_z.Write()
  residual_px.Write()
  residual_py.Write()
  residual_pz.Write()

  outfile.Close()


if __name__ == "__main__" :
  run()
  print

