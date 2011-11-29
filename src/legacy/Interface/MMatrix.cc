// MAUS WARNING: THIS IS LEGACY CODE.
//This file is a part of G4MICE
//
//G4MICE is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//xboa is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with xboa in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.
//
//Some more details on implementation
//
//To get the templates right in cc file, you need to declare a function
//and then tell compiler which template objects to compile - otherwise
//you will get a linker error
//
//I am trying to replace CLHEP matrix here, as CLHEP has less functionality
//and is probably worse written than GSL. A similar thing exists in boost
//library
//

#include "Interface/MMatrix.hh"
#include "Interface/Squeal.hh"

#include "gsl/gsl_linalg.h"
#include "gsl/gsl_eigen.h"

///////////////////////////////// MMATRIX ///////////////////////////////

//template class MMatrix<double>;

/////////// CONSTRUCTORS, DESTRUCTORS

template <class Tmplt>
MMatrix<Tmplt>::MMatrix() : _matrix(NULL)
{}
template MMatrix<double>   ::MMatrix();
template MMatrix<m_complex>::MMatrix();


template <>
void MMatrix<double>::delete_matrix()
{
  if(_matrix != NULL) gsl_matrix_free( (gsl_matrix*)_matrix );
  _matrix = NULL;
}

template <>
void MMatrix<m_complex>::delete_matrix()
{
  if(_matrix != NULL) gsl_matrix_complex_free( (gsl_matrix_complex*)_matrix );
  _matrix = NULL;
}


template <>
MMatrix<double>& MMatrix<double>::operator= (const MMatrix<double>& mm)
{ 
  if (&mm == this) return *this;
  delete_matrix();
  if(!mm._matrix) { _matrix = NULL; return *this; }
  _matrix = gsl_matrix_alloc( mm.num_row(), mm.num_col() ); 
  gsl_matrix_memcpy((gsl_matrix*)_matrix, (const gsl_matrix*)mm._matrix);  
  return *this;
}

template <>
MMatrix<m_complex>& MMatrix<m_complex>::operator= (const MMatrix<m_complex>& mm)
{ 
  if (&mm == this) return *this;
  delete_matrix();
  if(!mm._matrix) { _matrix = NULL; return *this; }
  _matrix = gsl_matrix_complex_alloc( mm.num_row(), mm.num_col() ); 
  gsl_matrix_complex_memcpy((gsl_matrix_complex*)_matrix, (const gsl_matrix_complex*)mm._matrix);  
  return *this;
}

template <>
MMatrix<double>::MMatrix( const MMatrix<double>& mm ) : _matrix(NULL)
{
  if(mm._matrix)
  {
    _matrix = gsl_matrix_alloc(mm.num_row(), mm.num_col() );
    gsl_matrix_memcpy( (gsl_matrix*)_matrix, (gsl_matrix*)mm._matrix );
  }
}

template <>
MMatrix<m_complex>::MMatrix( const MMatrix<m_complex>& mm ) : _matrix(NULL)
{
  if(mm._matrix)
  {
    _matrix = gsl_matrix_complex_alloc( mm.num_row(), mm.num_col() );
    gsl_matrix_complex_memcpy( (gsl_matrix_complex*)_matrix, (gsl_matrix_complex*)mm._matrix );
  }
}

template <class Tmplt>
MMatrix<Tmplt>::MMatrix(size_t i, size_t j, Tmplt* data_beg ) : _matrix(NULL)
{
  build_matrix(i, j, data_beg);
}
template MMatrix<double>   ::MMatrix(size_t i, size_t j, double*    data_beg );
template MMatrix<m_complex>::MMatrix(size_t i, size_t j, m_complex* data_beg );

template <class Tmplt>
MMatrix<Tmplt>::MMatrix(size_t i, size_t j, Tmplt  value    )
{
  build_matrix(i, j);
  for(size_t a=1; a<=i; a++)
    for(size_t b=1; b<=j; b++)
      operator()(a,b) = value;
}
template MMatrix<double>   ::MMatrix(size_t i, size_t j, double    value);
template MMatrix<m_complex>::MMatrix(size_t i, size_t j, m_complex value);

template <class Tmplt>
MMatrix<Tmplt>::MMatrix(size_t i, size_t j )
{
  build_matrix(i, j);
}
template MMatrix<double>   ::MMatrix(size_t i, size_t j );
template MMatrix<m_complex>::MMatrix(size_t i, size_t j );

