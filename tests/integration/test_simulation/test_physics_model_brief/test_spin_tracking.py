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

"""Check that the spin vector precesses in a magnetic field"""

import unittest
import subprocess
import os
import ROOT
import libMausCpp # pylint: disable = W0611
import math

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "simulate_mice.py")
PLOT_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                                    "plots", "physics_list")
TEST_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                                "test_simulation", "test_physics_model_brief")
SETUP_DONE = False
HIST_PERSIST = []
IS_MAIN = __name__ == "__main__"

class SpinTrackingTest(unittest.TestCase): # pylint: disable = R0904
    """Check that the spin vector precesses in a magnetic field"""

    def setUp(self): # pylint: disable = C0103
        """Set up the test"""
        self.file = "${MAUS_ROOT_DIR}/tmp/test_spin_tracking"
  
    def run_simulation(self):
        """Run the simulation"""
        print "Running spin tracking simulation"
        log_file = open(os.path.expandvars(self.file+".log"), "w")
        config = os.path.join(TEST_DIR, 'spin_tracking_config.py')
        proc = subprocess.Popen([SIM_PATH, '--configuration_file', config],
                                        stdout=log_file,
                                        stderr=subprocess.STDOUT)
        proc.wait()

    def parse_primary(self, primary): # pylint: disable = R0201
        """Extract data from primary"""
        pos = primary.GetPosition()
        spin = primary.GetSpin()
        return {
            "position":[pos.x(), pos.y(), pos.z()],
            "field":None,
            "spin":[spin.x(), spin.y(), spin.z()]
        }

    def parse_virtual(self, virtual_hit): # pylint: disable = R0201
        """Extract data from virtual hit"""
        pos = virtual_hit.GetPosition()
        spin = virtual_hit.GetSpin()
        field = virtual_hit.GetBField()
        return {
            "position":[pos.x(), pos.y(), pos.z()],
            "field":[field.x(), field.y(), field.z()],
            "spin":[spin.x(), spin.y(), spin.z()]
        }

    def hack_data(self):
        """Extract the data from the data structure"""
        print "  Hacking data"
        # pylint: disable = E1101
        root_file = ROOT.TFile(self.file+".root", "READ")
        maus_data = ROOT.MAUS.Data()
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", maus_data)
        tree.GetEntry(0)
        mc_event = maus_data.GetSpill().GetMCEvents()[0]
        data = [self.parse_primary(mc_event.GetPrimary())]
        for i in range(mc_event.GetVirtualHits().size()):
            vhit = mc_event.GetVirtualHits()[i]
            data.append(self.parse_virtual(vhit))
        return data

    def check_data(self, data):
        """Check that the data is okay"""
        print "  Checking data"
        self.assertAlmostEqual(data[0]["spin"][0], 0., 6)
        self.assertAlmostEqual(data[0]["spin"][1], 0., 6)
        self.assertAlmostEqual(data[0]["spin"][2], 1., 6)
        for hit in data[1:]:
            self.assertAlmostEqual(hit["field"][1], 0.0001)
        dphi2 = None
        for i in range(len(data[3:])):
            spin0 = data[i+1]["spin"]
            spin2 = data[i+3]["spin"]
            phi0 = math.atan2(spin0[0], spin0[2])
            phi2 = math.atan2(spin2[0], spin2[2])
            if dphi2 == None:
                dphi2 = phi2-phi0
            self.assertAlmostEqual(phi2-phi0, dphi2, 4)

    def test_spin_tracking(self):
        """Run simulation, extract data and check spin precesses in the field"""
        self.run_simulation()
        data = self.hack_data()
        print "  Found", len(data)-1, "virtual hits and 1 primary"
        self.check_data(data)

if __name__ == "__main__":
    unittest.main()
