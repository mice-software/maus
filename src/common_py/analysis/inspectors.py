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

# pylint: disable = W0311, E1101, W0613, C0111, R0911, W0621, C0103, R0902
# pylint: disable = W0102, R0915, R0201


import analysis
import analysis.covariances

import ROOT
import math
import numpy


class PhaseSpace2DInspector() :

  def __init__(self, plane_id, ensemble_size) :
    self.plane = plane_id
    self.position = 0.0
    self.ensemble_size = ensemble_size
    self.covariance = analysis.covariances.CovarianceMatrix()
    self.ensemble_covariance = analysis.covariances.CovarianceMatrix()
    self.covariance_matrix = None

    self.momentum_bias = None
    self.momentum_correlation = None

    self.parent_covariance = None
    self.parent_covariance_inv = None
    self.parent_emittance = None

    self.position_plot = ROOT.TH2F(\
           'inspected_position_{0}'.format(self.plane), 'Beam Position', \
                                        100, -400.0, 400.0, 100, -400.0, 400.0)
    self.momentum_plot = ROOT.TH2F(\
           'inspected_momentum_{0}'.format(self.plane), 'Beam Momentum', \
                                        100, -400.0, 400.0, 100, -400.0, 400.0)

    self.x_phasespace_plot = ROOT.TH2F(\
        'inspected_x_phasespace_{0}'.format(self.plane), 'X-Phasespace', \
                                        100, -400.0, 400.0, 100, -400.0, 400.0)
    self.y_phasespace_plot = ROOT.TH2F(\
        'inspected_y_phasespace_{0}'.format(self.plane), 'Y-Phasespace', \
                                        100, -400.0, 400.0, 100, -400.0, 400.0)
    self.xy_phasespace_plot = ROOT.TH2F(\
        'inspected_xpy_phasespace_{0}'.format(self.plane), 'X-Py-Phasespace', \
                                        100, -400.0, 400.0, 100, -400.0, 400.0)
    self.yx_phasespace_plot = ROOT.TH2F(\
        'inspected_ypx_phasespace_{0}'.format(self.plane), 'Y-Px-Phasespace', \
                                        100, -400.0, 400.0, 100, -400.0, 400.0)
    self.rpt_phasespace_plot = ROOT.TH2F(\
        'inspected_rpt_phasespace_{0}'.format(self.plane), 'r-Pt-Phasespace', \
                                        100, 0.0, 400.0, 100, 0.0, 400.0)
    self.phi_phasespace_plot = ROOT.TH1F(\
      'inspected_phi_phasespace_{0}'.format(self.plane), '#phi-Phasespace', \
                                                               100, -4.0, 4.0 )
    self.theta_phasespace_plot = ROOT.TH1F(\
      'inspected_theta_phasespace_{0}'.format(self.plane), '#theta-Phasespace', \
                                                               100, -4.0, 4.0 )

    self.pz_plot = ROOT.TH1F(\
              'inspected_pz_{0}'.format(self.plane), 'p_{z}', 400, 0.0, 400.0 )
    self.p_plot = ROOT.TH1F(\
                   'inspected_p_{0}'.format(self.plane), 'p', 400, 0.0, 400.0 )

    
    self.emittance_plot = ROOT.TH1F(\
              'inspected_emittance_{0}'.format(self.plane), 'Emittance', \
                                                               1000, 0.0, 20.0)
    self.emittance_x_plot = ROOT.TH1F(\
           'inspected_emittance_x_{0}'.format(self.plane), 'X Emittance', \
                                                            1000, 0.0, 20.0)
    self.emittance_y_plot = ROOT.TH1F(\
         'inspected_emittance_y_{0}'.format(self.plane), 'Y Emittance', \
                                                            1000, 0.0, 20.0)
    self.alpha_plot = ROOT.TH1F(\
               'inspected_alpha_{0}'.format(self.plane), 'Alpha Function', \
                                                             2000, -5.0, 5.0)
    self.alpha_x_plot = ROOT.TH1F(\
               'inspected_alpha_x_{0}'.format(self.plane), 'X Alpha Function', \
                                                             2000, -5.0, 5.0)
    self.alpha_y_plot = ROOT.TH1F(\
               'inspected_alpha_y_{0}'.format(self.plane), 'Y Alpha Function', \
                                                             2000, -5.0, 5.0)
    self.beta_plot = ROOT.TH1F(\
               'inspected_beta_{0}'.format(self.plane), 'Beta Function', \
                                                             3000, 0.0, 3000.0)
    self.beta_x_plot = ROOT.TH1F(\
               'inspected_beta_x_{0}'.format(self.plane), 'X Beta Function', \
                                                             3000, 0.0, 3000.0)
    self.beta_y_plot = ROOT.TH1F(\
               'inspected_beta_y_{0}'.format(self.plane), 'Y Beta Function', \
                                                             3000, 0.0, 3000.0)
    self.total_momentum_plot = ROOT.TH1F(\
              'inspected_total_momentum_{0}'.format(self.plane), 'Momentum', \
                                                              1000, 0.0, 500.0)

    self.amplitude_plot = ROOT.TH1F(\
            'single_particle_amplitudes_{0}'.format(self.plane), 'Amplitude', \
                                                              200, 0.0, 100.0)
    self.amplitude_momentum_plot = ROOT.TH2F(\
        'inspected_A_p_phasespace_{0}'.format(self.plane), 'A-p-Phasespace' , \
                                           200, 0.0, 100.0, 260, 130.0, 260.0 )



  def set_covariance_correction(self, correction, axes=['x', 'px', 'y', 'py']) :
    self.covariance.set_covariance_correction( correction, axes )
    self.ensemble_covariance.set_covariance_correction( correction, axes )


  def set_parent_covariance(self, matrix) :
    self.parent_covariance = numpy.array(matrix)
    self.parent_covariance_inv = numpy.linalg.inv(self.parent_covariance)
    self.parent_emittance = analysis.covariances.emittance_from_matrix(self.parent_covariance)


  def set_momentum_correction(self, constant, gradient) :
    self.momentum_bias = constant
    self.momentum_correlation = gradient


  def add_hit(self, hit) :
    if self.momentum_bias is not None :
      p = hit.get_p()
