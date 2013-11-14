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

# pylint: disable=C0103

"""
Test maus_cpp.simulation
"""

import unittest
import os
import json
import StringIO

import Configuration
import maus_cpp.globals
import maus_cpp.simulation

class MiceModuleTestCase(unittest.TestCase): # pylint: disable=R0904
    """
    Test maus_cpp.mice_module
    """
    def setUp(self):
        """set globals"""
        conf = StringIO.StringIO(u"keep_tracks = True")
        self.config = Configuration.Configuration().getConfigJSON(conf, False)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(self.config)

    def tearDown(self):
        maus_cpp.globals.death()

    def _test_track_particles_bad(self):
        """test maus_cpp.simulation.track_particles"""
        try:
            maus_cpp.simulation.track_particles()
            self.assertTrue(False, "Should have thrown TypeError")
        except TypeError:
            pass
        try:
            maus_cpp.simulation.track_particles(1.)
            self.assertTrue(False, "Should have thrown TypeError")
        except TypeError:
            pass
        try:
            maus_cpp.simulation.track_particles("abc")
            self.assertTrue(False, "Should have thrown RuntimeError")
        except RuntimeError:
            pass
        try:
            maus_cpp.simulation.track_particles("{}")
            self.assertTrue(False, "Should have thrown RuntimeError")
        except RuntimeError:
            pass

    def test_track_particles_good(self):
        out = maus_cpp.simulation.track_particles("[]")
        self.assertEqual(out, "[]")
        ref = {
            "position":{"x":0., "y":0., "z":0.},
            "momentum":{"x":0., "y":0., "z":1.},
            "energy":1000., "particle_id":2212, "time":0.0, "random_seed":0
        }
        mc_in = [{"primary":ref}, {"primary":ref}]
        mc_out = maus_cpp.simulation.track_particles(json.dumps(mc_in))
        mc_out = json.loads(mc_out)
        self.assertEqual(len(mc_in), len(mc_out))                                 
        for i, event in enumerate(mc_out):
            self.assertTrue("primary" in event.keys())
            prim_in = mc_in[i]["primary"]
            prim_out = mc_out[i]["primary"]
            for item in ["energy", "time"]:
                self.assertAlmostEqual(prim_in[item], prim_out[item])
            for item in ["particle_id", "random_seed"]:
                self.assertEqual(prim_in[item], prim_out[item])
            for vec in ["position", "momentum"]:
                for axis in ["x", "y", "z"]:
                    self.assertAlmostEqual(prim_in[vec][axis],
                                           prim_out[vec][axis])
            # check that the MC ran and did something, anything
            self.assertGreater(len(mc_out[i]["tracks"]), 0)

if __name__ == "__main__":
    unittest.main()

