import StringIO
import unittest

import Configuration
import maus_cpp.globals
import maus_cpp.error_propagation as err_prop

class TestErrorPropagation(unittest.TestCase):

    def setUp(self): # pylint: disable=C0103
        """Set up test"""
        config_options = StringIO.StringIO(unicode("""
simulation_geometry_filename = "Test.dat"
reconstruction_geometry_filename = "Test.dat"
        """))
        self.config = Configuration.Configuration().getConfigJSON(
                                                         config_options, False)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(self.config)

    def test_error_propagation(self):
        centroid = [0. for i in range(8)]
        centroid[3] = -200.
        centroid[4] = 226.
        centroid[7] = 200.
        ellipse = [[0. for j in range(6)] for i in range(6)]
        for i in range(6):
            ellipse[i][i] = 1.
        print centroid
        for row in ellipse:
            for element in row:
                print element,
            print
        centroid, ellipse = err_prop.propagate_errors(centroid, ellipse, 200.)
        print centroid
        for row in ellipse:
            for element in row:
                print element,
            print

if __name__ == "__main__":
    unittest.main()

