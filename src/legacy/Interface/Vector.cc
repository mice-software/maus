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

#include <complex>
#include <vector>

#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_vector_complex_double.h"

#include "Interface/Complex.hh"
#include "Interface/Vector.hh"

using namespace MAUS;

namespace MAUS
{

template class GslVector<gsl_vector>;
template class GslVector<gsl_vector_complex>;
template class Vector<gsl_vector>;
template class Vector<MAUS::complex>;

//############################
// GslVector (public)
//############################

//protected specialization dependancies
template <> void GslVector<gsl_vector>::delete_vector();
template <> void GslVector<gsl_vector_complex>::delete_vector();

template <typename GslType> GslVector<GslType>::GslVector() : vector_(NULL)
{ }
/*
template<> GslVector<gsl_vector>::GslVector()
{
  vector_ = NULL;
}
template<> GslVector<MAUS::complex>::GslVector()
{
  vector_ = NULL;
}
*/
template <typename GslType>
GslVector<GslType>::GslVector(size_t i) : vector_(NULL)
{ 
  build_vector(i); 
}

template<typename GslType>
GslVector<GslType>::GslVector(const GslVector<GslType>& original_instance)
{
  (*this) = original_instance;
}

template <typename GslType> template <typename GslType2>
GslVector<GslType>::GslVector(const GslVector<GslType2>& mv) : vector_(NULL)
{
  *this = mv;
}

template <class GslType>
GslVector<GslType>::~GslVector()
{
  delete_vector();
}

template <class GslType>
size_t GslVector<GslType>::size() const
{
  if(vector_ != NULL)
  {
    return vector_->size;
  }

  return 0;
}

//TODO: use gsl_vector_ptr and gsl_vector_complex_ptr instead of operator[]
template <class GslType>
GslVector<GslType> GslVector<GslType>::subvector(size_t begin_index, size_t end_index)
                                      const
{
  GslVector<GslType> subvector(end_index-begin_index);
  for(size_t index=begin_index; index<end_index; ++index)
  {
    subvector[index] = this[begin_index+index];
  }
  return subvector;
}

//*************************
// Assignment Operators
//*************************

template <>
GslVector<gsl_vector>& GslVector<gsl_vector>::operator=(const GslVector<gsl_vector>& rhs)
{ 
  if (&rhs != this)
  {
    delete_vector();
    
    if(rhs.vector_ == NULL)
    {
      vector_ = NULL;
    }
    else
    {
      vector_ = gsl_vector_alloc(rhs.size());
      gsl_vector_memcpy(vector_, rhs.vector_);
    }
  }
  
  return *this;
}

template <>
GslVector<gsl_vector_complex>& GslVector<gsl_vector_complex>::operator=(const GslVector<gsl_vector_complex>& rhs)
{ 
  if (&rhs != this)
  {
    delete_vector();
    
    if(rhs.vector_ == NULL)
    {
      vector_ = NULL;
    }
    else
    {
      vector_
        = gsl_vector_complex_alloc(rhs.size()); 
      gsl_vector_complex_memcpy(vector_,
                                rhs.vector_);  
    }
  }
  
  return *this;
}

template<>
GslVector<gsl_vector>& GslVector<gsl_vector>::operator+=(const GslVector<gsl_vector>& rhs)
{
  gsl_vector_add(vector_, rhs.vector_);
  return *this;
}

template<>
GslVector<gsl_vector_complex>& GslVector<gsl_vector_complex>::operator+=(const GslVector<gsl_vector_complex>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    gsl_vector_complex_add(vector_, rhs.vector_);
  }
  return *this;
}

template<class GslType>
GslVector<GslType>& GslVector<GslType>::operator-=(const GslVector<GslType>& rhs)
{
  return (*this) += (-rhs);
}

