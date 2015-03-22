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
import itertools
import gc
import copy

import MAUS
import ROOT
import xboa
import numpy

# definitions of MAUS data structure for PyROOT
import libMausCpp #pylint: disable = W0611

import covariances
import duplicate_print


p_value_cut = 0.05
pt_cut = 150.0
pz_cut = 300.0
pt_low_cut = 0.0
aperture_cut = 189.0
pid_cut = -13
allow_4point_tracks = False

automatic_virtual_association = True
max_association_attempts = 100
upstream_virtual = 17
downstream_virtual = 53
virtual_plane_dict = []

re_center_values = False
skip_on_virtuals = False

recon_plane_num = 0
recon_station_num = 1


produce_full_analysis = True
produce_errors = False

save_summary = True
print_eps = False
print_everything = False
produce_residuals = True
save_single_particle_amplitudes = False
save_virtual_planes = False
produce_dat_file = False

save_corrections = False
load_upstream_correction = 'measure_emittance_correction_matrix_up.dat'
load_downstream_correction = 'measure_emittance_correction_matrix_down.dat'
#load_upstream_correction = None
#load_downstream_correction = None
upstream_correction = None
downstream_correction = None

virt_location_tolerance = 1.0
errors_bin_size = 2500



# NOT YET SUPPORTED!
use_pattRec = False


number_spills = 0
number_events = 0
number_recon_cut = 0
number_straight_recon_up = 0
number_straight_recon_down = 0
number_virtual_cut = 0
number_virtual_e = 0
number_4point_tracks = 0
skipped_recon = 0
skipped_virtual = 0
skipped_mutual = 0
non_skipped = 0
missing_half_virtual = 0
missing_half_recon = 0

number_OOPS = 0

number_recon_tracks = 0
number_recon_hits = 0
number_virtual_hits = 0

recon_flag = False
virtual_flag = False

hit_trues = 0
recon_trues = 0

missing_counter = 0
mismatched_hit_num = 0

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

upstream_emittance_error = ROOT.TH1F( "emittance_mc_upstream", "Histogram of Upstream Emittance for every " + str( errors_bin_size ) + " Events", 500, 0.0, 20.0 )
downstream_emittance_error = ROOT.TH1F( "emittance_mc_downstream", "Histogram of Downstream Emittance for every " + str( errors_bin_size ) + " Events", 500, 0.0, 20.0 )
fractional_emittance_error = ROOT.TH1F( "emittance_mc_fractional_change", "Histogram of Fractional Emittance Change for every " + str( errors_bin_size ) + " Events", 200, -1.0, 1.0 )
delta_emittance_error = ROOT.TH1F( "delta_mc_emittance_change", "Histogram of the Emittance Change for every " + str( errors_bin_size ) + " Events", 200, -1.0, 1.0 )

upstream_emittance_error_corrected = ROOT.TH1F( "emittance_mc_upstream_corrected", "Histogram of Upstream Corrected Emittance for every " + str( errors_bin_size ) + " Events", 500, 0.0, 20.0 )
downstream_emittance_error_corrected = ROOT.TH1F( "emittance_mc_downstream_corrected", "Histogram of Downstream Corrected Emittance for every " + str( errors_bin_size ) + " Events", 500, 0.0, 20.0 )
fractional_emittance_error_corrected = ROOT.TH1F( "emittance_mc_fractional_change_corrected", "Histogram of Fractional Corrected Emittance Change for every " + str( errors_bin_size ) + " Events", 200, -1.0, 1.0 )
delta_emittance_error_corrected = ROOT.TH1F( "delta_mc_emittance_change_corrected", "Histogram of the Corrected Emittance Change for every " + str( errors_bin_size ) + " Events", 200, -1.0, 1.0 )


muon_mass = 105.6583715





mc_covariance_up = covariances.covariance_matrix()
mc_covariance_down = covariances.covariance_matrix()
recon_covariance_up = covariances.covariance_matrix()
recon_covariance_down = covariances.covariance_matrix()

correction_up = covariances.correction_matrix()
correction_down = covariances.correction_matrix()



virtual_bunches = []






