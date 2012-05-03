# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""
Does some global data structure set up
"""

import json
import ErrorHandler

class MapPyReconSetup:
    """
    Creates a recon_events branch containing a list of branches one for each
    detector
    """
    def __init__(self):
        """
        Does nothing
        """
        pass

    def birth(self, json_configuration):
        """
        Returns true
        """
        return True

    def process(self, json_document):
        """
        Set up recon_events branch

        Loop over the V1290 in the trigger. Extract the part_event_number and for
        """
        try:
            spill = json.loads(json_document)
        except ValueError:
            spill = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return json.dumps(spill)

        spill["recon_events"] = []

        # check data integrity
        if "daq_data" not in spill or type(spill["daq_data"]) != type({}):
            return json.dumps(spill)
        daq_data = spill["daq_data"]
        if "trigger" not in daq_data or \
            type(daq_data["trigger"]) != type([]):
            return json.dumps(spill)

        # list of events
        ev_number_list = [trig["V1290"][0]["part_event_number"] \
                                                for trig in daq_data["trigger"]]
        # now make events
        for event_number in ev_number_list:
            spill["recon_events"].append(
                                         {"part_event_number":event_number,
                                          "trigger_event":{},
                                          "tof_event":{},
                                          "sci_fi_event":{},
                                          "ckov_event":{},
                                          "emr_event":{},
                                          "kl_event":{},
                                          "global_event":{},
            })

        return json.dumps(spill)

    def death(self):
        return True

