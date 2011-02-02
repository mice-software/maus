#!/bin/bash

# Generate .dat files for the geometry of Horizontal and Vertical Slabs and Sci Slabs 

# Parameters
XSIZE=100.0
YSIZE=11
ZSIZE=2
NPMT=2

PLANE=2

MAXSLAB=8

# --------------------

SLAB=0
while [ ${SLAB} -le ${MAXSLAB} ]
do

# Horizontal first

# ORIENTATION=Horizontal

OBJECT=CalorimeterPlane${PLANE}Slab${SLAB}
SCIOBJECT=${OBJECT}Sci
FILE=${OBJECT}.dat
SCIFILE=${SCIOBJECT}.dat

# EMCal0Slab0.dat like files first

echo // FILES/Models/Modules/EmCal/${FILE} 1>$FILE
echo //  1>>$FILE
echo  1>>$FILE

echo Module ${OBJECT} 1>>$FILE
echo { 1>>$FILE
echo   '  'Volume Box 1>>$FILE
echo   '  'Dimensions ${XSIZE} ${YSIZE} ${ZSIZE}  cm 1>>$FILE
echo   '  'PropertyInt Slab ${SLAB} 1>>$FILE
echo   '  'PropertyInt numPMTs ${NPMT} 1>>$FILE
echo   '  'PropertyDouble BlueColour 1.0 1>>$FILE
echo   '  'Module EmCal/${SCIFILE} 1>>$FILE
echo   '  '{ 1>>$FILE
echo   '      'Position 0.0 0.0 0.0 mm 1>>$FILE
echo   '      'Rotation 0.0 0.0 0.0 degree 1>>$FILE
echo   '  '} 1>>$FILE
echo } 1>>$FILE

# EmCal0Slab0Sci.dat like files next

echo // FILES/Models/Modules/EmCal/${SCIFILE} 1>$SCIFILE
echo //  1>>$SCIFILE
echo  1>>$SCIFILE

echo Module ${SCIOBJECT} 1>>$SCIFILE
echo { 1>>$SCIFILE
echo   '  'Volume Box 1>>$SCIFILE
echo   '  'Dimensions ${XSIZE} ${YSIZE} ${ZSIZE}  cm 1>>$SCIFILE
echo   '  'PropertyString SensitiveDetector EMCAL 1>>$SCIFILE
echo   '  'PropertyString Material POLYSTYRENE 1>>$SCIFILE
echo   '  'PropertyDouble GreenColour 1.0 1>>$SCIFILE
echo   '  'PropertyDouble RedColour 1.0 1>>$SCIFILE
echo } 1>>$SCIFILE

  SLAB=$((SLAB+1))

done
