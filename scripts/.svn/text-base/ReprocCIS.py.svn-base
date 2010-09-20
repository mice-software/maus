#!/bin/env python
# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>

import os, sys

runs = sys.argv[1:]

# Handle the case where a string of runs is passed
if len(str(runs[0]).split(" ")) != 1 :
        runs = runs[0].split(" ")

print "# execute me"
print "mkdir -p /tmp/$USER/recis"

for run in runs:
    assert(run.isdigit())

    command = "nsls /castor/cern.ch/grid/atlas/DAQ/tile/2009/daq/ | grep %d | xargs -i stager_get -M /castor/cern.ch/grid/atlas/DAQ/tile/2009/daq/{}" % (int(run))
    print command

    command = "nsls /castor/cern.ch/grid/atlas/DAQ/tile/2009/daq/ | grep %d | xargs -i rfcp /castor/cern.ch/grid/atlas/DAQ/tile/2009/daq/{} /tmp/$USER/recis" % (int(run))
    print command

    command = "athena -c \"RunNumber=%s;RunFromLocal=True;TileCisRun=True;EvtMin=200;InputDirectory='/tmp/$USER/recis';OutputDirectory='/tmp/$USER/recis';doTileMon=False;doTileCalib=True\" jobOptions_TileMobiComm.py" % run
    print command



