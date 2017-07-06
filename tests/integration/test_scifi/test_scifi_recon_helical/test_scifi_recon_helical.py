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
wrk_dir = os.path.join(mrd,
          "tests/integration/test_scifi/test_scifi_recon_helical/")
lsq_datacard_name = os.path.join(wrk_dir, "datacard_mc_helical")
lsq_root_file_name = os.path.join(wrk_dir, "output_helical_lsq.root")
lsq_log_file_name = "tmp/test_scifi_recon_helical_lsq.log"
minuit_datacard_name = os.path.join(wrk_dir, "datacard_mc_helical_minuit")
minuit_root_file_name = os.path.join(wrk_dir, "output_helical_minuit.root")
minuit_log_file_name = "tmp/test_scifi_recon_helical_minuit.log"
simulation = os.path.join(wrk_dir, "simulate_scifi.py")

def run_simulation(datacard_name, output_file_name, log_file_name ):
    """ Run the simulation """

    print "Using " + simulation

    call_options = [simulation,
                    "-configuration_file", datacard_name,
                    "-output_root_file_name", output_file_name,
                    "-verbose_level", "0"]

    log_file = open(log_file_name, 'w')
    print 'Running simulate_scifi, logging in', log_file_name
    proc = sub.Popen(call_options, stdout=log_file, stderr=sub.STDOUT)
    proc.wait()
    return proc, log_file_name



class TestSciFiReconHelical(unittest.TestCase): # pylint: disable=R0904
    """ Run the scifi helical recon and check output """

    def test_helical_recon_lsq(self):
        """ TestSciFiRecon: Run helical simulation & check the output (LSQ) """

        # Run the simulation and check it completes with return code 0
        proc, log_file_name = run_simulation(lsq_datacard_name, \
          lsq_root_file_name, lsq_log_file_name)
        self.assertEqual(proc.returncode, 0, msg="Check log "+log_file_name)

        # Check the ROOT output is as expected
        root_file = ROOT.TFile(lsq_root_file_name, "READ")
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = ROOT.TTree()
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)

        # Check chi^2 average is reasonably low for helical pat rec tracks
        tree.Draw(
          "_spill._recon._scifi_event._scifihelicalprtracks._line_sz_chisq>>h5")
        h5 = ROOT.gDirectory.Get('h5')
        self.assertLess(h5.GetMean(), 12)
        self.assertLess(h5.GetRMS(), 50)
        self.assertGreater(h5.GetEntries(), 190)

        tree.Draw(
          "_spill._recon._scifi_event._scifihelicalprtracks._circle_chisq>>h6")
        h6 = ROOT.gDirectory.Get('h6')
        self.assertLess(h6.GetMean(), 1.0)
        self.assertLess(h6.GetRMS(), 2)
        self.assertGreater(h6.GetEntries(), 190)

    def test_helical_recon_minuit(self):
        """ TestSciFiRecon: Run helical simulation & check \
            the output (MINUIT) """

        # Run the simulation and check it completes with return code 0
        proc, log_file_name = run_simulation(minuit_datacard_name, \
          minuit_root_file_name, minuit_log_file_name)
        self.assertEqual(proc.returncode, 0, msg="Check log "+log_file_name)

        # Check the ROOT output is as expected
        root_file = ROOT.TFile(minuit_root_file_name, "READ")
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = ROOT.TTree()
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)

        # Check chi^2 average is reasonably low for helical pat rec tracks
        tree.Draw(
          "_spill._recon._scifi_event._scifihelicalprtracks._line_sz_chisq>>h7")
        h7 = ROOT.gDirectory.Get('h7')
        self.assertLess(h7.GetMean(), 12)
        self.assertLess(h7.GetRMS(), 50)
        self.assertGreater(h7.GetEntries(), 190)

        tree.Draw(
          "_spill._recon._scifi_event._scifihelicalprtracks._circle_chisq>>h8")
        h8 = ROOT.gDirectory.Get('h8')
        self.assertLess(h8.GetMean(), 1.0)
        self.assertLess(h8.GetRMS(), 2)
        self.assertGreater(h8.GetEntries(), 190)

if __name__ == "__main__":
    unittest.main()
