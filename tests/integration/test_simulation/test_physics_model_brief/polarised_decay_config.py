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
output_root_file_name = \
            os.path.expandvars("${MAUS_ROOT_DIR}/tmp/test_polarised_decay.root")
spill_generator_number_of_spills = 10
verbose_level = 0
keep_tracks = True
polarised_decay = True
physics_processes = "mean_energy_loss"

simulation_geometry_filename = os.path.expandvars("${MAUS_ROOT_DIR}/tests/"+\
      "integration/test_simulation/test_physics_model_brief/"+\
      "PolarisedDecayGeometry.dat")
simulation_reference_particle = {
    "position":{"x":0.0, "y":-0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":13, "energy":200.0, "time":0.0, "random_seed":0
}

production_threshold = 1e9
muon_half_life = 1./300. # ct = 1 mm

beam = {
    "particle_generator":"counter",
    "random_seed":0,
    "definitions":[{
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":0.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":2212,
            "energy":2000.0,
            "time":0.,
            "random_seed":0
        },
       "n_particles_per_spill":1,
       "random_seed_algorithm":"incrementing_random",
       "transverse":{"transverse_mode":"pencil"},
        "longitudinal":{"longitudinal_mode":"pencil","momentum_variable":"p"},
       "coupling":{"coupling_mode":"none"}
    }, {
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":0.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":13,
            "energy":106.0,
            "time":0.,
            "random_seed":0
        },
       "n_particles_per_spill":100,
       "random_seed_algorithm":"incrementing_random",
       "transverse":{"transverse_mode":"pencil"},
        "longitudinal":{"longitudinal_mode":"pencil","momentum_variable":"p"},
       "coupling":{"coupling_mode":"none"},
       "beam_polarisation":{
           "polarisation_mode":"gaussian_unit_vectors",
           "beam_mean_x":0.,
           "beam_mean_y":0.,
           "beam_mean_z":1.e6,
           "beam_sigma_x":1.,
           "beam_sigma_y":1.,
           "beam_sigma_z":1.,
       }
    }, {
       "reference":{
            "position":{"x":0.0, "y":0.0, "z":0.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-13,
            "energy":106.0,
            "time":0.,
            "random_seed":0
        },
       "n_particles_per_spill":100,
       "random_seed_algorithm":"incrementing_random",
       "transverse":{"transverse_mode":"pencil"},
        "longitudinal":{"longitudinal_mode":"pencil","momentum_variable":"p"},
       "coupling":{"coupling_mode":"none"},
       "beam_polarisation":{
           "polarisation_mode":"gaussian_unit_vectors",
           "beam_mean_x":0.,
           "beam_mean_y":0.,
           "beam_mean_z":-1.e6,
           "beam_sigma_x":1.,
           "beam_sigma_y":1.,
           "beam_sigma_z":1.,
       }
    }
    ],
}
