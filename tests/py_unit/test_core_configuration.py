import json
import unittest
import subprocess
import glob
import os
import sys
import io

from Configuration import Configuration

class ConfigurationTestCase(unittest.TestCase):
    @classmethod
    def setUp(cls):
        cls.temp_argv = sys.argv
        sys.argv = [""]

    @classmethod
    def tearDown(cls):
        sys.argv = cls.temp_argv

    def test_defaults(self):
        """Check that we load configuration defaults correctly"""
        a_config = Configuration()
        maus_config = json.loads(a_config.getConfigJSON())

        ## test setup
        MAUSRootDir = os.environ.get('MAUS_ROOT_DIR')
        self.assertNotEqual(MAUSRootDir,  None)

        configDict = {}
        defaultFilename = '%s/src/common_py/ConfigurationDefaults.py' % MAUSRootDir
        exec(open(defaultFilename,'r').read(), globals(), configDict)

        # compare; note we are allowed additional entries in maus_config that
        # are hard coded (e.g. version number)
        for key in configDict.keys():
            if key != "maus_version": # changed at runtime, tested below
                self.assertEqual(configDict[key], maus_config[key])

    def test_version(self):
        """Check that the version is defined correctly"""
        config = json.loads(Configuration().getConfigJSON())
        words = config['maus_version'].split()
        self.assertEqual(words[0], 'MAUS')
        assert words[1] == 'development' or words[1] == 'release'
        self.assertEqual(words[2], 'version')
        numbers = words[-1].split('.')
        self.assertEqual(len(numbers), 3)
        for num in numbers: int(num) # should be able to convert to number

    def test_new_value(self):
        """Test that we can create a configuration value from an input file"""
        stringFile = io.StringIO(u"test = 4")
        c = Configuration()
        value = c.getConfigJSON(stringFile)

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue["test"], 4)

    def test_overwrite_value(self):
        """Test that we can overwrite configuration value from an input file"""
        stringFile = io.StringIO(u"simulation_geometry_filename = 'Stage4Something.dat'")
        conf = Configuration()
        value = conf.getConfigJSON(stringFile)

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue["simulation_geometry_filename"], 'Stage4Something.dat')

    def test_string_to_bool(self):
        """Test conversion from string to boolean type"""
        for a_string in ["tRue", "t", "Y", "1", "yEs"]:
            self.assertTrue(Configuration().string_to_bool(a_string))
        for a_string in ["faLse", "f", "N", "0", "nO"]:
            self.assertFalse(Configuration().string_to_bool(a_string))
        self.assertRaises(ValueError, Configuration().string_to_bool, "bob")
        self.assertRaises(TypeError, Configuration().string_to_bool, 0)

    def test_command_line_arguments_str(self):
        """Test parsing string from command line to configuration"""
        sys.argv = ["", "-input_string", "test_in"]
        test_out = Configuration().command_line_arguments({
                                                      "input_string":""})
        self.assertEqual(test_out["input_string"], "test_in")


    def test_command_line_arguments_number(self):
        """Test parsing number from command line to configuration"""
        sys.argv = ["", "-input_int", "10"] # int as int okay
        test_out = Configuration().command_line_arguments({
                                                      "input_int":0})
        self.assertEqual(test_out["input_int"], 10)

        sys.argv = ["", "-input_float", "10.1"] # float as int -> error
        self.assertRaises(ValueError, Configuration().command_line_arguments, {
                                                      "input_float":0}) # int

        sys.argv = ["", "-input_float", "10"] # int as float -> float
        test_out = Configuration().command_line_arguments({
                                                      "input_float":0.})
        self.assertEqual(type(test_out["input_float"]), type(10.))
        self.assertAlmostEqual(test_out["input_float"], float(10))

        sys.argv = ["", "-input_float", "10."]# float as float -> float
        test_out = Configuration().command_line_arguments({
                                                      "input_float":0.})
        self.assertAlmostEqual(test_out["input_float"], 10.)

    def test_command_line_arguments_bool(self):
        """Test parsing bool from command line to configuration"""
        sys.argv = ["", "-input_bool", "yEs"]
        test_out = Configuration().command_line_arguments({
                                                      "input_bool":True})
        self.assertEqual(test_out["input_bool"], True)

        sys.argv = ["", "-input_bool", "bob"] 
        self.assertRaises(ValueError, Configuration().command_line_arguments, 
                                                           {"input_bool":True})

    def test_command_line_arguments_dict(self):
        """Test parsing bool from command line to configuration"""
        sys.argv = ["", "-input_dict", "{}"]
        self.assertRaises(NotImplementedError, 
                      Configuration().command_line_arguments, {"input_dict":{}})

        sys.argv = ["", "-input_list", "[]"]
        self.assertRaises(NotImplementedError,
                      Configuration().command_line_arguments, {"input_list":[]})

if __name__ == '__main__':
    unittest.main()
