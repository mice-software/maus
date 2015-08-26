# pylint: disable = C0103, C0111
# one_big_file - puts everything in output_root_file_name
# one_file_per_run - splits and inserts xxx_<run_number>.xxx for each run, like
#                    maus_output_1111.root
# end_of_run_file_per_run - as above, and places in
#        <end_of_run_output_root_directory>/<run_number>/<output_root_file_name>
#      users responsibility to ensure that <end_of_run_output_root_directory>
#      exists but MAUS will make <run_number> subdirectories
output_root_file_mode = "end_of_run_file_per_run"
end_of_run_output_root_directory = "/home/mice/MAUS/offline_reco_target/"
## reconstruction data cards from tracker group - DR 07/22/2015
simulation_geometry_filename = "Stage4.dat"
reconstruction_geometry_filename = "Stage4.dat" 
SciFiDigitizationNPECut = 2.0
SciFiPRHelicalOn = False 
