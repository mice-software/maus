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

"""
Tests that the configuration database is available
"""

import unittest

import cdb

READ_URL_LIST = ["http://cdb.mice.rl.ac.uk", "http://preprodcdb.mice.rl.ac.uk"]
WRITE_URL_LIST = ["http://preprodcdb.mice.rl.ac.uk"]

class TestCdb(unittest.TestCase): #pylint: disable = R0904
    """
    Test that the cdb specified in URL is available
    """

    @classmethod
    def test_read_access(cls):
        """Test that we can access the CDB with read access"""
        # throw exception if not available
        for url in READ_URL_LIST:
            cdb.AlarmHandler(url)
            cdb.Beamline(url)
            cdb.Cabling(url)
            cdb.Calibration(url)
            cdb.Geometry(url)
            cdb.PIDCtrl(url)
            cdb.StateMachine(url)
            cdb.Target(url)

    @classmethod
    def test_write_access(cls):
        """Test that we can access the CDB with write access"""
        # throw CDBTempoaryError exception if not available
        for url in WRITE_URL_LIST:
            cdb.AlarmHandlerSuperMouse(url)
            cdb.BeamlineSuperMouse(url)
            cdb.CablingSuperMouse(url)
            cdb.CalibrationSuperMouse(url)
            cdb.GeometrySuperMouse(url)
            cdb.PIDCtrlSuperMouse(url)
            cdb.StateMachineSuperMouse(url)
            cdb.TargetSuperMouse(url)

if __name__ == "__main__":
    unittest.main()

