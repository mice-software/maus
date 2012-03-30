"""
Tests for ReducePyScalersTable module.
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

from datetime import datetime
import json
import time
import unittest

from ReducePyScalersTable import ReducePyScalersTable

class ReducePyScalersTableTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for ReducePyScalersTable.
    """

    @classmethod
    def setUpClass(self): # pylint: disable=C0202
        """ 
        Prepare for test by setting up worker.
        @param self Object reference.
        """
        self.__reducer = ReducePyScalersTable()

    def setUp(self):
        """ 
        Invoke "birth" and check for success.
        @param self Object reference.
        """
        success = self.__reducer.birth("{}")
        if not success:
            raise Exception('Test setUp failed', 'reducer.birth() failed')

    def test_invalid_json(self):
        """
        Test "process" with a bad JSON document as an argument string.
        @param self Object reference.
        """
        result_str = self.__reducer.process("{")
        result = json.loads(result_str)
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        self.assertTrue("ReducePyScalersTable" in errors,
            "No ReducePyScalersTable field")        
        errors = errors["ReducePyScalersTable"]
        self.assertTrue(len(errors) >= 1, "Missing error trace")
        self.assertEquals("<type 'exceptions.ValueError'>: Expecting object: line 1 column 0 (char 0)", errors[0], "Unexpected error trace") # pylint: disable=C0301

    def test_no_daq_data(self):
        """
        Test "process" with a JSON document with no "daq_data".
        @param self Object reference.
        """
        result = self.__process({})
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        self.assertTrue("ReducePyScalersTable" in errors,
            "No ReducePyScalersTable field")        
        errors = errors["ReducePyScalersTable"]
        self.assertTrue(len(errors) >= 1, "Missing error trace")
        self.assertEquals("<type 'exceptions.KeyError'>: 'daq_data is not in spill'", errors[0], "Unexpected error trace") # pylint: disable=C0301

    def test_daq_data_none(self):
        """
        Test "process" with a JSON document with a "daq_data"
        with value None.
        @param self Object reference.
        """
        result = self.__process({"daq_data":None})
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        self.assertTrue("ReducePyScalersTable" in errors,
            "No ReducePyScalersTable field")        
        errors = errors["ReducePyScalersTable"]
        self.assertTrue(len(errors) >= 1, "Missing error trace")
        self.assertEquals("<type 'exceptions.ValueError'>: daq_data is None", errors[0], "Unexpected error trace") # pylint: disable=C0301

    def test_no_v830(self):
        """
        Test "process" with a JSON document with no "V830".
        @param self Object reference.
        """
        result = self.__process({"daq_data":{}})
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        self.assertTrue("ReducePyScalersTable" in errors,
            "No ReducePyScalersTable field")        
        errors = errors["ReducePyScalersTable"]
        self.assertTrue(len(errors) >= 1, "Missing error trace")
        self.assertEquals("<type 'exceptions.KeyError'>: 'V830 is not in spill'", errors[0], "Unexpected error trace") # pylint: disable=C0301

    def test_no_channels(self):
        """
        Test "process" with a JSON document with no "channels".
        @param self Object reference.
        """
        result = self.__process({"daq_data":{"V830":{}}})
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        self.assertTrue("ReducePyScalersTable" in errors,
            "No ReducePyScalersTable field")        
        errors = errors["ReducePyScalersTable"]
        self.assertTrue(len(errors) >= 1, "Missing error trace")
        self.assertEquals("<type 'exceptions.KeyError'>: 'channels is not in spill'", errors[0], "Unexpected error trace") # pylint: disable=C0301

    def test_no_ch0(self):
        """
        Test "process" with a JSON document with no "ch0".
        @param self Object reference.
        """
        result = self.__process({"daq_data":{"V830":{"channels":{}}}})
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        self.assertTrue("ReducePyScalersTable" in errors,
            "No ReducePyScalersTable field")        
        errors = errors["ReducePyScalersTable"]
        self.assertTrue(len(errors) >= 1, "Missing error trace")
        self.assertEquals("<type 'exceptions.KeyError'>: 'ch0 is not in spill'", errors[0], "Unexpected error trace") # pylint: disable=C0301

    def __process(self, json_doc):
        """
        Convert given JSON document to a string and pass to "process".
        @param self Object reference.
        @param json_doc JSON document.
        @returns JSON document string from "process".
        """
        json_str = json.dumps(json_doc)
        result_str = self.__reducer.process(json_str)
        return json.loads(result_str)

    @staticmethod
    def __get_spill(event, time_stamp, value):
        """
        Create a sample spill.
        @param event Event ID to put in spill.
        @param time_stamp Time stamp to put in spill.
        @params value Value for each channel.
        @return spill
        """
        spill = {}
        spill["daq_data"] = {}
        spill["daq_data"]["V830"] = {}
        scalers = spill["daq_data"]["V830"]
        scalers["phys_event_number"] = event    
        scalers["time_stamp"] = time_stamp
        hits = {}
        hits["ch0"] = value
        hits["ch1"] = value
        hits["ch2"] = value
        hits["ch3"] = value
        hits["ch4"] = value
        hits["ch12"] = value
        scalers["channels"] = hits
        return spill

    def test_process_spill(self):
        """
        Test "process" with a spill.
        @param self Object reference.
        """
        current_time = time.time()
        spill = ReducePyScalersTableTestCase.__get_spill( \
            "process_spill", current_time, 1)
        result = self.__process(spill)
        self.__check_result(result, "process_spill", current_time, 1, 1)

    def test_process_multiple_spills(self):
        """
        Test "process" with multiple JSON documents so to cycle
        past the window size of the average of the 10 most recent
        values.
        @param self Object reference.
        """
        for i in range(0, 14):
            # Calculate expected values.
            base = i - 9
            if (base < 0):
                base = 0
            values = range(base, i + 1)
            average = sum(values) / len(values)
            event = "multiple_spills %d" % i
            current_time = time.time()
            spill = ReducePyScalersTableTestCase.__get_spill( \
                event, current_time, i)
            result = self.__process(spill)
            self.__check_result(result, event, current_time, i, average)
        # Send down an end of run.
        end_of_run = {"daq_data":None, "daq_event_type":"end_of_run", \
            "run_num":1, "spill_num":-1}
        result = self.__process(end_of_run)
        self.__check_result(result, event, current_time, 13, average)

    def test_end_of_run(self):
        """
        Test "process" with a JSON document which is an end_of_run.
        @param self Object reference.
        """
        end_of_run = {"daq_data":None, "daq_event_type":"end_of_run", \
            "run_num":1, "spill_num":-1}
        result = self.__process(end_of_run)
        self.__check_result(result, "", None, 0, 0)
        result = self.__process(end_of_run)
        self.assertEquals({}, result, "Unexpected spill after end_of_run")

    def __check_result(self, result, event, time_stamp, value, average): # pylint: disable=R0913, C0301
        """
        Check result spill for process.
        @param self Object reference. 
        @param result spill to validate.
        @param event Event ID to put in spill.
        @param time_stamp Time stamp to put in spill.
        @param value Expected value.
        @param average Expected average.
        """
        self.assertTrue("table" in result, "No table")
        table = result["table"]
        self.assertTrue("keywords" in table, "No keywords")
        self.assertTrue("description" in table, "No description")
        if (time_stamp != None):
            time_str = datetime.fromtimestamp(time_stamp)
        else:
            time_str = ""
        description = "Scaler counts from channel data for event: %s at time: %s" % (event, time_str) # pylint: disable=C0301
        self.assertEquals(description, table["description"],
            "Unexpected description")
        self.assertTrue("data" in table, "No data")
        data = table["data"]
        self.assertEquals(6, len(data), "Unexpected number of rows")
        for i in range(0, 6):
            row = data[i]
            self.assertEquals(value, row[1], "Unexpected value")
            self.assertEquals(average, row[2], "Unexpected average")

    def tearDown(self):
        """
        Invoke "death".
        @param self Object reference.
        """
        success = self.__reducer.death()
        if not success:
            raise Exception('Test setUp failed', 'reducer.death() failed')

    @classmethod
    def tearDownClass(self): # pylint: disable=C0202
        """ 
        Set the worker to None.
        @param self Object reference.
        """
        self.__reducer = None

if __name__ == '__main__':
    unittest.main()
