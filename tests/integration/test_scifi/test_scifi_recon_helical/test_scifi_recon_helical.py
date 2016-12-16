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
helical_datacard_name = os.path.join(wrk_dir, "datacard_mc_helical")
helical_root_file_name = os.path.join(wrk_dir, "maus_output_helical.root")
simulation = os.path.join(wrk_dir, "simulate_scifi.py")

def run_helical_simulation():
    """ Run the helical tracks simulation """

    print "Using " + simulation

    call_options = [simulation,
                    "-configuration_file", helical_datacard_name,
                    "-output_root_file_name", helical_root_file_name,
                    "-verbose_level", "0"]

    log_file_name = os.path.join(mrd, "tmp/test_scifi_recon.log")
    log_file = open(log_file_name, 'w')
    print 'Running simulate_scifi, logging in', log_file_name
    proc = sub.Popen(call_options, stdout=log_file, stderr=sub.STDOUT)
    proc.wait()
    return proc, log_file_name

class TestSciFiReconHelical(unittest.TestCase): # pylint: disable=R0904
    """ Run the scifi helical recon and check output """

    def test_helical_recon(self):
        """ TestSciFiRecon: Run the helical simulation and check the output """

        # Run the simulation and check it completes with return code 0
        proc, log_file_name = run_helical_simulation()
        self.assertEqual(proc.returncode, 0, msg="Check log "+log_file_name)

        # Check the ROOT output is as expected
        root_file = ROOT.TFile(helical_root_file_name, "READ")
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = ROOT.TTree()
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)

        # Check chi^2 average is reasonably low for helical pat rec tracks
        tree.Draw(
          "_spill._recon._scifi_event._scifihelicalprtracks._line_sz_chisq>>h5")
        h5 = ROOT.gDirectory.Get('h5')
        self.assertLess(h5.GetMean(), 5)
        self.assertLess(h5.GetRMS(), 8)
        self.assertGreater(h5.GetEntries(), 190)

        tree.Draw(
          "_spill._recon._scifi_event._scifihelicalprtracks._circle_chisq>>h6")
        h6 = ROOT.gDirectory.Get('h6')
        self.assertLess(h6.GetMean(), 1.0)
        self.assertLess(h6.GetRMS(), 2)
        self.assertGreater(h6.GetEntries(), 190)

        print "WARNING: Track Fit still under development."+\
          "Disabling remaining tests for now"

#        # Check the mean and standard deviation of some final track data
#        tree.Draw("_spill._recon._scifi_event._scifitracks._P_value>>h1")
#        h1 = ROOT.gDirectory.Get('h1')
#        self.assertAlmostEqual(h1.GetMean(), 0.50, delta=0.1)
#        self.assertLess(h1.GetRMS(), 0.35)
#
#        tree.Draw("_spill._recon._scifi_event._scifitracks._chi2>>h2")
#        h2 = ROOT.gDirectory.Get('h2')
#        self.assertLess(h2.GetMean(), 15)
#        self.assertLess(h2.GetRMS(), 10)
#
#        # Most particles should be identified correctly as positives
#        tree.Draw("_spill._recon._scifi_event._scifitracks._charge>>h4")
#        h4 = ROOT.gDirectory.Get('h4')
#        self.assertGreater(h4.GetMean(), 0.95)

if __name__ == "__main__":
    unittest.main()
