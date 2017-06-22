#!/usr/bin/env python

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

# pylint: disable=C0103

"""
Test maus_cpp.optics_model
"""

import json
import unittest
import os

import Configuration
import maus_cpp.globals
import maus_cpp.mice_module
import maus_cpp.covariance_matrix
from maus_cpp.covariance_matrix import CovarianceMatrix
import maus_cpp.phase_space_vector
from maus_cpp.phase_space_vector import PhaseSpaceVector
import maus_cpp.optics_model
from maus_cpp.optics_model import OpticsModel

REF = {
    "position":{"x":0., "y":0., "z":1000.001},
    "momentum":{"x":0., "y":0., "z":1.},
    "particle_id":-13, "energy":226., "random_seed":0, "time":0.
}

class OpticsModelTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.optics_model"""
    def setUp(self):
        """import datacards"""
        self.test_config = ""
        self.good_mod = os.path.expandvars(\
              "${MAUS_ROOT_DIR}/tests/py_unit/test_maus_cpp/"+\
              "test_optics/optics_model.dat")
        self.no_planes_mod = os.path.expandvars("Test.dat")

    def _set_geometry(self, geom_filename):
        """Geometry setup"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.test_config = Configuration.Configuration().getConfigJSON()
        json_config = json.loads(self.test_config)
        json_config["simulation_geometry_filename"] = geom_filename
        json_config["simulation_reference_particle"] = REF
        json_config["physics_processes"] = "none"
        self.test_config = json.dumps(json_config)
        maus_cpp.globals.birth(self.test_config)
        maus_cpp.globals.set_monte_carlo_mice_modules(
                                maus_cpp.mice_module.MiceModule(geom_filename))

    def test_init_no_globals(self):
        """Test maus_cpp.optics_model.__init__() and deallocation"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        try:
            OpticsModel()
            self.assertTrue(False, msg="Should throw an exception if globals "+\
                                       "is not initialised")
        except RuntimeError:
            pass

    def test_init_all_okay(self):
        """Test maus_cpp.optics_model.__init__() and deallocation"""
        self._set_geometry(self.good_mod)
        optics = OpticsModel()
        optics.__init__() # legal, should reinitialise

    def test_transport_covariance_matrix_no_planes(self):
        """
        Test maus_cpp.optics_model.Optics().transport_covariance_matrix()
        with no virtuals
        """
        self._set_geometry(self.no_planes_mod)
        optics = OpticsModel()
        cm_in = CovarianceMatrix()
        try:
            optics.transport_covariance_matrix(cm_in, 2000.)
            self.assertTrue(False, "Should throw when no virtuals")
        except RuntimeError:
            pass

    def test_transport_covariance_matrix_bad_type(self):
        """
        Test maus_cpp.optics_model.Optics().transport_covariance_matrix()
        with non cm
        """
        self._set_geometry(self.good_mod)
        optics = OpticsModel()
        try:
            optics.transport_covariance_matrix("should be a cm", 2000.)
            self.assertTrue(False, "Should throw when wrong type passed")
        except TypeError:
            pass

    def test_transport_covariance_matrix(self):
        """Test maus_cpp.optics_model.Optics().transport_covariance_matrix()"""
        self._set_geometry(self.good_mod)
        optics = OpticsModel()
        cm_in = maus_cpp.covariance_matrix.create_from_penn_parameters(
                  mass=105.658, momentum=200., emittance_t=6., beta_t=333.,
                 emittance_l=1., beta_l=10., bz=0.)
        # check energy, px RMS does not change (no fields)
        cm_out_1 = optics.transport_covariance_matrix(cm_in, 2000.)
        for i in [2, 4, 6]:
            self.assertAlmostEqual(cm_in.get_element(i, i),
                                  cm_out_1.get_element(i, i), 1,
                                msg="\nIN\n"+str(cm_in)+"\nOUT\n"+str(cm_out_1))
        cm_out_2 = optics.transport_covariance_matrix(cm_in, 3000.)
        for i in range(3):
            self.assertGreater(abs(cm_out_1.get_element(2*i+1, 2*i+2)), 1.)
        # check that we have some growth of correlation between e.g. x, px
        self.assertAlmostEqual(cm_out_1.get_element(3, 4),
                               cm_out_1.get_element(5, 6), 2)
        for i in [2, 4, 6]:
            self.assertAlmostEqual(cm_in.get_element(i, i),
                                  cm_out_2.get_element(i, i), 1,
                                msg="\nIN\n"+str(cm_in)+"\nOUT\n"+str(cm_out_2))
        # check no coupling between phase space planes
        for i in [2, 4, 5]:
            self.assertAlmostEqual(
                  cm_in.get_element(i+1, i)-cm_out_1.get_element(i+1, i), 
                  cm_out_1.get_element(i+1, i)-cm_out_2.get_element(i+1, i),
                  1,
                  msg="\nIN\n"+str(cm_in)+"\nOUT\n"+str(cm_out_2))


    def test_transport_phase_space_vector_no_planes(self):
        """
        Test maus_cpp.optics_model.Optics().transport_phase_space_vector() with
        no virtuals
        """
        self._set_geometry(self.no_planes_mod)
        optics = OpticsModel()
        psv_in = PhaseSpaceVector()
        try:
            optics.transport_phase_space_vector(psv_in, 2000.)
            self.assertTrue(False, "Should throw when no virtuals")
        except RuntimeError:
            pass

    def test_transport_phase_space_vector_bad_type(self):
        """
        Test maus_cpp.optics_model.Optics().transport_phase_space_vector() with
        no virtuals
        """
        self._set_geometry(self.good_mod)
        optics = OpticsModel()
        try:
            optics.transport_phase_space_vector("should be a psv", 2000.)
            self.assertTrue(False, "Should throw when wrong type passed")
        except TypeError:
            pass

    def test_transport_phase_space_vector(self):
        """Test maus_cpp.optics_model.Optics().transport_phase_space_vector()"""
        self._set_geometry(self.good_mod)
        optics = OpticsModel()
        psv_in = maus_cpp.phase_space_vector.create_from_coordinates \
                                                    (0.1, 226., 1., 2., 3., 4.)
        psv_out = optics.transport_phase_space_vector(psv_in, 2000.)
        pz = (226.**2-105.6583715**2.-2.**2-4.**2)**0.5
        dz = 1000.
        t_expected = psv_in.get_t()+psv_out.get_energy()/pz/300.*dz # c=300.
        x_expected = psv_in.get_x()+psv_in.get_px()/pz*dz
        y_expected = psv_in.get_y()+psv_in.get_py()/pz*dz
        self.assertAlmostEqual(psv_out.get_t()/t_expected, 1, 3)
        self.assertAlmostEqual(psv_out.get_energy()/psv_in.get_energy(), 1., 5)
        self.assertAlmostEqual(psv_out.get_x()/x_expected, 1., 3)
        self.assertAlmostEqual(psv_out.get_px()/psv_in.get_px(), 1., 5)
        self.assertAlmostEqual(psv_out.get_y()/y_expected, 1., 3)
        self.assertAlmostEqual(psv_out.get_py()/psv_in.get_py(), 1., 5)

if __name__ == "__main__":
    unittest.main()

