#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
Configuration to generate a beam distribution with binomial distribution in
the spill and various distributions for difference particle types
"""

#pylint: disable = C0103

import os

output_root_file_name = os.path.expandvars(
            "${MAUS_ROOT_DIR}/tmp/test_production_threshold.root"
)
physics_processes = "standard"
spill_generator_number_of_spills = 1
simulation_geometry_filename = os.path.expandvars("${MAUS_ROOT_DIR}/tests/"+\
      "integration/test_simulation/test_physics_model_brief/"+\
      "ProductionThresholdTest.dat")

keep_steps = True
verbose_level = 0
production_threshold = 1e12
fine_grained_production_threshold = {
# makes electrons, no gammas, default for positrons
"Threshold":{"default":-1,
             "e+":0.01,
             "e-":0.01,
             "gamma":0.01},
"ThresholdInt":{"default":-1,
             "11":0.01,
             "-11":0.01,
             "22":0.01},
# defaults
"NoThreshold":{},
# G4 defaults
"NegativeCut":{"default":-1},
# Region not defined
"NoRegion":{"default":-1},
"KL1":{"default":1e12},
"KL2":{"default":-1},
}

definitions = [{
       "reference":{
          "position":{"x":x_position, "y":-0.0, "z":0.0},
          "momentum":{"x":0.0, "y":0.0, "z":1.0},
          "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10,
       },
       "random_seed_algorithm":"beam_seed",
       "n_particles_per_spill":1,
       "transverse":{
          "transverse_mode":"pencil",
       },
       "longitudinal":{"longitudinal_mode":"pencil",
                   "momentum_variable":"p",},
       "coupling":{"coupling_mode":"none"}
} for x_position in [3500.0, 2500.0, 1500.0, 1000.0, 500.,
                     0.0, -500., -1000., -1500.]]

beam = {
    "particle_generator":"counter",
    "random_seed":0,
    "definitions":definitions
}

