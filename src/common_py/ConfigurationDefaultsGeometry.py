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

## The Configuration defaults
#  These are the default values for MAUS and its components.
#  Please DO NOT CHANGE this unless you want to change this
#  quantity for everybody.  If you want to fiddle, then
#  please load your own overriding configuration by
#  consulting the documentation.
#

# This is a copied file to test geometry information
#which will be added to the defaults later
#pylint: disable = C0103, C0301, C0111

map_reduce_type = "native_python" # can also be "native_python_profile" using cProfile in python

# Used, for now, to determine what level of c++ log messages are reported to the user
# 0 = debug info (and std::cout)
# 1 = run info
# 2 = warnings
# 3 = errors (and std::cerr)
# 4 = fatal
# >4 = silent
# Doesnt effect python
verbose_level = 4
errors_to_stderr = verbose_level < 4
errors_to_json = True
on_error = 'none' # halt or raise

# Used by MapPyRemoveTracks.
keep_only_muon_tracks = False 

# Used by MapCppSimulation
keep_tracks = False # set to true to keep start and end point of every track
keep_steps = False # set to true to keep start and end point of every track and
                   # every step point
simulation_geometry_filename = "Stage6.dat" # geometry used by simulation
maximum_number_of_steps = 10000
simulation_reference_particle = {"position":{"x":0.0, "y":-0.0, "z":-4700.0}, "momentum":{"x":0.0, "y":0.0, "z":1.0}, "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10}

# geant4 visualisation (not event display)
geant4_visualisation = False
visualisation_viewer = "VRML2FILE"  # only supported option
visualisation_theta = 90.
visualisation_phi = 90.
visualisation_zoom = 1.

# this is used by reconstruction
reconstruction_geometry_filename = simulation_geometry_filename

#Used by MapPyCreateSpill
CreateSpill_Nparticles = 2 #55

#Used by MapPyBeamMaker
emittance4D = 6 #mm
centralPz = 200 #MeV/c
sigmaPz = 5 #MeV/c
sigmaTime = 0 #ns; default: 0

# tof digitization
TOFconversionFactor = 0.01 # MeV
TOFpmtTimeResolution = 0.1 # nanosecond
TOFattenuationLength = 140 * 10 # mm
TOFadcConversionFactor = 0.125
TOFtdcConversionFactor = 0.025 # nanosecond
TOFpmtQuantumEfficiency = 0.25
TOFscintLightSpeed =  170.0 # mm/ns

# scifi tracker digitization
#SciFiDeadChanFName = ""
SciFiMUXNum = 7
SciFiFiberDecayConst = 2.7
SciFiFiberConvFactor =  3047.1
SciFiFiberRefractiveIndex = 1.6
SciFiFiberCriticalAngle = 0.4782 # rad
SciFiFiberTrappingEff = 0.056
SciFiFiberMirrorEff = 0.6
SciFiFiberTransmissionEff = 0.8
SciFiMUXTransmissionEff = 1.0
SciFiFiberRunLength = 5000.0 # mm
SciFivlpcQE = 0.8
SciFivlpcEnergyRes = 4.0 # MeV
SciFivlpcTimeRes = 0.2 # ns
SciFiadcBits = 8
SciFiadcFactor = 6.0
SciFitdcBits = 16
SciFitdcFactor = 1.0
SciFinElecChanPerPlane = 1429
SciFinPlanes = 3
SciFinStations = 5
SciFinTrackers = 2

# geometry upload/download information
#Upload
# Arguments below MUST be included
GeometryDirectory   = "/home/matt/maus-littlefield/src/common_py/geometry/testCases/testGeometry"
GeometryDescription = "This is a standard note It uses the standard test case"
# Optional Arguments 1 = yes, 0 = no
Zip_File         = 0
Delete_Originals = 0

#Download
#Donwloading of geometries
#Below, name the directory where the geometry will be downloaded.
DownloadDir = "/home/matt/maus-littlefield/src/common_py/geometry/Download/"
#Download Geometry for a specific ID
#Name the start and stop time for which the geometry was valid. Stop time is optional
StartTime = "2011-09-02 09:00:00"
StopTime  = None

