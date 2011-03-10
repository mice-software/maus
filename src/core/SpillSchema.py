# sample schema for a Spill
# I haven't filled it in really because I think it will probably change...

three_vec = {
  "type":"object","minimum":0,"properties":{
    "x":{"type":"number"},
    "y":{"type":"number"},
    "z":{"type":"number"}
  }
}

mc_step = {"type":"object", "properties":{}} #needs to be filled out...

mc_track = {"type":"object", "properties":{}} #needs to be filled out...

mc_hit = {"type":"object", "properties":{}} #needs to be filled out...

mc_particles = {"type":"object","properties":{
  "hits":mc_hit,
  "tracks":mc_track,
  "particle_id":{"type":"integer"},
  "position":three_vec,
  "unit_momentum":three_vec,
  "energy":{"type":"number"}
}}

mc = {
  "type":"object", "properties":{
    "particles":mc_particles
  },
}

spill = {"type":"object", "properties":{"mc":mc}}

