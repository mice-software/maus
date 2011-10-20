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

## The Configuration defaults
#  These are the default values for MAUS and its components.
#  Please DO NOT CHANGE this unless you want to change this
#  quantity for everybody.  If you want to fiddle, then
#  please load your own overriding configuration by
#  consulting the documentation.
#
import os

type_of_dataflow = 'pipeline_single_thread'

output_json_file_name = "simulation.out"
output_json_file_type = "text"


# Used, for now, to determine what level of c++ log messages are reported to the user
# 0 = debug info (and std::cout)
# 1 = run info
# 2 = warnings
# 3 = errors (and std::cerr)
# 4 = fatal
# >4 = silent
# Doesnt effect python
verbose_level = 4
errors_to_stderr = None # None = from verbose_level; else True or False
errors_to_json = True
on_error = 'none' # none, halt or raise

# Used by MapPyRemoveTracks.
keep_only_muon_tracks = False

# Used by MapCppSimulation
keep_tracks = False # set to true to keep start and end point of every track
keep_steps = False # set to true to keep start and end point of every track and
                   # every step point
simulation_geometry_filename = "Stage6.dat" # geometry used by simulation
maximum_number_of_steps = 10000
simulation_reference_particle = {
    "position":{"x":0.0, "y":-0.0, "z":-5500.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
}

# geant4 visualisation (not event display)
geant4_visualisation = False
visualisation_viewer = "VRML2FILE"  # only supported option
visualisation_theta = 90.
visualisation_phi = 90.
visualisation_zoom = 1.

# used by InputPySpillGenerator to determine the number of empty spills that
# will be generated by the simulation
spill_generator_number_of_spills = 10
# used by MapPyBeamMaker to generate input particle data
# This is a sample beam distribution based on guesses by Chris Rogers of what
# an optimised beam might look like
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
           "position":{"x":0.0, "y":-0.0, "z":-5500.0},
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

# this is used by reconstruction
reconstruction_geometry_filename = simulation_geometry_filename

# scifi tracker digitization
#SciFiDeadChanFName = ""
SciFiMUXNum = 7
SciFiFiberDecayConst = 2.7
SciFiFiberConvFactor =  3047.1
SciFiFiberRefractiveIndex = 1.6
SciFiFiberCriticalAngle = 0.4782 # rad
SciFiFiberTrappingEff = 0.056
SciFiFiberMirrorEff = 0.6
SciFiFiberTransmissionEff = 0.8
SciFiMUXTransmissionEff = 1.0
SciFiFiberRunLength = 5000.0 # mm
SciFivlpcQE = 0.8
SciFivlpcEnergyRes = 4.0 # MeV
SciFivlpcTimeRes = 0.2 # ns
SciFiadcBits = 8
SciFiadcFactor = 6.0
SciFitdcBits = 16
SciFitdcFactor = 1.0
SciFinElecChanPerPlane = 1429
SciFinPlanes = 3
SciFinStations = 5
SciFinTrackers = 2

# this is used by ImputCppRealData
Number_of_DAQ_Events = -1
Enable_V1290_Unpacking = True
Enable_V1731_Unpacking = True
Enable_V1724_Unpacking = True
Enable_V830_Unpacking = True
Enable_VLSB_Unpacking = True
Enable_DBB_Unpacking = True
Do_V1731_Zero_Suppression = False
V1731_Zero_Suppression_Threshold = 100
Do_V1724_Zero_Suppression = True
V1724_Zero_Suppression_Threshold = 100
Enable_TOF = True
Enable_EMR = True
Enable_KL = True
Enable_CKOV = True
DAQ_cabling_file = "/files/cabling/DAQChannelMap.txt"

# tof digitization
TOFconversionFactor = 0.01 # MeV
TOFpmtTimeResolution = 0.1 # nanosecond
TOFattenuationLength = 140 * 10 # mm
TOFadcConversionFactor = 0.125
TOFtdcConversionFactor = 0.025 # nanosecond
TOFpmtQuantumEfficiency = 0.25
TOFscintLightSpeed =  170.0 # mm/ns

# this is used by the reconstuction of the TOF detectors
TOF_trigger_station = "tof1"
#TOF_trigger_station = "tof0"
TOF_cabling_file = "/files/cabling/TOFChannelMap.txt"
TOF_TW_calibration_file = "/files/calibration/tofcalibTW.txt"
TOF_T0_calibration_file = "/files/calibration/tofcalibT0_trTOF1.txt"
#TOF_T0_calibration_file = "/files/calibration/tofcalibT0_trTOF0.txt"
TOF_Trigger_calibration_file = "/files/calibration/tofcalibTrigger_trTOF1.txt"
#TOF_Trigger_calibration_file = "/files/calibration/tofcalibTrigger_trTOF0.txt"
TOF_findTriggerPixelCut = 0.5 # nanosecond
TOF_makeSpacePiontCut = 0.5 # nanosecond
Enable_timeWalk_correction = True
Enable_triggerDelay_correction = True
Enable_t0_correction = True

daq_data_path = '%s/src/input/InputCppDAQData' % os.environ.get("MAUS_ROOT_DIR") # path to daq data
daq_data_file = '02873.003' # file name for daq data

maus_version = "" # set at runtime - do not edit this (changes are ignored)
configuration_file = "" # should be set on the command line only (else ignored)