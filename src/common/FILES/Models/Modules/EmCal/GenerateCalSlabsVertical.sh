#!/bin/bash

# Generate .dat files for the geometry of Horizontal and Vertical Slabs and Sci Slabs 

# Parameters
XSIZE=11
YSIZE=100
ZSIZE=1
NPMT=2

PLANE=1

MAXSLAB=8

# --------------------
LAYERFILE=CalorimeterPlane${PLANE}.dat
echo // FILES/Models/Modules/EmCal/${LAYERFILE} 1>$LAYERFILE
echo //  1>>$LAYERFILE
echo  1>>$LAYERFILE
echo Module CalorimeterPlane${PLANE}>>$LAYERFILE
echo { 1>>$LAYERFILE
echo   '  'Volume Box 1>>$LAYERFILE
echo   '  'Dimensions $((MAXSLAB*XSIZE+XSIZE)) ${YSIZE} ${ZSIZE}  cm 1>>$LAYERFILE
echo   '  'PropertyInt Layer ${PLANE}>>$LAYERFILE
echo   '  'PropertyInt numSlabs ${MAXSLAB}>>$LAYERFILE
echo   '  'PropertyBool Invisible 1 >>$LAYERFILE
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
echo   '  'PropertyInt Layer ${PLANE} 1>>$FILE
echo   '  'PropertyInt Cell ${SLAB} 1>>$FILE
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
echo   '  'PropertyInt Layer ${PLANE} 1>>$SCIFILE
echo   '  'PropertyInt Cell ${SLAB} 1>>$SCIFILE
echo   '  'PropertyString SensitiveDetector EMCAL 1>>$SCIFILE
echo   '  'PropertyString Material POLYSTYRENE 1>>$SCIFILE
echo   '  'PropertyDouble GreenColour 1.0 1>>$SCIFILE
echo   '  'PropertyDouble RedColour 1.0 1>>$SCIFILE
echo } 1>>$SCIFILE

echo '  'Module EmCal/$FILE 1>>$LAYERFILE
echo '  '{ 1>>$LAYERFILE
echo '    'Position $((SLAB*XSIZE-MAXSLAB/2*XSIZE)) 0.0 0.0 cm>>$LAYERFILE
echo '    'Rotation 0.0 0.0 0.0 degree>>$LAYERFILE
echo '  '} 1>>$LAYERFILE

SLAB=$((SLAB+1))
done
echo } 1>>$LAYERFILE
