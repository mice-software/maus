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
#  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk> 
import json

class MapPyDoNothing:
    def Birth(self, argJsonConfigDocument):
        return True

    ## Process to pass input to output
    def Process(self, str):
        # check if valid document
        try:
            doc = json.loads(str)
        except ValueError:
            doc = {"errors": {"bad_json_document": "unable to do json.loads on input"} }

        return json.dumps(doc)

    def Death(self):
        return True



