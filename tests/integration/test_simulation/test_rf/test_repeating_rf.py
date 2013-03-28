#!/usr/bin/env python2.7
"""test_repeating_rf"""
import unittest
import run_simulation#pylint: disable =F0401

class RepeatingRFTestCase(unittest.TestCase):#pylint: disable =R0904
    """
    Test that when we have many rf cavities, they get phased okay.
    """
    def test_repeating_rf(self):#pylint: disable =R0201
        """test_repeating_rf"""
         # check default ref particle is ok
        my_datacards = """simulation_geometry_filename = "RepeatingPillBox.dat" """#pylint: disable =C0301
        my_input = """{"mc":[{"position":{"x":0.0, "y":-0.0, "z":0.0}, "momentum":{"x":0.0, "y":0.0, "z":1.0}, "particle_id":-13, "energy":210.0, "time":0.0, "random_seed":10}]}"""#pylint: disable =C0301
        run_simulation.run(my_input,
                           my_datacards,
                           'test_repeating_rf.json')

if __name__ == '__main__':
    unittest.main()

