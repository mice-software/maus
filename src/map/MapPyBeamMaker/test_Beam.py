import unittest
import Beam
import xboa.Hit
import copy

TEST_PRIM = {
    "position":{"x":1.0, "y":2.0, "z":3.0},
    "momentum":{"x":1.0, "y":2.0, "z":3.0},
    "particle_id":-13,
    "energy":235.0,
    "time":2.0,
    "random_seed":0
}

TEST_REF = {'local_weight': 1.0, 'energy': 235.0, 'pid': -13, 'ey': 0.0, 'ex': 0.0, 'ez': 0.0, 'e_dep': 0.0, 'pz': 168.3007057379333, 'event_number': 0, 'px': 56.10023524597777, 'py': 112.20047049195554, 'charge': 0.0, 'station': 0, 'global_weight': 1.0, 'status': 0, 'proper_time': 0.0, 'path_length': 0.0, 'particleNumber': 0, 'particle_number': 0, 'bx': 0.0, 'by': 0.0, 'bz': 0.0, 'sz': 0.0, 'sy': 0.0, 'sx': 0.0, 'eventNumber': 0, 'mass': 105.6583668, 't': 2.0, 'y': 2.0, 'x': 1.0, 'z': 3.0}

class test_Beam(unittest.TestCase):
    @classmethod
    def setUp(cls):
        pass

    def test_birth_particle_generator(self):
        beam = Beam.Beam()
        test_gen = ["n_particles_per_spill"]
        beam._Beam__birth_particle_generator({"n_particles_per_spill":1}, "counter")       
        beam._Beam__birth_particle_generator({"weight":2.}, "binomial")
        beam._Beam__birth_particle_generator({"weight":2.}, "overwrite_existing")
        try:
            beam._Beam__birth_particle_generator({"n_particles_per_spill":0}, "counter")
            raise RuntimeError("Should have made a ValueError")
        except ValueError:
            pass
        try:
            beam._Beam__birth_particle_generator({"weight":0.}, "binomial")
            raise RuntimeError("Should have made a ValueError")
        except ValueError:
            pass
        try:
            beam._Beam__birth_particle_generator({"weight":-1.}, "overwrite_existing")
            raise RuntimeError("Should have made a ValueError")
        except ValueError:
            pass
        

    def test_birth_reference_particle(self):
        beam = Beam.Beam()
        beam._Beam__birth_reference_particle({"reference":TEST_PRIM}) 
        self.assertEqual( beam.reference, xboa.Hit.Hit.new_from_dict(TEST_REF) )
        test_broken = copy.deepcopy(TEST_PRIM)
        test_broken["position"] = 0
        self.assertRaises( ValueError, beam._Beam__birth_reference_particle, ({"reference":test_broken}) )

    def test_birth_transverse_mode(self):
        beam = Beam.Beam()
        beam._Beam__birth_reference_particle({"reference":TEST_PRIM}) 
        self.assertEqual( beam.reference, xboa.Hit.Hit.new_from_dict(TEST_REF) )
        test_broken = copy.deepcopy(TEST_PRIM)
        test_broken["position"] = 0
        self.assertRaises( ValueError, beam._Beam__birth_reference_particle, ({"reference":test_broken}) )

if __name__=="__main__":
    unittest.main()


