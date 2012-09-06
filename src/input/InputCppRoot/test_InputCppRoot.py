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
        self.fname = os.path.join \
                  (os.environ["MAUS_ROOT_DIR"], "tmp", "test_inputCppRoot.root")
        root_file = ROOT.TFile(self.fname, "RECREATE") # pylint: disable = E1101
        spill = ROOT.MAUS.Spill() # pylint: disable = E1101
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = ROOT.TTree("Spill", "TTree") # pylint: disable = E1101
        tree.Branch("data", data, data.GetSizeOf(), 1)
        tree.Fill()
        spill.SetScalars(ROOT.MAUS.Scalars()) # pylint: disable = E1101
        spill.SetEMRSpillData(ROOT.MAUS.EMRSpillData()) # pylint: disable = E1101, C0301
        spill.SetDAQData(ROOT.MAUS.DAQData()) # pylint: disable = E1101
        spill.SetMCEvents(ROOT.MAUS.MCEventArray()) # pylint: disable = E1101
        spill.SetReconEvents(ROOT.MAUS.ReconEventArray()) # pylint: disable = E1101, C0301
        spill.SetSpillNumber(1)
        data.SetSpill(spill)
        tree.Fill()
        tree.Fill()
        tree.Write()
        job_header_data = ROOT.MAUS.JobHeaderData() # pylint: disable = E1101
        job_header = ROOT.MAUS.JobHeader() # pylint: disable = E1101
        tree2 = ROOT.TTree("JobHeader", "TTree") # pylint: disable = E1101
        tree2.Branch("job_header", job_header_data,
                    job_header_data.GetSizeOf(), 1)
        job_header.SetJsonConfiguration("mushrooms")
        job_header_data.SetJobHeader(job_header)
        tree2.Fill()
        job_header.SetJsonConfiguration("omelette")
        tree2.Fill()
        tree2.Write()
        root_file.Close()

    def test_birth_death(self):
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
        Try reading a spill event
        """
        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps({"input_root_file_name":self.fname}))
        # bad event (no branches set)
        self.assertEqual(inputter.emitter_cpp(), "")
        # normal event
        json_event = json.loads(inputter.emitter_cpp())
        self.assertEqual \
           (json_event["spill_number"], 1, msg=json.dumps(json_event, indent=2))
        # normal event
        json_event = json.loads(inputter.emitter_cpp())
        self.assertEqual \
           (json_event["spill_number"], 1, msg=json.dumps(json_event, indent=2))
        # out of events
        self.assertEqual(inputter.emitter_cpp(), "")

    def test_load_job_header_bad_no_branch(self):
        """
        Try reading a bad job header
        """
        # set up file with no job_header
        my_fname = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp",
                                         "test_inputCppRoot_no_job_header.root")
        root_file = ROOT.TFile(my_fname, "RECREATE") # pylint: disable = E1101
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = ROOT.TTree("Spill", "TTree") # pylint: disable = E1101
        tree.Branch("data", data, data.GetSizeOf(), 1)
        tree.Fill()
        tree.Write()
        root_file.Close()
        # try reading
        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps({"input_root_file_name":my_fname}))
        self.assertEqual(inputter.load_job_header(), "")
        inputter.death()
        # now an empty job header tree (no branch)
        root_file = ROOT.TFile(my_fname, "UPDATE") # pylint: disable = E1101
        tree2 = ROOT.TTree("JobHeader", "TTree") # pylint: disable = E1101
        tree2.Write()
        root_file.Close()
        # try reading
        inputter.birth(json.dumps({"input_root_file_name":my_fname}))
        self.assertEqual(inputter.load_job_header(), "")
        inputter.death()
        # Nb: I tried a malformed job_header branch i.e. with Spill instead of
        # JobHeader. It crashes with a segv, I think that's okay though...
        # slight worry when it comes to backward compatibility (i.e. what
        # happens if JobHeader gets new data added to it?).

if __name__ == "__main__":
    unittest.main()

