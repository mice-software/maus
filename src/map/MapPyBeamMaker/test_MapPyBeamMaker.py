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

#warning/error associated with access to privates
#pylint: disable = W0212
#erroneous complaint because python changes private function names
#pylint: disable = E1101
#pylint name is MAUS standard
#pylint: disable = C0103

import unittest
import copy
import json
import xboa

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
            "n_particles_per_spill":7,
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
        },{
            "reference":TEST_REFERENCE_PI,
            "random_seed_algorithm":"random",
            "weight":2.,
            "transverse":{"transverse_mode":"pencil"},
            "longitudinal":{"longitudinal_mode":"pencil",
                            "momentum_variable":"p"},
            "coupling":{"coupling_mode":"none"}
        }]
    }
}

# diable too many public methods
class TestMapPyBeamMaker(unittest.TestCase): # pylint: disable = R0904
    """ set of tests for MapPyBeamMaker """
    @classmethod
    def setUp(cls): #pylint: disable = C0103
        """Initialise beam_maker object"""
        cls.beam_maker = MapPyBeamMaker()

    def tearDown(cls): #pylint: disable = E0213
        """Close beam_maker object"""
        cls.assertTrue(cls.beam_maker.death())
 
    def test_birth_empty_part_counter(self):
        """Check that we set the seed, particle generator correctly"""
        beam_def = TEST_COUNTER["beam"]
        self.beam_maker._MapPyBeamMaker__birth_empty_particles(beam_def)
        self.assertEqual(self.beam_maker.seed, beam_def["random_seed"])
        self.assertEqual(self.beam_maker.particle_generator, "counter")

    def test_birth_empty_part_binomial(self):
        """
        Check that we set the binomial variables; throw an error if binomial
        variables are out of range
        """
        beam_def = copy.deepcopy(TEST_BINOMIAL["beam"])
        self.beam_maker._MapPyBeamMaker__birth_empty_particles(beam_def)
        self.assertEqual(self.beam_maker.seed, beam_def["random_seed"])
        self.assertEqual(self.beam_maker.particle_generator, "binomial")
        self.assertEqual(self.beam_maker.binomial_n, beam_def["binomial_n"])
        self.assertAlmostEqual(self.beam_maker.binomial_p, \
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
        """
        Check that we throw an error if particle generator unknown
        """
        beam_def = copy.deepcopy(TEST_COUNTER["beam"])
        beam_def["particle_generator"] = "BLIB"
        self.assertRaises(ValueError, 
                        self.beam_maker._MapPyBeamMaker__birth_empty_particles,
                        beam_def)

    def test_birth(self):
        """
        Check that the birth goes okay
        """
        self.assertTrue(self.beam_maker.birth(json.dumps(TEST_OVERWRITE)))
        self.assertEqual(len(self.beam_maker.beams), 3)
        self.assertEqual(self.beam_maker.particle_generator,
                        "overwrite_existing")
        self.assertFalse(self.beam_maker.birth(""))
        self.assertFalse(self.beam_maker.birth(json.dumps({})))

    def test_process_check_spill(self):
        """
        Check that we throw an error if the spill is bad
        """
        self.assertRaises(KeyError,
               self.beam_maker._MapPyBeamMaker__process_check_spill, {"mc":""})
        self.assertEqual( \
                self.beam_maker._MapPyBeamMaker__process_check_spill({}),\
                {"mc":[]})
        self.assertEqual(
               self.beam_maker._MapPyBeamMaker__process_check_spill({"mc":[]}),\
               {"mc":[]} )
        self.assertEqual(
             self.beam_maker._MapPyBeamMaker__process_check_spill({"mc":[""]}),\
             {"mc":[""]})

    def test_process_gen_empty_ovrwrt(self):
        """
        Check that we generate empty primaries for each existing particle in
        overwrite_existing mode
        """
        spill = {"mc":[{}, {}, {}]}
        self.beam_maker.birth(json.dumps(TEST_OVERWRITE))
        self.assertEqual(
           self.beam_maker._MapPyBeamMaker__process_gen_empty(spill),
           [{"primary":{}}]*3)
        self.assertEqual(
           spill["mc"],
           [{"primary":{}}]*3)

    def test_process_gen_empty_counter(self):
        """
        Check that we append correct number of empty primaries in counter mode
        """
        spill = {"mc":[{}, {}, {}]}
        self.beam_maker.birth(json.dumps(TEST_COUNTER))
        self.assertEqual(
           self.beam_maker._MapPyBeamMaker__process_gen_empty(spill),
           [{'primary':{}}]*16)
        self.assertEqual(spill["mc"], [{}]*3+[{'primary':{}}]*16)

    def test_process_gen_empty_binomial(self):
        """
        Check that we append a random number of empty primaries within allowed
        range
        """
        spill = {"mc":[{}, {}, {}]}
        self.beam_maker.birth(json.dumps(TEST_BINOMIAL))
        particles = self.beam_maker._MapPyBeamMaker__process_gen_empty(spill)
        for part in particles:
            self.assertEqual(part, {"primary":{}})
        self.assertTrue(len(particles) > 1) # true with high probability
        self.assertTrue(len(particles) < 51) # always true

    def test_process_choose_beam_cntr(self):
        """
        Check that we sample from the correct beam for each particle when in
        counter mode
        """
        self.beam_maker.birth(json.dumps(TEST_COUNTER))
        for i in range(16):
            a_beam = self.beam_maker._MapPyBeamMaker__process_choose_beam(i)
            if i < 9:
                self.assertEqual(a_beam.particle_seed_algorithm, 
                                 "incrementing_random")
            else:
                self.assertEqual(a_beam.particle_seed_algorithm, 
                                 "random")

    def test_process_choose_beam_bnml(self):
        """
        Check that we sample from beams with large weight in binomial mode
        """
        self.beam_maker.birth(json.dumps(TEST_BINOMIAL))
        beam_one, beam_two = 0, 0
        for i in range(16):
            a_beam = self.beam_maker._MapPyBeamMaker__process_choose_beam(i)
            if a_beam.particle_seed_algorithm == "incrementing_random":
                beam_one += 1
            if a_beam.particle_seed_algorithm == "random":
                beam_two += 1
        self.assertGreater(beam_two, beam_one)

    def test_process_choose_beam_ovrwrt(self):
        """
        Check that we sample from beams with large weight in ovewrwrite mode
        """
        self.beam_maker.birth(json.dumps(TEST_OVERWRITE))
        beam_counter = [0, 0, 0]
        for i in range(16):
            a_beam = self.beam_maker._MapPyBeamMaker__process_choose_beam(i)
            for j, algo in enumerate(["incrementing", "beam_seed", "random"]):
                if a_beam.particle_seed_algorithm == algo:
                    beam_counter[j] += 1
        self.assertGreater(beam_counter[1], beam_counter[0])
        self.assertGreater(beam_counter[2], beam_counter[0])

    def test_process(self):
        """
        Check overall that process works okay
        """
        self.beam_maker.birth(json.dumps(TEST_OVERWRITE))
        spill_out = self.beam_maker.process(json.dumps({"mc":[{}]*50}))
        spill_out = json.loads(spill_out)
        self.assertEqual(len(spill_out["mc"]), 50)
        primary_list = [particle["primary"] for particle in spill_out["mc"]]
        for primary in primary_list:
            hit = xboa.Hit.Hit.new_from_maus_object("maus_primary", primary, 0)
            self.assertTrue(hit.check())
        spill_out = json.loads(self.beam_maker.process(json.dumps({"mc":""})))
        self.assertEqual(len(spill_out["errors"]["MapPyBeamMaker"]), 1)
        
if __name__ == "__main__":
    unittest.main()