template<class GslType>
GslVector<GslType>& GslVector<GslType>::operator*=(const GslVector<GslType>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    size_t size = this->size();
    if (rhs.size() != size)
    {
      throw(Squeal(
        Squeal::recoverable,
        "Attempted to perform the product of two vectors of different sizes.",
        "MAUS::GslVector<GslType>::operator*="));
    }
    
    //TODO: use gsl_vector_ptr and gsl_vector_complex_ptr instead of operator[]
    for(size_t index=0; index<size; ++index)
    {
      (*this)[index] *= rhs[index];
    }
  }
}

//*************************
// Algebraic Operators
//*************************

template <class GslType>
const GslVector<GslType> GslVector<GslType>::operator+(const GslVector<GslType>&  rhs) const
{
  return GslVector<GslType>(*this) += rhs;
}

template <class GslType>
const GslVector<GslType> GslVector<GslType>::operator-(const GslVector<GslType>&  rhs) const
{
  return GslVector<GslType>(*this) -= rhs;
}

template <class GslType>
const GslVector<GslType> GslVector<GslType>::operator*(const GslVector<GslType>&  rhs) const
{
  return GslVector<GslType>(*this) *= rhs;
}

//*************************
// Comparison Operators
//*************************

//TODO: use gsl_vector_ptr and gsl_vector_complex_ptr instead of operator[]
template <class GslType>
const bool GslVector<GslType>::operator==(const GslVector<GslType>& rhs) const
{
  size_t this_size = size();
  if(this_size != rhs.size())
  {
    return false;
  }
    
  for(size_t index=0; index<size; ++index)
  {
    if((*this)[index] != rhs[index])
    {
      return false;
    }
  }

  return true;
}

template <class GslType>
const bool GslVector<GslType>::operator!=(const GslVector<GslType>& rhs) const
{
  return !((*this) == rhs);
}

//############################
// GslVector (protected)
//############################

template <typename GslType>
void GslVector<GslType>::build_vector(size_t size)
{
  if(vector_ != NULL)
  {
    gsl_vector_free(vector_);
  }
  vector_ = gsl_vector_alloc(size);
}

template <>
void GslVector<gsl_vector>::delete_vector()
{
  if(vector_ != NULL)
  {
    gsl_vector_free(vector_);
  }
}

template <>
void GslVector<gsl_vector_complex>::delete_vector()
{
  if(vector_ != NULL)
  {
    gsl_vector_complex_free(vector_);
  }
}

//****************************
// Conversion Functions
//****************************

GslVector<gsl_vector> real(const GslVector<gsl_vector_complex>& complex_vector)
{
  GslVector<gsl_vector> double_vector(complex_vector.size());
  for(size_t i=1; i<=complex_vector.size(); ++i)
  {
    double_vector(i) = MAUS::Complex::real(complex_vector(i));
  }
  return double_vector;
}

GslVector<gsl_vector> imag(const GslVector<gsl_vector_complex>& complex_vector)
{
  GslVector<gsl_vector> double_vector(complex_vector.size());
  for(size_t i=1; i<=complex_vector.size(); ++i)
  {
    double_vector(i) = MAUS::Complex::imag(complex_vector(i));
  }
  return double_vector;
}

} //namespace MAUS

//############################
// Vector (public)
//############################

//*************************
// Constructors
//*************************

template <typename StdType>
Vector<StdType>::Vector(size_t size, StdType  value) : vector_(NULL)
{
  build_vector(size);
  for(size_t i=0; i<size; ++i)
  {
    operator[](i) = value;
  }
}

template <typename StdType>
Vector<StdType>::Vector(const StdType* array_beginning,
                        const StdType* array_end) : vector_(NULL)
{
  build_vector(array_beginning, array_end);
}

template <typename StdType>
Vector<StdType>::Vector(
                              const std::vector<StdType, std::allocator<StdType> >& std_vector) : vector_(NULL)
{
  //FIXME: I don't like the assumption that the address of
  //       std::vector::operator[] is a pointer to a contiguous block of
  //       memory. The implementation could be a linked list or something
  //       that is not an array. Just pass the vector instance to build_vector
  //       and use an iterator.
  build_vector(&std_vector[0], &std_vector.back()+1);
}

