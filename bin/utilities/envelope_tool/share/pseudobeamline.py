"""Test configuration for envelope tool"""
#pylint: disable=C0103

import os

simulation_geometry_filename = os.path.expandvars("${MAUS_ROOT_DIR}/bin/"+\
              "utilities/envelope_tool/share/PseudoBeamlineGeometry.dat")
simulation_reference_particle = { # used for setting particle phase
    "position":{"x":1600.0, "y":0.0, "z":0.0},
    "momentum":{"x":-0.5, "y":0.0, "z":0.5},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
}
verbose_level = 5
max_step_length = 10.

TransferMapOpticsModel_Deltas = {"t":0.01, "E":0.01,
                                 "x":0.01, "Px":0.01,
                                 "y":0.01, "Py":0.01}


