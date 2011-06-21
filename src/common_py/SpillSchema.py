# sample schema for a Spill

three_vec = {
  "type":"object","properties":{
    "x":{"type":"number"},
    "y":{"type":"number"},
    "z":{"type":"number"}
  }
}

required_three_vec = three_vec
required_three_vec["required"] = True

class mc:
  primary = {"type":"object", "required":True, "properties":{
      "particle_id":{"type":"integer"},
      "energy":{"type":"number"},
      "time":{"type":"number"},
      "position":three_vec,
      "unit_momentum":three_vec,
    }
  }

  steps = {"type":"array", "items":[{
    "type":"object", "properties":{
      "position":required_three_vec,
      "momentum":required_three_vec,
      "proper_time":{"type":"number", "required":True},
      "path_length":{"type":"number", "required":True},
      "time":{"type":"number", "required":True},
      "energy":{"type":"number", "required":True},
      "energy_deposited":{"type":"number", "required":True},
    }
  }]} #needs to be filled out...

  tracks = {"type":"object", "properties":{
    "steps":steps,
    "initial_position":required_three_vec,
    "initial_momentum":required_three_vec,
    "final_position":required_three_vec,
    "final_momentum":required_three_vec,
    "particle_id":{"type":"integer", "required":True},
    "track_id":{"type":"integer", "required":True},
    "parent_track_id":{"type":"integer", "required":True},
  }} #needs to be filled out...

  hits = {"type":"object", "properties":{}} #needs to be filled out...

  virtual_hits = {"type":"array", "items":[{
    "type":"object", "properties":{
      "station_id":{"type":"integer", "required":True},
      "particle_id":{"type":"integer", "required":True},
      "track_id":{"type":"integer", "required":True},
      "time":{"type":"number", "required":True},
      "mass":{"type":"number", "required":True},
      "charge":{"type":"number", "required":True},
      "proper_time":{"type":"number", "required":True},
      "path_length":{"type":"number", "required":True},
      "position":required_three_vec,
      "momentum":required_three_vec,
      "b_field":required_three_vec,
      "e_field":required_three_vec
    }
  }]}

  particles = {"type":"object", "required":True, "properties":{
    "primary":primary,
    "hits":hits,
    "tracks":tracks,
    "virtual_hits":virtual_hits
  }}

  spill = {
    "type":"object", "properties":{
      "particles":particles
    },
  }

spill = {"type":"object", "properties":{"mc_particles":mc.spill}}

