#!/bin/bash

# Generate .dat files for the geometry of Horizontal and Vertical Cells and Sci Cells 

# Parameters
XSIZE=100.0
YSIZE=4
ZSIZE=4
NPMT=2

PLANE=0

MAXSLAB=24
LAYERFILE=CalorimeterPlane${PLANE}.dat
echo // FILES/Models/Modules/EmCal/${LAYERFILE} 1>$LAYERFILE
echo //  1>>$LAYERFILE
echo  1>>$LAYERFILE
echo Module CalorimeterPlane${PLANE}>>$LAYERFILE
echo { 1>>$LAYERFILE
echo   '  'Volume Box 1>>$LAYERFILE
echo   '  'Dimensions ${XSIZE} $((MAXSLAB*YSIZE+YSIZE)) ${ZSIZE}  cm 1>>$LAYERFILE
echo   '  'PropertyInt Plane ${PLANE}>>$LAYERFILE
echo   '  'PropertyInt numSlabs ${MAXSLAB}>>$LAYERFILE
echo   '  'PropertyBool Invisible 1 >>$LAYERFILE
# --------------------

SLAB=0
while [ ${SLAB} -le ${MAXSLAB} ]
do

OBJECT=CalorimeterPlane${PLANE}KLCell${SLAB}
FILE=${OBJECT}.dat

# EMCal0Cell0.dat like files first

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
echo   '  'PropertyDouble BlueColour 0.5 1>>$FILE
echo   '  'PropertyDouble RedColour 0.5 1>>$FILE
echo   '  'PropertyDouble GreenColour 0.5 1>>$FILE
echo   '  'PropertyString Material Pb 1>>$FILE
echo   '  'Module EmCal/KLGlue.dat 1>>$FILE
echo   '  '{ 1>>$FILE
echo   '    'Position 0 0 0 cm 1>>$FILE
echo   '    'Rotation 0 0 0 degree 1>>$FILE
echo   '  '} 1>>$FILE
echo   '  'Module EmCal/KLFiber.dat 1>>$FILE
echo   '    '{ 1>>$FILE
echo   '    'Position 0 0 0 cm 1>>$FILE
echo   '    'Rotation 0 0 0 degree 1>>$FILE
echo   '  '} 1>>$FILE
echo } 1>>$FILE

echo '  'Module EmCal/$FILE 1>>$LAYERFILE
echo '  '{ 1>>$LAYERFILE
echo '    'Position 0.0 $((SLAB*YSIZE-MAXSLAB/2*YSIZE)) 0.0 cm 1>>$LAYERFILE
echo '    'Rotation 0.0 0.0 0.0 degree 1>>$LAYERFILE
echo '  '} 1>>$LAYERFILE

  SLAB=$((SLAB+1))

done
echo } 1>>$LAYERFILE

