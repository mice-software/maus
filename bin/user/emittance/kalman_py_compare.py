
import sys
import os
import math
import array

import ROOT
import MAUS
import numpy

import libMausCpp

from kalman_py import kalman
from kalman_py import propagators
from kalman_py import measurements

import emittance_analysis as em_ana


DIM = 5 # Helical Tracks
ROTATE = True # Rotate for tracker
CHANNEL_RES = 1.0 / 12.0 # Channel Resolution
FIBER_PITCH = 3.5*0.427

residual_x     = ROOT.TH1F( "resiual_x", "Residuals in x", 200, -200.0, 200.0 )
residual_px    = ROOT.TH1F( "resiual_px", "Residuals in px", 200, -200.0, 200.0 )
residual_y     = ROOT.TH1F( "resiual_y", "Residuals in y", 200, -200.0, 200.0 )
residual_py    = ROOT.TH1F( "resiual_py", "Residuals in py", 200, -200.0, 200.0 )
residual_pz    = ROOT.TH1F( "resiual_pz", "Residuals in pz", 200, -1.0, 1.0 )

def main( filenames ) :
  
  file_reader = em_ana.extraction.maus_reader( filenames )

  propagator = propagators.maus_helix( Bz=-0.004 )
  measurement = measurements.measure_helix_fibers( 1.0/FIBER_PITCH )
  track_fit = kalman.fitter( propagator=propagator, measurement=measurement )

  while file_reader.next_event() :
#    sys.stdout.write( ' Spill ' + str( file_reader.get_current_spill_number() ) + ' of ' + str( file_reader.get_current_number_spills() ) + ' in File ' + str( file_reader.get_current_filenumber() ) + ' of ' + str( file_reader.get_number_files() ) + '   \r' )
#    sys.stdout.flush()

    scifi_event = file_reader.get_event( 'scifi' )

    tracks = scifi_event.scifitracks()

    for i in range( len( tracks ) ) :
      track = tracks[i]
      if track.GetAlgorithmUsed() != 1 : # Skip the straight tracks
        print "HERE!"
        continue

      print
      print "############################################################"
      print

      data_track = kalman.track( 1 ) # Data track

      trackpoints = track.scifitrackpoints()
      if len( trackpoints ) == 0 : 
        print
        print "No track points!"
        continue

########## EXTRACT DATA
      for j in range( len( trackpoints ) ) :
        trackpoint = trackpoints[j]
        cluster = trackpoint.get_cluster_pointer() # WHAT IF NULL!!??
        position = trackpoint.pos().z()

        print "MAUS =", position, ":", trackpoint.pos().x(), trackpoint.mom().x(), trackpoint.pos().y(), trackpoint.mom().y(), 1.0/trackpoint.mom().z()

        if not cluster :
          data_track.append( kalman.state( position=position, dimension=1 ) )
          continue

        vec = numpy.zeros( (1, 1) )
        cov = numpy.zeros( (1, 1) )

        alpha = cluster.get_alpha()
        print "Data =", alpha
        vec[0] = alpha
        cov[0,0] = CHANNEL_RES
        print "Val =", vec[0]

#        print cluster.get_direction().x(),
#        print cluster.get_direction().y(),
#        print cluster.get_direction().z() 
        data_point = kalman.state( position=position, vector=vec, covariance=cov )
        data_track.append( data_point )

