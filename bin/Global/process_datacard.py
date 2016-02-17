"""
Datacard to process straight track data
"""
# pylint: disable=C0103, W0611

import os

simulation_geometry_filename = \
  '%s/files/geometry/download/108-nofield/ParentGeometryFile.dat' \
  % os.environ.get("MAUS_ROOT_DIR")

reconstruction_geometry_filename = \
  '%s/files/geometry/download/108-nofield/ParentGeometryFile.dat' \
  % os.environ.get("MAUS_ROOT_DIR")

input_root_file_name = '7157_unpacked.root'
verbose_level = 0

track_matching_energy_loss = False
track_matching_pid_hypothesis = "all"

track_matching_tolerances = {
  "TOF0x":30.0,
  "TOF0y":30.0,
  "TOF1x":40.0,
  "TOF1y":40.0,
  "TOF2x":40.0,
  "TOF2y":40.0,
  "KLy":32.0,
  "EMRx":1.0, # Multiplier for the standard tolerance which is the
  "EMRy":1.0, # reconstructed error*sqrt(12)
  "TOF12maxSpeed":1.0, # fraction of c to calculate travel time between TOFs
  "TOF12minSpeed":0.5, # for through matching
}
