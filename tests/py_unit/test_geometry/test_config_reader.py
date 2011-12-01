"""
M. Littlefield
"""
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

import unittest
from geometry.ConfigReader import Configreader

class test_config_reader(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    This class tests the module ConfigReader.py
    which reads the configuration defaults and makes
    the info available for the geometry files. This test
    ensures it is doing what it is expected to.
    """
    def test_readconfig(self): #pylint: disable  = R0201
        """
        test_config_reader::test_readconfig

        Just check we can instantiate the config reader
        """
        Configreader()

if __name__ == '__main__':
    unittest.main()
