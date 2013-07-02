simulation_reference_particle = {
    "position":{"x":0.0, "y":-0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
}

keep_steps=False
keep_tracks=True
beam = {
    "particle_generator":"counter",
    "random_seed":0,
    "definitions":[
    {
       "reference":simulation_reference_particle,
       "random_seed_algorithm":"incrementing",
       "n_particles_per_spill":10,
       "transverse":{
          "transverse_mode":"pencil",
          "emittance_4d":6.,
          "normalised_angular_momentum":0.1,
          "bz":4.e-3
       },
       "longitudinal":{"longitudinal_mode":"pencil",
                   "momentum_variable":"p",},
       "coupling":{"coupling_mode":"none"}
    },]
}

kinetic_energy_threshold = 0.1
simulation_geometry_filename = "Stage4.dat"