template <class Tmplt>
MMatrix<Tmplt> MMatrix<Tmplt>::Diagonal(size_t i, Tmplt diag_value, Tmplt off_diag_value)
{
  MMatrix<Tmplt> mm(i,i);
  for(size_t a=1; a<=i; a++)
  {
    for(size_t b=1;   b< a; b++) mm(a, b) = off_diag_value;
    for(size_t b=a+1; b<=i; b++) mm(a, b) = off_diag_value;
    mm(a,a) = diag_value;
  }
  return mm;
}
template MMatrix<double>    MMatrix<double>   ::Diagonal(size_t i, double    diag, double    off_diag);
template MMatrix<m_complex> MMatrix<m_complex>::Diagonal(size_t i, m_complex diag, m_complex off_diag);


template <class Tmplt>
MMatrix<Tmplt>::~MMatrix()
{
  delete_matrix(); 
}
template MMatrix<double>::~MMatrix();
template MMatrix<m_complex>::~MMatrix();

template <>
void MMatrix<double>::build_matrix(size_t i, size_t j)
{
  _matrix = gsl_matrix_alloc(i, j);
}

template <>
void MMatrix<m_complex>::build_matrix(size_t i, size_t j)
{
  _matrix = gsl_matrix_complex_alloc(i, j);
}

template <>
void MMatrix<double>::build_matrix(size_t i, size_t j, double* data)
{
  _matrix = gsl_matrix_alloc(i, j);
  for(size_t a=0; a<i; a++)
    for(size_t b=0; b<j; b++)
      operator()(a+1,b+1) = data[b*i + a];
}

template <>
void MMatrix<m_complex>::build_matrix(size_t i, size_t j, m_complex* data)
{
  _matrix = gsl_matrix_complex_alloc(i, j);
  for(size_t a=0; a<i; a++)
    for(size_t b=0; b<j; b++)
     operator()(a+1,b+1) = data[b*i + a];
}


////////////////// HIGH LEVEL FUNCTIONS

template <>
m_complex MMatrix<m_complex>::determinant() const
{
  int signum = 1;

  if(num_row() != num_col()) throw(Squeal(Squeal::recoverable, "Attempt to get determinant of non-square matrix", "MMatrix::determinant()"));
  gsl_permutation * p = gsl_permutation_alloc (num_row());
  MMatrix<m_complex> copy(*this);
  gsl_linalg_complex_LU_decomp ((gsl_matrix_complex*)copy._matrix, p, &signum);
  gsl_permutation_free(p);
  return gsl_linalg_complex_LU_det((gsl_matrix_complex*)copy._matrix, signum);
}

template <>
double MMatrix<double>::determinant() const
{
  int signum = 1;
  if(num_row() != num_col()) throw(Squeal(Squeal::recoverable, "Attempt to get determinant of non-square matrix", "MMatrix::determinant()"));
  gsl_permutation * p = gsl_permutation_alloc (num_row());
  MMatrix<double> copy(*this);
  gsl_linalg_LU_decomp ((gsl_matrix*)copy._matrix, p, &signum);
  double d = gsl_linalg_LU_det((gsl_matrix*)copy._matrix,  signum);
  gsl_permutation_free(p);
  return d;
}

template <class Tmplt>
MMatrix<Tmplt> MMatrix<Tmplt>::inverse()     const
{
  MMatrix<Tmplt> copy(*this);
  copy.invert();
  return copy;
}
template MMatrix<double>    MMatrix<double>   ::inverse() const;
template MMatrix<m_complex> MMatrix<m_complex>::inverse() const;


template <>
void MMatrix<m_complex>::invert() 
{
  if(num_row() != num_col()) throw(Squeal(Squeal::recoverable, "Attempt to get inverse of non-square matrix", "MMatrix::invert()"));
  gsl_permutation* perm = gsl_permutation_alloc(num_row());
  MMatrix<m_complex>  lu(*this); //hold LU decomposition
  int s=0;
  gsl_linalg_complex_LU_decomp ( (gsl_matrix_complex*)lu._matrix, perm, &s);
  gsl_linalg_complex_LU_invert ( (gsl_matrix_complex*)lu._matrix, perm, (gsl_matrix_complex*)_matrix);
  gsl_permutation_free( perm );
  for(size_t i=1; i<=num_row(); i++) 
    for(size_t j=1; j<=num_row(); j++)
      if((*this)(i,j) != (*this)(i,j)) throw(Squeal(Squeal::recoverable, "Failed to invert matrix - singular?", "MMatrix::invert()"));
}

