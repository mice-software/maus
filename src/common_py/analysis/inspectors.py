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
# pylint: disable = W0102


import analysis
import analysis.covariances

import ROOT


class PhaseSpace2DInspector() :

  def __init__(self, plane_id, ensemble_size) :
    self.plane = plane_id
    self.position = 0.0
    self.ensemble_size = ensemble_size
    self.covariance = analysis.covariances.CovarianceMatrix()

    self.position_plot = ROOT.TH2F(\
           'inspected_position_{0}'.format(self.plane), 'Beam Position', \
                                        400, -200.0, 200.0, 400, -200.0, 200.0)
    self.momentum_plot = ROOT.TH2F(\
           'inspected_momentum_{0}'.format(self.plane), 'Beam Momentum', \
                                        400, -200.0, 200.0, 400, -200.0, 200.0)
    self.x_phasespace_plot = ROOT.TH2F(\
        'inspected_x_phasespace_{0}'.format(self.plane), 'X-Phasespace', \
                                        400, -200.0, 200.0, 400, -200.0, 200.0)
    self.y_phasespace_plot = ROOT.TH2F(\
        'inspected_y_phasespace_{0}'.format(self.plane), 'Y-Phasespace', \
                                        400, -200.0, 200.0, 400, -200.0, 200.0)
    self.xy_phasespace_plot = ROOT.TH2F(\
        'inspected_xpy_phasespace_{0}'.format(self.plane), 'X-Py-Phasespace', \
                                        400, -200.0, 200.0, 400, -200.0, 200.0)
    self.yx_phasespace_plot = ROOT.TH2F(\
        'inspected_ypx_phasespace_{0}'.format(self.plane), 'Y-Px-Phasespace', \
                                        400, -200.0, 200.0, 400, -200.0, 200.0)

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
                'inspected_momentum_{0}'.format(self.plane), 'Momentum', \
                                                              1000, 0.0, 500.0)


  def set_covariance_correction(self, correction, axes=['x', 'px', 'y', 'py']) :
    self.covariance.set_covariance_correction( correction, axes )


  def add_hit(self, hit) :
    self.covariance.add_hit(hit)

    self.position_plot.Fill(hit.get_x(), hit.get_y())
    self.momentum_plot.Fill(hit.get_px(), hit.get_py())
    self.x_phasespace_plot.Fill(hit.get_x(), hit.get_px())
    self.y_phasespace_plot.Fill(hit.get_y(), hit.get_py())
    self.xy_phasespace_plot.Fill(hit.get_x(), hit.get_py())
    self.yx_phasespace_plot.Fill(hit.get_y(), hit.get_px())
    self.pz_plot.Fill(hit.get_pz())
    self.p_plot.Fill(hit.get_p())
    self.position = hit.get_z()

    if self.ensemble_size > 1 and \
                               self.covariance.length() > self.ensemble_size :
      self.fill_plots()


  def fill_plots(self) :
    if self.covariance.length() < (self.ensemble_size / 2) :
      return

    self.emittance_plot.Fill(self.covariance.get_emittance(\
                                                          ['x','px','y','py']))
    self.emittance_x_plot.Fill(self.covariance.get_emittance(['x','px']))
    self.emittance_y_plot.Fill(self.covariance.get_emittance(['y','py']))
    self.alpha_plot.Fill(self.covariance.get_alpha(['x','y']))
    self.alpha_x_plot.Fill(self.covariance.get_alpha(['x']))
    self.alpha_y_plot.Fill(self.covariance.get_alpha(['y']))
    self.beta_plot.Fill(self.covariance.get_beta(['x','y']))
    self.beta_x_plot.Fill(self.covariance.get_beta(['x']))
    self.beta_y_plot.Fill(self.covariance.get_beta(['y']))
    self.total_momentum_plot.Fill(self.covariance.get_momentum())
  
    self.covariance.clear()


  def get_current_ensemble_size(self) :
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

    return ins_plots


  def get_data_dictionary(self) :
    ins_data = {}
    ins_data['position'] = self.position
    ins_data['x_mean'] = self.position_plot.GetMean(1)
    ins_data['y_mean'] = self.position_plot.GetMean(2)
    ins_data['px_mean'] = self.momentum_plot.GetMean(1)
    ins_data['py_mean'] = self.momentum_plot.GetMean(2)
    ins_data['emittance_mean'] = self.emittance_plot.GetMean()
    ins_data['emittance_x_mean'] = self.emittance_x_plot.GetMean()
    ins_data['emittance_y_mean'] = self.emittance_y_plot.GetMean()
    ins_data['beta_mean'] = self.beta_plot.GetMean()
    ins_data['beta_x_mean'] = self.beta_x_plot.GetMean()
    ins_data['beta_y_mean'] = self.beta_y_plot.GetMean()
    ins_data['alpha_mean'] = self.alpha_plot.GetMean()
    ins_data['alpha_x_mean'] = self.alpha_x_plot.GetMean() 
    ins_data['alpha_y_mean'] = self.alpha_y_plot.GetMean()
    ins_data['momentum_mean'] = self.momentum_plot.GetMean()

    ins_data['x_rms'] = self.position_plot.GetRMS(1)
    ins_data['y_rms'] = self.position_plot.GetRMS(2)
    ins_data['px_rms'] = self.momentum_plot.GetRMS(1)
    ins_data['py_rms'] = self.momentum_plot.GetRMS(2)
    ins_data['emittance_rms'] = self.emittance_plot.GetRMS()
    ins_data['emittance_x_rms'] = self.emittance_x_plot.GetRMS()
    ins_data['emittance_y_rms'] = self.emittance_y_plot.GetRMS()
    ins_data['beta_rms'] = self.beta_plot.GetRMS()
    ins_data['beta_x_rms'] = self.beta_x_plot.GetRMS()
    ins_data['beta_y_rms'] = self.beta_y_plot.GetRMS()
    ins_data['alpha_rms'] = self.alpha_plot.GetRMS()
    ins_data['alpha_x_rms'] = self.alpha_x_plot.GetRMS() 
    ins_data['alpha_y_rms'] = self.alpha_y_plot.GetRMS()
    ins_data['momentum_rms'] = self.momentum_plot.GetRMS()

    return ins_data


