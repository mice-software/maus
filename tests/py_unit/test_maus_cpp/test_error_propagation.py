import StringIO
import unittest
import json

import xboa.common
from xboa.bunch import Bunch
import Configuration
import maus_cpp.globals
import maus_cpp.field
import maus_cpp.error_propagation as err_prop

class TestErrorPropagation(unittest.TestCase):

    def setUp(self): # pylint: disable=C0103
        """Set up test"""
        config_str = Configuration.Configuration().getConfigJSON(
                                                         StringIO.StringIO(""), False)
        self.config = json.loads(config_str)

    def _new_geometry(self, geometry_name):
        self.config["simulation_geometry_filename"] = geometry_name
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(json.dumps(self.config))

    def test_step_demo(self):
        self._new_geometry("tests/py_unit/test_maus_cpp/cd_step_frozen_53_1_140_no-de.dat")
        centroid = [0. for i in range(8)]
        mass = xboa.common.pdg_pid_to_mass[13]
        mom = 140.
        centroid[3] = -3700.
        centroid[4] = (mom**2+mass**2)**0.5
        centroid[7] = mom
        print maus_cpp.field.str(True)
        kappa = 4e-3*0.15/mom*1e3;
        beta = 1/kappa;
        ellipse_trans = Bunch.build_penn_ellipse(6., mass, beta, 0., mom, 0., 4e-3, 1.) 
        ellipse = [[0. for j in range(6)] for i in range(6)]
        for i in range(6):
            k = [0, 3, 1, 4, 2, 5][i]
            for j in range(6):
               l = [0, 3, 1, 4, 2, 5][j]
               if i > 1 and j > 1:
                   ellipse[k][l] = ellipse_trans[i-2, j-2]
               elif i == j:
                   ellipse[k][l] = 1.
        for z_max in range(-3700, 3701, 3700):
            centroid_out, ellipse_out = err_prop.propagate_errors(centroid, ellipse, z_max)
            for q in centroid_out[0:4]:
                print str(round(q, 1)).ljust(10),
            print
            for p in centroid_out[4:8]:
                print str(round(p, 1)).ljust(10),
            print
            for row in ellipse_out:
                for element in row:
                    print str(round(element, 1)).rjust(10),
                print


if __name__ == "__main__":
    unittest.main()

