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

import copy
import gc
import math

import track_point
import extraction


####################################################################################################
## CLASSES
####################################################################################################

class virtual_plane_dictionary() :

  def __init__( self, tolerance ) :
    self.__the_dict = None
    self.__trackers = [0,1]
    self.__stations = [1,2,3,4,5]
    self.__planes = [0,1,2]

    self.__default_tolerance = tolerance

    self.clear()


  def _check_location( self, tracker, station, plane ) :
    if not tracker in self.__trackers :
      raise ValueError( "No Such Tracker: "+str(tracker) )
    if not station in self.__stations :
      raise ValueError( "No Such Station: "+str(station) )
    if not plane in self.__planes :
      raise ValueError( "No Such Plane: "+str(plane) )
    return

  def __repr__( self ) :
    return_string = ""
    for tr in self.__trackers :
      for st in self.__stations :
        for pl in self.__planes :
          plane, tol = self.__the_dict[tr][st-1][pl]
          return_string += "{ "+str(tr)+", "+str(st)+", "+str(pl)+" } : "+str(plane)+"  |  "+str(tol)+"\n"
    return return_string

 
  def clear( self ) :
    """
    """
    self.__the_dict = []
    gc.collect()

    for i in range( 2 ) :
      stations = []
      for j in range( 5 ) :
        planes = []
        for k in range( 3 ) :
          planes.append( ( -1, self.__default_tolerance ) )
        stations.append( planes )
      self.__the_dict.append( stations )


  def set_virtual( self, tracker, station, plane, virt_id, tolerance ) :
    """
    """
    self._check_location( tracker, station, plane )
    self.__the_dict[tracker][station-1][plane] = ( virt_id, tolerance )
    return


  def has_virtual( self, tracker, station, plane ) :
    """
    """
    self._check_location( tracker, station, plane )
    if self.__the_dict[tracker][station-1][plane][0] != -1 :
      return True
    else :
      return False


  def get_virtual( self, tracker, station, plane ) :
    """
    """
    self._check_location( tracker, station, plane )
    if self.__the_dict[tracker][station-1][plane][0] != -1 :
      return self.__the_dict[tracker][station-1][plane]
    else :
      return None, None


  def get_plane( self, virt_id ) :
    """
    """
    for tr in self.__trackers :
      for st in self.__stations :
        for pl in self.__planes :
          if self.__the_dict[tr][st-1][pl][0] == virt_id :
            return ( tr, st, pl )
    return None


  def get_dict( self ) :
    """
    """
    return copy.deepcopy( self.__the_dict )


  def has_all_planes( self ) :
    """
    """
    for tr in self.__trackers :
      for st in self.__stations :
        for pl in self.__planes :
          if self.__the_dict[tr][st-1][pl][0] == -1 :
            return False
    return True



####################################################################################################
## FUNCTIONS
####################################################################################################


def fit_gaussian( hist, fit_min = 0, fit_max = 0 ) :
  """
    Fits a Gaussian to the histogram and returns a tuple of (mean, stddev) for the fitted gaussian.
  """
  fit_result = hist.Fit( "gaus", "Q0S", "", fit_min, fit_max )
  func = hist.GetFunction( "gaus" )
  mean = func.GetParameter(1)
  stddev = func.GetParameter(2)
  return mean, stddev


def associate_virtuals( virtual_plane_dict, scifitracks, virtualhits, trackers=None, stations=None, planes=None ) :
  """
    Helper function for locating the virtual planes using scifi tracks and
    virtual hits.

    Returns True if successful, and False if they could not ALL be found.

    virtual_plane_dict is actually a set of nested lists structured similarly
    to the tracker construction:
      virtual_plane_dict[ TRACKER_NUM ][ STATION_ID - 1 ][ PLANE_NUM ]

    Note that the station ID runs from 1 to 5, and so requires 1 to be 
    subtracted.

    scifitracks is an must be a list of MAUS SciFiTrack objects

    virtualhits must be a list of MAUS VirtualHit objects

    To specifically check for individual trackers, stations or planes, change
    the respective parameter to a range of ID numbers  of the components to
    concentrate on.
  """
  tracker_range = []
  station_range = []
  plane_range = []
  if trackers is None :
    tracker_range = [ 0, 1 ]
  else :
    for t in trackers :
      if t > 1 or t < 0 :
        raise ValueError( 'Tracker ' + str( t ) + ' not a valid Tracker ID.' )
    else :
      tracker_range = trackers
  if stations is None :
    station_range = [ 0, 1, 2, 3, 4 ]
  else :
    for s in stations :
      if s < 1 or s > 4 :
        raise ValueError( 'Station ' + str( s ) + ' not a valid Station ID.' )
    else :
      station_range = [ stations[i] - 1 for i in range( len ( stations ) ) ]
  if planes is None :
    plane_range = [ 0, 1, 2 ]
  else :
    for p in planes :
      if p < 0 or p > 2 :
        raise ValueError( 'Plane ' + str( p ) + ' not a valid Plane ID.' )
    else :
      plane_range = planes


  for i in range( scifitracks.size() ) :
    trackpoints = scifitracks[i].scifitrackpoints()
    for point in trackpoints :
      z_pos = point.pos()[2]
      tracker = point.tracker()
      station = point.station()
      plane = point.plane()

      for hit in virtualhits :
        test = hit.GetPosition()[2]
        diff = abs( test - z_pos )
        num, tol = virtual_plane_dict.get_virtual( tracker, station, plane ) 
        if num is None or tol > diff :
          virtual_plane_dict.set_virtual( tracker, station, plane, hit.GetStationId(), diff )

  return virtual_plane_dict.has_all_planes()



