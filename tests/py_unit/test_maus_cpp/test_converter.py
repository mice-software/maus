#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

"""
Tests for maus_cpp.converter
"""

import unittest
import os

import ROOT
import libMausCpp # pylint: disable=W0611

from maus_cpp import converter

def test_data():
    """Generate a test MAUS.Data"""
    data = ROOT.MAUS.Data() # pylint: disable=E1101
    spill = ROOT.MAUS.Spill() # pylint: disable=E1101
    spill.SetRunNumber(99)
    spill.SetSpillNumber(999)
    data.SetSpill(spill)
    return data 

class ConverterTestCase(unittest.TestCase): #pylint: disable=R0904
    """Tests for maus_cpp.converter"""
    def _test_convert(self):
        """Tests for maus_cpp.converter.X_repr()"""
        self.assertRaises(ValueError, converter.json_repr, ())
        self.assertRaises(ValueError, converter.json_repr, ("",))
        dummy_tree = ROOT.TTree() # pylint: disable=E1101
        self.assertRaises(ValueError, converter.json_repr, dummy_tree)
        maus_data = test_data()
        self.assertRaises(ValueError, converter.json_repr, (maus_data, ""))
        json_dict = converter.json_repr(maus_data)
        json_dict = converter.json_repr(maus_data)
        converter.json_repr(json_dict)
        converter.json_repr(json_dict)
        self.assertEqual(json_dict["spill_number"], 999)
        converter.data_repr(maus_data)
        converter.string_repr(maus_data)

    def test_data(self):
        """Regression - memory issue in datastructure #1466"""
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        _filename = root_dir+ \
                   '/tests/py_unit/test_maus_cpp/test_converter_regression.json'
        _file = open(_filename, 'r')
        output_3 = converter.data_repr(_file.read())
        for i in range(2):
            print output_3.GetSpill()
            print output_3.GetSpill().GetMCEvents(), output_3.GetSpill().GetMCEvents().size()
            for event in output_3.GetSpill().GetMCEvents():
                print event
            spill_out = converter.data_repr(output_3)

if __name__ == "__main__":
    unittest.main()

