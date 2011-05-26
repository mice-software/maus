import os
import json
import types
import unittest

from Configuration import Configuration

import MAUS

class MapPyTOFCablingTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        self._datapath = '%s/src/input/InputCppRealData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        self._datafile = '02873.003'

    def test_something(self):
        inputter = MAUS.InputCppRealData(self._datapath, self._datafile)
        inputter.birth("{}")

        mapper = MAUS.MapPyTOFCabling()
        success = mapper.birth("{}")
        self.assertTrue(success)

        for data in inputter.emitter():
            return
            data = json.loads(data)
            if data['daq_data'] is None:
                continue

            for subdata in data['daq_data']:
                if subdata is None:
                    continue

                if 'tdc' in subdata.keys():
                    print subdata['tdc'].keys()
                    subsubdata = subdata['tdc']
                    print 'geo', subsubdata['geo']
                    print 'ttt', subsubdata['trigger_time_tag']

                    new_hits = []
                    for hit in subsubdata['hits']:
                        del hit['trailing_time']
                        new_hits.append(hit)
                    print new_hits[0-4]
                    
                    
              #result = mapper.process("")
              #shaSum = hashlib.sha1(result).hexdigest()
              #self.assertEqual(shaSum, self.expectedShaSumTracks)
        mapper.death()


if __name__ == '__main__':
    unittest.main()
