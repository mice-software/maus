"""
Datacard to run with simulation of 200MeV/c muons
"""
# pylint: disable=C0103, W0611

import os
job_number = int(os.getenv("LSB_JOBINDEX", 100))
input_root_file_name = "200MeV_mu_plus_hypothesis_Global_Recon" + str(job_number) + ".root"
output_root_file_name = "200MeV_mu_plus_hypothesis_Global_Recon" + str(job_number) + ".root"

output_json_file_name = "200MeV_mu_plus_hypothesis_Global_Recon" + str(job_number) + ".json"
output_json_file_type = "text"
input_json_file_name = "200MeV_mu_plus_hypothesis_Global_Recon" + str(job_number) + ".json"
Ntuples_filename = "ntuples" + str(job_number) + ".root"
efficiency_filename = "efficiency" + str(job_number) + ".csv"
# Used by MapPyRemoveTracks.
keep_only_muon_tracks = False

# Used by MapCppSimulation
keep_tracks = True # set to true to keep start and end point of every track
keep_steps = False # set to true to keep start and end point of every track and
                   # every step point
simulation_geometry_filename = os.getenv("MAUS_ROOT_DIR") + "/files/geometry/download/ParentGeometryFile.dat" # geometry used by simulation
maximum_number_of_steps = 10000
simulation_reference_particle = {
    "position":{"x":0.0, "y":-0.0, "z":12900.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13, "energy":244.3, "time":0.0, "random_seed":job_number
}

#266.2 muon -13, 281.4 pion 211, 244.3 positron -11

muon_half_life = 1.e24
# used by InputPySpillGenerator to determine the number of empty spills that
# will be generated by the simulation
spill_generator_number_of_spills = 1000
# used by MapPyBeamMaker to generate input particle data
# This is a sample beam distribution based on guesses by Chris Rogers of what
# an optimised beam might look like
beam = {
    "particle_generator":"counter", # routine for generating empty primaries
    "random_seed":job_number, # random seed for beam generation; controls also how the MC
                     # seeds are generated
    "definitions":[
    {
       "reference":simulation_reference_particle, # reference particle
       "random_seed_algorithm":"incrementing_random", # algorithm for seeding MC
       "n_particles_per_spill":1, # probability of generating a particle
       "transverse":{
          "transverse_mode":"twiss",
          "beta_x":1000.,
          "alpha_x":0.,
          "emittance_x":6.0, # 4d emittance
          #~ "emittance_x":0.0001,
          "beta_y":8000.,
          "alpha_y":0.5,
          "emittance_y":6.0,
          #~ "emittance_y":0.0001,
          },
       #~ "transverse":{
          #~ "transverse_mode":"constant_solenoid", # transverse distribution matched to constant solenoid field
          #~ "emittance_4d":1., # 4d emittance
          #~ "normalised_angular_momentum":0.1, # angular momentum from diffuser
          #~ "bz":4.e-3 # magnetic field strength for angular momentum calculation
       #~ },
       "longitudinal":{"longitudinal_mode":"sawtooth_time",
                   "momentum_variable":"p",
                   "sigma_p":25., # RMS total momentum
                   "t_start":-1.e6, # start time of sawtooth
                   "t_end":+1.e6}, # end time of sawtooth
       "coupling":{"coupling_mode":"none"} # no dispersion
    }]
}

particle_decay = True
physics_processes = "mean_energy_loss";
TOF_calib_by = "date";
TOF_calib_date_from = "2013-10-01 00:00:00";
#~ header_and_footer_mode = "dont_append"
verbose_level = 1
track_matching_pid_hypothesis = "kMuPlus"
