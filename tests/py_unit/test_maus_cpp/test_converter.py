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

def make_test_data():
    """Generate a test MAUS.Data"""
    data = ROOT.MAUS.Data() # pylint: disable=E1101
    spill = ROOT.MAUS.Spill() # pylint: disable=E1101
    spill.SetRunNumber(99)
    spill.SetSpillNumber(999)
    data.SetSpill(spill)
    return data 

def make_test_job_header():
    """ Generate a test MAUS.JobHeader"""
    data = ROOT.MAUS.JobHeaderData() # pylint: disable=E1101
    job_header = ROOT.MAUS.JobHeader() # pylint: disable=E1101
    job_header.SetMausVersionNumber("version")
    data.SetJobHeader(job_header)
    return data 

def make_test_job_footer():
    """ Generate a test MAUS.JobFooter"""
    data = ROOT.MAUS.JobFooterData() # pylint: disable=E1101
    job_footer = ROOT.MAUS.JobFooter() # pylint: disable=E1101
    data.SetJobFooter(job_footer)
    return data 

def make_test_run_header():
    """ Generate a test MAUS.RunHeader"""
    data = ROOT.MAUS.RunHeaderData() # pylint: disable=E1101
    run_header = ROOT.MAUS.RunHeader() # pylint: disable=E1101
    data.SetRunHeader(run_header)
    return data 

def make_test_run_footer():
    """ Generate a test MAUS.RunFooter"""
    data = ROOT.MAUS.RunFooterData() # pylint: disable=E1101
    run_footer = ROOT.MAUS.RunFooter() # pylint: disable=E1101
    data.SetRunFooter(run_footer)
    return data 

class ConverterTestCase(unittest.TestCase): #pylint: disable=R0904
    """Tests for maus_cpp.converter"""
    def test_convert_bad(self):
        """Tests for maus_cpp.converter.X_repr() from bad"""
        self.assertRaises(ValueError, converter.json_repr, ())
        self.assertRaises(ValueError, converter.json_repr, ("",))
        dummy_tree = ROOT.TTree() # pylint: disable=E1101
        self.assertRaises(ValueError, converter.json_repr, dummy_tree)

    def test_convert_data(self):
        """Tests for maus_cpp.converter.X_repr() to/from MAUS.Data"""
        maus_data = make_test_data()
        self.assertRaises(ValueError, converter.json_repr, (maus_data, ""))
        json_dict = converter.json_repr(maus_data)
        json_dict = converter.json_repr(maus_data)
        converter.json_repr(json_dict)
        converter.json_repr(json_dict)
        self.assertEqual(json_dict["spill_number"], 999)
        converter.data_repr(json_dict)
        converter.string_repr(maus_data)

    def test_convert_job_header(self):
        """Tests for maus_cpp.converter.X_repr() from MAUS.JobHeader"""
        maus_jh = make_test_job_header()
        self.assertRaises(ValueError, converter.data_repr, (maus_jh,))
        json_dict = converter.json_repr(maus_jh)
        self.assertEqual(json_dict["maus_version"], "version")
        converter.job_header_repr(json_dict)
        converter.string_repr(maus_jh)

    def test_convert_job_footer(self):
        """Tests for maus_cpp.converter.X_repr() from MAUS.JobFooter"""
        maus_jf = make_test_job_footer()
        self.assertRaises(ValueError, converter.data_repr, (maus_jf,))
        json_dict = converter.json_repr(maus_jf)
        self.assertEqual(json_dict["maus_event_type"], "JobFooter")
        converter.job_footer_repr(json_dict)
        converter.string_repr(maus_jf)

    def test_convert_run_header(self):
        """Tests for maus_cpp.converter.X_repr() from MAUS.RunHeader"""
        maus_rh = make_test_run_header()
        self.assertRaises(ValueError, converter.data_repr, (maus_rh,))
        json_dict = converter.json_repr(maus_rh)
        self.assertEqual(json_dict["maus_event_type"], "RunHeader")
        converter.run_header_repr(json_dict)
        converter.string_repr(maus_rh)

    def test_convert_run_footer(self):
        """Tests for maus_cpp.converter.X_repr() from MAUS.RunFooter"""
        maus_rf = make_test_run_footer()
        self.assertRaises(ValueError, converter.data_repr, (maus_rf,))
        json_dict = converter.json_repr(maus_rf)
        self.assertEqual(json_dict["maus_event_type"], "RunFooter")
        converter.run_footer_repr(json_dict)
        converter.string_repr(maus_rf)

    def test_data(self): # pylint: disable = R0201
        """Regression - memory issue in datastructure #1466"""
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        _filename = root_dir+ \
                   '/tests/py_unit/test_maus_cpp/test_converter_regression.json'
        _file = open(_filename, 'r')
        output_3 = converter.data_repr(_file.read())
        for i in range(2): # pylint: disable = W0612
            converter.data_repr(output_3)

    def test_del_data(self):
        """Check that we can delete MAUS data"""
        try:
            converter.del_data_repr("", "")
            self.assertTrue(False, "Should have thrown")
        except TypeError:
            pass
        try:
            converter.del_data_repr("")
            self.assertTrue(False, "Should have thrown")
        except TypeError:
            pass
        maus_data = make_test_data()
        converter.del_data_repr(maus_data)

if __name__ == "__main__":
    unittest.main()