def run() :
  if len( sys.argv ) <= 1 :
    print "No File Supplied."
    sys.exit( 0 )

  global number_spills
  global number_events
  global number_recon_cut
  global number_virtual_cut
  global number_virtual_e
  global skipped_recon
  global skipped_virtual
  global skipped_mutual
  global non_skipped
  global missing_half_virtual
  global missing_half_recon
  global number_straight_recon_up
  global number_straight_recon_down
  global number_4point_tracks

  global number_OOPS

  global hit_trues
  global recon_trues
  global number_recon_tracks
  global number_recon_hits
  global number_virtual_hits
  global missing_counter
  global mismatched_hit_num

  global recon_flag
  global virtual_flag

  global automatic_virtual_association
  global max_association_attempts
  global upstream_virtual
  global downstream_virtual
  global virt_location_tolerance
  global virtual_plane_dict

  global produce_full_analysis

  global re_center_values
  global save_virtual_planes
  global produce_errors
  global produce_dat_file
  global produce_residuals
  global print_eps
  global save_summary
  global save_corrections

  global errors_bin_size
  global load_upstream_correction
  global load_downstream_correction
  global upstream_correction
  global downstream_correction

  logging = None
  if save_summary :
    logging = duplicate_print.Tee( 'measure_emittance_logging.txt' )
  
  if automatic_virtual_association is True :
    print "Locating Virtual Planes"
    print "Please Wait..."

    filename = sys.argv[1]

    infile = ROOT.TFile( filename, "READ" )

    data = ROOT.MAUS.Data()
    tree = infile.Get("Spill")
    tree.SetBranchAddress("data", data)

    virtual_plane_dict = []

    for i in range( 2 ) :
      stations = []
      for j in range( 5 ) :
        planes = []
        for k in range( 3 ) :
          planes.append( ( -1, virt_location_tolerance ) )
        stations.append( planes )
      virtual_plane_dict.append( stations )

    spill = None
    attempt_count = 0
    for i in range( tree.GetEntries() ) :
      tree.GetEntry( i )
      spill = data.GetSpill()
      if spill.GetDaqEventType() != "physics_event" :
        continue

      if spill.GetReconEvents().size() != spill.GetMCEvents().size() :
        print
        print "ERROR Non-Identicle Numbers of Events between MC and Recon!"
        print
        sys.exit( 0 )

      event_count = 0
      number_events = spill.GetReconEvents().size()

      while attempt_count < max_association_attempts and event_count < number_events :
        if associate_virtuals( spill.GetReconEvents()[event_count].GetSciFiEvent().scifitracks(), \
             spill.GetMCEvents()[event_count].GetVirtualHits() ) is True :
          break
        attempt_count += 1
        event_count += 1
      else :
        if attempt_count == max_association_attempts :
          print 
          print "ERROR Could not successfully associate all virtual planes after", max_association_attempts, "attempts"
          print
#          print virtual_plane_dict
#          print
          sys.exit( 0 )
        else :
          continue
      break

    upstream_virtual = virtual_plane_dict[0][recon_station_num - 1][recon_plane_num][0]
    downstream_virtual = virtual_plane_dict[1][recon_station_num - 1][recon_plane_num][0]

    print
    print "Found reference planes. Numbers:", upstream_virtual, "and", downstream_virtual
    print

    gc.collect()


  if load_upstream_correction != None and len( load_upstream_correction ) > 0 :
    upstream_correction = numpy.loadtxt( load_upstream_correction )
  if load_downstream_correction != None and len( load_downstream_correction ) > 0 :
    downstream_correction = numpy.loadtxt( load_downstream_correction )

  print "Loading", len( sys.argv ) - 1, "File(s)"
  print

  kalman1_bunch = xboa.Bunch.Bunch()
  kalman2_bunch = xboa.Bunch.Bunch()
  virtual1_bunch = xboa.Bunch.Bunch()
  virtual2_bunch = xboa.Bunch.Bunch()

  current_upstream = xboa.Bunch.Bunch()
  current_downstream = xboa.Bunch.Bunch()

  current_num = 0
  spill_counter = 0

  virtual_bunches = []


  for fileNum in range( 1, len( sys.argv ) ) :

    sys.stdout.write( '  File ' + str( fileNum ) + ' of ' + str( len( sys.argv ) - 1 ) + '   \r' )
    sys.stdout.flush()
   
    filename = sys.argv[fileNum]

    infile = ROOT.TFile( filename, "READ" )

    data = ROOT.MAUS.Data()
    tree = infile.Get("Spill")
    tree.SetBranchAddress("data", data)


    for i in range( tree.GetEntries() ) :
      tree.GetEntry( i )
      spill = data.GetSpill()
      spill_counter += 1 

#    print number_spills, spill.GetSpillNumber(), spill.GetDaqEventType()
      if spill.GetDaqEventType() != "physics_event" :
#      print "Skipping Spill"
        continue

      if spill.GetReconEvents().size() != spill.GetMCEvents().size() :
        print
        print "ERROR Non-Identicle Numbers of Events between MC and Recon!"
        print
        sys.exit( 0 )

      number_spills += 1
      number_events += spill.GetReconEvents().size()

      for j in range( spill.GetReconEvents().size() ) :
        recon_flag = False
        virtual_flag = False

        if spill.GetReconEvents()[j].GetSciFiEvent().scifitracks().size() < 2 :
          recon_flag = True
        if spill.GetMCEvents()[j].GetVirtualHits().size() < 2 :
          virtual_flag = True