def get_virtual_plane_locations( filename, associate_tolerance = 1.0e+32, max_attempts = 1000 ) :
  """
    Return a 3-tier nested list of plane IDs allowing them to be assigned to the tracker planes,
    embedded in an easy to use class.
  """
  plane_dict = virtual_plane_dictionary( tolerance = associate_tolerance )

# Initialise the file reader
  reader = extraction.maus_reader( filename )

  attempt_count = 0
  while attempt_count < max_attempts and reader.next_event() :

    scifi_event = reader.get_event( 'scifi' )
    mc_event = reader.get_event( 'mc' )
# See if we can associate ALL tracker planes with a virtual plane
    if associate_virtuals( plane_dict, scifi_event.scifitracks(), mc_event.GetVirtualHits() ) is True :
      break
    attempt_count += 1

  else :
    if attempt_count >= max_attempts :
      ValueError( "ERROR Could not successfully associate all virtual planes after", max_attempts, "attempts" )
    else : 
      ValueError( "ERROR Ran out of events to locate virtual planes." )
  gc.collect()
  return plane_dict


def make_virt_recon_pairs( virtual_points, recon_points, check_pid = False, check_length = True, average_mc = False, max_separation = 1.0e+32, max_z_separation = 1.0 ) :
  """
    Matches the pairs of virtual and reconstucted trackpoints and returns a
    list of tuples of the result.

    Options:
      check_pid : Checks to make sure that both trackpoints have the same assigned PID
      check_length : Ensure that there are enough virtual points to match to all the reconstructed points.
      max_separation : Only match if the points are within some radial distance of each other
      max_z_separation : Only match if the points are within some longitudinal distance of each other.
  """
  results = []

  if check_length and len( recon_points ) > len( virtual_points ) :
    raise ValueError( 'Not enough Monte Carlo points to match to Reconstructed points.' )

  if average_mc :
    tmp_pos = [ 0.0, 0.0, 0.0 ]
    tmp_mom = [ 0.0, 0.0, 0.0 ]
    tmp_time = 0.0
    count = 0
    for recon in recon_points :
      current_virt = None
      for virt in virtual_points :
        dx = recon.get_x() - virt.get_x()
        dy = recon.get_y() - virt.get_y()
        dz = math.fabs( recon.get_z() - virt.get_z() )
        sep = math.sqrt( dx**2 + dy**2 )
        if sep < max_separation and dz < max_z_separation :
          if check_pid and virt.get_pid() != recon.get_pid() :
            continue 
          else :
            results.append( (recon, virt ) )
            break
#            count += 1
#            tmp_pos[0] += virt.get_x()
#            tmp_pos[1] += virt.get_y()
#            tmp_pos[2] += virt.get_z()
#            tmp_mom[0] += virt.get_px()
#            tmp_mom[1] += virt.get_py()
#            tmp_mom[2] += virt.get_pz()
#      if count > 0 :
#        results.append( ( recon, track_point( x = (tmp_pos[0] / count),
#                y = (tmp_pos[1] / count),
#                z = (tmp_pos[2] / count),
#                px = (tmp_mom[0] / count),
#                py = (tmp_mom[1] / count),
#                pz = (tmp_mom[2] / count),
#                time = (tmp_time / count),
#                mass = recon.get_mass(),
#                pid = recon.get_pid() ) ) )
  else:
    for recon in recon_points :
      current_virt = None
      current_sep = max_separation
      for virt in virtual_points :
        dx = recon.get_x() - virt.get_x()
        dy = recon.get_y() - virt.get_y()
        dz = math.fabs( recon.get_z() - virt.get_z() )
        sep = math.sqrt( dx**2 + dy**2 )
#        if ( dz < max_z_separation ) and ( sep < current_sep ) :
        if ( dz < max_z_separation ) :
#          print "\nMatching:"#, sep, "<", current_sep, "  |  dz =", dz
#          print "Recon:", recon.get_x(), recon.get_y(), recon.get_z()
#          print "Virtual:", virt.get_x(), virt.get_y(), virt.get_z()
          if ( sep < current_sep ) :
            if check_pid and virt.get_pid() != recon.get_pid() :
              continue 
            else :
              current_virt = virt
              current_sep = sep
      if current_virt is not None :
#        print "Found one!"
        results.append( ( copy.copy( recon ), copy.copy( current_virt ) ) )

  return results


