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
simulation_geometry_filename = "SingleStationRecon.dat" # geometry used by simulation

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
   "value_step":0.1, "min_value":-729016., "max_value":-729014.}, # ns
  {"name":"Energy", "fixed":False, "initial_value":228.3856922541968,
   "value_step":1., "min_value":105.7, "max_value":500.}, # MeV
  {"name":"x", "fixed":False, "initial_value":0.2328821487729247,
   "value_step":0.001, "min_value":-150., "max_value":150.}, # mm
  {"name":"Px", "fixed":False, "initial_value":-2.601873947882971,
   "value_step":0.1, "min_value":-100., "max_value":100.}, # MeV/c
  {"name":"y", "fixed":False, "initial_value":3.179251533985435,
   "value_step":0.001, "min_value":-150., "max_value":150.}, # mm
  {"name":"Py", "fixed":False, "initial_value":-0.9875514985167606,
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
PolynomialOpticsModel_order = 1

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
  "mc_smeared": [
    {
      "sci_fi_hits": [
        {
          "energy_deposited": 0.1729183541526194, 
          "energy": 227.11366652463892, 
          "particle_id": -13, 
          "track_id": 1, 
          "channel_id": {
            "fibre_number": 627, 
            "station_number": 1, 
            "tracker_number": 0, 
            "plane_number": 2
          }, 
          "charge": 1.0, 
          "time": -728983.9093284216, 
          "position": {
            "y": -38.43920797522543, 
            "x": 88.72492019183504, 
            "z": -7046.417031754418
          }, 
          "momentum": {
            "y": 2.652595490795199, 
            "x": -3.5111437342160956, 
            "z": 200.99143175405456
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
            "y": 0.09638668339803946, 
            "x": -14.865729808538612, 
            "z": -14206.839525165857
          }, 
          "momentum": {
            "y": -1.6641515844351127, 
            "x": -1.6230424274479633, 
            "z": 204.11687439579526
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
            "y": 0.192064127875594, 
            "x": -15.369477866057817, 
            "z": -14157.417517255903
          }, 
          "momentum": {
            "y": -3.5171247323826327, 
            "x": -6.186383512783181, 
            "z": 184.08370481662806
          }
        }
      ], 
      "tof_hits": [
        {
          "energy_deposited": 0.0, 
          "energy": 228.82704214320376, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8144571319, 
          "position": {
            "y": 0.6191211210249946, 
            "x": -14.188256427155121, 
            "z": -14182.829011572763
          }, 
          "momentum": {
            "y": -1.3499286434679738, 
            "x": -0.7857482870658332, 
            "z": 202.9671845834554
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.8223331735232615, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8226525454, 
          "position": {
            "y": -0.6589087380645804, 
            "x": -14.438519865019867, 
            "z": -14183.887011807432
          }, 
          "momentum": {
            "y": 2.3092053654621854, 
            "x": 1.1127262279188628, 
            "z": -2.4292764161771587
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.8132645403089978, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8223828371, 
          "position": {
            "y": 0.05499038200768125, 
            "x": -14.435342209283464, 
            "z": -14185.02627421439
          }, 
          "momentum": {
            "y": 0.9467195417080868, 
            "x": -1.8525192115076565, 
            "z": 0.8870938109679305
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7989396724995309, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8221115292, 
          "position": {
            "y": -0.007151592248525185, 
            "x": -14.235511645964413, 
            "z": -14185.782198461058
          }, 
          "momentum": {
            "y": -1.162620475498243, 
            "x": 3.3110277859878257, 
            "z": -0.6140958073764937
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7851933046778066, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8218386766, 
          "position": {
            "y": -0.11679061334278183, 
            "x": -13.913490360507092, 
            "z": -14185.244301033394
          }, 
          "momentum": {
            "y": 0.09000962313262512, 
            "x": 4.640008679072564, 
            "z": 2.638483632459116
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.742433727252333, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.821563285, 
          "position": {
            "y": -0.5681553579898891, 
            "x": -13.866803172136116, 
            "z": -14185.19382525648
          }, 
          "momentum": {
            "y": -4.013339575153989, 
            "x": -3.3482479335468627, 
            "z": -1.0574965330610309
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7248047877173869, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8212853322, 
          "position": {
            "y": -0.2601738508463693, 
            "x": -14.511960759881624, 
            "z": -14184.457571869048
          }, 
          "momentum": {
            "y": 0.9157443876654083, 
            "x": 2.348009209904424, 
            "z": 2.0707602558483584
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7108499023606117, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8209969861, 
          "position": {
            "y": -0.17486457396720756, 
            "x": -13.870810467515645, 
            "z": -14185.039872858344
          }, 
          "momentum": {
            "y": -1.105290388848153, 
            "x": 0.4382237329568194, 
            "z": -1.9422122601891936
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.6964572418492783, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8207039955, 
          "position": {
            "y": -0.5081234369470936, 
            "x": -14.969705878884747, 
            "z": -14185.332074114003
          }, 
          "momentum": {
            "y": -0.6524232143133192, 
            "x": 5.436472593037665, 
            "z": 3.3238406627996446
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.6729822906920172, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8204072751, 
          "position": {
            "y": -0.884719878286697, 
            "x": -13.726559254866153, 
            "z": -14185.431721830619
          }, 
          "momentum": {
            "y": 4.196724719289619, 
            "x": -6.488691518432168, 
            "z": 6.287825707448543
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.6521094333945497, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.819989144, 
          "position": {
            "y": -0.5082926309208906, 
            "x": -13.286506220849141, 
            "z": -14184.654553945875
          }, 
          "momentum": {
            "y": -2.051892276513007, 
            "x": -3.8972332025263996, 
            "z": -7.387489557528402
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.6186969082879457, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8196790031, 
          "position": {
            "y": -0.02049884276092634, 
            "x": -14.0037631717427, 
            "z": -14185.71079085089
          }, 
          "momentum": {
            "y": 0.3405370680814492, 
            "x": -0.03111099422952704, 
            "z": -1.434554090269409
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.581895889540226, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8193627292, 
          "position": {
            "y": 0.052845365122696886, 
            "x": -13.582774982384086, 
            "z": -14185.010352766389
          }, 
          "momentum": {
            "y": -1.7956974615068295, 
            "x": 2.5159768976356744, 
            "z": 3.697264133816359
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.5458786267187202, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8190363487, 
          "position": {
            "y": -0.47037483335876, 
            "x": -13.19530544335588, 
            "z": -14184.257816692272
          }, 
          "momentum": {
            "y": -0.7182715341277143, 
            "x": -1.5661183763954534, 
            "z": 3.271947591090718
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.51099891, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8187404167, 
          "position": {
            "y": -0.2854135144239695, 
            "x": -14.28596824942175, 
            "z": -14184.673090248683
          }, 
          "momentum": {
            "y": -0.5222927341697529, 
            "x": -1.9882718482816988, 
            "z": -1.4363393509530034
          }
        }, 
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
          "time": -729010.8125750526, 
          "position": {
            "y": 0.6188638552574593, 
            "x": -14.19542194929825, 
            "z": -14182.329011572763
          }, 
          "momentum": {
            "y": -1.6131260392373745, 
            "x": -0.3861341854236522, 
            "z": 202.7795311373139
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 1.6586008031357, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8168673082, 
          "position": {
            "y": -0.9774744375518446, 
            "x": -14.720247512910271, 
            "z": -14183.463756311812
          }, 
          "momentum": {
            "y": -0.17789052306007347, 
            "x": 0.31724508537789986, 
            "z": 0.07728421672058561
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 1.5488951436125, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8166494931, 
          "position": {
            "y": -0.4848976954680323, 
            "x": -14.638299948032083, 
            "z": -14183.742082880877
          }, 
          "momentum": {
            "y": 1.9866871545665725, 
            "x": 6.114478674639692, 
            "z": -4.001008396806986
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.873654061919551, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8239218259, 
          "position": {
            "y": 0.4611407651358542, 
            "x": -14.701161549552046, 
            "z": -14184.998760852372
          }, 
          "momentum": {
            "y": -1.785875699336278, 
            "x": 6.175774985064098, 
            "z": 0.27182790425640485
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.8615669930703413, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8235963491, 
          "position": {
            "y": -0.03540109679501232, 
            "x": -14.548797716772238, 
            "z": -14184.40195193642
          }, 
          "momentum": {
            "y": -0.4699020888966283, 
            "x": 3.7646243414147906, 
            "z": -0.6159702438100778
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.847668342851079, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.823267901, 
          "position": {
            "y": 0.1723946517003641, 
            "x": -15.004463990263313, 
            "z": -14185.032426748361
          }, 
          "momentum": {
            "y": 6.81623223674456, 
            "x": 0.1005321085824809, 
            "z": 2.0944147191763376
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.8460705518401511, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8229373433, 
          "position": {
            "y": 0.3219808835527842, 
            "x": -14.546224095821401, 
            "z": -14184.315116692564
          }, 
          "momentum": {
            "y": -1.9482000133921868, 
            "x": 2.604160331398636, 
            "z": 1.6221496822752508
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 1.380451931661755, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8142101826, 
          "position": {
            "y": 0.6109555927066899, 
            "x": -14.024987141003963, 
            "z": -14182.973181345791
          }, 
          "momentum": {
            "y": 0.33235431334108534, 
            "x": 1.250453690212755, 
            "z": -1.1715183621029044
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 1.15484689911698, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8112072569, 
          "position": {
            "y": 0.5494252086747197, 
            "x": -13.326255464817391, 
            "z": -14182.412938914003
          }, 
          "momentum": {
            "y": 2.297383224014727, 
            "x": -3.9359730395712718, 
            "z": -0.10769786891302509
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.9381916675460579, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8082121091, 
          "position": {
            "y": -0.26755242693018544, 
            "x": -12.084006957341035, 
            "z": -14180.266018178843
          }, 
          "momentum": {
            "y": -3.6074347872880774, 
            "x": 3.8278723804513213, 
            "z": -1.6484587052376432
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7229807833976859, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8052596137, 
          "position": {
            "y": -0.33255703866125075, 
            "x": -11.532648400592132, 
            "z": -14181.13802568109
          }, 
          "momentum": {
            "y": -0.918392735244276, 
            "x": 6.890500202313617, 
            "z": -6.0217776356315165
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.51099891, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8025090945, 
          "position": {
            "y": -1.4305386434309502, 
            "x": -11.474477523986772, 
            "z": -14181.825163025176
          }, 
          "momentum": {
            "y": -3.7004763774161002, 
            "x": 1.2808055491745178, 
            "z": -0.7618030626795124
          }
        }
      ], 
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
          "time": -728983.9093284216, 
          "position": {
            "y": -38.3227530757034, 
            "x": 88.87485740241073, 
            "z": -7046.449999999999
          }, 
          "momentum": {
            "y": -2.602012121823872, 
            "x": -6.668562664152506, 
            "z": 201.1696438794265
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
          "energy_deposited": 0.0, 
          "energy": 228.82704214320376, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8144571319, 
          "position": {
            "y": 0.06586220598363882, 
            "x": -14.45648339014987, 
            "z": -14182.55
          }, 
          "momentum": {
            "y": -0.1242901422699316, 
            "x": -2.863043714555592, 
            "z": 202.1344272404045
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.8223331735232615, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8226525454, 
          "position": {
            "y": -0.6589087380645804, 
            "x": -14.438519865019867, 
            "z": -14183.887011807432
          }, 
          "momentum": {
            "y": 2.3092053654621854, 
            "x": 1.1127262279188628, 
            "z": -2.4292764161771587
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.8132645403089978, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8223828371, 
          "position": {
            "y": 0.05499038200768125, 
            "x": -14.435342209283464, 
            "z": -14185.02627421439
          }, 
          "momentum": {
            "y": 0.9467195417080868, 
            "x": -1.8525192115076565, 
            "z": 0.8870938109679305
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7989396724995309, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8221115292, 
          "position": {
            "y": -0.007151592248525185, 
            "x": -14.235511645964413, 
            "z": -14185.782198461058
          }, 
          "momentum": {
            "y": -1.162620475498243, 
            "x": 3.3110277859878257, 
            "z": -0.6140958073764937
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7851933046778066, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8218386766, 
          "position": {
            "y": -0.11679061334278183, 
            "x": -13.913490360507092, 
            "z": -14185.244301033394
          }, 
          "momentum": {
            "y": 0.09000962313262512, 
            "x": 4.640008679072564, 
            "z": 2.638483632459116
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.742433727252333, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.821563285, 
          "position": {
            "y": -0.5681553579898891, 
            "x": -13.866803172136116, 
            "z": -14185.19382525648
          }, 
          "momentum": {
            "y": -4.013339575153989, 
            "x": -3.3482479335468627, 
            "z": -1.0574965330610309
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7248047877173869, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8212853322, 
          "position": {
            "y": -0.2601738508463693, 
            "x": -14.511960759881624, 
            "z": -14184.457571869048
          }, 
          "momentum": {
            "y": 0.9157443876654083, 
            "x": 2.348009209904424, 
            "z": 2.0707602558483584
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7108499023606117, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8209969861, 
          "position": {
            "y": -0.17486457396720756, 
            "x": -13.870810467515645, 
            "z": -14185.039872858344
          }, 
          "momentum": {
            "y": -1.105290388848153, 
            "x": 0.4382237329568194, 
            "z": -1.9422122601891936
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.6964572418492783, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8207039955, 
          "position": {
            "y": -0.5081234369470936, 
            "x": -14.969705878884747, 
            "z": -14185.332074114003
          }, 
          "momentum": {
            "y": -0.6524232143133192, 
            "x": 5.436472593037665, 
            "z": 3.3238406627996446
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.6729822906920172, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8204072751, 
          "position": {
            "y": -0.884719878286697, 
            "x": -13.726559254866153, 
            "z": -14185.431721830619
          }, 
          "momentum": {
            "y": 4.196724719289619, 
            "x": -6.488691518432168, 
            "z": 6.287825707448543
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.6521094333945497, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.819989144, 
          "position": {
            "y": -0.5082926309208906, 
            "x": -13.286506220849141, 
            "z": -14184.654553945875
          }, 
          "momentum": {
            "y": -2.051892276513007, 
            "x": -3.8972332025263996, 
            "z": -7.387489557528402
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.6186969082879457, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8196790031, 
          "position": {
            "y": -0.02049884276092634, 
            "x": -14.0037631717427, 
            "z": -14185.71079085089
          }, 
          "momentum": {
            "y": 0.3405370680814492, 
            "x": -0.03111099422952704, 
            "z": -1.434554090269409
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.581895889540226, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8193627292, 
          "position": {
            "y": 0.052845365122696886, 
            "x": -13.582774982384086, 
            "z": -14185.010352766389
          }, 
          "momentum": {
            "y": -1.7956974615068295, 
            "x": 2.5159768976356744, 
            "z": 3.697264133816359
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.5458786267187202, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8190363487, 
          "position": {
            "y": -0.47037483335876, 
            "x": -13.19530544335588, 
            "z": -14184.257816692272
          }, 
          "momentum": {
            "y": -0.7182715341277143, 
            "x": -1.5661183763954534, 
            "z": 3.271947591090718
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.51099891, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8187404167, 
          "position": {
            "y": -0.2854135144239695, 
            "x": -14.28596824942175, 
            "z": -14184.673090248683
          }, 
          "momentum": {
            "y": -0.5222927341697529, 
            "x": -1.9882718482816988, 
            "z": -1.4363393509530034
          }
        }, 
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
          "time": -729010.8125750526, 
          "position": {
            "y": 0.6188638552574593, 
            "x": -14.19542194929825, 
            "z": -14182.329011572763
          }, 
          "momentum": {
            "y": -1.6131260392373745, 
            "x": -0.3861341854236522, 
            "z": 202.7795311373139
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 1.6586008031357, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8168673082, 
          "position": {
            "y": -0.9774744375518446, 
            "x": -14.720247512910271, 
            "z": -14183.463756311812
          }, 
          "momentum": {
            "y": -0.17789052306007347, 
            "x": 0.31724508537789986, 
            "z": 0.07728421672058561
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 1.5488951436125, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8166494931, 
          "position": {
            "y": -0.4848976954680323, 
            "x": -14.638299948032083, 
            "z": -14183.742082880877
          }, 
          "momentum": {
            "y": 1.9866871545665725, 
            "x": 6.114478674639692, 
            "z": -4.001008396806986
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.873654061919551, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8239218259, 
          "position": {
            "y": 0.4611407651358542, 
            "x": -14.701161549552046, 
            "z": -14184.998760852372
          }, 
          "momentum": {
            "y": -1.785875699336278, 
            "x": 6.175774985064098, 
            "z": 0.27182790425640485
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.8615669930703413, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8235963491, 
          "position": {
            "y": -0.03540109679501232, 
            "x": -14.548797716772238, 
            "z": -14184.40195193642
          }, 
          "momentum": {
            "y": -0.4699020888966283, 
            "x": 3.7646243414147906, 
            "z": -0.6159702438100778
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.847668342851079, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.823267901, 
          "position": {
            "y": 0.1723946517003641, 
            "x": -15.004463990263313, 
            "z": -14185.032426748361
          }, 
          "momentum": {
            "y": 6.81623223674456, 
            "x": 0.1005321085824809, 
            "z": 2.0944147191763376
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.8460705518401511, 
          "particle_id": 11, 
          "track_id": 13, 
          "channel_id": {
            "station_number": 0, 
            "plane": 0, 
            "slab": 5
          }, 
          "charge": -1.0, 
          "time": -729010.8229373433, 
          "position": {
            "y": 0.3219808835527842, 
            "x": -14.546224095821401, 
            "z": -14184.315116692564
          }, 
          "momentum": {
            "y": -1.9482000133921868, 
            "x": 2.604160331398636, 
            "z": 1.6221496822752508
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 1.380451931661755, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8142101826, 
          "position": {
            "y": 0.6109555927066899, 
            "x": -14.024987141003963, 
            "z": -14182.973181345791
          }, 
          "momentum": {
            "y": 0.33235431334108534, 
            "x": 1.250453690212755, 
            "z": -1.1715183621029044
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 1.15484689911698, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8112072569, 
          "position": {
            "y": 0.5494252086747197, 
            "x": -13.326255464817391, 
            "z": -14182.412938914003
          }, 
          "momentum": {
            "y": 2.297383224014727, 
            "x": -3.9359730395712718, 
            "z": -0.10769786891302509
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.9381916675460579, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8082121091, 
          "position": {
            "y": -0.26755242693018544, 
            "x": -12.084006957341035, 
            "z": -14180.266018178843
          }, 
          "momentum": {
            "y": -3.6074347872880774, 
            "x": 3.8278723804513213, 
            "z": -1.6484587052376432
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.7229807833976859, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8052596137, 
          "position": {
            "y": -0.33255703866125075, 
            "x": -11.532648400592132, 
            "z": -14181.13802568109
          }, 
          "momentum": {
            "y": -0.918392735244276, 
            "x": 6.890500202313617, 
            "z": -6.0217776356315165
          }
        }, 
        {
          "energy_deposited": 0.0, 
          "energy": 0.51099891, 
          "particle_id": 11, 
          "track_id": 14, 
          "channel_id": {
            "station_number": 0, 
            "plane": 1, 
            "slab": 4
          }, 
          "charge": -1.0, 
          "time": -729010.8025090945, 
          "position": {
            "y": -1.4305386434309502, 
            "x": -11.474477523986772, 
            "z": -14181.825163025176
          }, 
          "momentum": {
            "y": -3.7004763774161002, 
            "x": 1.2808055491745178, 
            "z": -0.7618030626795124
          }
        }
      ], 
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
