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
Test for MapCppSimulation visualisation; we need a separate test as want to
reinitialise geant4 with visualisation on
"""
# pylint: disable=C0103
# pylint: disable=R0904

import glob
import os
import json
import unittest

from MapCppSimulation import MapCppSimulation

class MapCppSimulationVisualisationTestCase(unittest.TestCase):
    """
    Test for visualisation of simulation
    """
    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202
        self.float_tolerance = 1e-9
        self.mapper = MapCppSimulation()
        success = self.mapper.birth(json.dumps(self.configuration))
        self.particle = {"primary":{
                "position":{"x":1.,"y":2.,"z":3.},
                "momentum":{"x":4.,"y":5.,"z":6.},
                "particle_id":-13,
                "random_seed":1,
                "energy":110.,
                "time":7.,
                "statistical_weight":1.
            }}
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

    @classmethod
    def tearDownClass(self): # pylint: disable=C0103, C0202
        success = self.mapper.death()
        if not success:
            raise Exception('DestructFail', 'Could not kill worker')
        self.mapper = None

    ######## tests on Process #########
    def test_mc_vrml2file(self):  # should make a vrml file
        """
        Check we can make a vrml file
        
        Only supported option right now is vrml as we need this to feed into the
        event viewer.
        """
        good_event = {
            "mc_events":[self.particle,self.particle]
        }
        result = self.mapper.process(json.dumps(good_event))
        if "errors" in result:
            raise Exception('test_mc_vis made an error')
        if len(glob.glob('g4_*.wrl')) < 1:
            raise Exception('test_mc_vis failed to make a VRML file')
        for filename in glob.glob('g4_*.wrl'):
            os.rename(filename, os.environ['MAUS_ROOT_DIR']+'/tmp/'+filename) 

    configuration = {
      "verbose_level":2,
      "simulation_geometry_filename":"Test.dat",
      "reconstruction_geometry_filename":"Test.dat",
      "maximum_number_of_steps":1000,
      "keep_steps":True,
      "keep_tracks":True,
      "simulation_reference_particle":{
        "position":{"x":0.0, "y":-0.0, "z":-4700.0},
        "momentum":{"x":0.0, "y":0.0, "z":1.0},
        "particle_id":-13,
        "energy":226.0,
        "time":0.0,
        "random_seed":10
      },
      "geant4_visualisation":True,
      "visualisation_viewer":"VRML2FILE",
      "visualisation_theta":90.,
      "visualisation_phi":90.,
      "visualisation_zoom":1.,
      "accumulate_tracks":0,
      "default_vis_colour":{"red":0. , "green":100.  ,"blue":0.},
      "pi_plus_vis_colour":{"red":255. , "green":250.  ,"blue":240.},
      "pi_minus_vis_colour":{"red":105. , "green":105.  ,"blue":105.},
      "mu_plus_vis_colour":{"red":25. , "green":25.  ,"blue":112.},
      "mu_minus_vis_colour":{"red":135. , "green":206.  ,"blue":250.},
      "e_plus_vis_colour":{"red":250. , "green":0.  ,"blue":0.},
      "e_minus_vis_colour":{"red":250. , "green":69.  ,"blue":0.},
      "gamma_vis_colour":{"red":255. , "green":20.  ,"blue":147.},
      "neutron_vis_colour":{"red":139. , "green":69.  ,"blue":19.},
      "photon_vis_colour":{"red":255. , "green":255.  ,"blue":0.},
      "physics_model":"QGSP_BERT",
      "physics_processes":"standard",
      "reference_physics_processes":"mean_energy_loss",
      "particle_decay":True,
      "charged_pion_half_life":-1.,
      "muon_half_life":-1.,
      "production_threshold":0.5,
    }
if __name__ == '__main__':
    unittest.main()