##  RECON TRACKS  ##
        reconTrackArray = None
        if use_pattRec == True :
          reconTrackArray = spill.GetReconEvents()[j].GetSciFiEvent().helicalprtracks()
        else :
          reconTrackArray = spill.GetReconEvents()[j].GetSciFiEvent().scifitracks()

        number_recon_tracks += reconTrackArray.size()
      
        tracker1_count = 0
        tracker2_count = 0
        for k in range( reconTrackArray.size() ) :
          track = reconTrackArray[k]
          number_recon_hits += track.scifitrackpoints().size()

          if check_recon( track ) is False :
            recon_flag = True

          if track.tracker() == 0 :
            tracker1_count += 1
          elif track.tracker() == 1 :
            tracker2_count += 1

        if tracker1_count != 1 or tracker2_count != 1 :
          recon_flag = True
          missing_half_recon += 1

##  VIRTUAL PLANE TRACKS  ##
        if skip_on_virtuals is True :
          number_virtual_hits += spill.GetMCEvents()[j].GetVirtualHits().size()
          virtual1_count = 0
          virtual2_count = 0
          for k in range( spill.GetMCEvents()[j].GetVirtualHits().size() ) :
            hit = spill.GetMCEvents()[j].GetVirtualHits()[k]

            if hit.GetStationId() == upstream_virtual :
              virtual1_count += 1
              if check_hit( hit ) is False :
                virtual_flag = True
            elif hit.GetStationId() == downstream_virtual :
              virtual2_count += 1
              if check_hit( hit ) is False :
                virtual_flag = True

          if virtual1_count != 1 or virtual2_count != 1 :
            virtual_flag = True
            missing_half_virtual += 1
        else :
          virtual_flag = False

##  THE DECISION  ##

        if virtual_flag == recon_flag :
          if virtual_flag is True :
            skipped_mutual += 1
            continue
        else :
          if virtual_flag is True:
            skipped_virtual += 1
          if recon_flag is True:
            skipped_recon += 1
          continue

        non_skipped += 1


        reconHits_up = []
        reconHits_down = []
        MCHits_up = []
        MCHits_down = []
##  Save the Hits  ##
        if produce_full_analysis :
          for k in range( reconTrackArray.size() ) :
            scifiTrack = reconTrackArray[k]

            reconHit = None
            if scifiTrack.tracker() == 0 :
              reconHit = getHits( scifiTrack )
              if reconHit is not None :
                kalman1_bunch.append( reconHit )
                reconHits_up.append( reconHit )
            elif scifiTrack.tracker() == 1 :
              reconHit  = getHits( scifiTrack )
              if reconHit is not None :
                kalman2_bunch.append( reconHit )
                reconHits_down.append( reconHit )

