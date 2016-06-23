import os

solenoid_transverse =  {
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":13900.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-13, "energy":226., "time":0.0, "random_seed":0,
            "spin":{"x":0.0, "y":0.0, "z":1.0}
       },
       "random_seed_algorithm":"incrementing_random",
       "n_particles_per_spill":100,
       "transverse":{
          "transverse_mode":"constant_solenoid",
          "normalised_angular_momentum":0.,
          "emittance_4d":3.5,
          "bz":4e-3,
       },
       "longitudinal":{"longitudinal_mode":"pencil",
                   "momentum_variable":"energy",
        },
       "coupling":{"coupling_mode":"none"}
}
max_track_length = 3000.
verbose_level = 2
physics_processes = "standard"
spill_generator_number_of_spills = 1
beam = {
    "particle_generator":"counter",
    "random_seed":0,
    "definitions":[
       solenoid_transverse
    ]
}
simulation_geometry_filename = os.path.expandvars("geometry_07469/ParentGeometryFile_pry_mc.dat")

