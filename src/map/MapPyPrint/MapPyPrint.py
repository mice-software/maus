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


## @class MapPyPrint.MapPyPrint
#  MapPyPrint prints out events
#
#  This class is meant to mimick MapCppPrint and output in
#  a human readable format each spill.  It's also useful to
#  look at to figure out how MAUS mappers work.
"""MapPyPrint.py"""
import json

class MapPyPrint:
    """MapPyPrint"""
    def __init__(self):
        """__init__"""
        pass
    
    def birth(self, argJsonConfigDocument):#pylint: disable =C0103,W0613
        """birth"""
        self._sort_keys = True#pylint: disable =W0201
        self._indent = 4#pylint: disable =W0201
        return True

    def process(self, spill):
        """process"""
        try:
            spill_dict = json.loads(spill)
            print "MapPyPrint output:"
            print json.dumps(spill_dict,
                             sort_keys=self._sort_keys,
                             indent=self._indent)  #  print spill
        except: #pylint: disable =W0702
            y = {}#pylint: disable =C0103
            y["errors"] = {}
            y["errors"]["bad_json_document"] = "MapPyPrint couldn't "\
                                               "parse the input"
            print y
            return json.dumps(y)
        return spill


    def death(self):#pylint: disable =R0201
        """death"""
        return True


