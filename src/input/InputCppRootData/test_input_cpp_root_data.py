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
# pylint: disable = E1101

"""
Test for OutputCppRoot
"""

import unittest
import json
import ROOT
import os

import maus_cpp.converter
import _InputCppRootData as InputCppRootData

class TestInputCppRootData(unittest.TestCase): # pylint: disable=R0904
    """
    Test we can write out ROOT tree.

    Check we can birth and death correctly; check we can write simple ROOT trees
    especially in context of - how are errors handled - 
    """
    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202, R0915, R0914
        """
        Make a sample TFile
        """
        self.fname = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp",
                                  "test_input_cpp_root_data.root")
        root_file = ROOT.TFile(self.fname, "RECREATE")
        spill = ROOT.MAUS.Spill()
        data = ROOT.MAUS.Data()
        tree = ROOT.TTree("Spill", "TTree")
        tree.Branch("data", data, data.GetSizeOf(), 1)
        spill.SetScalars(ROOT.MAUS.Scalars())
        spill.SetDAQData(ROOT.MAUS.DAQData())
        spill.SetMCEvents(ROOT.MAUS.MCEventPArray())
        spill.SetReconEvents(ROOT.MAUS.ReconEventPArray())
        # test branch makes segmentation fault... from ROOT side
        # spill.SetTestBranch(ROOT.MAUS.TestBranch())
        spill.SetRunNumber(10)
        data.SetSpill(spill)
        for num in range( 1, 11 ) : # Add 10 spills
            spill.SetSpillNumber(num)
            tree.Fill()
        spill.SetRunNumber(11)
        for num in range( 1, 3 ) : # Add 2 spills
            spill.SetSpillNumber(num)
            tree.Fill()
        spill.SetRunNumber(12)
        for num in range( 1, 3 ) : # Add 2 spills
            spill.SetSpillNumber(num)
            tree.Fill()
        tree.Write()

        job_header_data = ROOT.MAUS.JobHeaderData()
        job_header = ROOT.MAUS.JobHeader()
        tree2 = ROOT.TTree("JobHeader", "TTree")
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
        inputter = InputCppRootData.InputCppRootData()
        inputter.birth(json.dumps({"input_root_file_name":self.fname}))
        inputter.death()
        inputter.death()
        inputter.birth(json.dumps({"input_root_file_name":self.fname}))
        inputter.death()

        inputter_2 = InputCppRootData.InputCppRootData(self.fname)
        inputter_2.death()

        inputter_3 = InputCppRootData.InputCppRootData()
        inputter_3.death()

    def __test_event(self, inputter, checks):
        """
        Run the inputter, check that output contains key:value pair.
        """
        event = inputter.emitter().next()
        json_event = json.loads(maus_cpp.converter.string_repr(event))
        maus_cpp.converter.del_data_repr(event)
        for key, value in checks.iteritems():
            self.assertEqual(json_event[key], value,
              msg=str(key)+":"+str(value)+"\n"+json.dumps(json_event, indent=2))

    def test_read_events(self):
        """
        Try reading a realistic data structure
        """
        inputter = InputCppRootData.InputCppRootData()
        inputter.birth(json.dumps({"input_root_file_name":self.fname,
                                   "selected_spills":[]}))

        # should ignore headers and go straight for spills...
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":1})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":2})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":3})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":4})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":5})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":6})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":7})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":8})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":9})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":10})

        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":11,
                                     "spill_number":1})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":11,
                                     "spill_number":2})

        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":12,
                                     "spill_number":1})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":12,
                                     "spill_number":2})

        try:
            inputter.emitter().next()
            self.assertTrue(False, msg="Should have raised StopIteration")
        except StopIteration:
            pass

    def test_read_spills_only(self):
        """
        Try reading a data structure with missing tree
        """
        fname = os.path.join(os.environ["MAUS_ROOT_DIR"],
                             "tmp",
                             "test_input_cpp_root_data_2.root")
        root_file = ROOT.TFile(fname, "RECREATE")

        spill = ROOT.MAUS.Spill()
        data = ROOT.MAUS.Data()
        tree2 = ROOT.TTree("Spill", "TTree")
        tree2.Branch("data", data, data.GetSizeOf(), 1)
        spill.SetScalars(ROOT.MAUS.Scalars())
        spill.SetDAQData(ROOT.MAUS.DAQData())
        spill.SetMCEvents(ROOT.MAUS.MCEventPArray())
        spill.SetReconEvents(ROOT.MAUS.ReconEventPArray())
        data.SetSpill(spill)
        tree2.Fill()
        tree2.Fill()
        tree2.Write()

        root_file.Close()

        inputter = InputCppRootData.InputCppRootData()
        inputter.birth(json.dumps({"input_root_file_name":fname}))

        self.__test_event(inputter, {"maus_event_type":"Spill"})
        self.__test_event(inputter, {"maus_event_type":"Spill"})
        try:
            inputter.emitter().next()
            self.assertTrue(False, msg="Should have raised StopIteration")
        except StopIteration:
            pass


    def _test_read_selected_spills( self ):
        """
          Tests the functionality to select individual spills from a data file.
          Selection is performed by using an array of spill numbers passed via
          the configuration file.
        """
        inputter = InputCppRootData.InputCppRootData()
        inputter.birth(json.dumps( { "input_root_file_name":self.fname, \
                                                "selected_spills":[2, 5, 8] } ))
        self.assertTrue(inputter.useSelectedSpills())
        # run 10
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":2})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":5})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":8})
        self.__test_event(inputter, {"maus_event_type":"RunFooter",
                                     "run_number":10})
        self.__test_event(inputter, {"maus_event_type":"RunFooter",
                                     "run_number":10})
       

if __name__ == "__main__":
    unittest.main()

