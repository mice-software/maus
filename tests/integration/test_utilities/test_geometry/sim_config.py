import os
simulation_geometry_filename = os.environ['MAUS_ROOT_DIR']+'/tmp/geometry_download_test/ParentGeometryFile.dat'

beam = {
    "particle_generator":"counter", # routine for generating empty primaries
    "random_seed":1,
    "definitions":[
    ##### MUONS #######
    {
       "reference":{
          "position":{"x":0.0, "y":-0.0, "z":-5500.0},
          "momentum":{"x":0.0, "y":0.0, "z":1.0},
          "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
        }, # reference particle
       "random_seed_algorithm":"incrementing_random", # algorithm for seeding MC
       "n_particles_per_spill":1, # probability of generating a particle
       "transverse":{
          "transverse_mode":"pencil", # transverse distribution matched to constant solenoid field
       },
       "longitudinal":{"longitudinal_mode":"pencil", # longitudinal distribution sawtooth in time
                   "momentum_variable":"p", # Gaussian in total momentum (options energy, pz)
        }, # end time of sawtooth
       "coupling":{"coupling_mode":"none"} # no dispersion
    },]
}

verbose_level = 1
