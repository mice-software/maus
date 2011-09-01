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

"""Script to test the BeamMaker Application"""

import unittest
import copy
import json

import xboa.Common as Common

from MapPyBeamMaker import MapPyBeamMaker

TEST_REFERENCE_MU = {
    "position":{"x":0.0, "y":-0.0, "z":-4700.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13,
    "energy":235.0,
    "time":0.0,
    "random_seed":0
}

TEST_REFERENCE_PI = {
    "position":{"x":0.0, "y":-0.0, "z":-4700.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":211,
    "energy":235.0,
    "time":0.0,
    "random_seed":0
}

TEST_COUNTER = {
    "beam":{
        "particle_generator":"counter",
        "random_seed":2,
        "definitions":[{
            "reference":TEST_REFERENCE_MU,
            "random_seed_algorithm":"incrementing_random",
            "n_particles_per_spill":9,
            "transverse":{"transverse_mode":"pencil"},
            "longitudinal":{"longitudinal_mode":"pencil",
                            "momentum_variable":"p"},
            "coupling":{"coupling_mode":"none"}
        },{
            "reference":TEST_REFERENCE_MU,
            "random_seed_algorithm":"random",
            "n_particles_per_spill":9,
            "transverse":{"transverse_mode":"pencil"},
            "longitudinal":{"longitudinal_mode":"pencil",
                            "momentum_variable":"p"},
            "coupling":{"coupling_mode":"none"}
        }]
    }
}

TEST_BINOMIAL = {
    "beam":{
        "particle_generator":"binomial",
        "binomial_n":50,
        "binomial_p":0.5,
        "random_seed":5,
        "definitions":[{
            "reference":TEST_REFERENCE_MU,
            "random_seed_algorithm":"incrementing_random",
            "weight":1e-6,
            "transverse":{"transverse_mode":"pencil"},
            "longitudinal":{"longitudinal_mode":"pencil",
                            "momentum_variable":"p"},
            "coupling":{"coupling_mode":"none"}
        },{
            "reference":TEST_REFERENCE_PI,
            "random_seed_algorithm":"random",
            "weight":1.,
            "transverse":{"transverse_mode":"pencil"},
            "longitudinal":{"longitudinal_mode":"pencil",
                            "momentum_variable":"p"},
            "coupling":{"coupling_mode":"none"}
        }]
    }
}

TEST_OVERWRITE = {
    "beam":{
        "particle_generator":"overwrite_existing",
        "random_seed":5,
        "definitions":[{
            "reference":TEST_REFERENCE_MU,
            "random_seed_algorithm":"incrementing",
            "weight":1e-6,
            "transverse":{"transverse_mode":"pencil"},
            "longitudinal":{"longitudinal_mode":"pencil",
                            "momentum_variable":"p"},
            "coupling":{"coupling_mode":"none"}
        },{
            "reference":TEST_REFERENCE_PI,
            "random_seed_algorithm":"beam_seed",
            "weight":1.,
            "transverse":{"transverse_mode":"pencil"},
            "longitudinal":{"longitudinal_mode":"pencil",
                            "momentum_variable":"p"},
            "coupling":{"coupling_mode":"none"}
        }]
    }
}

class MapPyBeamMakerTestCase(unittest.TestCase):
    """ set of tests for MapPyBeamMaker """
    @classmethod
    def setUpClass(cls):
        cls.beam_maker = MapPyBeamMaker()

    @classmethod
    def tearDownClass(cls):
        pass
 
    def test_init(self):
        MapPyBeamMaker()

    def test_birth_empty_particles_counter(self):
        beam_def = TEST_COUNTER["beam"]
        self.beam_maker._MapPyBeamMaker__birth_empty_particles(beam_def)
        self.assertEqual(self.beam_maker.seed, beam_def["random_seed"])
        self.assertEqual(self.beam_maker.particle_generator, "counter")

    def test_birth_empty_particles_counter(self):
        beam_def = TEST_COUNTER["beam"]
        self.beam_maker._MapPyBeamMaker__birth_empty_particles(beam_def)
        self.assertEqual(self.beam_maker.seed, beam_def["random_seed"])
        self.assertEqual(self.beam_maker.particle_generator, "counter")

    def test_birth_empty_particles_binomial(self):
        beam_def = copy.deepcopy(TEST_BINOMIAL["beam"])
        self.beam_maker._MapPyBeamMaker__birth_empty_particles(beam_def)
        self.assertEqual(self.beam_maker.seed, beam_def["random_seed"])
        self.assertEqual(self.beam_maker.particle_generator, "binomial")
        self.assertEqual(self.beam_maker.binomial_n, beam_def["binomial_n"])
        self.assertAlmostEqual(self.beam_maker.binomial_p,\
                               beam_def["binomial_p"])
        beam_def["binomial_n"] = -1
        self.assertRaises(ValueError,
                         self.beam_maker._MapPyBeamMaker__birth_empty_particles,
                         beam_def)
        beam_def["binomial_n"] = 50
        beam_def["binomial_p"] = 0.
        self.assertRaises(ValueError,
                         self.beam_maker._MapPyBeamMaker__birth_empty_particles,
                         beam_def)
        beam_def["binomial_p"] = 1.0000001
        self.assertRaises(ValueError,
                         self.beam_maker._MapPyBeamMaker__birth_empty_particles,
                         beam_def)

    def test_birth_empty_particles_bad(self):
        beam_def = copy.deepcopy(TEST_COUNTER["beam"])
        beam_def["particle_generator"] = "BLIB"
        self.assertRaises(ValueError, 
                        self.beam_maker._MapPyBeamMaker__birth_empty_particles,
                        beam_def)

    def test_birth(self):
        self.assertTrue(self.beam_maker.birth(json.dumps(TEST_BINOMIAL)))
        self.assertEqual(len(self.beam_maker.beams), 2)
        self.assertEqual(self.beam_maker.particle_generator, "binomial")
        self.assertFalse(self.beam_maker.birth(""))
        self.assertFalse(self.beam_maker.birth(json.dumps({})))

    def test_process_check_spill(self):
        self.assertRaises(KeyError,
               self.beam_maker._MapPyBeamMaker__process_check_spill, {})
        self.assertRaises(KeyError,
               self.beam_maker._MapPyBeamMaker__process_check_spill, {"mc":""})
        self.assertTrue(
               self.beam_maker._MapPyBeamMaker__process_check_spill({"mc":[]}))
        self.assertTrue(
               self.beam_maker._MapPyBeamMaker__process_check_spill({"mc":[""]}))

    def test_process_generate_multiple_primaries():

if __name__ == "__main__":
    unittest.main()


