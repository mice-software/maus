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


class stats() :
  """
    Records the means and standard deviations of values such that they
    don't all need to be stored in memory
  """
  def __init__( self ) :
    """
      Zero the counters and summations.
    """
    self.__counter = 0
    self.__sum = 0.0
    self.__sum_squares = 0.0


  def clear( self ) :
    """
      Reset all counters and summations
    """
    self.__counter = 0
    self.__sum = 0.0
    self.__sum_squares = 0.0


  def record( self, value ) :
    """
      Add a value to the list
    """
    self.__counter += 1
    self.__sum += value
    self.__sum_squares += value**2


  def get_mean( self ) :
    """
      Return the mean value
    """
    return self.__sum / float( self.__counter )


  def get_standard_deviation( self, mean = None ) :
    """
      Return the standard deviation about a mean.
      If the mean is None, us the mean of the data.
      """
    if mean is None :
      mean = self.get_mean()

    return math.sqrt( self.__sum_squares - mean**2 )

  
  def get_standard_error( self, mean = None ) :
    """
      Return the standard error given by:

      std_deviation / sqrt( num_entries )
    """
    if mean is None :
      mean = self.get_mean()

    return ( math.sqrt( self.__sum_squares - mean**2 ) ) / float( self.__counter )


  def get_sum( self ) :
    """
      Return the sum of values
    """
    return self.__sum


  def get_sum_squares( self ) :
    """
      Return the sum of the squares of the values
    """
    return self.__sum_squares


