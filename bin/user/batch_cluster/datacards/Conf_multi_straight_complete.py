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

mrd = os.getenv("MAUS_ROOT_DIR")
work_dir = mrd + "/bin/user/batch_cluster/"
geometry_dir = work_dir + "geometry/"
beams_dir = work_dir + "beams/"

simulation_geometry_filename = geometry_dir + "Kalman_Test_noField.dat"
geometry_use_active_rotations = True

task_id = int( os.getenv( "SGE_TASK_ID", 0 ) )
file_id = str( task_id ).zfill( 3 )
input_filename = beams_dir + "test_beams/uniform_momentum_zero_position_straights/uniform_1000_"+file_id+".dat"

random_seed_beam = int(os.getenv( "MAUS_RANDOM_SEED", 9 ))
random_seed_reference = 2 * random_seed_beam

z_start_pos = 0.0

simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":0.0, "z":z_start_pos},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13, "energy":226.2, "time":0.0, "random_seed":random_seed_reference
}

# geant4 visualisation (not event display)
geant4_visualisation = False
accumulate_tracks = 0


spill_generator_number_of_spills = 100

beam = {
    "particle_generator":"file",
    "random_seed":random_seed_beam,
    "beam_file_format" : "icool_for003",
    "beam_file" : input_filename,
    "file_particles_per_spill" : 1,
    "definitions":[ {
       "reference":simulation_reference_particle,
       "random_seed_algorithm":"incrementing_random"
    } ]
}

#physics_processes = "none"
physics_processes = "standard"
#physics_processes = "mean_energy_loss"
particle_decay = False

SciFiBadChannelsFileName = ""

SciFiClusterReconOn = True
SciFiSpacepointReconOn = True

SciFiKalman_use_MCS = True # flag to add MCS to the Kalman Fit

SciFiSeedCovariance = 10000.0
SciFiSeedPatRec = True


