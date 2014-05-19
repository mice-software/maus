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
Configuration to generate a beam distribution with negative energy - algorithm
should never make a nan
"""

#pylint: disable = C0103, R0801

import os
mrd = os.environ["MAUS_ROOT_DIR"]

simulation_geometry_filename = os.path.join(
  mrd, "tests", "integration", "test_simulation", "test_beam_maker", 
  "BeamTest.dat"
)
output_root_file_name = os.path.join(mrd, "tmp", "test_beammaker_output.root")
input_root_file_name = output_root_file_name # for conversion

spill_generator_number_of_spills = 1
verbose_level = 1
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
            "spin":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-13,
            "energy":105.659, # ~ muon mass
            "time":2.e6,
            "random_seed":0
        }, # reference particle
       "random_seed_algorithm":"incrementing_random", # algorithm for seeding MC
       "n_particles_per_spill":1000, # roughly half would have negative energy
       "transverse":{
            "transverse_mode":"pencil", 
        },
        "longitudinal":{
            "longitudinal_mode":"sawtooth_time",
                   "momentum_variable":"energy",
                   "sigma_energy":25.,
                   "t_start":0.,
                   "t_end":+1.},
       "coupling":{"coupling_mode":"none"}
    }]
}
