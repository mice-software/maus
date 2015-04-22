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
import operator

import MAUS
import ROOT
import xboa
import numpy
import math
from array import array

import libMausCpp

import emittance_analysis as em_ana


####################################################################################################


## Other Stuff
associate_max_attempts = 1000
associate_tolerance = 0.001

z_pairing_tolerance = 0.02 # Required this large due to lack of patrec z info!
r_pairing_tolerance = 200.0

virtual_plane_events = 10

require_clean_tracks = False

cut_straight_tracks = False
cut_residual = 180.0
cut_p_value = 0.05
cut_pid = -13

print_virtual_planes = False

save_data_all_planes = True

analyse_kalman = True
analyse_patrec = True


####################################################################################################
## Performance plots

number_spacepoints = ROOT.TH1F( "number_spacepoints", "Number of spacepoints in each tracker", 21, 0.0, 21.0 );
number_clusters = ROOT.TH1F( "number_clusters", "Number of clusters in each tracker", 51, 0.0, 51.0 );

patrec_up_charge = ROOT.TH1F( "patrec_up_charge", "Charge of upstream Pat Rec", 3, -1.0, 2.0 );
patrec_up_numberpoints = ROOT.TH1F( "patrec_up_numberpoints", "Number of stations in upstream Pat Rec", 6, 0.0, 6.0 );
patrec_up_chisquaredndf = ROOT.TH1F( "patrec_up_chisquaredndf", "Chisquared per degree of freedom in upstream Pat Rec", 200, 0.0, 200.0 );
patrec_up_tracktype = ROOT.TH1F( "patrec_up_tracktype", "Tracktypes in upstream Pat Rec", 2, 0.0, 2.0 );

patrec_up_residual_x = ROOT.TH1F( "patrec_up_res_x", "Upstream x Residual", 100, -20.0, 20.0 )
patrec_up_residual_y = ROOT.TH1F( "patrec_up_res_y", "Upstream y Residual", 100, -20.0, 20.0 )
patrec_up_residual_px = ROOT.TH1F( "patrec_up_res_px", "Upstream px Residual", 100, -20.0, 20.0 )
patrec_up_residual_py = ROOT.TH1F( "patrec_up_res_py", "Upstream py Residual", 100, -20.0, 20.0 )
patrec_up_residual_pz = ROOT.TH1F( "patrec_up_res_pz", "Upstream pz Residual", 100, -20.0, 20.0 )

patrec_down_charge = ROOT.TH1F( "patrec_down_charge", "Charge of downstream Pat Rec", 3, -1.0, 2.0 );
patrec_down_numberpoints = ROOT.TH1F( "patrec_down_numberpoints", "Number of stations in downstream Pat Rec", 6, 0.0, 6.0 );
patrec_down_chisquaredndf = ROOT.TH1F( "patrec_down_chisquaredndf", "Chisquared per degree of freedom in downstream Pat Rec", 200, 0.0, 200.0 );
patrec_down_tracktype = ROOT.TH1F( "patrec_down_tracktype", "Tracktypes in downstream Pat Rec", 2, 0.0, 2.0 );

patrec_down_residual_x = ROOT.TH1F( "patrec_down_res_x", "Downstream x Residual", 100, -20.0, 20.0 )
patrec_down_residual_y = ROOT.TH1F( "patrec_down_res_y", "Downstream y Residual", 100, -20.0, 20.0 )
patrec_down_residual_px = ROOT.TH1F( "patrec_down_res_px", "Downstream px Residual", 100, -20.0, 20.0 )
patrec_down_residual_py = ROOT.TH1F( "patrec_down_res_py", "Downstream py Residual", 100, -20.0, 20.0 )
patrec_down_residual_pz = ROOT.TH1F( "patrec_down_res_pz", "Downstream pz Residual", 100, -20.0, 20.0 )

kalman_up_plane = ROOT.TH1F( "kalman_up_plane", "Used planes in upstream Kalman", 15, 0.0, 15.0 );
kalman_up_chisquaredndf = ROOT.TH1F( "kalman_up_chisquaredndf", "Chisquared per degree of freedom in upstream Kalman", 200, 0.0, 200.0 );
kalman_up_pvalue = ROOT.TH1F( "kalman_up_pvalue", "P Values in upstream Kalman", 100, 0.0, 1.0 );