########## INIT TRACK

      seed_vec, seed_cov = get_seed( track )
      seed = kalman.state( position=data_track[0].get_position(), vector=seed_vec, covariance=seed_cov )

      print "SEED =", seed

      track_fit.set_data( data_track )
      track_fit.set_seed( seed )

      track_fit.filter()
      track_fit.smooth()

      print
      print "DATA       ="
      print data_track
      print 
      print "PREDICTED  ="
      print track_fit.get_predicted()
      print
      print "FILTERED   ="
      print track_fit.get_filtered()
      print
      print "SMOOTHEDED ="
      print track_fit.get_smoothed()
      print
      
      lastpoint = len( trackpoints ) - 1
      smoo = track_fit.get_smoothed()

      residual_x.Fill( smoo[lastpoint].get_vector()[0] + trackpoints[lastpoint].pos()[0] )
      residual_px.Fill( smoo[lastpoint].get_vector()[1] + trackpoints[lastpoint].pos()[1] )
      residual_y.Fill( smoo[lastpoint].get_vector()[2] - trackpoints[lastpoint].mom()[0] )
      residual_py.Fill( smoo[lastpoint].get_vector()[3] - trackpoints[lastpoint].mom()[1] )
      residual_pz.Fill( 1.0/smoo[lastpoint].get_vector()[4] - trackpoints[lastpoint].mom()[2] )
      

  print "Analysed All Events.                             "

  canvas = ROOT.TCanvas( "output_canv" )
  canvas.Divide( 3, 2 )
  canvas.cd( 1 )
  residual_x.Draw()
  canvas.cd( 2 )
  residual_px.Draw()
  canvas.cd( 3 )
  residual_y.Draw()
  canvas.cd( 4 )
  residual_py.Draw()
  canvas.cd( 5 )
  residual_pz.Draw()

  raw_input( "Continue..." ) 

def get_seed( track ) :
  vec_state = track.GetSeedState()
  vec_cov = track.GetSeedCovariance()
  length = track.GetSeedState().size()

  state = numpy.zeros( (length) )
  cov = numpy.zeros( (length, length) )

  for i in range( length ) :
    state[i] = vec_state[i]
    for j in range( length ) :
      cov[i,j] = vec_cov[ (i*length) + j ]

  if ROTATE :
    state[0] = -state[0]
    state[1] = -state[1]

  return state, cov

#def get_seed( helix ) :
#  c  = 2.99792458
#  charge = helix.get_charge()
#  Bz = 0.40
#  if helix.get_tracker() == 1 :
#    Bz = -Bz
#  else :
#    Bz = Bz
#
#  length = 1100.0
#
#  r  = helix.get_R()
#  pt = math.fabs( charge * c * Bz * r )
#  dsdz = math.fabs( helix.get_dsdz() )
#  x0 = helix.get_circle_x0()
#  y0 = helix.get_circle_y0()
#  s = helix.get_line_sz_c() - charge * length * dsdz
#  phi_0 = s / r
#  phi = phi_0 + 3.14159265/2.0
#
#  patrec_momentum = numpy.zeros( (3) )
#  patrec_momentum[0] = pt*math.cos(phi)
#  patrec_momentum[1] = pt*math.sin(phi)
#  patrec_momentum[2] = math.fabs(pt/dsdz)
##  double P = patrec_momentum.mag()
##  double patrec_bias; // Account for two planes of energy loss
##  if ( _tracker == 0 ) {
##    patrec_bias = (P + 1.4) / P
##  } else {
##    patrec_bias = (P - 1.4) / P
##  }
##  patrec_momentum = patrec_bias * patrec_momentum
#
#  x = x0 + r*math.cos(phi_0)
#  px = patrec_momentum[0]
#  y = y0 + r*math.sin(phi_0)
#  py = patrec_momentum[1]
#  kappa = charge / patrec_momentum[2]
#
#  a = numpy.zeros( (5, 1) )
#  a[0,0] = -x
#  a[1,0] = -px
#  a[2,0] = y
#  a[3,0] = py
#  a[4,0] = kappa
#
#  cov = numpy.zeros( (5, 5) )
#  for i in range( 5 ) :
#    cov[i,i] = 1000.0
#
#  return a, cov




if __name__ == "__main__" :
  print
  print "Comparing MAUS Kalman to the Kalman_py"
  print
  filenames = sys.argv[1:]
  print "Found", len( filenames ), "Files to Analyse"
  print
  main( filenames )
  print
  print "Complete."
  print