template <>
void MMatrix<double>::invert()
{
  if(num_row() != num_col()) throw(Squeal(Squeal::recoverable, "Attempt to get inverse of non-square matrix", "MMatrix::invert()"));
  gsl_permutation* perm = gsl_permutation_alloc(num_row());
  MMatrix<double>  lu(*this); //hold LU decomposition
  int s=0;
  gsl_linalg_LU_decomp ( (gsl_matrix*)lu._matrix, perm, &s);
  gsl_linalg_LU_invert ( (gsl_matrix*)lu._matrix, perm, (gsl_matrix*)_matrix);
  gsl_permutation_free( perm );
  for(size_t i=1; i<=num_row(); i++) 
    for(size_t j=1; j<=num_row(); j++)
      if(operator()(i,j) != operator()(i,j)) throw(Squeal(Squeal::recoverable, "Failed to invert matrix - singular?", "MMatrix::invert()"));
}

template <>
MMatrix<double> MMatrix<double>::T()     const
{
  MMatrix<double>  in(num_col(), num_row());
  gsl_matrix_transpose_memcpy ((gsl_matrix*)in._matrix, (gsl_matrix*)_matrix);
  return in;
}

template <>
MMatrix<m_complex> MMatrix<m_complex>::T()     const
{
  MMatrix<m_complex>  in(num_col(), num_row());
  gsl_matrix_complex_transpose_memcpy ((gsl_matrix_complex*)in._matrix, (gsl_matrix_complex*)_matrix);
  return in;
}

template <class Tmplt>
MMatrix<Tmplt>  MMatrix<Tmplt>::sub(size_t min_row, size_t max_row, size_t min_col, size_t max_col) const
{
  MMatrix<Tmplt> sub_matrix(max_row-min_row+1, max_col-min_col+1);
  for(size_t i=min_row; i<=max_row; i++)
    for(size_t j=min_col; j<=max_col; j++)
      sub_matrix(i-min_row+1, j-min_col+1) = operator()(i,j);
  return sub_matrix;
}
template MMatrix<double>    MMatrix<double>   ::sub(size_t min_row, size_t max_row, size_t min_col, size_t max_col) const;
template MMatrix<m_complex> MMatrix<m_complex>::sub(size_t min_row, size_t max_row, size_t min_col, size_t max_col) const;


template <class Tmplt>
Tmplt MMatrix<Tmplt>::trace() const
{
  Tmplt t = operator()(1,1);
  for(size_t i=2; i<=num_row() && i<=num_col(); i++) t+= operator()(i,i);
  return t;
}
template double    MMatrix<double>::trace()    const; 
template m_complex MMatrix<m_complex>::trace() const;


template <class Tmplt>
MVector<m_complex> MMatrix<Tmplt>::eigenvalues() const
{
  if(num_row() != num_col()) throw(Squeal(Squeal::recoverable, "Attempt to get eigenvectors of non-square matrix", "MMatrix<double>::eigenvalues") );
  MMatrix<Tmplt> temp = *this;
  MVector<m_complex> evals(num_row(), MAUS::Complex::complex(2.,-1.));
  gsl_eigen_nonsymm_workspace * w = gsl_eigen_nonsymm_alloc(num_row());
  gsl_eigen_nonsymm_params(0, 1, w);
  int ierr = gsl_eigen_nonsymm(get_matrix(temp), evals.get_vector(evals), w);
  gsl_eigen_nonsymm_free(w);
  if(ierr != 0) throw(Squeal(Squeal::recoverable, "Failed to calculate eigenvalue", "MMatrix<Tmplt>::eigenvalues"));
  return evals;
}
template MVector<m_complex> MMatrix<double>   ::eigenvalues() const;

template <class Tmplt>
std::pair<MVector<m_complex>, MMatrix<m_complex> > MMatrix<Tmplt>::eigenvectors() const
{
  if(num_row() != num_col()) throw(Squeal(Squeal::recoverable, "Attempt to get eigenvectors of non-square matrix", "MMatrix<double>::eigenvectors") );
  MMatrix<Tmplt>     temp = *this;
  MVector<m_complex> evals(num_row());
  MMatrix<m_complex> evecs(num_row(), num_row());
  gsl_eigen_nonsymmv_workspace * w = gsl_eigen_nonsymmv_alloc(num_row());
  int ierr = gsl_eigen_nonsymmv(get_matrix(temp), evals.get_vector(evals), get_matrix(evecs), w);
  gsl_eigen_nonsymmv_free(w);
  if(ierr != 0) throw(Squeal(Squeal::recoverable, "Failed to calculate eigenvalue", "MMatrix<Tmplt>::eigenvectors"));
  return std::pair<MVector<m_complex>, MMatrix<m_complex> > (evals, evecs) ;
}
template std::pair<MVector<m_complex>, MMatrix<m_complex> > MMatrix<double>::eigenvectors() const;

///////////// OPERATORS

MMatrix<double>& operator *= (MMatrix<double>& m1,  MMatrix<double>  m2)
{ 
  MMatrix<double> out(m1.num_row(), m2.num_col());  
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1., (gsl_matrix*)m1._matrix, (gsl_matrix*)m2._matrix, 0., (gsl_matrix*)out._matrix); 
  m1 = out;
  return m1;
}