##  Save the errors hist  ##
        if produce_errors :
          current_num += 1
          for k in range( reconTrackArray.size() ) :
            scifiTrack = reconTrackArray[k]

            reconHit = None
            if scifiTrack.tracker() == 0 :
              reconHit = getHits( scifiTrack )
              if reconHit is not None :
                current_upstream.append( reconHit )
            elif scifiTrack.tracker() == 1 :
              reconHit  = getHits( scifiTrack )
              if reconHit is not None :
                current_downstream.append( reconHit )
          if current_num == errors_bin_size :
            up_em = current_upstream.get_emittance( ['x', 'y'] )
            cov_up = current_upstream.covariance_matrix( ['x', 'px', 'y', 'py'] )
            down_em = current_downstream.get_emittance( ['x', 'y'] )
            cov_down = current_downstream.covariance_matrix( ['x', 'px', 'y', 'py'] )

            upstream_emittance_error.Fill( up_em )
            downstream_emittance_error.Fill( down_em )
            fractional_emittance_error.Fill( ( down_em / up_em ) - 1.0 )
            delta_emittance_error.Fill( ( down_em - up_em ) )


            if upstream_correction != None  and downstream_correction != None :
              correct_up = cov_up + upstream_correction
              correct_down = cov_down + downstream_correction
              up_em = math.sqrt( math.sqrt( numpy.linalg.det( correct_up ) ) ) / muon_mass
              down_em = math.sqrt( math.sqrt( numpy.linalg.det( correct_down ) ) ) / muon_mass
            
              upstream_emittance_error_corrected.Fill( up_em )
              downstream_emittance_error_corrected.Fill( down_em )
              fractional_emittance_error_corrected.Fill( ( down_em / up_em ) - 1.0 )
              delta_emittance_error_corrected.Fill( ( down_em - up_em ) )


            current_upstream = xboa.Bunch.Bunch()
            current_downstream = xboa.Bunch.Bunch()
            current_num = 0
            

         


        for k in range( spill.GetMCEvents()[j].GetVirtualHits().size() ) :
          virtualHit = spill.GetMCEvents()[j].GetVirtualHits()[k]

          MCHit = None
          if virtualHit.GetStationId() == upstream_virtual :
            MCHit = getVirtualHit( virtualHit )
            if MCHit is not None :
              virtual1_bunch.append( MCHit )
              MCHits_up.append( MCHit )
          elif virtualHit.GetStationId() == downstream_virtual :
            MCHit = getVirtualHit( virtualHit )
            if MCHit is not None :
              virtual2_bunch.append( MCHit )
              MCHits_down.append( MCHit )
          
          if save_virtual_planes or produce_dat_file :
            if len( virtual_bunches ) <= virtualHit.GetStationId() - 1 :
              for i in range( len( virtual_bunches ), virtualHit.GetStationId() ) :
                virtual_bunches.append( xboa.Bunch.Bunch() )
            appendVirtualHit( virtual_bunches[ virtualHit.GetStationId() - 1 ], virtualHit )

        if len( reconHits_up ) != len( MCHits_up ) or \
            len( reconHits_down ) != len( MCHits_down ) or \
            len( reconHits_up ) != 1 :
          mismatched_hit_num += 1
        elif produce_residuals :
          residual_x_up.Fill( reconHits_up[0].get('x') - MCHits_up[0].get('x') )
          residual_y_up.Fill( reconHits_up[0].get('y') - MCHits_up[0].get('y') )
          residual_z_up.Fill( reconHits_up[0].get('z') - MCHits_up[0].get('z') )
          residual_px_up.Fill( reconHits_up[0].get('px') - MCHits_up[0].get('px') )
          residual_py_up.Fill( reconHits_up[0].get('py') - MCHits_up[0].get('py') )
          residual_pt_up.Fill( reconHits_up[0].get('pt') - MCHits_up[0].get('pt') )
          residual_pz_up.Fill( reconHits_up[0].get('pz') - MCHits_up[0].get('pz') )
          residual_phi_up.Fill( math.atan2( reconHits_up[0].get('y'), reconHits_up[0].get('x') ) -\
                                math.atan2( MCHits_up[0].get('y'), MCHits_up[0].get('x') ) )
          residual_pt_pz_up.Fill( reconHits_up[0].get('pt') - MCHits_up[0].get('pt'), MCHits_up[0].get('pz') )
          residual_pz_pz_up.Fill( reconHits_up[0].get('pz') - MCHits_up[0].get('pz'), MCHits_up[0].get('pz') )
          correction_up.add_hit( reconHits_up[0], MCHits_up[0] )
          mc_covariance_up.add_hit( MCHits_up[0] )
          recon_covariance_up.add_hit( reconHits_up[0] )

          residual_x_down.Fill( reconHits_down[0].get('x') - MCHits_down[0].get('x') )
          residual_y_down.Fill( reconHits_down[0].get('y') - MCHits_down[0].get('y') )
          residual_z_down.Fill( reconHits_down[0].get('z') - MCHits_down[0].get('z') )
          residual_px_down.Fill( reconHits_down[0].get('px') - MCHits_down[0].get('px') )
          residual_py_down.Fill( reconHits_down[0].get('py') - MCHits_down[0].get('py') )
          residual_pt_down.Fill( reconHits_down[0].get('pt') - MCHits_down[0].get('pt') )
          residual_pz_down.Fill( reconHits_down[0].get('pz') - MCHits_down[0].get('pz') )
          residual_phi_down.Fill( math.atan2( reconHits_down[0].get('y'), reconHits_down[0].get('x') ) -\
                                math.atan2( MCHits_down[0].get('y'), MCHits_down[0].get('x') ) )
          residual_pt_pz_down.Fill( reconHits_down[0].get('pt') - MCHits_down[0].get('pt'), MCHits_down[0].get('pz') )
          residual_pz_pz_down.Fill( reconHits_down[0].get('pz') - MCHits_down[0].get('pz'), MCHits_down[0].get('pz') )
          correction_down.add_hit( reconHits_down[0], MCHits_down[0] )
          mc_covariance_down.add_hit( MCHits_down[0] )
          recon_covariance_down.add_hit( reconHits_down[0] )
        else :
          correction_up.add_hit( reconHits_up[0], MCHits_up[0] )
          correction_down.add_hit( reconHits_down[0], MCHits_down[0] )
          mc_covariance_up.add_hit( MCHits_up[0] )
          recon_covariance_up.add_hit( reconHits_up[0] )
          mc_covariance_down.add_hit( MCHits_down[0] )
          recon_covariance_down.add_hit( reconHits_down[0] )

      del spill
      gc.collect()
    gc.collect()
