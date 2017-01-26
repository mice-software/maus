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


## @class ReducePyDoNothing.ReducePyDoNothing
#  ReducePyDoNothing returns the input
"""ReducePyDoNothing.py"""
# import json
# import types
import copy

class ReducePyDoNothing:
    """ReducePyDoNothing"""
    def __init__(self):
        """__init__"""
        pass
    
    def birth(self, configuration_json):#pylint: disable =R0201,W0613
        """birth"""
        return True

    def process(self, spill):#pylint: disable =R0201
        """process"""
        new_spill = copy.deepcopy(spill)
        return new_spill

    def death(self):#pylint: disable =R0201
        """death"""
        return True



