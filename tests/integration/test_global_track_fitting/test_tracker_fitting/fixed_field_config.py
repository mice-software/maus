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
global_track_fits = [
    {
        "will_require_triplet_space_points":False,
        "min_step_size":0.1,
        "max_step_size":100.,
        "will_smooth":False,
        "mass_hypothesis":105.658,
        "charge_hypothesis":1.,
        "min_z":13000.,
        "max_z":16000.,
        "seed":"USTrackerTrackFitSeed",
        "max_iteration":1,
        "detectors":["Tracker0_5", "Tracker0_4", "Tracker0_3", "Tracker0_2", "Tracker0_1"]#, "Virtual"]
    }
]
global_track_fit_materials = ["POLYSTYRENE", "He", "AIR"]

max_track_length = 3000.
verbose_level = 2
physics_processes = "standard"
spill_generator_number_of_spills = 10
beam = {
    "particle_generator":"counter",
    "random_seed":0,
    "definitions":[
       solenoid_transverse
    ]
}
#simulation_geometry_filename = os.path.expandvars("geometry_07469/ParentGeometryFile.dat")
simulation_geometry_filename = os.path.expandvars("geometry_07469/ParentGeometryFile_4T.dat")


#SciFiClusterReconOn = True
#SciFiSpacepointReconOn = True
#SciFiPatRecOn = True
#SciFiSeedPatRec = True

