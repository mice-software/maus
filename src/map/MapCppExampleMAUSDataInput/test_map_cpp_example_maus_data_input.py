import ROOT
import unittest
import json

import _MapCppExampleMAUSDataInput
from _MapCppExampleMAUSDataInput import MapCppExampleMAUSDataInput

import Configuration

class test_map_cpp_example(unittest.TestCase):
    def setUp(self):
        self.config = Configuration.Configuration().getConfigJSON()

    def test_new_init_dealloc(self):
        map_example = MapCppExampleMAUSDataInput()

    def test_birth(self):
        map_example = MapCppExampleMAUSDataInput()
        self.assertRaises(TypeError, map_example.birth)
        self.assertRaises(TypeError, map_example.birth, (1,))
        self.assertRaises(TypeError, map_example.birth, ("", ""))
        map_example.birth(json.dumps(self.config))
        self.assertRaises(ValueError, map_example.birth, "")

    def test_death(self):
        map_example = MapCppExampleMAUSDataInput()
        map_example.death()

    def test_process(self):
        data_1 = \
        '{"daq_event_type":"","errors":{},"maus_event_type":"Spill",'+\
        '"run_number":99,"spill_number":666}'
        map_example = MapCppExampleMAUSDataInput()
        map_example.birth(json.dumps(self.config))
        data_2 = map_example.process(data_1)
        self.assertEqual(data_2.GetSpill().GetSpillNumber(),
                         667)
        data_3 = map_example.process(data_2)
        self.assertEqual(data_3.GetSpill().GetSpillNumber(),
                         data_2.GetSpill().GetSpillNumber()+1)

    def test_can_convert(self):
        self.assertTrue(MapCppExampleMAUSDataInput().can_convert)

    def test_import(self):
        from MAUS import MapCppExampleMAUSDataInput

if __name__ == "__main__":
    unittest.main()
