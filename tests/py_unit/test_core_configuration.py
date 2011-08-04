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
        value = c.getConfigJSON()

        ## test setup
        MAUSRootDir = os.environ.get('MAUS_ROOT_DIR')
        self.assertNotEqual(MAUSRootDir,  None)

        configDict = {}
        defaultFilename = '%s/src/common_py/ConfigurationDefaults.py' % MAUSRootDir
        exec(open(defaultFilename,'r').read(), globals(), configDict)

        configJSONStr = json.JSONEncoder().encode(configDict)

        ## compare
        self.assertEqual(value, configJSONStr)

    def test_version(self):
        value = c.getConfigJSON(stringFile)


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
