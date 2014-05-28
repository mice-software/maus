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
    def __init__(self, recent_window = 10):
        """
        Set initial attribute values.
        @param self Object reference.
        @param recent_window Window size determining last N values to
        hold.
        """
        self._total = 0  # Total to date.
        self._count = 0  # Count of values added to total.
        self._recent_window = recent_window 
        self._recent = [] # list of last _recent_window values.
 
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
        if len(self._recent) > self._recent_window:
            self._recent.pop(0)
 
    def get_count(self):
        """
        Get number of values added to date.
        @param self Object reference.
        @return count
        """
        return self._count

    def set_recent_window(self, recent_window):
        """
        Set number of recent values that will be held.
        @param self Object reference.
        @param recent_window Recent window size.
        """
        self._recent_window = recent_window

    def get_recent_window(self):
        """
        Get number of recent values that will be held.
        @param self Object reference.
        @return window
        """
        return self._recent_window

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
    {"keywords": [...list of data keywords...],
     "description": "...a description of the data...",
     "table_headings": ["Scaler", "Last read value", 
         "Average of last 10 values", "Average over run"],
     "table_data":[["...average name...", 
                    LAST_READ_VALUE,
                    AVERAGE_OF_MOST_RECENT_VALUES,
                    AVERAGE_OVER_RUN],...]}
    @endverbatim

    The caller can configure the worker and specify:

    -Recent scalers window ("recent_scalers_window"). Default: 10. 
     Number of most recent values for which MOST_RECENT_AVERAGE above
     is calculated. 

    In cases where a spill is input that does not contain information
    needed to calculate the scaler values then a spill, as above,
    is output with the last good values, plus an "errors" field
    describing the missing information e.g.
    @verbatim
    {"errors": {..., "bad_json_document": "unable to do json.loads on input"}}
    {"errors": {..., "...": "..."}}
    @endverbatim
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        self._headings = ["Scaler", 
            "Last read value", 
            "Average of last N values", 
            "Average over run"]
        # Channel IDs.
        self._channels = ["ch0", "ch1", "ch2", "ch3", "ch4", "ch12"]
        # Recent window size.
        self._recent_window = 10
        # Channel ID, scaler name, counts.
        self._scalers = []
        self._scalers.append(("ch0", "Triggers", 
            Scaler(self._recent_window)))
        self._scalers.append(("ch1", "Trigger Requests", 
            Scaler(self._recent_window)))
        self._scalers.append(("ch2", "GVA", Scaler(self._recent_window)))
        self._scalers.append(("ch3", "TOF0", Scaler(self._recent_window)))
        self._scalers.append(("ch4", "TOF1", Scaler(self._recent_window)))
        self._scalers.append(("ch12", "10 MHz clock",
            Scaler(self._recent_window)))
        self._event = ""
        self._time = None
        # Has an end_of_run been processed?
        self._run_ended = False

    def birth(self, config_json): # pylint: disable=W0613
        """
        Configure worker from data cards. This resets the data in the
        object. The data cards are unused.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True if configuration succeeded. 
        """
        config_doc = json.loads(config_json)
        if 'recent_scalers_window' in config_doc:
            self._recent_window = int(config_doc["recent_scalers_window"])
        self._reset_data()
        return True

    def process(self, json_spill_doc):
        """
        Update the averages with data from the current spill
        and output a table with the new averages. If the spill
        does not contain the expected data then the last good
        averages are output (if the ErrorHandler is configured
        not to raise exceptions).
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
            result = self._create_output()
            ErrorHandler.HandleException(result, self)
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
            scaler.set_recent_window(self._recent_window)
        self._event     = ""
        self._time      = None
        self._run_ended = False
        self._headings[2] = \
            "Average of last %d values" % self._recent_window

    def _process_spill(self, spill): #pylint: disable=R0912
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

        if "maus_event_type" not in spill:
            out = self._create_output()
            if "errors" not in out:
                out["errors"] = {}
            out["errors"]["ReducePyScalersTable"] = \
                                        "Bad input spill - no maus_event_type"
            return out
        elif spill["maus_event_type"] != "Spill": 
            return self._create_output()

        if "daq_event_type" not in spill or \
           spill["daq_event_type"] != "physics_event":
            return self._create_output()

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

        # Update
        self._event = scalers["phys_event_number"]
        self._time = scalers["time_stamp"]
        for (channel, _, scaler) in self._scalers:
            scaler.add_value(hits[channel])
        return self._create_output()

    def _create_output(self):
        """
        Create JSON document for output.
        @param self Object reference.
        @return JSON document.
        """
        table = {}
        table["keywords"] = ["Scalers"]
        table["table_headings"] = self._headings
        if (self._time != None):
            time_str = str(datetime.fromtimestamp(self._time))
        else:
            time_str = ""
        description = \
            "Scaler counts from channel data for event: %s at time: %s" \
            % (self._event, time_str)
        table["description"] = description
        rows = []
        for (_, name, scaler) in self._scalers:
            rows.append([name, 
                scaler.get_recent_value(),
                scaler.get_recent_average(),
                scaler.get_average()])
        table["table_data"] = rows
        return table

    def get_scalers(self):
        """
        Get scaler counts.
        @param self Object reference.
        @return ordered list of scaler counts in form 
        (CHANNEL_NAME, SCALER_NAME, SCALER).
        """
        return self._scalers
 
