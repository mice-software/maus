#!/usr/bin/env python

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

""" Run the examples and check they return 0 """

#pylint: disable = E1101
#pylint: disable = C0103

import os
import subprocess as sub
import unittest
import ROOT
import libMausCpp #pylint: disable = W0611

mrd = os.environ["MAUS_ROOT_DIR"]
wrk_dir = os.path.join(mrd, "tests/integration/test_scifi/test_scifi_recon/")
config_file_name = os.path.join(wrk_dir, "datacard_mc_helical")
root_file_name = os.path.join(wrk_dir, "maus_output.root")
simulation = os.path.join(wrk_dir, "simulate_scifi.py")

def run_simulation():
    """ Run the simulation """

    print "Using " + simulation

    call_options = [simulation,
                    "-configuration_file", config_file_name,
                    "-output_root_file_name", root_file_name,
                    "-verbose_level", "0"]

    log_file_name = os.path.join(mrd, "tmp/test_scifi_recon.log")
    log_file = open(log_file_name, 'w')
    print 'Running simulate_scifi, logging in', log_file_name
    proc = sub.Popen(call_options, stdout=log_file, stderr=sub.STDOUT)
    proc.wait()
    return proc, log_file_name

class TestSciFiRecon(unittest.TestCase): # pylint: disable=R0904
    """ Run the examples and check they return 0 """

    def test_recon(self):
        """ Run the simulation and check the output """

        # Run the simulation and check it completes with return code 0
        proc, log_file_name = run_simulation()
        self.assertEqual(proc.returncode, 0, msg="Check log "+log_file_name)

        # Check the ROOT output is as expected
        root_file = ROOT.TFile(root_file_name, "READ")
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = ROOT.TTree()
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)
        tree.GetEntry(2)  # Use the 3rd spill
        spill = data.GetSpill()
        self.assertEqual(spill.GetDaqEventType(), "physics_event")
        self.assertEqual(spill.GetReconEvents().size(), 2)
        evt = spill.GetReconEvents()[0].GetSciFiEvent()
        self.assertEqual(evt.spacepoints().size(), 10)
        self.assertEqual(evt.helicalprtracks().size(), 2)

if __name__ == "__main__":
    unittest.main()