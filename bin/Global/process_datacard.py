"""
Datacard to process straight track data
"""
# pylint: disable=C0103, W0611

import os

# Change this to the location where you downloaded the
# CDB geometry. Just use 'full/file/path' if they are not in your MAUS install 
simulation_geometry_filename = \
  '%s/files/geometry/download/run7469/ParentGeometryFile.dat' \
  % os.environ.get("MAUS_ROOT_DIR")

# Uncomment this if the reconstruction geometry is different from the MC geom.
# This should not be the case unless there was a problem with the geometry.
# reconstruction_geometry_filename = \
#   '%s/files/geometry/download/run7469/ParentGeometryFile.dat' \
#   % os.environ.get("MAUS_ROOT_DIR")

# Change this to the filename of your unpacked data file
input_root_file_name = '07469_recon.root'

# Change this to the filename of your ouput file
# which will either be a input toor file for PDF production (trackmatched_mc file)
# or a trackmatched_data file
output_root_file_name = 'trackmatched_data.root'

# 0 -> debug output (e.g. messages about detector matches) on, 1 -> off
verbose_level = 1

# If for a given run there are no fields between TOF1 and
# EMR, this can be set to False for a slight speed-up, though the TOF0
# tolerance would have to be increased. Should really always be true.
track_matching_energy_loss = True

# Matching will be attempted with all pid hypotheses (mu, pi, e +/-)
# Can be changed to either one (never several) of
# "kEPlus", "kEMinus", "kMuPlus", "kMuMinus", "kPiPlus", "kPiMinus" but
# for real data reconstruction should be left at "all"
track_matching_pid_hypothesis = "all"

# Tolerances for track matching, in mm unless otherwise specified.
# When editing, keep in mind the fundamental limit imposed by detector
# resolution.
track_matching_tolerances = {
  "TOF0t":2.0,
  "TOF1x":40.0,
  "TOF1y":40.0,
  "TOF2x":40.0,
  "TOF2y":40.0,
  "KLy":32.0, # KL only reconstructs y position
  "EMRx":40.0, # Multiplier for the standard tolerance which is the
  "EMRy":40.0, # reconstructed error*sqrt(12)
  "TOF12maxSpeed":1.0, # fraction of c to calculate travel time between TOFs
  "TOF12minSpeed":0.5, # for through matching
}
