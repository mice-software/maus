#!/bin/bash

WRKDIR=/afs/phas.gla.ac.uk/user/j/jnugent/private/PhD_Year_1/Coding/MAUS/maus/src/map/MapPyBeamlineSimulation/G4bl

echo WORKING DIRECTORY: $WRKDIR  

source /afs/phas.gla.ac.uk/user/j/jnugent/private/PhD_Year_1/Coding/G4beamline/G4beamline-2.12/bin/g4bl-setup.sh

VERS=v1.0

Ppi=480
Pmu=238.0
q1=1.232291
q2=-1.539583
q3=1.070833
d1=-1.492140
ds=133.72
d2=-0.46
q4=1.0557
q5=-1.41565
q6=0.93898
q7=0.9453
q8=-1.4306
q9=1.22245
KinCut=5
RANSE=$RANDOM
# exceptional number of protons
# 5E10
###Protons=1E13
protonNumber=1E12
#protonWeight=1
momentumCut=50

OUTDIR=/afs/phas.gla.ac.uk/user/j/jnugent/private/PhD_Year_1/Coding/MAUS/maus/src/map/MapPyBeamlineSimulation/G4bl/output
mkdir $OUTDIR
cd $OUTDIR

INFILE='MAY09-B1B2-positives-param.in'

#g4bl $WRKDIR/$INFILE Ppi=$Ppi Pmu=$Pmu momentumCut=$momentumCut kineticEnergyCut=$KinCut RANSE=$RANSE steppingVerbose=0 WRKDIR=$WRKDIR

g4bl $WRKDIR/$INFILE  Ppi=$Ppi Pmu=$Pmu q1=$q1 q2=$q2 q3=$q3 d1=$d1 ds=$ds d2=$d2 q4=$q4 q5=$q5 q6=$q6 q7=$q7 q8=$q8 q9=$q9 momentumCut=$momentumCut kineticEnergyCut=$KinCut RANSE=$RANSE steppingVerbose=0 WRKDIR=$WRKDIR protonNumber=$protonNumber #viewer=best 

