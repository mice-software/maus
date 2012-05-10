"""
Run the examples and check they return 0
"""

import os
import glob
import subprocess
import unittest

def run_example(example_name):
    """
    Run an example as an executable
    """
    print 'Running', example_name
    os.chdir(os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp'))
    log = 'test_examples_'+example_name.split('/')[-1]+'.log'
    log = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", log)
    log_file = open(log, 'w')
    proc = subprocess.Popen([example_name],
                            stdout=log_file, stderr=subprocess.STDOUT)
    proc.communicate('\n')
    return proc, log

class TestExamples(unittest.TestCase): # pylint: disable=R0904
    """
    Run the examples and check they return 0
    """

    def test_all(self):
        """
        Get everything in bin/examples and check it runs with returncode 0
        """
        examples_dir = os.path.join\
                           (os.environ["MAUS_ROOT_DIR"], "bin", "examples", "*")
        examples = glob.glob(examples_dir)
        for item in examples:
            proc, log = run_example(item)
            self.assertEqual(proc.returncode, 0, msg="Check logfile "+log)

if __name__ == "__main__":
    unittest.main()