MMatrix<m_complex> & operator *= (MMatrix<m_complex>& m1,  MMatrix<m_complex>  m2)
{ 
  MMatrix<m_complex> out(m1.num_row(), m2.num_col()); 
  gsl_blas_zgemm( CblasNoTrans, CblasNoTrans, MAUS::Complex::complex(1.), (gsl_matrix_complex*)m1._matrix, 
                  (gsl_matrix_complex*)m2._matrix, MAUS::Complex::complex(0.), (gsl_matrix_complex*)out._matrix); 
  m1 = out; 
  return m1;
}

template <class Tmplt> std::ostream& operator<<(std::ostream& out, MMatrix<Tmplt> mat)
{
  out << mat.num_row() << " " << mat.num_col() << "\n";
  for(size_t i=1; i<=mat.num_row(); i++)
  {
    out << "  ";
    for(size_t j=1; j<mat.num_col(); j++)
      out << mat(i,j) << "   ";
    out << mat(i,mat.num_col()) << "\n";
  }
  return out;
}
template std::ostream& operator<<(std::ostream& out, MMatrix<double>    mat);
template std::ostream& operator<<(std::ostream& out, MMatrix<m_complex> mat);

template <class Tmplt> std::istream& operator>>(std::istream& in, MMatrix<Tmplt>& mat)
{
  size_t nr, nc;
  in >> nr >> nc;
  mat = MMatrix<Tmplt>(nr, nc);
  for(size_t i=1; i<=nr; i++)
    for(size_t j=1; j<=nc; j++)
      in >> mat(i,j);
  return in;
}
template std::istream& operator>>(std::istream& in, MMatrix<double>&    mat);
template std::istream& operator>>(std::istream& in, MMatrix<m_complex>& mat);


///////////////// INTERFACES
const gsl_matrix*         MMatrix_to_gsl(const MMatrix<double>&    m)  
{
  if(m._matrix == NULL) throw(Squeal(Squeal::recoverable, "Attempt to reference uninitialised matrix", "MMatrix_to_gsl"));
  return (gsl_matrix*)m._matrix;
}

const gsl_matrix_complex* MMatrix_to_gsl(const MMatrix<m_complex>& m) 
{
  if(m._matrix == NULL) throw(Squeal(Squeal::recoverable, "Attempt to reference uninitialised matrix", "MMatrix_to_gsl"));
  return (gsl_matrix_complex*)m._matrix;
}

MMatrix<double> re(MMatrix<m_complex> mc)
{
  MMatrix<double> md(mc.num_row(), mc.num_col());
  for(size_t i=1; i<=mc.num_row(); i++)
    for(size_t j=1; j<=mc.num_col(); j++)
      md(i,j) = MAUS::real(mc(i,j));
  return md;
}

MMatrix<double> im(MMatrix<m_complex> mc)
{
  MMatrix<double> md(mc.num_row(), mc.num_col());
  for(size_t i=1; i<=mc.num_row(); i++)
    for(size_t j=1; j<=mc.num_col(); j++)
      md(i,j) = MAUS::imag(mc(i,j));
  return md;
}

MMatrix<m_complex> complex(MMatrix<double> real)
{
  MMatrix<m_complex> mc(real.num_row(), real.num_col());
  for(size_t i=1; i<=mc.num_row(); i++)
    for(size_t j=1; j<=mc.num_col(); j++)
      mc(i,j) = MAUS::Complex::complex(real(i,j));
  return mc;
}

MMatrix<m_complex> complex(MMatrix<double> real, MMatrix<double> imaginary)
{
  if(real.num_row() != imaginary.num_row() || real.num_col() != imaginary.num_col()) 
    throw(Squeal(Squeal::recoverable, "Attempt to build complex matrix when real and imaginary matrix don't have the same size", "MMatrix<m_complex>::complex"));
  MMatrix<m_complex> mc(real.num_row(), real.num_col());
  for(size_t i=1; i<=mc.num_row(); i++)
    for(size_t j=1; j<=mc.num_col(); j++)
      mc(i,j) = MAUS::Complex::complex(real(i,j), imaginary(i,j));
  return mc;
}

template <class Tmplt>
MVector<Tmplt> MMatrix<Tmplt>::get_mvector(size_t column) const
{
  MVector<Tmplt> temp(num_row());
  for(size_t i=1; i<=num_row(); i++) temp(i) = operator()(i,column);
  return temp;
}
template MVector<double> MMatrix<double>::get_mvector(size_t column) const;
template MVector<m_complex> MMatrix<m_complex>::get_mvector(size_t column) const;

