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
  Provides an implementation of a covariance matrix calculator.

  Doesn't store individual hits - so no limitations due to memory usage.

  Also includes a correction matrix calculator which analyses the differences
  between an MC and Reconstructed hits to produce the systematic corrections
  to the reconstructed covariance matrix; in additiona to some useful functions.
"""

# pylint: disable = W0311, E1101, W0102, R0902, C0103, W0141

import numpy
import math

from analysis.tools import MUON_MASS
import analysis.hit_types

VARIABLE_LIST = [ 'x', 'px', 'y', 'py', 'z', 'pz', 't', 'E' ]
POSITION_VARIABLES = [ 'x', 'y', 'z', 't' ]
CONJUGATE_PAIRS = { 'x':'px', 'y':'py', 'z':'pz', 't':'E', \
                                        'px':'x', 'py':'y', 'pz':'z', 'E':'t' }

VARIABLE_ENUMERATION = {}
for var_num, var in enumerate(VARIABLE_LIST) :
  VARIABLE_ENUMERATION[var] = var_num


def check_axes( axes, master_list=None ) :
  """
    Checks a list of axes against the 'VARIABLE_LIST' variable.
    Throws an exception if there is an incompatibility.
  """
  if master_list == None :
    master_list = VARIABLE_LIST
  for axis in axes :
    if axis not in master_list :
      raise KeyError( 'Could not find axis "' + str( axis ) + \
                                                '" in the allowed variables.' )


def emittance_from_matrix( matrix, mass = None ) :
  """
    Calculate the emittance from a 2D covariance matrix. Requires the matrix 
    to be square and have dimension 2nx2n for integer n.

    If no mass is provided, the muon mass is used by default.
  """
  if mass is None :
    mass = MUON_MASS
  if ( len( matrix ) % 2.0 ) != 0.0 :
    raise ArithmeticError( 'A Matrix with even numbers of rows and columns is \
                                         Required to calculate an Emittance.' )

  if isinstance( matrix, numpy.matrix ) :
    shape = matrix.shape
    if ( shape[0] != shape[1] ) :
      print len( matrix ), len( matrix[0] ), map( len, matrix )
      raise ArithmeticError( 'A 2D Square Matrix is \
                                         Required to calculate an Emittance.' )

  elif len( matrix ) != len( matrix[0] ) :
    print len( matrix ), len( matrix[0] ), map( len, matrix )
    raise ArithmeticError( 'A 2D Square Matrix is \
                                         Required to calculate an Emittance.' )

  dim = len( matrix )
  return ( numpy.linalg.det( matrix ) ** ( 1.0 / dim ) ) / mass


def get_conjugates( axes ) :
  """
    Returns a list of axis labels, corresponding to the conjuate labels in the 
    supplied list.
  """
  check_axes( axes )
  new_list = []
  for axis in axes :
    new_list.append( CONJUGATE_PAIRS[axis] )

  return new_list


class covariance_matrix() :
  """
    Performs running calculations of a covariance matrix to allow for larger
    datasets to be analysed with a smaller memory footprint. Running sums and 
    totals are stored, to be combined when necessary to form the final
    covariance matrix.

    Uses the whole 6D phase space, however the user may request subsets.
  """

  def __init__( self ) :
    """
      Initialse the object
      Set counters to zero, and values to zero.
    """
    self._numVars = len( VARIABLE_LIST )
    self._mean_vector = numpy.zeros( self._numVars )
    self._product_matrix = numpy.zeros( ( self._numVars, self._numVars ) )
    self._mean_momentum = 0.0
    self._num_particles = 0


  def length( self ) :
    """
      Return the number of particles added to the matrix
    """
    return self._num_particles


  def clear( self ) :
    """ 
      Set our two matrices to zeros.
      As if nothing had happened!
    """
    self._mean_vector = numpy.zeros( self._numVars )
    self._product_matrix = numpy.zeros( ( self._numVars, self._numVars ) )
    self._mean_momentum = 0.0
    self._num_particles = 0

  def add_hit_scifi( self, scifi_hit ) :
    """
      Add a scifi hit and extract the information into the class.
      Accepts both xboa type hits and the local emittance analysis type hits.
    """
    self.add_hit( analysis.hit_types.hit(scifi_track_point=scifi_hit) )


  def add_hit( self, hit ) :
    """
      Add a hit and extract the information into the class.
      Accepts both xboa type hits and the local emittance analysis type hits.
    """
    for row, rowvar in enumerate( VARIABLE_LIST ) :
      for col, colvar in enumerate( VARIABLE_LIST ) :
        self._product_matrix[row][col] += hit.get( rowvar ) *\
                                           hit.get( colvar )

      self._mean_vector[row] += hit.get( rowvar )

    self._mean_momentum += hit.get_p()
    self._num_particles += 1


  def add_dict( self, dictionary ) :
    """
      Add a hit by specifying the individual components in the 6D phase space
      in a dictionary of form { 'x': x, 'y': y ... }

      All variables in the 'VARIABLE_LIST' list must be provided.
    """
    for key in VARIABLE_LIST.keys :
      if key not in dictionary.keys :
        raise KeyError( str( key ) + ' not found in supplied dictionary.' )

    for row, rowvar in enumerate( VARIABLE_LIST ) :
      for col, colvar in enumerate( VARIABLE_LIST ) :
        self._product_matrix[row][col] += dictionary[ rowvar ] *\
                                           dictionary[ colvar ]

      self._mean_vector[row] += dictionary[rowvar]

    self._mean_momentum += math.sqrt(dictionary['px']**2 + \
                                    dictionary['py']**2 + dictionary['pz']**2)
    self._num_particles += 1


  def get_covariance_matrix( self, axes = [ 'x', 'px', 'y', 'py' ] ) :
    """
      Combines the vector and matrix to form the covariance matrix and returns
      it.
    """
    check_axes( axes )
    if self._num_particles == 0 :
      raise ZeroDivisionError( 'No particles found. Cannot divide by zero.' )
    cov_matrix = numpy.empty( ( len( axes ), len( axes ) ) )

    for covrow, rowvar in enumerate( axes ) :
      row = -1
      for num, test in enumerate( VARIABLE_LIST ) :
        if rowvar == test :
          row = num
      for covcol, colvar in enumerate( axes ) :
        col = -1
        for num, test in enumerate( VARIABLE_LIST ) :
          if colvar == test :
            col = num
        cov_matrix[covrow][covcol] = ( self._product_matrix[row][col] - \
          ( self._mean_vector[row] * self._mean_vector[col] ) /\
          self._num_particles ) / self._num_particles

    return cov_matrix


  def get_component( self, axes = [ 'x', 'x' ] ) :
    """
      Returns a single component from the covariance matrix. Length of axes 
      must therefore be two.
    """
    check_axes( axes )
    if len( axes ) != 2 :
      raise ValueError( "Must supply two axis labels to obtain a \
                                                            single component" )

#    row = -1
#    col = -1
#    for num, test in enumerate( VARIABLE_LIST ) :
#      if axes[0] == test :
#        row = num
#        break
#    for num, test in enumerate( VARIABLE_LIST ) :
#      if axes[1] == test :
#        col = num
#        break
    row = VARIABLE_ENUMERATION[axes[0]]
    col = VARIABLE_ENUMERATION[axes[1]]

    return ( self._product_matrix[row][col] - \
           ( self._mean_vector[row] * self._mean_vector[col] ) /\
           self._num_particles ) / self._num_particles


  def get_determinant( self, axes = [ 'x', 'px', 'y', 'py' ] ) :
    """ 
      Returns the determinant of the covariace matrix
    """
    return numpy.linalg.det( self.get_covariance_matrix( axes ) )


  def get_momentum( self ) :
    """
      Returns the averaged value of momentum for the ensemble of particles
    """
    if self._num_particles > 0 :
      return self._mean_momentum / self._num_particles
    else :
      return 0.0

  def get_emittance( self, axes = [ 'x', 'px', 'y', 'py' ], mass = None ) :
    """
      Calculates the emittance of covariance matrix.

      Requires an even number of axes to  be provided, defaults to 4D
      transverse emittance.

      Emittance calculated as:
      ( Det( Cov ) ** ( 1 / len( axes ) ) ) / mass
      """
    check_axes( axes )
    if len( axes ) % 2 != 0 :
      raise FloatingPointError( 'Must supply and even number of axes to \
                                                      calculate an emittance' )
    if mass == 0 :
      raise ZeroDivisionError( 'Cannot divide by a mass of zero.' )

    if mass is None :
      mass = MUON_MASS
    power = 1.0 / float( len( axes ) )
    return ( self.get_determinant( axes )**power ) / mass


  def get_alpha( self, axes = ['x', 'y'], mass = None, emitt = None ) :
    """
      Calculats a alpha function for the axes specified.

      The axes provided should be positional axes only, e.g. ['x', 'y']

      Beta function calculated as:
      ( Sum( Axis Covariances ) / ( emittance * mass * len( axes ) )
    """
    check_axes( axes, master_list=POSITION_VARIABLES )
    if mass == 0.0 :
      raise ZeroDivisionError( 'Cannot divide by a mass of zero.' )

    if mass is None :
      mass = MUON_MASS

    num = len( axes )
    conjugates = get_conjugates( axes )
    full_axes = axes + conjugates
    if emitt is None :
      emitt = self.get_emittance( mass=mass, axes=full_axes )

    cov_mat = self.get_covariance_matrix( full_axes )

    covariance_sum = 0.0
    for i in range( num ) :
      covariance_sum += cov_mat[i][num+i]

    return - ( covariance_sum / num ) / ( emitt * mass )


  def get_beta( self, axes = ['x', 'y'], mass = None, momentum = None, \
                                                               emitt = None ) :
    """
      Calculats a beta function for the axes specified.

      The axes provided should be positional axes only, e.g. ['x', 'y']

      Beta function calculated as:
      ( Sum( Axis Variances ) * momentum / ( emittance * mass * len( axes ) )
    """
    check_axes( axes, master_list=POSITION_VARIABLES )
    if mass == 0.0 :
      raise ZeroDivisionError( 'Cannot divide by a mass of zero.' )

    if mass is None :
      mass = MUON_MASS
    if momentum is None :
      momentum = self._mean_momentum / self._num_particles
#      raise ValueError( 'Please provide a value of mean beam momentum' )
    if momentum <= 0.0 :
      raise ValueError( 'Expected positive value of momentum' )

    num = len( axes )
    cov_mat = self.get_covariance_matrix( axes )
    full_axes = axes + get_conjugates( axes )
    if emitt is None :
      emitt = self.get_emittance( mass=mass, axes=full_axes )

    variance_sum = 0.0
    for i in range( len( axes ) ) :
      variance_sum += cov_mat[i][i]

    return ( variance_sum / num ) * ( momentum / ( emitt * mass ) )


  def get_means( self, axes = ['x', 'px', 'y', 'py', 'z', 'pz', 't', 'E' ] ) :
    """
      Returns an array of mean values for each of the parameters in the axis
      list.
    """
    check_axes( axes )
    means_vec = {}
    for axis in axes :
      rownum = VARIABLE_ENUMERATION[axis]
      means_vec[axis] = self._mean_vector[rownum] / self._num_particles
    return means_vec


  def get_mean( self, axis = 'x' ) :
    """
      Returns an array of mean values for each of the parameters in the axis
      list.
    """
    if not axis in VARIABLE_LIST :
      raise ValueError( 'Could not find axis with label: '+str(axis) )
    rownum = VARIABLE_ENUMERATION[axis]
    return self._mean_vector[rownum] / self._num_particles

#    for num, test in enumerate( VARIABLE_LIST ) :
#      if axis == test :
#        return self._mean_vector[num] / self._num_particles



class correction_matrix :
  """
    Performs a running calculation of the correct matrix to the covariance 
    matrix using in the emittance calculations.

    Using a subset of the 6D phase space (i.e. x,px,y,py) makes it faster!

    Initialise and then throw pairs of recon & MC hits at the class and it 
    saves the information in an efficient way.

    Uses the assumption that a measured hit has paramerters m[i], that are 
    related to the physical parameters by:

    m_i = u_i + d_i

    where d is some error. Then the covariance matrix, V is an approximation, 
    with corrections R & C such that:

    V_{true} = V_{meas} - R^T - R - C

  """
  def __init__( self ) :
    """
      Initialise the class. Sets counters and values all to zero.

      Records the full 6D phase space. Users can request a subset for most
      operations.

      Designed to have a low memory footprint and high speed. Rather than 
      toring every hit, running totals and sums are used to generate the
      matrices when required.
    """
    self._counter = 0
    self._length = len( VARIABLE_LIST )
    self._R_products = numpy.zeros( ( self._length, self._length ) )
    self._C_products = numpy.zeros( ( self._length, self._length ) )
    self._V_products = numpy.zeros( ( self._length, self._length ) )
    self._d_vector = numpy.zeros( ( self._length ) )
    self._u_vector = numpy.zeros( ( self._length ) )
    self._m_vector = numpy.zeros( ( self._length ) )

  def add_hit( self, recon, MC ) : 
    """
      Adds a pair of hits to the class. The difference in the properties of
      the two hits is used to create the correction matrix.

      Any hit-class that as a \"get(<string>) \" function is supported
    """
    deltas = numpy.zeros( ( self._length ) )

    for i in range( self._length ) :
      deltas[i] = recon.get( VARIABLE_LIST[i] ) - MC.get( VARIABLE_LIST[i] )

    for i in range( self._length ) :
      self._d_vector[i] += deltas[i]
      self._u_vector[i] += MC.get( VARIABLE_LIST[i] )

    for i in range( self._length ) :
      for j in range( self._length ) :
        self._R_products[i][j] += MC.get( VARIABLE_LIST[i] ) * deltas[j]
        self._C_products[i][j] += deltas[i] * deltas[j]

    self._counter += 1


  def __len__( self ) :
    """
      Return the total number of hits that have been processed.
    """
    return self._counter

  def number_hits( self ) :
    """
      Return the total number of hits that have been processed.
    """
    return self._counter


  def get_R_matrix( self, axes = [ 'x', 'px', 'y', 'py', 'z', 'pz' ] ) :
    """
      Calculates the R Correction matrix given by:
      R_{ij} = cov( u_i, d_j )

      Using the axes supplied. Defaults to the 4x4 Transverse case.
    """
    check_axes( axes )
    R = numpy.zeros( ( len( axes ), len( axes ) ) )


    for covrow, rowvar in enumerate( axes ) :
      row = -1
      for num, test in enumerate( VARIABLE_LIST ) :
        if rowvar == test :
          row = num
      for covcol, colvar in enumerate( axes ) :
        col = -1
        for num, test in enumerate( VARIABLE_LIST ) :
          if colvar == test :
            col = num

        R[covrow][covcol] = \
                  ( self._R_products[row][col] / float( self._counter ) ) - \
                          ( ( self._u_vector[row] * self._d_vector[col] ) / \
                                                 float( self._counter ** 2 ) )

    return R

  def get_C_matrix( self, axes = [ 'x', 'px', 'y', 'py', 'z', 'pz' ] ) :
    """
      Calculates the C Correction matrix given by:
      C_{ij} = cov( d_i, d_j )

      Using the axes supplied. Defaults to the 4x4 transverse case.
    """
    check_axes( axes )
    C = numpy.zeros( ( len( axes ), len( axes ) ) )

    for covrow, rowvar in enumerate( axes ) :
      row = -1
      for num, test in enumerate( VARIABLE_LIST ) :
        if rowvar == test :
          row = num
      for covcol, colvar in enumerate( axes ) :
        col = -1
        for num, test in enumerate( VARIABLE_LIST ) :
          if colvar == test :
            col = num

        C[covrow][covcol] = ( self._C_products[row][col] / \
                                                   float( self._counter ) ) -\
                          ( ( self._d_vector[row] * self._d_vector[col] ) / \
                                                 float( self._counter ** 2 ) )

    return C

  def get_full_correction( self, axes = [ 'x', 'px', 'y', 'py' ] ) :
    """ 
      Returns the full correction matrix to be added to the covariance matrix
    """
    R = self.get_R_matrix( axes )
    C = self.get_C_matrix( axes )
    Rt = numpy.matrix.transpose( R )

    new_matrix = -R - Rt - C
    return new_matrix


  def correct_covariance( self, cov_mat, axes = [ 'x', 'px', 'y', 'py' ] ) :
    """
      Calculate and apply the three correction matrices to the covariance 
      matrix.
    """
    correction = self.get_full_correction( axes )

    new_cov = cov_mat + correction

    return new_cov
    

  def clear_all( self ) :
    """
      Resets all the data to zeros. As if nothing ever happened!
    """
    self._R_products = numpy.zeros( ( self._length, self._length ) )
    self._C_products = numpy.zeros( ( self._length, self._length ) )
    self._d_vector = numpy.zeros( ( self._length ) )
    self._u_vector = numpy.zeros( ( self._length ) )
    self._counter = 0

  def save_R_matrix( self, filename ) :
    """
      Saves the R Matrix to a File to be used or examined later
    """
    numpy.savetxt( filename, self.get_R_matrix() )

  def save_C_matrix( self, filename ) :
    """ 
      Saves the C Matrix to a File to be used or examined later
    """
    numpy.savetxt( filename, self.get_C_matrix() )

  def save_full_correction( self, filename ) :
    """
      Saves the full correction matrix to a file to be used or examined later
    """
    numpy.savetxt( filename, self.get_full_correction() )


