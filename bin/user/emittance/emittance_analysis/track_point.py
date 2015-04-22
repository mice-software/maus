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

import math
import xboa
import xboa.Bunch
import xboa.Hit
import xboa.Common


class track_point() :
  """
    A simple, unified class that holds the essential information, when copied
    from MAUS data types, during various analyses.

    Some syntax was borrowed from XBoa for simplicity and because I like the
    interface!
  """
  def __init__( self, x = 0.0, y = 0.0, z = 0.0, px = 0.0, py = 0.0, pz = 0.0, time = 0.0, mass = 105.6583715, p_value = 1.0, pid = 13, reference = None, scifi_track_point = None, virtual_track_point = None ) :
    """
      Initialise the object. this can be done in three ways:
      1. Specify all components by hand
      2. Build from a sci-fr trackpoint
      3. Build from a virtual trackpoint
    """
    if scifi_track_point is None and virtual_track_point is None :
      self.__x = x
      self.__y = y
      self.__z = z
      self.__px = px
      self.__py = py
      self.__pz = pz
      self.__time = time
      self.__mass = mass
      self.__p_value = p_value
      self.__pid = pid
      self.__reference = reference

    elif scifi_track_point is not None and virtual_track_point is None :
      self.__x = scifi_track_point.pos()[0]
      self.__y = scifi_track_point.pos()[1]
      self.__z = scifi_track_point.pos()[2]
      self.__px = scifi_track_point.mom()[0]
      self.__py = scifi_track_point.mom()[1]
      self.__pz = scifi_track_point.mom()[2]
      self.__time = time
      self.__mass = mass
      self.__p_value = p_value
      self.__pid = pid
      if reference is None :
        self.__reference = str(scifi_track_point.tracker())+"."+str(scifi_track_point.station())+"."+str(scifi_track_point.plane())
      else : 
        self.__reference = reference

    elif scifi_track_point is None and virtual_track_point is not None :
      self.__x = virtual_track_point.GetPosition()[0]
      self.__y = virtual_track_point.GetPosition()[1]
      self.__z = virtual_track_point.GetPosition()[2]
      self.__px = virtual_track_point.GetMomentum()[0]
      self.__py = virtual_track_point.GetMomentum()[1]
      self.__pz = virtual_track_point.GetMomentum()[2]
      self.__time = virtual_track_point.GetTime()
      self.__mass = mass
      self.__p_value = 1.0
      self.__pid = virtual_track_point.GetParticleId()
      self.__reference = reference
      if reference is None :
        self.__reference = virtual_track_point.GetStationId()
      else :
        self.__reference = reference

    else :
      raise ValueError( 'Please supply precisely one of "virtual_track_point" or "scifi_track_point", or specify all values explicitly.' )


  def __str__( self ) :
    """
      Return a string of the parameters.
      Mostly useful for debuging.
    """
    return '(' + str( self.__x ) + ',' +\
                 str( self.__y ) + ',' +\
                 str( self.__z ) + '):[' +\
                 str( self.__px ) + ',' +\
                 str( self.__py ) + ',' +\
                 str( self.__pz ) + ']:' +\
                 str( self.__reference )


  def __repr__( self ) :
    """
      Return a string of the parameters.
      Mostly useful for debuging.
    """
    return '(' + str( self.__x ) + ',' +\
                 str( self.__y ) + ',' +\
                 str( self.__z ) + '):[' +\
                 str( self.__px ) + ',' +\
                 str( self.__py ) + ',' +\
                 str( self.__pz ) + ']:' +\
                 str( self.__reference )



  def set_x( self, val ) :
    self.__x = val

  def set_y( self, val ) :
    self.__y = val

  def set_z( self, val ) :
    self.__z = val

  def set_px( self, val ) :
    self.__px = val

  def set_py( self, val ) :
    self.__py = val

  def set_pz( self, val ) :
    self.__pz = val

  def set_time( self, val ) :
    self.__time = val

  def set_mass( self, val ) :
    self.__mass = val

  def set_p_value( self, val ) :
    self.__p_value = val

  def set_pid( self, val ) :
    self.__pid = val

  def set_reference( self, string ) :
    self.__reference = string

  def get_x( self ) :
    return self.__x

  def get_y( self ) :
    return self.__y

  def get_z( self ) :
    return self.__z

  def get_px( self ) :
    return self.__px

  def get_py( self ) :
    return self.__py

  def get_pz( self ) :
    return self.__pz

  def get_time( self ) :
    return self.__time

  def get_p( self ) :
    return math.sqrt( self.__px**2 + self.__py**2 + self.__pz**2 )

  def get_r( self ) :
    return math.sqrt( self.__x**2 + self.__y**2 )

  def get_energy( self ) :
    return math.sqrt( self.get_mass()**2 + self.get_p()**2 )

  def get_pt( self ) :
    return math.sqrt( self.__px**2 + self.__py**2 )

  def get_mx( self ) :
    return self.get_px() / self.get_pz()

  def get_my( self ) :
    return self.get_py() / self.get_pz()

  def get_mass( self ) :
    return self.__mass

  def get_p_value( self ) :
    return self.__p_value

  def get_pid( self ) :
    return self.__pid


  def get_reference( self ) :
    return self.__reference



  def get( self, string ) :
    """
      Return the value of the variable described in the string
    """
    if not string in track_point.__track_point_get_keys :
      return None
    else :
      return track_point.__track_point_get_variables[ string ]( self )


  def get_variable_list() :
    return track_point.__track_point_get_keys
  get_variable_list = staticmethod( get_variable_list )



  __track_point_get_keys = []
  __track_point_get_variables = { 'x':get_x, 'y':get_y, 'z':get_z,
                              'px':get_px, 'py':get_py, 'pz':get_pz,
                              'mx':get_mx, 'my':get_my,
                              't':get_time, 'p':get_p, 'r':get_r,
                              'E':get_energy, 'pt':get_pt, 'm':get_mass,
                              'mass':get_mass, 'energy':get_energy,
                              'pvalue':get_p_value, 'pid':get_pid }

  for key, val in __track_point_get_variables.iteritems() :
    __track_point_get_keys.append( key )




####################################################################################################




def make_xboa_hit( hit ) :

  hit = xboa.Hit.Hit.new_from_dict(
      { 'x' : hit.get_x(),
        'y' : hit.get_y(),
        'z' : hit.get_z(),
        'px' : hit.get_px(),
        'py' : hit.get_py(),
        'pz' : hit.get_pz(),
        'pid' : abs( hit.get_pid() ),
        'mass' : xboa.Common.pdg_pid_to_mass[ abs( hit.get_pid() ) ] }, 'energy' )
  return hit


