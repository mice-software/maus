# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
#

"""Tests for MapPyCkov"""
# pylint: disable = C0103
import os
import json
import unittest
from Configuration import Configuration
import MAUS

class MapPyCkovTestCase(unittest.TestCase):# pylint: disable = R0904
    """Test for MapPyCkov"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Initialize a Ckov object"""
        cls.mapper = MAUS.MapPyCkov()
        cls.c = Configuration()
        
    def test_empty(self):
        """Check against empty configuration"""
        result = self.mapper.birth("")
        self.assertFalse(result)
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_init(self):
        """Check that birth works properly"""
        success = self.mapper.birth(self.c.getConfigJSON())
        self.assertTrue(success)

    def test_no_data(self):
        """Check that against data stream is empty"""
        test1 = ('%s/src/map/MapPyCkov/noDataTest.txt'%
                                        os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1, 'r')
        data = fin.read()
        #test = json.dumps({"daq_data":None, "bob":0})
        #test no data.
        result = self.mapper.process(data)
        spill = json.loads(result)
        no_ckov = True
        if 'ckov' in spill:
            no_ckov = False
        self.assertTrue(no_ckov)
        
    def test_process(self):
        """Check MapPyCkov process function"""
        test2 = ('%s/src/map/MapPyCkov/goodmap2.txt' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2, 'r')
        data = fin.readline()
        #test with some events.
        result = self.mapper.process(data)
        spill = json.loads(result)
        #print 'spill= ',spill
        if "digits" not in spill:
            print 'no digits'

        #test the ckov output
        n_part_events = int(len(spill['recon_events'])) - 1
        #test there are no dupilcate events
        last_part_event_number = \
             spill['recon_events'][-1]['ckov_event']['ckov_digits'][-1]['A']['part_event_number']
        self.assertEqual(last_part_event_number, n_part_events)

    def tearDown(self): #pylint: disable = C0103
        """Check death works"""
        success = self.mapper.death()
        if not success:
            raise Exception('Initialize Fail', 'Could not start worker')
        #self.mapper = None


if __name__ == '__main__':
    unittest.main()
