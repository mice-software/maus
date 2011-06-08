"""Script to test the BeamMaker App"""

import json
import unittest

from Configuration import Configuration
import io
from MapPyBeamMaker import MapPyBeamMaker

class MapPyBeamMakerTestCase(unittest.TestCase):
    """Class or something"""
    @classmethod
    def setUpClass(self):
        """bla"""
        testJson2 = {}
        testJson2['mc'] = []
        testJson2['mc'].append({})
        testJson2['mc'].append({})
       # testJson2['mc'].append({})
        self.good_spill = json.dumps(testJson2)

        testJson = {}
        testJson['mc'] = []
        testJson['mc'].append({})
        testJson['mc'][0] = {'particle_id' : 13}
        testJson['mc'][0] = {'x' : -1.032}
        testJson['mc'][0]['hits'] = {}
      #  testJson['mc'].append(testJson['mc'][0])
        #testJson['mc'].append({})
        self.testString1 = json.dumps(testJson)
      
        
    def test_junk(self):
        '''test with junk input'''

        mapper = MapPyBeamMaker()

        c = Configuration()
  
        success = mapper.birth(c.getConfigJSON())
        self.assertTrue(success)

        result = mapper.process("asdghakfdh")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])
        mapper.death()

    def test_good_datacards(self):
         '''test with sensible datacards'''
         mapper = MapPyBeamMaker()

         c = {}
         c["emittance4D"] = 6.0
         c["centralPz"] = 200.0
         c["sigmaPz"] = 1.0
         c["sigmaTime"] = 0.0
   
         success = mapper.birth(json.dumps(c))
         self.assertTrue(success)
         
         result = mapper.process(self.good_spill)
         doc = json.loads(result)
        
         self.assertFalse("errors" in doc)
       #  self.assertTrue(len(doc["mc"]) == 10)

#       "x" in doc["mc"][1]) #[0]
         
         mapper.death()
         
             
     
        #len(doc["mc"]) == 10
        #"x" in doc["mc"][0]
        #"y" in doc....
        
      #  print doc["mc"][0]["x"]
       
    def test_bad_datacards(self):
        '''test with duff datacards'''

        mapper = MapPyBeamMaker()

        c = {}
        c["emittance4D"] = 1.0
        c["centralPz"] = 1.0
        c["sigmaPz"] = 1.0
        c["sigmaTime"] = 1.0
            
        for key in c.keys():
            new_c = c.copy()
            new_c[key] = "duff"

            with self.assertRaises(AssertionError):
                success = mapper.birth(json.dumps(new_c))

            del new_c[key]
            success = mapper.birth(json.dumps(new_c))
            self.assertFalse(success)

        for key in c.keys():
            new_c = c.copy()
            new_c[key] = "-1.0"

            with self.assertRaises(AssertionError):
                success = mapper.birth(json.dumps(new_c))
        
        mapper.death()
    
    def test_empty(self):
        """empty input - no spill structure"""
        mapper = MapPyBeamMaker()

        c = Configuration()
        success = mapper.birth(c.getConfigJSON())
        self.assertTrue(success)

        result = mapper.process("{}")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("no spill" in doc["errors"])

        mapper.death()

##     def test_return(self):
##         """Test with existing mc"""
##         mapper = MapPyBeamMaker()

##         c = Configuration()
  
##         success = mapper.birth(c.getConfigJSON())
##         self.assertTrue(success)

##         result = mapper.process(self.testString1)
##         doc = json.loads(result)
##         self.assertTrue("errors" in doc)
##         self.assertTrue("pre-existing_branch" in doc["errors"])
##         mapper.death()
        
 
    
    def test_BeamMakerCards(self):
        """Check sensible DataCards have been provided by the user"""
        mapper = MapPyBeamMaker()
        
        c = Configuration()
        success = mapper.birth(c.getConfigJSON())
        self.assertTrue(success)

      #  N = mapper._CreateNparticles
      #  self.assertGreater(N,0,"Positive N required")
        
     
        self.assertGreater(mapper._emittance4d, 0, "negative emittance4d supplied")
        self.assertGreater(mapper._central_pz, 0, "negative central Pz supplied")

        self.assertGreater(mapper._sigma_pz, 0, "negative sigmaPz supplied")

        self.assertGreaterEqual( mapper._sigma_time, 0, "negative sigmaTime supplied")

        mapper.death()
        
if __name__ == '__main__':
    unittest.main()

