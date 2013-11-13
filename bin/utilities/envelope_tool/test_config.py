simulation_geometry_filename = "TestGeometry.dat"
simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
}
verbose_level = 5
max_step_length = 1.

TransferMapOpticsModel_Deltas = {"t":0.01, "E":0.01,
                                 "x":0.01, "Px":0.01,
                                 "y":0.01, "Py":0.01}


