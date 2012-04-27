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


## @class MapPyDoNothing.MapPyDoNothing
#  MapPyDoNothing does nothing except return input to output
#
#  Useful for debugging.
#
import json

class MapPyDoNothing:
    def birth(self, argJsonConfigDocument):
        return True

    ## process to pass input to output
    def process(self, input_json_doc):
        # check if valid document
        return input_json_doc

    def death(self):
        return True



