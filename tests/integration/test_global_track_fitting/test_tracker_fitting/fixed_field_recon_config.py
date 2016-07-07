import os


global_track_fits = [
    {
        "will_require_triplet_space_points":False,
        "min_step_size":0.1,
        "max_step_size":100.,
        "will_smooth":False,
        "mass_hypothesis":105.658,
        "charge_hypothesis":1.,
        "min_z":13000.,
        "max_z":16000.,
        "seed":"USTrackerTrackFitSeed",
        "max_iteration":1,
        "detectors":["Tracker0_5", "Tracker0_4", "Tracker0_3", "Tracker0_2", "Tracker0_1"]#, "Virtual"]
    }
]
global_track_fit_materials = ["POLYSTYRENE", "He", "AIR"]
verbose_level = 2
simulation_geometry_filename = os.path.expandvars("parent_geometries/ParentGeometryFile_4T.dat")

