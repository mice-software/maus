simulation_geometry_filename = "BeamTest.dat"
spill_generator_number_of_spills = 1000
beam = {
    "particle_generator":"binomial", # routine for generating empty primaries
    "binomial_n":20, # number of coin tosses
    "binomial_p":0.1, # probability of making a particle on each toss
    "random_seed":5, # random seed for beam generation; controls also how the MC
                     # seeds are generated
    "definitions":[
    ##### MUONS #######
    {
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":3.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-13,
            "energy":226.0,
            "time":0.0,
            "random_seed":0
        }, # reference particle
       "random_seed_algorithm":"incrementing_random", # algorithm for seeding MC
       "weight":90., # probability of generating a particle
       "transverse":{
            "transverse_mode":"penn", 
            "emittance_4d":6., 
            "beta_4d":333.,
            "alpha_4d":1., 
            "normalised_angular_momentum":2.,
            "bz":4.e-3
        },
        "longitudinal":{
            "longitudinal_mode":"sawtooth_time",
                   "momentum_variable":"p",
                   "sigma_p":25.,
                   "t_start":-1.e6,
                   "t_end":+1.e6},
       "coupling":{"coupling_mode":"none"}
    },
    ##### PIONS #####
    { # as above...
       "reference":{
           "position":{"x":0.0, "y":-0.0, "z":-5500.0},
           "momentum":{"x":0.0, "y":0.0, "z":1.0},
           "particle_id":211, "energy":285.0, "time":0.0, "random_seed":10
       },
       "random_seed_algorithm":"incrementing_random",
       "weight":2.,
       "transverse":{"transverse_mode":"constant_solenoid", "emittance_4d":6.,
            "normalised_angular_momentum":0.1, "bz":4.e-3},
       "longitudinal":{"longitudinal_mode":"uniform_time",
                   "momentum_variable":"p",
                   "sigma_p":25.,
                   "t_start":-1.e6,
                   "t_end":+1.e6},
       "coupling":{"coupling_mode":"none"}
    },
    ##### ELECTRONS #####
    { # as above...
        "reference":{
            "position":{"x":0.0, "y":-0.0, "z":-5500.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-11, "energy":200.0, "time":0.0, "random_seed":10
        },
        "random_seed_algorithm":"incrementing_random",
        "weight":8.,
       "transverse":{"transverse_mode":"constant_solenoid", "emittance_4d":6.,
            "normalised_angular_momentum":0.1, "bz":4.e-3},
       "longitudinal":{"longitudinal_mode":"sawtooth_time",
                   "momentum_variable":"p",
                   "sigma_p":25.,
                   "t_start":-1.e6,
                   "t_end":+1.e6},
        "coupling":{"coupling_mode":"none"}
    }]
}
