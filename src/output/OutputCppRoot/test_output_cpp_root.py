#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
import MAUS
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
        self.output = MAUS.OutputCppRoot()
        self.outdir = os.environ["MAUS_ROOT_DIR"]+"/tmp/test_output_cpp_root/"
        self.outfile = self.outdir+"test_outputCppRoot.root"
        try:
            os.mkdir(self.outdir)
        except OSError:
            pass
        try:
            os.mkdir(self.outdir+"/end_of_run/")
        except OSError:
            pass
        self.on_error_standard = ErrorHandler.DefaultHandler().on_error
        #ref = {"$ref":"#test_branch/double_by_value"}
        self.test_data = {
            "scalars":{},
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
        self.cards["output_root_file_mode"] = "one_big_file"
        self.cards["end_of_run_output_root_directory"] = \
                                                      self.outdir+"/end_of_run/"
        self.cards["verbose_level"] = 2
        self.cards = json.dumps(self.cards)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(self.cards)
        self.output.birth(self.cards)
        #ErrorHandler.DefaultHandler().on_error = 'raise'

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
        an_output = MAUS.OutputCppRoot()
        # for now this does NOT return an exception... looks like it is caught
        # by the API... set ErrorHandler.on_error to 'raise' and we raise a
        # std::exception killing the tests (oops)
        an_output.birth(json.dumps({}))
        an_output.birth(json.dumps({'output_root_file_name':'',
                                    'output_root_file_mode':'one_big_file',
                                    'end_of_run_output_root_directory':'tmp'}))
        an_output.birth(json.dumps({'output_root_file_name':'test.root',
                                    'output_root_file_mode':'',
                                    'end_of_run_output_root_directory':'tmp'}))
        an_output.birth(json.dumps({'output_root_file_name':'test.root',
                                    'output_root_file_mode':'one',
                                    'end_of_run_output_root_directory':'tmp'}))
        # this is okay (dir can just be '')
        an_output.birth(json.dumps({'output_root_file_name':'test.root',
                                    'output_root_file_mode':'one_big_file',
                                    'end_of_run_output_root_directory':''}))
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

    def __check_job_header(self, json_conf_text, n_entries, entry, fname=None):
        """
        Check that json_header entry has json_conf_text 
        """
        if fname == None:
            fname = self.outfile
        root_file = ROOT.TFile(fname, "READ") # pylint: disable = E1101
        job_header = ROOT.MAUS.JobHeaderData() # pylint: disable = E1101
        tree = root_file.Get("JobHeader")
        tree.SetBranchAddress("job_header", job_header)
        self.assertEqual(tree.GetEntries(), n_entries)
        tree.GetEntry(entry)
        self.assertEqual(job_header.GetJobHeader().GetJsonConfiguration(),
                         json_conf_text)
        root_file.Close()

    def __check_job_footer(self, datetime_string, n_entries, entry, fname=None):
        """
        Check that json_footer entry has datetime_string
        """
        if fname == None:
            fname = self.outfile
        root_file = ROOT.TFile(fname, "READ") # pylint: disable = E1101
        job_footer = ROOT.MAUS.JobFooterData() # pylint: disable = E1101
        tree = root_file.Get("JobFooter")
        tree.SetBranchAddress("job_footer", job_footer)
        self.assertEqual(tree.GetEntries(), n_entries)
        tree.GetEntry(entry)
        self.assertEqual(job_footer.GetJobFooter().GetEndOfJob().GetDateTime(),
                         datetime_string)
        root_file.Close()

    def __check_run_header(self, run_number, n_entries, entry, fname=None):
        """
        Check that json_header entry has json_conf_text 
        """
        if fname == None:
            fname = self.outfile
        root_file = ROOT.TFile(fname, "READ") # pylint: disable = E1101
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

    def test_one_file_per_run(self):
        """
        test_OutputCppRoot.test_one_file_per_run: one_file_per_run option
        """
        my_output = MAUS.OutputCppRoot()
        cards_py = json.loads(self.cards)
        cards_py["output_root_file_mode"] = "one_file_per_run"
        self.cards = json.dumps(cards_py)
        my_output.birth(self.cards)
        self.test_data["run_number"] = 10
        self.test_run_header["run_number"] = 10
        for i in range(3): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_data)
            ))
        for i in range(2): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_run_header)
            ))
        for i in range(3): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_data)
            ))
        self.test_run_header["run_number"] = 9
        for i in range(3): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_run_header)
            ))
        for i in range(3): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_job_header)
            ))
        my_output.death()
        self.__check_job_header("output cpp root test", 3, 0,
                                        self.outdir+"test_outputCppRoot_0.root")
        self.__check_run_header(10, 2, 0,
                                       self.outdir+"test_outputCppRoot_10.root")
        self.__check_run_header(9, 3, 0,
                                        self.outdir+"test_outputCppRoot_9.root")

    def test_end_of_run_file_per_run(self):
        """
        test_OutputCppRoot.test_one_file_per_run: end_of_run_file_per_run option
        """
        my_output = MAUS.OutputCppRoot()
        cards_py = json.loads(self.cards)
        cards_py["output_root_file_mode"] = "end_of_run_file_per_run"
        cards_py["output_root_file_name"] = "test_outputCppRoot.root"
        cards_py["end_of_run_output_root_directory"] = \
                                                      self.outdir+"/end_of_run/"
        self.cards = json.dumps(cards_py)
        my_output.birth(self.cards)
        self.test_data["run_number"] = 10
        self.test_run_header["run_number"] = 10
        for i in range(3): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_data)
            ))
        for i in range(2): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_run_header)
            ))
        for i in range(3): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_data)
            ))
        self.test_run_header["run_number"] = 9
        for i in range(3): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_run_header)
            ))
        self.assertTrue(my_output.save(
            json.dumps(self.test_job_footer)
        ))
        self.test_run_footer["run_number"] = 9
        self.assertTrue(my_output.save(
            json.dumps(self.test_run_footer)
        ))
        for i in range(3): # pylint: disable = W0612
            self.assertTrue(my_output.save(
                json.dumps(self.test_job_header)
            ))
        my_output.death()
        self.__check_job_header("output cpp root test", 3, 0,
                           self.outdir+"/end_of_run/0/test_outputCppRoot.root")
        self.__check_run_header(10, 2, 0,
                           self.outdir+"/end_of_run/10/test_outputCppRoot.root")
        self.__check_run_header(9, 3, 0,
                           self.outdir+"/end_of_run/9/test_outputCppRoot.root")
        self.__check_job_footer("1977-04-04T00:00:00.000000", 1, 0,
                           self.outdir+"/end_of_run/0/test_outputCppRoot.root")


if __name__ == "__main__":
    unittest.main()


