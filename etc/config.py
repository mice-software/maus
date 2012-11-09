"""
Configuration for Global Reconstruction
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

import os

# Non-default settings

# geometry used by simulation
#simulation_geometry_filename = "Stage4.dat"
#simulation_geometry_filename = "/home/lane/Development/maus/issue_854/files/geometry/download/90D_rotated_geometry.dat"
simulation_geometry_filename = "SingleStation.dat"

geant4_visualisation = True
verbose_level=0
particle_decay=False
accumulate_tracks = 1

# We need enough spills that we have a good chance of having the single-muon
# beam make it through the entire lattice
spill_generator_number_of_spills=1

simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":-0.0, "z":-15450.0},
    "momentum":{"x":0.0, "y":0.0, "z":200.0},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
}
"""
simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":0.0, "z":100.0},
    "momentum":{"x":0.0, "y":0.0, "z":200.0},
    "particle_id":-13, "energy":226.2, "time":0.0, "random_seed":10
}
"""

beam = {
    "particle_generator":"counter",
    "random_seed":5, # random seed for beam generation; controls also how the MC
                     # seeds are generated
    "definitions":[
    ##### MUONS #######
    {
       "n_particles_per_spill":500,
       "reference":simulation_reference_particle, # reference particle
       "random_seed_algorithm":"incrementing_random", # algorithm for seeding MC
       "weight":90., # probability of generating a particle
       "transverse":{
          "transverse_mode":"twiss", # transverse distribution asymmetric in x and y and uncoupled
          "beta_x":1000., # 1000 mm beta function
          "alpha_x":0.5, # beam converging (into Q7-9)
          "emittance_x":0.5, # smallish emittance
          "beta_y":1000., # 1000 mmm beta function
          "alpha_y":0.5, # beam converging (into Q7-9)
          "emittance_y":0.5, # smallish emittance
       },
       "longitudinal":{"longitudinal_mode":"sawtooth_time", # longitudinal distribution sawtooth in time
                   "momentum_variable":"p", # Gaussian in total momentum (options energy, pz)
                   "sigma_p":25., # RMS total momentum
                   "t_start":-1.e6, # start time of sawtooth
                   "t_end":+1.e6}, # end time of sawtooth
       "coupling":{"coupling_mode":"none"} # no dispersion
    }]
}