kalman_down_plane = ROOT.TH1F( "kalman_down_plane", "Used planes in downstream Kalman", 15, 0.0, 15.0 );
kalman_down_chisquaredndf = ROOT.TH1F( "kalman_down_chisquaredndf", "Chisquared per degree of freedom in downstream Kalman", 200, 0.0, 200.0 );
kalman_down_pvalue = ROOT.TH1F( "kalman_down_pvalue", "P Values in downstream Kalman", 100, 0.0, 1.0 );

kalman_up_pull = ROOT.TH2F( "kalman_up_pull", "Distribution of residuals for each plane", 14, 0.0, 14.0, 500, 50.0, 50.0 );
kalman_up_residuals = ROOT.TH2F( "kalman_up_residuals", "Distribution of residuals for each plane", 14, 0.0, 14.0, 500, 50.0, 50.0 );
kalman_up_smoothedresiduals = ROOT.TH2F( "kalman_up_smoothedresiduals", "Distribution of residuals for each plane", 14, 0.0, 14.0, 500, 50.0, 50.0 );
kalman_down_pull = ROOT.TH2F( "kalman_down_pull", "Distribution of residuals for each plane", 14, 0.0, 14.0, 500, 50.0, 50.0 );
kalman_down_residuals = ROOT.TH2F( "kalman_down_residuals", "Distribution of residuals for each plane", 14, 0.0, 14.0, 500, 50.0, 50.0 );
kalman_down_smoothedresiduals = ROOT.TH2F( "kalman_down_smoothedresiduals", "Distribution of residuals for each plane", 14, 0.0, 14.0, 500, 50.0, 50.0 );


####################################################################################################


def main( maus_root_files ) :

  print "Locating Virtual Planes"
  print "Please Wait..."

  virtual_plane_dict = em_ana.common.get_virtual_plane_locations( maus_root_files[0], virtual_plane_events )

  trackers_list = []
  for i in [0,1] :
    if virtual_plane_dict.has_virtual( i, 1, 0 ) :
      trackers_list.append( i )

  print "Found Trackers:", trackers_list

  stations_list = [ 1, 2, 3, 4, 5 ]
  planes_list = [ 0, 1, 2 ]
  data_sources = [ 'mc', 'scifi' ]

#  reference_planes = [ virtual_plane_dict[ i ][ recon_station - 1 ][ recon_plane ][0] for i in range( len( virtual_plane_dict ) ) ]

  print
  gc.collect()


####################################################################################################


  print "Virtual Planes For Kalman:"
  analysis_objects = {}
  for i in trackers_list :
    for j in stations_list :
      for k in planes_list :
        if not virtual_plane_dict.has_virtual( i, j, k ) : continue
        plane_num, plane_tolerance = virtual_plane_dict.get_virtual( i, j, k )
        print "Num =", plane_num, " Tol =", plane_tolerance
        tracker_id = str(i)+"."+str(j)+"."+str(k)
        new_obj = em_ana.recon_container.recon_container(ref="kalman")
#        new_obj.init( virtual_plane_number=plane_num, measurement_plane=tracker_id, plot_phase_space=False )
        new_obj.init( virtual_plane_number=plane_num, measurement_plane=tracker_id )
#        new_obj.init( measurement_plane=tracker_id, plot_residuals=False )
        analysis_objects[ plane_num ] = new_obj
        
  print
  print "Virtual Planes For Pattern Recognition::"
  patrec_analysis_objects = {}
  for i in trackers_list :
    if not virtual_plane_dict.has_virtual( i, 1, 0 ) : continue
    plane_num, plane_tolerance = virtual_plane_dict.get_virtual( i, 1, 0 )
    print "Num =", plane_num, " Tol =", plane_tolerance
    tracker_id = str(i)
    new_obj = em_ana.recon_container.recon_container(ref="patrec")
#        new_obj.init( virtual_plane_number=plane_num, measurement_plane=tracker_id, plot_phase_space=False )
    new_obj.init( virtual_plane_number=plane_num, measurement_plane=tracker_id )
    patrec_analysis_objects[ plane_num ] = new_obj

#  print analysis_objects
#  print

  if print_virtual_planes :
    print virtual_plane_dict
 
####################################################################################################


  file_reader = em_ana.extraction.maus_reader( maus_root_files )

  number_files = file_reader.get_number_files()

  print
  print "Loading", number_files, "File(s)"
  print

  scifi_extractor = em_ana.extraction.scifi_tracks()
#  scifi_extractor.add_cut( 'pvalue', cut_p_value, operator.lt )
  scifi_extractor.set_allow_straight_tracks( not cut_straight_tracks )
