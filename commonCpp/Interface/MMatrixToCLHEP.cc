#include "MMatrixToCLHEP.hh"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

#include "MMatrix.hh"
#include "MVector.hh"

CLHEP::HepMatrix MMatrix_to_CLHEP(MMatrix<double> mm)
{
  CLHEP::HepMatrix cm(mm.num_row(), mm.num_col(), 0);
  for(size_t i=1; i<= mm.num_row(); i++)
    for(size_t j=1; j<= mm.num_col(); j++)
      cm(i,j) = mm(i,j);
  return cm;
}

CLHEP::HepVector MVector_to_CLHEP(MVector<double> mv)
{
  CLHEP::HepVector cv(mv.num_row(), 0);
  for(size_t i=1; i<= mv.num_row(); i++)
    cv(i) = mv(i);
  return cv;
}

MMatrix<double> CLHEP_to_MMatrix(CLHEP::HepMatrix cm)
{
  MMatrix<double> mm(cm.num_row(), cm.num_col(), 0.);
  for(size_t i=1; i<= mm.num_row(); i++)
    for(size_t j=1; j<= mm.num_col(); j++)
      mm(i,j) = cm(i,j);
  return mm;
}

MVector<double> CLHEP_to_MVector(CLHEP::HepVector cv)
{
  MVector<double> mv(cv.num_row(), 0.);
  for(int i=1; i<= cv.num_row(); i++)
    mv(i) = cv(i);
  return mv;

}



