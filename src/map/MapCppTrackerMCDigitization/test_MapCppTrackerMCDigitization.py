# pylint: disable=R0904,C0103
"""Test for MapCppTrackerMCDigitization """
import json
import unittest
import os
from Configuration import Configuration
# import ErrorHandler

from MapCppTrackerMCDigitization import MapCppTrackerMCDigitization

class MapCppTrackerMCDigitizationTestCase(unittest.TestCase):
    """ The MapCppTrackerRecon test.
    Member functions are:

    - bool birth(std::string argJsonConfigDocument);
    - bool death();
    - std::string process(std::string document);
    - void digitization(SciFiSpill &spill, Json::Value &root);
    - void fill_digits_vector(Json::Value &digits, SciFiSpill &a_spill);
    - void cluster_recon(SciFiEvent &evt);
    - void spacepoint_recon(SciFiEvent &evt);
    - void save_to_json(SciFiEvent &evt);
    - void print_event_info(SciFiEvent &event);
    Functions added for testing purpose only:
    - JsonToString(Json::Value json_in);
    - ConvertToJson(std::string jsonString);

    The functions listed above are not elementar; the rely upon calls
    to other functions, for which we have cpp unit tests
    (check the tests/cpp_unit/Recon/ directory).

    This python test will work over the process
    """
    @classmethod
    def setUpClass(self):  # pylint: disable-msg=C0103,C0202
        """ Class Initializer.
            The set up is called before each test function
            is called.
        """
        self.mapper = MapCppTrackerMCDigitization()
        conf = json.loads(Configuration().getConfigJSON())
        conf["reconstruction_geometry_filename"] = "Stage6.dat"
        # Test whether the configuration files were loaded correctly at birth
        success = self.mapper.birth(json.dumps(conf))
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')


    def test_death(self):
        """ Test to make sure death occurs """
        self.assertTrue(self.mapper.death())

    def test_process(self):
        """ Test of the process function """
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        assert root_dir != None
        assert os.path.isdir(root_dir)
        _filename = \
        '%s/src/map/MapCppTrackerMCDigitization/simulation_sample' % root_dir
        assert os.path.isfile(_filename)
        _file = open(_filename, 'r')
        # File is open.
        # Spill 1 is corrupted.
        spill_1 = _file.readline().rstrip()
        output_1 = self.mapper.process(spill_1)
        self.assertTrue("errors" in json.loads(output_1))
        # Spill 2 is sain.
        spill_2 = _file.readline().rstrip()
        output_2 = self.mapper.process(spill_2)
        self.assertTrue("tracker_digits" in json.loads(output_2))
        # spill 3 is end of event
        spill_3 = _file.readline().rstrip()
        output_3 = self.mapper.process(spill_3)
        self.assertTrue("END_OF_RUN" in json.loads(output_3))
        # self.assertFalse("errors" in json.loads(output_3))
        # Close file.
        _file.close()

    @classmethod
    def tear_down_class(self):# pylint: disable-msg=C0202
        """___"""
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
