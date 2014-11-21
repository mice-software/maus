##  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
Configuration to generate a beam distribution with amplitude-momentum
correlation
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

ref_p = 100. # update momentum in test script as well
mass = 105.658
energy = (ref_p**2+mass**2)**0.5
spill_generator_number_of_spills = 100
verbose_level = 1
beam = {
    "particle_generator":"counter",
    "random_seed":6,
    "definitions":[
    ##### MUONS #######
    {
       "random_seed_algorithm":"incrementing_random", # algorithm for seeding MC
       "n_particles_per_spill":10,
       "reference":{
            "position":{"x":500.0, "y":0.0, "z":3.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "spin":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-13,
            "energy":energy,
            "time":2.e6,
            "random_seed":0
        }, # reference particle
       "transverse":{
            "transverse_mode":"penn", 
            "emittance_4d":1., 
            "beta_4d":333.,
            "alpha_4d":1., 
            "normalised_angular_momentum":2.,
            "bz":4.e-3
        },
        "longitudinal":{
            "longitudinal_mode":"pencil",
            "momentum_variable":"pz",
        },
        "coupling":{"coupling_mode":"none"},
        "a-p_correlation":{
             "momentum_variable":"pz",
             "magnitude":0.001,
        },
    }]
}
