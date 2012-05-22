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
verbose_level=0
particle_decay=False

# We need enough spills that we have a good chance of having the single-muon
# beam make it through the entire lattice
spill_generator_number_of_spills=50

beam = {
    "particle_generator":"counter",
    "random_seed":5, # random seed for beam generation; controls also how the MC
                     # seeds are generated
    "definitions":[
    ##### MUONS #######
    {
       "n_particles_per_spill":1,
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

# Additional global track reconstruction settings
data_acquisition_modes = ['Random', 'Testing', 'Simulation', 'Live']
data_acquisition_mode = 'Random'
testing_data = {
  "beam_polarity_negative":False,
  "detectors":[
  {
    "id":1,  # TOF0
    "plane":1000.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":3,  # TOF1
    "plane":2000.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":4,  # Tracker 1, Station 1
    "plane":5000.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":5,  # Tracker 1, Station 2
    "plane":5100.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":6,  # Tracker 1, Station 3
    "plane":5200.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":7,  # Tracker 1, Station 4
    "plane":5300.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":8,  # Tracker 1, Station 5
    "plane":5400.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":9,  # Tracker 2, Station 1
    "plane":5000.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":10,  # Tracker 2, Station 2
    "plane":5100.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":11,  # Tracker 2, Station 3
    "plane":5200.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":12,  # Tracker 2, Station 4
    "plane":5300.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  },
  {
    "id":13,  # Tracker 2, Station 5
    "plane":5400.0,
    "uncertainties":[
      [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
      [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
    ]
  }],
  "mc_events": [
    {
      "sci_fi_hits": [
        {
          "energy_deposited": 0.03918918457426493, 
          "energy": 182.0342047369771, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "fibre_number": 712, 
            "station_number": 2, 
            "tracker_number": 0, 
            "plane_number": 2
          }, 
          "charge": 1.0, 
          "time": -443316.0186282694, 
          "position": {
            "y": -7.674034607450571, 
            "x": 3.27451965619477, 
            "z": -4850.464392707308
          }, 
          "momentum": {
            "y": -18.62521565507426, 
            "x": -15.45772528416643, 
            "z": 146.2426554549149
          }
        }, 
        {
          "energy_deposited": 0.03064835765900547, 
          "energy": 183.6643610171762, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "fibre_number": 682, 
            "station_number": 5, 
            "tracker_number": 0, 
            "plane_number": 1
          }, 
          "charge": 1.0, 
          "time": -443319.7446595106, 
          "position": {
            "y": 12.75970606446028, 
            "x": 6.433703616514025, 
            "z": -5749.651039475932
          }, 
          "momentum": {
            "y": -23.81917122882773, 
            "x": 4.894642146686818, 
            "z": 148.2484049746077
          }
        }, 
        {
          "energy_deposited": 0.08601753488412993, 
          "energy": 148.6012011943035, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "fibre_number": 902, 
            "station_number": 5, 
            "tracker_number": 1, 
            "plane_number": 1
          }, 
          "charge": 1.0, 
          "time": -443268.753253904, 
          "position": {
            "y": -28.40004408119435, 
            "x": -16.06689957138289, 
            "z": 5749.25668439525
          }, 
          "momentum": {
            "y": -14.01733094192246, 
            "x": 0.2536523698226235, 
            "z": 103.547426812885
          }
        }, 
      ], 
      "tof_hits": [
        {
          "energy_deposited": 0.0, 
          "energy": 217.8755315031993, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": 1.0, 
          "time": -443352.7557241592, 
          "position": {
            "y": 2.518175782592233, 
            "x": 20.14702737084145, 
            "z": -14207.55
          }, 
          "momentum": {
            "y": -0.8930180376107749, 
            "x": 3.497512306746812, 
            "z": 190.507269319809
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 195.2892364489483, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "station_number": 1, 
            "plane": 0, 
            "slab": 3
          }, 
          "charge": 1.0, 
          "time": -443322.3189645602, 
          "position": {
            "y": 13.43182178637196, 
            "x": -62.3024883003872, 
            "z": -6383.45
          }, 
          "momentum": {
            "y": -0.2399029701777772, 
            "x": 13.23640123138932, 
            "z": 163.7037828251376
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 147.5271776652669, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "station_number": 2, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": 1.0, 
          "time": -443265.0273640204, 
          "position": {
            "y": -40.85535285342303, 
            "x": -63.80219479320501, 
            "z": 6524.5
          }, 
          "momentum": {
            "y": -10.9664799994799, 
            "x": -15.25040867907277, 
            "z": 101.231082236937
          }
        } 
      ], 
      "primary": {
        "random_seed": 953453439, 
        "energy": 222.8506406695172, 
        "particle_id": -13, 
        "time": -443355.759630607, 
        "position": {
          "y": -14.73099106842985, 
          "x": 3.343270739829844, 
          "z": -15000.0
        }, 
        "momentum": {
          "y": 3.927840738970513, 
          "x": 4.024390370816589, 
          "z": 196.1302983254844
        }
      }
    }
  ]
}

reconstruction_track_fitters = ['Minuit', 'Kalman Filter', 'TOF']
reconstruction_track_fitter = 'Minuit'

reconstruction_optics_models = ['Differentiating', 'Integrating',
                                'Least Squares', 'Runge Kutta',
                                'Linear Approximation']
reconstruction_optics_model = 'Linear Approximation'

LeastSquaresOpticsModel_algorithms = ['Unconstrained Polynomial',
                                      'Constrained Polynomial',
                                      'Constrained Chi Squared',
                                      'Sweeping Chi Squared',
                                      'Sweeping Chi Squared Variable Walls']
LeastSquaresOpticsModel_algorithm = 'Unconstrained Polynomial'

LeastSquaresOpticsModel_Delta_t  =  1  # ns
LeastSquaresOpticsModel_Delta_E  =  1  # MeV
LeastSquaresOpticsModel_Delta_x  =  1  # mm
LeastSquaresOpticsModel_Delta_Px =  1  # MeV
LeastSquaresOpticsModel_Delta_y  =  1  # mm
LeastSquaresOpticsModel_Delta_Py =  1  # MeV
