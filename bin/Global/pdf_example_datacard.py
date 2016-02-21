"""
Datacard to create PDFs from MC data that has been processed by
MapCppGlobalReconImport and MapCppGlobalTrackMatching

PDFs can be produced from MC created using g4bl, or from using
InputPySpillGenerator in MAUS, however in the latter case, the number
of particles per spill must be set to 1 when simulating, as
MapCppGlobalTrackMatching currently can't handle multiple particles
per spill, due to the MC trigger

Directories will be created in files/PID/ for each particle hypothesis,
tagged with the unique identifier. These directories will contain the
root files containing the individual histograms. These histograms should then
be combined into a single root file, using the hadd command (see hadd man
page for usage), and this is the root file that will be used by when running
the global PID
"""
# pylint: disable=C0103, W0611

import os
import datetime

# Use the current time and date as a unique identifier when creating files to
# contain PDFs. A unique_identifier is required by the reducer, and PDF 
# production will fail without one.
now = datetime.datetime.now()
unique_identifier = now.strftime("%Y_%m_%dT%H_%M_%S_%f")

# The input root file/json document containing global tracks
input_root_file_name = "maus_output.root"

# PID MICE configuration, 'step_4' for Step IV running,
# 'commissioning' for field free commissioning data (straight tracks)
pid_config = "step_4"

# Tag used by both MapCppGlobalPID and ReduceCppGlobalPID, determines which
# PDFs to perform PID against/which PDFs to produce (in this case, set based
# upon input MC beam). A typical tag here would be the emittance and momentum,
# e.g. 3-140, 6-200, etc.
# The tag used for the PDFs must match the tag you use when doing PID
pid_beam_setting = "6-200"

# Polarity of running mode, used by pid to select whether to run pid against
#  positive or negative particles, value can be "positive" or "negative".
pid_beamline_polarity = "positive"
