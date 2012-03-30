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
               "data":[[...row...],[...row...],...]}}
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
        self._triggers         = []
        self._trigger_requests = []
        self._gva              = []
        self._tof0             = []
        self._tof1             = []
        self._clock            = []
        self._event            = ""
        self._time             = None
        self._window           = 11
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
        self._triggers         = []
        self._trigger_requests = []
        self._gva              = []
        self._tof0             = []
        self._tof1             = []
        self._clock            = []
        self._event            = ""
        self._time             = None
        self._run_ended        = False

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

        self._triggers.append(hits["ch0"])
        if len(self._triggers) == self._window:
            self._triggers.pop(0)

        self._trigger_requests.append(hits["ch1"])
        if len(self._trigger_requests) == self._window:
            self._trigger_requests.pop(0)

        self._gva.append(hits["ch2"])
        if len(self._gva) == self._window:
            self._gva.pop(0)

        self._tof0.append(hits["ch3"])
        if len(self._tof0) == self._window:
            self._tof0.pop(0)

        self._tof1.append(hits["ch4"])
        if len(self._tof1) == self._window:
            self._tof1.pop(0)

        self._clock.append(hits["ch12"])
        if len(self._clock) == self._window:
            self._clock.pop(0)

    @staticmethod
    def _get_data_list(data_name, data):
        """"
        Given a list of integers return a list of form
        [data_name, last item in list, average value of list]
        or, for a 0 length list, a list of form
        [data_name, 0, 0]  
        @param data_name Name of data.
        @param data List of values.
        @return list.
        """
        if len(data):
            data_list = [data_name, data[-1], sum(data) / len(data)]
        else:
            data_list = [data_name, 0, 0]
        return data_list

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
        rows.append(ReducePyScalersTable._get_data_list( \
            "Triggers", self._triggers))
        rows.append(ReducePyScalersTable._get_data_list( \
            "Trigger requests", self._trigger_requests))
        rows.append(ReducePyScalersTable._get_data_list( \
            "GVA", self._gva))
        rows.append(ReducePyScalersTable._get_data_list( \
            "TOF0", self._tof0))
        rows.append(ReducePyScalersTable._get_data_list( \
            "TOF1", self._tof1))
        rows.append(ReducePyScalersTable._get_data_list( \
            "10 MHz clock", self._clock))
        content["data"] = rows
        return table
