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

import os
import unittest
import json

import ROOT

import ErrorHandler
import Configuration
import OutputCppRoot
import maus_cpp.globals 

class TestOutputCppRoot(unittest.TestCase): # pylint: disable=R0904, R0902
    """
    Test we can write out ROOT tree.

    Check we can birth and death correctly; check we can write simple ROOT trees
    especially in context of - how are errors handled - 
    """

    def setUp(self): # pylint: disable=C0103, C0202
        """
        Define cards and initialise Output
        """
        self.output = OutputCppRoot.OutputCppRoot()
        self.outfile = os.path.join \
                 (os.environ["MAUS_ROOT_DIR"], "tmp", "test_outputCppRoot.root")
        self.on_error_standard = ErrorHandler.DefaultHandler().on_error
        #ref = {"$ref":"#test_branch/double_by_value"}
        self.test_data = {
            "scalars":{},
            "emr_spill_data":{},
            "spill_number":1,
            "run_number":1,
            "daq_event_type":"physics_event",
            "recon_events":[],
            "mc_events":[],
            "maus_event_type":"Spill",
        }
        self.test_job_header = {
                "start_of_job":{"date_time":"1976-04-04T00:00:00.000000"},
                "bzr_configuration":"",
                "bzr_revision":"",
                "bzr_status":"",
                "maus_version":"",
                "json_configuration":"output cpp root test",
                "maus_event_type":"JobHeader",
            }
        self.test_run_header = {
                "run_number":1,
                "maus_event_type":"RunHeader"
        }
        self.test_run_footer = {
                "run_number":-1,
                "maus_event_type":"RunFooter"
        }
        self.test_job_footer = {
                "end_of_job":{"date_time":"1977-04-04T00:00:00.000000"},
                "maus_event_type":"JobFooter",
            }
        self.cards = Configuration.Configuration().getConfigJSON()
        self.cards = json.loads(self.cards)
        self.cards["output_root_file_name"] = self.outfile
        self.cards["verbose_level"] = 2
        self.cards = json.dumps(self.cards)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(self.cards)
        self.output.birth(self.cards)

    def tearDown(self): # pylint: disable=C0103, C0202
        """
        Destructor
        """
        ErrorHandler.DefaultHandler().on_error = self.on_error_standard
        self.output.death()
        maus_cpp.globals.death(self.cards)

    def _save_event(self, event):
        """
        Experimenting with nested functions - looks like this is making segv
        when output.save is nested
        """
        return self.output.save(event)

    def sub_function(self, event):
        """
        Experimenting with nested functions - looks like this is making segv
        """
        return self._save_event(event)

    def sub_function_2(self, event):
        """
        Experimenting with nested functions - looks like this is making segv
        """
        return self._save_event(event)

    def test_birth_death(self):
        """
        Check that we can birth and death properly
        """
        an_output = OutputCppRoot.OutputCppRoot()
        #self.assertRaises(ErrorHandler.CppError, an_output.birth, '{}')
        #self.assertRaises(ErrorHandler.CppError, an_output.birth,
        #                  '{"output_root_file_name":""}')
        # for now this does NOT return an exception... needs fixing
        an_output.birth(json.dumps({}))
        an_output.birth(json.dumps({'output_root_file_name':''}))
        an_output.birth(self.cards)
        an_output.death()

    def test_save_normal_event(self):
        """
        Try saving a few standard events
        """
        self.assertTrue(self._save_event(
            json.dumps(self.test_data)
        ))
        self.assertTrue(self._save_event(
            json.dumps(self.test_data)
        ))
        self.assertTrue(self._save_event(
            json.dumps(self.test_data)
        ))
        self.output.death() # close the ROOT file
        root_file = ROOT.TFile(self.outfile, "READ") # pylint: disable = E1101
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)
        self.assertEqual(tree.GetEntries(), 3)
        for i in range(tree.GetEntries()):
            tree.GetEntry(i)
            self.assertNotEqual(data.GetSpill().GetSpillNumber(), 0)

    def test_save_bad_event(self):
        """
        Check that if passed a bad event, code fails gracefully
        """
        self.assertFalse(self._save_event(json.dumps({"no_branch":{}})))
        self.assertFalse(self._save_event(''))

    def test_save_bad_job_header(self):
        """
        Check that if passed a bad header, code fails gracefully
        """
        self.assertFalse(self._save_event(json.dumps({"":{}})))
        self.assertFalse(self._save_event(''))

    def __check_job_header(self, json_conf_text, n_entries, entry):
        """
        Check that json_header entry has json_conf_text 
        """
        root_file = ROOT.TFile(self.outfile, "READ") # pylint: disable = E1101
        job_header = ROOT.MAUS.JobHeaderData() # pylint: disable = E1101
        tree = root_file.Get("JobHeader")
        tree.SetBranchAddress("job_header", job_header)
        self.assertEqual(tree.GetEntries(), n_entries)
        tree.GetEntry(entry)
        self.assertEqual(job_header.GetJobHeader().GetJsonConfiguration(),
                         json_conf_text)
        root_file.Close()

    def __check_job_footer(self, datetime_string, n_entries, entry):
        """
        Check that json_footer entry has datetime_string
        """
        root_file = ROOT.TFile(self.outfile, "READ") # pylint: disable = E1101
        job_footer = ROOT.MAUS.JobFooterData() # pylint: disable = E1101
        tree = root_file.Get("JobFooter")
        tree.SetBranchAddress("job_footer", job_footer)
        self.assertEqual(tree.GetEntries(), n_entries)
        tree.GetEntry(entry)
        self.assertEqual(job_footer.GetJobFooter().GetEndOfJob().GetDateTime(),
                         datetime_string)
        root_file.Close()

    def __check_run_header(self, run_number, n_entries, entry):
        """
        Check that json_header entry has json_conf_text 
        """
        root_file = ROOT.TFile(self.outfile, "READ") # pylint: disable = E1101
        run_header = ROOT.MAUS.RunHeaderData() # pylint: disable = E1101
        tree = root_file.Get("RunHeader")
        tree.SetBranchAddress("run_header", run_header)
        self.assertEqual(tree.GetEntries(), n_entries)
        tree.GetEntry(entry)
        self.assertEqual(run_header.GetRunHeader().GetRunNumber(),
                         run_number)
        root_file.Close()

    def test_save_normal_job_header(self):
        """
        Try saving a standard header; check we can only save job header once
        """
        self.assertTrue(self._save_event(
            json.dumps(self.test_job_header)
        ))
        self.test_job_header["json_configuration"] = "output cpp root test 2"
        self.assertTrue(self._save_event(
            json.dumps(self.test_job_header)
        ))
        self.output.death()
        self.__check_job_header("output cpp root test", 2, 0)
        self.__check_job_header("output cpp root test 2", 2, 1)

    def test_save_run_header(self):
        """
        test_OutputCppRoot: Try saving a run header
        """
        self.assertTrue(self.sub_function(
            json.dumps(self.test_run_header)
        ))
        self.test_run_header["run_number"] = 2
        for i in range(1000): # pylint:disable=W0612
            self.assertTrue(self.sub_function(
                json.dumps(self.test_run_header)
            ))
        self.test_run_header["run_number"] = 3
        self.sub_function_2(json.dumps(self.test_run_header))
        #self._save_event(json.dumps(self.test_run_header)) SEGMENTATION FAULT
        #self.output.save(json.dumps(self.test_run_header)) SEGMENTATION FAULT
        self.output.death()
        self.__check_run_header(1, 1002, 0)
        self.__check_run_header(2, 1002, 1000)

    def test_mixed_types(self):
        """
        test_OutputCppRoot.test_mixed_types check we can load alternating types
        """
        self.assertTrue(self._save_event(
            json.dumps(self.test_job_header)
        ))
        self.assertTrue(self._save_event(
            json.dumps(self.test_data)
        ))
        self.test_job_header["json_configuration"] = "output cpp root test 2"
        self.assertTrue(self._save_event(
            json.dumps(self.test_job_header)
        ))
        self.assertTrue(self._save_event(
            json.dumps(self.test_job_footer)
        ))
        self.assertTrue(self._save_event(
            json.dumps(self.test_data)
        ))
        self.assertTrue(self._save_event(
            json.dumps(self.test_job_footer)
        ))
        self.output.death()
        self.__check_job_header("output cpp root test", 2, 0)
        self.__check_job_header("output cpp root test 2", 2, 1)
        self.__check_job_footer("1977-04-04T00:00:00.000000", 2, 1)

if __name__ == "__main__":
    unittest.main()


