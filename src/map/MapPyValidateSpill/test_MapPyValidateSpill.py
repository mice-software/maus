import unittest

import json

import validictory

import ErrorHandler
import MapPyValidateSpill
from MapPyValidateSpill import MapPyValidateSpill
import SpillSchema
import SchemaSchema

class MapPyValidateSpillTestCase(unittest.TestCase):
  def test_birth(self): #dumb check that nothing crashes during birth or death
    mapper = MapPyValidateSpill()
    mapper.Birth()
    mapper.Death()

  def __test_validate_schema(self):
    ### DISABLED due to valedictory bug (fails to parse "dependencies" object) ###
    validictory.validate(SpillSchema.three_vec, SchemaSchema.schema, required_by_default=False)
    validictory.validate(SpillSchema.mc_track_hit, SchemaSchema.schema)
    validictory.validate(SpillSchema.mc_particle, SchemaSchema.schema)
    validictory.validate(SpillSchema.mc_tracks, SchemaSchema.schema)
    validictory.validate(SpillSchema.mc, SchemaSchema.schema)
    validictory.validate(SpillSchema.spill, SchemaSchema.schema)

  def __test_spill(self, spill, raisesError=False):
    mapper = MapPyValidateSpill()
    mapper.Birth()
    out = json.loads(mapper.Process(spill))
    map_name = mapper.__class__.__name__ 
    ErrorHandler.DefaultHandler().on_error = "raise"
  
  def test_mc(self):
    self.__test_spill(self.empty_spill)
    self.__test_spill(self.empty_mc)
    self.__test_spill(self.empty_particle)
    self.__test_spill(self.empty_particle_hit)
    self.__test_spill(self.empty_particle_track)
    self.__test_spill(self.particle_pid)
    with self.assertRaises(ValueError):
      self.__test_spill(self.bad_particle_pid)


  empty_spill    = """{}"""
  empty_mc       = """{"mc":{}}"""
  empty_particle = """{"mc":{"particles":{}}}"""
  empty_particle_hit = """{"mc":{"particles":{"hit":{}}}}"""
  empty_particle_track = """{"mc":{"particles":{"track":{}}}}"""
  particle_pid = """{"mc":{"particles":{"particle_id":-13}}}"""
  bad_particle_pid = """{"mc":{"particles":{"particle_id":-13.1}}}"""

if __name__ == '__main__':
    unittest.main()
