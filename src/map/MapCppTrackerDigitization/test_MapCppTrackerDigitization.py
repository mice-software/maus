import json
import unittest
import os
import random
from Configuration import Configuration
# import ErrorHandler

from MapCppTrackerDigitization import MapCppTrackerDigitization
from MapPyFakeTestSimulation import MapPyFakeTestSimulation

class MapCppTrackerDigitizationTestCase(unittest.TestCase):
    """ The Tracker Digitization test.
    Member functions are:

    - bool birth(std::string argJsonConfigDocument); Y
    - bool death(); Y
    - std::string process(std::string document); Y - but doesn't really do anything.
    - bool check_sanity_mc(std::string document); Y
    - int get_tdc_counts(Json::Value ahit); Y
    - double get_npe(double edep); Y
    - int get_chan_no(Json::Value ahit);
    - std::vector<Json::Value> make_all_digits(Json::Value hits); Not really an elementary function - calls others.
    - Json::Value make_bundle(std::vector<Json::Value> _alldigits); Y
    - int compute_adc_counts(double nPE); Y
    - bool check_param(Json::Value* hit1, Json::Value* hit2); Y

    """
    @classmethod
    def setUpClass(self):
        """ Class Initializer.
            The set up is called before each test function
            is called.
        """
        self.mapper = MapCppTrackerDigitization()
        self.preMapper = MapPyFakeTestSimulation()
        conf = Configuration()
        # print json.dumps(json.loads(conf.getConfigJSON()), indent=2)
        self.preMapper.birth(conf.getConfigJSON())

        # Test whether the configuration files were loaded correctly at birth
        success = self.mapper.birth(conf.getConfigJSON())

        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

        # Read a line from /src/map/MapPyFakeTestSimulation/mausput_digits
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        assert root_dir != None
        assert os.path.isdir(root_dir)
        self._filename = \
        '%s/src/map/MapPyFakeTestSimulation/mausput_digits' % root_dir
        assert os.path.isfile(self._filename)
        self._file = open(self._filename, 'r')
        self._document = self._file.readline().rstrip()
        self._file.close()

    def test_death(self):
        """ Test to make sure death occurs """
        self.assertTrue(self.mapper.death())

    def test_process(self):
        """ Test of the process function """
        data = self.preMapper.process("{}")
        newdata = self.mapper.process(data)
        #Expect a digits branch in the Json output file
        self.assertTrue("digits" in json.loads(newdata))

    def test_sanity(self):
        """ Calls the function check_sanity_mc.
            This test checks if a change in the data structure
            broke the Tracker Digitization.
        """
        self.assertTrue(self.mapper.check_sanity_mc(self._document))

    def test_get_tdc(self):
        """ Test the get_tdc_counts function """
        hit_time_string = """{"time": 66000}"""
        hit_time = self.mapper.ConvertToJson(hit_time_string)
        # Expect the generated TDC count to be greater or
        # equal to 0 and less or equal to 65535
        self.assertTrue(self.mapper.get_tdc_counts(hit_time) <= 65535 \
        and self.mapper.get_tdc_counts(hit_time) >= 0)

    def test_get_npe(self):
        """ Test the get_npe function
            NPE is obtained by multiplying edep by some constants.
            If edep = 0, we expect NPE to be 0
        """
        edep = 0
        self.assertEqual(self.mapper.get_npe(edep), 0)

    def test_compute_adc(self):
        """ Test the compute_adc_counts function """
        numb_pe = 0
        # Assert that if npe = 0, the adc count is 0
        self.assertEqual(self.mapper.compute_adc_counts(numb_pe), 0)
        # Generate a random value for my_npe
        my_npe = random.uniform(0, 30)
        # Expect the generated ADC count to be greater
        # or equal to 0 and less than or equal to 256
        self.assertTrue((self.mapper.compute_adc_counts(my_npe) <= 256) \
        | (self.mapper.compute_adc_counts(my_npe) >= 0))

    def test_make_bundle(self):
        """ Test make_bundle function """
        list_of_digits = []
        # create two digits in the same bundle of seven fibers
        a_digit_string = """{"tdc_counts": 100,
                        "number_photoelectrons": 2,
                        "channel_id": {
                            "tracker_number": 0,
                            "station_number": 1,
                            "type": "Tracker",
                            "plane_number": 0,
                            "fiber_number": 740,
                            "channel_number": 77
                            },
                        "time": 100,
                        "isUsed": 0
                        }"""
        a_digit = self.mapper.ConvertToJson(a_digit_string)

        another_digit_string = """{"tdc_counts": 101,
                        "number_photoelectrons": 3,
                        "channel_id": {
                            "tracker_number": 0,
                            "station_number": 1,
                            "type": "Tracker",
                            "plane_number": 0,
                            "fiber_number": 739,
                            "channel_number": 77
                            },
                        "time": 101,
                        "isUsed": 0
                        }"""
        another_digit = self.mapper.ConvertToJson(another_digit_string)
        # append both to the list
        list_of_digits.append(a_digit)
        list_of_digits.append(another_digit)
        # The test does not work (probably because of the Json vector
        # check if the json output comes with the npe's summed
        json_output = self.mapper.make_bundle(list_of_digits)
        # this json_output is a 'SwigPyObject' object.. it is non-subscriptable
        # will convert to string and run the possible tests over that.
        json_string = self.mapper.JsonToString(json_output)
        doc = json.dumps(json_string)
        # Expect a new entry, 'adc_counts', in the output Json file
        self.assertTrue("adc_counts" in doc)
        # Expect the "isUsed" entry to be erased from the input Json file
        self.assertFalse("isUsed" in doc)

    def test_get_chan_no(self):
        """ The MiceModules are loaded and
            the channel number is calculated,
            given the copy number of the fiber were the hit was found.
            This calculation is more properly tested by comparing its result
            with the legacy computation in SciFiSD.
            This is done for each single tracker hit during run time
            (assertion at line 91).
        """
        a_hit_string = """{ "channel_id": {
                            "tracker_number": 0,
                            "station_number": 1,
                            "type": "Tracker",
                            "plane_number": 0,
                            "fiber_number": 700
                            }
                          }"""
        a_hit = self.mapper.ConvertToJson(a_hit_string)
        channel_output = self.mapper.get_chan_no(a_hit)
        self.assertTrue( channel_output < 215 and channel_output > 0 )

    def test_check_param(self):
        """ test the Check_param function """
        digit1_string = """{"channel_id":{"tracker_number":0,
                    "station_number":0, "channel_number": 0}}"""
        digit1 = self.mapper.ConvertToJson(digit1_string)
        # Passing "two" digits with the same tracker,station,channel
        # number must return true
        self.assertTrue(self.mapper.check_param(digit1, digit1))
        # If we create a non-neighouring digit...
        digit2_string = """{"channel_id":{"tracker_number":0,
                    "station_number":0, "channel_number": 12}}"""
        digit2 = self.mapper.ConvertToJson(digit2_string)
        # ... the function should return false.
        self.assertFalse(self.mapper.check_param(digit1, digit2))

    @classmethod
    def tear_down_class(self):
        """___"""
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