#    sys.stdout.write( '\r' + ( 100 * ' ' ) )
  gc.collect()

  sys.stdout.write( '\rAll ' + str( len( sys.argv ) - 1 ) + ' File(s) Loaded                            \n' )
  sys.stdout.flush()

  print
  print "Total", number_events, "Events"
  print "Total", number_spills, "Spills"
  print
  print "Total", number_recon_tracks, "Recon TRACKS"
  print "Total", number_recon_hits, "Recon HITS"
  print "Total", number_virtual_hits, "Virtual HITS"
  print
  print "Found ", len( kalman1_bunch ) + len( kalman2_bunch ), " Kalman Points"
  print "Found ", len( virtual1_bunch ) + len( virtual2_bunch ), " Virtual Hits"
  print
  print "Didn't skip", non_skipped, "Tracks/Events"
  print
  print "Raw Hit Trues : ", hit_trues
  print "Raw Recon Trues : ", recon_trues
  print
  print "Skipped", skipped_recon, "SciFi Tracks"
  print "Skipped", skipped_virtual, "Virtuals"
  print "Skipped", skipped_mutual, "Mutually"
  print
  print "Found", missing_counter, "Missing Track Points"
  print
  print "Cut", number_recon_cut, "Particles from the Reconstruction."
  print "Cut", number_virtual_cut, "Particles from the Virtual Planes."
  print 
  print "Found", number_straight_recon_up, "Upstream Straight Tracks"
  print "Found", number_straight_recon_down, "Downstream Straight Tracks"
  print "Removed", number_4point_tracks, "4 Point Tracks"
  print 
  print "Removed", number_virtual_e, "non-muons from Virtual Planes"
  print
  print missing_half_recon, "Incomplete SciFi Tracks"
  print missing_half_virtual, "Incomplete Virtual Tracks"
  print
  print "Mismatched = ", mismatched_hit_num
  print
  print "OOPSies = ", number_OOPS
  print
  print "Calculating Emittances"
  print

  sys.stdout.flush()

  if produce_full_analysis :

    if save_single_particle_amplitudes is True :
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


    virtual1_means = virtual1_bunch.mean( ['x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy', 'bz' ] )
    virtual2_means = virtual2_bunch.mean( ['x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy', 'bz' ] )
    kalman1_means = kalman1_bunch.mean( ['x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy', 'bz' ] )
    kalman2_means = kalman2_bunch.mean( ['x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy', 'bz' ] )


    if re_center_values is True :
      for hit in virtual1_bunch :
        hit.set( 'x', hit.get( 'x' ) - virtual1_means['x'] )
        hit.set( 'y', hit.get( 'y' ) - virtual1_means['y'] )
        hit.set( 'z', hit.get( 'z' ) - virtual1_means['z'] )
        hit.set( 'px', hit.get( 'px' ) - virtual1_means['px'] )
        hit.set( 'py', hit.get( 'py' ) - virtual1_means['py'] )
        hit.set( 'pz', hit.get( 'pz' ) - virtual1_means['pz'] )
      for hit in virtual2_bunch :
        hit.set( 'x', hit.get( 'x' ) - virtual2_means['x'] )
        hit.set( 'y', hit.get( 'y' ) - virtual2_means['y'] )
        hit.set( 'z', hit.get( 'z' ) - virtual2_means['z'] )
        hit.set( 'px', hit.get( 'px' ) - virtual2_means['px'] )
        hit.set( 'py', hit.get( 'py' ) - virtual2_means['py'] )
        hit.set( 'pz', hit.get( 'pz' ) - virtual2_means['pz'] )
      for hit in kalman1_bunch :
        hit.set( 'x', hit.get( 'x' ) - kalman1_means['x'] )
        hit.set( 'y', hit.get( 'y' ) - kalman1_means['y'] )
        hit.set( 'z', hit.get( 'z' ) - kalman1_means['z'] )
        hit.set( 'px', hit.get( 'px' ) - kalman1_means['px'] )
        hit.set( 'py', hit.get( 'py' ) - kalman1_means['py'] )
        hit.set( 'pz', hit.get( 'pz' ) - kalman1_means['pz'] )
      for hit in kalman2_bunch :
        hit.set( 'x', hit.get( 'x' ) - kalman2_means['x'] )
        hit.set( 'y', hit.get( 'y' ) - kalman2_means['y'] )
        hit.set( 'z', hit.get( 'z' ) - kalman2_means['z'] )
        hit.set( 'px', hit.get( 'px' ) - kalman2_means['px'] )
        hit.set( 'py', hit.get( 'py' ) - kalman2_means['py'] )
        hit.set( 'pz', hit.get( 'pz' ) - kalman2_means['pz'] )


    virtual1_emittance = virtual1_bunch.get_emittance( ['x', 'y'] )
    virtual1_weight = virtual1_bunch.bunch_weight()
    virtual1_beta = virtual1_bunch.get_beta( ['x', 'y'] )

    virtual2_emittance = virtual2_bunch.get_emittance( ['x', 'y'] )
    virtual2_weight = virtual2_bunch.bunch_weight()
    virtual2_beta = virtual2_bunch.get_beta( ['x', 'y'] )


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

    sys.stdout.flush()

    if produce_dat_file or save_virtual_planes :
      print_bunches = [ virtual_bunches[ i ] for i in range( len( virtual_bunches ) ) if virtual_bunches[i].bunch_weight() > 1.0 and len( virtual_bunches[i] ) > 1 ]

      if produce_dat_file :
        with open( "measure_emittance_virtual_plane_data.dat", 'w' ) as outfile :
          outfile.write( '# Position (x,y,z)   -   Momentum(x,y,z,tot)  -  Energy  -  Beta  -  Alpha  -  Buch Weight\n' )
          for bunch in print_bunches :
            means = bunch.mean( [ 'x', 'y', 'z', 'px', 'py', 'pz', 'p', 'energy' ] )
            outfile.write( str( means['x'] ) + ' ' + str( means['y'] ) + ' ' + str( means['z'] ) + ' ' +
                           str( means['px'] ) + ' ' + str( means['py'] ) + ' ' + str( means['pz'] ) + ' ' +
                           str( means['p'] ) + ' ' + str( means['energy'] ) + ' ' +
                           str( bunch.get_emittance( ['x','y'] ) ) + ' ' + str( bunch.get_beta( ['x','y'] ) ) + ' ' + str( bunch.get_alpha( ['x','y'] ) ) + ' ' +
                           str( bunch.bunch_weight() ) + '\n' )


      if save_virtual_planes :
        outfile = ROOT.TFile.Open( "measure_emittance_graphs.root", "RECREATE" )
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'bunch_weight', [''], 'm', '' )
        if print_eps :
          canvas.Print( 'measure_emittance_bunchWeight.eps', 'eps' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'emittance', ['x','y'], 'm', 'mm' )
        if print_eps :
          canvas.Print( 'measure_emittance_emittance.eps', 'eps' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'beta', ['x','y'], 'm', 'cm' )
        if print_eps :
          canvas.Print( 'measure_emittance_beta.eps', 'eps' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'alpha', ['x','y'], 'm', 'cm' )
        if print_eps :
          canvas.Print( 'measure_emittance_alpha.eps', 'eps' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'mean', ['energy'], 'm', 'MeV' )
        if print_eps :
          canvas.Print( 'measure_emittance_energy.eps', 'eps' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'mean', ['p'], 'm', 'MeV' )
        if print_eps :
          canvas.Print( 'measure_emittance_momentum.eps', 'eps' )
        graph.Write()
        canvas.Close()
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'mean', ['pz'], 'm', 'MeV' )
        if print_eps :
          canvas.Print( 'measure_emittance_z_momentum.eps', 'eps' )
        graph.Write()
        canvas.Close()
