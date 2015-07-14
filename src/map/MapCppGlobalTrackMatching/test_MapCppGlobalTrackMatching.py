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

    def test_init(self):
        """Check birth with default configuration"""
        self.mapper.birth(self. c.getConfigJSON())
    
    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppGlobalTrackMatching/noDataTest.txt' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        self.mapper.birth(self.c.getConfigJSON())
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse('global_event' in spill_out)

    def test_invalid_json_birth(self):
        """Check birth with an invalid json input"""
        test2 = ('%s/src/map/MapCppGlobalTrackMatching/invalid.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin1 = open(test2,'r')
        data = fin1.read()
        # test with no data.
        self.assertRaises(ValueError, self.mapper.birth, data)
        test3 = ('%s/src/map/MapCppGlobalTrackMatching/Global_TM_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin2 = open(test3,'r')
        fin2.readline()
        fin2.readline()
        fin2.readline()
        line = fin2.readline()
        result = self.mapper.process(line)
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("MapCppGlobalTrackMatching" in doc["errors"])

    def test_invalid_json_process(self):
        """Check process with an invalid json input"""
        self.mapper.birth(self. c.getConfigJSON())
        test4 = ('%s/src/map/MapCppGlobalTrackMatching/invalid.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test4,'r')
        data = fin.read()
        result = self.mapper.process(data)
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("MapCppGlobalTrackMatching" in doc["errors"])

    def test_fill_Global_Event(self):
        """Check that process makes global tracks from TOF and tracker data"""
        test5 = ('%s/src/map/MapCppGlobalTrackMatching/global_tm_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        self.mapper.birth(self. c.getConfigJSON())
        fin = open(test5,'r')
        line = fin.read()
        result = self.mapper.process(line)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertTrue('recon_events' in spill_out)
        revtarray = spill_out['recon_events']
        self.assertEqual(1, len(revtarray))
        revt = revtarray[0]
        self.assertTrue('global_event' in revt)
        self.assertTrue('track_points' in revt['global_event'])
        numTMtrackpoints = 0
        for i in revt['global_event']['track_points']:
            self.assertTrue('mapper_name' in i)
            if i['mapper_name'] == 'MapCppGlobalTrackMatching':
                numTMtrackpoints += 1
        self.assertEqual(numTMtrackpoints, 43)
        self.assertTrue('tracks' in revt['global_event'])
        self.assertEqual(4, len(revt['global_event']['tracks']))
        numTMtracks = 0
        for i in revt['global_event']['tracks']:
            self.assertTrue('mapper_name' in i)
            if i['mapper_name'] == 'MapCppGlobalTrackMatching':
                numTMtracks += 1
        self.assertEqual(numTMtracks, 1)
        self.assertTrue('space_points' in revt['global_event'])
        self.assertEqual(40, len(revt['global_event']['space_points']))
        self.assertTrue('primary_chains' in revt['global_event'])
        self.assertEqual(0, len(revt['global_event']['primary_chains']))

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that we can death() MapCppGlobalTrackMatching"""
        cls.mapper.death()

if __name__ == '__main__':
    unittest.main()
