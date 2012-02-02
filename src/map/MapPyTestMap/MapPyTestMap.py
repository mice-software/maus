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
        self.__birth_result = True
        self.__process_result = True
        self.__death_result = True

    def birth(self, json_doc):
        """
        Birth the mapper. If the configuration doc has a key
        fail_birth then birth returns False. If it has a key
        fail_death then, when death is called, it will return
        False.
        @param self Object reference.
        @param json_doc JSON configuration document.
        @return False if fail_birth is in json_doc else return 
        True.
        """
        config = json.loads(json_doc)
        if (config.has_key("worker_key")):
            self.config_value = config["worker_key"]
        self.__death_result = not config.has_key("fail_death")
        self.birth_called = True
        return not config.has_key("fail_birth")

    def process(self, spill_doc): # pylint:disable = R0201
        """
        Process a spill append self.map_id to a "processed" field.
        If no "processed" field then create one.
        @param self Object reference.
        @param spill JSON spill document.
        @return updated spill document.
        """
        spill = json.loads(spill_doc)
        if (not spill.has_key("processed")):
            spill["processed"] = []
        spill["processed"].append(self.map_id)
        self.process_called = True
        return json.dumps(spill)

    def death(self):
        """
        Death the mapper.
        @param self Object reference.
        @return True unless birth was given a configuration that
        requested this return False.
        """
        self.death_called = True
        return self.__death_result
