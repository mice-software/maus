import json
import unittest
import subprocess
import glob
import os
import sys
import StringIO

import Configuration

class ConfigurationTestCase(unittest.TestCase):
    def test_defaults(self):
        ## actual data
        value = Configuration.getConfigJSON()

        ## test setup
        MAUSRootDir = os.environ.get('MAUS_ROOT_DIR')
        self.assertNotEqual(MAUSRootDir,  None)

        configDict = {}
        defaultFilename = '%s/core/ConfigurationDefaults.py' % MAUSRootDir
        exec(open(defaultFilename,'r').read(), globals(), configDict)

        configJSONStr = json.JSONEncoder().encode(configDict)

        ## compare
        self.assertEqual(value, configJSONStr)

    def test_new_value(self):
        ## actual data
        stringFile = StringIO.StringIO("test = 4")
        value = Configuration.getConfigJSON(stringFile)

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue["test"], 4)

    def test_overwrite_value(self):
        ## actual data
        stringFile = StringIO.StringIO("geometry_file = 'Stage4Something.dat'")
        value = Configuration.getConfigJSON(stringFile)

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue["geometry_file"], 'Stage4Something.dat')


if __name__ == '__main__':
    unittest.main()
