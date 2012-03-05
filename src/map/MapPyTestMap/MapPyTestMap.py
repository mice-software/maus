"""
Map for use in testing other components.
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

# pylint: disable=C0103

import json

class MapPyTestMap: # pylint:disable = R0902
    """
    Simple map class for testing purposes.
    """
    OK = 0
    FAIL = 1
    EXCEPTION = 2

    def __init__(self, map_id = ""):
        """
        Constructor.
        @param self Object reference.
        @param map_id ID of this class.
        """
        self.map_id = map_id
        # Configuration value.
        self.config_value = ""
        # Flags to indicate if these functions were called.
        self.birth_called = False
        self.process_called = False
        self.death_called = False
        # Flags holding result of these function calls.
        self.__process_result = MapPyTestMap.OK
        self.__death_result = MapPyTestMap.OK

    def birth(self, json_doc):
        """
        Birth the mapper. If the configuration doc has a key
        birth_result of value FAIL then birth returns false. If the
        value is EXCEPTION then a ValueError is thrown. 
        @param self Object reference.
        @param json_doc JSON configuration document.
        @return False if the configuration document has a
        birth_result key with value FAIL, else return True.
        @raise ValueError if the configuration document has a
        birth_result key with value EXCEPTION 
        """
        config = json.loads(json_doc)
        self.birth_called = True
        if (config.has_key("birth_result")):
            if (config["birth_result"] == MapPyTestMap.FAIL):
                return False
            if (config["birth_result"] == MapPyTestMap.EXCEPTION):
                raise ValueError("Birth exception")
        if (config.has_key("worker_key")):
            self.config_value = config["worker_key"]
        if (config.has_key("process_result")):
            self.__process_result = config["process_result"]
        if (config.has_key("death_result")):
            self.__death_result = config["death_result"]
        return True

    def process(self, spill_doc): # pylint:disable = R0201
        """
        Process a spill. If the configuration document provided
        to birth had a process_result key with value EXCEPTION
        then a ValueError is raised. Else, process appends self.map_id
        to a "processed" field. If no "processed" field is in the
        spill then one is created.
        @param self Object reference.
        @param spill JSON spill document.
        @return updated spill document.
        @raise ValueError if the configuration document provided
        to birth had a process_result key with value EXCEPTION
        """
        self.process_called = True
        if (self.__process_result == MapPyTestMap.EXCEPTION):
            raise ValueError("Process exception")
        spill = json.loads(spill_doc)
        if (not spill.has_key("processed")):
            spill["processed"] = []
        spill["processed"].append(self.map_id)
        return json.dumps(spill)

    def death(self):
        """
        Death the mapper.
        @param self Object reference.
        @return False if the configuration document provided to birth
        had a death_result key with value FAIL, else return True.
        @raise ValueError if the configuration document provided to
        birth had a death_result key with value EXCEPTION 
        """
        self.death_called = True
        if (self.__death_result == MapPyTestMap.FAIL):
            return False
        if (self.__death_result == MapPyTestMap.EXCEPTION):
            raise ValueError("Death exception")
        return True
