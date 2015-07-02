"""
The Configuration defaults

These are the default values for MAUS and its components. Please DO NOT CHANGE
this unless you want to change this quantity for everybody. Values can be
overridden by setting configuration_file parameter on the comamnd line, for
example

bin/simulate_mice.py --configuration_file my_configuration.py
"""


#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

# NOTE: please use lower_case_and_underscores for new configuration cards

import os

type_of_dataflow = 'pipeline_single_thread'

input_json_file_name = "maus_input.json"
input_json_file_type = "text"
output_json_file_name = "maus_output.json"
output_json_file_type = "text"

input_root_file_name = "maus_input.root"
output_root_file_name = "maus_output.root"

# For InputCppRoot Enter an array of spill numbers here to select them from the
# input root file. Leaving the array empty automatically loads all spills.
# (Note present version emits selected spills for all input run numbers)
# e.g. selected_spills = [ 2, 34, 432, 3464 ]
selected_spills = []

# one_big_file - puts everything in output_root_file_name
# one_file_per_run - splits and inserts xxx_<run_number>.xxx for each run, like
#                    maus_output_1111.root
# end_of_run_file_per_run - as above, and places in
#        <end_of_run_output_root_directory>/<run_number>/<output_root_file_name>
#      users responsibility to ensure that <end_of_run_output_root_directory>
#      exists but MAUS will make <run_number> subdirectories
output_root_file_mode = "one_big_file"
end_of_run_output_root_directory = os.environ.get("MAUS_WEB_MEDIA_RAW")+"/end_of_run/" \
              if (os.environ.get("MAUS_WEB_MEDIA_RAW") != None) else os.getcwd()

# Used, for now, to determine what level of
# c++ log messages are reported to the user:
# 0 = debug info (and std::cout)
# 1 = run info
# 2 = warnings
# 3 = errors (and std::cerr)
# 4 = fatal
# >4 = silent
# Doesnt effect python
verbose_level = 1
errors_to_stderr = None # None = from verbose_level; else True or False
errors_to_json = True
on_error = 'none' # none, halt or raise
will_do_stack_trace = verbose_level < 1 # set to True to make stack trace on C++
                                        # exception

# set how headers and footers are handled - "append" will set to
# append headers and footers to the output; dont_append will set to not append
# headers and footers to the output. Affects JobHeader, JobFooter, RunHeader and
# RunFooter
#
# For example, if the user wants to copy data from one format to another, he
# should set to dont_append to avoid header and footer information being taken
# to the output
header_and_footer_mode = "append" #append or dont_append

# Dictionary of variable to be set when using G4BL to generate particles. If
# "get_magnet_currents_pa_cdb" is set to True magnet currents & proton absorber
# thickness will be retrieved from the CDB for the run_number specified
g4bl = {"run_number":2873,"q_1":1.066,"q_2":-1.332,"q_3":0.927,"d_1":-1.302,"d_2":-0.396,\
        "d_s":3.837,"particles_per_spill":0,"rotation_angle":30,"translation_z":680.31,\
        "protonabsorberin":1,"proton_absorber_thickness":93,"proton_number":1E9,"proton_weight":1,\
        "particle_charge":'all',"file_path":'MAUS_ROOT_DIR/src/map/MapPyBeamlineSimulation/G4bl',\
        "get_magnet_currents_pa_cdb":False}

# Used by MapPyRemoveTracks.
keep_only_muon_tracks = False

# Used by MapCppSimulation
keep_tracks = False # set to true to keep start and end point of every track
keep_steps = False # set to true to keep start and end point of every track and
                   # every step point
# simulation_geometry_filename = "Stage4.dat" # geometry used by simulation - should be replaced by CDB version
simulation_geometry_filename = "Test.dat" # geometry used by simulation - should be replaced by CDB version
simulation_geometry_debug    = False
check_volume_overlaps = False
maximum_number_of_steps = 50000000 # particles are killed after this number of
                                 # steps (assumed to be stuck in the fields)
simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":-0.0, "z":-6400.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10,
    "spin":{"x":0.0, "y":0.0, "z":1.0}
   # "beam_polarisation" : "Flat"
}
everything_special_virtual = False

# geant4 physics model
physics_model = "QGSP_BERT" # Physics package loaded by MAUS to set default values; modifications can be made
reference_physics_processes = "mean_energy_loss" # controls the physics processes of the reference particle. Set to "none" to disable all physics processes; or "mean_energy_loss" to make the reference particle see deterministic energy loss only
physics_processes = "standard" # controls the physics processes of normal particles. Set to "none" to disable all physics processes; "mean_energy_loss" to enable deterministic energy loss only; or "standard" to enable all physics processes
particle_decay = True # set to true to activate particle decay, or False to inactivate particle decay
polarised_decay = False # set to true to make muons decay with the correct distribution if the beam is polarised; if true, will force spin tracking on (if spin tracking is off, spin vectors will not be propagated to the decay point)
charged_pion_half_life = -1. # set the pi+, pi- half life [ns]. Negative value means use geant4 default
muon_half_life = -1. # set the mu+, mu- half life [ns]. Negative value means use geant4 default
production_threshold = 0.5 # set the threshold for delta ray production [mm]
fine_grained_production_threshold = { # set the production threshold per pid and per Geant4 region; regions are defined in MiceModules
# "region_name":{"11":0.5, "-11":100.}
# "another_region_name":{"11":0.5, "-11":100.}
}
default_keep_or_kill = True
# map of string pdg pids; always keep particles on creation if their pdg maps to True; always kill particles on creation if their pdg maps to False. Default comes from default_keep_or_kill
keep_or_kill_particles = {"mu+":True, "mu-":True,
                          "nu_e":False, "anti_nu_e":False,
                          "nu_mu":False, "anti_nu_mu":False,
                          "nu_tau":False, "anti_nu_tau":False,
}
max_step_length = 100. # default maximum step size during tracking (override with G4StepMax in MiceModule)
max_track_time = 1.e9 # kill tracks with time above max_time (override with G4TimeMax in MiceModule)
max_track_length = 1.e8 # kill tracks with track length above max_track_length (override with G4TrackMax in MiceModule)
kinetic_energy_threshold = 0.1 # kill tracks with initial kinetic energy below energy_threshold (override with G4KinMin in MiceModule)
field_tracker_absolute_error = 1.e-4 # set absolute error on MAUS internal stepping routines - used by e.g. VirtualPlanes to control accuracy of interpolation
field_tracker_relative_error = 1.e-4 # set relative error on MAUS internal stepping routines - used by e.g. VirtualPlanes to control accuracy of interpolation

stepping_algorithm = "ClassicalRK4" # numerical integration routine
spin_tracking = False # set to true to use G4 routines to precess the spin vector as particles go through EM fields
delta_one_step = -1. # Geant4 step accuracy parameter
delta_intersection = -1.
epsilon_min = -1.
epsilon_max = -1.
miss_distance = -1.

maximum_module_depth = 10 # maximum depth for MiceModules as used by the simulation
# geant4 visualisation (not event display)
geant4_visualisation = False
visualisation_viewer = "VRML2FILE"  # only supported option
visualisation_theta = 90.
visualisation_phi = 90.
visualisation_zoom = 1.
accumulate_tracks = 0 # this accumulates the tracks into one vrml file 1 = yes, 0 = no
#particle colour alterations
default_vis_colour = {"red":0. , "green":100.  ,"blue":0.} #green
pi_plus_vis_colour = {"red":255. , "green":250.  ,"blue":240.} # white
pi_minus_vis_colour = {"red":105. , "green":105.  ,"blue":105.} # grey
mu_plus_vis_colour = {"red":25. , "green":25.  ,"blue":112.} # dark blue
mu_minus_vis_colour = {"red":135. , "green":206.  ,"blue":250.} # light blue
e_plus_vis_colour = {"red":250. , "green":0.  ,"blue":0.} # red
e_minus_vis_colour = {"red":250. , "green":69.  ,"blue":0.} # orange red
gamma_vis_colour = {"red":255. , "green":20.  ,"blue":147.} # pink
neutron_vis_colour = {"red":139. , "green":69.  ,"blue":19.} # brown
photon_vis_colour = {"red":255. , "green":255.  ,"blue":0.} # yellow

