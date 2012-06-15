"""
Unit test for ReducePyScalers module
"""
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
#

# Invalid name # pylint: disable=C0103
# Too many public methods # pylint: disable=R0904
# Access to a protected member # pylint: disable=W0212

import unittest
import json
from Configuration import Configuration
from ReducePyScalers import ReducePyScalers

class ReducePyScalersTestCase(unittest.TestCase):
    """ Class to test ReducePyScalers module """

    def setUp(self):
        """ Initialiser. setUp is called before each test function is called."""
        self._reducer = ReducePyScalers()
        conf = Configuration()
        config_doc = json.loads(conf.getConfigJSON())

        key = 'output_scalers_file_name'
        if ( key in config_doc ):
            self._output_file_name = config_doc[key]
        else:
            self._output_file_name = 'scalers.dat'

        # Test whether the configuration files were loaded correctly at birth
        success = self._reducer.birth(conf.getConfigJSON())
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

    def tearDown(self):
        """ Called after each test. Tests death. """
        self.assertTrue(self._reducer.death())

    def test_init(self):
        """ Test the constructor of the reducer class """
        self.assertEqual(self._reducer._hits, {})
        self.assertEqual(self._reducer._triggers, [])
        self.assertEqual(self._reducer._trigger_requests, [])
        self.assertEqual(self._reducer._gva, [])
        self.assertEqual(self._reducer._tof0, [])
        self.assertEqual(self._reducer._tof1, [])
        self.assertEqual(self._reducer._lm12, [])
        self.assertEqual(self._reducer._lm34, [])
        self.assertEqual(self._reducer._lm1234, [])
        self.assertEqual(self._reducer._clock, [])

    def test_birth(self):
        """Check birth() function works when it should """
        # Close the output file and check the header was written
        self.assertTrue(self._reducer.death())
        fin = open( self._output_file_name, 'r')
        header = 'Timestamp\tSpill\tTrigs\tTReqs\tGVA1\tTOF0\t'
        header = header + 'TOF1\tLM12\tLM34\tLM1234\tClock\n'
        line = fin.readline()
        self.assertEqual(line, header)

    def test_birth_exception(self):
        """ Check birth returns false if handed a nonsense json_config """
        self.assertFalse(self._reducer.birth('blah'))

    def test_add(self):
        """ Check add function works """

        # Add a spill
        hits = {'ch0':0, 'ch1':1, 'ch2':2, 'ch3':3, 'ch4':4, \
                'ch9':9, 'ch10':10, 'ch11':11, 'ch12':12}
        self._reducer.add(hits)

        # Add a second spill
        hits = {'ch0':10, 'ch1':11, 'ch2':12, 'ch3':13, 'ch4':14, \
                'ch9':19, 'ch10':110, 'ch11':111, 'ch12':112}
        self._reducer.add(hits)

        # Check both spills are added
        self.assertEqual(self._reducer._triggers, [0, 10])
        self.assertEqual(self._reducer._trigger_requests, [1, 11])
        self.assertEqual(self._reducer._gva, [2, 12])
        self.assertEqual(self._reducer._tof0, [3, 13])
        self.assertEqual(self._reducer._tof1, [4, 14])
        self.assertEqual(self._reducer._lm12, [9, 19])
        self.assertEqual(self._reducer._lm34, [10, 110])
        self.assertEqual(self._reducer._lm1234, [11, 111])
        self.assertEqual(self._reducer._clock, [12, 112])

    def test_add_remove_entries(self):
        """ Check add func never makes members have a length longer than 10 """

        # Add 11 spills, 1st spill added should be removed after last spill is
        for i in range(11):
            hits = {'ch0':i, 'ch1':i, 'ch2':i, 'ch3':i, 'ch4':i, \
                    'ch9':i, 'ch10':i, 'ch11':i, 'ch12':i}
            self._reducer.add(hits)


        # Check the length # pylint: disable=W0212
        self.assertEqual(len(self._reducer._triggers), 10)
        self.assertEqual(len(self._reducer._trigger_requests), 10)
        self.assertEqual(len(self._reducer._gva), 10)
        self.assertEqual(len(self._reducer._tof0), 10)
        self.assertEqual(len(self._reducer._tof1), 10)
        self.assertEqual(len(self._reducer._lm12), 10)
        self.assertEqual(len(self._reducer._lm34), 10)
        self.assertEqual(len(self._reducer._lm1234), 10)
        self.assertEqual(len(self._reducer._clock), 10)

        # Check the last spill added
        self.assertEqual(self._reducer._triggers[9], 10)
        self.assertEqual(self._reducer._trigger_requests[9], 10)
        self.assertEqual(self._reducer._gva[9], 10)
        self.assertEqual(self._reducer._tof0[9], 10)
        self.assertEqual(self._reducer._tof1[9], 10)
        self.assertEqual(self._reducer._lm12[9], 10)
        self.assertEqual(self._reducer._lm34[9], 10)
        self.assertEqual(self._reducer._lm1234[9], 10)
        self.assertEqual(self._reducer._clock[9], 10)

        # Check the first spill remaining
        self.assertEqual(self._reducer._triggers[0], 1)
        self.assertEqual(self._reducer._trigger_requests[0], 1)
        self.assertEqual(self._reducer._gva[0], 1)
        self.assertEqual(self._reducer._tof0[0], 1)
        self.assertEqual(self._reducer._tof1[0], 1)
        self.assertEqual(self._reducer._lm12[0], 1)
        self.assertEqual(self._reducer._lm34[0], 1)
        self.assertEqual(self._reducer._lm1234[0], 1)
        self.assertEqual(self._reducer._clock[0], 1)


    def test_dump(self):
        """ Check dump function works when it should """

        # Add a spill
        hits = {'ch0':0, 'ch1':1, 'ch2':2, 'ch3':3, 'ch4':4, \
                'ch9':9, 'ch10':10, 'ch11':11, 'ch12':12}
        self._reducer.add(hits)
        self._reducer.dump(False)

        # Add a second spill
        hits = {'ch0':10, 'ch1':11, 'ch2':12, 'ch3':13, 'ch4':14, \
                'ch9':19, 'ch10':110, 'ch11':111, 'ch12':112}
        self._reducer.add(hits)
        self._reducer.dump(False)

        self._reducer.death()

        fin = open( self._output_file_name, 'r')

        # Ignore header line
        fin.readline()

        # Check first spill
        first_data_line = '0\t1\t2\t3\t4\t9\t10\t11\t12\n'
        line = fin.readline()
        self.assertEqual(line, first_data_line)

        # Check second spill
        second_data_line = '10\t11\t12\t13\t14\t19\t110\t111\t112\n'
        line = fin.readline()
        self.assertEqual(line, second_data_line)

        fin.close()

    def test_dump_no_data(self):
        """ Check dump function works when no data is present """
        self._reducer.dump(False)
        self._reducer.death()
        fin = open( self._output_file_name, 'r')

        # Ignore header line
        fin.readline()

        # Check first spill
        first_data_line = '0\t0\t0\t0\t0\t0\t0\t0\t0\n'
        line = fin.readline()
        self.assertEqual(line, first_data_line)


    def test_process(self):
        """ Test the process function using a small amount of real json data """
        line = \
        '{"daq_data": {"V830": {"channels": {"ch27": 3, "ch26": 2, "ch25": 1, \
        "ch24": 1, "ch23": 2, "ch22": 6, "ch21": 13, "ch20": 6, "ch29": 14, \
        "ch28": 12, "ch30": 7, "ch31": 2, "ch12": 3237, "ch13": 0, \
        "ch10": 315, "ch11": 0, "ch16": 0, "ch17": 1, "ch14": 0, "ch15": 0, \
        "ch18": 3, "ch19": 11, "ch0": 36, "ch1": 38, "ch2": 2227, "ch3": 89, \
        "ch4": 38, "ch5": 0, "ch6": 98, "ch7": 7, "ch8": 1151, "ch9": 1132}, \
        "time_stamp": 1281804484, "phys_event_number": 0}}}'

        self._reducer.process(line)
        self._reducer.death()

        fin = open( self._output_file_name, 'r')

        # Ignore header line
        fin.readline()

        first_line = '1281804484\t0\t36\t38\t2227\t89\t38\t1132\t315\t0\t3237\n'
        line = fin.readline()
        self.assertEqual(line, first_line)

    def test_process_no_daq_data(self):
        """ Test that process returns the json doc
            it was given if no daq_data entry is present """
        line = \
        '{"blah": {"V830": {"channels": {"ch27": 3, "ch26": 2, "ch25": 1, \
        "ch24": 1, "ch23": 2, "ch22": 6, "ch21": 13, "ch20": 6, "ch29": 14, \
        "ch28": 12, "ch30": 7, "ch31": 2, "ch12": 3237, "ch13": 0, \
        "ch10": 315, "ch11": 0, "ch16": 0, "ch17": 1, "ch14": 0, "ch15": 0, \
        "ch18": 3, "ch19": 11, "ch0": 36, "ch1": 38, "ch2": 2227, "ch3": 89, \
        "ch4": 38, "ch5": 0, "ch6": 98, "ch7": 7, "ch8": 1151, "ch9": 1132}, \
        "time_stamp": 1281804484, "phys_event_number": 0}}}'

        returned_line = self._reducer.process(line)
        self.assertEqual(line, returned_line)

    def test_process_no_V830(self):
        """ Test that process returns the json doc
            it was given if no V830 entry is present """
        line = \
        '{"daq_data": {"blah": {"channels": {"ch27": 3, "ch26": 2, "ch25": 1, \
        "ch24": 1, "ch23": 2, "ch22": 6, "ch21": 13, "ch20": 6, "ch29": 14, \
        "ch28": 12, "ch30": 7, "ch31": 2, "ch12": 3237, "ch13": 0, \
        "ch10": 315, "ch11": 0, "ch16": 0, "ch17": 1, "ch14": 0, "ch15": 0, \
        "ch18": 3, "ch19": 11, "ch0": 36, "ch1": 38, "ch2": 2227, "ch3": 89, \
        "ch4": 38, "ch5": 0, "ch6": 98, "ch7": 7, "ch8": 1151, "ch9": 1132}, \
        "time_stamp": 1281804484, "phys_event_number": 0}}}'

        returned_line = self._reducer.process(line)
        self.assertEqual(line, returned_line)

    def test_process_no_channels(self):
        """ Test that process returns the json doc
        it was given if no channels entry is present """
        line = \
        '{"daq_data": {"V830": {"blah": {"ch27": 3, "ch26": 2, "ch25": 1, \
        "ch24": 1, "ch23": 2, "ch22": 6, "ch21": 13, "ch20": 6, "ch29": 14, \
        "ch28": 12, "ch30": 7, "ch31": 2, "ch12": 3237, "ch13": 0, \
        "ch10": 315, "ch11": 0, "ch16": 0, "ch17": 1, "ch14": 0, "ch15": 0, \
        "ch18": 3, "ch19": 11, "ch0": 36, "ch1": 38, "ch2": 2227, "ch3": 89, \
        "ch4": 38, "ch5": 0, "ch6": 98, "ch7": 7, "ch8": 1151, "ch9": 1132}, \
        "time_stamp": 1281804484, "phys_event_number": 0}}}'

        returned_line = self._reducer.process(line)
        self.assertEqual(line, returned_line)


if __name__ == '__main__':
    unittest.main()
