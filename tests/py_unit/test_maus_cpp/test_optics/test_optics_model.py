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
import StringIO
import os

import Configuration
import maus_cpp.globals
import maus_cpp.covariance_matrix
from maus_cpp.covariance_matrix import CovarianceMatrix
import maus_cpp.phase_space_vector
from maus_cpp.phase_space_vector import PhaseSpaceVector
import maus_cpp.optics_model
from maus_cpp.optics_model import OpticsModel

GEO = \
     "${MAUS_ROOT_DIR}/tests/py_unit/test_maus_cpp/test_optics/optics_model.dat"

REF = {
    "position":{"x":0., "y":0., "z":0.},
    "momentum":{"x":0., "y":0., "z":1.},
    "particle_id":-13, "energy":226., "random_seed":0, "time":0.
}

class OpticsModelTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.optics_model"""
    def setUp(self):
        """import datacards"""
        self.test_config = ""
        self._set_geometry(os.path.expandvars(GEO))

    def _set_geometry(self, geom_filename):
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.test_config = Configuration.Configuration().getConfigJSON()
        json_config = json.loads(self.test_config)
        json_config["simulation_geometry_filename"] = geom_filename
        json_config["simulation_reference_particle"] = REF
        json_config["physics_processes"] = "none"
        self.test_config = json.dumps(json_config)
        maus_cpp.globals.birth(self.test_config)

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
        optics = OpticsModel()
        optics.__init__() # legal, should reinitialise

    def test_transport_covariance_matrix_bad(self):
        """
        Test maus_cpp.optics_model.Optics().transport_covariance_matrix()
        with no virtuals
        """
        self._set_geometry("Test.dat")
        optics = OpticsModel()
        cm_in = CovarianceMatrix()
        try:
            cm_out = optics.transport_covariance_matrix(cm_in)
            self.assertTrue(False, "Should throw when no virtuals")
        except RuntimeError:
            pass
        try:
            cm_out = optics.transport_covariance_matrix("should be a cm")
            self.assertTrue(False, "Should throw when wrong type passed")
        except RuntimeError:
            pass

    def test_transport_covariance_matrix(self):
        """Test maus_cpp.optics_model.Optics().transport_covariance_matrix()"""
        optics = OpticsModel()
        cm_in = maus_cpp.covariance_matrix.create_from_penn_parameters(
                  mass=105.658, momentum=200., emittance_t=6., beta_t=333.,
                 emittance_l=1., beta_l=10.)
        cm_out = optics.transport_covariance_matrix(cm_in)

    def test_transport_phase_space_vector_bad(self):
        """
        Test maus_cpp.optics_model.Optics().transport_phase_space_vector() with
        no virtuals
        """
        return
        self._set_geometry("Test.dat")
        optics = OpticsModel()
        psv_in = PhaseSpaceVector()
        try:
            psv_out = optics.transport_phase_space_vector(psv_in)
            self.assertTrue(False, "Should throw when no virtuals")
        except RuntimeError:
            pass
        try:
            psv_out = optics.transport_phase_space_vector("should be a psv")
            self.assertTrue(False, "Should throw when wrong type passed")
        except RuntimeError:
            pass

    def test_transport_phase_space_vector(self):
        """Test maus_cpp.optics_model.Optics().transport_phase_space_vector()"""
        optics = OpticsModel()
        psv_in = maus_cpp.phase_space_vector.create_from_coordinates \
                                                      (0., 226., 0., 0., 0., 0.)
        psv_out = optics.transport_phase_space_vector(psv_in)


if __name__ == "__main__":
    unittest.main()


