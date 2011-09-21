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
#  ReducePyDoNothing string concatenates two spills
#
#  ReducePyDoNothing takes the string representation
#  of two JSON documents and concatenates them with
#  a line break between them.
import json
import types

class ReducePyDoNothing:
    def birth(self, configurationJson):
        return True

    def process(self, x, y):
        x = x.rstrip()
        y = y.rstrip()
        return "%s\n%s" % (x,y)

    def death(self):
        return True



