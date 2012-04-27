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
    subproc = subprocess.Popen([SIMULATION,
                                "-output_json_file_name", json_file_name,
                                "-simulation_geometry_filename", "Test.dat"])
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
    def _test_analyze_offline(self): #pylint: disable=R0201
        """
        check that we can run the offline analysis, convert to root, convert
        back to json
        """
        ana_json = os.path.join \
                              (MRD, "tmp", "test_root_io_offline_analysis.json")
        ana_root = os.path.join \
                              (MRD, "tmp", "test_root_io_offline_analysis.root")
        run_analyze_offline(ana_json)
        run_json_to_root(ana_json, ana_root)

    def test_simulate_mice(self): #pylint: disable=R0201
        """
        check that we can run simulate_mice, convert to root, convert back to
        json
        """
        sim_json = os.path.join \
                              (MRD, "tmp", "test_root_io_simulate_mice_IN.json")
        sim_root = os.path.join \
                              (MRD, "tmp", "test_root_io_simulate_mice.root")
        sim_json_out = os.path.join \
                             (MRD, "tmp", "test_root_io_simulate_mice_OUT.json")
#        run_mc_simulation(sim_json)
        run_json_to_root(sim_json, sim_root)
        run_root_to_json(sim_root, sim_json_out)


if __name__ == "__main__":
    unittest.main()