#      correction = 1.0 + (self.momentum_bias + self.momentum_correlation*p) / p
      correction = ((p - self.momentum_bias) / \
                                         (1.0 - self.momentum_correlation)) / p
      hit.set_px(hit.get_px()*correction)
      hit.set_py(hit.get_py()*correction)
      hit.set_pz(hit.get_pz()*correction)

    if self.parent_covariance_inv is not None :
      vector = numpy.array(hit.get_as_vector()[2:6])
      amplitude = self.parent_emittance*vector.transpose().dot(self.parent_covariance_inv.dot(vector))
      self.amplitude_plot.Fill(amplitude, hit.get_weight())
      self.amplitude_momentum_plot.Fill(amplitude, hit.get_p(), hit.get_weight())

    self.covariance.add_hit(hit)

    self.position_plot.Fill(hit.get_x(), hit.get_y(), hit.get_weight())
    self.momentum_plot.Fill(hit.get_px(), hit.get_py(), hit.get_weight())
    self.x_phasespace_plot.Fill(hit.get_x(), hit.get_px(), hit.get_weight())
    self.y_phasespace_plot.Fill(hit.get_y(), hit.get_py(), hit.get_weight())
    self.xy_phasespace_plot.Fill(hit.get_x(), hit.get_py(), hit.get_weight())
    self.yx_phasespace_plot.Fill(hit.get_y(), hit.get_px(), hit.get_weight())
    self.rpt_phasespace_plot.Fill(hit.get_r(), hit.get_pt(), hit.get_weight())
    self.phi_phasespace_plot.Fill(hit.get_phi(), hit.get_weight())
    self.theta_phasespace_plot.Fill(hit.get_theta(), hit.get_weight())
    self.pz_plot.Fill(hit.get_pz(), hit.get_weight())
    self.p_plot.Fill(hit.get_p(), hit.get_weight())
    self.position = hit.get_z()

    if self.ensemble_size > 1 :
      self.ensemble_covariance.add_hit(hit)

      if self.covariance.length() > self.ensemble_size :
        self.fill_plots()


  def fill_plots(self) :
    if ( self.covariance.length() < (self.ensemble_size / 2) ) or self.ensemble_size < 2 :
      return

    self.emittance_plot.Fill(self.covariance.get_emittance(\
                                                          ['x','px','y','py']))
    self.emittance_x_plot.Fill(self.ensemble_covariance.get_emittance(['x','px']))
    self.emittance_y_plot.Fill(self.ensemble_covariance.get_emittance(['y','py']))
    self.alpha_plot.Fill(self.ensemble_covariance.get_alpha(['x','y']))
    self.alpha_x_plot.Fill(self.ensemble_covariance.get_alpha(['x']))
    self.alpha_y_plot.Fill(self.ensemble_covariance.get_alpha(['y']))
    self.beta_plot.Fill(self.ensemble_covariance.get_beta(['x','y']))
    self.beta_x_plot.Fill(self.ensemble_covariance.get_beta(['x']))
    self.beta_y_plot.Fill(self.ensemble_covariance.get_beta(['y']))
    self.total_momentum_plot.Fill(self.ensemble_covariance.get_momentum())

    self.covariance_matrix = self.ensemble_covariance.get_covariance_matrix(['x', 'px', 'y', 'py'])
    self.ensemble_covariance.clear()


  def get_current_ensemble_size(self) :
    return self.ensemble_covariance.length()


  def get_length(self) :
    return self.covariance.length()


  def get_number_ensembles(self) :
    return self.emittance_plot.GetEntries()


  def get_plot_dictionary(self) :
    ins_plots = {}
    ins_plots['x_y'] = self.position_plot
    ins_plots['px_py'] = self.momentum_plot
    ins_plots['x_px'] = self.x_phasespace_plot
    ins_plots['y_py'] = self.y_phasespace_plot
    ins_plots['x_py'] = self.xy_phasespace_plot
    ins_plots['y_px'] = self.yx_phasespace_plot
    ins_plots['r_pt'] = self.rpt_phasespace_plot
    ins_plots['phi'] = self.phi_phasespace_plot
    ins_plots['theta'] = self.theta_phasespace_plot
    ins_plots['pz'] = self.pz_plot
    ins_plots['p'] = self.p_plot
    ins_plots['emittance'] = self.emittance_plot
    ins_plots['emittance_x'] = self.emittance_x_plot
    ins_plots['emittance_y'] = self.emittance_y_plot
    ins_plots['alpha'] = self.alpha_plot
    ins_plots['alpha_x'] = self.alpha_x_plot
    ins_plots['alpha_y'] = self.alpha_y_plot
    ins_plots['beta'] = self.beta_plot
    ins_plots['beta_x'] = self.beta_x_plot
    ins_plots['beta_y'] = self.beta_y_plot
    ins_plots['momentum'] = self.total_momentum_plot
    ins_plots['amplitude'] = self.amplitude_plot
    ins_plots['amplitude_momentum'] = self.amplitude_momentum_plot

    return ins_plots


  def get_data_dictionary(self) :
    ins_data = {}
    ins_data['position'] = self.position
    ins_data['plane'] = self.plane

    ins_data['x_mean'] = self.position_plot.GetMean(1)
    ins_data['y_mean'] = self.position_plot.GetMean(2)
    ins_data['px_mean'] = self.momentum_plot.GetMean(1)
    ins_data['py_mean'] = self.momentum_plot.GetMean(2)
    ins_data['x_rms'] = self.position_plot.GetRMS(1)
    ins_data['y_rms'] = self.position_plot.GetRMS(2)
    ins_data['px_rms'] = self.momentum_plot.GetRMS(1)
    ins_data['py_rms'] = self.momentum_plot.GetRMS(2)

    number = self.covariance.length()
    if number > 1 :
      ins_data['emittance'] = self.covariance.get_emittance(['x', 'px', 'y', 'py'])
      ins_data['emittance_x'] = self.covariance.get_emittance(['x', 'px'])
      ins_data['emittance_y'] = self.covariance.get_emittance(['y', 'py'])
      ins_data['beta'] = self.covariance.get_beta(['x', 'y'])
      ins_data['beta_x'] = self.covariance.get_beta(['x'])
      ins_data['beta_y'] = self.covariance.get_beta(['y'])
      ins_data['alpha'] = self.covariance.get_alpha(['x', 'y'])
      ins_data['alpha_x'] = self.covariance.get_alpha(['x'])
      ins_data['alpha_y'] = self.covariance.get_alpha(['y'])
      ins_data['momentum'] = self.p_plot.GetMean()
      ins_data['number_particles'] = number

      ins_data['emittance_error'] = ins_data['emittance'] / math.sqrt(2.0*(number - 1))
      ins_data['emittance_x_error'] = ins_data['emittance_x'] / math.sqrt(number - 1)
      ins_data['emittance_y_error'] = ins_data['emittance_y'] / math.sqrt(number - 1)
      ins_data['beta_error'] = 0.0
      ins_data['beta_x_error'] = 0.0
      ins_data['beta_y_error'] = 0.0
      ins_data['alpha_error'] = 0.0
      ins_data['alpha_x_error'] = 0.0
      ins_data['alpha_y_error'] = 0.0
      ins_data['momentum_error'] = self.p_plot.GetRMS() / math.sqrt(number)

      cov = self.covariance.get_covariance_matrix(['x', 'px', 'y', 'py'])

    else :
      ins_data['emittance'] = 0.0
      ins_data['emittance_x'] = 0.0
      ins_data['emittance_y'] = 0.0
      ins_data['beta'] = 0.0
      ins_data['beta_x'] = 0.0
      ins_data['beta_y'] = 0.0
      ins_data['alpha'] = 0.0
      ins_data['alpha_x'] = 0.0
      ins_data['alpha_y'] = 0.0
      ins_data['momentum'] = 0.0
      ins_data['number_particles'] = 0

      ins_data['emittance_error'] = 0.0
      ins_data['emittance_x_error'] = 0.0
      ins_data['emittance_y_error'] = 0.0
      ins_data['beta_error'] = 0.0
      ins_data['beta_x_error'] = 0.0
      ins_data['beta_y_error'] = 0.0
      ins_data['alpha_error'] = 0.0
      ins_data['alpha_x_error'] = 0.0
      ins_data['alpha_y_error'] = 0.0
      ins_data['momentum_error'] = 0.0

      cov = [ [ 0.0 for i in range(4) ] for j in range(4) ]

    ins_data['covariance_matrix'] = [ [ cov[i][j] for i in range(4) ] for j in range(4) ]


    return ins_data