//*************************
// Indexing Operators
//*************************

template <>
const double& Vector<double>::operator[](const size_t i) const
{
  return *gsl_vector_ptr(vector_, i);
}

template <>
const complex& Vector<complex>::operator[](const size_t i) const
{
  return *gsl_vector_complex_ptr(vector_, i);
}

template <>
double& Vector<double>::operator[](const size_t i)
{
  return *gsl_vector_ptr(vector_, i);
}

template <>
complex& Vector<complex>::operator[](const size_t i)
{
  return *gsl_vector_complex_ptr(vector_, i);
}

template<typename StdType>
StdType& Vector<StdType>::operator()(const size_t i)
{
  return (*this)[i-1];
}

template<typename StdType>
const StdType& Vector<StdType>::operator()(const size_t i) const
{
  return (*this)[i-1];
}

//*************************
// Assignment Operators
//*************************

template<>
Vector<gsl_vector>& Vector<double>::operator*=(const double& rhs)
{
  gsl_vector_scale(vector_, rhs);
  return *this;
}

template<>
Vector<gsl_vector_complex>& Vector<complex>::operator*=(const complex& rhs)
{
  gsl_vector_complex_scale(vector_, rhs);
  return *this;
}

template<class StdType>
Vector<StdType>& Vector<StdType>::operator/=(const StdType& rhs)
{
  (*this) *= 1.0 / rhs;
  return *this;
}

//*************************
// Algebraic Operators
//*************************

template <class StdType>
const Vector<StdType> Vector<StdType>::operator*(const StdType&          rhs) const
{
  return Vector<StdType>(*this) *= rhs;
}

template <class StdType>
const Vector<StdType> Vector<StdType>::operator/(const StdType&          rhs) const
{
  return Vector<StdType>(*this) /= rhs;
}

//############################
// Vector (protected)
//############################

template <class GslType>
void Vector<GslType>::build_vector(const GslType* data_begin,
                                      const GslType* data_end)
{
  build_vector(data_end - data_begin);
  for(size_t i=0; i<size(); ++i)
  {
    operator()(i+1) = data_begin[i];
  }
}

//############################
// Free Functions/Operators
//############################

//*************************
// Unitary Operators
//*************************

template <class GslType>
MAUS::GslVector<GslType> operator-(const MAUS::GslVector<GslType>& operand)
{
  size_t size = operand.size();
  GslVector<GslType> vector(operand);
  for(size_t index=0; index<size; ++index)
  {
    vector[index] = -vector[index];
  }

  return vector;
}
template MAUS::GslVector<gsl_vector> operator-(
  const MAUS::GslVector<gsl_vector>& operand);
template MAUS::GslVector<gsl_vector_complex> operator-(
  const MAUS::GslVector<gsl_vector_complex>& operand);

//*************************
// Scalar Operators
//*************************

template <typename GslType, typename StdType>
MAUS::GslVector<GslType> operator*(const StdType&                   scalar,
                                   const MAUS::GslVector<GslType>&  vector) 
{
  return vector * scalar;
}
template MAUS::GslVector<gsl_vector> operator*(
  const double&                      scalar,
  const MAUS::GslVector<gsl_vector>& vector);
template
MAUS::GslVector<gsl_vector_complex> operator*(
  const complex&                              scalar,
  const MAUS::GslVector<gsl_vector_complex>&  vector);

//*************************
// Stream Operators
//*************************

template <class GslType>
std::ostream& operator<<(std::ostream&        out,
                         const GslVector<GslType>& vector)
{
  out << "(";
  size_t size = vector.size();
  for(size_t index=1; index<=size; ++index)
  {
    out << vector(index);
    if (index < size)
    {
      out << ", ";
    }
  }
  out << ")" << std::endl;
  return out; 
}
template std::ostream& operator<<(std::ostream&        out,
                                  const GslVector<gsl_vector>& vector);
template std::ostream& operator<<(std::ostream&        out,
                                  const GslVector<gsl_vector_complex>& vector);

//TODO: need operator>>()
