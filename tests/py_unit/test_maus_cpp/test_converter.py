import unittest

import ROOT

import libMausCpp
from maus_cpp import converter

def test_data():
    data = ROOT.MAUS.Data()
    spill = ROOT.MAUS.Spill()
    spill.SetRunNumber(99)
    spill.SetSpillNumber(999)
    data.SetSpill(spill)
    return data 

class ConverterTestCase(unittest.TestCase):
    def test_convert(self):
        self.assertRaises(ValueError, converter.json_repr, ())
        self.assertRaises(ValueError, converter.json_repr, ("",))
        dummy_tree = ROOT.TTree()
        self.assertRaises(ValueError,converter.json_repr, dummy_tree)
        maus_data = test_data()
        self.assertRaises(ValueError, converter.json_repr, (maus_data, ""))
        json_dict = converter.json_repr(maus_data)
        self.assertEqual(json_dict["spill_number"], 999)
        converter.data_repr(maus_data)
        converter.string_repr(maus_data)

if __name__ == "__main__":
    unittest.main()

