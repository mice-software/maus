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
  Used for selecting  distributions of particles from a larger
  distribution.

  The parent distribution must be known prior to invocation as both the
  covariance matrix and number of particles is required.
"""

# pylint: disable = W0311, E1101, W0102, R0902, C0103, R0201, W0223
# pylint: disable = R0912, R0913, R0914, R0915, W0221, C0302

import numpy
from scipy.stats import chi2
import math
import ROOT

import analysis.hit_types
import analysis.inspectors
import analysis.covariances


MAX_STATISTICAL_WEIGHT = 10.0


################################################################################

def gaussian(x, mean, var) :
  """
    Analytically calculate values for a Gaussian distribution with given mean
    and variance
  """
  var_inv = 1.0/var
  vector = x - mean
  const = 1.0 / math.sqrt( 2.0*math.pi * var )
  exp = -0.5*( vector*vector*var_inv )

  return const * math.exp( exp )

def multivariate_gaussian(x, mean, cov) :
  """
    Analytically calculate vectors for a multivariate Gaussian distribution
    with given mean vector and covariance matrix
  """
  cov_inv = numpy.linalg.inv(cov)
  vector = x - mean
  const = 1.0 / math.sqrt( numpy.linalg.det( 2.0*math.pi * cov ) )
  exp = -0.5*( vector.transpose().dot(cov_inv.dot(vector)) )

  return const * math.exp( exp )

################################################################################


class Sampler_base(object) :
  """
    Base class for sampler-type object definitions.
    Outlines the interface.
  """
  def __init__(self) :
    pass


  def accept(self, hit) :
    """
      Test method. Returns true if the supplied hit should be accepted into the 
      daughter distribution.
    """
    raise NotImplementedError("Sample_base.accept()"+\
        " Method not overriden by derived class")


  def weight(self, hit) :
    """
      Test method. Returns the statistical weight of the hit for sampling.
    """
    raise NotImplementedError("Sample_base.accept()"+\
        " Method not overriden by derived class")


  def calculate_parent(self, x) :
    """
      Returns the value of the parent distribution at a position x
    """
    raise NotImplementedError("Sample_base.calculate_parent()"+\
        " Method not overriden by derived class")


  def calculate_daughter(self, x) :
    """
      Returns the value of the daughter distribution at a position x
    """
    raise NotImplementedError("Sample_base.calculate_daughter()"+\
        " Method not overriden by derived class")

  def get_plots(self) :
    """
      Returns a dictionary of plots for analysis.
    """
    return {}




class GaussianMomentumSampler(Sampler_base) :
  """
    Select a gaussian momentum distribution
  """
  def __init__(self, parent_dist, mean, std) :
    """
      Init.
      Requires parent TH1F momenutm distribution
      Mean of daughter distribution
      STD of daughter distribution
    """
    Sampler_base.__init__(self)
    self.__mean = mean
    self.__var = std*std

    self.__parent_distribution = parent_dist
    self.__parent_distribution.Scale(\
                                 1.0 / self.__parent_distribution.GetEntries())

    self.__max = 5.0*std
    self.__normalisation = 1.0e+20
    self.__weight_normalisation = 0.0


    # ACCEPT / REJECT
    for bin_i in range(self.__parent_distribution.GetNbinsX()) :
      x = self.__parent_distribution.GetBinCenter(bin_i)
      if abs(x - mean) > self.__max :
        continue

      if self.calculate_daughter(x) > 0.0 : #Divide by zero errors
        norm = self.calculate_parent(x) / self.calculate_daughter(x)
      else :
        norm = self.__normalisation

      if norm < self.__normalisation :
        self.__normalisation = norm


    # STATISTICAL WEIGHTING
    max_daughter = 0.0
    max_parent = 0.0

    for bin_i in range(self.__parent_distribution.GetNbinsX()) :
      x = self.__parent_distribution.GetBinCenter(bin_i)
      if abs(x - mean) > self.__max :
        continue

      daughter = self.calculate_daughter(x)
      parent = self.calculate_parent(x)
      if daughter > max_daughter :
        max_daughter = daughter
        max_parent = parent
        
    self.__weight_normalisation = max_parent / max_daughter


  def calculate_parent(self, x) :
    """
      Calculate parent probability for momentum
    """
    found_bin = self.__parent_distribution.FindBin(x)
    return self.__parent_distribution.GetBinContent(found_bin)


  def calculate_daughter(self, x) :
    """
      Calculate daughter probability for momentum
    """
    return gaussian( x, self.__mean, self.__var )


  def accept(self, hit) :
    p = hit.get_p()

    if abs(p - self.__mean) > self.__max :
      return False

    u = numpy.random.sample() #Uniform random number (0,1)
    g_y = self.calculate_daughter(p)*self.__normalisation
    f_y = self.calculate_parent(p)

    if ( u >= (g_y / f_y) ) :
      return False
    else :
      return True


  def weight(self, hit) :
    p = hit.get_p()

    if abs(p - self.__mean) > self.__max :
      return 0.0

    g_y = self.calculate_daughter(p)*self.__weight_normalisation
    f_y = self.calculate_parent(p)

    ratio = (g_y / f_y)
    return ratio



class MomentumWindowSampler(Sampler_base) :
  """
    Sample a square momentum selection
  """
  def __init__(self, parent_dist, mean, window) :
    Sampler_base.__init__(self)
    self.__mean = mean
    self.__window = window/2.0

    self.__parent_distribution = parent_dist


  def calculate_parent(self, x) :
    """
      Nothing to do
    """
    return None


  def calculate_daughter(self, x) :
    """
      Nothing to do
    """
    return None


  def accept(self, hit) :
    p = hit.get_p()

    if abs(p - self.__mean) > self.__window :
      return False
    else :
      return True




class GaussianXYSampler(Sampler_base) :
  """
    Multivariate Gaussian Sampling class.
    Assumes both the parent and daughter distributions are gaussian.
  """

  def __init__(self, parent_means, parent_cov, means, cov) :
    """
      Expects 
    """
    Sampler_base.__init__(self)
    if parent_means.shape != (2,) :
      raise ValueError("Parent Means must be a 2x1 vector")
    if parent_cov.shape != (2, 2) :
      raise ValueError("Parent Covariance must be a 2x2 matrix")
    if means.shape != (2,) :
      raise ValueError("Means must be a 2x1 vector")
    if cov.shape != (2,2) :
      raise ValueError("Covariance must be a 2x2 matrix")

    self.__parent_means = parent_means
    self.__parent_covariance = parent_cov
    self.__means = means
    self.__covariance = cov

    self.__normalisation = math.sqrt(numpy.linalg.det(self.__covariance) / \
                                     numpy.linalg.det(self.__parent_covariance))


  def calculate_parent(self, x) :
    """
      Returns the value of the parent distribution at a position x
    """
    return multivariate_gaussian(x, self.__parent_means, \
                                                      self.__parent_covariance)


  def calculate_daughter(self, x) :
    """
      Returns the value of the daughter distribution at a position x
    """
    return multivariate_gaussian(x, self.__means, self.__covariance)


  def accept(self, hit) :
    """
      Returns true if the hit-vector is to be acceted into the daughter
      distribution, otherwise returns false.
    """
    vector = [ hit.get_x(), hit.get_y() ]

    u = numpy.random.sample() #Uniform random number (0,1)
    g_y = self.calculate_daughter(vector)*self.__normalisation
    f_y = self.calculate_parent(vector)

    if ( u >= (g_y / f_y) ) :
      return False
    else :
      return True




class Gaussian4DSampler(Sampler_base) :
  """
    Multivariate Gaussian Sampling class.
    Assumes both the parent and daughter distributions are gaussian.
  """

  def __init__(self, parent_means, parent_cov, means, cov) :
    """
      Expects 
    """
    Sampler_base.__init__(self)
    if parent_means.shape != (4,) :
      raise ValueError("Parent Means must be a 4x1 vector")
    if parent_cov.shape != (4, 4) :
      raise ValueError("Parent Covariance must be a 4x4 matrix")
    if means.shape != (4,) :
      raise ValueError("Means must be a 4x1 vector")
    if cov.shape != (4,4) :
      raise ValueError("Covariance must be a 4x4 matrix")

    self.__parent_means = parent_means
    self.__parent_covariance = parent_cov
    self.__means = means
    self.__covariance = cov

    self.__normalisation = math.sqrt(numpy.linalg.det(self.__covariance) / \
                                     numpy.linalg.det(self.__parent_covariance))


  def calculate_parent(self, x) :
    """
      Returns the value of the parent distribution at a position x
    """
    return multivariate_gaussian(x, self.__parent_means, \
                                                      self.__parent_covariance)


  def calculate_daughter(self, x) :
    """
      Returns the value of the daughter distribution at a position x
    """
    return multivariate_gaussian(x, self.__means, self.__covariance)


  def accept(self, hit) :
    """
      Returns true if the hit-vector is to be acceted into the daughter
      distribution, otherwise returns false.
    """
    vector = hit.get_as_vector()[2:6]

    u = numpy.random.sample() #Uniform random number (0,1)
    g_y = self.calculate_daughter(vector)*self.__normalisation
    f_y = self.calculate_parent(vector)

    if ( u >= (g_y / f_y) ) :
      return False
    else :
      return True


class Amplitude4DSampler(Sampler_base) :
  """
    1D Chi-Squared Distribution Sampler
    Assumes both the parent and daughter distributions are Chi-Squared
    distributed.
  """

  def __init__(self, parent_distribution, covariance, emittance, max_x=1.0e12) :
    """
      Setup Amplitude4D Sampler. Requires the parent amplitude distribtion,
      parent covariance matrix, required emittance and a maximum amplitude
      range.
    """
    Sampler_base.__init__(self)
    if type(parent_distribution) != ROOT.TH1F :
      raise ValueError("Parent distribution must be a ROOT::TH1F")
    if covariance.shape != (4,4) :
      raise ValueError("Covariance must be a 4x4 matrix")

    self.__parent_distribution = parent_distribution
    self.__parent_distribution.Scale(\
                                 1.0 / self.__parent_distribution.GetEntries())
    self.__weight_normalisation = 0.0
    self.__normalisation = 1.0e+20
    self.__parent_emittance =\
                         analysis.covariances.emittance_from_matrix(covariance)
    self.__emittance = emittance
    self.__parent_covariance = covariance
    self.__parent_covariance_inv = numpy.linalg.inv(covariance)
    self.__ndf = 4
    self.__max = max_x

    # ACCEPT / REJECT
    for bin_i in range(self.__parent_distribution.GetNbinsX()) :
      x = self.__parent_distribution.GetBinCenter(bin_i)
      if x > self.__max :
        break

      if self.calculate_daughter(x) > 0.0 : #Divide by zero errors
        norm = self.calculate_parent(x) / self.calculate_daughter(x)
      else :
        norm = self.__normalisation

      if norm < self.__normalisation :
        self.__normalisation = norm


    # STATISTICAL WEIGHTING
    max_daughter = 0.0
    max_parent = 0.0

    for bin_i in range(self.__parent_distribution.GetNbinsX()) :
      x = self.__parent_distribution.GetBinCenter(bin_i)
      if x > self.__max :
        continue

      daughter = self.calculate_daughter(x)
      parent = self.calculate_parent(x)
      if daughter > max_daughter :
        max_daughter = daughter
        max_parent = parent
        
    self.__weight_normalisation = max_parent / max_daughter

    print "Parent =", self.__parent_emittance
    print "Select =", self.__emittance


  def _get_amplitude(self, x) :
    """
      Return the amplitude of a particle given the state vector.
    """
    amplitude = self.__parent_emittance*x.transpose().dot(\
                                         self.__parent_covariance_inv.dot(x))
    return amplitude


  def calculate_parent(self, x) :
    """
      Calculate parent probability for amplitude
    """
    found_bin = self.__parent_distribution.FindBin(x)
    return self.__parent_distribution.GetBinContent(found_bin)


  def calculate_daughter(self, x) :
    """
      Calculate daughter probability for amplitude
    """
#    c = x*self.__emittance/(self.__parent_emittance**2)
    c = x/self.__emittance
    return chi2.pdf(c, self.__ndf)


  def accept(self, hit) :
    """
      Returns true if the hit-vector is to be acceted into the daughter
      distribution, otherwise returns false.
    """

    x = self._get_amplitude(numpy.array(hit.get_as_vector()[2:6]))

    if x > self.__max :
      return False

    u = numpy.random.sample() #Uniform random number (0,1)
    g_y = self.calculate_daughter(x)*self.__normalisation
    f_y = self.calculate_parent(x)

    if ( u >= (g_y / f_y) ) :
      return False
    else :
      return True


  def weight(self, hit) :
    x = self._get_amplitude(numpy.array(hit.get_as_vector()[2:6]))

    if x > self.__max :
      return 0.0

    g_y = self.calculate_daughter(x)*self.__weight_normalisation
    f_y = self.calculate_parent(x)

    ratio = (g_y / f_y)
    return ratio




class UniformAmplitude4DSampler(Sampler_base) :
  """
    1D Flat Amplitude Distribution Sampler
  """

  def __init__(self, parent_distribution, covariance, amplitude) :
    """

    """
    Sampler_base.__init__(self)
    if type(parent_distribution) != ROOT.TH1F :
      raise ValueError("Parent distribution must be a ROOT::TH1F")
    if covariance.shape != (4,4) :
      raise ValueError("Covariance must be a 4x4 matrix")

    self.__parent_distribution = parent_distribution
    self.__parent_distribution.Scale(\
                                 1.0 / self.__parent_distribution.GetEntries())
    self.__weight_normalisation = 0.0
    self.__normalisation = 1.0e+20
    self.__parent_emittance =\
                         analysis.covariances.emittance_from_matrix(covariance)
    self.__max_amplitude = amplitude
    self.__square = 1.0 / amplitude
    self.__parent_covariance = covariance
    self.__parent_covariance_inv = numpy.linalg.inv(covariance)

    # ACCEPT / REJECT
    for bin_i in range(1, self.__parent_distribution.GetNbinsX()) :
      x = self.__parent_distribution.GetBinCenter(bin_i)
      if x > self.__max_amplitude :
        break

      if self.calculate_daughter(x) > 0.0 : #Divide by zero errors
        norm = self.calculate_parent(x) / self.calculate_daughter(x)
      else :
        norm = self.__normalisation

      if norm < self.__normalisation :
        self.__normalisation = norm

    # STATISTICAL WEIGHTING
    x = self.__max_amplitude * 0.5
    max_daughter = self.calculate_daughter(x)
    max_parent = self.calculate_parent(x)
    self.__weight_normalisation = max_parent / max_daughter


  def _get_amplitude(self, x) :
    """
      Calculate the amplitude of a particle given the state vector
    """
    amplitude = self.__parent_emittance*x.transpose().dot(\
                                         self.__parent_covariance_inv.dot(x))
    return amplitude


  def calculate_parent(self, x) :
    """
      Calculate parent probability for amplitude
    """
    found_bin = self.__parent_distribution.FindBin(x)
    return self.__parent_distribution.GetBinContent(found_bin)


  def calculate_daughter(self, x) :
    """
      Calculate daughter probability for amplitude
    """
    if x > self.__max_amplitude :
      return 0.0
    else :
      return self.__square


  def accept(self, hit) :
    """
      Returns true if the hit-vector is to be acceted into the daughter
      distribution, otherwise returns false.
    """

    x = self._get_amplitude(numpy.array(hit.get_as_vector()[2:6]))

    if x > self.__max_amplitude :
      return False

    u = numpy.random.sample() #Uniform random number (0,1)
    g_y = self.calculate_daughter(x)*self.__normalisation
    f_y = self.calculate_parent(x)

    if ( u >= (g_y / f_y) ) :
      return False
    else :
      return True


  def weight(self, hit) :
    x = self._get_amplitude(numpy.array(hit.get_as_vector()[2:6]))

    if x > self.__max_amplitude :
      return 0.0

    g_y = self.calculate_daughter(x)*self.__weight_normalisation
    f_y = self.calculate_parent(x)

    ratio = (g_y / f_y)
    return ratio



class XYPhaseSpaceSampler(Sampler_base) :
  """
    1D Chi-Squared Distribution Sampler
    Assumes both the parent and daughter distributions are Chi-Squared
    distributed.
  """

  def __init__(self, parent_distribution_x, parent_distribution_y,\
                                                           means, covariance) :
    """
      Initialise the transverse phase space sampler. Requires the x-px phase
      space distribution, the y-py phase space distribution, the means of the 
      distributions and the covariance matrix.
    """
    Sampler_base.__init__(self)
    if type(parent_distribution_x) != ROOT.TH2F :
      raise ValueError("Parent distributions must be a ROOT::TH2F")
    if type(parent_distribution_y) != ROOT.TH2F :
      raise ValueError("Parent distributions must be a ROOT::TH2F")
    if means.shape != (2,) :
      raise ValueError("Means must be a 2x1 vector")
    if covariance.shape != (2, 2) :
      raise ValueError("Covariance must be a 2x2 vector")

    self.__parent_distribution_x = parent_distribution_x
    self.__parent_distribution_y = parent_distribution_y
    self.__parent_distribution_x.Scale(\
                               1.0 / self.__parent_distribution_x.GetEntries())
    self.__parent_distribution_y.Scale(\
                               1.0 / self.__parent_distribution_y.GetEntries())
    self.__weight_normalisation_x = 0.0
    self.__weight_normalisation_y = 0.0
    self.__normalisation_x = 1.0e+20
    self.__normalisation_y = 1.0e+20
    self.__max = 1.0e+20
    self.__means = means
    self.__covariance = covariance
    self.__covariance_inv = numpy.linalg.inv(covariance)

    self.__max = 9.0

    # ACCEPT / REJECT
    for bin_i in range(self.__parent_distribution_x.GetNbinsX()) :
      for bin_j in range(self.__parent_distribution_x.GetNbinsY()) :
        x = self.__parent_distribution_x.GetXaxis().GetBinCenter(bin_i)
        px = self.__parent_distribution_x.GetYaxis().GetBinCenter(bin_j)
        vec = numpy.array([x,px])
        if not self._is_contained(vec) :
          continue

        if self.calculate_daughter(vec) > 0.0 : #Divide by zero errors
          norm = self.calculate_parent(vec, self.__parent_distribution_x) / \
                                      self.calculate_daughter(numpy.array(vec))
        else :
          norm = self.__normalisation_x

        if norm < self.__normalisation_x :
          self.__normalisation_x = norm

    for bin_i in range(self.__parent_distribution_y.GetNbinsX()) :
      for bin_j in range(self.__parent_distribution_y.GetNbinsY()) :
        x = self.__parent_distribution_y.GetXaxis().GetBinCenter(bin_i)
        px = self.__parent_distribution_y.GetYaxis().GetBinCenter(bin_j)
        vec = numpy.array([x,px])
        if not self._is_contained(vec) :
          continue

        if self.calculate_daughter(vec) > 0.0 : #Divide by zero errors
          norm = self.calculate_parent(vec, self.__parent_distribution_y) / \
                                      self.calculate_daughter(numpy.array(vec))
        else :
          norm = self.__normalisation_y

        if norm < self.__normalisation_y :
          self.__normalisation_y = norm


    # STATISTICAL WEIGHTING
    max_daughter = 0.0
    max_parent = 0.0

    for bin_i in range(self.__parent_distribution.GetNbinsX()) :
      for bin_j in range(self.__parent_distribution_x.GetNbinsY()) :
        x = self.__parent_distribution_x.GetXaxis().GetBinCenter(bin_i)
        px = self.__parent_distribution_x.GetYaxis().GetBinCenter(bin_j)
        vec = numpy.array([x,px])
        if not self._is_contained(vec) :
          continue

        daughter = self.calculate_daughter(vec)
        parent = self.calculate_parent(vec)
        if daughter > max_daughter :
          max_daughter = daughter
          max_parent = parent
          
    self.__weight_normalisation_x = max_parent / max_daughter

    max_daughter = 0.0
    max_parent = 0.0

    for bin_i in range(self.__parent_distribution_y.GetNbinsX()) :
      for bin_j in range(self.__parent_distribution_y.GetNbinsY()) :
        x = self.__parent_distribution_y.GetXaxis().GetBinCenter(bin_i)
        px = self.__parent_distribution_y.GetYaxis().GetBinCenter(bin_j)
        vec = numpy.array([x,px])
        if not self._is_contained(vec) :
          continue

        daughter = self.calculate_daughter(vec)
        parent = self.calculate_parent(vec)
        if daughter > max_daughter :
          max_daughter = daughter
          max_parent = parent
          
    self.__weight_normalisation_y = max_parent / max_daughter



  def _is_contained(self, x) :
    """
      Function to determine whether or not a particle's state vector is 
      contained withing the amplitude maximum
    """
    amp = x.transpose().dot(self.__covariance_inv.dot(x))
#    print amp
    if amp < self.__max :
      return True
    else :
      return False


  def calculate_parent(self, x, distribution=None) :
    """
      Calculate parent probability for the vector x, given the parent
      distribution
    """
    if distribution is None :
      distribution = self.__parent_distribution_x

    found_bin = distribution.FindBin(x[0], x[1])
    return distribution.GetBinContent(found_bin)


  def calculate_daughter(self, x) :
    """
      Calculate daughter probability for a 4D vector
    """
    return multivariate_gaussian(x, self.__means, self.__covariance)


  def accept(self, hit) :
    """
      Returns true if the hit-vector is to be acceted into the daughter
      distribution, otherwise returns false.
    """
    x = [ hit.get_x(), hit.get_px() ]
    y = [ hit.get_y(), hit.get_py() ]

    if not self._is_contained(x) or not self._is_contained(y) :
      return False

    u = numpy.random.sample() #Uniform random number (0,1)
    g_y_x = self.calculate_daughter(x)*self.__normalisation_x
    f_y_x = self.calculate_parent(x, self.__parent_distribution_x)

    g_y_y = self.calculate_daughter(y)*self.__normalisation_y
    f_y_y = self.calculate_parent(y, self.__parent_distribution_y)

    if ( u >= (g_y_x / f_y_x) ) and ( u >= (g_y_y / f_y_y) ) :
      return False
    else :
      return True


  def weight(self, hit) :
    x = [ hit.get_x(), hit.get_px() ]
    y = [ hit.get_y(), hit.get_py() ]

    if not self._is_contained(x) or not self._is_contained(y) :
      return 0.0

    g_y_x = self.calculate_daughter(x)*self.__weight_normalisation_x
    f_y_x = self.calculate_parent(x, self.__parent_distribution_x)

    g_y_y = self.calculate_daughter(y)*self.__weight_normalisation_y
    f_y_y = self.calculate_parent(y, self.__parent_distribution_y)

    ratio_x = (g_y_x / f_y_x)
    ratio_y = (g_y_y / f_y_y)
    return ratio_x*ratio_y



class XY4DPhaseSpaceSampler(Sampler_base) :
  """
    4D Sampler for beams that require the 6 2D projections from which the sample
    is made.
  """

  def __init__(self, x_px, x_py, y_px, y_py, x_y, px_py, means, covariance) :
    """

    """
    Sampler_base.__init__(self)
    if type(x_px) != ROOT.TH2F :
      raise ValueError("Parent distributions must be a ROOT::TH2F. Not "\
                                                              +str(type(x_px)))
    if type(x_py) != ROOT.TH2F :
      raise ValueError("Parent distributions must be a ROOT::TH2F. Not "\
                                                              +str(type(x_py)))
    if type(y_py) != ROOT.TH2F :
      raise ValueError("Parent distributions must be a ROOT::TH2F. Not "\
                                                              +str(type(y_py)))
    if type(y_px) != ROOT.TH2F :
      raise ValueError("Parent distributions must be a ROOT::TH2F. Not "\
                                                              +str(type(y_px)))
    if type(px_py) != ROOT.TH2F :
      raise ValueError("Parent distributions must be a ROOT::TH2F. Not "\
                                                             +str(type(px_py)))
    if type(x_y) != ROOT.TH2F :
      raise ValueError("Parent distributions must be a ROOT::TH2F. Not "\
                                                               +str(type(x_y)))

    if means.shape != (4,) :
      raise ValueError("Means must be a 4x1 vector")
    if covariance.shape != (4, 4) :
      raise ValueError("Covariance must be a 4x4 vector")

    self.__all_cells = [(0, 1), (0, 2), (0, 3), (2, 1), (1, 3), (2, 3)]

    self.__distributions = [ [ None for _ in range(4) ] for _ in range(4) ]
    self.__distributions[0][1] = x_px
    self.__distributions[0][2] = x_y
    self.__distributions[0][3] = x_py
    self.__distributions[2][1] = y_px
    self.__distributions[1][3] = px_py
    self.__distributions[2][3] = y_py

    self.__normalisations = [ [ None for _ in range(4) ] for _ in range(4) ]
    for a, b in self.__all_cells:
      self.__normalisations[a][b] = 1.0e+20

    self.__weight_normalisations =\
                               [ [ None for _ in range(4) ] for _ in range(4) ]
    for a, b in self.__all_cells:
      self.__weight_normalisations[a][b] = 0.0

    for a, b in self.__all_cells:
      self.__distributions[a][b].Scale(\
                             1.0/(self.__distributions[a][b].GetEntries()**2) )

    self.__means = means
    self.__covariance = covariance
    self.__covariance_inv = numpy.linalg.inv(covariance)

    self.__max = 12.0
    self.__parent_min = 0.5

    # ACCEPT / REJECT
    while True :
      for a, b in self.__all_cells :
        dist = self.__distributions[a][b]
        self.__normalisations[a][b] = 1.0e+20

        for bin_i in range(dist.GetNbinsX()) :
          for bin_j in range(dist.GetNbinsY()) :
            x = dist.GetXaxis().GetBinCenter(bin_i)
            px = dist.GetYaxis().GetBinCenter(bin_j)

            vec = numpy.array([0.0, 0.0, 0.0, 0.0])
            vec[a] = x
            vec[b] = px

            if not self._is_contained(vec) :
              continue

            if self.calculate_daughter(vec) > 0.0 : #Divide by zero errors
              norm = self.calculate_parent(numpy.array([x, px]), dist) / \
                                                   self.calculate_daughter(vec)
            else :
              norm = self.__normalisations[a][b]

            if norm < self.__normalisations[a][b] :
              self.__normalisations[a][b] = norm

        if self.__normalisations[a][b] < self.__parent_min :
          self.__max = self.__max - 1.0
          break

      else :
        break

    # STATISTICAL WEIGHTING
    for a, b in self.__all_cells :
      dist = self.__distributions[a][b]
      self.__normalisations[a][b] = 0.0
      max_daughter = 0.0
      max_parent = 0.0

      for bin_i in range(dist.GetNbinsX()) :
        for bin_j in range(dist.GetNbinsY()) :
          x = dist.GetXaxis().GetBinCenter(bin_i)
          px = dist.GetYaxis().GetBinCenter(bin_j)

          vec = numpy.array([0.0, 0.0, 0.0, 0.0])
          vec[a] = x
          vec[b] = px

          if not self._is_contained(vec) :
            continue

          daughter = self.calculate_daughter(vec)
          parent = self.calculate_parent(numpy.array([x, px]), dist)

          if daughter > max_daughter :
            max_daughter = daughter
            max_parent = parent

      self.__weight_normalisation[a][b] = max_parent / max_daughter


  def _is_contained(self, x) :
    """
      Function to determine whether or not a particle's state vector is 
      contained withing the amplitude maximum
    """
    amp = x.transpose().dot(self.__covariance_inv.dot(x))
    if amp < self.__max :
#      print amp, x
      return True
    else :
      return False


  def calculate_parent(self, x, distribution=None) :
    """
      Calculate parent probability for a vector x, given the parent
      distribution
    """
    if distribution is None :
      distribution = self.__x_y

    found_bin = distribution.FindBin(x[0], x[1])
    return distribution.GetBinContent(found_bin)


  def calculate_daughter(self, x) :
    """
      Calculate daughter probability for a 4D vector
    """
    return multivariate_gaussian(x, self.__means, self.__covariance)


  def accept(self, hit) :
    """
      Returns true if the hit-vector is to be acceted into the daughter
      distribution, otherwise returns false.
    """

    vec = numpy.array(hit.get_as_vector()[2:6])

#    if not self._is_contained(vec) :
#      return False

    u = numpy.random.sample() #Uniform random number (0,1)
    daughter = self.calculate_daughter(vec)

    for a, b in self.__all_cells :
      dist = self.__distributions[a][b]
      vector = [ vec[a], vec[b] ]
      parent_result = self.calculate_parent(vector, dist)

      result = (daughter*self.__normalisations[a][b] / parent_result)
      if (u < result ) :
        continue
      else :
        break
    else : 
#      self.__selection_inspector.add_hit(vec)
      return True

    return False


  def weight(self, hit) :
    vec = numpy.array(hit.get_as_vector()[2:6])

    daughter = self.calculate_daughter(vec)
    ratio = 1.0

    for a, b in self.__all_cells :
      dist = self.__distributions[a][b]
      vector = [ vec[a], vec[b] ]
      parent_result = self.calculate_parent(vector, dist)

      result = (daughter*self.__weight_normalisations[a][b] / parent_result)

      ratio *= result

    return ratio

