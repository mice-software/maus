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
        tree.GetEntry(1)
        spill = data.GetSpill()
        print "Using spill " + str(spill.GetSpillNumber())

        self.assertEqual(spill.GetDaqEventType(), "physics_event")
        self.assertEqual(spill.GetReconEvents().size(), 2)

        # Use the first recon event only
        evt = spill.GetReconEvents()[0].GetSciFiEvent()

        # Check have found tracks with correct params & correct seed spoints
        self.assertEqual(evt.spacepoints().size(), 10)
        trks = evt.helicalprtracks()
        self.assertEqual(trks.size(), 2)

        sds0 = trks[0].get_spacepoints() # Tracker 1
        self.assertAlmostEqual(trks[0].get_R(), 38.68, 2)
        self.assertAlmostEqual(trks[0].get_dsdz(), -0.2304, 4)
        self.assertEqual(sds0.size(), 5)

        sds1 = trks[1].get_spacepoints() # Tracker 2
        self.assertAlmostEqual(trks[1].get_R(), 18.86, 2)
        self.assertAlmostEqual(trks[1].get_dsdz(), 0.1155, 4)
        self.assertEqual(sds1.size(), 5)

if __name__ == "__main__":
    unittest.main()
