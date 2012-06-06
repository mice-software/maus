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
MapPyDoNothing

MapPyDoNothing does nothing except return input to output

Intended for testing.
"""

class MapPyDoNothing:
    """Test class for debugging"""

    def __init__(self):
        """Nothing to initialise"""
        pass

    def birth(self, input_json_config): # pylint: disable=W0613, R0201
        """Nothing to birth, just returns true"""
        return True

    def process(self, input_json_doc): # pylint: disable=R0201
        """Just return the input"""
        return input_json_doc

    def death(self): # pylint: disable=R0201
        """Just return true"""
        return True