####################################################################################################


class CoolingInspector() :

  def __init__(self) :
    self.position = 0.0
    self.number_particles = 0

#    self.momentum_bias = None
#    self.momentum_correlation = None

    self.upstream_parent_covariance = None
    self.upstream_parent_covariance_inv = None
    self.upstream_parent_emittance = None

    self.downstream_parent_covariance = None
    self.downstream_parent_covariance_inv = None
    self.downstream_parent_emittance = None

    self.upstream_amplitude_plot = ROOT.TH1F(\
                 'upstream_reference_amplitude', 'Amplitude', 200, 0.0, 100.0)
    self.downstream_amplitude_plot = ROOT.TH1F(\
                'downstream_reference_amplitude', 'Amplitude', 200, 0.0, 100.0)

    self.upstream_amplitude_transmitted_plot = ROOT.TH1F(\
                 'upstream_reference_amplitude_transmitted', 'Amplitude', 200, 0.0, 100.0)
    self.downstream_amplitude_transmitted_plot = ROOT.TH1F(\
                'downstream_reference_amplitude_transmitted', 'Amplitude', 200, 0.0, 100.0)

    self.upstream_amplitude_scraped_plot = ROOT.TH1F(\
                 'upstream_reference_amplitude_scraped', 'Amplitude', 200, 0.0, 100.0)
    self.downstream_amplitude_scraped_plot = ROOT.TH1F(\
                'downstream_reference_amplitude_scarped', 'Amplitude', 200, 0.0, 100.0)

    self.amplitude_change_plot = ROOT.TH1F(\
          'reference_amplitude_change', 'Amplitude Change', 200, -100.0, 100.0)
    self.amplitude_change_amplitude_plot = ROOT.TH2F(\
                  'reference_amplitude_change_amplitude', 'Amplitude Change', \
                                          200, 0.0, 100.0, 200, -100.0, 100.0 )
    self.amplitude_mobility_plot = ROOT.TH2F(\
                        'reference_amplitude_mobility', 'Amplitude Mobility', \
                                             100, 0.0, 100.0, 100, 0.0, 100.0 )


