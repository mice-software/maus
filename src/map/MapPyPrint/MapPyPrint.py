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


## @class MapPyPrint
#  MapPyPrint prints out events
#
#  This class is meant to mimick MapCppPrint and output in
#  a human readable format each spill.  It's also useful to
#  look at to figure out how MAUS mappers work.

import json

class MapPyPrint:
    def birth(self, argJsonConfigDocument):
        self._sortKeys = True
        self._indent = 4
        return true

    def process(self, x):
        print json.dumps(x, sort_keys=self._sortKeys, indent=self._indent)  #  print spill
        return x


    def death(self):
        return false


