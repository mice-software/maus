"""
test_accumulate_coloured_particles.py
"""
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

import unittest
import os
import subprocess
import glob

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "simulate_mice.py")
TEST_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                        "test_simulation", "test_accumulate_coloured_particles")
CONFIG_PATH = os.path.join(TEST_DIR, "test_particles_config.txt")
FILE_NAME = os.environ['MAUS_ROOT_DIR']+'/tmp/accumulate_tracks'

def run_simulations():
    """
    Run simulation to generate some data. We only want to do this once, so I
    pull it out into a separate part of the test.
    """
    test_out = open(FILE_NAME, 'w')
    subproc = subprocess.Popen([SIM_PATH, '-configuration_file', CONFIG_PATH], \
                                      stdout=test_out, stderr=subprocess.STDOUT)
    subproc.wait()
    test_out.close()

def file_cleanup():
    """
    This method cleans up the vrml and simulation.out files produced by the 
    simulation. The first method must check the vrml file but it is not needed 
    afterwards.
    """
    for filename in glob.glob('g4_*.wrl'):
        os.remove(filename)
    for filename in glob.glob('*.out'):
        os.remove(filename)

class AccumulateColouredParticles(unittest.TestCase): #pylint: disable = R0904
    """
    This class has two tests. One checks that particles are accumulated into one
    vrml output file when the tag is selected in configuration defaults. The 
    second checks that the DrawByParticleID model has been registered so 
    particles will have specified colours in the vrml.
    """
    def setUp(self): # pylint: disable=C0103, C0202
        """ Run Simulation """
        run_simulations()
    
    def test_for_accumulated_tracks(self):  #pylint: disable=R0201
        """ Check that we have accumulated particles """
        if len(glob.glob('g4_*.wrl')) > 1:
            raise Exception('more than one vrml, tracks not accumulated')
        file_cleanup()

    def test_coloured_particles(self):
        """ Check that we have coloured particles """
        test_in = open(FILE_NAME, 'r')
        found_model = 0
        for lines in test_in.readlines():
            if lines.find('G4TrajectoryDrawByParticleID') >= 0:
                found_model += 1
        self.assertEqual(found_model, 1, 'DrawByParticleID model not found')
        file_cleanup()

if __name__ == '__main__':
    unittest.main()
