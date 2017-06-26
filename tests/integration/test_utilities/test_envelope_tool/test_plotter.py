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

"""Tests for plotter class"""

import os
import sys  
import math
import unittest
import ROOT
from xboa.hit import Hit
import xboa.common as Common

import maus_cpp.covariance_matrix

ENV_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/bin/utilities/envelope_tool/lib")
sys.path.append(ENV_DIR)
import plotter # pylint: disable=F0401

# pylint: disable=E1101

class PlotterTest(unittest.TestCase): # pylint: disable=C0103, R0904
    """Tests for plotter class"""
    def setUp(self): # pylint: disable=C0103
        ref = Hit.new_from_dict({'pid':13, 'px':1., 'py':2., 'pz':3.,
                                'x':-1, 'y':-2, 'z':-3,
                                'mass':Common.pdg_pid_to_mass[13],
                                'charge':Common.pdg_pid_to_charge[13]},
                                'energy')
        self.ref_list = [ref]*4

        self.p_dict = {
          'e_t':1.,
          'b_t':2.,
          'e_l':3.,
          'b_l':4.,
          'a_t':5.,
          'a_l':6.,
          'l_t':7.,
          'd_x':8.,
          'dp_x':9.,
          'd_y':10.,
          'dp_y':11.,
        }
        self.penn_ellipses = [
            maus_cpp.covariance_matrix.create_from_penn_parameters(
                mass = ref['mass'],
                momentum = ref['p'],
                emittance_t = self.p_dict['e_t']+i*12.,
                beta_t = self.p_dict["b_t"]+i*12.,
                emittance_l = self.p_dict["e_l"]+i*12.,
                beta_l = self.p_dict["b_l"]+i*12.,
                alpha_t = self.p_dict["a_t"]+i*12.,
                alpha_l = self.p_dict["a_l"]+i*12.,
                charge = ref["charge"],
                bz = 0.,
                ltwiddle = self.p_dict["l_t"]+i*12.,
                dispersion_x = self.p_dict["d_x"]+i*12.,
                dispersion_prime_x = self.p_dict["dp_x"]+i*12.,
                dispersion_y = self.p_dict["d_y"]+i*12.,
                dispersion_prime_y = self.p_dict["dp_y"]+i*12.,
            ) for i in range(3)]
        self.t_dict = {
          'e_x':1.,
          'b_x':2.,
          'e_y':3.,
          'b_y':4.,
          'e_l':5.,
          'b_l':6.,
          'a_x':7.,
          'a_y':8.,
          'a_l':9.,
          'd_x':10.,
          'dp_x':11.,
          'd_y':12.,
          'dp_y':13.,
        }
        self.twiss_ellipses = [
            maus_cpp.covariance_matrix.create_from_twiss_parameters(
                mass = ref['mass'],
                momentum = ref['p'],
                emittance_x = self.t_dict['e_x']+i*14.,
                beta_x = self.t_dict["b_x"]+i*14.,
                emittance_y = self.t_dict['e_y']+i*14.,
                beta_y = self.t_dict["b_y"]+i*14.,
                emittance_l = self.t_dict["e_l"]+i*14.,
                beta_l = self.t_dict["b_l"]+i*14.,
                alpha_x = self.t_dict["a_x"]+i*14.,
                alpha_y = self.t_dict["a_y"]+i*14.,
                alpha_l = self.t_dict["a_l"]+i*14.,
                dispersion_x = self.t_dict["d_x"]+i*14.,
                dispersion_prime_x = self.t_dict["dp_x"]+i*14.,
                dispersion_y = self.t_dict["d_y"]+i*14.,
                dispersion_prime_y = self.t_dict["dp_y"]+i*14.,
            ) for i in range(3)]

        self.field_list = [{
          'field_type': 'Multipole',
          'position': {'x': 5.0, 'y': 4.0, 'z': 6.0},
          'aperture': {'x': 8.0, 'y': 7.0, 'z': 9.0},
          'outer': {'x': 10.0, 'y': 11.0, 'z': 12.0},
          'scale_factor': -0.5,
          'rotation': {'x':math.pi/6., 'y': 0.0, 'z': 0.0},
          'field_name': 'D1'
        }, {
          'field_type': 'Solenoid',
          'position': {'x': 5.0, 'y': 4.0, 'z': 6.0},
          'aperture': {'x': 8.0, 'y': 7.0, 'z': 9.0},
          'outer': {'x': 11.0, 'y': 12.0, 'z': 13.0},
          'scale_factor': 0.5,
          'rotation': {'x':0.0, 'y': math.pi/6., 'z': 0.0},
          'field_name': 'D2'
        }]

    def _test_graph(self, ref_x, test_x):
        """Compare two lists of floats and check they are almost equal"""
        self.assertEqual(len(ref_x), len(test_x))
        for i, item in enumerate(ref_x):
            mess = str(i)+', '+str(ref_x)+', '+str(test_x)
            self.assertAlmostEqual(item, test_x[i], msg=mess)

    def test_physical_apertures(self):
        """Test envelope_tool.plotter physical apertures"""
        option = [{"variable_type":0, "first_var":0, "plot_apertures":False}]
        canvas = ROOT.TCanvas("a", "b")
        my_plot = plotter.Plotter(option, canvas, [], [], self.field_list)
        self.assertEqual(len(my_plot.x_var), 0)
        self.assertEqual(len(my_plot.y_var), 0)
        option = [{"variable_type":0, "first_var":0, "plot_apertures":True}]
        my_plot = plotter.Plotter(option, canvas, [], [], self.field_list)
        self.assertEqual(len(my_plot.x_var), 4)
        self.assertEqual(len(my_plot.y_var), 4)
        for i in range(1, 3):
            for j in range(1, 3):
                option = [{"variable_type":i,
                           "first_var":j, "plot_apertures":True}]
                my_plot = plotter.Plotter(option, canvas, [], [],
                                          self.field_list)
                self.assertEqual(len(my_plot.x_var), 4)
                self.assertEqual(len(my_plot.y_var), 4)
        option = [{"variable_type":1, "first_var":1, "plot_apertures":True}]
        my_plot = plotter.Plotter(option, canvas, [], [], self.field_list)
        self._test_graph(my_plot.x_var[0], [6.0, 15.0, 18.0, 6.0, 6.0])
        self._test_graph(my_plot.y_var[0], [13.0, 13.0, 15.0, 15.0, 13.0])
        self._test_graph(my_plot.x_var[1], my_plot.x_var[0])
        self._test_graph(my_plot.y_var[1], [-3.0, -3.0, -5.0, -5.0, -3.0])
        option = [{"variable_type":1, "first_var":2, "plot_apertures":True}]
        my_plot = plotter.Plotter(option, canvas, [], [], self.field_list)
        test_x = [6.0+math.sin(math.pi/6.)*7.0,
                  6.0+math.sin(math.pi/6.)*7.0+math.cos(math.pi/6.)*9.0,
                  6.0+math.sin(math.pi/6.)*11.0+math.cos(math.pi/6.)*12.0,
                  6.0+math.sin(math.pi/6.)*11.0,
                  6.0+math.sin(math.pi/6.)*7.0]
        self._test_graph(my_plot.x_var[0], test_x)
        test_x = [6.0-math.sin(math.pi/6.)*7.0,
                  6.0-math.sin(math.pi/6.)*7.0+math.cos(math.pi/6.)*9.0,
                  6.0-math.sin(math.pi/6.)*11.0+math.cos(math.pi/6.)*12.0,
                  6.0-math.sin(math.pi/6.)*11.0,
                  6.0-math.sin(math.pi/6.)*7.0]
        self._test_graph(my_plot.x_var[1], test_x)
        test_y = [4.0+math.cos(math.pi/6.)*7.0,
                  4.0+math.cos(math.pi/6.)*7.0-math.sin(math.pi/6.)*9.0,
                  4.0+math.cos(math.pi/6.)*11.0-math.sin(math.pi/6.)*12.0,
                  4.0+math.cos(math.pi/6.)*11.0,
                  4.0+math.cos(math.pi/6.)*7.0]
        self._test_graph(my_plot.y_var[0], test_y)
        test_y = [4.0-math.cos(math.pi/6.)*7.0,
                  4.0-math.cos(math.pi/6.)*7.0-math.sin(math.pi/6.)*9.0,
                  4.0-math.cos(math.pi/6.)*11.0-math.sin(math.pi/6.)*12.0,
                  4.0-math.cos(math.pi/6.)*11.0,
                  4.0-math.cos(math.pi/6.)*7.0]
        self._test_graph(my_plot.y_var[1], test_y)
        # don't test solenoid in rotation
        self._test_graph(my_plot.x_var[2], [1.5, 10.5, 12.5, -0.5, 1.5])
        self._test_graph(my_plot.y_var[2], [11.0, 11.0, 16.0, 16.0, 11.0])
        self._test_graph(my_plot.x_var[3], my_plot.x_var[2])
        self._test_graph(my_plot.y_var[3], [-3.0, -3.0, -8.0, -8.0, -3.0])

    def test_scaled_apertures(self):
        """Test envelope_tool.plotter scaled apertures"""
        option = [{"variable_type":3, "first_var":1, "plot_apertures":False}]
        canvas = ROOT.TCanvas("c", "d")
        my_plot = plotter.Plotter(option, canvas, [], [], self.field_list)
        self.assertEqual(len(my_plot.x_var), 0)
        self.assertEqual(len(my_plot.y_var), 0)
        option = [{"variable_type":3, "first_var":1, "plot_apertures":True}]
        my_plot = plotter.Plotter(option, canvas, [], [], self.field_list)
        self.assertEqual(len(my_plot.x_var), 2)
        self.assertEqual(len(my_plot.y_var), 2)
        my_plot = plotter.Plotter(option, canvas, [0.], [0.], self.field_list)
        self.assertEqual(len(my_plot.x_var), 2)
        self.assertEqual(len(my_plot.y_var), 2)
        self._test_graph(my_plot.x_var[0], [6.0, 15.0, 18.0, 6.0, 6.0])
        self._test_graph(my_plot.x_var[1], [1.5, 10.5, 12.5, -0.5, 1.5])
        for i in [0, 1]:
            self.assertAlmostEqual(my_plot.y_var[i][0], my_plot.y_var[i][1])
            self.assertGreater(my_plot.y_var[i][2], my_plot.y_var[i][1])
            self.assertAlmostEqual(my_plot.y_var[i][2], my_plot.y_var[i][3])
            self.assertAlmostEqual(my_plot.y_var[i][0], my_plot.y_var[i][4])

    # def test_physics(self):
    #    """Test envelope_tool.plotter physics calculations"""
    #    # please note #1377 - dispersion sign appears to be incorrect in
    #    # CovarianceMatrix::CreateFrom<Blah>Parameters(...)
    #    canvas = ROOT.TCanvas("c", "d")
    #    for data in [(4, 4, 'b_l'), (4, 3, 'b_t'),
    #                 (5, 4, 'a_l'), (5, 3, 'a_t'), 
    #                (7, 4, 'e_l'), (7, 3, 'e_t'),
    #                 (8, 1, 'd_x'), (9, 1, 'dp_x'),
    #                 (8, 2, 'd_y'), (9, 2, 'dp_y'),
    #         ]:
    #        option = [{"variable_type":data[0], "first_var":data[1],
    #                  "plot_apertures":True}]
    #        my_plot = plotter.Plotter(option, canvas,
    #                                  self.ref_list, self.penn_ellipses, [])
    #        self._test_graph(my_plot.x_var[0],
    #                         [ref['z'] for ref in self.ref_list[0:3]])
    #        self._test_graph(my_plot.y_var[0],
    #                         [self.p_dict[data[2]]+i*12. for i in range(3)])

    #    for ellipse in self.twiss_ellipses:
    #        print ellipse
    #    for data in [(4, 1, 'b_x'), (4, 2, 'b_y'), (4, 4, 'b_l'),
    #                 (5, 1, 'a_x'), (5, 2, 'a_y'), (5, 4, 'a_l'),
    #                 (7, 1, 'e_x'), (7, 2, 'e_y'), (7, 4, 'e_l'),
    #                 (9, 1, 'dp_x'), #(8, 1, 'd_x'),
    #                 (9, 2, 'dp_y'), #(8, 2, 'd_y'),
    #         ]:
    #        print data
    #        option = [{"variable_type":data[0], "first_var":data[1],
    #                  "plot_apertures":True}]
    #        my_plot = plotter.Plotter(option, canvas,
    #                                  self.ref_list, self.twiss_ellipses, [])
    #        self._test_graph(my_plot.x_var[0],
    #                         [ref['z'] for ref in self.ref_list[0:3]])
    #        self._test_graph(my_plot.y_var[0],
    #                         [self.t_dict[data[2]]+i*14. for i in range(3)])

if __name__ == "__main__":
    unittest.main()
