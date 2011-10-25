import unittest
#import tempfile
import os
import subprocess

class RootTest(unittest.TestCase):
    def test_geometries(self):
"""Checks that no other program is grabbing the MAUS help function"""
        test_out = open(os.environ['MAUS_ROOT_DIR']+'/tests/py_unit/no_root_help_test', 'r+')
        subprocess.Popen([os.environ['MAUS_ROOT_DIR']+'/bin/simulate_mice.py', '-h'], stdout=test_out)
        first_line = test_out.readline()
        assumed_output = 'usage: simulate_mice.py [-h]'
        self.assertEqual(assumed_output,first_line[:28])

if __name__ == '__main__':
    unittest.main()
