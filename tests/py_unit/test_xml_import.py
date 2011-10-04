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
Some evil is required to get libxml2 and libxslt to build correctly. Here we
check that the evil produced desired results.
"""

import unittest

class ImportXMLLibrariesTest(unittest.TestCase):
    """
    Test that we can import libxml2 and libxslt correctly
    """
    def test_import_libxml2(self): #pylint: disable=R0201
        """
        Test import of libxml2
        """
        import libxml2

    def test_import_libxslt(self): #pylint: disable=R0201
        """
        Test import of libxslt
        """
        import libxslt