#  scifi_extractor.set_min_num_trackpoints( 15 ) # Not yet implemented

  patrec_extractor = em_ana.extraction.scifi_patrec()
  patrec_extractor.set_allow_straight_tracks( not cut_straight_tracks )
  patrec_extractor.set_upstream_z( 1150.5 )
  patrec_extractor.set_downstream_z( 49.5 )

  virtual_extractor = em_ana.extraction.virtual_hits()
#  virtual_extractor.add_cut( 'pid', cut_pid, operator.ne )

  while file_reader.next_event() :

    sys.stdout.write( ' Spill ' + str( file_reader.get_current_spill_number() ) + ' of ' + str( file_reader.get_current_number_spills() ) + ' in File ' + str( file_reader.get_current_filenumber() ) + ' of ' + str( file_reader.get_number_files() ) + '   \r' )
    sys.stdout.flush()


    scifi_event = file_reader.get_event( 'scifi' )
    mc_event = file_reader.get_event( 'mc' )

    fill_performance_plots( scifi_event )
    
    virtual_extractor.load_tracks( mc_event )

    if analyse_kalman :
      scifi_extractor.load_tracks( scifi_event )
      kalman_paired_trackpoints = em_ana.common.make_virt_recon_pairs( scifi_extractor.get_raw_trackpoints(), virtual_extractor.get_raw_trackpoints(), check_length=False, max_z_separation=z_pairing_tolerance, max_separation=r_pairing_tolerance )

#      print "Found", len( virtual_extractor.get_raw_trackpoints() ), "Virtual Hits                                         "
#      print "Found", len( scifi_extractor.get_raw_trackpoints() ), "Trackpoints, Matched", len( kalman_paired_trackpoints ), "                           "

      for mc_hit, recon_hit in kalman_paired_trackpoints :
        plane_num = mc_hit.get_reference()
        if plane_num in analysis_objects :
          analysis_objects[plane_num].add_hit( recon_hit=recon_hit, mc_hit=mc_hit )
          if recon_hit.get_reference() != analysis_objects[plane_num].get_measurement_plane() :
            print recon_hit.get_reference(), analysis_objects[plane_num].get_measurement_plane(), "                           "
        else :
          print "Plane Number Not Found: ", plane_num, "                           "


    if analyse_patrec :
      patrec_extractor.load_tracks( scifi_event )
      patrec_paired_trackpoints = em_ana.common.make_virt_recon_pairs( patrec_extractor.get_raw_trackpoints(), virtual_extractor.get_raw_trackpoints(), check_length=False, max_z_separation=z_pairing_tolerance, max_separation=r_pairing_tolerance )

      for mc_hit, recon_hit in patrec_paired_trackpoints :
        plane_num = mc_hit.get_reference()
        if plane_num in patrec_analysis_objects :
          patrec_analysis_objects[plane_num].add_hit( recon_hit=recon_hit, mc_hit=mc_hit )
#        if recon_hit.get_reference() != patrec_analysis_objects[plane_num].get_measurement_plane() :
#          print recon_hit.get_reference(), patrec_analysis_objects[plane_num].get_measurement_plane(), "                           "
        else :
          print "Plane Number Not Found: ", plane_num, "                           "



  print "All Events Loaded                                                           "
  print
  print "Saving Plots"
  print

  if save_data_all_planes :
    for key, value in analysis_objects.iteritems() :
      filename = "tracker_analysis_kalman_"+str( key )+"_"
      value.save_histograms( filename+"histograms.root" )
      value.save_phase_spaces( filename+"phase_spaces.root" )
      value.save_residuals( filename+"residuals.root" )

    for key, value in patrec_analysis_objects.iteritems() :
      filename = "tracker_analysis_patrec_"+str( key )+"_"
      value.save_histograms( filename+"histograms.root" )
      value.save_phase_spaces( filename+"phase_spaces.root" )
      value.save_residuals( filename+"residuals.root" )


  save_performance_plots( "tracker_analysis_kalman_performance.root", analysis_objects )
  save_performance_plots( "tracker_analysis_patrec_performance.root", patrec_analysis_objects )





####################################################################################################

