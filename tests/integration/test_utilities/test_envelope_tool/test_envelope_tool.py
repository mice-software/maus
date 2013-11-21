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
Test for the envelope_tool GUI object
"""

import math
import time
import sys
import signal
import subprocess
import os
import unittest
import ROOT

from xboa.Hit import Hit
import xboa.Common as Common
import maus_cpp.covariance_matrix
from maus_cpp.covariance_matrix import CovarianceMatrix


ENV_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/bin/utilities/envelope_tool/")
sys.path.append(ENV_DIR)
sys.path.append(ENV_DIR+"lib/")
TEST_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/tests/"+\
                               "integration/test_utilities/test_envelope_tool/")
sys.argv = sys.argv[0:1]

import envelope_tool # pylint: disable=F0401
import lattice # pylint: disable=F0401

def _ell_equal(ellipse_1, ellipse_2):
    """Check that ellipse_1 and ellipse_2 are almost equal"""
    for i in range(1, 7):
        for j in range(1, 7):
            ele1 = ellipse_1.get_element(i, j)
            ele2 = ellipse_2.get_element(i, j)
            mess = "i="+str(i)+", j="+str(j)+"\n"+str(ellipse_1)+"\n"+\
                                                str(ellipse_2)
            if abs(ele1) < 1e-9:
                if abs(ele1-ele2) > 1e-9:
                    return False, mess
            else:
                if abs(ele1/ele2 - 1.) > 1e-2:
                    return False, mess
    return True, ""


# pylint: disable=C0103
@unittest.skipIf(ROOT.gROOT.IsBatch(), "Needs a GUI window")
class EnvelopeToolTest(unittest.TestCase): # pylint: disable=R0904
    """Tests for envelope main window"""
    def setUp(self):
        envelope_tool.set_share_dirs()

    def test_command_line(self):
        """Test we can run envelope tool from the command line okay"""
        log = os.path.expandvars("${MAUS_TMP_DIR}/test_envelope_tool.log")
        proc = subprocess.Popen([ENV_DIR+"envelope_tool.py"],
                                 stdin=subprocess.PIPE, stdout=open(log, "w"),
                                 stderr=subprocess.STDOUT)
        # I don't check passing arguments from the command line      
        time.sleep(2)
        proc.send_signal(signal.SIGINT)
        time.sleep(2)
        self.assertEqual(proc.poll(), 0)

    def test_envelope_tool(self):
        """Test buttons in envelope_tool window work okay"""
        main_window = envelope_tool.MainWindow()

        self.assertEqual(main_window.beam_setup, None)
        beam_button = main_window.window.get_frame("&Beam Setup", "button")
        beam_button.Clicked()
        self.assertNotEqual(main_window.beam_setup, None)

        self.assertEqual(main_window.magnet_setup, None)
        magnet_button = main_window.window.get_frame("&Magnet Setup", "button")
        magnet_button.Clicked()
        self.assertNotEqual(main_window.magnet_setup, None)

        self.assertEqual(main_window.plot_setup, None)
        magnet_button = main_window.window.get_frame("&Plot Setup", "button")
        magnet_button.Clicked()
        self.assertNotEqual(main_window.plot_setup, None)

        exit_button = main_window.window.get_frame("E&xit", "button")
        exit_button.Clicked()
        time.sleep(1)
        self.assertEqual(main_window.window.main_frame, None)

@unittest.skipIf(ROOT.gROOT.IsBatch(), "Needs a GUI window")
class BeamSetupTest(unittest.TestCase): # pylint: disable=R0904
    """Tests for beam setup and subwindows"""

    def setUp(self):
        envelope_tool.set_share_dirs()
        self.red_dict = {'pid':13, 'px':1., 'py':2., 'pz':3., 
                         'x':-1, 'y':-2, 'z':-3}
        self.hit_red = Hit.new_from_dict(self.red_dict)
        self.hit_full = Hit.new_from_dict({'pid':13, 'px':1., 'py':2., 'pz':3.,
                                    'x':-1, 'y':-2, 'z':-3,
                                    'mass':Common.pdg_pid_to_mass[13],
                                    'charge':Common.pdg_pid_to_charge[13]},
                                    'energy')
        self.main_window = envelope_tool.MainWindow()
        self.main_window.window.get_frame("&Beam Setup", "button").Clicked()
        self.beam_setup = self.main_window.beam_setup
        self.cov = CovarianceMatrix()
        for i in range(1, 7):
            for j in range(1, 7):
                self.cov.set_element(i, j, i*j)
            self.cov.set_element(i, i, i*i+50.)

    def _ell_equal(self, ell1, ell2):
        """Wrapper for _ell_equal"""
        passed, message = _ell_equal(ell1, ell2)
        self.assertTrue(passed, msg=message)

    def test_beam_setup_set_get(self): # pylint: disable=R0914
        """Test envelope_tool.beam_setup set/get reference"""
        # first get the beam_setup window
        # check set_reference calculates mass, charge, energy correctly
        # and basic get_reference functionality
        self.beam_setup.set_reference(self.hit_red)
        for key, set_value in self.red_dict.iteritems():
            text_value = self.beam_setup.window.get_text_entry(key, type(1.))
            self.assertAlmostEqual(text_value, float(set_value))
        self.assertEqual(self.beam_setup.get_reference(), self.hit_full)

        # check set_covariance_matrix
        self.beam_setup.set_matrix(self.cov)
        axis_list = ["t", "e", "x", "px", "y", "py"]
        test = self.beam_setup.get_matrix()
        for i, ax_i in enumerate(axis_list):
            for j, ax_j in enumerate(axis_list):
                value = self.beam_setup.window.get_text_entry(ax_i+ax_j,
                                                              type(1.))
                self.assertAlmostEqual(self.cov.get_element(i+1, j+1), value)
                self.assertAlmostEqual(self.cov.get_element(i+1, j+1),
                                       test.get_element(i+1, j+1))
        
    def test_beam_setup_twiss(self):
        """Test beam_setup twiss button"""
        self.assertEqual(self.beam_setup.matrix_select, None)
        self.beam_setup.window.get_frame("&Twiss", "button").Clicked()
        self.assertNotEqual(self.beam_setup.matrix_select, None)

    def test_beam_setup_penn(self):
        """Test beam_setup penn button"""
        self.beam_setup.window.get_frame("&Penn", "button").Clicked()
        self.assertNotEqual(self.beam_setup.matrix_select, None)

    def test_beam_setup_okay(self):
        """Test beam_setup okay button"""
        self.beam_setup.set_matrix(self.cov)
        self.beam_setup.set_reference(self.hit_red)
        self.beam_setup.window.get_frame("&Okay", "button").Clicked()
        self.assertEqual(self.main_window.beam_setup, None)
        self.assertEqual(self.main_window.lattice.ref_list[0], self.hit_full)
        self._ell_equal(self.main_window.lattice.ellipse_list[0], self.cov)

    def test_beam_setup_cancel(self):
        """Test beam_setup cancel button"""
        test_matrix = self.beam_setup.get_matrix()
        test_ref = self.beam_setup.get_reference()
        self.beam_setup.window.get_frame("&Okay", "button").Clicked()
        self.main_window.window.get_frame("&Beam Setup", "button").Clicked()
        self.beam_setup = self.main_window.beam_setup
        self.beam_setup.set_matrix(self.cov)
        self.beam_setup.set_reference(self.hit_red)
        self.beam_setup.window.get_frame("&Cancel", "button").Clicked()
        for var in Hit.get_variables():
            self.assertAlmostEqual(self.main_window.lattice.ref_list[0][var],
                                   test_ref[var], 3)
        self._ell_equal(self.main_window.lattice.ellipse_list[0], test_matrix)

    def test_penn_setup(self):
        """Test penn_setup logic"""
        self.beam_setup.window.get_frame("&Penn", "button").Clicked()
        penn = self.beam_setup.matrix_select

        test_dict = {"beta_trans":444., "alpha_trans":1., "emittance_trans":6.0,
                     "B0":-4, "Lc": 1.,
                     "beta_l":10., "alpha_l":1., "emittance_l":10.,
                     "disp_x":5., "disp_prime_x":1.,
                     "disp_y":10., "disp_prime_y":2.
                    }
        for name, entry in test_dict.iteritems():
            penn.window.set_text_entry(name, entry)
        # Check Constant Beta
        self.assertAlmostEqual(
                     penn.window.get_text_entry("beta_trans", type(1.)), 444.)
        penn.window.get_frame("&Constant Beta", "button").Clicked()
        self.assertAlmostEqual(
                  penn.window.get_text_entry("beta_trans", type(1.)), 471.21349)
        # Check Cancel
        test_cov = self.beam_setup.get_matrix()
        penn.window.get_frame("&Cancel", "button").Clicked()
        # cancel so no change
        self._ell_equal(test_cov, self.beam_setup.get_matrix())
        self.assertEqual(self.beam_setup.matrix_select, None)

        # now check Okay
        self.beam_setup.window.get_frame("&Penn", "button").Clicked()
        penn = self.beam_setup.matrix_select
        for name, entry in test_dict.iteritems():
            penn.window.set_text_entry(name, entry)
        penn.window.get_frame("&Okay", "button").Clicked()
        ref = self.beam_setup.get_reference()
        # not the factor -1 in B0...
        test_cov = maus_cpp.covariance_matrix.create_from_penn_parameters(
          ref["mass"], ref["p"], test_dict["emittance_trans"],
          test_dict["beta_trans"], test_dict["emittance_l"],
          test_dict["beta_l"], test_dict["alpha_trans"], test_dict["alpha_l"],
          ref["charge"], -test_dict["B0"]*1e-3, test_dict["Lc"],
          test_dict["disp_x"],
          test_dict["disp_prime_x"], test_dict["disp_y"],
          test_dict["disp_prime_y"]
        )
        self._ell_equal(test_cov, self.beam_setup.get_matrix())
        self.assertEqual(self.beam_setup.matrix_select, None)

    def test_penn_setup_b0(self):
        """Test the B0 button"""
        sys.argv += ["--configuration_file", TEST_DIR+"test_config_2.py"]
        self.main_window.lattice = lattice.Lattice() # resets fields I hope
        self.beam_setup.window.get_frame("&Penn", "button").Clicked()
        penn = self.beam_setup.matrix_select
        # Check Get BO
        self.hit_full['x'] = 100.
        self.hit_full['y'] = 100.
        self.hit_full['z'] = 100.
        self.beam_setup.set_reference(self.hit_full)
        penn.window.get_frame("Get &B0", "button").Clicked()
        b_vec = maus_cpp.field.get_field_value(100., 100., 100., 0.)
        b_mag = 0.
        for i in range(3):
            b_mag += b_vec[i]**2
        self.assertGreater(1.e3*b_mag**0.5, 1e-2) # check bfield is non-zero
        self.assertAlmostEqual(penn.window.get_text_entry("B0", type(1.)),
                               1.e3*b_mag**0.5, 3) # check field is correct

    def test_twiss_setup(self):
        """Test twiss_setup logic"""
        test_dict = {"beta_x":444., "alpha_x":0.1, "emittance_x":6.0,
                     "beta_y":222., "alpha_y":0.2, "emittance_y":3.0,
                     "beta_l":10., "alpha_l":-0.1, "emittance_l":20.,
                     "disp_x":0.5, "disp_prime_x":0.1,
                     "disp_y":0.11, "disp_prime_y":0.2
                    }
        ref = self.beam_setup.get_reference()

        # test Cancel button
        self.beam_setup.window.get_frame("&Twiss", "button").Clicked()
        twiss = self.beam_setup.matrix_select
        test_cov = self.beam_setup.get_matrix()
        for name, entry in test_dict.iteritems():
            twiss.window.set_text_entry(name, entry)
        twiss.window.get_frame("&Cancel", "button").Clicked()
        self._ell_equal(test_cov, self.beam_setup.get_matrix())
        self.assertEqual(self.beam_setup.matrix_select, None)

        # test Okay button
        self.beam_setup.window.get_frame("&Twiss", "button").Clicked()
        twiss = self.beam_setup.matrix_select
        for name, entry in test_dict.iteritems():
            twiss.window.set_text_entry(name, entry)
        twiss.window.get_frame("&Okay", "button").Clicked()
        test_cov = maus_cpp.covariance_matrix.create_from_twiss_parameters(
          ref["mass"], ref["p"], 
          test_dict["emittance_x"], test_dict["beta_x"],
          test_dict["emittance_y"], test_dict["beta_y"],
          test_dict["emittance_l"], test_dict["beta_l"],
          test_dict["alpha_x"], test_dict["alpha_y"], test_dict["alpha_l"],
          test_dict["disp_x"], test_dict["disp_prime_x"],
          test_dict["disp_y"], test_dict["disp_prime_y"]
        )
        self._ell_equal(test_cov, self.beam_setup.get_matrix())
        self.assertEqual(self.beam_setup.matrix_select, None)

    def test_plot_setup(self):
        """Test plot_setup logic"""
        main_window = envelope_tool.MainWindow()
        main_window.window.get_frame("&Plot Setup", "button").Clicked()
        plot_setup = main_window.plot_setup
        # check select action
        first_var = plot_setup.window.get_frame("first_var_0", "drop_down")
        self.assertEqual(first_var.GetNumberOfEntries(), 1)
        plot_setup.window.get_frame("variable_type_0", "drop_down").Select(1)
        self.assertGreater(first_var.GetNumberOfEntries(), 1)
        first_var.Select(1)
        # check cancel action
        test_options = main_window.plot_setup_options
        plot_setup.window.get_frame("&Cancel", "button").Clicked()
        self.assertEqual(main_window.plot_setup, None)
        self.assertEqual(main_window.plot_setup_options, test_options)
        # Check okay action
        main_window.window.get_frame("&Plot Setup", "button").Clicked()
        plot_setup = main_window.plot_setup
        plot_setup.window.get_frame("variable_type_0", "drop_down").Select(1)
        plot_setup.window.get_frame("first_var_0", "drop_down").Select(1)
        plot_setup.window.get_frame("plot_apertures",
                                    "check_button").SetState(0)
        test_options = [{"variable_type":1, "first_var":1,
                         "plot_apertures":False}]
        plot_setup.window.get_frame("&Okay", "button").Clicked()
        self.assertEqual(main_window.plot_setup, None)
        self.assertEqual(main_window.plot_setup_options, test_options)

@unittest.skipIf(ROOT.gROOT.IsBatch(), "Needs a GUI window")
class MagnetSetupTest(unittest.TestCase): # pylint: disable=R0904
    """Tests for magnet setup"""
    @classmethod
    def setUpClass(cls):
        envelope_tool.set_share_dirs()

    def _ell_equal(self, ell1, ell2):
        """Wrapper for _ell_equal"""
        passed, message = _ell_equal(ell1, ell2)
        self.assertTrue(passed, msg=message)

    def test_magnet_setup(self):
        """Test magnet_setup logic"""
        # Check that this works in the absence of fields
        main_window = envelope_tool.MainWindow()
        for button in ["&Cancel", "&Okay"]:
            main_window.window.get_frame("&Magnet Setup", "button").Clicked()
            magnet_setup = main_window.magnet_setup
            magnet_setup.window.get_frame(button, "button").Clicked()
        main_window.window.get_frame("E&xit", "button").Clicked()
        # now with fields
        sys.argv += ["--configuration_file", TEST_DIR+"test_config.py"]
        main_window = envelope_tool.MainWindow()
        main_window.window.get_frame("&Magnet Setup", "button").Clicked()
        magnet_setup = main_window.magnet_setup
        # Check field data is represented (manipulator worked)
        test_data = {"Q1":-1., "D2":-0.9, "Q2":1.}
        for test, data in test_data.iteritems():
            text = magnet_setup.window.get_text_entry(test, type(1.))
            self.assertAlmostEqual(text, data)
        # Check cancel action
        magnet_setup.window.set_text_entry("Q1", 2.)
        magnet_setup.window.get_frame("&Cancel", "button").Clicked()
        main_window.window.get_frame("&Magnet Setup", "button").Clicked()
        magnet_setup = main_window.magnet_setup
        q1 = magnet_setup.window.get_text_entry("Q1", type(1.))
        self.assertAlmostEqual(q1, test_data["Q1"])
        # Check okay action
        magnet_setup.window.set_text_entry("Q1", 2.)
        magnet_setup.window.get_frame("&Okay", "button").Clicked()
        main_window.window.get_frame("&Magnet Setup", "button").Clicked()
        magnet_setup = main_window.magnet_setup
        q1 = magnet_setup.window.get_text_entry("Q1", type(1.))
        self.assertAlmostEqual(q1, 2.)
        sys.argv = sys.argv[0:-2]

    def _field_list_equal(self, field_list, test_field):
        """Check that field list elements are equal"""
        for field in field_list:
            if field["field_name"] == test_field["field_name"]:
                self.assertEqual(sorted(test_field.keys()),
                                 sorted(field.keys()))
                for key, ref in test_field.iteritems():
                    if type(ref) == type(""):
                        self.assertEqual(ref, field[key])
                    if type(ref) == type(1.):
                        self.assertAlmostEqual(ref, field[key])
                    if type(ref) == type({}):
                        self.assertEqual(sorted(ref.keys()), ["x", "y", "z"])
                        for axis in ["x", "y", "z"]:
                            self.assertAlmostEqual(ref[axis],
                                                   field[key][axis])

    def test_lattice_get_set_fields(self):
        """Test lattice - get set fields"""
        sys.argv += ["--configuration_file", TEST_DIR+"test_config.py"]
        main_window = envelope_tool.MainWindow()
        # get_set_fields
        field_list = main_window.lattice.get_field_list()
        test_q1 = {
            "field_name":"Q1",
            "field_type":"Multipole",
            "scale_factor":-1.,
            "aperture":{"x":172., "y":172., "z":550.},
            "outer":{"x":250., "y":250., "z":550.},
            "position":{"x":1600., "y":0., "z":0.},
            "rotation":{"x":0., "y":45./180.*math.pi, "z":0.}
        }
        self.assertEqual(len(field_list), 3)
        sorted_names = sorted([field["field_name"] for field in field_list])
        self.assertEqual(sorted_names, ["D2", "Q1", "Q2"])
        self._field_list_equal(field_list, test_q1)
        for field in field_list:
            field["scale_factor"] = 2.
        main_window.lattice.set_fields(field_list)
        field_list_2 = main_window.lattice.get_field_list()
        test_q1["scale_factor"] = 2.
        self._field_list_equal(field_list_2, test_q1)


    def test_lattice_get_set_beam(self):
        """Test lattice - get set beam"""
        sys.argv += ["--configuration_file", TEST_DIR+"test_config.py"]
        _lattice = envelope_tool.MainWindow().lattice
        self.assertGreater(len(_lattice.ref_list), 1)
        self.assertGreater(len(_lattice.ellipse_list), 1)
        hit_ref, ellipse_ref = _lattice.get_beam()
        hit_ref['pid'] = 2212
        ellipse_ref.set_element(1, 1, 999)
        _lattice.set_beam(hit_ref, ellipse_ref)
        hit, ellipse = _lattice.get_beam()
        self.assertEqual(hit_ref['pid'], hit['pid'])
        self._ell_equal(ellipse, ellipse_ref)
        self.assertEqual(len(_lattice.ref_list), 1)
        self.assertEqual(len(_lattice.ellipse_list), 1)


    def test_lattice_run_lattice_ref(self):
        """Test lattice - run lattice"""
        # try changing the field configuration and reference particle; check
        # that the reference particle comes out okay
        sys.argv += ["--configuration_file", TEST_DIR+"test_config.py"]
        _lattice = envelope_tool.MainWindow().lattice
        self.assertGreater(len(_lattice.ref_list), 1)
        self.assertGreater(len(_lattice.ellipse_list), 1)
        last_hit = _lattice.ref_list[-1]
        field_list = _lattice.get_field_list()
        for item in field_list:
            item["scale_factor"] *= 2.
        _lattice.set_fields(field_list)
        hit_ref, ellipse_ref = _lattice.get_beam()
        hit_ref['p'] = hit_ref['p']*2.
        hit_ref.mass_shell_condition('energy')
        _lattice.set_beam(hit_ref, ellipse_ref)
        _lattice.run_lattice()
        test_hit = _lattice.ref_list[-1]
        for pos in ['x', 'y', 'z']:
            self.assertAlmostEqual(last_hit[pos], test_hit[pos], 3)
        for mom in ['px', 'py', 'pz']:
            self.assertAlmostEqual(last_hit[mom]*2., test_hit[mom], 1)

    def test_lattice_run_lattice_ellipses(self):
        """Test lattice - run lattice"""
        # check that ellipse propagates okay
        # MICE SFoFo lattice nearly periodic with beta=431.
        sys.argv += ["--configuration_file", TEST_DIR+"test_config_2.py"]
        _lattice = envelope_tool.MainWindow().lattice
        self.assertGreater(len(_lattice.ref_list), 1)
        self.assertGreater(len(_lattice.ellipse_list), 1)
        ref, ellipse = _lattice.get_beam()
        ellipse = maus_cpp.covariance_matrix.create_from_penn_parameters(
          ref["mass"], ref["p"], 1.0, 431., 10., 10.
        )
        _lattice.set_beam(ref, ellipse)
        _lattice.run_lattice()
        last_ellipse = _lattice.ellipse_list[-1]
        ratio_sxx = ellipse.get_element(3, 3)/last_ellipse.get_element(3, 3)
        self.assertLess(abs(ratio_sxx-1.), 5e-2)
        ratio_spp = ellipse.get_element(4, 4)/last_ellipse.get_element(4, 4)
        self.assertLess(abs(ratio_spp-1.), 5e-2)

if __name__ == "__main__":
    unittest.main()

