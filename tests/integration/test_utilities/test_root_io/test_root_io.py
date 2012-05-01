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
ROOT_TO_JSON = os.path.join(MRD, "bin", "user", "root_to_json.py")
JSON_TO_ROOT = os.path.join(MRD, "bin", "user", "json_to_root.py")

def run_analyze_offline(json_file_name):
    """
    Run the offline analysis with default dataset
    """
    try:
        os.remove(json_file_name)
    except OSError:
        pass
    subproc = subprocess.Popen([ANALYSIS,
                                "-output_json_file_name", json_file_name])
    subproc.wait()

def run_mc_simulation(json_file_name):
    """
    Run the offline analysis with default dataset
    """
    try:
        os.remove(json_file_name)
    except OSError:
        pass
    config_file_name = os.path.join(MRD,
           "tests/integration/test_utilities/test_root_io/root_io_mc_config.py")
    subproc = subprocess.Popen([SIMULATION,
                                "-configuration_file", config_file_name,
                                "-output_json_file_name", json_file_name,
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
                                "-input_json_file_name", json_file_name,
                                "-output_root_file_name", root_file_name,
                                "-verbose_level", "1"])

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
                                "-input_root_file_name", root_file_name,
                                "-output_json_file_name", json_file_name,
                                "-verbose_level", "1"])
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
        run_analyze_offline(ana_json_in)
        run_json_to_root(ana_json_in, ana_root)
        run_root_to_json(ana_root, ana_json_out)
        self.__check_equality(ana_json_in, ana_json_out)


    def _test_simulate_mice(self): #pylint: disable=R0201
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
        #run_mc_simulation(sim_json_in)
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
            jout = json.loads(json_fin[i])
            self.__almost_equal(jin, jout, verbose=False)


    def __almost_equal(self, item1, item2, verbose=True, depth=0, tol=1e-9):
        """
        Check that two json data structures are the same
        """
        if verbose == True:
            print depth
        my_msg =  "item1:\n"+json.dumps(item1, indent=2)
        my_msg += "item2:\n"+json.dumps(item2, indent=2)
        self.assertEqual(type(item1), type(item2), msg=my_msg)
        if type(item1) == type({}):
            self.assertEqual(item1.keys(), item2.keys(), msg=my_msg)
            if verbose == True:
                print item1.keys()
                print item2.keys()
            for key in item1.keys():
                self.__almost_equal(item1[key], item2[key],
                                        verbose=verbose, depth=depth+1, tol=tol)
        elif type(item1) == type([]):
            self.assertEqual(len(item1), len(item2), msg=my_msg)
            for i in range(len(item1)):
                self.__almost_equal(item1[i], item2[i],
                                        verbose=verbose, depth=depth+1, tol=tol)
        elif type(item1) == type(1.):
            self.assertAlmostEqual(item1, item2, tol+item1*tol, msg=my_msg)
        else:
            self.assertEqual(item1, item2, msg=my_msg)

if __name__ == "__main__":
    unittest.main()


