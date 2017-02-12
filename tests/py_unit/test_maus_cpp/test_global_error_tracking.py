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

"""Test the global error propagation routines"""

import StringIO
import unittest
import json
import os

import numpy
import xboa.common
from xboa.bunch import Bunch
import Configuration
import maus_cpp.globals
import maus_cpp.field
import maus_cpp.global_error_tracking as err_prop

class TestErrorPropagation(unittest.TestCase): #pylint: disable=R0904
    """Test the global error propagation routines"""
    def setUp(self): # pylint: disable=C0103
        """Set up test"""
        config_str = Configuration.Configuration().getConfigJSON(
                                                   StringIO.StringIO(""), False)
        self.config = json.loads(config_str)
        self._new_geometry("Test.dat")

    def _new_geometry(self, geometry_name):
        """build a new geometry and set up MAUS"""
        self.config["simulation_geometry_filename"] = geometry_name
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(json.dumps(self.config))

    @classmethod
    def _convert_matrix(cls, ellipse_in, forwards):
        """convert a matrix to a different variable ordering"""
        ellipse_out = [[0. for i in range(6)] for j in range(6)]
        for i in range(6):
            k_ind = [0, 3, 1, 4, 2, 5][i]
            for j in range(6):
                l_ind = [0, 3, 1, 4, 2, 5][j]
                if forwards:
                    ellipse_out[k_ind][l_ind] = ellipse_in[i][j]
                else:
                    ellipse_out[i][j] = ellipse_in[k_ind][l_ind]
        return ellipse_out

    def _get_tm_numerical(self, centroid, delta, dz_):
        """Clean up output from _tm_matrix"""
        tm_fine = self._tm_matrix(centroid, delta, dz_)
        tm_coarse_1 = self._tm_matrix(centroid, delta*10, dz_)
        tm_coarse_2 = self._tm_matrix(centroid, delta, dz_*10)
        tm0 = [[tm_fine[i][j] for j in range(6)] for i in range(6)]
        for i in range(6):
            for j in range(6):
                if abs(tm_coarse_1[i][j]) < 1e-9 or \
                   abs(tm_coarse_2[i][j]) < 1e-9:
                    tm0[i][j] = 0
                elif abs(tm_fine[i][j]/tm_coarse_1[i][j]) < 0.2:
                    tm0[i][j] = 0
                elif abs(tm_fine[i][j]/tm_coarse_2[i][j]) < 0.2:
                    tm0[i][j] = 0
        for i in range(8):
            print str(round(centroid[i], 8)).rjust(12),
        print
        for i in range(6):
            for j in range(6):
                print str(round(tm_fine[i][j], 8)).rjust(12),
            print
        return tm0

    @classmethod
    def _tm_matrix(cls, centroid, delta, dz0): #pylint: disable=R0914
        """Calculate the transfer matrix numerically"""
        mass = xboa.common.pdg_pid_to_mass[13]
        ellipse = [[0. for i in range(6)] for j in range(6)]
        tm_numerical = [[0. for i in range(6)] for j in range(6)]
        for i in range(7):
            if i == 3:
                continue
            x_pos = [x for x in centroid]
            x_pos[i] += delta
            x_pos[7] = (x_pos[4]**2-x_pos[5]**2-x_pos[6]**2-mass**2)**0.5
            tracking = err_prop.GlobalErrorTracking()
            tracking.set_min_step_size(1.)
            x_pos, dummy = \
                         tracking.propagate_errors(x_pos, ellipse, x_pos[3]+dz0)

            x_neg = [x for x in centroid]
            x_neg[i] -= delta
            x_neg[7] = (x_pos[4]**2-x_pos[5]**2-x_pos[6]**2-mass**2)**0.5
            x_neg, dummy = \
                         tracking.propagate_errors(x_neg, ellipse, x_neg[3]+dz0)

            for j in range(7):
                if j == 3:
                    continue
                k_ind = i
                if k_ind > 3:
                    k_ind -= 1
                l_ind = j
                if l_ind > 3:
                    l_ind -= 1
                tm_numerical[l_ind][k_ind] = (x_pos[j]-x_neg[j])/2./delta/dz0
            tm_numerical[k_ind][k_ind] -= 1/dz0
        return tm_numerical

    def _print_output(self, centroid, ellipse): #pylint: disable=R0914
        """Print some diagnostics"""
        for pos in centroid[0:4]:
            print str(round(pos, 1)).ljust(10),
        print
        for mom in centroid[4:8]:
            print str(round(mom, 1)).ljust(10),
        print
        ellipse = self._convert_matrix(ellipse, False)
        trans_ellipse = \
                     [[ellipse[i][j] for i in range(2, 6)] for j in range(2, 6)]

        mass = xboa.common.pdg_pid_to_mass[13]
        dummy, dummy, b_z, dummy, dummy, dummy = maus_cpp.field.get_field_value\
                            (centroid[1], centroid[2], centroid[3], centroid[0])
        eps_trans = (numpy.linalg.det(trans_ellipse))**0.25/mass
        kappa = 0.15*b_z/centroid[7]*1e3
        beta_trans = (trans_ellipse[0][0]+trans_ellipse[2][2])*\
                                                  centroid[7]/(2*mass*eps_trans)
        l_twiddle = (trans_ellipse[0][3]-trans_ellipse[3][2])+\
                                                 mass*eps_trans*beta_trans*kappa
        print "Bz:", b_z
        print "Emittance_trans:", eps_trans
        print "Beta_trans:", beta_trans
        print "L_twiddle:", l_twiddle
        print "Ellipse:"
        for row in ellipse:
            for element in row:
                print str(round(element, 1)).rjust(10),
            print

    def _test_step_demo(self): #pylint: disable=R0914
        """Test the demo lattice"""
        self._new_geometry\
               ("tests/py_unit/test_maus_cpp/cd_step_frozen_53_1_140_no-de.dat")
        centroid = [0. for i in range(8)]
        mass = xboa.common.pdg_pid_to_mass[13]
        mom = 140.
        centroid[3] = -4500.
        centroid[4] = (mom**2+mass**2)**0.5
        centroid[7] = mom
        print maus_cpp.field.str(True)
        kappa = 4e-3*0.15/mom*1e3
        beta = 1/kappa
        dummy, dummy, b_z, dummy, dummy, dummy = maus_cpp.field.get_field_value\
                            (centroid[1], centroid[2], centroid[3], centroid[0])
        ellipse_trans = Bunch.build_penn_ellipse\
                                         (6., mass, beta, 0., mom, 0., b_z, 1.)
        ellipse = [[0. for i in range(6)] for j in range(6)]
        for i in range(6):
            for j in range(6):
                if i == j and i < 2:
                    ellipse[i][j] = 1.
                if i > 1 and j > 1:
                    ellipse[i][j] = ellipse_trans[i-2, j-2]
        ellipse = self._convert_matrix(ellipse, True)
        for z_max in range(-4500, 4500, 100):
            tracking = err_prop.GlobalErrorTracking()
            tracking.set_step_size(1.)
            centroid_out, ellipse_out = tracking.propagate_errors\
                                                      (centroid, ellipse, z_max)
            self._print_output(centroid_out, ellipse_out)

    def test_get_set_deviations(self):
        """Check get/set deviations"""
        tracking = err_prop.GlobalErrorTracking()
        ref_deviations = (0.1, 0.2, 0.3, 0.4)
        tracking.set_deviations(*ref_deviations) #pylint: disable=W0142
        test_deviations = tracking.get_deviations()
        for i in range(4):
            self.assertAlmostEqual(test_deviations[i], ref_deviations[i])

    def test_get_set_step_size(self):
        """Check get/set step size"""
        tracking = err_prop.GlobalErrorTracking()
        step_size_list = (1.928, -3, 8)
        tracking.set_max_step_size(1e6)
        for step_size in step_size_list:
            tracking.set_min_step_size(step_size)
            self.assertAlmostEqual(tracking.get_min_step_size(), abs(step_size))
        tracking.set_min_step_size(1e-6)
        for step_size in step_size_list:
            tracking.set_max_step_size(step_size)
            self.assertAlmostEqual(tracking.get_max_step_size(), abs(step_size))
        try:
            tracking.set_min_step_size(tracking.get_max_step_size()*2.)
            self.assertTrue(False, msg="Should have raised exception")
        except RuntimeError:
            pass
        try:
            tracking.set_max_step_size(tracking.get_min_step_size()/2.)
            self.assertTrue(False, msg="Should have raised exception")
        except RuntimeError:
            pass

    def test_get_set_tracking_model(self):
        """Check get/set tracking model"""
        tracking = err_prop.GlobalErrorTracking()
        for model in ["em_rk4_forwards_static", "em_rk4_backwards_static",
                      "em_rk4_forwards_dynamic", "em_rk4_backwards_dynamic"]:
            tracking.set_tracking_model(model)
            self.assertEqual(tracking.get_tracking_model(), model) 
        try:
            tracking.set_tracking_model("boaty mcboatface")
            raise ValueError("Should have thrown")
        except RuntimeError:
            pass

    def test_get_set_geometry_model(self):
        """Check get/set geometry model"""
        tracking = err_prop.GlobalErrorTracking()
        for model in ["geant4", "axial_lookup", "geant4"]:
            tracking.set_geometry_model(model)
            self.assertEqual(tracking.get_geometry_model(), model) 
        try:
            tracking.set_geometry_model("boaty mcboatface")
            raise ValueError("Should have thrown")
        except RuntimeError:
            pass


    def test_get_set_mcs_model(self):
        """Check get/set mcs model"""
        tracking = err_prop.GlobalErrorTracking()
        for mcs_model in "no_mcs", "moliere_forwards", "moliere_backwards":
            tracking.set_scattering_model(mcs_model)
            self.assertEqual(tracking.get_scattering_model(), mcs_model)
        try:
            tracking.set_scattering_model("boaty mcboatface")
            raise ValueError("Should have thrown")
        except RuntimeError:
            pass

    def test_get_set_eloss_model(self):
        """Check get/set eloss model"""
        tracking = err_prop.GlobalErrorTracking()
        for eloss_model in ("no_eloss",
                            "bethe_bloch_forwards",
                            "bethe_bloch_backwards"):
            tracking.set_energy_loss_model(eloss_model)
            self.assertEqual(tracking.get_energy_loss_model(), eloss_model)
        try:
            tracking.set_energy_loss_model("boaty mcboatface")
            raise ValueError("Should have thrown")
        except RuntimeError:
            pass

    def test_step_fs2a(self):
        """Check fs2a lattice"""
        self._new_geometry(os.path.expandvars(
            "${MAUS_ROOT_DIR}/tests/py_unit/test_maus_cpp/fs2a_derivatives.dat"
        ))
        centroid = [0. for i in range(8)]
        mass = xboa.common.pdg_pid_to_mass[13]
        mom = 200.
        centroid[3] = 0.
        centroid[4] = (mom**2+mass**2)**0.5
        centroid[7] = mom
        #print maus_cpp.field.str(True)
        beta = 942.75
        dummy, dummy, b_z, dummy, dummy, dummy = maus_cpp.field.get_field_value\
                            (centroid[1], centroid[2], centroid[3], centroid[0])
        ellipse_trans = Bunch.build_penn_ellipse\
                            (6., mass, beta, 0., mom, 0., b_z, 1.) 
        ellipse = [[0. for i in range(6)] for j in range(6)]
        for i in range(6):
            for j in range(6):
                if i == j and i < 2:
                    ellipse[i][j] = 1.
                if i > 1 and j > 1:
                    ellipse[i][j] = ellipse_trans[i-2, j-2]
        ellipse = self._convert_matrix(ellipse, True)
        while True:
            for z_max in range(0, 751, 750):
                print "\nz_max:", z_max, "============"
                tracking = err_prop.GlobalErrorTracking()
                tracking.set_min_step_size(1.)
                centroid_out, ellipse_out = tracking.propagate_errors\
                                                          (centroid, ellipse, z_max)
                self._print_output(centroid_out, ellipse_out)

    def test_tm(self):
        """Test transfer matrix calculation"""
        self._new_geometry(os.path.expandvars(
            "${MAUS_ROOT_DIR}/tests/py_unit/test_maus_cpp/fs2a_derivatives.dat"
        ))
        mass = xboa.common.pdg_pid_to_mass[13]
        centroid = [0. for i in range(8)]
        centroid[1] = 1.
        centroid[2] = 2.
        centroid[3] = 5.
        centroid[5] = 200.
        centroid[6] = 60.
        centroid[7] = 200.
        centroid[4] = (sum([p**2 for p in centroid[5:]])+mass**2)**0.5
        tracking = err_prop.GlobalErrorTracking()
        tracking.set_min_step_size(1.)
        tm_a = tracking.get_transfer_matrix(centroid, 1.)
        delta = 0.01
        zstep = 0.01
        tm_n = self._get_tm_numerical(centroid, delta, zstep)
        for tm0 in [tm_a, tm_n]:
            print "Transfer Matrix"
            #tm = self._convert_matrix(tm, False)
            for i, row in enumerate(tm0):
                tm0[i][i] += 1.
                for element in row:
                    print str(round(element, 10)).rjust(12),
                print
            print "Determinant:", numpy.linalg.det(numpy.array(tm0))

if __name__ == "__main__":
    unittest.main()

