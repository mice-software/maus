import unittest

import ROOT

import libMausCpp
from maus_cpp import converter

def test_data():
    data = ROOT.MAUS.Data()
    spill = ROOT.MAUS.Spill()
    spill.SetRunNumber(99)
    spill.SetSpillNumber(666)
    data.SetSpill(spill)
    return data 

class ConverterTestCase(unittest.TestCase):
    def test_cpp_to_json_spill_convert(self):
        self.assertRaises(ValueError, converter._cpp_to_json_spill_convert)
        self.assertRaises(ValueError, converter._cpp_to_json_spill_convert, ("",))
        dummy_tree = ROOT.TTree()
        self.assertRaises(ValueError, converter._cpp_to_json_spill_convert, dummy_tree)
        maus_data = test_data()
        self.assertRaises(ValueError, converter._cpp_to_json_spill_convert, (maus_data, ""))
        json_str = converter._cpp_to_json_spill_convert(maus_data)

    def test_json_to_cpp_spill_convert(self):
        with self.assertRaises(ValueError):
            converter._json_to_cpp_spill_convert()
        with self.assertRaises(ValueError):
            converter._json_to_cpp_spill_convert(1,)
        with self.assertRaises(ValueError):
            converter._json_to_cpp_spill_convert("", "")
        with self.assertRaises(ValueError):
            converter._json_to_cpp_spill_convert("",)
        ref_data = test_data()
        json_str = converter._cpp_to_json_spill_convert(ref_data)
        maus_data = converter._json_to_cpp_spill_convert(json_str,)
        self.assertEqual(maus_data.GetSpill().GetRunNumber(),
                         ref_data.GetSpill().GetRunNumber())


if __name__ == "__main__":
    unittest.main()

