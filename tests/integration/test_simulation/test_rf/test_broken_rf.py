#!/usr/bin/env python2.7

import unittest
import run_simulation
import ErrorHandler

class BrokenRFTestCase(unittest.TestCase):
    """
    Test that when we have a broken RF set up, simulation fails. In this case
    the rf cavity is behind the reference particle so never gets crossed.
    """

    def test_single_rf(self):
        my_datacards = """
simulation_geometry_filename = "BrokenPillBox.dat"
simulation_reference_particle = {"position":{"x":0.0, "y":0.0, "z":-1000.0}, "momentum":{"x":1.0, "y":0.0, "z":0.0}, "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10}
"""
        my_input = """{"mc":[{"position":{"x":0.0, "y":-0.0, "z":0.0}, "momentum":{"x":0.0, "y":0.0, "z":1.0}, "particle_id":-13, "energy":210.0, "time":0.0, "random_seed":10}]}"""
        try:
            run_simulation.run(my_input, my_datacards, 'test_broken_rf.json')
            raise RuntimeError('Expected AssertionError - but none found')
        except AssertionError:
            pass

if __name__ == '__main__':
    unittest.main()

