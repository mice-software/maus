import json
import unittest
import subprocess
import glob
import os
import sys

from MapCppSimulation import MapCppSimulation

class MapCppSimulationTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        if not os.environ.get("MICEFILES"):
            raise Exception('InitializeFail', 'MICEFILES environmental variable not set')

        if not os.path.isdir(os.environ.get("MICEFILES")):
            raise Exception('InitializeFail', 'MICEFILES is not a directory')


    def test_geometries(self):
        self.assertTrue(os.environ.get("MICEFILES") != None)
        self.assertTrue(os.path.isdir(os.environ.get("MICEFILES")))

        self.assertTrue(os.environ.get("MAUS_ROOT_DIR") != None)
        self.assertTrue(os.path.isdir(os.environ.get("MAUS_ROOT_DIR")))
        
        files = glob.glob("%s/Models/Configurations/*.dat" % os.environ.get("MICEFILES"))
        self.assertTrue(len(files) != 0)

        for file in files:
            args = ['python', '%s/tests/unit/test_geometry.py' %  os.environ.get("MAUS_ROOT_DIR"), file]
            p = subprocess.Popen(args)
            p.wait()
            self.assertEqual(p.returncode, 0)

    def test_bad_geometry(self):
        args = ['python', '%s/tests/unit/test_geometry.py' %  os.environ.get("MAUS_ROOT_DIR"), '/dev/null']
        p = subprocess.Popen(args)
        p.wait()
        self.assertNotEqual(p.returncode, 0)


if __name__ == '__main__':
    if len(sys.argv) == 1:
        unittest.main()
    if len(sys.argv) == 2:
        file = str(sys.argv[1])
        
        map = MapCppSimulation()
        map.Birth("""{ "simulation_geometry_filename" : "%s" } """ % file)
        map.Death()        
