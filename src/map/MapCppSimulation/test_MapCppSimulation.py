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
Test for MapCppSimulation - note also separate tests for visualisation
"""
# pylint: disable=C0103
# pylint: disable=R0904

import json
import unittest
import os
import subprocess

import Configuration
import maus_cpp.globals
import maus_cpp.converter
from MAUS import MapCppSimulation

class MapCppSimulationTestCase(unittest.TestCase):
    """
    Check simulation works for bad inputs, good inputs; and throws appropriate
    errors for bad inputs. Note that there's a lot of testing elsewhere for the
    detailed logic in Simulation package (e.g. test_cpp_unit) so I don't repeat
    those tests here.
    """
    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202
        """
        Check we can construct the mapper; set primary particle
        """
        self.float_tolerance = 1e-9
        self.mapper = MapCppSimulation()
        if not maus_cpp.globals.has_instance():
            maus_cpp.globals.birth(json.dumps(self.configuration))
        self.mapper.birth(json.dumps(self.configuration))
        self.particle = {"primary":{
                "position":{"x":1.,"y":2.,"z":3.},
                "momentum":{"x":4.,"y":5.,"z":6.},
                "spin":{"x":0., "y":0., "z":1.},
                "particle_id":-13,
                "random_seed":1,
                "energy":110.,
                "time":7.,
            }}

    @classmethod
    def tearDownClass(self): # pylint: disable=C0103, C0202
        """
        Check we can call death on the mapper
        """
        self.mapper.death()

    ######## tests on Process #########
    def test_birth(self):  # pylint: disable=R0201
        """Check we get an error for bad input to birth"""
        test_mapper = MapCppSimulation()
        test_mapper.birth(json.dumps(self.configuration))

    def test_empty(self):
        """Check mapper runs for empty string, returning an error"""
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertIn("errors", doc)
        self.assertIn("MapCppSimulation", doc["errors"])

    def test_no_mc_branch(self):
        """Check mapper runs for no mc string, returning an error"""
        result = self.mapper.process("{}")
        doc = maus_cpp.converter.json_repr(result)
        self.assertIn("errors", doc)
        self.assertIn("MapCppSimulation", doc["errors"])

    def test_mc_bad_type(self):
        """Check mapper runs for mc with wrong type, returning an error"""
        result = self.mapper.process("""{"mc_events" : 0.0}""")
        doc = maus_cpp.converter.json_repr(result)
        self.assertIn("errors", doc)
        self.assertIn("MapCppSimulation", doc["errors"])

    def test_mc_good(self):
        """
        Check mapper runs for mc good. Check it tracks primaries by testing
        the initial value of track[0], etc
        """
        good_event = {
            "mc_events":[self.particle,self.particle],
            "daq_event_type":"physics_event",
            "daq_data":{},
            "maus_event_type":"Spill",
            "run_number":0,
            "spill_number":0,
        }
        result = self.mapper.process(good_event)
        doc = maus_cpp.converter.json_repr(result)
        self.assertEqual(doc["errors"], {})
        ev_0 = doc["mc_events"][0]["tracks"][0]
        ev_1 = doc["mc_events"][1]["tracks"][0]
        for event in [ev_0, ev_1]:
            for pos in ["x", "y", "z"]:
                self.assertAlmostEqual(
                    event["initial_position"][pos],
                    self.particle["primary"]["position"][pos], msg=str(event)
                 )
            p_in = event["initial_momentum"]
            energy = (p_in["x"]**2+p_in["y"]**2+p_in["z"]**2+105.658**2)**0.5
            self.assertAlmostEqual( self.particle["primary"]["energy"],
                                    energy, msg=str(event), delta = 0.01 )
            self.assertIn("final_momentum", event)
            self.assertIn("final_position", event)

    def test_visualisation(self):
        """ Call test_visualisation """
        test = os.path.join(os.environ['MAUS_ROOT_DIR'], 'src', 'map', \
                           'MapCppSimulation', 'run_visualisation_for_tests.py')
        ps = subprocess.Popen(['python', test])
        ps.wait()
        self.assertEqual(ps.returncode, 0, msg='Failed to run visualisation')

    def test_visualisation_no_event(self):
        """ Call test_visualisation_no_event """
        test = os.path.join(os.environ['MAUS_ROOT_DIR'], 'src', 'map', \
                           'MapCppSimulation', \
                                      'run_visualisation_vrml2file_no_event.py')
        ps = subprocess.Popen(['python', test])
        ps.wait()
        self.assertEqual(ps.returncode, 0, msg='Failed to run visualisation')


    configuration = json.loads(Configuration.Configuration().getConfigJSON())
    configuration["verbose_level"] = 2
    configuration["keep_steps"] = True

if __name__ == '__main__':
    unittest.main()

