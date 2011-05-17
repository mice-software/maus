import json
import unittest

from Configuration import Configuration
import io
from MapPyCreateSpill import MapPyCreateSpill

class MapPyCreateSpillTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        """bla"""
        testJson = {}
        testJson['mc'] = []
        testJson['mc'].append({'tracks': {}})
        testJson['mc'][0]['tracks']['track1'] = {'particle_id' : 13}
        testJson['mc'][0]['tracks']['track2'] = {'particle_id' : -13}
        testJson['mc'][0]['tracks']['track3'] = {'particle_id' : 5}
        testJson['mc'][0]['hits'] = {}
        testJson['mc'].append(testJson['mc'][0])
        #testJson['mc'].append({})
        self.testString1 = json.dumps(testJson)

    def test_empty(self):
        mapper = MapPyCreateSpill()

        c = Configuration()
        success = mapper.birth(c.getConfigJSON())
        self.assertTrue(success)

        result = mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

        mapper.death()

    def test_mc_branch(self):
        mapper = MapPyCreateSpill()

        c = Configuration()
        success = mapper.birth(c.getConfigJSON())
        self.assertTrue(success)

        result = mapper.process(self.testString1)
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("pre-existing_branch" in doc["errors"])
        mapper.death()

    def test_Npositive(self):
        mapper = MapPyCreateSpill()
        
        c = Configuration()
        success = mapper.birth(c.getConfigJSON())
        self.assertTrue(success)
        
        N = mapper.n_particles
        print " CHECK " + str(N)
        self.assertGreater(N,0,"Positive N required")

        mapper.death()

    def test_return(self):
        '''Ensures that Process returns something'''
        mapper = MapPyCreateSpill()
        self.assertTrue(mapper.birth("{}"))
        
        result = mapper.process("{}")
        self.assertEqual(result, '{"mc": []}')
        self.assertTrue(mapper.death())

        resultStr = mapper.process("{}")
        result = json.loads(mapper.process(resultStr))

        self.assertTrue("errors" in result)



if __name__ == '__main__':
    unittest.main()
