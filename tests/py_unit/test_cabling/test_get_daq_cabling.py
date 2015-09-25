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

"""Tests for src/common_py/cabling/get_daq_cabling.py"""
import json
import unittest
# pylint: disable = E0611, F0401, E1101, C0103
import cabling.get_daq_cabling as gdc_
from Configuration import Configuration

class TestGetCabling(unittest.TestCase): #pylint: disable = R0904
    """test get_daq_cabling"""

    def test_init(self):
        """Test class initializes"""

    def test_get_cabling(self): #pylint: disable = R0201
        """Test we can download cabling for proper device, date, run"""
	# load the configuration and get the daq cabling date parameter
        config = Configuration()
        lconfig = json.loads(config.getConfigJSON())
        cablingdate = lconfig['DAQ_cabling_date']
        # get cabling for default config date

        # check we can get cabling for valid detector
        dev = "DAQ"
        gdc_.GetCabling().get_cabling_for_date(dev, cablingdate)

        # check we can get cabling for valid det and specified date
        cablingdate = '2012-05-27 12:00:00.0'
        gdc_.GetCabling().get_cabling_for_date(dev, cablingdate)

    def test_by_run(self): #pylint: disable = R0201
        """Test we can download cabling for valid run"""
        dev = "DAQ"
        run = 7273
        gdc_.GetCabling().get_cabling_for_run(dev, run)

    def test_invalid_device(self): #pylint: disable = R0201
        """Test failure for invalid device name"""
        dev = "Junk"
        cablingdate = '2012-05-27 12:00:00.0'
        run = 7066
        self.assertRaises(Exception, \
          gdc_.GetCabling().get_cabling_for_date, dev, cablingdate)
        self.assertRaises(Exception, \
          gdc_.GetCabling().get_cabling_for_run, dev, run)

    def test_invalid_date(self): #pylint: disable = R0201
        """Test failure for invalid date"""
        # check we error for invalid detector
        dev = "DAQ"
        cablingdate = '2014'
        res = gdc_.GetCabling().get_cabling_for_date(dev, cablingdate)
        self.assertEqual(res, 'cdb_permanent_error')

    def test_no_args(self): #pylint: disable = R0201
        """Test failure for empty arguments"""
        # check we error in case of no args
        dev = ""
        cablingdate = ''
        run = ''
        self.assertRaises(Exception, \
          gdc_.GetCabling().get_cabling_for_date, dev, cablingdate)
        self.assertRaises(Exception, \
          gdc_.GetCabling().get_cabling_for_run, dev, run)

    def test_invalid_run(self): #pylint: disable = R0201
        """Test failure for invalid run number"""
        dev = "DAQ"
        run = 1
        self.assertRaises(Exception, \
          gdc_.GetCabling().get_cabling_for_run, dev, run)
        run = ""
        self.assertRaises(Exception, \
          gdc_.GetCabling().get_cabling_for_run, dev, run)

    def test_reset(self): #pylint: disable = R0201
        """Test we can reset"""
        gdc_.GetCabling().reset()

if __name__ == '__main__':
    unittest.main()

