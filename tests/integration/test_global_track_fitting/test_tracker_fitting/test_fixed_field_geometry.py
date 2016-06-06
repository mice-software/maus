import os
import subprocess
import unittest

class FixedFieldGeometryTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.test_dir = os.path.expandvars("${MAUS_ROOT_DIR}/tests/"+\
                   "integration/test_global_track_fitting/test_tracker_fitting"
        cls.sim = os.path.expandvars("${MAUS_ROOT_DIR}/bin/simulate_mice.py")

    def generate_dataset(self):
        config = os.path.join(self.test_dir, "fixed_field_config.py")
        geometry = os.path.join(self.test_dir, "fixed_field_geometry.dat")
        subprocess.POpen([self.sim, "--configuration_file", config])

    def test_fixed_field(self):
        pass

if __name__ == "__main__":
    unittest.main()
