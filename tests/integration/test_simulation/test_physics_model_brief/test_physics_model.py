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
High level tests for the beam maker - some pushed back from unit tests because
they are too slow. Check that we can generate appropriate distributions and that
the simulate_mice.py executable interfaces correctly.

Use Kolmogorov Smirnov test to compare distributions a lot of the time...
"""

import unittest
import subprocess
import os
import sys  

from xboa.Bunch import Bunch #pylint: disable=F0401

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "simulate_mice.py")
PLOT_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                                    "plots", "physics_list")
TEST_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                                "test_simulation", "test_physics_model_brief")
SETUP_DONE = False
CONV_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "utilities", "root_to_json.py")

def make_plot_dir():
    """
    Make a directory to contain plots from this test
    """
    try:
        os.mkdir(PLOT_DIR)
    except OSError:
        pass # dir already exists

def run_simulation(ref_phys, phys, dec, pi_half, mu_half, prod): #pylint: disable = R0913, C0301
    """
    Run simulation to generate some data. We only want to do this once, so I
    pull it out into a separate part of the test.
    """
    out_name = os.path.join(MAUS_ROOT_DIR, "tmp", "test_physics_model.")
    for value in [ref_phys, phys, dec, pi_half, mu_half]:
        out_name += str(value)+"_"
    out_name += str(prod)
    log_file = open(out_name+".log", "w")
    print "Running", out_name,
    sys.stdout.flush()
    config = os.path.join(TEST_DIR, 'physics_model_config.py')
    proc = subprocess.Popen([SIM_PATH, '-configuration_file', config, 
      "-reference_physics_processes", str(ref_phys),
      "-physics_processes", str(phys),
      "-particle_decay", str(dec),
      "-charged_pion_half_life", str(pi_half),
      "-muon_half_life", str(mu_half),
      "-production_threshold", str(prod),
      "-output_root_file_name", str(out_name)+".root",
    ], stdout=log_file, stderr=subprocess.STDOUT)
    proc.wait()
    print "...done"
    return out_name+".root"


class PhysicsModelTest(unittest.TestCase): # pylint: disable = R0904
    """
    Test that physics model datacards work correctly
    """

    def _test_reference_physics(self):
        """
        Check that reference_physics_processes works okay

        Want to check that the hadronic, energy straggling, decay, scattering
        are disabled in reference particle mode; and that energy loss is either
        disabled or mean only (deterministic).

        So fire hadron that decays i.e. pion; assert that there is some energy
        lost in material when required; assert that the energy loss is the same
        for two consecutive particles (i.e. no stochastic contribution); check
        energy loss by check that the time through RF cavity is 0 (for 0 phase).
        Better to write reference particle parameters, but unfortunately that
        isn't implemented yet.

        Long 100 m baseline is chosen st pion is likely to decay.
        """
        file_no_de = run_simulation("none", "none", False, -1., -1., 5.)
        bunch_no_de = Bunch.new_dict_from_read_builtin('maus_root_virtual_hit',
                                                         file_no_de, "pid")[211]
        # assert no energy lost in absorber
        self.assertAlmostEqual(bunch_no_de[0]['pz'], bunch_no_de[1]['pz'], 3)
        # assert no energy change in RF => reference particle also sees no
        # energy loss in absorber
        self.assertAlmostEqual(bunch_no_de[1]['pz'], bunch_no_de[2]['pz'], 3)
        # assert no scattering
        self.assertAlmostEqual(bunch_no_de[1]['px'], 0., 3)

        file_de = run_simulation("mean_energy_loss", "mean_energy_loss",
                                                          False, -1., -1., 5.)
        bunch_de = Bunch.new_dict_from_read_builtin('maus_root_virtual_hit',
                                                            file_de, "pid")[211]
        # assert energy lost in absorber
        self.assertNotAlmostEqual(bunch_de[0]['pz'], bunch_de[1]['pz'], 3)
        # assert no energy change in RF => reference particle also sees same
        # energy loss in absorber
        self.assertAlmostEqual(bunch_de[1]['pz'], bunch_de[2]['pz'], 3)
        # assert no scattering
        self.assertAlmostEqual(bunch_no_de[1]['px'], 0., 3)

    def _test_physics(self):
        """
        Check physics_processes datacard works okay

        Check that we lose same energy in mean_energy_loss mode for two
        particles with different seeds and that we have no transverse scatter.

        Check that we lose different energy amount in standard mode and that we
        do have a transverse scatter
        """
        # I want to check that we can set mean energy loss even when reference
        # particle has no energy loss
        file_de = run_simulation("none", "mean_energy_loss", False,
                                                                  -1., -1., 5.)
        bunch_de = Bunch.new_dict_from_read_builtin('maus_root_virtual_hit',
                                                           file_de, "pid")[211]
        # should lose energy in absorber
        de_hits_1 = bunch_de.get_hits('station', 1) # pions us of material
        de_hits_2 = bunch_de.get_hits('station', 2) # pions ds of material
        # check we lose energy through the absorber
        self.assertNotAlmostEqual(de_hits_1 [0]['pz'], de_hits_2[1]['pz'], 3)
        # check two particles lose same energy through absorber (no stochastics)
        self.assertAlmostEqual(de_hits_2 [0]['pz'], de_hits_2[1]['pz'], 3)
        # check no transverse
        self.assertAlmostEqual(de_hits_2[1]['px'], 0., 3)

        file_strag = run_simulation("none", "standard", False, -1., -1., 5.)
        bunch_strag = Bunch.new_dict_from_read_builtin('maus_root_virtual_hit',
                                                      file_strag, "pid")[211]
        strag_hits = bunch_strag.get_hits('station', 2) # pions ds of material
        # check two particles lose different energy through absorber 
        # (no stochastics)
        self.assertNotAlmostEqual(strag_hits[0]['pz'], strag_hits[1]['pz'], 3)
        # check for transverse distribution
        self.assertNotAlmostEqual(strag_hits[0]['px'], 0., 7)

    def test_decay_defaults(self):
        """
        Check that we get particle decay with default settings
        (regression #1470)
        """
        # defaults
        file_decay = run_simulation("none", "none", True,
                                     -1, -1, 5.)
        bunch = Bunch.new_list_from_read_builtin \
                                   ('maus_root_virtual_hit', file_decay)[-1]
        # pions decay to muons decay to electrons
        for hit in bunch:
            self.assertEqual(hit["pid"], -11)

    def _test_decay_lifetime(self):
        """
        Check that we can control particle lifetime
        """
        # decays on, short pion lifetime, long muon lifetime
        file_decay = run_simulation("none", "mean_energy_loss", True,
                                    1.e-9, 1.e+9, 5.)
        bunch = Bunch.new_dict_from_read_builtin \
                                   ('maus_root_virtual_hit', file_decay, "pid")
        # pions should decay immediately; muons should never decay
        self.assertTrue(-13 in bunch.keys())
        self.assertFalse(211 in bunch.keys())
    
        # decays on, long pion lifetime, short muon lifetime
        file_decay = run_simulation("none", "mean_energy_loss", True,
                                    1.e+9, 1.e-9, 5.)
        bunch = Bunch.new_dict_from_read_builtin \
                                   ('maus_root_virtual_hit', file_decay, "pid")
        # muons should decay immediately; pions should never decay
        self.assertTrue(211 in bunch.keys())
        self.assertFalse(-13 in bunch.keys())

    def _test_decay_disabled(self):
        """
        Check that we can disable particle decay
        """
        # decays off, long pion lifetime, long muon lifetime
        file_decay = run_simulation("none", "mean_energy_loss", False,
                                     1.e-9, 1.e-9, 5.)
        bunch = Bunch.new_dict_from_read_builtin \
                                   ('maus_root_virtual_hit', file_decay, "pid")
        # decays off
        self.assertTrue(211 in bunch.keys())
        self.assertTrue(-13 in bunch.keys())

if __name__ == "__main__":
    unittest.main()

