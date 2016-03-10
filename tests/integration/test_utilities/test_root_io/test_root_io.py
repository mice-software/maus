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

"""
Check that we can output a json document from offline analysis or monte carlo,
convert to root and then convert back to json
"""

import os
import subprocess
import unittest
import json

MRD = os.getenv("MAUS_ROOT_DIR")
ANALYSIS = os.path.join(MRD, "bin", "analyze_data_offline.py")
SIMULATION = os.path.join(MRD, "bin", "simulate_mice.py")
ROOT_TO_JSON = os.path.join(MRD, "bin", "utilities", "root_to_json.py")
JSON_TO_ROOT = os.path.join(MRD, "bin", "utilities", "json_to_root.py")

# set a different test file for stepIV
RUN_NUMBER = "5466"
if os.environ['MAUS_UNPACKER_VERSION'] == "StepIV":
    RUN_NUMBER = "6008"

def run_analyze_offline(root_file_name):
    """
    Run the offline analysis with default dataset
    """
    try:
        os.remove(root_file_name)
    except OSError:
        pass
    subproc = subprocess.Popen([ANALYSIS,
                                "--output_root_file_name", root_file_name,
                                "--daq_data_file", RUN_NUMBER,
                                "--TOF_findTriggerPixelCut", "2.0"])
    subproc.wait()

def run_mc_simulation(root_file_name):
    """
    Run the offline analysis with default dataset
    """
    try:
        os.remove(root_file_name)
    except OSError:
        pass
    config_file_name = os.path.join(MRD,
           "tests/integration/test_utilities/test_root_io/root_io_mc_config.py")
    subproc = subprocess.Popen([SIMULATION,
                                "--configuration_file", config_file_name,
                                "--output_root_file_name", root_file_name,
                                ])
    subproc.wait()

def run_json_to_root(json_file_name, root_file_name):
    """
    Run the converter to convert from json to root
    """
    try:
        os.remove(root_file_name)
    except OSError:
        pass
    subproc = subprocess.Popen([JSON_TO_ROOT,
                                "--input_json_file_name", json_file_name,
                                "--output_root_file_name", root_file_name,
                                "--verbose_level", "1",
                                "--header_and_footer_mode", "dont_append",])

    subproc.wait()

def run_root_to_json(root_file_name, json_file_name):
    """
    Run the converter to convert from json to root
    """
    try:
        os.remove(json_file_name)
    except OSError:
        pass
    subproc = subprocess.Popen([ROOT_TO_JSON,
                                "--input_root_file_name", root_file_name,
                                "--output_json_file_name", json_file_name,
                                "--verbose_level", "1",
                                "--header_and_footer_mode", "dont_append",])
    subproc.wait()

class RootIOTest(unittest.TestCase): #pylint: disable=R0904
    """
    Check that the conversion from ROOT to JSON works okay
    """
    def test_analyze_offline(self): #pylint: disable=R0201
        """
        Check that we can run the offline analysis, convert to root, convert
        back to json
        """
        ana_json_in = os.path.join \
                           (MRD, "tmp", "test_root_io_offline_analysis_IN.json")
        ana_root = os.path.join \
                              (MRD, "tmp", "test_root_io_offline_analysis.root")
        ana_json_out = os.path.join \
                          (MRD, "tmp", "test_root_io_offline_analysis_OUT.json")
        run_analyze_offline(ana_root)
        run_root_to_json(ana_root, ana_json_in)
        run_json_to_root(ana_json_in, ana_root)
        run_root_to_json(ana_root, ana_json_out)
        self.__check_equality(ana_json_in, ana_json_out)


    def test_simulate_mice(self): #pylint: disable=R0201
        """
        Check that we can run simulate_mice, convert to root, convert back to
        json
        """
        sim_json_in = os.path.join \
                              (MRD, "tmp", "test_root_io_simulate_mice_IN.json")
        sim_root = os.path.join \
                              (MRD, "tmp", "test_root_io_simulate_mice.root")
        sim_json_out = os.path.join \
                             (MRD, "tmp", "test_root_io_simulate_mice_OUT.json")
        run_mc_simulation(sim_root)
        run_root_to_json(sim_root, sim_json_in)
        run_json_to_root(sim_json_in, sim_root)
        run_root_to_json(sim_root, sim_json_out)
        self.__check_equality(sim_json_in, sim_json_out)

    def __check_equality(self, json_in_fname, json_out_fname):
        """
        Check that two files containing json documents on each line are the same 
        """
        json_fin = open(json_in_fname).readlines()
        json_fout = open(json_out_fname).readlines()
        self.assertEqual(len(json_fin), len(json_fout))
        for i in range(len(json_fin)):
            jin = json.loads(json_fin[i])
            jout = json.loads(json_fout[i])
            self.__almost_equal(jin, jout, verbose=False)


    def __almost_equal(self, # pylint: disable=R0913, R0912
                       item_in, item_out, verbose=True, branch=None, tol=1e-9):
        """
        Check that two json data structures are the same.

        If there are extra branches in item_out that's okay but we require that
        they have default constructors (empty arrays, dicts and strings,
        numerics are 0, bools are either true or false).
        """
        if branch == None:
            branch = []
        if verbose == True:
            print branch
        my_msg =  "item_in:\n  "+json.dumps(item_in, indent=2)
        my_msg += "\nitem_out:\n  "+json.dumps(item_out, indent=2)
        my_msg += "\nin branch:\n  "+str(branch)
        if type(item_in) != type(None):
            self.assertEqual(type(item_in), type(item_out), msg=my_msg)
        if type(item_in) == type({}):
            keys_in = item_in.keys()
            keys_out = item_out.keys()
            for key in keys_in:
                self.assertTrue(key in keys_out, msg=my_msg+\
                                                      "\nMissing key "+str(key))
            if verbose == True:
                print keys_in
                print keys_out
            for key in keys_in:
                self.__almost_equal(item_in[key], item_out[key],
                                  verbose=verbose, branch=branch+[key], tol=tol)
            for key in keys_out:
                if key not in keys_in:
                    self.__is_default_value(item_out[key])
        elif type(item_in) == type([]):
            self.assertEqual(len(item_in), len(item_out), msg=my_msg)
            for i in range(len(item_in)):
                self.__almost_equal(item_in[i], item_out[i],
                                 verbose=verbose, branch=branch+['[]'], tol=tol)
        elif type(item_in) == type(1.):
            my_tol = tol+abs(item_in)*tol 
            self.assertTrue(abs(item_in-item_out) < my_tol,
                msg=my_msg+"\nFloats were different  in: "+str(item_in)+\
                "  out: "+str(item_out)+"  diff: "+str(item_in-item_out)+\
                "  tol: "+str(my_tol))
        elif type(item_in) == type(None):
            self.__is_default_value(item_out)
        else:
            self.assertEqual(item_in, item_out, msg=my_msg)

    def __is_default_value(self, value):
        """
        Check that a json value has some default value
        """
        if type(value) == type("") or type(value) == type([]):
            self.assertEqual(len(value), 0)
        elif type(value) == type({}):
            self.assertEqual(len(value.keys()), 0)
        elif type(value) == type(0.):
            self.assertAlmostEqual(value, 0., 1e-12)
        elif type(value) == type(0):
            self.assertEqual(value, 0)
        #None and bool types are always okay

if __name__ == "__main__":
    unittest.main()


