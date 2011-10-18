import os
import json
import types
import unittest

from Configuration import Configuration

import MAUS

class MapPyScalersAnalysisTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        self._datapath = '%s/src/input/InputCppDAQData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        self._datafile = '02873'
        self._c = Configuration()

    def test_something(self):
        inputter = MAUS.InputCppDAQData(self._datapath, self._datafile)
        inputter.birth( self._c.getConfigJSON() )

        mapper = MAUS.MapPyScalersAnalysis()
        success = mapper.birth("{}")
        self.assertTrue(success)

        success = mapper.add("")
        self.assertFalse(success)

        success = mapper.add("{}")
        self.assertFalse(success)

        success = mapper.dump()
        self.assertTrue(success)

        result = mapper.process("{}")
        self.assertEqual(len(result), 2)

        mapper.death()

if __name__ == '__main__':
    unittest.main()