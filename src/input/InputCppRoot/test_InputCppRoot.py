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


# pylint: disable=C0103

"""
Test for OutputCppRoot
"""

import unittest
import json
import ROOT
import os

import InputCppRoot

class TestInputCppRoot(unittest.TestCase): # pylint: disable=R0904
    """
    Test we can write out ROOT tree.

    Check we can birth and death correctly; check we can write simple ROOT trees
    especially in context of - how are errors handled - 
    """
    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202
        """
        Make a sample TFile
        """
        inputter = InputCppRoot.InputCppRoot()
        self.fname = os.path.join \
                  (os.environ["MAUS_ROOT_DIR"], "tmp", "test_inputCppRoot.root")
        root_file = ROOT.TFile(self.fname, "RECREATE") # pylint: disable = E1101
        spill = ROOT.MAUS.Spill() # pylint: disable = E1101
        tree = ROOT.TTree("Spill", "TTree") # pylint: disable = E1101
        tree.Branch("spill", spill, inputter.my_sizeof(), 1)
        tree.Fill()
        spill.SetScalars(ROOT.MAUS.Scalars()) # pylint: disable = E1101
        spill.SetEMRSpillData(ROOT.MAUS.EMRSpillData()) # pylint: disable = E1101, C0301
        spill.SetDAQData(ROOT.MAUS.DAQData()) # pylint: disable = E1101
        spill.SetMCEvents(ROOT.MAUS.MCEventArray()) # pylint: disable = E1101
        spill.SetReconEvents(ROOT.MAUS.ReconEventArray()) # pylint: disable = E1101, C0301
        spill.SetSpillNumber(1)
        tree.Fill()
        tree.Fill()
        tree.Write()
        root_file.Close()

    def _test_birth_death(self):
        """
        Check that we can birth and death properly
        """
        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps({"input_root_file_name":self.fname}))
        inputter.death()
        inputter.death()
        inputter.birth(json.dumps({"input_root_file_name":self.fname}))
        inputter.death()

        inputter_2 = InputCppRoot.InputCppRoot(self.fname)
        inputter_2.death()

        inputter_3 = InputCppRoot.InputCppRoot()
        inputter_3.death()

    def test_read_normal_event(self):
        """
        Try saving a few standard events
        """
        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps({"input_root_file_name":self.fname}))
        # bad event (no branches set)
        self.assertEqual(inputter.getNextEvent(), "")
        # normal event
        json_event = json.loads(inputter.getNextEvent())
        self.assertEqual \
             (json_event["scalars"], {}, msg=json.dumps(json_event, indent=2))
        # normal event
        json_event = json.loads(inputter.getNextEvent())
        self.assertEqual \
             (json_event["scalars"], {}, msg=json.dumps(json_event, indent=2))
        # out of events
        self.assertEqual(inputter.getNextEvent(), "")

if __name__ == "__main__":
    unittest.main()

