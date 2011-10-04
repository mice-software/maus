import os
import md5
import unittest
import json
from Configuration import Configuration
from InputCppData import InputCppData

class InputCppDataTestCase(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        self._datapath = '%s/src/input/InputCppData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        self._datafile = '02873'
        self._c = Configuration()

    def test_init(self):
        self.mapper = InputCppData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth( self._c.getConfigJSON() ))
        # Check re-init without closing fails
        self.assertFalse(self.mapper.birth( self._c.getConfigJSON() ))
        self.assertTrue(self.mapper.death())
        return

    # Test a single event
    def test_single(self):
        self.mapper = InputCppData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth(self. _c.getConfigJSON() ))
        # Get a single event and check it's the right size
        self.assertTrue(self.mapper.readNextEvent())
        data = self.mapper.getCurEvent()
        # Data shold be 66 (an empty spill, first event is start of burst)
        self.assertEqual(len(data), 65)
        self.assertTrue(self.mapper.death())
        return

    # Test reading the whole file
    def test_multi(self):
        self.mapper = InputCppData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth( self._c.getConfigJSON() ))
        event_count = 0

        # We can try md5'ing the whole dataset
        digester = md5.new()

        for i in self.mapper.emitter():
            digester.update(i)
            event_count = event_count + 1

        # We should now have processed 26 events
        self.assertEqual(event_count, 26)

        # Check the md5 sum matches the expected value
        self.assertEqual(digester.hexdigest(), '808b39ed52f5ec72528722a4429f5988')

        self.assertTrue(self.mapper.death())

    @classmethod
    def tearDownClass(self):
        pass

if __name__ == '__main__':
    unittest.main()
