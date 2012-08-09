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
Configuration to generate a beam distribution with binomial distribution in
the spill and various distributions for difference particle types
"""

#pylint: disable = C0103, R0801

import os
mrd = os.environ["MAUS_ROOT_DIR"]

simulation_geometry_filename = os.path.join(
  mrd, "tests", "integration", "test_simulation", "test_physics_model_brief", 
  "PhysicsModelTest.dat"
)
spill_generator_number_of_spills = 2
verbose_level = 0
keep_tracks = True

# energy is very low - so that particles probably decay if they are unstable
simulation_reference_particle = {
    "position":{"x":0.0, "y":-0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":211, "energy":140.0, "time":0.0, "random_seed":0
}

beam = {
    "particle_generator":"counter",
    "random_seed":0,
    "definitions":[
    {
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":0.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":211,
            "energy":300.0,
            "time":0.,
            "random_seed":0
        },
       "n_particles_per_spill":1,
       "random_seed_algorithm":"incrementing_random",
       "transverse":{"transverse_mode":"pencil"},
        "longitudinal":{"longitudinal_mode":"pencil","momentum_variable":"p"},
       "coupling":{"coupling_mode":"none"}
    },
    {
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":0.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-13,
            "energy":226.0,
            "time":0.,
            "random_seed":0
        },
       "n_particles_per_spill":1,
       "random_seed_algorithm":"incrementing_random",
       "transverse":{"transverse_mode":"pencil"},
        "longitudinal":{"longitudinal_mode":"pencil","momentum_variable":"p"},
       "coupling":{"coupling_mode":"none"}
    },
    ],
}
