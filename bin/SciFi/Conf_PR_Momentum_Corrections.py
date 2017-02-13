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

output_root_file_name = "maus_output_PR_momentum_corrections.root"

simulation_geometry_filename = "bin/SciFi/SciFi_Test_Geometry.dat"
geometry_use_active_rotations = True


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


spill_generator_number_of_spills = 5000

beam={
  "particle_generator":"counter",
  "random_seed":random_seed_beam,
  "definitions":[ {
    "reference":simulation_reference_particle,
    "random_seed_algorithm":"incrementing_random",
    "n_particles_per_spill":10,
    "transverse":{
        "transverse_mode":"uniform",
#        "range_radius":1.0,
        "fit_solenoid":4.0e-3,
        "range_pt":180.0
    },
    "longitudinal":{
        "longitudinal_mode":"uniform",
        "range_p":[130.0, 250.0],
        "momentum_variable":"pz"
    },
    "coupling":{"coupling_mode":"none"},
  } ]
}




physics_processes = "standard"

particle_decay = False
