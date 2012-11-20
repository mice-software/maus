#!/usr/bin/env python2.7
"""test_single_rf"""
import unittest
import run_simulation#pylint: disable =F0401

class SingleRFTestCase(unittest.TestCase):#pylint: disable =R0904
    """SingleRFTestCase"""
    def test_single_rf(self):#pylint: disable =R0201
        """test_single_rf"""
        my_datacards = """
simulation_geometry_filename = "PillBox.dat"
simulation_reference_particle = {"position":{"x":0.0, "y":0.0, "z":-1000.0}, "momentum":{"x":1.0, "y":0.0, "z":0.0}, "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10}
        """
        my_input = """{"mc":[{"primary":{"position":{"x":0.0, "y":-0.0, "z":0.0}, "momentum":{"x":0.0, "y":0.0, "z":1.0}, "particle_id":-13, "energy":210.0, "time":0.0, "random_seed":10}}]}"""#pylint: disable =C0301
        run_simulation.run(my_input,
                           my_datacards,
                           'test_single_rf.json')

if __name__ == '__main__':
    unittest.main()

