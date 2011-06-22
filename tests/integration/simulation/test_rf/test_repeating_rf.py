#!/usr/bin/env python2.7

import unittest
import run_simulation

class RepeatingRFTestCase(unittest.TestCase):
    """
    Test that when we have many rf cavities, they get phased okay.
    """
    def test_repeating_rf(self):
        my_datacards = """simulation_geometry_filename = "RepeatingPillBox.dat" """ # check default ref particle is ok
        my_input = """{"mc":[{"position":{"x":0.0, "y":-0.0, "z":0.0}, "momentum":{"x":0.0, "y":0.0, "z":1.0}, "particle_id":-13, "energy":210.0, "time":0.0, "random_seed":10}]}"""
        run_simulation.run(my_input, my_datacards, 'test_repeating_rf.json')

if __name__ == '__main__':
    unittest.main()
