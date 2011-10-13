import io  #  generic python library for I/O
import sys # for command line arguments
import os
import json
import unittest
from Configuration import Configuration
import MAUS

from MapCppTOFDigits import MapCppTOFDigits

class MapCppTOFDigitsTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.mapper = MAUS.MapCppTOFDigits()
        self.c = Configuration()

    def test_empty(self):
        result = self.mapper.birth("")
        self.assertFalse(result)
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_init(self):
        success = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(success)

    def test_no_data(self):
        test1 = '%s/src/map/MapCppTOFDigits/noDataTest.txt' %os.environ.get("MAUS_ROOT_DIR")
        f = open(test1,'r')
        data = f.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = json.loads(result)
        self.assertFalse('digits' in spill_out)

    def test_process(self):
        test2 = '%s/src/map/MapCppTOFDigits/processTest.txt' %os.environ.get("MAUS_ROOT_DIR")
        f = open(test2,'r')
        data = f.read()
        # test withe some crazy events.
        result = self.mapper.process(data)
        spill_in = json.loads(data)
        spill_out = json.loads(result)

        # test the tof0 output
        n_part_events = len(spill_out['digits']['tof0'])
        self.assertEqual(n_part_events,2)
        n_digits_partEv0 = len(spill_out['digits']['tof0'][0])
        self.assertEqual(n_digits_partEv0,3)
        self.assertFalse(spill_out['digits']['tof0'][1])
        # test the creation of the digit
        digit0_partEv0_tof0 = spill_out['digits']['tof0'][0][0]
        l_time = spill_in['daq_data']['tof0'][0]['V1290'][0]['leading_time']
        charge_mm = spill_in['daq_data']['tof0'][0]['V1724'][0]['charge_mm']
        trig = spill_in['daq_data']['trigger'][0]['V1290'][1]['leading_time']
        trig_req = spill_in['daq_data']['trigger_request'][0]['V1290'][1]['leading_time']
        self.assertEqual(l_time, digit0_partEv0_tof0['leading_time'])
        self.assertEqual(charge_mm, digit0_partEv0_tof0['charge_mm'])
        self.assertEqual(trig, digit0_partEv0_tof0['trigger_leading_time'])
        self.assertEqual(trig_req, digit0_partEv0_tof0['trigger_request_leading_time'])

        # test the tof1 output
        n_part_events = len(spill_out['digits']['tof1'])
        self.assertEqual(n_part_events,2)
        n_digits_partEv0_tof1 = len(spill_out['digits']['tof1'][0])
        self.assertEqual(n_digits_partEv0,3)
        n_digits_partEv1_tof1 = len(spill_out['digits']['tof1'][1])
        self.assertEqual(n_digits_partEv1_tof1,3)
        digit2_partEv1_tof1 = spill_out['digits']['tof1'][1][2]
        self.assertFalse('charge_mm' in digit2_partEv1_tof1)
        self.assertFalse('charge_pm' in digit2_partEv1_tof1)

        # test the tof2 output
        n_part_events = len(spill_out['digits']['tof2'])
        self.assertEqual(n_part_events,2)
        self.assertFalse(spill_out['digits']['tof2'][0])
        self.assertFalse(spill_out['digits']['tof2'][1])

    @classmethod
    def tearDownClass(self):
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()