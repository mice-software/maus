spill_generator_number_of_spills = 1

beam = {
    "particle_generator":"counter",
    "random_seed":0,
    "definitions":[{
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":0.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":2212, "energy":5000.0, "time":0.0, "random_seed":0
       },
       "random_seed_algorithm":"incrementing_random",
       "n_particles_per_spill":1,
       "transverse":{
          "transverse_mode":"pencil",
       },
       "longitudinal":{"longitudinal_mode":"pencil",
                   "momentum_variable":"p",
       },
       "coupling":{"coupling_mode":"none"}
    },{
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":101.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":2212, "energy":5000.0, "time":0.0, "random_seed":0
       },
       "random_seed_algorithm":"incrementing_random",
       "n_particles_per_spill":1,
       "transverse":{
          "transverse_mode":"pencil",
       },
       "longitudinal":{"longitudinal_mode":"pencil",
                   "momentum_variable":"p",
       },
       "coupling":{"coupling_mode":"none"}
    },{
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":101.0},
            "momentum":{"x":0.0, "y":0.0, "z":-1.0},
            "particle_id":2212, "energy":5000.0, "time":0.0, "random_seed":0
       },
       "random_seed_algorithm":"incrementing_random",
       "n_particles_per_spill":1,
       "transverse":{
          "transverse_mode":"pencil",
       },
       "longitudinal":{"longitudinal_mode":"pencil",
                   "momentum_variable":"p",
       },
       "coupling":{"coupling_mode":"none"}
    },{
       "reference":{
            "position":{"x":101.0, "y":0.0, "z":0.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":2212, "energy":5000.0, "time":0.0, "random_seed":0
       },
       "random_seed_algorithm":"incrementing_random",
       "n_particles_per_spill":1,
       "transverse":{
          "transverse_mode":"pencil",
       },
       "longitudinal":{"longitudinal_mode":"pencil",
                   "momentum_variable":"p",
       },
       "coupling":{"coupling_mode":"none"}
    }]
}

