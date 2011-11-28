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

"""
Configuration to generate a beam distribution with default values - just to
do sanity check that we generate a beam
"""

#pylint: disable = C0103, R0801

import os
verbose_level = 1

simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":-0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":0
}

beam = {
    "particle_generator":"counter", # routine for generating empty primaries
    "random_seed":0, # random seed for beam generation; controls also how the MC
                     # seeds are generated
    "definitions":[
    ##### MUONS #######
    {
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":3.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":0,
            "energy":226.0,
            "time":2.e6,
            "random_seed":0
        }, # reference particle
       "n_particles_per_spill":1,
       "random_seed_algorithm":"incrementing_random", # algorithm for seeding MC
       "weight":90., # probability of generating a particle
       "transverse":{
            "transverse_mode":"pencil", 
        },
        "longitudinal":{
            "longitudinal_mode":"pencil",
            "momentum_variable":"p"
        },
       "coupling":{"coupling_mode":"none"}
    }]
}

spill_generator_number_of_spills = 1
output_json_file_name = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", \
                                   "test_makes_error_messages_simulation_2.out")