# used by InputPySpillGenerator to determine the number of empty spills that
# will be generated by the simulation
spill_generator_number_of_spills = 100
# used by MapPyBeamMaker to generate input particle data
# This is a sample beam distribution based on guesses by Chris Rogers of what
# an optimised beam might look like
beam = {
    "particle_generator":"binomial", # routine for generating empty primaries
    "binomial_n":50, # number of coin tosses
    "binomial_p":0.5, # probability of making a particle on each toss
    "random_seed":5, # random seed for beam generation; controls also how the MC
                     # seeds are generated

#    "particle_generator":"g4bl", # Uses G4BL as input for MAUS
#    "g4bl_generator":"True", # Call G4BL each time new spill is created
#    "random_seed":5,
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
       #"spin":{"x":0.0, "y":0.0, "z":1.0}
    },
    ##### PIONS #####
    { # as above...
       "reference":{
           "position":{"x":0.0, "y":-0.0, "z":-6400.0},
           "momentum":{"x":0.0, "y":0.0, "z":1.0},
           "spin":{"x":0.0, "y":0.0, "z":1.0},
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
      # "spin":{"x":0.0, "y":0.0, "z":1.0}
    },
    ##### ELECTRONS #####
    { # as above...
        "reference":{
            "position":{"x":0.0, "y":-0.0, "z":-6400.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "spin":{"x":0.0, "y":0.0, "z":1.0},
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
       # "spin":{"x":0.0, "y":0.0, "z":1.0}
    }]
}

# this is used by reconstruction; if set to an empty string, automatically
# acquires simulation_geometry_filename
reconstruction_geometry_filename = ""

# scifi tracker digitization
#SciFiDeadChanFName = ""
SciFiDigitizationNPECut = 1.0
SciFiMappingFileName = "scifi_mapping_2015-06-16.txt"
SciFiCalibrationFileName = "scifi_calibration_2015-06-16.txt"
SciFiMUXNum = 7
SciFiFiberDecayConst = 2.7
SciFiFiberConvFactor =  3047.1
SciFiFiberTrappingEff = 0.056
SciFiFiberMirrorEff = 0.6
SciFiFiberTransmissionEff = 0.8
SciFiMUXTransmissionEff = 1.0
SciFivlpcQE = 0.8
SciFivlpcEnergyRes = 4.0 # MeV
SciFivlpcTimeRes = 0.2 # ns
SciFiadcFactor = 6.0
SciFitdcBits = 16
SciFitdcFactor = 1.0
SciFinPlanes = 3
SciFinStations = 5
SciFinTrackers = 2
SciFiNPECut = 2.0 # photoelectrons
SciFiClustExcept = 100 # exception is thrown
SciFi_sigma_tracker0_station5 = 0.4298 # Position error associated with station 5 (mm)
SciFi_sigma_triplet = 0.3844 # Position error for stations 1 to 4 (mm)
SciFi_sigma_z = 0.081 # mm
SciFi_sigma_duplet =  0.6197 # mm
SciFi_sigma_phi_1to4 = 1.0
SciFi_sigma_phi_5 = 1.0
SciFiPRHelicalOn = True # Flag to turn on the tracker helical pattern recognition
SciFiPRStraightOn = True # Flag to turn on the tracker straight pattern recognition
SciFiPatRecVerbosity = 0 # The verbosity of the pat rec (0 - quiet, 1 - more)
SciFiStraightRoadCut = 2.0 # The road cut in pat rec for straights (mm)
SciFiStraightChi2Cut = 15.0 # Chi^2 on pat rec straight track fit
SciFiRadiusResCut = 150.0 # Helix radius cut (mm)
SciFiPatRecCircleChi2Cut = 15.0 # Chi^2 on pat rec circle fit
SciFiNTurnsCut = 0.75 # Cut used when resolving number of turns between tracker stations (mm)
SciFiPatRecSZChi2Cut = 4.0 # Chi^2 cut on pat rec s-z fit
SciFiMaxPt = 180.0 # Transverse momentum upper limit cut used in pattern recognition
SciFiMinPz = 50.0 # Longitudinal momentum lower limit cut used in pattern recognition
SciFiDarkCountProababilty = 0.017 #probability of dark count due to thermal electron
SciFiParams_Pitch = 1.4945
SciFiParams_Station_Radius = 160.
SciFiParams_RMS = 370.
# Parameters used for MCS and E loss correction
#SciFiParams_Z = 5.61291
#SciFiParams_A = 104.15 # g/mol
SciFiParams_Z = 3.5
SciFiParams_A = 6.5
SciFiParams_Plane_Width = 0.6523 # mm
SciFiParams_Radiation_Length = 413.124 # mm
SciFiParams_Density = 1.06 #g/cm3, 0.00106 g/mm3
SciFiParams_Mean_Excitation_Energy = 68.7 # eV
SciFiParams_Density_Correction = 0.164541
#MylarParams_Z = 5.
#MylarParams_A = 192.2 # g/mol
MylarParams_Z = 8.727
MylarParams_A = 4.545 # g/mol
MylarParams_Plane_Width = 0.025# mm
MylarParams_Radiation_Length = 285.364  # mm
MylarParams_Density = 1.4 #g/cm3, 0.0014 g/mm3
MylarParams_Mean_Excitation_Energy = 78.7 # eV
MylarParams_Density_Correction = 0.126782
GasParams_Z = 2.
GasParams_A = 4. # g/mol
GasParams_Radiation_Length = 5671130. # mm
GasParams_Density = 0.000166322 # 1.66322e-04 g/cm3
GasParams_Mean_Excitation_Energy = 41.8 # eV
GasParams_Density_Correction  = 0.13443
SciFiSeedCovariance = 1000.0 # Error estimate for Seed values of the Kalman Fit
SciFiSeedPatRec = True
SciFiKalmanOn = True # Flag to turn on the tracker Kalman Fit
SciFiKalmanCorrectPz = True # Flag to turn on the Kalman Pz correlations
SciFiPatRecOn = True # Flag to turn on the tracker Pattern Recognition
SciFiKalman_use_MCS = True # flag to add MCS to the Kalman Fit
SciFiKalman_use_Eloss = True # flag to add Eloss to the Kalman Fit
SciFiKalmanVerbose  = False # Dump information per fitted track
SciFiDefaultMomentum = 200.0 # Default momentum to assume for straight tracks

# configuration database
cdb_upload_url = "http://cdb.mice.rl.ac.uk/cdb/" # target URL for configuration database uploads TestServer::http://rgma19.pp.rl.ac.uk:8080/cdb/
cdb_download_url = "http://cdb.mice.rl.ac.uk/cdb/" # target URL for configuration database downloads
cdb_cc_download_url = "" # "http://preprodcdb.mice.rl.ac.uk" # target URL for cooling channel configuration database downloads.

# geometry download
geometry_download_wsdl = "geometry?wsdl" # name of the web service used for downloads
geometry_download_directory   = "%s/files/geometry/download" % os.environ.get("MAUS_ROOT_DIR") # name of the local directory where downloads will be placed
geometry_download_by = 'id' # choose 'run_number' to download by run number, 'current' to use
                                    # the currently valid geometry or 'id' to use the cdb internal id
                                    # (e.g. if it is desired to access an old version of a particular
                                    # geometry)
geometry_download_beamline_for_run = 0
geometry_download_beamline_tag = ''
geometry_download_coolingchannel_tag = ''
geometry_download_run_number = 0
geometry_download_id = 49
geometry_download_cleanup = True # set to True to clean up after download
g4_step_max = 5.0 # this is the value which shall be placed in the Mice Modules which have been translated from CAD

# geometry upload
geometry_upload_wsdl = "geometrySuperMouse?wsdl" # name of the web service used for uploads
geometry_upload_directory = "%s/files/geometry/upload" % os.environ.get("MAUS_ROOT_DIR") # name of the local directory where uploads are drawn from
geometry_upload_note = "" # note, pushed to the server to describe the geometry. A note must be specified here (default will throw an exception).
geometry_upload_valid_from = "" # date-time in format like: that the specified installation was made in the experiment. A date-time must be specified here (default will throw an exception).
technical_drawing_name = "" #name and version of the technical drawing from which the CAD model came from.
geometry_upload_cleanup = True # set to True to clean up after upload
technical_drawing_name = "" #name and version of the technical drawing from which the CAD model came from.

#dates need to get geomtry ids
get_ids_start_time = "2012-05-08 09:00:00"
get_ids_stop_time = "2012-05-22 15:47:34.856000"
get_ids_create_file = True

#get beamline information
# This executable will give the run numbers of the runs which the CDB has information on.
# The information is the magnet currents, reasons for run and other information which
# is specific to that run. When downloading a geometry by run number the beamline
# information is merged with the geometrical infomation. Options for querying
# beamline information are; 'all_entries' returns a list of all run numbers with beamline info.
#                           'run_number'  prints whether info is held for this run number or not.
#                           'dates'       returns a list of run numbers with info during specified time period.
get_beamline_by = "all_entries"
get_beamline_run_number = ""
get_beamline_start_time = ""
get_beamline_stop_time = ""

# File Numbers
# This following section gives the files numbers of each detector. The numbers speficy the technical drawing
# number for the sphere which represents each detector in the CAD model. These tags are seen in the style sheet
# and are used to replace the location sphere with the detecor geometry whether it is legacy or GDML.
tof_0_file_number = "Iges_10"
tof_1_file_number = "Iges_11"
tof_2_file_number = "Iges_13"
ckov1_file_number = "Iges_19"
ckov2_file_number = "Iges_21"
kl_file_number = "Iges_14"
emr_file_number = "Iges_15"
tracker0_file_number = "Iges_17"
tracker1_file_number = "Iges_18"
absorber0_file_number = "9999"
absorber1_file_number = "Iges_16"
absorber2_file_number = "9999"

# Survey fit information
survey_measurement_record = ""
# This file should include position references and true locations of each detector.
survey_reference_position = ""
use_gdml_source           = True
# Survey targets
survey_target_detectors = []

# this is used by ImputCppRealData
Number_of_DAQ_Events = -1
Input_Use_JSON = False
Phys_Events_Only = False
Calib_Events_Only = False
Enable_V1290_Unpacking = True
Enable_V1731_Unpacking = True
Enable_V1724_Unpacking = True
Enable_V830_Unpacking = True
Enable_VLSB_Unpacking = True
Enable_VLSB_C_Unpacking = True
Enable_DBB_Unpacking = True
Enable_DBBChain_Unpacking = True
Do_V1731_Zero_Suppression = False
V1731_Zero_Suppression_Threshold = 100
Do_V1724_Zero_Suppression = True
V1724_Zero_Suppression_Threshold = 100
Do_VLSB_Zero_Suppression = True
VLSB_Zero_Suppression_Threshold = 40
Do_VLSB_C_Zero_Suppression = False
VLSB_C_Zero_Suppression_Threshold = 30
Enable_TOF = True
Enable_EMR = True
Enable_KL = True
Enable_CKOV = True
DAQ_cabling_file = "/files/cabling/DAQChannelMap.txt"
DAQ_cabling_file_StepI = "/files/cabling/DAQChannelMap_preRun6541.txt"
DAQ_hostname = 'miceraid5'
DAQ_monitor_name = 'MICE_Online_Monitor'
daq_online_file = '' # set to a file name to force InputCppDAQOnlineData to take
                     # data from a file - mock-up of online for testing, not for
                     # production use (use offline recon here)
daq_online_spill_delay_time = 0. # delay in seconds between daq reads, intended
                                 # for mocking MICE target pulses

# tof digitization
TOFconversionFactor = 0.005 # MeV
TOFpmtTimeResolution = 0.1 # nanosecond
TOFattenuationLength = 140 * 10 # mm
TOFadcConversionFactor = 0.125
TOFtdcConversionFactor = 0.025 # nanosecond
TOFpmtQuantumEfficiency = 0.25
TOFscintLightSpeed =  170.0 # mm/ns

# KL digitization
KLconversionFactor = 0.000125 # MeV
KLlightCollectionEff = 0.031
KLlightGuideEff  = 0.85
KLquantumEff = 0.26
KLlightSpeed =  170.0 # mm/ns
KLattLengthLong  = 2400.0 # mm
KLattLengthShort =  200.0 # mm
KLattLengthLongNorm  = 0.655 # mm
KLattLengthShortNorm   = 0.345 # mm
KLadcConversionFactor = 250000 # nphe/adc
KLpmtGain = 2000000
KLpmtSigmaGain = 1000000

# EMR characteristics
EMRnumberOfPlanes = 48
EMRnumberOfBars = 60 # number of bars in one plane (+ test channel 0)
EMRnBars = 2832 # total number of bars in the EMR
EMRbarLength = 110 # cm, length of a scintillating bar
EMRbarWidth = 33 # mm, base of the triangle
EMRbarHeight = 17 # mm, height of the triangle
EMRgap = 0.5 # mm, gap between two adjacent bars

# EMR event pre-selection
EMRtotNoiseLow = 0
EMRtotNoiseUp = 7 # noise time over threshold window

EMRdeltatSignalLow = -240 # Step I
EMRdeltatSignalUp = -220 # Step I
EMRdeltatNoiseLow = -220 # Step I
EMRdeltatNoiseUp = -175 # Step I

# EMR digitization
EMRdoSampling = 1 # sample number of scintillating photons as a Poisson distribution
EMRnphPerMeV = 2000 # number of photons per MeV
EMRtrapEff = 0.04 # photon trapping efficiency
EMRseed = 2 # seed state of the pseudorandom number generator, change equivalent to a power cycle of fADC
EMRattenWLSf = 2.0 # dB/m, attentuation factor of the WLS fibres
EMRattenCLRf = 0.35 # dB/m, attentuation factor of the clear fibres
EMRspillWidth = 4000000 # DBB counts
EMRbirksConstant = 0.126 # mm/MeV
EMRaveragePathLength = 17 # mm
EMRsignalEnergyThreshold = 0.8 # Me
EMRfom = "median" # figure_Of-Merit for signal calibration

EMRdbbCount = 2.5 # ns, duration of a DBB cycle (f=400MHz)
EMRqeMAPMT = 0.25 # MAPMT quantum efficiency
EMRnadcPerPeMAPMT = 6 # number of ADC counts per photoelectron in the MAPMT
EMRelectronicsResponseSpreadMAPMT = 8 # ADC counts
EMRtimeResponseSpread = 1 # ADC counts
EMRtotFuncP1 = -60.5 
EMRtotFuncP2 = 15.0
EMRtotFuncP3 = 70.0
EMRtotFuncP4 = 2.0 # time over threshold vs charge logarithmic fit parameters
EMRdeltatShift = 12 # ADC counts, distance from the trigger

EMRfadcCount = 2.0 # ns, duration of an fADC cycle (f=500MHz)
EMRqeSAPMT = 0.11 # SAPMT quantum efficiency
EMRnadcPerPeSAPMT = 2 # number of ADC counts per photoelectron in the SAPMT
EMRelectronicsResponseSpreadSAPMT = 1 # ADC count
EMRbaselinePosition = 123 # SAPMT signal baseline
EMRbaselineSpread = 10 # SAPMT signal baseline spread
EMRmaximumNoiseLevel = 50 # SAPMT noise maximum value
EMRacquisitionWindow = 302 # ADC counts
EMRsignalIntegrationWindow = 40
EMRarrivalTimeShift = 40
EMRarrivalTimeSpread = 33
EMRarrivalTimeGausWidth = 3
EMRarrivalTimeUniformWidth = 12.5
EMRpulseShapeLandauWidth = 2

# EMR reconstruction
EMRsecondaryHitsBunchingWidth = 10 # ADC

EMRprimaryTriggerMinXhits = 1
EMRprimaryTriggerMinYhits = 1
EMRprimaryTriggerMinNhits = 4
EMRsecondaryTriggerMinXhits = 1
EMRsecondaryTriggerMinYhits = 1
EMRsecondaryTriggerMinNhits = 2
EMRsecondaryTriggerMinTot = 4

EMRmaxSecondaryToPrimaryTrackDistance = 100

# this is used by the reconstuction of the TOF detectors
TOF_trigger_station = "tof1"

# this sets the source for the calibrations
# by default it is from CDB
# set it to 'file' if you want to load local files
# if you set file, then uncomment the calib files below
TOF_calib_source = "CDB"

TOF_cabling_file = "/files/cabling/TOFChannelMap.txt"
#TOF_TW_calibration_file = "/files/calibration/tofcalibTW_dec2011.txt"
#TOF_T0_calibration_file = "/files/calibration/tofcalibT0_trTOF1_dec2011.txt"
#TOF_T0_calibration_file = "/files/calibration/tofcalibT0_trTOF0.txt"
#TOF_Trigger_calibration_file = "/files/calibration/tofcalibTrigger_trTOF1_dec2011.txt"
#TOF_Trigger_calibration_file = "/files/calibration/tofcalibTrigger_trTOF0.txt"

TOF_findTriggerPixelCut = 0.5 # nanosecond
TOF_makeSpacePointCut = 0.5 # nanosecond

# get calibrations by either a) run_number or b) date
# default is by run_number
# if set to "date" then set the appropriate TOF_calib_date_from flag below
TOF_calib_by = "run_number"

# the date for which we want the cabling and calibration
# date can be 'current' or a date in YYYY-MM-DD hh:mm:ss format
TOF_calib_date_from = 'current'
TOF_cabling_date_from = 'current'

Enable_timeWalk_correction = True
Enable_triggerDelay_correction = True
Enable_t0_correction = True

# this is used by the reconstuction of the KL detectors
# this sets the source for the calibrations
# by default it is from CDB
# set it to 'file' if you want to load local files
# if you set file, then uncomment the calib files below
KL_calib_source = "CDB"
KL_calib_date_from = 'current'
# uncomment the KL_calibration_file card below if you set KL_calib_source=file
#KL_calibration_file = "/files/calibration/klcalib.txt"

KL_cabling_file = "/files/cabling/KLChannelMap.txt"

Enable_klgain_correction = True

# this is used by the reconstuction of the EMR detectors
EMR_cabling_file = "/files/cabling/EMRChannelMap.txt"

# this sets the source of the EMR clear fiber length map
EMR_clear_fiber_length_map = "/files/cabling/EMRClearFiberLengthMap.txt"

# this sets the source of the EMR connector attenuation map
EMR_connector_attenuation_map = "/files/cabling/EMRConnectorAttenuationMap.txt"

# this sets the source of the calibrations for the EMR detector
EMR_calib_source = "CDB"
EMR_calib_date_from = 'current'
# uncomment the EMR_calibration_file card below if you set EMR_calib_source=file
#EMR_calib_file = "/files/calibration/emrcalib_cosmics_march2014.txt"

daq_data_path = '%s/src/input/InputCppDAQData' % os.environ.get("MAUS_ROOT_DIR") # path to daq data. Multiple locations can be specified with a space
daq_data_file = '05466.001' # file name for daq data; if this is just a integer string, MAUS assumes this is a run number. Multiple entries can be specified separated by a space

maus_version = "" # set at runtime - do not edit this (changes are ignored)
configuration_file = "" # should be set on the command line only (else ignored)

doc_store_class = "docstore.MongoDBDocumentStore.MongoDBDocumentStore"
doc_collection_name = "spills" # Default document collection name. Only needed if using multi_process mode. If "auto" then a collection name will be auto-generated for spills output by input-transform workflows.
doc_store_event_cache_size = 10**8 # Maximum size of the Mongo cache to cache at any one time in multiprocessing mode, as used by e.g. online code. Corresponds to ~ n/3 spills.

mongodb_host = "localhost" # Default MongoDB host name. Only needed if using MongoDBDocumentStore.
mongodb_port = 27017 # Default MongoDB port. Only needed if using MongoDBDocumentStore.
mongodb_database_name = "mausdb" # Default MongoDB database name. Only needed if using MongoDBDocumentStore.
mongodb_collection_name = "spills" # Default MongoDB collection name. Only needed if using MongoDBDocucmentStore.

# in multiprocessing mode, the timeout after which reconstruction of an event will be abandonded [s]
reconstruction_timeout = 10
# refresh rate for refreshing plots
reduce_plot_refresh_rate = 5
# Default OutputPyImage image directory. MAUS web application directory.
image_directory = os.environ.get("MAUS_WEB_MEDIA_RAW") if (os.environ.get("MAUS_WEB_MEDIA_RAW") != None) else os.getcwd()
# Default OutputPyImage image directory for end of run data. Will end up as image_directory+"/end_of_run/"
end_of_run_image_directory = ''
# Default OutputPyFile output directory. MAUS web application directory.
output_file_directory = os.environ.get("MAUS_WEB_MEDIA_RAW") if (os.environ.get("MAUS_WEB_MEDIA_RAW") != None) else os.getcwd()

PolynomialOpticsModel_order = 1
PolynomialOpticsModel_algorithms = ["LeastSquares",
                    "ConstrainedLeastSquares", "ConstrainedChiSquared",
                    "SweepingChiSquared", "SweepingChiSquaredWithVariableWalls"]
PolynomialOpticsModel_algorithm = "LeastSquares"
# deltas for numerical derivative calculation of Optics transfer maps
TransferMapOpticsModel_Deltas = {"t":0.01, "E":0.1,
                                 "x":0.1, "Px":0.1,
                                 "y":0.1, "Py":0.01}

# Default location of root file containing PDF histograms used for Global PID
PID_PDFs_file =  '%s/src/map/MapCppGlobalPID/PIDhists.root' % os.environ.get("MAUS_ROOT_DIR")
# Particle hypothesis used in Global PID when creating PDFs from MC data.
# For PDFs to be produced, this must be set, preferably as the type of simulated particle
# i.e. for a simulation of 200MeV/c muons, set flag to "200MeV_mu_plus"
global_pid_hypothesis = ""
# Unique identifier used when creating PDFs in Global PID to distinguish between PDFs for
# the same hypothesis generated at different times. For PDFs to be produced, this must be set.
# Any string can be used but date and time is recommended, by using python datetime module and
# the line unique_identifier = (datetime.datetime.now()).strftime("%Y_%m_%dT%H_%M_%S_%f")
unique_identifier = ""

geometry_validation = { # see bin/utilities/geometry_validation.py for docs
    "file_name":os.path.expandvars("${MAUS_TMP_DIR}/geometry_validation.json"),
    "will_plot":True,
    "will_track":True,
    "z_start":-6000.,
    "z_end":6000.,
    "x_start":0.,
    "x_step":1.,
    "y_start":0.,
    "y_step":0.,
    "n_steps":301,
    "plot_formats":["root", "png"],
    "1d_material_plot":os.path.expandvars("${MAUS_TMP_DIR}/geometry_validation_materials_1d"),
    "2d_material_plot":os.path.expandvars("${MAUS_TMP_DIR}/geometry_validation_materials_2d"),
    "1d_volume_plot":os.path.expandvars("${MAUS_TMP_DIR}/geometry_validation_volumes_1d"),
    "2d_volume_plot":os.path.expandvars("${MAUS_TMP_DIR}/geometry_validation_volumes_2d"),
    "2d_volume_plot_label_size":0.25,
}
