"""
Config file for test_MapCppGlobalPID.py
"""

# pylint: disable=C0103

import os

# Default location of root file containing PDF histograms used for Global PID
PID_PDFs_file = '%s/src/map/MapCppGlobalPID/PIDhists.root'\
                % os.environ.get("MAUS_ROOT_DIR")
# PID_PDFs_file =  '%s/src/map/MapCppGlobalPID/com_pid_hists.root'\
# % os.environ.get("MAUS_ROOT_DIR")
# Tag used by both MapCppGlobalPID and ReduceCppGlobalPID, determines which
# PDFs to perform PID against/which PDFs to produce (in this case, set based
# upon input MC beam). A typical tag here would be the emittance and momentum,
# e.g. 3-140, 6-200, etc.
pid_beam_setting = "200MeV"
# Unique identifier used when creating PDFs in Global PID to distinguish between
# PDFs for the same hypothesis generated at different times. For PDFs to be
# produced, this must be set.
# Any string can be used but date and time is recommended, by using python
# datetime module and the line
# unique_identifier = (datetime.datetime.now()).strftime("%Y_%m_%dT%H_%M_%S_%f")
unique_identifier = ""
# Polarity of running mode, used by pid to select whether to run pid against
#  positive or negative particles, value can be "positive" or "negative".
pid_beamline_polarity = "positive"

# Bounds set on values of PID variables when running PID
pid_bounds = {
    # Bounds for PIDVarA
    "minA":20,"maxA":40,
    # PIDVarB
    "XminB":10, "XmaxB":250, "YminB":20, "YmaxB":40,
    # PIDVarC
    "XminC":50, "XmaxC":350, "YminC":0, "YmaxC":8000,
    # PIDVarD
    "minD":0, "maxD":8000,
    # PIDVarE
    "minE":0, "maxE":1000,
    # PIDVarF
    "XminF":50, "XmaxF":350, "YminF":0, "YmaxF":1000,
    # PIDVarG
    "minG":0, "maxG":1,
    # PIDVarH
    "XminH":50, "XmaxH":350, "YminH":0, "YmaxH":1,
    # PIDVarI
    "XminI":50, "XmaxI":350, "YminI":0, "YmaxI":140,
    # PIDVarJ
    "XminJ":50, "XmaxJ":350, "YminJ":0, "YmaxJ":140,
    # ComPIDVarA
    "minComA":30, "maxComA":50,
    # ComPIDVarB
    "XminComB":30, "XmaxComB":50, "YminComB":0, "YmaxComB":8000,
    # ComPIDVarC
    "minComC":0, "maxComC":8000,
    # ComPIDVarD
    "minComD":0, "maxComD":1000,
    # ComPIDVarE
    "XminComE":30, "XmaxComE":50, "YminComE":0, "YmaxComE":1000,
    # ComPIDVarF
    "minComF":0, "maxComF":1,
    # ComPIDVarG
    "XminComG":30, "XmaxComG":50, "YminComG":0, "YmaxComG":1,
    # ComPIDVarH
    "XminComH":30, "XmaxComH":50, "YminComH":0, "YmaxComH":40,
    # ComPIDVarI
    "XminComI":30, "XmaxComI":50, "YminComI":0, "YmaxComI":140
}

# PID MICE configuration, 'step_4' for Step IV running, 'commissioning' for
# field free commissioning data
pid_config = "step_4"
# PID running mode - selects which PID variables are used. 'online' corresponds
# to less beam (momentum) dependent variables, 'offline' uses all variables and
# requires that specific PDFs for the beam already exist. 'custom' allows user
# to choose which variables to use, and these should then be set as datacards.
# However it is not recommended to use the custom setting unless you are the
# person currently developing the Global PID.
pid_mode = "custom"
# If pid_mode = "custom", variables to use should be set here as a space
# separated list, i.e. custom_pid_set = "PIDVarA PIDVarC PIDVarD". 
custom_pid_set = "PIDVarA"
# PID confidence level- set the margin (in %) between the confidence levels of
# competing pid hypotheses before they are selected as the correct hypothesis
pid_confidence_level = 10
# PID track selection- select which tracks from TrackMatching to perform PID on.
# Can perform PID on all tracks by setting to "all", or on all downstream tracks
# (set to "DS"), all upstream (set to "US"), through tracks (set to "Through"),
# or the upstream or downstream components of the throught track (set to
# "Through-US" or "Through-DS" respectively). Or a combination of the above can
# be used, entered as a space separated list,
# e.g. "Through Through-US Through-DS"
pid_track_selection = "Through"
