#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""
Handler for making plots of beam envelopes based on a list of ellipses and
a list of Hits (representing the reference trajectory).
"""

import math
import copy
import numpy

from xboa.Bunch import Bunch
import xboa.Common as Common

from plot_setup import PlotSetup # pylint: disable=W0403

class Plotter:
    """
    Plotter makes a histogram and a list of graphs and Draws them on the 
    specified canvas.

    Superimpose apertures over the plot - for envelope and mean "x" and "y" the
    physical apertures are superimposed. For everything else, apertures are
    pushed into the top of the canvas (making a reference z trajectory).
    """
    def __init__(self, plot_options, canvas, #pylint: disable=R0913
                 ref_list, ellipse_list, magnets):
        """
        Plot a graph, specified by plot_options, on canvas. 

        Plot data is taken from ref_list (for the reference trajectory), 
        ellipse_list (for the beam envelope) and magnets (for apertures)

        - plot_options: map specifying variable type (should be a class)
        - canvas: ROOT canvas where plots will be made
        - ref_list: list of reference hits makes the reference trajectory
        - ellipse_list: list of beam ellipses defines the envelope
        - magnets: list of magnets for plotting apertures
        """
        self.ref_list = ref_list
        self.ellipse_list = ellipse_list
        self.var_type = PlotSetup.get_variable_type(plot_options)
        self.first_var = PlotSetup.get_first_var(plot_options)
        self.x_var = []
        self.y_var = []
        self.magnets = magnets
        try:
            if self.var_type != "mean" and len(self.ellipse_list) <= 1:
                print "Failed to propagate ellipses"
            else:
                self.set_variables_function()()
        except KeyError:
            print "Did not recognise plot option "+str(self.var_type)
        if plot_options[0]["plot_apertures"]:
            self.get_apertures()
        plot_name = self.var_type+":"+self.first_var
        canvas.cd()
        hist, graph_list = Common.make_root_multigraph(plot_name,
                                    self.x_var, "z [mm]", self.y_var, plot_name)
        hist.Draw()
        for graph in graph_list:
            graph.SetLineColor(1)
            graph.Draw('l')
        canvas.Update()

    def get_apertures(self):
        """
        Convert from magnet to a set of apertures and add them to the list
        of graphs
        """
        if len(self.magnets) == 0:
            return
        axis = self._axis()
        magnet_x, magnet_y = [], []
        for a_magnet in self.magnets:
            x_list = [
                a_magnet["aperture"][axis],
                a_magnet["aperture"][axis],
                a_magnet["outer"][axis],
                a_magnet["outer"][axis],
                a_magnet["aperture"][axis],
            ]
            if self.position_is_start_dict[a_magnet["field_type"]]:
                z_list = [
                    0.,
                    +a_magnet["aperture"]["z"],
                    +a_magnet["outer"]["z"],
                    0.,
                    0.,
                ]
            else:
                z_list = [
                    -a_magnet["aperture"]["z"]/2.,
                    +a_magnet["aperture"]["z"]/2.,
                    +a_magnet["outer"]["z"]/2.,
                    -a_magnet["outer"]["z"]/2.,
                    -a_magnet["aperture"]["z"]/2.,
                ]
            if self._show_physical_apertures(): # as in physical scalings
                z_pair, x_pair = self._transform(z_list, x_list, a_magnet)
                magnet_x += z_pair
                magnet_y += x_pair
            else:
                z_list = [z_pos+a_magnet["position"]["z"] for z_pos in z_list]
                magnet_x.append(z_list)
                magnet_y.append(x_list)
        if not self._show_physical_apertures(): # non-physical scalings
            self._get_scaled_apertures(magnet_y, axis)
        self.x_var += magnet_x
        self.y_var += magnet_y

    def _axis(self):
        """
        Return the axis to be used for plotting magnet apertures
        """
        if self.first_var in ["x", "y"]:
            return self.first_var
        else:
            return "x"

    def _transform(self, points_x, points_y, a_magnet):
        """
        Rotate magnet aperture boxes
        
        For plotting rotated physical apertures
        """
        axis = self._axis()
        phi = a_magnet["rotation"][{"x":"y", "y":"x"}[axis]]
        translation = [a_magnet["position"]["z"], a_magnet["position"][axis]]
        list_points_x = [points_x, copy.deepcopy(points_x)]
        list_points_y = [points_y, copy.deepcopy(points_y)]
        list_points_y[1] = [-y for y in list_points_y[1]]
        for i in range(2):
            for j in range(len(points_x)):
                x_temp = list_points_x[i][j]*math.cos(phi)+\
                                               list_points_y[i][j]*math.sin(phi)
                y_temp = list_points_y[i][j]*math.cos(phi)-\
                                               list_points_x[i][j]*math.sin(phi)
                list_points_x[i][j] = x_temp+translation[0]
                list_points_y[i][j] = y_temp+translation[1]
        return list_points_x, list_points_y

    def get_means(self):
        """Fill data for mean from ref_list and ellipse_list"""
        self.y_var = [[reference[self.first_var] \
                                                for reference in self.ref_list]]
        self.x_var = [[reference['z'] for reference in self.ref_list]]

    def get_rms(self):
        """Fill data for RMS from ref_list and ellipse_list"""
        my_var = self.ellipse_var.index(self.first_var)+1
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[ell.get_element(my_var, my_var)**0.5 \
                                                  for ell in self.ellipse_list]]

    def get_envelope(self):
        """
        Fill data for envelope from ref_list and ellipse_list

        Envelope has a positive element and a negative element
        """
        self.get_rms()
        self.x_var *= 2
        rms_list = self.y_var[0]
        self.y_var = [range(len(rms_list)), range(len(rms_list))]
        for i, rms in enumerate(rms_list):
            if i < len(self.ref_list):
                ref_var = self.ref_list[i][self.first_var]
                self.y_var[0][i] = ref_var-rms
                self.y_var[1][i] = ref_var+rms

    def get_beta(self):
        """Fill data for optical beta from ref_list and ellipse_list"""
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            ref = self.ref_list[i]
            beta = 0
            axes = self.axis_dict[self.first_var]
            axes = [self.ellipse_var.index(var)+1 for var in axes]
            for axis in axes:
                beta += ell.get_element(axis, axis)/float(len(axes))
            beta /= ref['mass']*self._get_emittance(ref, ell)/ref['p']
            self.y_var[0].append(beta)

    def get_alpha(self):
        """Fill data for alpha from ref_list and ellipse_list"""
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            ref = self.ref_list[i]
            alpha = 0
            axes = self.axis_dict[self.first_var]
            axes = [self.ellipse_var.index(var)+1 for var in axes]
            for axis in axes:
                alpha -= ell.get_element(axis+1, axis)/float(len(axes))
            alpha /= ref['mass']*self._get_emittance(ref, ell)
            self.y_var[0].append(alpha)

    def get_gamma(self):
        """Fill data for optical gamma from ref_list and ellipse_list"""
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            ref = self.ref_list[i]
            gamma = 0
            axes = self.axis_dict[self.first_var]
            axes = [self.ellipse_var.index(var)+1 for var in axes]
            for axis in axes:
                gamma -= ell.get_element(axis+1, axis+1)/float(len(axes))
            gamma /= ref['mass']*self._get_emittance(ref, ell)*ref['p']
            self.y_var[0].append(gamma)

    def get_emittance(self):
        """Fill data for emittance from ref_list and ellipse_list"""
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            self.y_var[0].append(self._get_emittance(self.ref_list[i], ell))

    def get_dispersion(self):
        """Fill data for D from ref_list and ellipse_list"""
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            energy = self.ref_list[i]['energy']
            axis_str = self.axis_dict[self.first_var][0]
            axis_int = self.ellipse_var.index(axis_str)+1
            disp = ell.get_element(2, axis_int)*energy/ell.get_element(2, 2)
            self.y_var[0].append(disp)

    def get_dispersion_prime(self):
        """Fill data for D' from ref_list and ellipse_list"""
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            energy = self.ref_list[i]['energy']
            axis_str = self.axis_dict[self.first_var][0]
            axis_int = self.ellipse_var.index(axis_str)+2
            disp_p = ell.get_element(2, axis_int)*energy/ell.get_element(2, 2)
            self.y_var[0].append(disp_p)

    def _get_emittance(self, ref, ellipse):
        """Get the beam emittance for given reference hit and ellipse"""
        axis_list = self.axis_dict[self.first_var]
        el_list = self.el_dict[self.first_var]
        bunch = Bunch.new_from_hits([ref])
        cov_matrix = [None]*len(el_list)
        for cov_i, ell_i in enumerate(el_list):
            cov_matrix[cov_i] = [None]*len(el_list)
            for cov_j, ell_j in enumerate(el_list):
                cov_matrix[cov_i][cov_j] = ellipse.get_element(ell_i, ell_j)
        cov_matrix = numpy.array(cov_matrix)
        return bunch.get_emittance(axis_list, cov_matrix)

    def _show_physical_apertures(self):
        """
        Return true if we should show physical apertures; return false if we
        should show scaled apertures
        """
        return self.var_type in ["envelope", "mean", "<Select plot type>"] and \
               self.first_var in ["x", "y", ""]

    def _get_scaled_apertures(self, magnet_y, axis):
        """
        Fill graph data with apertures, scaled to occupy only the top 10% of the
        canvas
        """
        graph_max = max([max(y_temp) for y_temp in self.y_var])
        graph_min = min([min(y_temp) for y_temp in self.y_var])
        graph_delta = max([max(y_temp) for y_temp in self.y_var])-\
                      graph_min
        if graph_delta == 0.:
            graph_delta = 1.
        field_min = min([a_magnet["aperture"][axis] \
                                                 for a_magnet in self.magnets])
        field_max = max([a_magnet["outer"][axis] for a_magnet in self.magnets])
        field_delta = field_max-field_min
        if field_delta == 0.:
            field_delta = 1.
        scaling = self.scale_factor*graph_delta/field_delta
        offset = (graph_max-field_min*scaling)+graph_delta*self.stay_clear
        for this_magnet_y in magnet_y:
            for i in range(5):
                this_magnet_y[i] = this_magnet_y[i]*scaling+offset
        return magnet_y

    def set_variables_function(self):
        """
        Return a function that will extract graph data from beam ellipse and
        reference particle for self,var_type
        """
        plot_options_dict = {
            "mean":self.get_means,
            "envelope":self.get_envelope,
            "RMS":self.get_rms,
            "beta":self.get_beta,
            "alpha":self.get_alpha,
            "gamma":self.get_gamma,
            "emittance":self.get_emittance,
            "dispersion":self.get_dispersion,
            "dispersion_prime":self.get_dispersion_prime
        }
        return plot_options_dict[self.var_type]

    ellipse_var = ['t', 'energy', 'x', 'px', 'y', 'py']
    axis_dict = {'x':['x'], 'y':['y'], 'transverse':['x', 'y'],
                 'longitudinal':['t']}
    el_dict = {'x':range(3, 5), 'y':range(5, 7), 'transverse':range(3, 7),
               'longitudinal':range(1, 3)}
    scale_factor = 0.2
    stay_clear = 0.05

    position_is_start_dict = {
        "Multipole":True,
        "Solenoid":False
    }

