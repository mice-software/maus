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
Run xboa tests as part of MAUS integration tests
"""

import unittest
import ROOT
import xboa.test.XBOATest

class TestXBOA(unittest.TestCase): # pylint: disable=R0904
    """
    Run the tests and check they return 0
    """

    def test_xboa(self):
        """
        Run the xboa test
        """
        ROOT.gROOT.SetBatch(True) #pylint: disable=E1101
        passes, fails, warns = xboa.test.XBOATest.test_all()
        self.assertEqual(fails, 0, msg = str((passes, fails, warns)))
        ROOT.gROOT.SetBatch(False) #pylint: disable=E1101

if __name__ == "__main__":
    unittest.main()

