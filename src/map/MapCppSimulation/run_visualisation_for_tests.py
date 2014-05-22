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
Test for MapCppSimulation visualisation; we need a separate test as want to
reinitialise geant4 with visualisation on
"""
# pylint: disable=C0103
# pylint: disable=R0904

import glob
import os
import json
import unittest

import Configuration
from MapCppSimulation import MapCppSimulation

class MapCppSimulationVisualisationTestCase(unittest.TestCase):
    """
    Test for visualisation of simulation
    """
    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202
        self.float_tolerance = 1e-9
        self.mapper = MapCppSimulation()
        success = self.mapper.birth(json.dumps(self.configuration))
        self.particle = {"primary":{
                "position":{"x":1.,"y":2.,"z":3.},
                "momentum":{"x":4.,"y":5.,"z":6.},
                "spin":{"x":0., "y":0., "z":1.},
                "particle_id":-13,
                "random_seed":1,
                "energy":110.,
                "time":7.,
                "statistical_weight":1.
            }}
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

    @classmethod
    def tearDownClass(self): # pylint: disable=C0103, C0202
        success = self.mapper.death()
        if not success:
            raise Exception('DestructFail', 'Could not kill worker')
        self.mapper = None

    ######## tests on Process #########
    def test_mc_vrml2file(self):  # should make a vrml file
        """
        Check we can make a vrml file
        
        Only supported option right now is vrml as we need this to feed into the
        event viewer.
        """
        good_event = {
            "mc_events":[self.particle,self.particle]
        }
        result = self.mapper.process(json.dumps(good_event))
        if "errors" in result:
            raise Exception('test_mc_vis made an error')
        if len(glob.glob('g4_*.wrl')) < 1:
            raise Exception('test_mc_vis failed to make a VRML file')
        for filename in glob.glob('g4_*.wrl'):
            os.rename(filename, os.environ['MAUS_ROOT_DIR']+'/tmp/'+filename) 

    configuration = json.loads(Configuration.Configuration().getConfigJSON())
    configuration["verbose_level"] = 2
    configuration["keep_steps"] = True
    configuration["geant4_visualisation"] = True

if __name__ == '__main__':
    unittest.main()
