# This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""Tests for MapCppGlobalTrackMatching"""

# pylint: disable = C0103

import os
import json
import unittest
from Configuration import Configuration
import maus_cpp.converter
from _MapCppGlobalTrackMatching import MapCppGlobalTrackMatching

class MapCppGlobalTMTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for MapCppGlobalTrackMatching"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MapCppGlobalTrackMatching()
        cls.c = Configuration()
    
    def test_empty(self):
        """Check can handle empty configuration"""
        self.assertRaises(ValueError, self.mapper.birth, "",)
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppGlobalTrackMatching" in doc["errors"])

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppGlobalTrackMatching/nodata.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        ###
        # self.mapper.birth(self.c.getConfigJSON())
        test_configuration = self.c.getConfigJSON()
        test_conf_json = json.loads(test_configuration)
        test_conf_json['reconstruction_geometry_filename'] = (
            os.environ.get("MAUS_ROOT_DIR") +
            "/tests/cpp_unit/Recon/Global/TestGeometries/SimpleBeamline.dat")
        test_conf_json['simulation_geometry_filename'] = \
            test_conf_json['reconstruction_geometry_filename']
        test_configuration = json.dumps(test_conf_json)
        self.mapper.birth(test_configuration)
        ###
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse('global_event' in spill_out)

    def test_invalid_json_birth(self):
        """Check birth with an invalid json input"""
        test2 = ('%s/src/map/MapCppGlobalTrackMatching/invalid.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin1 = open(test2,'r')
        data = fin1.read()
        self.assertRaises(ValueError, self.mapper.birth, data)
        # test with valid data but invalid configuration
        test3 = ('%s/src/map/MapCppGlobalTrackMatching/testdata.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin2 = open(test3,'r')
        line = fin2.readline()
        result = self.mapper.process(line)
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("MapCppGlobalTrackMatching" in doc["errors"])

    def test_invalid_json_process(self):
        """Check process with an invalid json input"""
        ###
        # self.mapper.birth(self.c.getConfigJSON())
        test_configuration = self.c.getConfigJSON()
        test_conf_json = json.loads(test_configuration)
        test_conf_json['reconstruction_geometry_filename'] = (
            os.environ.get("MAUS_ROOT_DIR") +
            "/tests/cpp_unit/Recon/Global/TestGeometries/SimpleBeamline.dat")
        test_conf_json['simulation_geometry_filename'] = \
            test_conf_json['reconstruction_geometry_filename']
        test_configuration = json.dumps(test_conf_json)
        self.mapper.birth(test_configuration)
        ###
        test3 = ('%s/src/map/MapCppGlobalTrackMatching/invalid.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test3,'r')
        data = fin.read()
        result = self.mapper.process(data)
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("MapCppGlobalTrackMatching" in doc["errors"])

    def test_fill_Global_Event(self): # pylint: disable = R0914
        """Check that process makes global tracks from TOF and tracker data"""
        test4 = ('%s/src/map/MapCppGlobalTrackMatching/testdata.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        ###
        # self.mapper.birth(self.c.getConfigJSON())
        test_configuration = self.c.getConfigJSON()
        test_conf_json = json.loads(test_configuration)
        test_conf_json['reconstruction_geometry_filename'] = (
            os.environ.get("MAUS_ROOT_DIR") +
            "/tests/cpp_unit/Recon/Global/TestGeometries/SimpleBeamline.dat")
        test_conf_json['simulation_geometry_filename'] = \
            test_conf_json['reconstruction_geometry_filename']
        test_conf_json['track_matching_pid_hypothesis'] = "kMuPlus"
        test_configuration = json.dumps(test_conf_json)
        self.mapper.birth(test_configuration)
        ###
        fin = open(test4,'r')
        line = fin.read()
        result = self.mapper.process(line)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertTrue('recon_events' in spill_out)
        revtarray = spill_out['recon_events']
        self.assertEqual(1, len(revtarray))
        revt = revtarray[0]
        self.assertTrue('global_event' in revt)
        self.assertTrue('track_points' in revt['global_event'])
        num_us_trackpoints = 0
        num_ds_trackpoints = 0
        for i in revt['global_event']['track_points']:
            self.assertTrue('mapper_name' in i)
            if i['mapper_name'] == 'MapCppGlobalTrackMatching-US':
                num_us_trackpoints += 1
            if i['mapper_name'] == 'MapCppGlobalTrackMatching-DS':
                num_ds_trackpoints += 1
        self.assertEqual(num_us_trackpoints, 3)
        self.assertEqual(num_ds_trackpoints, 4)
        self.assertTrue('tracks' in revt['global_event'])
        num_us_tracks = 0
        num_ds_tracks = 0
        num_through_tracks = 0
        for i in revt['global_event']['tracks']:
            self.assertTrue('mapper_name' in i)
            if i['mapper_name'] == 'MapCppGlobalTrackMatching-US':
                num_us_tracks += 1
            if i['mapper_name'] == 'MapCppGlobalTrackMatching-DS':
                num_ds_tracks += 1
            if i['mapper_name'] == 'MapCppGlobalTrackMatching-Through':
                num_through_tracks += 1
        self.assertEqual(num_us_tracks, 1)
        self.assertEqual(num_ds_tracks, 1)
        self.assertEqual(num_through_tracks, 1)

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that we can death() MapCppGlobalTrackMatching"""
        cls.mapper.death()

if __name__ == '__main__':
    unittest.main()
