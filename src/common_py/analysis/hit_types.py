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
  Defines the AnalysisHit class.

  A generic hit class that can be used to simplify the analysis of track data.

  Note that this class is used by the add_hit meth
"""

# pylint: disable = W0311, R0902, R0904, R0913, C0103

import math
import analysis.tools as tools

class AnalysisHit() :
  """
    A simple, unified class that holds the essential information, when copied
    from MAUS data types, during various analyses.

    Some syntax was borrowed from XBoa for simplicity and because I like the
    interface!
  """
  def __init__( self, x = 0.0, y = 0.0, z = 0.0, \
                      px = 0.0, py = 0.0, pz = 0.0, station = 0, \
                      time = 0.0, mass = 105.6583715, p_value = 1.0, pid = 13, \
                      scifi_track_point = None, virtual_track_point = None ) :
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
      self.__station = station
#      self.__reference = reference

    elif scifi_track_point is not None and virtual_track_point is None :
      self.__x = scifi_track_point.pos().x()
      self.__y = scifi_track_point.pos().y()
      self.__z = scifi_track_point.pos().z()
      self.__px = scifi_track_point.mom().x()
      self.__py = scifi_track_point.mom().y()
      self.__pz = scifi_track_point.mom().z()
      self.__time = time
      self.__mass = mass
      self.__p_value = p_value
      self.__pid = pid
      self.__station = tools.calculate_plane_id(scifi_track_point.tracker(), \
          scifi_track_point.station(), scifi_track_point.plane())
#      if reference is None :
#        self.__reference = str(scifi_track_point.tracker())+"."+\
#            str(scifi_track_point.station())+"."+str(scifi_track_point.plane())
#      else :
#        self.__reference = reference
      if math.isnan(self.__x) :
        raise ValueError("NaN Values Received from Scifi Track Point")

    elif scifi_track_point is None and virtual_track_point is not None :
      self.__x = virtual_track_point.GetPosition().x()
      self.__y = virtual_track_point.GetPosition().y()
      self.__z = virtual_track_point.GetPosition().z()
      self.__px = virtual_track_point.GetMomentum().x()
      self.__py = virtual_track_point.GetMomentum().y()
      self.__pz = virtual_track_point.GetMomentum().z()
      self.__time = virtual_track_point.GetTime()
      self.__mass = mass
      self.__p_value = 1.0
      self.__pid = virtual_track_point.GetParticleId()
      self.__station = virtual_track_point.GetStationId()
#      self.__reference = reference
#      if reference is None :
#        self.__reference = virtual_track_point.GetStationId()
#      else :
#        self.__reference = reference

    else :
      print "WTF!"
      raise ValueError( 'Please supply precisely one of "virtual_track_point"'+\
                 ' or "scifi_track_point", or specify all values explicitly.' )


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
                 str( self.__pz ) + ']' +\
                 str( self.__station )
#                 str( self.__reference )


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
                 str( self.__pz ) + ']' +\
                 str( self.__station )
#                 str( self.__reference )



  def set_x( self, val ) :
    """
      Set the x position
    """
    self.__x = val

  def set_y( self, val ) :
    """
      Set the y position
    """
    self.__y = val

  def set_z( self, val ) :
    """
      Set the z position
    """
    self.__z = val

  def set_px( self, val ) :
    """
      Set the x momentum
    """
    self.__px = val

  def set_py( self, val ) :
    """
      Set the y momentum
    """
    self.__py = val

  def set_pz( self, val ) :
    """
      Set the z momentum
    """
    self.__pz = val

  def set_time( self, val ) :
    """
      Set the time
    """
    self.__time = val

  def set_mass( self, val ) :
    """
      Set the mass
    """
    self.__mass = val

  def set_p_value( self, val ) :
    """
      Set the p-value
    """
    self.__p_value = val

  def set_pid( self, val ) :
    """
      Set the particle ID
    """
    self.__pid = val

  def set_station( self, station ) :
    """
      Set the station ID
    """
    self.__station = station

#  def set_reference( self, string ) :
#    self.__reference = string

  def get_x( self ) :
    """
      Get the x position
    """
    return self.__x

  def get_y( self ) :
    """
      Get the y position
    """
    return self.__y

  def get_z( self ) :
    """
      Get the z position
    """
    return self.__z

  def get_px( self ) :
    """
      Get the x momentum
    """
    return self.__px

  def get_py( self ) :
    """
      Get the y momentum
    """
    return self.__py

  def get_pz( self ) :
    """
      Get the z momentum
    """
    return self.__pz

  def get_time( self ) :
    """
      Get the time
    """
    return self.__time

  def get_p( self ) :
    """
      Get the total momentum
    """
    if math.isnan(math.sqrt( self.__px**2 + self.__py**2 + self.__pz**2 )) :
      print "ISNAN!!!", self
    return math.sqrt( self.__px**2 + self.__py**2 + self.__pz**2 )

  def get_r( self ) :
    """
      Get the position radius
    """
    return math.sqrt( self.__x**2 + self.__y**2 )

  def get_phi( self ) :
    """
      Get the Phi angle (Position)
    """
    return math.atan2( self.__y, self.__x )

  def get_theta( self ) :
    """
      Get the Theta angle (momentum)
    """
    return math.atan2( self.__py, self.__px )

  def get_energy( self ) :
    """
      Get the total energy
    """
    return math.sqrt( self.get_mass()**2 + self.get_p()**2 )

  def get_pt( self ) :
    """
      Get the transverse momentum
    """
    return math.sqrt( self.__px**2 + self.__py**2 )

  def get_mx( self ) :
    """
      Get the x gradient
    """
    return self.get_px() / self.get_pz()

  def get_my( self ) :
    """
      Get the y gradient
    """
    return self.get_py() / self.get_pz()

  def get_mass( self ) :
    """
      Get the mass
    """
    return self.__mass

  def get_p_value( self ) :
    """
      Get the p-value
    """
    return self.__p_value

  def get_pid( self ) :
    """
      Get the particle ID
    """
    return self.__pid

  def get_station( self ) :
    """
      Get the particle ID
    """
    return self.__station


#  def get_reference( self ) :
#    return self.__reference



  def get( self, string ) :
    """
      Return the value of the variable described in the string
    """
    if not string in AnalysisHit.__hit_get_keys :
      return None
    else :
      return AnalysisHit.__hit_get_variables[ string ]( self )


  def get_as_vector( self ) :
    """
      Returns the 6D phase-space vector corresponding to the particle.
      [t, E, x, px, y, py, z, pz]
    """
    return [self.__time, self.get_energy(), self.__x, self.__px, self.__y,
            self.__py, self.__z, self.__pz]


  def get_variable_list() :
    """
        Return the list of variables that the user can request
    """
    return AnalysisHit.__hit_get_keys
  get_variable_list = staticmethod( get_variable_list )



  __hit_get_keys = []
  __hit_get_variables = { 'x':get_x, 'y':get_y, 'z':get_z,
                              'px':get_px, 'py':get_py, 'pz':get_pz,
                              'mx':get_mx, 'my':get_my,
                              't':get_time, 'p':get_p, 'r':get_r,
                              'station':get_station,
                              'E':get_energy, 'pt':get_pt, 'm':get_mass,
                              'mass':get_mass, 'energy':get_energy,
                              'pvalue':get_p_value, 'pid':get_pid }

  for key, val in __hit_get_variables.iteritems() :
    __hit_get_keys.append( key )