# This doesn't yet work properly...
#      canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'standard_deviation', ['pz'], 'm', 'MeV' )
#      if print_eps :
#        canvas.Print( 'measure_emittance_z_momentum_rms.eps', 'eps' )
#      graph.Write()
#      canvas.Close()  
        canvas, hist, graph = xboa.Bunch.Bunch.root_graph( print_bunches, 'mean', ['z'], 'mean', ['r'], 'm', 'm' )
        if print_eps :
          canvas.Print( 'measure_emittance_radius.eps', 'eps' )
        graph.Write()
        canvas.Close()
        outfile.Close()



    if print_everything :
      printPlots( virtual1_bunch, "traEmu_Virt1_" )
      printPlots( virtual2_bunch, "traEmu_Virt2_" )
      printPlots( kalman1_bunch, "traEmu_Kal1_" )
      printPlots( kalman2_bunch, "traEmu_Kal2_" )


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

    sys.stdout.flush()

    if save_single_particle_amplitudes :
      rootOutFile = ROOT.TFile.Open( "measure_emittance_single_particle_amplitudes.root", "RECREATE" )
      virt_SPA_up.Write()
      virt_SPA_down.Write()
      kalman_SPA_up.Write()
      kalman_SPA_down.Write()
      virt_SPA_change.Write()
      kalman_SPA_change.Write()
      rootOutFile.Close()


    print "=========================================="
    print "    ---    COVARIANCE MATRICES    ---     "
    print "=========================================="
    print
    print "Calculating Covariance Matrices"
    print "Please Wait."
    print 

    cov_up = kalman1_bunch.covariance_matrix( ['x','px','y','py'] )
    cov_down = kalman2_bunch.covariance_matrix( ['x','px','y','py'] )

    true_up = virtual1_bunch.covariance_matrix( ['x','px','y','py'] )
    true_down = virtual2_bunch.covariance_matrix( ['x','px','y','py'] )

    correct_up = None
    correct_down = None

    if upstream_correction != None :
      correct_up = cov_up + upstream_correction
    else :
      correct_up = correction_up.correct_covariance( cov_up )

    if downstream_correction != None :
      correct_down = cov_down + downstream_correction
    else :
      correct_down = correction_down.correct_covariance( cov_down )

    print
    print "Uncorrected Upstream Emittance    = ", math.sqrt( math.sqrt( numpy.linalg.det( cov_up ) ) ) / muon_mass
    print "True Upstream Emittance           = ", math.sqrt( math.sqrt( numpy.linalg.det( true_up ) ) ) / muon_mass
    print "Corrected Upstream Emittance      = ", math.sqrt( math.sqrt( numpy.linalg.det( correct_up ) ) ) / muon_mass
    print

    print
    print "Uncorrected Downstream Emittance  = ", math.sqrt( math.sqrt( numpy.linalg.det( cov_down ) ) ) / muon_mass
    print "True Downstream Emittance         = ", math.sqrt( math.sqrt( numpy.linalg.det( true_down ) ) ) / muon_mass
    print "Corrected Downstream Emittance    = ", math.sqrt( math.sqrt( numpy.linalg.det( correct_down ) ) ) / muon_mass
    print

    if save_corrections is True :
      correction_up.save_R_matrix( 'measure_emittance_R_matrix_up.dat' )
      correction_down.save_R_matrix( 'measure_emittance_R_matrix_down.dat' )
      correction_up.save_C_matrix( 'measure_emittance_C_matrix_up.dat' )
      correction_down.save_C_matrix( 'measure_emittance_C_matrix_down.dat' )
      correction_up.save_full_correction( 'measure_emittance_correction_matrix_up.dat' )
      correction_down.save_full_correction( 'measure_emittance_correction_matrix_down.dat' )


    print "================================================"
    print
    print "Custom Covariances"
    print "Upstream Recon   :", math.sqrt( math.sqrt( recon_covariance_up.get_determinant() ) ) / muon_mass
    print "Upstream MC      :", math.sqrt( math.sqrt( mc_covariance_up.get_determinant() ) ) / muon_mass

    print "Downstream Recon :", math.sqrt( math.sqrt( recon_covariance_down.get_determinant() ) ) / muon_mass
    print "Downstream Recon :", math.sqrt( math.sqrt( mc_covariance_down.get_determinant() ) ) / muon_mass
    print



  if produce_errors :
    print
    print "Producing Final Plots"
    print

    rootOutFile = ROOT.TFile.Open( "measure_emittance_errors.root", "RECREATE" )
    upstream_emittance_error.Write()
    downstream_emittance_error.Write()
    fractional_emittance_error.Write()
    delta_emittance_error.Write()
    upstream_emittance_error_corrected.Write()
    downstream_emittance_error_corrected.Write()
    fractional_emittance_error_corrected.Write()
    delta_emittance_error_corrected.Write()
    rootOutFile.Close()


  if produce_residuals :
    rootOutFile = ROOT.TFile.Open( "measure_emittance_residuals.root", "RECREATE" )
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


    print "Complete."
    print




