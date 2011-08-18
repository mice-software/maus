"""Script to test the BeamMaker Application"""


import json
import unittest
import ErrorHandler
import numbers

from Configuration import Configuration
from MapPyBeamMaker import MapPyBeamMaker

import xboa.Common as Common

CHARGE_SPECIES = +1
JSON_SAMPLE = {
    "beam":{
        "particle_generator":"counter",
        "random_seed":2,
        "definitions":[{
            "n_particles_per_spill":9,
            "reference":{
                "position":{"x":0.0, "y":-0.0, "z":-4700.0},
                "momentum":{"x":0.0, "y":0.0, "z":1.0},
                "particle_id":-13*CHARGE_SPECIES,
                "energy":235.0,
                "time":0.0,
                "random_seed":0
            },
            "transverse":{
                "transverse_mode":"penn",
                "emittance_4d":6.*Common.units["mm"],
                "beta_4d":333.*Common.units["mm"],
                "alpha_4d":0.,
                "normalised_angular_momentum":0.,
                "bz":4.*Common.units["T"]
            },
            "longitudinal":{
                "longitudinal_mode":"sawtooth_time",
                "t_start":0.,
                "t_end":1.e6,
                "momentum_defined_by":"p",
                "sigma_p":20.*Common.units["MeV/c"]
            },
            "coupling":{
                "coupling_mode":"none"
            }
        },{
            "n_particles_per_spill":2,
            "reference":{
                "position":{"x":0.0, "y":-0.0, "z":-4700.0},
                "momentum":{"x":0.0, "y":0.0, "z":1.0},
                "particle_id":211*CHARGE_SPECIES,
                "energy":207.0,
                "time":0.0,
                "random_seed":0
            },
            "transverse":{
                "transverse_mode":"penn",
                "emittance_4d":6.*Common.units["mm"],
                "beta_4d":333.*Common.units["mm"],
                "alpha_4d":0.,
                "normalised_angular_momentum":0.,
                "bz":4.*Common.units["T"]
            },
            "longitudinal":{
                "longitudinal_mode":"sawtooth_time",
                "t_start":0.,
                "t_end":1.e6,
                "momentum_defined_by":"p",
                "sigma_p":20.*Common.units["MeV/c"]
            },
            "coupling":{
                "coupling_mode":"none"
            }                                       
        }]
    }
}


class MapPyBeamMakerTestCase(unittest.TestCase):
    """ set of tests for MapPyBeamMaker """
    @classmethod
    def setUpClass(cls):
        pass

    @classmethod
    def tearDownClass(cls):
        pass
 
    def test_init(self):
        MapPyBeamMaker()

    def test_integration(self):
        """
        Check the whole flow works to produce a set of primaries
        """
        beam_maker = MapPyBeamMaker()
        beam_maker.birth(json.dumps(JSON_SAMPLE))
        for i in range(2):
            spill = beam_maker.process( json.dumps({"mc":[]}) )
            print spill

if __name__ == "__main__":
    unittest.main()


