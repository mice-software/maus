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
from Configuration import Configuration
import MAUS

class MapCppGlobalPID(unittest.TestCase): # pylint: disable = R0904
    """Tests for MapCppGlobalPID"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MAUS.MapCppGlobalPID()
        cls.c = Configuration()

    def test_empty(self):
        """Check can handle empty configuration"""
        result = self.mapper.birth("")
        self.assertFalse(result)
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_init(self):
        """Check birth with default configuration"""
        success = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(success)

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppGlobalPID/noDataTest.txt' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = json.loads(result)
        self.assertFalse('global_event' in spill_out)

    def test_invalid_json_birth(self):
        """Check birth with an invalid json input"""
        test2 = ('%s/src/map/MapCppGlobalPID/invalid.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin1 = open(test2,'r')
        data = fin1.read()
        # test with no data.
        result = self.mapper.birth(data)
        self.assertFalse(result)
        test3 = ('%s/src/map/MapCppGlobalPID/Global_Event.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin2 = open(test3,'r')
        line = fin2.readline()
        result = self.mapper.process(line)
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_invalid_json_process(self):
        """Check process with an invalid json input"""
        birthresult = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(birthresult)
        test3 = ('%s/src/map/MapCppGlobalPID/invalid.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test3,'r')
        data = fin.read()
        result = self.mapper.process(data)
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_muon_PID(self):
        """Check that process can identify muons"""
        test4 = ('%s/src/map/MapCppGlobalPID/muon_pid_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        birthresult = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(birthresult)
        fin = open(test4,'r')
        for line in fin:
            result = self.mapper.process(line)
            spill_out = json.loads(result)
            self.assertTrue('recon_events' in spill_out)
            revtarray = spill_out['recon_events']
            self.assertEqual(1, len(revtarray))
            revt = revtarray[0]
            self.assertTrue('global_event' in revt)
            gevt = revt['global_event']
            self.assertEqual(4, len(gevt)) 
            self.assertTrue('tracks' in gevt)
            tracksarray = gevt['tracks']
            self.assertEqual(1, len(gevt['tracks']))
            track = tracksarray[0]
            self.assertTrue('pid' in track)
            print track['pid']
            self.assertEqual(-13, track['pid'])

    def test_positron_PID(self):
        """Check that process can identify positrons"""
        test5 = ('%s/src/map/MapCppGlobalPID/positron_pid_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        birthresult = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(birthresult)
        fin = open(test5,'r')
        for line in fin:
            result = self.mapper.process(line)
            spill_out = json.loads(result)
            self.assertTrue('recon_events' in spill_out)
            revtarray = spill_out['recon_events']
            self.assertEqual(1, len(revtarray))
            revt = revtarray[0]
            self.assertTrue('global_event' in revt)
            gevt = revt['global_event']
            self.assertEqual(4, len(gevt)) 
            self.assertTrue('tracks' in gevt)
            tracksarray = gevt['tracks']
            self.assertEqual(1, len(gevt['tracks']))
            track = tracksarray[0]
            self.assertTrue('pid' in track)
            print track['pid']
            self.assertEqual(-11, track['pid'])

    def test_pion_PID(self):
        """Check that process can identify pions"""
        test6 = ('%s/src/map/MapCppGlobalPID/pion_pid_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        birthresult = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(birthresult)
        fin = open(test6,'r')
        for line in fin:
            result = self.mapper.process(line)
            spill_out = json.loads(result)
            self.assertTrue('recon_events' in spill_out)
            revtarray = spill_out['recon_events']
            self.assertEqual(1, len(revtarray))
            revt = revtarray[0]
            self.assertTrue('global_event' in revt)
            gevt = revt['global_event']
            self.assertEqual(4, len(gevt)) 
            self.assertTrue('tracks' in gevt)
            tracksarray = gevt['tracks']
            self.assertEqual(1, len(gevt['tracks']))
            track = tracksarray[0]
            self.assertTrue('pid' in track)
            print track['pid']
            self.assertEqual(211, track['pid'])

    def test_undef_PID(self):
        """Check that PID set to 0 for indistinguishable particles"""
        test7 = ('%s/src/map/MapCppGlobalPID/undef_pid_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        birthresult = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(birthresult)
        fin = open(test7,'r')
        for line in fin:
            result = self.mapper.process(line)
            spill_out = json.loads(result)
            self.assertTrue('recon_events' in spill_out)
            revtarray = spill_out['recon_events']
            self.assertEqual(1, len(revtarray))
            revt = revtarray[0]
            self.assertTrue('global_event' in revt)
            gevt = revt['global_event']
            self.assertTrue('tracks' in gevt)
            tracksarray = gevt['tracks']
            track = tracksarray[0]
            self.assertTrue('pid' in track)
            print track['pid']
            self.assertEqual(0, track['pid'])

    def test_invalid_logL(self):
        """Check that a track that returns an invalid logL does not get
        set a PID"""
        test8 = ('%s/src/map/MapCppGlobalPID/invalid_logL.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        birthresult = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(birthresult)
        fin = open(test8,'r')
        for line in fin:
            result = self.mapper.process(line)
            spill_out = json.loads(result)
            self.assertTrue('recon_events' in spill_out)
            revtarray = spill_out['recon_events']
            self.assertEqual(1, len(revtarray))
            revt = revtarray[0]
            self.assertTrue('global_event' in revt)
            gevt = revt['global_event']
            self.assertEqual(4, len(gevt)) 
            self.assertTrue('tracks' in gevt)
            tracksarray = gevt['tracks']
            self.assertEqual(3, len(gevt['tracks']))
            track = tracksarray[0]
            self.assertTrue('pid' in track)
            print track['pid']
            self.assertEqual(0, track['pid'])

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that we can death() MapCppGlobalPID"""
        success = cls.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
