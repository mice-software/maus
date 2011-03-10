import MapPyValidateSpill
from MapPyValidateSpill import MapPyValidateSpill
import json
import unittest
import validator
import SpillSchema
import SchemaSchema

class MapPyValidateSpillTestCase(unittest.TestCase):
  def test_birth(self): #dumb check that nothing crashes during birth
    mapper = MapPyValidateSpill()
    mapper.Birth()

  def __test_validate_schema(self):
    val = validator.JSONSchemaValidator(interactive_mode=False)
    val.validate(SpillSchema.three_vec_schema, SchemaSchema.schema)
    val.validate(SpillSchema.mc_track_hit_schema, SchemaSchema.schema)
    val.validate(SpillSchema.mc_particle_schema, SchemaSchema.schema)
    val.validate(SpillSchema.mc_tracks_schema, SchemaSchema.schema)
    val.validate(SpillSchema.mc_schema, SchemaSchema.schema)
    val.validate(SpillSchema.spill_schema, SchemaSchema.schema)

  def __test_spill(self, spill, raisesError=False):
    mapper = MapPyValidateSpill()
    mapper.Birth()
    out = json.loads(mapper.Process(spill))
    map_name = mapper.__class__.__name__ 
    if "errors" in out and map_name in out["errors"]:
      raise KeyError("Found error: "+str(out["errors"][map_name]))
  
  def test_mc(self):
    self.__test_spill(self.empty_spill)
    self.__test_spill(self.empty_mc)
    self.__test_spill(self.empty_particle)
    self.__test_spill(self.empty_particle_hit)
    self.__test_spill(self.empty_particle_track)
    self.__test_spill(self.particle_pid)
    with self.assertRaises(KeyError):
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
