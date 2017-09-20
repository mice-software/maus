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

"""Tests for MapCppGlobalPID"""

# pylint: disable = C0103

import os
import json
import unittest
import Configuration
import maus_cpp.converter
from _MapCppGlobalPID import MapCppGlobalPID

class MapCppGlobalPIDTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for MapCppGlobalPID"""
    config_file = ('%s/src/map/MapCppGlobalPID/test_config.py' %
                 os.environ.get("MAUS_ROOT_DIR"))
    config0 = json.loads(Configuration.Configuration().getConfigJSON
                         (config_file = (open(config_file,'r'))))

    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MapCppGlobalPID()


    def test_empty(self):
        """Check can handle empty configuration"""
        self.assertRaises(ValueError, self.mapper.birth, "",)
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppGlobalPID" in doc["errors"])

    def test_init(self):
        """Check birth with default configuration"""
        self.mapper.birth(json.dumps(self.config0))

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppGlobalPID/noDataTest.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        self.mapper.birth(json.dumps(self.config0))
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertEqual(len(spill_out["errors"]), 0)

    def test_invalid_json_birth(self):
        """Check birth with an invalid json input"""
        test2 = ('%s/src/map/MapCppGlobalPID/invalid.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin1 = open(test2,'r')
        data = fin1.read()
        # test with no data.
        self.assertRaises(ValueError, self.mapper.birth, data)
        test3 = ('%s/src/map/MapCppGlobalPID/Global_Event.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin2 = open(test3,'r')
        line = fin2.readline()
        result = self.mapper.process(line)
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("MapCppGlobalPID" in doc["errors"])

    def test_invalid_json_process(self):
        """Check process with an invalid json input"""
        self.mapper.birth(json.dumps(self.config0))
        test3 = ('%s/src/map/MapCppGlobalPID/invalid.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test3,'r')
        data = fin.read()
        result = self.mapper.process(data)
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("MapCppGlobalPID" in doc["errors"])

    def test_muon_PID(self):
        """Check that process can identify muons"""
        test4 = ('%s/src/map/MapCppGlobalPID/muon_pid_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        self.mapper.birth(json.dumps(self.config0))
        fin = open(test4,'r')
        for line in fin:
            result = self.mapper.process(line)
            spill_out = maus_cpp.converter.json_repr(result)
            self.assertTrue('recon_events' in spill_out)
            revtarray = spill_out['recon_events']
            self.assertEqual(1, len(revtarray))
            revt = revtarray[0]
            self.assertTrue('global_event' in revt)
            gevt = revt['global_event']
            self.assertEqual(4, len(gevt))
            self.assertTrue('tracks' in gevt)
            tracksarray = gevt['tracks']
            num_tracks = 0
            for track in tracksarray:
                if track['mapper_name'] == 'MapCppGlobalPID':
                    self.assertTrue('pid' in track)
                    self.assertEqual(-13, track['pid'])
                    num_tracks += 1
            self.assertEqual(num_tracks, 3)

    def test_positron_PID(self):
        """Check that process can identify positrons"""
        test5 = ('%s/src/map/MapCppGlobalPID/positron_pid_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        self.mapper.birth(json.dumps(self.config0))
        fin = open(test5,'r')
        for line in fin:
            result = self.mapper.process(line)
            spill_out = maus_cpp.converter.json_repr(result)
            self.assertTrue('recon_events' in spill_out)
            revtarray = spill_out['recon_events']
            self.assertEqual(1, len(revtarray))
            revt = revtarray[0]
            self.assertTrue('global_event' in revt)
            gevt = revt['global_event']
            self.assertEqual(4, len(gevt))
            self.assertTrue('tracks' in gevt)
            tracksarray = gevt['tracks']
            num_tracks = 0
            for track in tracksarray:
                if track['mapper_name'] == 'MapCppGlobalPID':
                    self.assertTrue('pid' in track)
                    self.assertEqual(-11, track['pid'])
                    num_tracks += 1
            self.assertEqual(num_tracks, 3)

    def test_pion_PID(self):
        """Check that process can identify pions"""
        test6 = ('%s/src/map/MapCppGlobalPID/pion_pid_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        self.mapper.birth(json.dumps(self.config0))
        fin = open(test6,'r')
        for line in fin:
            result = self.mapper.process(line)
            spill_out = maus_cpp.converter.json_repr(result)
            self.assertTrue('recon_events' in spill_out)
            revtarray = spill_out['recon_events']
            self.assertEqual(1, len(revtarray))
            revt = revtarray[0]
            self.assertTrue('global_event' in revt)
            gevt = revt['global_event']
            self.assertEqual(4, len(gevt))
            self.assertTrue('tracks' in gevt)
            tracksarray = gevt['tracks']
            num_tracks = 0
            for track in tracksarray:
                if track['mapper_name'] == 'MapCppGlobalPID':
                    self.assertTrue('pid' in track)
                    self.assertEqual(211, track['pid'])
                    num_tracks += 1
            self.assertEqual(num_tracks, 3)

    def test_invalid_logL(self):
        """Check that a track that returns an invalid logL does not get
        set a PID"""
        test8 = ('%s/src/map/MapCppGlobalPID/undef_pid_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        self.mapper.birth(json.dumps(self.config0))
        fin = open(test8,'r')
        for line in fin:
            result = self.mapper.process(line)
            spill_out = maus_cpp.converter.json_repr(result)
            self.assertTrue('recon_events' in spill_out)
            revtarray = spill_out['recon_events']
            self.assertEqual(1, len(revtarray))
            revt = revtarray[0]
            self.assertTrue('global_event' in revt)
            gevt = revt['global_event']
            self.assertEqual(4, len(gevt))
            self.assertTrue('tracks' in gevt)
            tracksarray = gevt['tracks']
            self.assertEqual(2, len(gevt['tracks']))
            num_tracks = 0
            for track in tracksarray:
                if track['mapper_name'] == 'MapCppGlobalPID':
                    num_tracks += 1
            self.assertEqual(num_tracks, 0)

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that we can death() MapCppGlobalPID"""
        cls.mapper.death()

if __name__ == '__main__':
    unittest.main()
