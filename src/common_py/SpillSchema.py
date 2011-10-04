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

#some common documentation definitions
pdg_pid_doc = "Identifies the particle type according to the PDG indexing system (http://hepdata.cedar.ac.uk/lbl/2011/reviews/rpp2011-rev-naming-scheme-hadrons.pdf)"

units = {"position":"[mm]", "momentum":"[MeV/c]", "energy":"[MeV]", "mass":"[MeV/c$^{2}$]", "e_field":"[MV/mm]", "b_field":"[kT]", "time":"[ns]", "charge":"[e$^{+}$ charge]"}


def dict_merge(dict_a, dict_b):
  """
  Merge two dicts to return a single dict.
    @param dict_a first dictionary
    @param dict_b second dictionary
  Note that if items are duplicated in dict_a and dict_b, they can be overwritten
  """
  return dict(dict_a.items()+dict_b.items())

class mc:
  primary = {"type":"object", "breakdoc":True, "description":"Describes the initial particle that is used as an input into the Monte Carlo simulation", "required":True, "properties":{
      "particle_id":{"type":"integer"},
      "energy":{"type":"number"},
      "time":{"type":"number"},
      "position":three_vec,
      "momentum":three_vec,
      "random_seed":{"type":"number"},
    }
  }

  steps = {"type":"array", "breakdoc":True, "description":"Stores information on each step in the tracking. Enabled by 'keep_steps' datacard", "items":[{
    "type":"object", "properties":{
      "position":dict_merge(required_three_vec, {"description":"Position of the step "+units["position"]}),
      "momentum":dict_merge(required_three_vec, {"description":"Momentum of the track that made the step "+units["momentum"]}),
      "proper_time":{"type":"number", "required":True, "description":"Proper time of track when it made the step "+units["time"]},
      "path_length":{"type":"number", "required":True, "description":"Distance travelled by the particle when it made the step "+units["position"]},
      "time":{"type":"number", "required":True, "description":"Time of the track in lab frame when it made the step "+units["time"]},
      "energy":{"type":"number", "required":True, "description":"Energy of the track "+units["energy"]},
      "energy_deposited":{"type":"number", "required":True, "description":"Energy deposited by the track on the previous step "+units["energy"]},
    }
  }]}

  tracks = {"type":"object", "breakdoc":True, "description":"Stores information on stepping information, initial and final position of the track. Enabled by 'keep_tracks' datacard", "properties":{
    "steps":steps,
    "initial_position":dict_merge(required_three_vec, {"description":"Initial position of the track "+units["position"]}),
    "initial_momentum":dict_merge(required_three_vec, {"description":"Initial momentum of the track "+units["momentum"]}),
    "final_position":dict_merge(required_three_vec, {"description":"Final position of the track "+units["position"]}),
    "final_momentum":dict_merge(required_three_vec, {"description":"Final momentum of the track "+units["momentum"]}),
    "particle_id":{"type":"integer", "required":True},
    "track_id":{"type":"integer", "required":True},
    "parent_track_id":{"type":"integer", "required":True},
  }}

  hits = {"type":"object", "breakdoc":True, "description":"Stores information on interactions of particles with sensitive detectors", "properties":{}} #needs to be filled out...

  virtual_hits = {"type":"array", "breakdoc":True, "description":"Virtual hits store information on all particles as they cross a user-defined plane in space, time or proper time", "items":[{
    "type":"object", "properties":{
      "station_id":{"type":"integer", "required":True, "description":"ID for the virtual plane that registered this hit. See MiceModules docs for options on how stations are numbered."},
      "particle_id":{"type":"integer", "required":True, "description":pdg_pid_doc},
      "track_id":{"type":"integer", "required":True, "description":"Identifier for the track that made the hit"},
      "time":{"type":"number", "required":True, "description":"particle time for the track that made the hit "+units["time"]},
      "mass":{"type":"number", "required":True, "description":"mass of the particle that made the hit "+units["mass"]},
      "charge":{"type":"number", "required":True, "description":"charge of the particle that made the hit "+units["charge"]},
      "proper_time":{"type":"number", "required":True, "description":"Relativistic proper time of the particle that made the hit "+units["time"]},
      "path_length":{"type":"number", "required":True, "description":"Total path length travelled of the particle that made the hit "+units["position"]},
      "position":dict_merge(required_three_vec, {"description":"Position of the hit "+units["position"]}),
      "momentum":dict_merge(required_three_vec, {"description":"Momentum of the track that made the hit "+units["momentum"]}),
      "b_field":dict_merge(required_three_vec, {"description":"Magnetic field at the position and time that the hit was recorded "+units["b_field"]}),
      "e_field":dict_merge(required_three_vec, {"description":"Electric field at the position and time that the hit was recorded "+units["e_field"]}),
    }
  }]}

  particles = {"type":"object", "required":True, "properties":{
    "primary":primary,
    "hits":hits,
    "tracks":tracks,
    "virtual_hits":virtual_hits
  }}

  spill = {
    "type":"array", "items":[particles],
  }

class daq_data:
  """
  Data output from the InputCppDAQData
  """
  

spill = {"type":"object", "properties":{"mc":mc.spill}}

