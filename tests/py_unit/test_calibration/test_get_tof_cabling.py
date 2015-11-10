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

"""Tests for src/common_py/calibration/get_tof_cabling.py"""
import json
import unittest
# pylint: disable = E0611, F0401, E1101
import calibration.get_tof_cabling
from Configuration import Configuration

class TestGetCabling(unittest.TestCase): #pylint: disable = R0904
    """test get_tof_cabling"""

    def test_init(self):
        """Test class initializes"""

    def test_get_cabling(self):
        """Test we can download cabling for specified detector"""
	# load the configuration and get the tof cabling date parameter
        config = Configuration()
        lconfig = json.loads(config.getConfigJSON())
        cablingdate = lconfig['TOF_calib_date_from']
        # get cabling for default config date
        gtc_ = calibration.get_tof_cabling.GetCabling()

        # check we can get cabling for valid detector
        dev = "TOF1"
        gtc_.get_cabling(dev, cablingdate)

        # check we can get cabling for valid det and default date
        gtc_.get_cabling(dev, cablingdate)

        # check we can get cabling for valid det and specified date
        cablingdate = '2012-05-27 12:00:00.0'
        gtc_.get_cabling(dev, cablingdate)

        # check we can get cabling for valid det and valid run
        runnumber = 7417
        gtc_.get_cabling_for_run(dev, runnumber)

        # check we can get cabling for valid det and run=0
        runnumber = 0
        gtc_.get_cabling_for_run(dev, runnumber)

        # check we raise exception for valid det and invalid run
        runnumber = 3
        self.assertRaises(Exception, gtc_.get_cabling_for_run, dev, runnumber)

        # check we error for invalid detector
        dev = "Junk"
        self.assertRaises(Exception, gtc_.get_cabling, dev, cablingdate)

        # check we error in case of invalid args
        dev = "junk"
        self.assertRaises(Exception, gtc_.get_cabling, dev, cablingdate)

        # check we error in case of no args
        dev = ""
        self.assertRaises(Exception, gtc_.get_cabling, dev, cablingdate)

    def test_reset(self):
        """Test we can reset"""
        gtc_ = calibration.get_tof_cabling.GetCabling()
        gtc_.reset()
        self.assertRaises(Exception, gtc_.get_cabling, "", "", "")

if __name__ == '__main__':
    unittest.main()

