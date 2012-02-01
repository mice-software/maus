import json
import unittest
import os
import random
from Configuration import Configuration
# import ErrorHandler

from MapCppTrackerRecon import MapCppTrackerRecon
from MapPyFakeTestSimulation import MapPyFakeTestSimulation

class MapCppTrackerReconTestCase(unittest.TestCase):
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
    def setUpClass(self):
        """ Class Initializer.
            The set up is called before each test function
            is called.
        """
        self.mapper = MapCppTrackerRecon()
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
        '%s/src/map/MapPyFakeTestSimulation/lab7_unpacked' % root_dir
        assert os.path.isfile(self._filename)
        self._file = open(self._filename, 'r')
        self._document = self._file.readline().rstrip()
        self._file.close()

    def test_death(self):
        """ Test to make sure death occurs """
        self.assertTrue(self.mapper.death())

    def test_process(self):
        """ Test of the process function """
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        assert root_dir != None
        assert os.path.isdir(root_dir)
        _filename = \
        '%s/src/map/MapPyFakeTestSimulation/lab7_unpacked' % root_dir
        assert os.path.isfile(_filename)
        _file = open(_filename, 'r')
        _document = _file.readline().rstrip()
        print _document
        #Expect an error in the Json output file
        output = self.mapper.process(_document)
        print output
        self.assertTrue("errors" in json.loads(output))
        _document = _file.readline().rstrip()
        print _document
        output = self.mapper.process(_document)
        print output
        self.assertTrue("digits" in json.loads(output))
        _file.close()

    @classmethod
    def tear_down_class(self):
        """___"""
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
