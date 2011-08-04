import json
import unittest
import subprocess
import glob
import os
import sys
import io

from Configuration import Configuration

class ConfigurationTestCase(unittest.TestCase):
    def test_defaults(self):
        ## actual data
        c = Configuration()
        maus_config = json.loads(c.getConfigJSON())

        ## test setup
        MAUSRootDir = os.environ.get('MAUS_ROOT_DIR')
        self.assertNotEqual(MAUSRootDir,  None)

        configDict = {}
        defaultFilename = '%s/src/common_py/ConfigurationDefaults.py' % MAUSRootDir
        exec(open(defaultFilename,'r').read(), globals(), configDict)

        # compare; note we are allowed additional entries in maus_config that
        # are hard coded (e.g. version number)
        for key in configDict.keys():
            self.assertEqual(configDict[key], maus_config[key])

    def test_version(self):
        config = json.loads(Configuration().getConfigJSON())
        words = config['maus_version'].split()
        self.assertEqual(words[0], 'MAUS')
        assert words[1] == 'development' or words[1] == 'release'
        self.assertEqual(words[2], 'version')
        numbers = words[-1].split('.')
        self.assertEqual(len(numbers), 3)
        for num in numbers: int(num) # should be able to convert to number

    def test_new_value(self):
        ## actual data
        stringFile = io.StringIO(u"test = 4")
        c = Configuration()
        value = c.getConfigJSON(stringFile)

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue["test"], 4)

    def test_overwrite_value(self):
        ## actual data
        stringFile = io.StringIO(u"simulation_geometry_filename = 'Stage4Something.dat'")
        c = Configuration()
        value = c.getConfigJSON(stringFile)

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue["simulation_geometry_filename"], 'Stage4Something.dat')


if __name__ == '__main__':
    unittest.main()