##################################################################################




def getHits( track ) :
  global recon_plane_num
  global recon_station_num
  trackPoints = track.scifitrackpoints()

  for point in trackPoints :
    if point.station() != recon_station_num or point.plane() != recon_plane_num : 
      continue

    x = point.pos()[0]
    y = point.pos()[1]
    z = point.pos()[2]
    px = point.mom()[0]
    py = point.mom()[1]
    pz = point.mom()[2]
    pid = 13
    bz = 4.0

    hit = xboa.Hit.Hit.new_from_dict( { 'x':x, 'y':y, 'z':z, 'px':px, 'py':py, 'pz':pz, 'pid':pid, 'bz':bz, 'mass':xboa.Common.pdg_pid_to_mass[pid] }, 'energy' )
    return hit
  return None


def getVirtualHit( spechit ) :

  if spechit.GetParticleId() != pid_cut :
    return None

  x = spechit.GetPosition()[0]
  y = spechit.GetPosition()[1]
  z = spechit.GetPosition()[2]
  px = spechit.GetMomentum()[0]
  py = spechit.GetMomentum()[1]
  pz = spechit.GetMomentum()[2]
  pid = abs( spechit.GetParticleId() )

  bz = 4.0

  hit = xboa.Hit.Hit.new_from_dict( { 'x':x, 'y':y, 'z':z, 'px':px, 'py':py, 'pz':pz, 'pid':pid, 'bz':bz, 'mass':xboa.Common.pdg_pid_to_mass[pid] }, 'energy' )
  return hit


def appendVirtualHit( bunch, spechit ) :

  if spechit.GetParticleId() != pid_cut :
    return

  x = spechit.GetPosition()[0]
  y = spechit.GetPosition()[1]
  z = spechit.GetPosition()[2]
  px = spechit.GetMomentum()[0]
  py = spechit.GetMomentum()[1]
  pz = spechit.GetMomentum()[2]
  pid = abs( spechit.GetParticleId() )

  bz = 4.0

  hit = xboa.Hit.Hit.new_from_dict( { 'x':x, 'y':y, 'z':z, 'px':px, 'py':py, 'pz':pz, 'pid':pid, 'bz':bz, 'mass':xboa.Common.pdg_pid_to_mass[pid] }, 'energy' )
  bunch.append( hit )


