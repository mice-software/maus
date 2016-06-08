import os
import subprocess
import unittest

class FixedFieldGeometryTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.test_dir = os.path.expandvars("${MAUS_ROOT_DIR}/tests/"+\
                   "integration/test_global_track_fitting/test_tracker_fitting")
        cls.sim = os.path.expandvars("${MAUS_ROOT_DIR}/bin/simulate_mice.py")
        cls.out = os.path.expandvars("${MAUS_TMP_DIR}/tmp/test_fixed_field_geometry.root")

    def generate_dataset(self):
        config = os.path.join(self.test_dir, "fixed_field_config.py")
        geometry = os.path.join(self.test_dir, "fixed_field_geometry.dat")
        proc = subprocess.Popen([self.sim, "--configuration_file", config,
                                    "--simulation_geometry_filename", geometry,
                                    "--output_root_file_name", self.out])
        proc.wait()

    def test_fixed_field(self):
        print "Generating data"
        self.generate_dataset()
        print "Generated data"

if __name__ == "__main__":
    unittest.main()