def find_virtual_planes( filename ) :
  virtual_plane_dict = []
  for i in range( 2 ) :
    stations = []
    for j in range( 5 ) :
      planes = []
      for k in range( 3 ) :
        planes.append( ( -1, associate_tolerance ) )
      stations.append( planes )
    virtual_plane_dict.append( stations )

  reader = em_ana.extraction.maus_reader( filename )

  attempt_count = 0
  trackers_list = []
  while attempt_count < associate_max_attempts and reader.next_event() :

    scifi_event = reader.get_event( 'scifi' )
    mc_event = reader.get_event( 'mc' )

    for track in scifi_event.scifitracks() :
      if not track.tracker() in trackers_list :
        trackers_list.append( track.tracker() )

    if em_ana.common.associate_virtuals( virtual_plane_dict, scifi_event.scifitracks(), mc_event.GetVirtualHits(), trackers=trackers_list ) is True :
      break
    attempt_count += 1

  else :
    if attempt_count >= associate_max_attempts :
      raise ValueError( "Could not locate all virtual planes." )
    else:
      raise ValueError( 'Could not assign enough virtual planes using the data' )

  return virtual_plane_dict


####################################################################################################

def fill_performance_plots( scifi_event ) :

  number_spacepoints.Fill( scifi_event.spacepoints().size() )
  number_clusters.Fill( scifi_event.clusters().size() )
  
  straights = scifi_event.straightprtracks()
  helicals = scifi_event.helicalprtracks()

  for S in straights :
    if S.get_tracker() == 0 :
      patrec_up_charge.Fill( 0.0 )
      patrec_up_numberpoints.Fill( S.get_num_points() )
      patrec_up_chisquaredndf.Fill( math.sqrt( S.get_y_chisq()*S.get_y_chisq() + S.get_x_chisq() * S.get_x_chisq() ) )
      patrec_up_tracktype.Fill( 0.0 )
    elif S.get_tracker() == 1 :
      patrec_down_charge.Fill( 0.0 )
      patrec_down_numberpoints.Fill( S.get_num_points() )
      patrec_down_chisquaredndf.Fill( math.sqrt( S.get_y_chisq()*S.get_y_chisq() + S.get_x_chisq() * S.get_x_chisq() ) )
      patrec_down_tracktype.Fill( 0.0 )
    else :
      print "Unknown Tracker:",S.get_tracker(),"- Straight PR"

  for H in helicals :
    if H.get_tracker() == 0 :
      patrec_up_charge.Fill( H.get_charge() )
      patrec_up_numberpoints.Fill( H.get_num_points() )
      patrec_up_chisquaredndf.Fill( H.get_chisq_dof() )
#      patrec_up_chisquaredndf.Fill( math.sqrt( H.get_line_sz_chisq()**2 + H.get_circle_chisq()**2 ) )
      patrec_up_tracktype.Fill( 1.0 )
    elif H.get_tracker() == 1 :
      patrec_down_charge.Fill( H.get_charge() )
      patrec_down_numberpoints.Fill( H.get_num_points() )
      patrec_down_chisquaredndf.Fill( H.get_chisq_dof() )
#      patrec_down_chisquaredndf.Fill( math.sqrt( H.get_line_sz_chisq()**2 + H.get_circle_chisq()**2 ) )
      patrec_down_tracktype.Fill( 1.0 )
    else :
      print "Unknown Tracker:",S.get_tracker(),"- Helical PR"


  tracks = scifi_event.scifitracks()

  for track in tracks :
    trackpoints = track.scifitrackpoints()

    if track.tracker() == 0 :

      for tp in trackpoints :
        id = ( tp.station() - 1 ) * 3 + tp.plane()
        kalman_up_plane.Fill( id )
        kalman_up_pull.Fill( id, tp.pull() )
        kalman_up_residuals.Fill( id, tp.residual() )
        kalman_up_smoothedresiduals.Fill( id, tp.smoothed_residual() )

      kalman_up_chisquaredndf.Fill( track.chi2() / float(track.ndf()) )
      kalman_up_pvalue.Fill( track.P_value() )

    elif track.tracker() == 1 :

      for tp in trackpoints :
        id = ( tp.station() - 1 ) * 3 + tp.plane()
        kalman_down_plane.Fill( id )
        kalman_down_pull.Fill( id, tp.pull() )
        kalman_down_residuals.Fill( id, tp.residual() )
        kalman_down_smoothedresiduals.Fill( id, tp.smoothed_residual() )

      kalman_down_chisquaredndf.Fill( track.chi2() / float(track.ndf()) )
      kalman_down_pvalue.Fill( track.P_value() )

    else :
      print "Uknown Tracker:",track.tracker(),"- Kalman"


