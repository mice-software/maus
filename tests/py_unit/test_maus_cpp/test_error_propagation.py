import StringIO
import unittest
import json

import numpy
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

    def _convert_matrix(self, ellipse_in, forwards):
        ellipse_out = [[0. for i in range(6)] for j in range(6)]
        for i in range(6):
            k = [0, 3, 1, 4, 2, 5][i]
            for j in range(6):
                l = [0, 3, 1, 4, 2, 5][j]
                if forwards:
                    ellipse_out[k][l] = ellipse_in[i][j]
                else:
                    ellipse_out[i][j] = ellipse_in[k][l]
        return ellipse_out

    def _print_output(self, centroid, ellipse, tm):
        for q in centroid[0:4]:
            print str(round(q, 1)).ljust(10),
        print
        for p in centroid[4:8]:
            print str(round(p, 1)).ljust(10),
        print
        ellipse = self._convert_matrix(ellipse, False)
        trans_ellipse = [[ellipse[i][j] for i in range(2, 6)] for j in range(2, 6)]

        mass = xboa.common.pdg_pid_to_mass[13]
        dummy, dummy, bz, dummy, dummy, dummy = maus_cpp.field.get_field_value(centroid[1], centroid[2], centroid[3], centroid[0])
        eps_trans = (numpy.linalg.det(trans_ellipse))**0.25/mass
        kappa = 0.15*bz/centroid[7]*1e3
        beta_trans = (trans_ellipse[0][0]+trans_ellipse[2][2])*centroid[7]/(2*mass*eps_trans)
        l_twiddle = (trans_ellipse[0][3]-trans_ellipse[3][2])+mass*eps_trans*beta_trans*kappa
        print "Bz:", bz
        print "Emittance_trans:", eps_trans
        print "Beta_trans:", beta_trans
        print "L_twiddle:", l_twiddle
        print "Ellipse:"
        for row in ellipse:
            for element in row:
                print str(round(element, 1)).rjust(10),
            print
        if tm == None:
            return
        dz = 1000.
        # M = 1+dM/dz*dz
        # GetTransferMatrix() returns dM/dz
        print "Transfer Matrix for dz", dz, ":"
        tm = self._convert_matrix(tm, False)
        for i, row in enumerate(tm):
            for j, element in enumerate(row):
                tm[i][j] *= dz
            tm[i][i] += 1.
            for j, element in enumerate(row):
                print str(round(tm[i][j], 5)).rjust(10),
            print
        print "Determinant:", numpy.linalg.det(numpy.array(tm))

    def _test_step_demo(self):
        self._new_geometry("tests/py_unit/test_maus_cpp/cd_step_frozen_53_1_140_no-de.dat")
        centroid = [0. for i in range(8)]
        mass = xboa.common.pdg_pid_to_mass[13]
        mom = 140.
        centroid[3] = -4500.
        centroid[4] = (mom**2+mass**2)**0.5
        centroid[7] = mom
        print maus_cpp.field.str(True)
        kappa = 4e-3*0.15/mom*1e3;
        beta = 1/kappa;
        dummy, dummy, bz, dummy, dummy, dummy = maus_cpp.field.get_field_value(centroid[1], centroid[2], centroid[3], centroid[0])
        ellipse_trans = Bunch.build_penn_ellipse(6., mass, beta, 0., mom, 0., bz, 1.) 
        ellipse = [[0. for i in range(6)] for j in range(6)]
        for i in range(6):
            for j in range(6):
                if i == j and i < 2:
                    ellipse[i][j] = 1.
                if i > 1 and j > 1:
                    ellipse[i][j] = ellipse_trans[i-2, j-2]
        ellipse = self._convert_matrix(ellipse, True)
        for z_max in range(-4500, -3699, 100):
            centroid_out, ellipse_out = err_prop.propagate_errors(centroid, ellipse, z_max)
            self._print_output(centroid_out, ellipse_out, None)


    def test_step_fs2a(self):
        self._new_geometry("tests/py_unit/test_maus_cpp/fs2a.dat")
        centroid = [0. for i in range(8)]
        mass = xboa.common.pdg_pid_to_mass[13]
        mom = 233.
        centroid[3] = 0.
        centroid[4] = (mom**2+mass**2)**0.5
        centroid[7] = mom
        print maus_cpp.field.str(True)
        beta = 800.;
        dummy, dummy, bz, dummy, dummy, dummy = maus_cpp.field.get_field_value(centroid[1], centroid[2], centroid[3], centroid[0])
        ellipse_trans = Bunch.build_penn_ellipse(6., mass, beta, 0., mom, 0., bz, 1.) 
        ellipse = [[0. for i in range(6)] for j in range(6)]
        for i in range(6):
            for j in range(6):
                if i == j and i < 2:
                    ellipse[i][j] = 1.
                if i > 1 and j > 1:
                    ellipse[i][j] = ellipse_trans[i-2, j-2]
        ellipse = self._convert_matrix(ellipse, True)
        for z_max in range(-0, 1501, 50):
            centroid_out, ellipse_out = err_prop.propagate_errors(centroid, ellipse, z_max)
            tm = err_prop.get_transfer_matrix(centroid)
            self._print_output(centroid_out, ellipse_out, tm)



if __name__ == "__main__":
    unittest.main()

