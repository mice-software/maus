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

"""Tests for MapCppCkovDigits"""
# pylint: disable = C0103
import os
import unittest
from Configuration import Configuration
import MAUS
import maus_cpp.converter

class MapCppCkovDigitsTestCase(unittest.TestCase):# pylint: disable = R0904
    """Test for MapCppCkovDigits"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Initialize a Ckov object"""
        cls.mapper = MAUS.MapCppCkovDigits()
        cls.c = Configuration()
        print '== done setup =='

    def test_empty(self):
        """Check against empty configuration"""
        self.assertRaises(ValueError, self.mapper.birth, "")
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppCkovDigits" in doc["errors"])
        print '>>> done empty'

    def test_init(self):
        """Check that birth works properly"""
        self.mapper.birth(self.c.getConfigJSON())

    def test_no_data(self):
        """Check that against data stream is empty"""
        test1 = ('%s/src/map/MapCppCkovDigits/noDataTest.txt'%
                                        os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1, 'r')
        data = fin.read()
        result = self.mapper.process(data)
        spill = maus_cpp.converter.json_repr(result)
        no_ckov = True
        if 'ckov' in spill:
            no_ckov = False
        self.assertTrue(no_ckov)

    def test_process(self):
        """Check MapCppCkovDigits process function"""
        test2 = ('%s/src/map/MapCppCkovDigits/goodmap2.txt' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2, 'r')
        data = fin.read()
        #test with some events.
        result = self.mapper.process(data)
        spill = maus_cpp.converter.json_repr(result)
        if "digits" not in spill:
            print 'no digits'

        #test the ckov output
        n_part_events = int(len(spill['recon_events'])) - 1
        #test there are no dupilcate events
        last_part_event_number = spill['recon_events'][-1]['ckov_event']\
                                   ['ckov_digits'][-1]['A']['part_event_number']
        self.assertEqual(last_part_event_number, n_part_events)

    #pylint: disable = E0213
    def tearDown(cls): #pylint: disable = C0103
        """Check death works"""
        cls.mapper.death()
        cls.mapper = None


if __name__ == '__main__':
    unittest.main()
