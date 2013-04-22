#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

import sys
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

def move_images(_images_before, _images_after, _example_name):
    """
    move any new images (in images_after, not in images_before)
    """
    plot_dir = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tests', 'integration',
                                                            'plots', 'examples')
    try:
        os.mkdir(plot_dir)
    except OSError:
        sys.excepthook(*sys.exc_info())
    
    plot_dir = os.path.join(plot_dir, _example_name)
    try:
        os.mkdir(plot_dir)
    except OSError: # probably the directory already exists
        sys.excepthook(*sys.exc_info())
    for image in _images_after:
        if image not in _images_before:
            file_name = os.path.split(image)[1]
            target = os.path.join(plot_dir, file_name)
            os.rename(image, target)

def run_example(example_name):
    """
    Run an example as an executable
    """
    os.chdir(os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp'))
    images_before = get_images()
    mangled_name = os.path.split(example_name)[-1] # just the file name
    mangled_name = os.path.splitext(mangled_name)[0] # just the prefix
    log = 'test_examples_'+mangled_name+'.log'
    log = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", log)
    log_file = open(log, 'w')
    print 'Running', example_name, 'logging in', log
    proc = subprocess.Popen([example_name],
                            stdout=log_file, stderr=subprocess.STDOUT)
    proc.communicate('\n')
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
        examples = glob.glob(examples_dir)
        for item in examples:
            proc, log = run_example(item)
            self.assertEqual(proc.returncode, 0, msg="Check logfile "+log)
        self._test_load_root_file_cpp()

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

