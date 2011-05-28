// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef MMatrixToCLHEP
#define MMatrixToCLHEP

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

#include "Interface/MMatrix.hh"
#include "Interface/MVector.hh"

CLHEP::HepMatrix MMatrix_to_CLHEP(MMatrix<double> mm);
CLHEP::HepVector MVector_to_CLHEP(MVector<double> mv);

MMatrix<double> CLHEP_to_MMatrix(CLHEP::HepMatrix cm);
MVector<double> CLHEP_to_MVector(CLHEP::HepVector cv);


#endif

