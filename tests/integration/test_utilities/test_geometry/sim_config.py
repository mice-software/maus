import os
simulation_geometry_filename = os.environ['MAUS_ROOT_DIR']+\
                            '/tmp/geometry_download_test/ParentGeometryFile.dat'

beam = {
    "particle_generator":"counter",
    "random_seed":1,
    "definitions":[
    {
       "reference":{
          "position":{"x":0.0, "y":-0.0, "z":-5500.0},
          "momentum":{"x":0.0, "y":0.0, "z":1.0},
          "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
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
    },]
}

verbose_level = 1
