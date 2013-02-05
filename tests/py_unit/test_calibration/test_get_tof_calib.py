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

"""Tests for src/common_py/get_tof_calib.py"""
import json
import unittest
# pylint: disable = E0611, F0401, E1101
import calibration.get_tof_calib
from Configuration import Configuration

class TestGetCalib(unittest.TestCase): #pylint: disable = R0904
    """test get_tof_calib"""

    def test_init(self):
        """Test class initializes"""

    def test_get_calib(self):
        """Test we can download calib for specified detector"""
	# load the configuration and get the tof calib date parameter
        config = Configuration()
        lconfig = json.loads(config.getConfigJSON())
        calibdate = lconfig['TOF_calib_date_from']
        #print 'calibdate= ', calibdate
        # get calibration for default config date
        gtc_ = calibration.get_tof_calib.GetCalib()

        # check we can get calib for valid detector
        dev = "TOF1"
        ctype = "t0"
        gtc_.get_calib(dev, ctype, calibdate)

        # check we can get calib for valid det and default date
        gtc_.get_calib(dev, ctype, calibdate)

        # check we can get calib for valid det and specified date
        calibdate = '2012-05-27 12:00:00.0'
        gtc_.get_calib(dev, ctype, calibdate)

        # check we error for invalid detector
        dev = "Junk"
        self.assertRaises(Exception, gtc_.get_calib, dev, ctype, calibdate)

        # check we error for invalid calib type
        dev = "TOF1"
        ctype = "junk"
        self.assertRaises(Exception, gtc_.get_calib, dev, ctype, calibdate)

        # check we error in case of invalid args
        dev = "junk"
        ctype = "junk"
        self.assertRaises(Exception, gtc_.get_calib, dev, ctype, calibdate)

        # check we error in case of no args
        dev = ""
        ctype = ""
        self.assertRaises(Exception, gtc_.get_calib, dev, ctype, calibdate)

    def test_reset(self):
        """Test we can reset"""
        gtc_ = calibration.get_tof_calib.GetCalib()
        gtc_.reset()
        self.assertRaises(Exception, gtc_.get_calib, "", "", "")

if __name__ == '__main__':
    unittest.main()

