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
Generate empty spills
"""

import json
import ErrorHandler

class InputPySpillGenerator:
    """
    Generates a user defined number of empty spills, defined by the
    spill_generator_number_of_spills. Empty spill is just an empty dict.
    """

    def __init__(self):
        """
        Initialise InputPySpillGenerator to 0
        """
        self._number_of_events = 0
        self._current_event = 0

    def birth(self, json_config):
        """
        birth() reads the number of spills from the configuration
        """
        try:
            config_doc = json.loads(json_config)
            self._number_of_events = \
                            int(config_doc["spill_generator_number_of_spills"])
            assert self._number_of_events > -1
            return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False

    def emitter(self):
        """
        Emit JSON documents

        This is a python generator that yields
        JSON documents until the end of the file
        or until the maximum number of events is hit.
        """
        while self._current_event < self._number_of_events:
            self._current_event += 1
            next_value = unicode("{}")
            # yield the current event (google 'python generators' if confused)
            yield next_value
        return

    def death(self):
        """
        death() resets the event counter to 0
        """
        self._current_event = 0
        return True

