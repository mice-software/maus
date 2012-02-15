verbose_level = 0

simulation_geometry_filename = "/home/matt/maus_littlefield/tmp/SIM_FILES/ParentGeometryFile.dat"
spill_generator_number_of_spills = 1

beam = {
    "particle_generator":"binomial", # routine for generating empty primaries
    "binomial_n":50, # number of coin tosses
    "binomial_p":0.5, # probability of making a particle on each toss
    "random_seed":5, # random seed for beam generation; controls also how the MC
                     # seeds are generated
    "definitions":[
    ##### MUONS #######
    {
       "reference":simulation_reference_particle, # reference particle
       "random_seed_algorithm":"incrementing_random", # algorithm for seeding MC
       "weight":90., # probability of generating a particle
       "transverse":{
          "transverse_mode":"constant_solenoid", # transverse distribution matched to constant solenoid field
          "emittance_4d":6., # 4d emittance
          "normalised_angular_momentum":0.1, # angular momentum from diffuser
          "bz":4.e-3 # magnetic field strength for angular momentum calculation
       },
       "longitudinal":{"longitudinal_mode":"sawtooth_time", # longitudinal distribution sawtooth in time
                   "momentum_variable":"p", # Gaussian in total momentum (options energy, pz)
                   "sigma_p":25., # RMS total momentum
                   "t_start":-1.e6, # start time of sawtooth
                   "t_end":+1.e6}, # end time of sawtooth
       "coupling":{"coupling_mode":"none"} # no dispersion
    },
    ##### PIONS #####
    { # as above...
       "reference":{
           "position":{"x":0.0, "y":-0.0, "z":0.0},
           "momentum":{"x":0.0, "y":0.0, "z":1.0},
           "particle_id":211, "energy":285.0, "time":0.0, "random_seed":10
       },
       "random_seed_algorithm":"incrementing_random",
       "weight":2.,
       "transverse":{"transverse_mode":"constant_solenoid", "emittance_4d":6.,
            "normalised_angular_momentum":0.1, "bz":4.e-3},
       "longitudinal":{"longitudinal_mode":"sawtooth_time",
                   "momentum_variable":"p",
                   "sigma_p":25.,
                   "t_start":-1.e6,
                   "t_end":+1.e6},
       "coupling":{"coupling_mode":"none"}
    },
    ##### ELECTRONS #####
    { # as above...
        "reference":{
            "position":{"x":0.0, "y":-0.0, "z":0.0},
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


# configuration database
cdb_upload_url = "http://cdb.mice.rl.ac.uk/cdb/" # target URL for configuration database uploads
cdb_download_url = "http://cdb.mice.rl.ac.uk/cdb/" # target URL for configuration database downloads

# geometry download
geometry_download_wsdl = "geometry?wsdl" # name of the web service used for downloads
geometry_download_directory   = "%s/files/geometry/download" % os.environ.get("MAUS_ROOT_DIR") # name of the local directory where downloads will be placed
geometry_download_by = 'run_number' # choose 'run_number' to download by run number, 'current' to use
                                    # the currently valid geometry or 'id' to use the cdb internal id 
                                    # (e.g. if it is desired to access an old version of a particular
                                    # geometry)
geometry_download_run_number = 0
geometry_download_id = 0
geometry_download_cleanup = True # set to True to clean up after download
g4_step_max = 5.0 # this is the value which shall be placed in the Mice Modules which have been translated from CAD

# geometry upload
geometry_upload_wsdl = "geometrySuperMouse?wsdl" # name of the web service used for uploads
geometry_upload_directory = "/home/matt/StepFiles/Quad/GDML_fastradModel" # name of the local directory where uploads are drawn from
geometry_upload_note = "this is a test quad with a field as well" # note, pushed to the server to describe the geometry. A note must be specified here (default will throw an exception).
geometry_upload_valid_from = "20/01/2012 09:00:00" # date-time in format like: that the specified installation was made in the experiment. A date-time must be specified here (default will throw an exception).
geometry_upload_cleanup = False # set to True to clean up after upload
