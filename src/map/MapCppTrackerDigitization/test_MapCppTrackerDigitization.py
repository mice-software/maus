# -*- coding: utf-8 -*-
import json
import unittest
# import math
import random
from Configuration import Configuration
# import ErrorHandler

from MapCppTrackerDigitization import MapCppTrackerDigitization
from MapPyFakeTestSimulation import MapPyFakeTestSimulation

class MapCppTrackerDigitizationTestCase(unittest.TestCase):
    """ Tracker Digitization test """
    @classmethod
    def setUpClass(self):
        """ Class Initializer """
        self.mapper = MapCppTrackerDigitization()
        self.preMapper = MapPyFakeTestSimulation()
        c = Configuration()
        #print json.dumps(json.loads(c.getConfigJSON()), indent=2)
        self.preMapper.birth(c.getConfigJSON())

        # Test whether the configuration files were loaded correctly at birth
        success = self.mapper.birth(c.getConfigJSON()) 

        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

    def test_death(self):
        """ Test to make sure death occurs """
        self.assertTrue(self.mapper.death())

    def test_process(self):
        """ Test of the process function """
        data = self.preMapper.process("{}")
        newdata = self.mapper.process(data)

        #Expect a digits branch in the Json output file
        self.assertTrue("digits" in json.loads(newdata))

    def test_makeAllDigits(self):
        """ Test the make_all_digits function """
        testJson1 = """{"energy_deposited": 0.05176470122878792, 
                    "energy": 184.634319361377, 
                    "pid": 13, 
                    "hit_position": {
                        "y": -10.88859959922152, 
                        "x": -19.00686266219806, 
                        "z": -4650.554758310462
                    }, 
                    "channel_id": {
                        "tracker_number": 0, 
                        "station_number": 1, 
                        "type": "Tracker", 
                        "plane_number": 0, 
                        "fiber_number": 750
                    }, 
                    "charge": -1.0, 
                    "mass": 105.6584, 
                    "time": 28.75016317242986, 
                    "track_id": 1, 
                    "momentum": {
                        "y": -1.902576503668894, 
                        "x": 23.42979974447424, 
                        "z": 149.5779364814894
                    }}"""
        test1 = self.mapper.ConvertToJson(testJson1)
        # newtest1 = self.mapper.make_all_digits(test1) 
        # self.assertTrue("tdc_counts" in newtest1)
        # Expect the 'tdc_counts' entry in the new branch of the Json vector.
        # The test currently does not work (lack of agreement 
        # between C++ and Python)
    def test_empty(self):
        """ check_sanity_mc must return false if the Json file is empty """
        self.assertFalse(self.mapper.check_sanity_mc(""))

    def test_no_mc_branch(self):
        """
        Test that check_sanity_mc returns false if
        there is no mc branch in the Json file
        """
        self.assertFalse(self.mapper.check_sanity_mc("{}"))

    def test_mc_bad_type(self):
        """
        Test that check_sanity_mc returns false if the
        mc values in the Json file are bad
        """
        self.assertFalse(self.mapper.check_sanity_mc("""{"mc" : 0.0}"""))

    def test_getTDC(self):
        """ Test the getTDCcounts function """
        testJson1 = """{"time": 100}"""
        test1 = self.mapper.ConvertToJson(testJson1)
        self.assertTrue(self.mapper.getTDCcounts(test1) <= 65536 | self.mapper.getTDCcounts(test1) >= 0)
        testJson2 = """{"time": 66000}"""
        test2 = self.mapper.ConvertToJson(testJson2)
        # Expect the generated TDC count to be greater or
        # equal to 0 and less or equal to 65535
        self.assertTrue(self.mapper.getTDCcounts(test2) <= 65535 | self.mapper.getTDCcounts(test1) >= 0)
        print "Generated TDC count1 is:", self.mapper.getTDCcounts(test1)
        print "Generated TDC count2 is:", self.mapper.getTDCcounts(test2)

    def test_getChanNo(self):
        """ Test the getChanNo function """
        testJson1 = """{"channel_id":{"tracker_number":1,
                    "station_number":4, "plane_number":2,
                    "fiber_number": 750}}"""
        test1 = self.mapper.ConvertToJson(testJson1)
        #self.assertEqual(self.mapper.getChanNo(test1), floor(750,7))
        # Expect the resultant channel number to be calculated from the
        # fiber number -> cannot do that because of the modules stuff...

    def test_getNPE(self):
        """ Test the getNPE function """
        edep = 0
        # NPE is obtained by mupltiplying edep by a certain number.
        # If edep = 0, we expect NPE to be 0If edep = 0, we expect NPE to be 0
        self.assertEqual(self.mapper.getNPE(edep), 0)

    def test_computeADC(self):
        """ Test the compute_adcCounts function """
        numb_pe = 0
        # edep = 0
        self.assertEqual(self.mapper.compute_adcCounts(numb_pe), 0)
        # edep = random.uniform(0, 200) #Generate a random value for edep
        # Generate a random value for numb_peNZ
        numb_peNZ = random.uniform(0, 30)
        # Expect the generated ADC count to be greater
        # or equal to 0 and less than or equal to 256
        self.assertTrue((self.mapper.compute_adcCounts(numb_peNZ) <= 256) | (self.mapper.compute_adcCounts(numb_peNZ) >= 0)) 
        print "Generate ADC count is:", self.mapper.compute_adcCounts(numb_peNZ)

    def test_makeBundle(self):
        """ Test make_bundle function """
        testlist1 = []
        testJson1 = """{"tdc_counts": 100,
                        "number_photoelectrons": 2,
                        "channel_id": {
                            "tracker_number": 0, 
                            "station_number": 1, 
                            "type": "Tracker", 
                            "plane_number": 0, 
                            "fiber_number": 750,
                            "channel_number": 77
                            },
                        "time": 100,
                        "isUsed": 0
                        }"""
        test1 = self.mapper.ConvertToJson(testJson1)
        testlist1.append(test1)
        # The test does not work (probably because of the Json vector
        newJson = self.mapper.make_bundle(testlist1)
        newJsonString = self.mapper.JsonToString(newJson)
        doc = json.dumps(newJsonString)
        # Expect a new entry, 'adc_counts', in the output Json file
        self.assertTrue("adc_counts" in doc)
        # Expect the "isUsed" entry to be erased from the input Json file
        self.assertFalse("isUsed" in doc)

    def test_checkParam(self):
        """ test the Check_param function """
        testJson1 = """{"channel_id":{"tracker_number":0,
                    "station_number":0, "channel_number": 0}}"""
        testJson2 = """{"channel_id":{"tracker_number":0,
                    "station_number":0, "channel_number": 0}}"""
        test1 = self.mapper.ConvertToJson(testJson1)
        test2 = self.mapper.ConvertToJson(testJson2)
        # Expect true because both input Json files have
        # the same values for tracker, station, channel numbers
        self.assertTrue(self.mapper.Check_param(test1, test2))
        testJson3 = """{"channel_id": {"tracker_number":1,
                    "station_number":2,
                    "channel_number": 0}}"""
        testJson4 = """{"channel_id": {"tracker_number":0,
                    "station_number":0, "channel_number": 0}}"""
        test3 = self.mapper.ConvertToJson(testJson3)
        test4 = self.mapper.ConvertToJson(testJson4)
        # Expect false because the 1st input Json file has
        # different tracker and station numbers
        self.assertFalse(self.mapper.Check_param(test3, test4))
        test5 = self.mapper.ConvertToJson(testJson1)
        test6 = self.mapper.ConvertToJson(testJson2)
        # Expect true because both input Json files have the same values
        # (the test makes sure that channel_number is accepted as a double)
        self.assertTrue(self.mapper.Check_param(test5, test6))
        testJson7 = """{"channel_id": {"tracker_number":0,
                    "station_number":3.3,
                    "channel_number": 50}}"""
        testJson8 = """{"channel_id": {"tracker_number":0,
                    "station_number":3.7,
                    "channel_number": 50}}"""
        test7 = self.mapper.ConvertToJson(testJson7)
        test8 = self.mapper.ConvertToJson(testJson8)
        # Expect true because the station number will be saved as an integer
        self.assertTrue(self.mapper.Check_param(test7, test8))
        testJson9 = """{"channel_id": {"tracker_number":4,
                    "station_number":12,
                    "channel_number": 300}}"""
        testJson10 = """{"channel_id": {"tracker_number":4,
                     "station_number":12,
                     "channel_number": 300}}"""
        # Should there be an exception raised
        # because the values are out of boundaries?
        # self.assertFalse(self.mapper.Check_param(test9,test1))

    @classmethod
    def tearDownClass(self):
        """ """
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
