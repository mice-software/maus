import unittest
#import tempfile
import os
import subprocess

class HelpTest(unittest.TestCase):
    def test_maus_help(self):
        """Checks that no other program is grabbing the MAUS help function"""
        file_name = os.environ['MAUS_ROOT_DIR']+'/tmp/help_message'
        test_out = open(file_name, 'w')
        subproc = subprocess.Popen(
            [os.environ['MAUS_ROOT_DIR']+'/bin/simulate_mice.py', '-h'], 
            stdout=test_out, stderr=subprocess.STDOUT)
        subproc.wait()
        test_out.close()
        test_in = open(file_name)
        first_line = test_in.readline()
        assumed_output = 'usage: simulate_mice.py [-h]'
        self.assertEqual(assumed_output,first_line[:28])

if __name__ == '__main__':
    unittest.main()