def check_recon( track ) :
  global number_OOPS
  global number_recon_cut
  global number_straight_recon_up
  global number_straight_recon_down
  global number_4point_tracks
  global recon_trues
  global missing_counter
  global recon_plane_num
  global recon_station_num
  global pt_low_cut
  global allow_4point_tracks

  if track.tracker() == 0 :
    p_value_up.Fill( track.P_value() )
  elif track.tracker() == 1 :
    p_value_down.Fill( track.P_value() )

  if track.P_value() < p_value_cut :
    number_recon_cut += 1
    return False

  if track.GetAlgorithmUsed() == 0 :
    if track.tracker() == 0 :
      number_straight_recon_up += 1
    elif track.tracker() == 1 :
      number_straight_recon_down += 1
    return False

  trackPoints = track.scifitrackpoints()

  if not allow_4point_tracks :
    stationSet = set( [] )
    for point in trackPoints :
      stationSet.add( point.station() )
    if len( stationSet ) < 5 :
      number_4point_tracks += 1
      number_recon_cut += 1
      return False

  for point in trackPoints :
    if point.station() != recon_station_num or point.plane() != recon_plane_num : 
      continue

    x = point.pos()[0]
    y = point.pos()[1]
    px = point.mom()[0]
    py = point.mom()[1]
    pz = point.mom()[2]
    pt = math.sqrt(px**2 + py**2)

    if pt > pt_cut or pz > pz_cut : 
      print "KAL - OOPS :", px, py, pz, track.P_value()
      number_OOPS += 1
      number_recon_cut += 1
      return False
    recon_trues += 1
    return True
  missing_counter += 1
  return False


def check_hit( hit ) :
  global hit_trues
  global number_virtual_cut
  global number_virtual_e
  if hit.GetParticleId() != pid_cut : 
    number_virtual_e += 1
    return False

  x = hit.GetPosition()[0]
  y = hit.GetPosition()[1]
  z = hit.GetPosition()[2]

  if ( math.sqrt( x*x + y*y ) > aperture_cut ) :
    number_virtual_cut += 1
    return False

  px = hit.GetMomentum()[0]
  py = hit.GetMomentum()[1]
  pz = hit.GetMomentum()[2]
  PT = math.sqrt(px**2 + py**2)

  if  PT > pt_cut : 
    number_virtual_cut += 1
    return False
  if PT < pt_low_cut :
    number_virtual_cut += 1
    return False
  if pz > pz_cut or pz < 0.0 :
    number_virtual_cut += 1
    return False

  hit_trues += 1
  return True



def associate_virtuals( scifitracks, virtualhits ) :
  global virtual_plane_dict
  global virt_location_tolerance

  for i in range( scifitracks.size() ) :
    trackpoints = scifitracks[i].scifitrackpoints()
    for point in trackpoints :
      z_pos = point.pos()[2]
      tracker = point.tracker()
      station = point.station() - 1
      plane = point.plane()

      for hit in virtualhits :
        test = hit.GetPosition()[2]
        diff = abs( test - z_pos )
        num, tol = virtual_plane_dict[tracker][station][plane]
        if tol > diff :
          virtual_plane_dict[tracker][station][plane] = ( hit.GetStationId(), diff )

  for i in range( 2 ) :
    for j in range( 5 ) :
      for k in range( 3 ) :
        if virtual_plane_dict[i][j][k][0] < 0 : return False

  return True


def printPlots( bunch, prefix ) :
    canvas, hist = bunch.root_histogram( 'x', 'm', 'px', 'MeV/c' )
    canvas.Print( prefix+"EmittanceX.eps", "eps")
    canvas, hist = bunch.root_histogram( 'y', 'm', 'py', 'MeV/c' )
    canvas.Print( prefix+"EmittanceY.eps", "eps")
    canvas, hist = bunch.root_histogram( 'pt', 'MeV/c' )
    canvas.Print( prefix+"PT.eps", "eps")
    canvas, hist = bunch.root_histogram( 'pz', 'MeV/c' )
    canvas.Print( prefix+"PZ.eps", "eps")

    filename = prefix+"hitList.txt"

    with open( filename, 'w' ) as outfile :
      outfile.write( '#  Position (x,y,z)  |  Momentum (px,py,pz)  |  Magnetic Field (bz) \n\n' )

      for hit in bunch :
        outfile.write( str(hit.get('x')) + ' '
                       + str(hit.get( 'y' )) + ' '
                       + str(hit.get( 'z' )) + ' '
                       + str(hit.get( 'px' )) + ' '
                       + str(hit.get( 'py' )) + ' '
                       + str(hit.get( 'pz' )) + ' '
                       + str(hit.get( 'bz' )) + '\n' )

if __name__ == "__main__":
  run()
  print
#  raw_input()


