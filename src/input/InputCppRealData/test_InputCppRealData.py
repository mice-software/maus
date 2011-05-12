import os
import md5
import unittest

from InputCppRealData import InputCppRealData

class InputCppRealDataTestCase(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        self._datapath = '%s/src/input/InputCppRealData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        self._datafile = '02873.003'

    def test_init(self):
        self.mapper = InputCppRealData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth("{}"))
        # Check re-init without closing fails
        self.assertFalse(self.mapper.birth("{}"))
        self.assertTrue(self.mapper.death())
        return

    # Test a single event
    def test_single(self):
        self.mapper = InputCppRealData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth("{}"))
        # Get a single event and check it's the right size
        self.assertTrue(self.mapper.readNextEvent())
        data = self.mapper.getCurEvent()
        # Data shold be 18 (an empty spill, first event is start of burst)
        self.assertEqual(len(data), 18)
        self.assertTrue(self.mapper.death())
        return

    # Test reading the whole file
    def test_multi(self):
        self.mapper = InputCppRealData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth("{}"))
        event_count = 0

        # We can try md5'ing the whole dataset
        digester = md5.new()

        for i in self.mapper.emitter():
            digester.update(i)
            event_count = event_count + 1

        # We should now have processed 26 events
        self.assertEqual(event_count, 26)

        # Check the md5 sum matches the expected value
        self.assertEqual(digester.hexdigest(), '16999cfce60f78cd0545724abfc23768')

        self.assertTrue(self.mapper.death())

    @classmethod
    def tearDownClass(self):
        pass

if __name__ == '__main__':
    unittest.main()
