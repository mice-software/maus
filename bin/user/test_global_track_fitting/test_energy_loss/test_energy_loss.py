import unittest
import json
import os

import xboa.common
import Configuration
import maus_cpp.globals
import maus_cpp.global_error_tracking
import maus_cpp.simulation

class TestErrorTrackingEnergyLoss(unittest.TestCase):

    def setUp(self):
        configuration = Configuration.Configuration().\
                                             getConfigJSON(command_line_args=False)
        configuration = json.loads(configuration)
        path = os.path.expandvars("${MAUS_ROOT_DIR}/tests/integration/"+\
                                "test_global_track_fitting/test_energy_loss/"+\
                                "test_energy_loss_geometry.dat")
        configuration["simulation_geometry_filename"] = path
        configuration["physics_processes"] = "mean_energy_loss"
        configuration = json.dumps(configuration)
        maus_cpp.globals.birth(configuration)
        maus_cpp.global_error_tracking.enable_material("MICE_LITHIUM_HYDRIDE")
        maus_cpp.global_error_tracking.enable_material("lH2")
        maus_cpp.global_error_tracking.enable_material("LITHIUM_HYDRIDE")
        self.tracking = maus_cpp.global_error_tracking.GlobalErrorTracking()
        self.tracking.set_energy_loss_model("bethe_bloch_forwards")

    def error_tracking_e_loss(self, energy_in):
        momentum = (energy_in**2 - xboa.common.pdg_pid_to_mass[13]**2)**0.5
        psv = [0., 0., 0., 0., float(energy_in), 0., 0., momentum]
        cov = [[0.]*6]*6
        psv, cov = self.tracking.propagate_errors(psv, cov, 2000.)
        energy_out = psv[4]
        return energy_in-energy_out

    def g4_e_loss(self, energy_in):
        primary_list = [{'primary':
              {
                'position':{'x':0., 'y':0., 'z':0.},
                'momentum':{'x':0., 'y':0., 'z':1.},
                'particle_id':-13,
                'energy':energy_in,
                'time':0.,
                'random_seed':0
              }
            }]
        primary_str = json.dumps(primary_list)
        output = json.loads(maus_cpp.simulation.track_particles(primary_str))
        vhit_1 = output[0]["virtual_hits"][0]
        energy_out = (vhit_1["momentum"]["z"]**2+\
                      xboa.common.pdg_pid_to_mass[13]**2)**0.5
        return energy_in - energy_out

    def test_energy_loss(self):
        for energy_in in [i*10+150 for i in range(20)]:
            error_tracking_de = self.error_tracking_e_loss(energy_in)
            g4_de = self.g4_e_loss(energy_in)
            print energy_in, error_tracking_de, g4_de
        self.assertTrue(False, msg="Need to make this a test and add more materials")

if __name__ == "__main__":
    unittest.main()
