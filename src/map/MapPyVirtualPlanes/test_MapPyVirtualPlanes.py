import hashlib
import json
import unittest

from MapPyVirtualPlanes import MapPyVirtualPlanes
from Configuration import Configuration
from MapPyFakeTestSimulation import MapPyFakeTestSimulation
from MapPyRemoveTracks import MapPyRemoveTracks
import io

class MapPyVirtualPlanesTestCase(unittest.TestCase):

    def test_birth_bad_config(self):
        self.mapper = MapPyVirtualPlanes()
        with self.assertRaises(ValueError):
            success = self.mapper.birth("")

    def test_birth_empty_json(self):
        self.mapper = MapPyVirtualPlanes()
        success = self.mapper.birth("{}")
        self.assertFalse(success)

    def test_birth_default_config(self):
        self.mapper = MapPyVirtualPlanes()
        c = Configuration()
        success = self.mapper.birth(c.getConfigJSON())
        self.assertTrue(success)

    def test_process_empty_json(self):
        self.mapper = MapPyVirtualPlanes()
        c = Configuration()
        success = self.mapper.birth(c.getConfigJSON())
        self.assertTrue(success) 
        result = json.loads(self.mapper.process("{}"))
        self.assertTrue("errors" in result)
        self.assertTrue("MapPyVirtualPlanes" in result["errors"])

    def test_process_empty(self):
        self.mapper = MapPyVirtualPlanes()
        c = Configuration()
        success = self.mapper.birth(c.getConfigJSON())
        self.assertTrue(success)
        result = json.loads(self.mapper.process(""))
        self.assertTrue("errors" in result)
        self.assertTrue("MapPyVirtualPlanes" in result["errors"])

    def test_with_fake_data(self):
        self.preMapper = MapPyFakeTestSimulation()
        self.preMapper2 = MapPyRemoveTracks()
        self.mapper = MapPyVirtualPlanes()
        
        c = Configuration()

        success = self.preMapper.birth(c.getConfigJSON())
        self.assertTrue(success)

        myConfig = io.StringIO(u"keep_only_muon_tracks = True")
        success = self.preMapper2.birth(c.getConfigJSON(myConfig))
        self.assertTrue(success)
                
        success = self.mapper.birth(c.getConfigJSON())
        self.assertTrue(success)

        resultStr = self.preMapper.process("{}")
        resultStr = self.preMapper2.process(resultStr)
        result = json.loads(self.mapper.process(resultStr))

        self.assertFalse("errors" in result)

        print 'len', len(result["mc"])
        for particle in result["mc"]:
            del particle["hits"]
            #print json.dumps(particle, sort_keys=True, indent=4)
            self.assertTrue("tracks" in particle)

        self.assertTrue("virtual_planes" in result)
        print result["virtual_planes"]


if __name__ == '__main__':
    unittest.main()
