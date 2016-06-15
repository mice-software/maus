"""
Datacard to process straight track data
"""
# pylint: disable=C0103, W0611

import os

# Change these two paths to the location where you downloaded the
# CDB geometry. Make sure to use a no-field geometry.
simulation_geometry_filename = \
  '%s/files/geometry/download/run7469/ParentGeometryFile.dat' \
  % os.environ.get("MAUS_ROOT_DIR")
reconstruction_geometry_filename = \
  '%s/files/geometry/download/run7469/ParentGeometryFile.dat' \
  % os.environ.get("MAUS_ROOT_DIR")

# Change this to the filename of your unpacked data file
input_root_file_name = '07469_recon.root'

# 0 -> debug output (e.g. messages about detector matches) on, 1 -> off
verbose_level = 1

# Don't change this, there is currently a bug affecting accessing local
# volumes in a CDB geometry.
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
  "EMRx":1.0, # Multiplier for the standard tolerance which is the
  "EMRy":1.0, # reconstructed error*sqrt(12)
  "TOF12maxSpeed":1.0, # fraction of c to calculate travel time between TOFs
  "TOF12minSpeed":0.5, # for through matching
}
