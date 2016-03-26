class Config(object):
    reco_file = "tmp/MAUS-v2.1.0/07469/07469_recon.root"
    cuts_file = "blackmore_data/run7469_extracted_data__MAUS2pt1b.root"
    geometry = "geometry_07469/ParentGeometryFile.dat"
    will_require_triplets = False # require triplet space points
    will_require_clusters = False # require one cluster per plane
    will_require_space_points = False # require one space point per station
    will_require_tof12 = False # require tof12 cuts
    required_trackers = [0]
    time_detector = "tof1"
    residual_detectors = {"tof1":["x", "y"], "tof0":["x", "y", "t"]}
    number_of_spills = 10000
    plot_dir = "plots"
    will_load_tk_space_points = False
    will_load_tk_track_points = True
    residuals_plots_nbins = 200


