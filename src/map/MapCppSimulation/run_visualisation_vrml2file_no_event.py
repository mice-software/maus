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

import maus_cpp.globals
import maus_cpp.converter
import Configuration
from MAUS import MapCppSimulation

class MapCppSimulationVisualisationTestCase(unittest.TestCase):
    """
    Test for visualisation of simulation
    """
    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202
        self.float_tolerance = 1e-9
        self.mapper = MapCppSimulation()
        if not maus_cpp.globals.has_instance():
            maus_cpp.globals.birth(json.dumps(self.configuration))
        self.mapper.birth(json.dumps(self.configuration))
        self.particle = {"primary":{
                "position":{"x":1.,"y":2.,"z":3.},
                "momentum":{"x":4.,"y":5.,"z":6.},
                "spin":{"x":4.,"y":5.,"z":6.},
                "particle_id":-13,
                "random_seed":1,
                "energy":110.,
                "time":7.,
            }}

    @classmethod
    def tearDownClass(self): # pylint: disable=C0103, C0202
        self.mapper.death()

    ######## tests on Process #########
    def test_mc_vrml2file_no_event(self):  # should make a vrml file
        """
        Check we can make a vrml file even when there is no G4 event.

        Only supported option right now is vrml as we need this to feed into the
        event viewer.
        """
        for filename in glob.glob('g4_*.wrl'):
            os.rename(filename, os.environ['MAUS_ROOT_DIR']+'/tmp/'+filename) 
        good_event = {
            "mc_events":[self.particle,self.particle],
            "daq_event_type":"physics_event",
            "maus_event_type":"Spill",
            "run_number":0,
            "spill_number":0,
        }
        result = self.mapper.process(good_event)
        result = maus_cpp.converter.json_repr(result)
        if "errors" in result and result["errors"] != {}:
            raise Exception('test_mc_vis made an error '+str(result["errors"]))
        if len(glob.glob('g4_*.wrl')) < 1:
            raise Exception('test_mc_vis_no_event failed to make a VRML file')
        for filename in glob.glob('g4_*.wrl'):
            os.rename(filename, os.environ['MAUS_ROOT_DIR']+'/tmp/'+filename) 

    configuration = json.loads(Configuration.Configuration().getConfigJSON())
    configuration["verbose_level"] = 2
    configuration["keep_steps"] = True
    configuration["geant4_visualisation"] = True

if __name__ == '__main__':
    unittest.main()