def save_performance_plots( filename, analysis_objects ) :

  x_residuals_mean = array('d')
  y_residuals_mean = array('d')
  px_residuals_mean = array('d')
  py_residuals_mean = array('d')
  pz_residuals_mean = array('d')
  x_residuals_rms = array('d')
  y_residuals_rms = array('d')
  px_residuals_rms = array('d')
  py_residuals_rms = array('d')
  pz_residuals_rms = array('d')
  points = array('d')
  zeros = array('d')

  for i, (key, val) in enumerate( analysis_objects.iteritems() ) :
    points.append(i)
    zeros.append(0)

    x_mean, x_std = em_ana.common.fit_gaussian( val.get_residual_plot( 'x' ) )
    y_mean, y_std = em_ana.common.fit_gaussian( val.get_residual_plot( 'y' ) )
    px_mean, px_std = em_ana.common.fit_gaussian( val.get_residual_plot( 'px' ) )
    py_mean, py_std = em_ana.common.fit_gaussian( val.get_residual_plot( 'py' ) )
    pz_mean, pz_std = em_ana.common.fit_gaussian( val.get_residual_plot( 'pz' ) )

    x_residuals_rms.append( x_std )
    y_residuals_rms.append( y_std )
    px_residuals_rms.append( px_std )
    py_residuals_rms.append( py_std )
    pz_residuals_rms.append( pz_std )

    x_residuals_mean.append( x_mean )
    y_residuals_mean.append( y_mean )
    px_residuals_mean.append( px_mean )
    py_residuals_mean.append( py_mean )
    pz_residuals_mean.append( pz_mean )


#  x_residual_rms_graph = ROOT.TGraph( len( points ), points, x_residuals_rms )
#  y_residual_rms_graph = ROOT.TGraph( len( points ), points, y_residuals_rms )
#  px_residual_rms_graph = ROOT.TGraph( len( points ), points, px_residuals_rms )
#  py_residual_rms_graph = ROOT.TGraph( len( points ), points, py_residuals_rms )
#  pz_residual_rms_graph = ROOT.TGraph( len( points ), points, pz_residuals_rms )

  x_residual_mean_graph = ROOT.TGraphErrors( len( points ), points, x_residuals_mean, zeros, x_residuals_rms )
  y_residual_mean_graph = ROOT.TGraphErrors( len( points ), points, y_residuals_mean, zeros, y_residuals_rms )
  px_residual_mean_graph = ROOT.TGraphErrors( len( points ), points, px_residuals_mean, zeros, px_residuals_rms )
  py_residual_mean_graph = ROOT.TGraphErrors( len( points ), points, py_residuals_mean, zeros, py_residuals_rms )
  pz_residual_mean_graph = ROOT.TGraphErrors( len( points ), points, pz_residuals_mean, zeros, pz_residuals_rms )


  outfile = ROOT.TFile( filename, "RECREATE" )

  number_spacepoints.Write()
  number_clusters.Write()
  patrec_up_charge.Write()
  patrec_up_numberpoints.Write()
  patrec_up_chisquaredndf.Write()
  patrec_up_tracktype.Write()
  patrec_down_charge.Write()
  patrec_down_numberpoints.Write()
  patrec_down_chisquaredndf.Write()
  patrec_down_tracktype.Write()
  kalman_up_plane.Write()
  kalman_up_chisquaredndf.Write()
  kalman_up_pvalue.Write()
  kalman_down_plane.Write()
  kalman_down_chisquaredndf.Write()
  kalman_down_pvalue.Write()
  kalman_up_pull.Write()
  kalman_up_residuals.Write()
  kalman_up_smoothedresiduals.Write()
  kalman_down_pull.Write()
  kalman_down_residuals.Write()
  kalman_down_smoothedresiduals.Write()

  x_residual_mean_graph.Write( "x_resolution" )
  y_residual_mean_graph.Write( "y_resolution" )
  px_residual_mean_graph.Write( "px_resolution" )
  py_residual_mean_graph.Write( "py_resolution" )
  pz_residual_mean_graph.Write( "pz_resolution" )
            
#  x_residual_rms_graph.Write( "x_residuals_rms" )
#  y_residual_rms_graph.Write( "y_residuals_rms" )
#  px_residual_rms_graph.Write( "px_residuals_rms" )
#  py_residual_rms_graph.Write( "py_residuals_rms" )
#  pz_residual_rms_graph.Write( "pz_residuals_rms" )
  
  outfile.Close()


####################################################################################################


try :

  if __name__ == "__main__" :

    files = sys.argv[1:]

    main( files )

    print
    print "Complete!"
    print

except Exception as e :
  print "An error occured"
  print
  print traceback.format_exc()
  print


