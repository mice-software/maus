#!/bin/bash

# Generate .dat files for the geometry of Horizontal and Vertical Slabs and Sci Slabs 

# Parameters
XSIZE=42.0
YSIZE=6.0
ZSIZE=2.5
NPMT=2

STATION=1

MAXSLAB=6

# --------------------

SLAB=0
while [ ${SLAB} -le ${MAXSLAB} ]
do

# Horizontal first

ORIENTATION=Horizontal

OBJECT=TOF${STATION}${ORIENTATION}Slab${SLAB}
SCIOBJECT=${OBJECT}Sci
FILE=${OBJECT}.dat
SCIFILE=${SCIOBJECT}.dat

# TOF0HorizontalSlab0.dat like files first

echo // FILES/Models/Modules/TOF/${FILE} 1>$FILE
echo //  1>>$FILE
echo  1>>$FILE

echo Module ${OBJECT} 1>>$FILE
echo { 1>>$FILE
echo   '  'Volume Box 1>>$FILE
echo   '  'Dimensions ${XSIZE} ${YSIZE} ${ZSIZE}  cm 1>>$FILE
echo   '  'PropertyInt Slab ${SLAB} 1>>$FILE
echo   '  'PropertyInt numPMTs ${NPMT} 1>>$FILE
echo   '  'PropertyDouble BlueColour 1.0 1>>$FILE
echo   '  'Module TOF/${SCIFILE} 1>>$FILE
echo   '  '{ 1>>$FILE
echo   '      'Position 0.0 0.0 0.0 mm 1>>$FILE
echo   '      'Rotation 0.0 0.0 0.0 degree 1>>$FILE
echo   '  '} 1>>$FILE
echo } 1>>$FILE

# TOF0HorizontalSlab0Sci.dat like files next

echo // FILES/Models/Modules/TOF/${SCIFILE} 1>$SCIFILE
echo //  1>>$SCIFILE
echo  1>>$SCIFILE

echo Module ${SCIOBJECT} 1>>$SCIFILE
echo { 1>>$SCIFILE
echo   '  'Volume Box 1>>$SCIFILE
echo   '  'Dimensions ${XSIZE} ${YSIZE} ${ZSIZE}  cm 1>>$SCIFILE
echo   '  'PropertyString SensitiveDetector TOF 1>>$SCIFILE
echo   '  'PropertyString Material POLYSTYRENE 1>>$SCIFILE
echo   '  'PropertyDouble GreenColour 1.0 1>>$SCIFILE
echo   '  'PropertyDouble RedColour 1.0 1>>$SCIFILE
echo } 1>>$SCIFILE

# Now go Vertical

ORIENTATION=Vertical

OBJECT=TOF${STATION}${ORIENTATION}Slab${SLAB}
SCIOBJECT=${OBJECT}Sci
FILE=${OBJECT}.dat
SCIFILE=${SCIOBJECT}.dat

echo // FILES/Models/Modules/TOF/${FILE} 1>$FILE
echo //  1>>$FILE
echo  1>>$FILE

echo Module ${OBJECT} 1>>$FILE
echo { 1>>$FILE
echo   '  'Volume Box 1>>$FILE
echo   '  'Dimensions ${YSIZE} ${XSIZE} ${ZSIZE}  cm 1>>$FILE
echo   '  'PropertyInt Slab ${SLAB} 1>>$FILE
echo   '  'PropertyInt numPMTs ${NPMT} 1>>$FILE
echo   '  'PropertyDouble BlueColour 1.0 1>>$FILE
echo   '  'Module TOF/${SCIFILE} 1>>$FILE
echo   '  '{ 1>>$FILE
echo   '      'Position 0.0 0.0 0.0 mm 1>>$FILE
echo   '      'Rotation 0.0 0.0 0.0 degree 1>>$FILE
echo   '  '} 1>>$FILE
echo } 1>>$FILE

echo // FILES/Models/Modules/TOF/${SCIFILE} 1>$SCIFILE
echo //  1>>$SCIFILE
echo  1>>$SCIFILE

echo Module ${SCIOBJECT} 1>>$SCIFILE
echo { 1>>$SCIFILE
echo   '  'Volume Box 1>>$SCIFILE
echo   '  'Dimensions ${YSIZE} ${XSIZE} ${ZSIZE}  cm 1>>$SCIFILE
echo   '  'PropertyString SensitiveDetector TOF 1>>$SCIFILE
echo   '  'PropertyString Material POLYSTYRENE 1>>$SCIFILE
echo   '  'PropertyDouble GreenColour 1.0 1>>$SCIFILE
echo   '  'PropertyDouble RedColour 1.0 1>>$SCIFILE
echo } 1>>$SCIFILE

  SLAB=$((SLAB+1))

done
