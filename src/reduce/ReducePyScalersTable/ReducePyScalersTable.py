"""
ReducePyScalersTable calculates averages of various information
including triggers, trigger requests, GVA, TOF0, TOF1 and clock,
updating these averages as subsequent spills are processed.
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

import json
import ErrorHandler
from datetime import datetime

class Scaler:
    """
    @class ReducePyScalersTable.Scaler maintains the total sum
    of values for a scaler as well as the last N values.
    """
    def __init__(self, window = 11):
        """
        Set initial attribute values.
        @param self Object reference.
        @param window Window size determining last N values to
        hold.
        """
        self._total  = 0  # Total to date.
        self._count  = 0  # Count of values added to total.
        self._window = window 
        self._recent = [] # list of last _window values.
 
    def add_value(self, value):
        """
        Add the value to total and also the most recent N values.
        If the set of recent values exceeds N then pop off the first
        one.
        @param self Object reference.
        @param value
        """
        self._total += value
        self._count += 1
        self._recent.append(value)
        if len(self._recent) == self._window:
            self._recent.pop(0)
 
    def get_count(self):
        """
        Get number of values added to date.
        @param self Object reference.
        @return count
        """
        return self._count

    def get_recent_window(self):
        """
        Get number of recent values that will be held.
        @param self Object reference.
        @return window
        """
        return self._window

    def get_average(self):
        """
        Get the average of all the values to date.
        @param self Object reference.
        @return average or 0 if no values have been added to date.
        """
        if (self._count > 0):
            return self._total / self._count
        else:
            return 0

    def get_recent_average(self):
        """
        Add the value to total and also the most recent N values.
        If the set of recent values exceeds N then pop off the first
        one.
        @param self Object reference.
        @return average or 0 if no values have been added to date.
        """
        if len(self._recent):
            return sum(self._recent) / len(self._recent)
        else:
            return 0

    def get_recent_value(self):
        """
        Get the last value added to the most recent N values.
        @param self Object reference.
        @return average or 0 if no values have been added to date.
        """
        if len(self._recent):
            return self._recent[-1]
        else:
            return 0

    def clear(self):
        """
        Set the total and count to 0 and empty the list of
        recent values.
        @param self Object reference.
        """
        self._total  = 0
        self._count  = 0
        self._recent = []

class ReducePyScalersTable: # pylint: disable=R0902
    """
    @class ReducePyScalersTable.ReducePyScalersTable calculates
    averages of various information including triggers, trigger
    requests, GVA, TOF0, TOF1 and clock, updating these averages as
    subsequent spills are processed:
    @verbatim
    ch0 - triggers
    ch1 - trigger requests
    ch2 - GVA
    ch3 - TOF0
    ch4 - TOF1
    ch12- 10 MHz clock
    @endverbatim
    This can be extended to include other channels of the scaler.

    Data is output as JSON documents of general form:

    @verbatim
    {"table": {"keywords": [...list of data keywords...],
               "description": "...a description of the data...",
               "data":[["...average name...", 
                        LAST_READ_VALUE,
                        AVERAGE_OF_LAST_10_VALUES,
                        AVERAGE_OVER_RUN],
                       [...,...,...,...],...]}}  
    @endverbatim

    In cases where a spill is input that contains errors (e.g. is
    badly formatted or is missing the data needed to update the
    averages) then a spill is output which is just the input spill 
    with an "errors" field containing the error e.g.

    @verbatim
    {"errors": {..., "bad_json_document": "unable to do json.loads on input"}}
    @endverbatim
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        # Channel IDs.
        self._channels = ["ch0", "ch1", "ch2", "ch3", "ch4", "ch12"]

        # Channel ID, scaler name, counts.
        self._scalers = []
        self._scalers.append(("ch0", "Triggers", Scaler()))
        self._scalers.append(("ch1", "Trigger Requests", Scaler()))
        self._scalers.append(("ch2", "GVA", Scaler()))
        self._scalers.append(("ch3", "TOF0", Scaler()))
        self._scalers.append(("ch4", "TOF1", Scaler()))
        self._scalers.append(("ch12", "10 MHz clock", Scaler()))
        self._event            = ""
        self._time             = None
        # Has an end_of_run been processed?
        self._run_ended        = False

    def birth(self, config_json): # pylint: disable=W0613
        """
        Configure worker from data cards. This resets the data in the
        object. The data cards are unused.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True if configuration succeeded. 
        """
        self._reset_data()
        return True

    def process(self, json_spill_doc):
        """
        Update the averages with data from the current spill
        and output a table with the new averages.
        @param self Object reference.
        @param json_spill_doc String with current JSON document.
        @returns JSON document containing the table of new 
        averages.
        """
        try:
            json_doc = json.loads(json_spill_doc.rstrip())
        except Exception: # pylint:disable=W0703
            json_doc = {}
            ErrorHandler.HandleException(json_doc, self)
            return unicode(json.dumps(json_doc))

        try:
            result = self._process_spill(json_doc)
        except Exception: # pylint:disable=W0703
            ErrorHandler.HandleException(json_doc, self)
            return unicode(json.dumps(json_doc))

        # Convert result to string.
        doc_list = [json.dumps(result), "\n"]
        return unicode("".join(doc_list))

    def death(self): #pylint: disable=R0201
        """
        Reset the data used to calculate the averages.
        @param self Object reference.
        @return True
        """
        self._reset_data()
        return True

    def _reset_data(self):
        """
        Reset data.
        @param self Object reference.
        """
        for (_, _, scaler) in self._scalers:
            scaler.clear()
        self._event     = ""
        self._time      = None
        self._run_ended = False

    def _process_spill(self, spill):
        """
        Check that the spill has the data necessary to update the
        averages then creates JSON documents in the format described 
        above. 
        @param self Object reference.
        @param spill Current spill.
        @returns JSON document.
        @throws KeyError if "daq_data" or "V830" or "channels" or
        "ch0" is missing from the spill.
        @throws ValueError if "daq_data" value is "None".
        """
        if (spill.has_key("daq_event_type") and
            spill["daq_event_type"] == "end_of_run"):
            if (not self._run_ended):
                self._run_ended = True
                return self._create_output()
            else:
                return {}

        if "daq_data" not in spill:
            raise KeyError("daq_data is not in spill")
        daq_data = spill["daq_data"]

        if daq_data is None:
            raise ValueError("daq_data is None")

        if "V830" not in daq_data:
            raise KeyError("V830 is not in spill")
        scalers = daq_data["V830"]

        if "channels" not in scalers:
            raise KeyError("channels is not in spill")

        hits = scalers["channels"]
        # Update is only done if hits["ch0"] exists.
        if  "ch0" not in hits:
            raise KeyError("ch0 is not in spill")

        event = scalers["phys_event_number"]
        time = scalers["time_stamp"]
        self._update_data(hits, event, time)

        return self._create_output()

    def _update_data(self, hits, event, time):
        """
        Update data used to calculate averages with information
        from the current spill.
        @param self Object reference.
        @param hits Data from current spill.        
        @param event Physical event number.
        @param time Time stamp.
        """
        self._event = event
        self._time = time
        for (channel, _, scaler) in self._scalers:
            scaler.add_value(hits[channel])

    def _create_output(self):
        """
        Create JSON @table@ document for output.
        @param self Object reference.
        @return JSON document.
        """
        table = {}
        table["table"] = {}
        content = table["table"]
        content["keywords"] = ["Scalers"]
        if (self._time != None):
            time_str = str(datetime.fromtimestamp(self._time))
        else:
            time_str = ""
        description = "Scaler counts from channel data for event: ", \
            self._event, " at time: ", time_str
        content["description"] = "".join(description)
        rows = []
        for (_, name, scaler) in self._scalers:
            rows.append([name, 
                scaler.get_recent_value(),
                scaler.get_recent_average()])
        content["data"] = rows
        return table
