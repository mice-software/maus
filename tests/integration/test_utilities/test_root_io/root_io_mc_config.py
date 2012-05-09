"""
Configuration for root io test
"""
#pylint: disable=C0103

verbose_level = 1
simulation_geometry_filename = "Stage6.dat"
spill_generator_number_of_spills = 2
beam = {
    "particle_generator":"counter",
    "random_seed":2,
    "definitions":[{ # 9 pions
        "reference":{
            "position":{"x":0.0, "y":-0.0, "z":-4700.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":211,
            "energy":235.0,
            "time":0.0,
            "random_seed":0
        },
        "random_seed_algorithm":"incrementing_random",
        "n_particles_per_spill":9,
        "transverse":{"transverse_mode":"pencil"},
        "longitudinal":{"longitudinal_mode":"pencil",
                        "momentum_variable":"p"},
        "coupling":{"coupling_mode":"none"}
    },{
        "reference":{ # 7 muons, random seed 2-9
            "position":{"x":0.0, "y":-0.0, "z":-4700.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-13,
            "energy":235.0,
            "time":0.0,
            "random_seed":0
        },
        "random_seed_algorithm":"incrementing",
        "n_particles_per_spill":7,
        "transverse":{"transverse_mode":"pencil"},
        "longitudinal":{"longitudinal_mode":"pencil",
                        "momentum_variable":"p"},
        "coupling":{"coupling_mode":"none"}
    },{
        "reference":{ # 1 high energy muon, try to get hits in all detectors
            "position":{"x":0.0, "y":-0.0, "z":-15000.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-13,
            "energy":23500.0,
            "time":0.0,
            "random_seed":0
        },
        "random_seed_algorithm":"incrementing",
        "n_particles_per_spill":1,
        "transverse":{"transverse_mode":"pencil"},
        "longitudinal":{"longitudinal_mode":"pencil",
                        "momentum_variable":"p"},
        "coupling":{"coupling_mode":"none"}
    }]
}