#  def set_covariance_correction(self, correction, axes=['x', 'px', 'y', 'py']) :
#    self.covariance.set_covariance_correction( correction, axes )
#    self.ensemble_covariance.set_covariance_correction( correction, axes )


  def set_upstream_covariance(self, matrix) :
    self.upstream_parent_covariance = numpy.array(matrix)
    self.upstream_parent_covariance_inv = numpy.linalg.inv(self.upstream_parent_covariance)
    self.upstream_parent_emittance = analysis.covariances.emittance_from_matrix(self.upstream_parent_covariance)


  def set_downstream_covariance(self, matrix) :
    self.downstream_parent_covariance = numpy.array(matrix)
    self.downstream_parent_covariance_inv = numpy.linalg.inv(self.downstream_parent_covariance)
    self.downstream_parent_emittance = analysis.covariances.emittance_from_matrix(self.downstream_parent_covariance)


#  def set_momentum_correction(self, constant, gradient) :
#    self.momentum_bias = constant
#    self.momentum_correlation = gradient


  def add_hits(self, up_hit, down_hit) :
#    if self.momentum_bias is not None :
#      p = hit.get_p()
##      correction = 1.0 + (self.momentum_bias + self.momentum_correlation*p) / p
#      correction = ((p - self.momentum_bias) / \
#                                         (1.0 - self.momentum_correlation)) / p
#      hit.set_px(hit.get_px()*correction)
#      hit.set_py(hit.get_py()*correction)
#      hit.set_pz(hit.get_pz()*correction)

    if (self.upstream_parent_covariance_inv is not None) and (self.downstream_parent_covariance_inv is not None) :
      self.number_particles += 1

      if up_hit is not None :
        vector = numpy.array(up_hit.get_as_vector()[2:6])
        up_amplitude = self.upstream_parent_emittance*vector.transpose().dot(self.upstream_parent_covariance_inv.dot(vector))
        self.upstream_amplitude_plot.Fill(up_amplitude)
        
        if down_hit is None :
          self.upstream_amplitude_scraped_plot.Fill(up_amplitude)
        else :
          self.upstream_amplitude_transmitted_plot.Fill(up_amplitude)

      if down_hit is not None :
        vector = numpy.array(down_hit.get_as_vector()[2:6])
        down_amplitude = self.downstream_parent_emittance*vector.transpose().dot(self.downstream_parent_covariance_inv.dot(vector))
        self.downstream_amplitude_plot.Fill(down_amplitude)

        if up_hit is None :
          self.downstream_amplitude_scraped_plot.Fill(down_amplitude)
        else :
          self.downstream_amplitude_transmitted_plot.Fill(down_amplitude)


      if (up_hit is not None) and (down_hit is not None) :
        DA = down_amplitude - up_amplitude
        self.amplitude_change_plot.Fill( DA )
        self.amplitude_change_amplitude_plot.Fill( up_amplitude, DA )
        self.amplitude_mobility_plot.Fill( up_amplitude, down_amplitude )


  def fill_plots(self) :
    pass


  def get_length(self) :
    return self.number_particles


  def get_plot_dictionary(self) :
    ins_plots = {}

    ins_plots['upstream_amplitude'] = self.upstream_amplitude_plot
    ins_plots['downstream_amplitude'] = self.downstream_amplitude_plot
    ins_plots['upstream_amplitude_transmitted'] = self.upstream_amplitude_transmitted_plot
    ins_plots['downstream_amplitude_transmitted'] = self.downstream_amplitude_transmitted_plot
    ins_plots['upstream_amplitude_scraped'] = self.upstream_amplitude_scraped_plot
    ins_plots['downstream_amplitude_scraped'] = self.downstream_amplitude_scraped_plot

    ins_plots['amplitude_change'] = self.amplitude_change_plot
    ins_plots['amplitude_change_amplitude'] = self.amplitude_change_amplitude_plot
    ins_plots['amplitude_mobility'] = self.amplitude_mobility_plot

    return ins_plots


  def get_data_dictionary(self) :
    ins_data = {}

    return ins_data

