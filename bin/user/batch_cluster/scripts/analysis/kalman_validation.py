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
  This script loads tracker Recon and MC data and compares the kalman
  reconstruction to the monte carlo truth to examine the difference and
  analyse the current status of Kalman.


  Script Aglorithm :

  - Create Virtual Plane - Tracker Plane lookup
    - Load Recon and MC Event
    - Find trackpoints in each tracker plane
    - Look for the nearest virtual planes in Z
    - Create lookup dictionary
  - Analyse all events
    - Bin Recon histograms
    - Bin Residual histograms
    - Bin Kalman Pulls
    - Bin P Values
"""

# pylint: disable = W0311, E1101, W0613, W0621, C0103, C0111, W0702, W0611

# Import MAUS Framework (Required!)
import MAUS

# Generic Python imports
import sys
import os
import argparse
import math
import array

# Third Party library import statements
import event_loader
from analysis import tools
from analysis import covariances
from analysis import hit_types
import ROOT


# Useful Constants and configuration
MUON_PID = 13

ALIGNMENT_TOLERANCE = 0.01

TRACK_ALGORITHM = 1

PAIRING_FUNCTION = None

REQUIRE_DATA = False

ALL_PLANE_IDS = [ x for x in range(-15, 0) ] + [ x for x in range(1, 16) ]

ALL_COMPONENTS = [ 'x', 'y', 'px', 'py', 'pz', 'mx', 'my' ] 
ALL_COMPONENT_ERRORS = [ comp+'_err' for comp in ALL_COMPONENTS ]

MOMENTUM_WINDOW = [0.0, 1.0E+9]

REQUIRE_ALL_VIRTUALS = True

REFERENCE_STATION = 1
REFERENCE_PLANE = 0

WEIGHTED_RESIDUALS = False


def init_plots_data() :
  """
    Initialised all the plots in a dictionary to pass around to the other 
    functions.
  """
  plot_dict = { 'recon' : {}, 'mc' : {}, 'residuals' : {}, 'pulls' : {}, 'performance' : {}, 'statistics' : {}, 'errors' : {} }

  for id_num in range( -15, 0 ) + range( 1, 16 ) :
    pl_id = str(id_num)
    for data_type in [ 'recon', 'mc' ] :
      plot_dict[data_type]['plane:'+pl_id+'_xy'] = ROOT.TH2F( 'plane:'+pl_id+'_'+data_type+'_xy', \
                      'Position', 500, -200.0, 200.0, 500, -200.0, 200.0 )

      plot_dict[data_type]['plane:'+pl_id+'_pxpy'] = ROOT.TH2F('plane:'+pl_id+'_'+data_type+'_pxpy', \
                      'Momentum', 500, -200.0, 200.0, 500, -200.0, 200.0 )

      plot_dict[data_type]['plane:'+pl_id+'_mxmy'] = ROOT.TH2F('plane:'+pl_id+'_'+data_type+'_mxmy', \
                      'Gradient', 500, -1.0, 1.0, 500, -1.0, 1.0 )

      plot_dict[data_type]['plane:'+pl_id+'_pt'] = ROOT.TH1F( 'plane:'+pl_id+'_'+data_type+'_pt', \
                                  'Transvere Momentum', 500, 0.0, 200.0 )

      plot_dict[data_type]['plane:'+pl_id+'_pz'] = ROOT.TH1F( 'plane:'+pl_id+'_'+data_type+'_pz', \
                              'Longitudinal Momentum', 500, 100.0, 300.0 )

      for component in ['x', 'y', 'px', 'py'] :
        plot_dict[data_type]['plane:'+pl_id+'_'+component] = ROOT.TH1F( 'plane:'+pl_id+'_'+data_type+'_'+component, \
                                'Component {0}'.format(component), 500, -200.0, 200.0 )
      for component in ['mx', 'my'] :
        plot_dict[data_type]['plane:'+pl_id+'_'+component] = ROOT.TH1F( 'plane:'+pl_id+'_'+data_type+'_'+component, \
                                'Component {0}'.format(component), 500, -1.0, 1.0 )

    if WEIGHTED_RESIDUALS :
      plot_dict['residuals']['plane:'+pl_id+'_xy'] = ROOT.TH2F( 'plane:'+pl_id+'_residuals_xy', \
                      'Position', 501, -10.02, 10.02, 501, -10.02, 10.02 )

      plot_dict['residuals']['plane:'+pl_id+'_pxpy'] = ROOT.TH2F( 'plane:'+pl_id+'_residuals_pxpy', \
                      'Momentum', 501, -10.02, 10.02, 501, -10.02, 10.02 )

      plot_dict['residuals']['plane:'+pl_id+'_mxmy'] = ROOT.TH2F( 'plane:'+pl_id+'_residuals_mxmy', \
                      'Gradient', 501, -10.02, 10.02, 501, -10.02, 10.02 )

      plot_dict['residuals']['plane:'+pl_id+'_pt'] = ROOT.TH1F( 'plane:'+pl_id+'_residuals_pt', \
                                  'Transvere Momentum', 501, -10.02, 10.02 )

#    plot_dict['residuals']['plane:'+pl_id+'_pz'] = ROOT.TH1F( 'plane:'+pl_id+'_residuals_pz', \
#                          'Longitudinal Momentum', 500, -50.0, 50.0 )

      for component in ['x', 'y', 'px', 'py', 'pz' ] :
        plot_dict['residuals']['plane:'+pl_id+'_'+component] = ROOT.TH1F( 'plane:'+pl_id+'_residuals_'+component, \
                              'Component {0}'.format(component), 501, -10.02, 10.02 )
      for component in ['mx', 'my'] :
        plot_dict['residuals']['plane:'+pl_id+'_'+component] = ROOT.TH1F( 'plane:'+pl_id+'_residuals_'+component, \
                              'Component {0}'.format(component), 501, -10.02, 10.02 )

    else :
      plot_dict['residuals']['plane:'+pl_id+'_xy'] = ROOT.TH2F( 'plane:'+pl_id+'_residuals_xy', \
                      'Position', 501, -10.02, 10.02, 501, -10.02, 10.02 )

      plot_dict['residuals']['plane:'+pl_id+'_pxpy'] = ROOT.TH2F( 'plane:'+pl_id+'_residuals_pxpy', \
                      'Momentum', 501, -50.1, 50.1, 501, -50.1, 50.1 )

      plot_dict['residuals']['plane:'+pl_id+'_mxmy'] = ROOT.TH2F( 'plane:'+pl_id+'_residuals_mxmy', \
                      'Gradient', 501, -0.1002, 0.1002, 501, -0.1002, 0.1002 )

      plot_dict['residuals']['plane:'+pl_id+'_pt'] = ROOT.TH1F( 'plane:'+pl_id+'_residuals_pt', \
                                  'Transvere Momentum', 501, -50.1, 50.1 )

#    plot_dict['residuals']['plane:'+pl_id+'_pz'] = ROOT.TH1F( 'plane:'+pl_id+'_residuals_pz', \
#                          'Longitudinal Momentum', 500, -50.0, 50.0 )

      for component in ['x', 'y', 'px', 'py', 'pz' ] :
        plot_dict['residuals']['plane:'+pl_id+'_'+component] = ROOT.TH1F( 'plane:'+pl_id+'_residuals_'+component, \
                              'Component {0}'.format(component), 500, -50.0, 50.0 )
      for component in ['mx', 'my'] :
        plot_dict['residuals']['plane:'+pl_id+'_'+component] = ROOT.TH1F( 'plane:'+pl_id+'_residuals_'+component, \
                              'Component {0}'.format(component), 500, -1.0, 1.0 )



    plot_dict['pulls']['plane:'+pl_id+'_pulls'] = ROOT.TH1F( 'plane:'+pl_id+'_pulls', "Kalman Pulls", \
                                                             101, -5.05, 5.05 )



  for component in ['x_err', 'y_err', 'px_err', 'py_err', 'pz_err' ] :
    plot_dict['errors'][component] = ROOT.TH2F( 'errors_'+component, \
                          'Component {0}'.format(component), 31, -15.5, 15.5, 100, 0.0, 10.0 )
  for component in ['mx_err', 'my_err'] :
    plot_dict['errors'][component] = ROOT.TH2F( 'errors_'+component, \
                          'Component {0}'.format(component), 31, -15.5, 15.5, 100, 0.0, 0.1 )



  plot_dict['pulls']['all'] = ROOT.TH2F( 'all_pulls', "Kalman Pulls", 31, -15.5, 15.5, 1000, -50.0, 50.0)


  plot_dict['statistics']['up_chi_sq'] = ROOT.TH1F( 'upstream_chi_sq', 'Upstream ${chi}^{2} Plots', \
                                                      200, 0.0, 100.0 )
  plot_dict['statistics']['down_chi_sq'] = ROOT.TH1F( 'upstream_chi_sq', 'Upstream ${chi}^{2} Plots', \
                                                      200, 0.0, 100.0 )

  plot_dict['statistics']['up_chi_sq_df'] = ROOT.TH1F( 'upstream_chi_sq_df', 'Upstream ${chi}^{2}/NDF Plots', \
                                                      500, 0.0, 100.0 )
  plot_dict['statistics']['down_chi_sq_df'] = ROOT.TH1F( 'upstream_chi_sq_df', 'Upstream ${chi}^{2}/NDF Plots', \
                                                      500, 0.0, 100.0 )

  plot_dict['statistics']['up_p_value'] = ROOT.TH1F( 'upstream_p_value', 'Upstream P-Value Plots', \
                                                      200, 0.0, 1.0 )
  plot_dict['statistics']['down_p_value'] = ROOT.TH1F( 'upstream_p_value', 'Upstream P-Value Plots', \
                                                      200, 0.0, 1.0 )

  plot_dict['statistics']['up_ndf'] = ROOT.TH1F( 'upstream_ndf', 'Upstream Number Degrees of Freedom Plots', \
                                                      20, 0.0, 20.0 )
  plot_dict['statistics']['down_ndf'] = ROOT.TH1F( 'upstream_ndf', 'Upstream Number Degrees of Freedom Plots', \
                                                      20, 0.0, 20.0 )

  plot_dict['statistics']['up_p_value_pt'] = ROOT.TH2F( 'upstream_p_value_pt', "Upstream P-Value against p_{#perp}", \
                                                      100, 0.0, 1.0, 200, 0.0, 200.0 )
  plot_dict['statistics']['down_p_value_pt'] = ROOT.TH2F( 'downstream_p_value_pt', "Upstream P-Value against p_{#perp}", \
                                                      100, 0.0, 1.0, 200, 0.0, 200.0 )

  plot_dict['statistics']['up_p_value_pz'] = ROOT.TH2F( 'upstream_p_value_pz', "Upstream P-Value against p_{z}", \
                                                      100, 0.0, 1.0, 100, 150.0, 250.0 )
  plot_dict['statistics']['down_p_value_pz'] = ROOT.TH2F( 'downstream_p_value_pz', "Upstream P-Value against p_{z}", \
                                                      100, 0.0, 1.0, 100, 150.0, 250.0 )


  data_dict = { 'counters' : {}, 'data' : {} }

  data_dict['counters']['missing_virtual_hits'] = 0
  data_dict['counters']['missing_reference_hits'] = 0
  data_dict['counters']['found_tracks_up'] = 0
  data_dict['counters']['found_tracks_down'] = 0
  data_dict['counters']['found_pairs'] = 0

  return plot_dict, data_dict


def create_virtual_plane_dict(file_reader, virtual_plane_dict) :
  """
    Matches up scifitrackpoints to virtual planes to make a lookup dictionary 
  """
#  virtual_plane_dict = {}
  for num in range( -15, 0, 1 ) :
    virtual_plane_dict[ num ] = ( -1, (ALIGNMENT_TOLERANCE * 100.0) )
  for num in range( 1, 16, 1 ) :
    virtual_plane_dict[ num ] = ( -1, (ALIGNMENT_TOLERANCE * 100.0) )

  while file_reader.next_event() :
    scifi_event = file_reader.get_event( 'scifi' )
    mc_event = file_reader.get_event( 'mc' )

    tracks = scifi_event.scifitracks()
    for track in tracks :
      trackpoints = track.scifitrackpoints()
      for trkpt in trackpoints :
        z_pos = trkpt.pos().z()
        plane_id = tools.calculate_plane_id(trkpt.tracker(), trkpt.station(), \
                                                              trkpt.plane() )

        for vhit_num in xrange(mc_event.GetVirtualHitsSize()) :
          vhit = mc_event.GetAVirtualHit(vhit_num)
          diff = math.fabs(vhit.GetPosition().z() - z_pos)

          if diff < virtual_plane_dict[ plane_id ][1] :
            virtual_plane_dict[ plane_id ] = ( vhit.GetStationId(), diff )
#            print plane_id, vhit.GetStationId(), diff

    done = True
    num_found = 0
    for plane in virtual_plane_dict :
      if virtual_plane_dict[plane][1] > ALIGNMENT_TOLERANCE :
#        print plane, virtual_plane_dict[plane]
        done = False
      else :
        num_found += 1
    sys.stdout.write( "- Finding Virtual Planes : Running ({0} of {1})\r".format(num_found, 30) )
    sys.stdout.flush()
    if done :
      break
  else :
    if REQUIRE_ALL_VIRTUALS :
      raise ValueError("Could not locate all virtuals planes")

  file_reader.reset()
  return virtual_plane_dict


def make_scifi_mc_pairs(plot_dict, data_dict, virtual_plane_dict, \
                                                        scif_event, mc_event) :
  """
    Make pairs of SciFiTrackpoints and MC VirtualHits
  """
  paired_hits = []

  tracks = scifi_event.scifitracks()
  for track in tracks :
    if track.GetAlgorithmUsed() != TRACK_ALGORITHM :
      continue
    if track.tracker() == 0 :
      plot_dict['statistics']['up_p_value'].Fill(track.P_value())
      plot_dict['statistics']['up_chi_sq'].Fill(track.chi2())
      plot_dict['statistics']['up_chi_sq_df'].Fill(track.chi2()/track.ndf())
      plot_dict['statistics']['up_ndf'].Fill(track.ndf())
    else :
      plot_dict['statistics']['down_p_value'].Fill(track.P_value())
      plot_dict['statistics']['down_chi_sq'].Fill(track.chi2())
      plot_dict['statistics']['down_chi_sq_df'].Fill(track.chi2()/track.ndf())
      plot_dict['statistics']['down_ndf'].Fill(track.ndf())

    trackpoints = track.scifitrackpoints()
    trackpoint = None

# Find a reference trackpoint
    for trackpoint in trackpoints :
      if trackpoint.station() == REFERENCE_STATION and trackpoint.plane() == REFERENCE_PLANE :
        pt = math.sqrt( trackpoint.mom().x()**2 + trackpoint.mom().y()**2 )
        pz = trackpoint.mom().z()
        if track.tracker() == 0 :
          plot_dict['statistics']['up_p_value_pt'].Fill(track.P_value(), pt)
          plot_dict['statistics']['up_p_value_pz'].Fill(track.P_value(), math.fabs(pz))
        else :
          plot_dict['statistics']['down_p_value_pt'].Fill(track.P_value(), pt)
          plot_dict['statistics']['down_p_value_pz'].Fill(track.P_value(), math.fabs(pz))

# If no data then give up!
      if REQUIRE_DATA and not trackpoint.has_data() :
        data_dict['counters']['missing_reference_hits'] += 1
        continue

      expected_virtual_plane = virtual_plane_dict[tools.calculate_plane_id( \
            trackpoint.tracker(), trackpoint.station(), trackpoint.plane())][0]


# Find the virtual hit
      for vhit_num in xrange(mc_event.GetVirtualHitsSize()) :
        vhit = mc_event.GetAVirtualHit(vhit_num)
        if vhit.GetStationId() == expected_virtual_plane and \
                                        abs(vhit.GetParticleId()) == MUON_PID :
          virtual_hit = vhit
          break
      else :
        data_dict['counters']['missing_virtual_hits'] += 1
        continue

      momentum = math.sqrt( vhit.GetMomentum().x()**2 + vhit.GetMomentum().y()**2 + vhit.GetMomentum().z()**2 )
      if momentum < MOMENTUM_WINDOW[0] or momentum > MOMENTUM_WINDOW[1] :
        continue

# Add to list
      pl_id = tools.calculate_plane_id( trackpoint.tracker(), trackpoint.station(), trackpoint.plane() )
      if trackpoint.has_data() :
        new_trackpoint = { 'pos' : [], 'mom' : [], 'grad': [], 'id' : pl_id, 'pull' : trackpoint.pull(), 'pos_err' : [], 'mom_err' : [], 'grad_err' : [] }
      else :
        new_trackpoint = { 'pos' : [], 'mom' : [], 'grad': [], 'id' : pl_id, 'pull' : None, 'pos_err' : [], 'mom_err' : [], 'grad_err' : [] }

      new_trackpoint['pos'].append(trackpoint.pos().x())
      new_trackpoint['pos'].append(trackpoint.pos().y())
      new_trackpoint['pos'].append(trackpoint.pos().z())
      new_trackpoint['mom'].append(trackpoint.mom().x())
      new_trackpoint['mom'].append(trackpoint.mom().y())
      new_trackpoint['mom'].append(trackpoint.mom().z())
      new_trackpoint['grad'].append(trackpoint.gradient().x())
      new_trackpoint['grad'].append(trackpoint.gradient().y())
      new_trackpoint['pos_err'].append(trackpoint.pos_error().x())
      new_trackpoint['pos_err'].append(trackpoint.pos_error().y())
      new_trackpoint['pos_err'].append(trackpoint.pos_error().z())
      new_trackpoint['mom_err'].append(trackpoint.mom_error().x())
      new_trackpoint['mom_err'].append(trackpoint.mom_error().y())
      new_trackpoint['mom_err'].append(trackpoint.mom_error().z())
      new_trackpoint['grad_err'].append(trackpoint.gradient_error().x())
      new_trackpoint['grad_err'].append(trackpoint.gradient_error().y())
      paired_hits.append((new_trackpoint, virtual_hit))
      data_dict['counters']['found_pairs'] += 1

  found_up = 0
  found_down = 0

  for scifi_hit, virt_hit in paired_hits :
#    if scifi_hit.tracker() == 0 :
    if new_trackpoint['id'] < 0 :
      found_up += 1
    else :
      found_down += 1


  for track in tracks :
    if track.GetAlgorithmUsed() != TRACK_ALGORITHM :
      continue


    
  data_dict['counters']['found_tracks_up'] += found_up
  data_dict['counters']['found_tracks_down'] += found_down

  return paired_hits


def make_spacepoint_mc_pairs(plot_dict, data_dict, virtual_plane_dict, \
                                                        scif_event, mc_event) :
  """
    Make pairs of SciFiSpacepoints and MC VirtualHits
  """
  paired_hits = []

  spacepoints = scifi_event.spacepoints()

# Find a reference spacepoint
  for spacepoint in spacepoints :
    for plane in range( 3 ) :

      pl_id = tools.calculate_plane_id(spacepoint.get_tracker(), spacepoint.get_station(), plane)

      expected_virtual_plane = virtual_plane_dict[pl_id][0]

# Find the virtual hit
      for vhit_num in xrange(mc_event.GetVirtualHitsSize()) :
        vhit = mc_event.GetAVirtualHit(vhit_num)
        if vhit.GetStationId() == expected_virtual_plane and \
                                        abs(vhit.GetParticleId()) == MUON_PID :
          virtual_hit = vhit
          break
      else :
        data_dict['counters']['missing_virtual_hits'] += 1
        continue

# Add to list
      new_spacepoint = { 'pos' : [], 'mom' : [0.0, 0.0, 200.0], 'grad' : [0.0, 0.0], 'id' : pl_id, 'pull' : 0.0, \
        'pos_err' : [-1.0, -1.0, -1.0], 'mom_err' : [-1.0, -1.0, -1.0], 'grad_err' : [-1.0, -1.0] }

      new_spacepoint['pos'].append(spacepoint.get_global_position().x())
      new_spacepoint['pos'].append(spacepoint.get_global_position().y())
      new_spacepoint['pos'].append(spacepoint.get_global_position().z())
      paired_hits.append((new_spacepoint, virtual_hit))
      data_dict['counters']['found_pairs'] += 1

  found_up = 0
  found_down = 0

  for scifi_hit, virt_hit in paired_hits :
    if scifi_hit['id'] < 0 :
      found_up += 1
    else :
      found_down += 1
    
  data_dict['counters']['found_tracks_up'] += found_up
  data_dict['counters']['found_tracks_down'] += found_down

  return paired_hits


def make_patrec_mc_pairs(plot_dict, data_dict, virtual_plane_dict, \
                                                        scif_event, mc_event) :
  """
    Make pairs of Pattern Recognition Tracks and MC VirtualHits
  """
  paired_hits = []

  if TRACK_ALGORITHM == 0 :
    tracks = scifi_event.straightprtracks()
  elif TRACK_ALGORITHM == 1 :
    tracks = scifi_event.helicalprtracks()

# Find a reference spacepoint
  for track in tracks :

    pl_id = tools.calculate_plane_id(track.get_tracker(), 1, 0) # Only examine ref plane atm

    expected_virtual_plane = virtual_plane_dict[pl_id][0]

# Find the virtual hit
    for vhit_num in xrange(mc_event.GetVirtualHitsSize()) :
      vhit = mc_event.GetAVirtualHit(vhit_num)
      if vhit.GetStationId() == expected_virtual_plane and \
                                      abs(vhit.GetParticleId()) == MUON_PID :
        virtual_hit = vhit
        break
    else :
      data_dict['counters']['missing_virtual_hits'] += 1
      continue

# Add to list
    new_pr_point = { 'pos' : [], 'mom' : [], 'grad' : [], 'id' : pl_id, 'pull' : 0.0, \
        'pos_err' : [-1.0, -1.0, -1.0], 'mom_err' : [-1.0, -1.0, -1.0], grad_err : [-1.0, -1.0] }

    new_pr_point['pos'].append(track.get_reference_position().x())
    new_pr_point['pos'].append(track.get_reference_position().y())
    new_pr_point['pos'].append(track.get_reference_position().z())
    new_pr_point['mom'].append(track.get_reference_momentum().x())
    new_pr_point['mom'].append(track.get_reference_momentum().y())
    new_pr_point['mom'].append(track.get_reference_momentum().z())
    new_pr_point['grad'].append(track.get_reference_momentum().x()/track.get_reference_momentum.z())
    new_pr_point['grad'].append(track.get_reference_momentum().y()/track.get_reference_momentum.z())
    paired_hits.append((new_pr_point, virtual_hit))
    data_dict['counters']['found_pairs'] += 1

    if track.get_tracker() == 0 :
      plot_dict['statistics']['up_p_value'].Fill(ROOT.TMath.Prob(track.get_chi_squared(), track.get_ndf()))
      plot_dict['statistics']['up_chi_sq'].Fill(track.get_chi_squared())
      plot_dict['statistics']['up_chi_sq_df'].Fill(track.get_chi_squared()/track.get_ndf())
      plot_dict['statistics']['up_ndf'].Fill(track.get_ndf())
    else :
      plot_dict['statistics']['down_p_value'].Fill(ROOT.TMath.Prob(track.get_chi_squared(), track.get_ndf()))
      plot_dict['statistics']['down_chi_sq'].Fill(track.get_chi_squared())
      plot_dict['statistics']['down_chi_sq_df'].Fill(track.get_chi_squared()/track.get_ndf())
      plot_dict['statistics']['down_ndf'].Fill(track.get_ndf())

  found_up = 0
  found_down = 0

  for scifi_hit, virt_hit in paired_hits :
    if scifi_hit['id'] < 0 :
      found_up += 1
    else :
      found_down += 1
    
  data_dict['counters']['found_tracks_up'] += found_up
  data_dict['counters']['found_tracks_down'] += found_down

  return paired_hits


def fill_plots(plot_dict, data_dict, hit_pairs) :
  """
    Fill Plots with Track and Residual Data
  """
  for scifi_hit, virt_hit in hit_pairs :
#    tracker = scifi_hit.tracker()
#    station = scifi_hit.station()
#    plane = scifi_hit.plane()
#    pl_id = str(tools.calculate_plane_id( tracker, station, plane ))


#    scifi_pos = [scifi_hit.pos().x(), scifi_hit.pos().y(), scifi_hit.pos().z()]
#    scifi_mom = [scifi_hit.mom().x(), scifi_hit.mom().y(), scifi_hit.mom().z()]
    scifi_pos = scifi_hit['pos']
    scifi_mom = scifi_hit['mom']
    scifi_pos_err = scifi_hit['pos_err']
    scifi_mom_err = scifi_hit['mom_err']
    scifi_grad_err = scifi_hit['grad_err']
    pl_id = str(scifi_hit['id'])
    virt_pos = [virt_hit.GetPosition().x(), \
                        virt_hit.GetPosition().y(), virt_hit.GetPosition().z()]
    virt_mom = [virt_hit.GetMomentum().x(), \
                        virt_hit.GetMomentum().y(), virt_hit.GetMomentum().z()]

    if WEIGHTED_RESIDUALS :
      res_pos = [ (scifi_pos[0] - virt_pos[0]) / scifi_pos_err[0], \
                  (scifi_pos[1] - virt_pos[1]) / scifi_pos_err[1], \
                  (scifi_pos[2] - virt_pos[2]) ]
      res_mom = [ (scifi_mom[0] - virt_mom[0]) / scifi_mom_err[0], \
                  (scifi_mom[1] - virt_mom[1]) / scifi_mom_err[1], \
                  (scifi_mom[2] - virt_mom[2]) / scifi_mom_err[2] ]
      res_gra = [ (scifi_mom[0]/scifi_mom[2] - virt_mom[0]/virt_mom[2]) / scifi_grad_err[0], \
                  (scifi_mom[1]/scifi_mom[2] - virt_mom[1]/virt_mom[2]) / scifi_grad_err[1] ]

    else :
      res_pos = [ scifi_pos[0] - virt_pos[0], \
                  scifi_pos[1] - virt_pos[1], \
                  scifi_pos[2] - virt_pos[2] ]
      res_mom = [ scifi_mom[0] - virt_mom[0], \
                  scifi_mom[1] - virt_mom[1], \
                  scifi_mom[2] - virt_mom[2] ]
      res_gra = [ scifi_mom[0]/scifi_mom[2] - virt_mom[0]/virt_mom[2], \
                  scifi_mom[1]/scifi_mom[2] - virt_mom[1]/virt_mom[2] ]

    Pt_mc = math.sqrt( virt_mom[0] ** 2 + virt_mom[1] ** 2 )
    Pz_mc = virt_mom[2]

    Pt_recon = math.sqrt( scifi_mom[0] ** 2 + scifi_mom[1] ** 2 )
    Pt_res = Pt_recon - Pt_mc


    plot_dict['mc']['plane:'+pl_id+'_x'].Fill( virt_pos[0] )
    plot_dict['mc']['plane:'+pl_id+'_y'].Fill( virt_pos[1] )
    plot_dict['mc']['plane:'+pl_id+'_px'].Fill( virt_mom[0] )
    plot_dict['mc']['plane:'+pl_id+'_py'].Fill( virt_mom[1] )
    plot_dict['mc']['plane:'+pl_id+'_xy'].Fill( virt_pos[0], virt_pos[1] )
    plot_dict['mc']['plane:'+pl_id+'_pxpy'].Fill( virt_mom[0], virt_mom[1] )
    plot_dict['mc']['plane:'+pl_id+'_mx'].Fill( virt_mom[0]/virt_mom[2] )
    plot_dict['mc']['plane:'+pl_id+'_my'].Fill( virt_mom[1]/virt_mom[2] )
    plot_dict['mc']['plane:'+pl_id+'_mxmy'].Fill( virt_mom[0]/virt_mom[2], virt_mom[1]/virt_mom[2] )
    plot_dict['mc']['plane:'+pl_id+'_pt'].Fill( Pt_mc )
    plot_dict['mc']['plane:'+pl_id+'_pz'].Fill( virt_mom[2] )

    plot_dict['recon']['plane:'+pl_id+'_x'].Fill( scifi_pos[0] )
    plot_dict['recon']['plane:'+pl_id+'_y'].Fill( scifi_pos[1] )
    plot_dict['recon']['plane:'+pl_id+'_px'].Fill( scifi_mom[0] )
    plot_dict['recon']['plane:'+pl_id+'_py'].Fill( scifi_mom[1] )
    plot_dict['recon']['plane:'+pl_id+'_xy'].Fill( scifi_pos[0], scifi_pos[1] )
    plot_dict['recon']['plane:'+pl_id+'_pxpy'].Fill( scifi_mom[0], scifi_mom[1] )
    plot_dict['recon']['plane:'+pl_id+'_mx'].Fill( scifi_mom[0]/scifi_mom[2] )
    plot_dict['recon']['plane:'+pl_id+'_my'].Fill( scifi_mom[1]/scifi_mom[2] )
    plot_dict['recon']['plane:'+pl_id+'_mxmy'].Fill( scifi_mom[0]/scifi_mom[2], scifi_mom[1]/scifi_mom[2] )
    plot_dict['recon']['plane:'+pl_id+'_pt'].Fill( Pt_recon )
    plot_dict['recon']['plane:'+pl_id+'_pz'].Fill( scifi_mom[2] )

    plot_dict['residuals']['plane:'+pl_id+'_x'].Fill( res_pos[0] )
    plot_dict['residuals']['plane:'+pl_id+'_y'].Fill( res_pos[1] )
    plot_dict['residuals']['plane:'+pl_id+'_px'].Fill( res_mom[0] )
    plot_dict['residuals']['plane:'+pl_id+'_py'].Fill( res_mom[1] )
    plot_dict['residuals']['plane:'+pl_id+'_xy'].Fill( res_pos[0], res_pos[1] )
    plot_dict['residuals']['plane:'+pl_id+'_pxpy'].Fill( res_mom[0], res_mom[1] )
    plot_dict['residuals']['plane:'+pl_id+'_mx'].Fill( res_gra[0] )
    plot_dict['residuals']['plane:'+pl_id+'_my'].Fill( res_gra[1] )
    plot_dict['residuals']['plane:'+pl_id+'_mxmy'].Fill( res_gra[0], res_gra[1] )
    plot_dict['residuals']['plane:'+pl_id+'_pt'].Fill( Pt_res )
    plot_dict['residuals']['plane:'+pl_id+'_pz'].Fill( res_mom[2] )

    plot_dict['errors']['x_err'].Fill( float(pl_id), scifi_pos_err[0] )
    plot_dict['errors']['y_err'].Fill( float(pl_id), scifi_pos_err[1] )
    plot_dict['errors']['px_err'].Fill( float(pl_id), scifi_mom_err[0] )
    plot_dict['errors']['py_err'].Fill( float(pl_id), scifi_mom_err[1] )
    plot_dict['errors']['pz_err'].Fill( float(pl_id), scifi_mom_err[2] )
    plot_dict['errors']['mx_err'].Fill( float(pl_id), scifi_grad_err[0] )
    plot_dict['errors']['my_err'].Fill( float(pl_id), scifi_grad_err[1] )

    if scifi_hit['pull'] is not None :
      plot_dict['pulls']['plane:'+pl_id+'_pulls'].Fill(scifi_hit['pull'])
      plot_dict['pulls']['all'].Fill(float(pl_id), scifi_hit['pull'])


 
def analyse_plots(plot_dict, data_dict) :

  for component in ALL_COMPONENTS :
    plane_num = array.array( 'd' )
    zeros = array.array( 'd' )
    mean = array.array( 'd' )
    std = array.array( 'd' )

    for plane in ALL_PLANE_IDS :
      pl_id = str(plane)
      plot = plot_dict['residuals']['plane:'+pl_id+'_'+component]

      zeros.append( 0.0 )
      plane_num.append( float(pl_id) )
      mean.append( plot.GetMean() )
      std.append( plot.GetStdDev() )

    performance_graph = ROOT.TGraphErrors( len(zeros), plane_num, mean, zeros, std )

    plot_dict['performance']['resolution_'+component] = performance_graph

#  for component in ALL_COMPONENT_ERRORS :
#    plane_num = array.array( 'd' )
#    zeros = array.array( 'd' )
#    mean = array.array( 'd' )
#    std = array.array( 'd' )
#
#    for plane in ALL_PLANE_IDS :
#      pl_id = str(plane)
#      plot = plot_dict['recon']['plane:'+pl_id+'_'+component]
#
#      zeros.append( 0.0 )
#      plane_num.append( float(pl_id) )
#      mean.append( plot.GetMean() )
#      std.append( plot.GetStdDev() )
#
#    performance_graph = ROOT.TGraphErrors( len(zeros), plane_num, mean, zeros, std )
#
#    plot_dict['performance']['resolution_'+component] = performance_graph




if __name__ == "__main__" : 
  ROOT.gROOT.SetBatch( True )
  ROOT.gErrorIgnoreLevel = ROOT.kError

  parser = argparse.ArgumentParser( description='An example script showing '+\
      'some basic data extraction and analysis routines' )

  parser.add_argument( 'maus_root_files', nargs='+', help='List of MAUS '+\
                  'output root files containing reconstructed straight tracks')

  parser.add_argument( '-N', '--max_num_events', type=int, \
                                   help='Maximum number of events to analyse.')

  parser.add_argument( '-O', '--output_filename', \
             default='kalman_validation_plots', help='Set the output filename')

  parser.add_argument( '-D', '--output_directory', \
                                 default='./', help='Set the output directory')

  parser.add_argument( '-P', '--print_plots', action='store_true', \
                        help="Flag to save the plots as individual pdf files" )

  parser.add_argument( '-R', '--weighted_residuals', action='store_true', \
                help="Weight all the residuals with the reconstruction error" )

  parser.add_argument( '--track_algorithm', type=int, default=1, \
                          help="Specify the track reconstruction algorithm. "+\
                             "1 for Helical Tracks and 0 for Straight Tracks" )

  parser.add_argument( '--momentum_window', nargs=2, default=MOMENTUM_WINDOW, \
type=float, help="Specify a momentum window within which to run the analysis.")

  parser.add_argument( '--data_type', type=str, default="kalman", \
                     help="Specify the type of reconstructed data to analyse. \
                                   Choose from \"kalman\" or \"spacepoint\"." )

#  parser.add_argument( '-C', '--configuration_file', help='Configuration '+\
#      'file for the reconstruction. I need the geometry information' )

  try :
    namespace = parser.parse_args()

    TRACK_ALGORITHM = namespace.track_algorithm

    MOMENTUM_WINDOW = namespace.momentum_window

    WEIGHTED_RESIDUALS = namespace.weighted_residuals

    if namespace.data_type == 'kalman' :
      PAIRING_FUNCTION = make_scifi_mc_pairs
    elif namespace.data_type == 'spacepoint' :
      PAIRING_FUNCTION = make_spacepoint_mc_pairs
    elif namespace.data_type == 'patrec' :
      PAIRING_FUNCTION = make_patrec_mc_pairs
    else :
      raise ValueError( \
                  "Cannot analyse data type: {0}".format(namespace.data_type) )
  except BaseException as ex:
    raise
  else :
##### 1. Load MAUS globals and geometry. - NOT NECESSARY AT PRESENT
    # geom = load_tracker_geometry(namespace.configuration_file)

##### 2. Intialise plots ######################################################
    print
    sys.stdout.write( "\n- Initialising Plots : Running\r" )
    sys.stdout.flush()
    plot_dict, data_dict = init_plots_data()
    sys.stdout.write(   "- Initialising Plots : Done   \n" )

    file_reader = event_loader.maus_reader(namespace.maus_root_files)
##### 3. Initialise Plane Dictionary ##########################################
    sys.stdout.write( "\n- Finding Virtual Planes : Running\r" )
    sys.stdout.flush()
    virtual_plane_dictionary = {}
    try :
      create_virtual_plane_dict(file_reader, virtual_plane_dictionary)
    except KeyboardInterrupt :
      print
      print " ###  Keyboard Interrupt  ###"
      print
    sys.stdout.write(   "- Finding Virtual Planes : Done   \n" )

##### 4. Load Events ##########################################################
    print "\n- Loading Spills...\n"
    try :
      while file_reader.next_event() and \
               file_reader.get_total_num_events() != namespace.max_num_events :
        try :
          sys.stdout.write( 
              '  Spill ' + str(file_reader.get_current_spill_number()) + \
              ' of ' + str(file_reader.get_current_number_spills()) + \
              ' in File ' + str(file_reader.get_current_filenumber()) + \
              ' of ' + str(file_reader.get_number_files()) + '             \r')
          sys.stdout.flush()

          scifi_event = file_reader.get_event( 'scifi' )
          mc_event = file_reader.get_event( 'mc' )

##### 5. Extract tracks and Fill Plots ########################################

          paired_hits = PAIRING_FUNCTION(plot_dict, data_dict, \
                               virtual_plane_dictionary, scifi_event, mc_event)

          fill_plots(plot_dict, data_dict, paired_hits)

        except ValueError :
          print "An Error Occured. Skipping Spill: " + \
                str(file_reader.get_current_spill_number()) + \
                " In File: " + str(file_reader.get_current_filenumber()) + "\n"
          continue

    except KeyboardInterrupt :
      print
      print " ###  Keyboard Interrupt  ###"
      print
    print "- {0:0.0f} Spills Loaded                                 ".format( \
                                            file_reader.get_total_num_spills())
##### 6. Analysing Plots ######################################################
    print"\n- Analysing Data...\n"

    analyse_plots(plot_dict, data_dict)

##### 7. Saving Plots and Data ################################################

    sys.stdout.write( "\n- Saving Plots and Data : Running\r" )
    sys.stdout.flush()

    plots_filename = os.path.join(namespace.output_directory, namespace.output_filename+".root")

    if namespace.print_plots :
      tools.print_plots(plot_dict, namespace.output_directory)

    tools.save_plots(plot_dict, plots_filename)
    sys.stdout.write(   "- Saving Plots and Data : Done   \n" )

  print 
  print "Complete."
  print

