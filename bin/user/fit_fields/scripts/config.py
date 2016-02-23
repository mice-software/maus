class Config(object):
    reco_file = "tmp/MAUS-v1.4.1/07475/07475_recon.root"
    geometry = "geometry_preprod736/ParentGeometryFile.dat"
    will_require_triplets = True # require triplet space points
    will_require_clusters = False # require one cluster per plane
    will_require_space_points = True # require one space point per station
    will_require_tof12 = True # require tof12 cuts
    required_trackers = [0, 1]
    fit_detectors = {"tku":["x", "y"], "tof1":["t"], "tof2":["t"], "tkd":[]}
    fitter_max_iterations = 500
    number_of_events = 5
    plot_dir = "plots"
