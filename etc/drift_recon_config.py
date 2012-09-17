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

# Geometry
simulation_geometry_filename = "Drift.dat" # geometry used by simulation

# geant4_visualisation = True
verbose_level=0
physics_processes = "mean_energy_loss"
particle_decay=False

# We need enough spills that we have a good chance of having the single-muon
# beam make it through the entire lattice
spill_generator_number_of_spills=1

simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":-0.0, "z":-15450.0},
    "momentum":{"x":0.0, "y":0.0, "z":200.0},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
}


# Additional global track reconstruction settings
data_acquisition_modes = ['Random', 'Testing',
                          'Simulation', 'Smeared', 'Live']
data_acquisition_mode = 'Simulation'

reconstruction_track_fitters = ['Minuit', 'Kalman Filter', 'TOF']
reconstruction_track_fitter = 'Minuit'

reconstruction_minuit_max_iterations = 100000
reconstruction_minuit_parameters = [
  {"name":"Time", "fixed":False, "initial_value":-729015.584310913,
   "value_step":0.1, "min_value":-729016., "max_value":729016.}, # ns
  {"name":"Energy", "fixed":False, "initial_value":228.3856922541968,
   "value_step":1., "min_value":105.7, "max_value":500.}, # MeV
  {"name":"x", "fixed":False, "initial_value":-0.5887174378015934,
   "value_step":0.001, "min_value":-150., "max_value":150.}, # mm
  {"name":"Px", "fixed":False, "initial_value":0.1701804010609224,
   "value_step":0.1, "min_value":-100., "max_value":100.}, # MeV/c
  {"name":"y", "fixed":False, "initial_value":0.7554039583923567,
   "value_step":0.001, "min_value":-150., "max_value":150.}, # mm
  {"name":"Py", "fixed":False, "initial_value":-1.018600216855729,
   "value_step":0.1, "min_value":-100., "max_value":100.} # MeV/c
]

reconstruction_optics_models = ['Differentiating', 'Integrating',
                                'Polynomial', 'Runge Kutta',
                                'Linear Approximation']
#reconstruction_optics_model = 'Linear Approximation'
reconstruction_optics_model = 'Polynomial'

PolynomialOpticsModel_algorithms = ['Least Squares',
                                      'Constrained Least Squares',
                                      'Constrained Chi Squared',
                                      'Sweeping Chi Squared',
                                      'Sweeping Chi Squared Variable Walls']
PolynomialOpticsModel_algorithm = 'Least Squares'
PolynomialOpticsModel_order = 2

TransferMapOpticsModel_EllipseDefinition = {
    "Emittance_T": 10.0,  # mm
    "Emittance_L": 0.01,  # ns
    "Beta_T":      650.,  # mm
    "Beta_L":      5.,    # ns
    "Alpha_T":     0.,
    "Alpha_L":     0.,
    "NormalisedAngularMomentum": 0.,
    "Bz":             0., # T
}

TransferMapOpticsModel_Deltas = {
  "t":1,  # ns
  "E":1,  # MeV
  "x":1,  # mm
  "Px":1, # MeV
  "y":1,  # mm
  "Py":1  # MeV
}

testing_data = {
  "detectors":[
  {
    "id":1,  # TOF0, Station 1
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
    "id":2,  # TOF0, Station 2
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
    "id":3,  # Cherenkov1
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
    "id":4,  # TOF1, Station 1
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
    "id":5,  # TOF1, Station 2
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
    "id":6,  # Tracker 1, Station 1
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
    "id":7,  # Tracker 1, Station 2
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
    "id":8,  # Tracker 1, Station 3
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
    "id":9,  # Tracker 1, Station 4
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
    "id":10,  # Tracker 1, Station 5
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
    "id":11,  # Tracker 2, Station 1
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
    "id":12,  # Tracker 2, Station 2
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
    "id":13,  # Tracker 2, Station 3
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
    "id":14,  # Tracker 2, Station 4
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
    "id":15,  # Tracker 2, Station 5
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
    "id":16,  # TOF 2, Station 1
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
    "id":17,  # TOF 2, Station 2
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
    "id":18,  # Cherenkov 2
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
    "id":18,  # Calorimeter
    "plane":5300.0,
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
          "energy_deposited": 0.1729183541526194, 
          "energy": 227.34151249655906, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "fibre_number": 627, 
            "station_number": 1, 
            "tracker_number": 0, 
            "plane_number": 2
          }, 
          "charge": 1.0, 
          "time": -729006.3629563523, 
          "position": {
            "y": -11.570051786165, 
            "x": 1.470531094799617, 
            "z": -13000.0
          }, 
          "momentum": {
            "y": -1.015379233040514, 
            "x": 0.1696422622421657, 
            "z": 201.8326277345766
          }
        }
      ],
      "special_virtual_hits": [
        {
          "energy_deposited": 0.0, 
          "energy": 228.1051321719965, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "station": 0
          }, 
          "charge": 1.0, 
          "time": -729010.9085602828, 
          "position": {
            "y": 0.07936605103162224, 
            "x": -14.11099981232446, 
            "z": -14207.55
          }, 
          "momentum": {
            "y": -0.1143051846570663, 
            "x": -2.743111298792011, 
            "z": 202.140337655232
          }
        }, 
        {
          "energy_deposited": 3.034031766351256e-05, 
          "energy": 216.2805722790105, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "station": 0
          }, 
          "charge": 1.0, 
          "time": -729010.7194486645, 
          "position": {
            "y": -0.002788759941515366, 
            "x": -15.03391041424933, 
            "z": -14157.54995
          }, 
          "momentum": {
            "y": -5.349453828358965, 
            "x": -3.336666443818177, 
            "z": 188.6102819454901
          }
        }
      ],
      "tof_hits": [
        {
          "energy_deposited": 0.170892203131691, 
          "energy": 227.9307253718137, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": 1.0, 
          "time": -729015.2080515635, 
          "position": {
            "y": 0.2523241320838684, 
            "x": -0.5046664772872547, 
            "z": -15350.0
          }, 
          "momentum": {
            "y": -1.018468755448896, 
            "x": 0.1701584374342036, 
            "z": 202.4467494398831
          }
        } 
      ], 
      "primary": {
        "random_seed": 953453411, 
        "energy": 228.3856922541968, 
        "particle_id": -13, 
        "time": -729015.584310913, 
        "position": {
          "y": 0.7554039583923567, 
          "x": -0.5887174378015934, 
          "z": -15450.0
        }, 
        "momentum": {
          "y": -1.018600216855729, 
          "x": 0.1701804010609224, 
          "z": 202.4728807613866
        }
      }, 
      "primary": {
        "random_seed": 953453411, 
        "energy": 228.3856922541968, 
        "particle_id": -13, 
        "time": -729015.584310913, 
        "position": {
          "y": 3.179251533985435, 
          "x": 0.2328821487729247, 
          "z": -15450.0
        }, 
        "momentum": {
          "y": -0.9875514985167606, 
          "x": -2.601873947882971, 
          "z": 202.4563877618646
        }
      }
    }
  ], 
  "spill_number": 1, 
  "run_number": 0, 
  "daq_event_type": "physics_event"
}
