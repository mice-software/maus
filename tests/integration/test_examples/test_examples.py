#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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


"""
Run the examples and check they return 0
"""

# Popen false errors
# pylint: disable=E1101

import time
import os
import glob
import subprocess
import unittest

def get_images():
    """
    Find any image files in cwd
    """
    image_list = []
    for extension in ["png", "gif", "jpg"]:
        image_list += glob.glob("*."+extension)
    return image_list

def safe_mkdir(my_dir):
    """
    Make sure a directory exists at my_dir, making a new one if it does not and
    raising OSError if there is something blocking
    """
    if not os.path.isdir(my_dir):
        os.mkdir(my_dir)


def move_images(_images_before, _images_after, _example_name):
    """
    move any new images (in images_after, not in images_before)
    """
    plot_dir = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tests', 'integration',
                                                            'plots', 'examples')
    safe_mkdir(plot_dir)
    plot_dir = os.path.join(plot_dir, _example_name)
    safe_mkdir(plot_dir)

    for image in _images_after:
        if image not in _images_before:
            file_name = os.path.split(image)[1]
            target = os.path.join(plot_dir, file_name)
            os.rename(image, target)

def run_example(example_name):
    """
    Run an example as an executable
    """
    log_dir = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', 'test_examples')
    safe_mkdir(log_dir)
    os.chdir(log_dir)
    images_before = get_images()
    mangled_name = os.path.split(example_name)[-1] # just the file name
    mangled_name = os.path.splitext(mangled_name)[0] # just the prefix
    log = mangled_name+'.log'
    log = os.path.join(log_dir, log)
    log_file = open(log, 'w')
    print 'Running', example_name, 'logging in', log
    proc = subprocess.Popen([example_name], stdin=subprocess.PIPE,
                            stdout=log_file, stderr=subprocess.STDOUT)
    while proc.poll() == None:
        proc.communicate('\n')
        time.sleep(1)
    proc.wait()
    move_images(images_before, get_images(), mangled_name)
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
                   (os.environ["MAUS_ROOT_DIR"], "bin", "examples", "*.py")
        examples_subdir = os.path.join\
                   (os.environ["MAUS_ROOT_DIR"], "bin", "examples", "*", "*.py")
        examples = glob.glob(examples_dir)+glob.glob(examples_subdir)
        for item in examples:
            proc, log = run_example(item)
            self.assertEqual(proc.returncode, 0, msg="Check logfile "+log)
            if "match_step_4_beta_function.py" in item:
                match_log = log
        self._test_optics_match_beta(match_log)
        self._test_load_root_file_cpp()

    def _test_optics_match_beta(self, match_log):
        fin = open(match_log)
        beta_start, beta_end = None, None
        for line in fin.readlines():
            if "beta at start" in line:
                beta_start = float(line.split()[-2])
            if "beta at end" in line:
                beta_end = float(line.split()[-2])
        self.assertAlmostEqual(beta_start, 322., 0) # mm
        self.assertAlmostEqual(beta_end, 322., 0) # mm


    def _test_load_root_file_cpp(self):
        """
        Test bin/examples/load_root_file_cpp; make clean; make; run test

        This has to be called after bin/examples/load_root_file.py (as it is
        dependent on file generation so we call it as a subfunction of test_all.
        """
        examples_dir = os.path.join\
          (os.environ["MAUS_ROOT_DIR"], "bin", "examples", "load_root_file_cpp")
        os.chdir(examples_dir)
        proc = subprocess.Popen(["make", "clean"])
        proc.wait()
        proc = subprocess.Popen(["make"])
        proc.wait()
        proc = subprocess.Popen(["./load_root_file"])
        proc.wait()
        self.assertEqual(proc.returncode, 0)

if __name__ == "__main__":
    unittest.main()

