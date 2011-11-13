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
#include <iostream>
#include <stdio.h>

#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_vector_complex_double.h"

#include "Interface/Complex.hh"
#include "Interface/Vector.hh"

using namespace MAUS;

namespace MAUS
{

//############################
// VectorBase (public)
//############################

//*************************
// Constructors
//*************************

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase() : vector_(NULL)
{ }
template VectorBase<double, gsl_vector>::VectorBase();
template VectorBase<complex, gsl_vector_complex>::VectorBase();

template<typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase(
  const VectorBase<StdType, GslType>& original_instance)
  : vector_(NULL)
{
  (*this) = original_instance;
}
template VectorBase<double, gsl_vector>::VectorBase(
  const VectorBase<double, gsl_vector>& original_instance);
template VectorBase<complex, gsl_vector_complex>::VectorBase(
  const VectorBase<complex, gsl_vector_complex>& original_instance);

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase(size_t i) : vector_(NULL)
{ 
  build_vector(i); 
}
template VectorBase<double, gsl_vector>::VectorBase(size_t i);
template VectorBase<complex, gsl_vector_complex>::VectorBase(size_t i);
  
template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase(size_t size, StdType  value)
{
  this->vector_ = NULL;
  VectorBase<StdType, GslType>::build_vector(size);
  for(size_t i=0; i<size; ++i)
  {
    (*this)[i] = value;
  }
}
template VectorBase<double, gsl_vector>::VectorBase(size_t size, double  value);
template VectorBase<complex, gsl_vector_complex>::VectorBase(
  size_t size, complex  value);
  
template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase(const StdType* array_beginning,
                                         const StdType* array_end)
{
  this->vector_ = NULL;
  build_vector(array_beginning, array_end);
}
template VectorBase<double, gsl_vector>::VectorBase(
  const double* array_beginning, const double* array_end);
template VectorBase<complex, gsl_vector_complex>::VectorBase(
  const complex* array_beginning, const complex* array_end);

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase(
  const std::vector<StdType, std::allocator<StdType> >& std_vector)
{
  this->vector_ = NULL;
  //FIXME: I don't like the assumption that the address of
  //       std::vector::operator[] is a pointer to a contiguous block of
  //       memory. The implementation could be a linked list or something
  //       that is not an array. Just pass the vector instance to build_vector
  //       and use an iterator.
  build_vector(&std_vector[0], &std_vector.back()+1);
}
template VectorBase<double, gsl_vector>::VectorBase(
  const std::vector<double, std::allocator<double> >& std_vector);
template VectorBase<complex, gsl_vector_complex>::VectorBase(
  const std::vector<complex, std::allocator<complex> >& std_vector);

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::~VectorBase()
{
  delete_vector();
}
template VectorBase<double, gsl_vector>::~VectorBase();
template VectorBase<complex, gsl_vector_complex>::~VectorBase();

//*************************
// Indexing Operators
//*************************

template <>
const double& VectorBase<double, gsl_vector>::operator[](const size_t i) const
{
  if (vector_ == NULL)
  {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to index an empty vector.",
      "MAUS::VectorBase<double, gsl_vector>::operator[]()"));
  }

  return *gsl_vector_ptr(vector_, i);
}

template <>
const complex& VectorBase<complex, gsl_vector_complex>::operator[](
  const size_t i) const
{
  if (vector_ == NULL)
  {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to index an empty vector.",
      "MAUS::VectorBase<complex, gsl_vector_complex>::operator[]()"));
  }

  return *gsl_vector_complex_ptr(vector_, i);
}

template <>
double& VectorBase<double, gsl_vector>::operator[](const size_t i)
{
  if (vector_ == NULL)
  {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to index an empty vector.",
      "MAUS::VectorBase<double, gsl_vector>::operator[]()"));
  }

  return *gsl_vector_ptr(vector_, i);
}

template <>
complex& VectorBase<complex, gsl_vector_complex>::operator[](const size_t i)
{
  if (vector_ == NULL)
  {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to index an empty vector.",
      "MAUS::VectorBase<complex, gsl_vector_complex>::operator[]()"));
  }

  return *gsl_vector_complex_ptr(vector_, i);
}

template<typename StdType, typename GslType>
StdType& VectorBase<StdType, GslType>::operator()(const size_t i)
{
  return (*this)[i-1];
}
template double& VectorBase<double, gsl_vector>::operator()(const size_t i);
template complex& VectorBase<complex, gsl_vector_complex>::operator()(
  const size_t i);

template<typename StdType, typename GslType>
const StdType& VectorBase<StdType, GslType>::operator()(const size_t i) const
{
  return (*this)[i-1];
}
template const double& VectorBase<double, gsl_vector>::operator()(
  const size_t i) const;
template const complex& VectorBase<complex, gsl_vector_complex>::operator()(
  const size_t i) const;

//*************************
// Size Functions
//*************************

template <typename StdType, typename GslType>
size_t VectorBase<StdType, GslType>::size() const
{
  if(vector_ != NULL)
  {
    return vector_->size;
  }

  return 0;
}
template size_t VectorBase<double, gsl_vector>::size() const;
template size_t VectorBase<complex, gsl_vector_complex>::size() const;

//*************************
// Subvector Functions
//*************************

template <>
VectorBase<double, gsl_vector> VectorBase<double, gsl_vector>::subvector(
  size_t begin_index, size_t end_index) const
{
  if(vector_ == NULL)
  {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to create a subvector from an empty vector.",
      "MAUS::VectorBase<double, gsl_vector>::subvector()"));
  }

  VectorBase<double, gsl_vector> subvector;

  size_t sub_size = end_index-begin_index;
  if (sub_size > 0)
  {
    subvector = VectorBase<double, gsl_vector>(sub_size);
    for(size_t index=0; index<sub_size; ++index)
    {
      *gsl_vector_ptr(subvector.vector_, index)
        = *gsl_vector_ptr(vector_, begin_index+index);

    }
  }

  return subvector;
}

template <> VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::subvector(
  size_t begin_index,
  size_t end_index) const
{
  if(vector_ == NULL)
  {
    throw(Squeal(
      Squeal::recoverable,
      "Attempted to create a subvector from an empty vector.",
      "MAUS::VectorBase<complex, gsl_vector_complex>::subvector()"));
  }
  
  VectorBase<complex, gsl_vector_complex> subvector;
  
  size_t sub_size = end_index-begin_index;
  if (sub_size > 0)
  {
    subvector = VectorBase<complex, gsl_vector_complex>(sub_size);
    for(size_t index=0; index<sub_size; ++index)
    {
      *gsl_vector_complex_ptr(subvector.vector_, index)
      = *gsl_vector_complex_ptr(vector_, begin_index+index);
      
    }
  }
  
  return subvector;
}

//*************************
// Assignment Operators
//*************************

template <>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator=(
  const VectorBase<double, gsl_vector>& rhs)
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

template <> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator=(
  const VectorBase<complex, gsl_vector_complex>& rhs)
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
      vector_ = gsl_vector_complex_alloc(rhs.size()); 
      gsl_vector_complex_memcpy(vector_, rhs.vector_);  
    }
  }
  
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator+=(
  const VectorBase<double, gsl_vector>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    gsl_vector_add(vector_, rhs.vector_);
  }
  return *this;
}

template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator+=(
  const VectorBase<complex, gsl_vector_complex>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    gsl_vector_complex_add(vector_, rhs.vector_);
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator-=(
  const VectorBase<double, gsl_vector>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    gsl_vector_sub(vector_, rhs.vector_);
  }
  return *this;
}

template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator-=(
  const VectorBase<complex, gsl_vector_complex>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    gsl_vector_complex_sub(vector_, rhs.vector_);
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator*=(
  const VectorBase<double, gsl_vector>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    size_t size = this->size();
    if (rhs.size() != size)
    {
      throw(Squeal(
        Squeal::recoverable,
        "Attempted to perform the product of two vectors of different sizes.",
        "MAUS::VectorBase<double, gsl_vector>::operator*="));
    }
    
    gsl_vector_mul(vector_, rhs.vector_);
  }
  
  return *this;
}

template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator*=(
  const VectorBase<complex, gsl_vector_complex>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    size_t size = this->size();
    if (rhs.size() != size)
    {
      throw(Squeal(
        Squeal::recoverable,
        "Attempted to perform the product of two vectors of different sizes.",
        "MAUS::VectorBase<complex, gsl_vector_complex>::operator*="));
    }
    
    gsl_vector_complex_mul(vector_, rhs.vector_);
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator/=(
  const VectorBase<double, gsl_vector>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    size_t size = this->size();
    if (rhs.size() != size)
    {
      throw(Squeal(
        Squeal::recoverable,
        "Attempted to perform the product of two vectors of different sizes.",
        "MAUS::VectorBase<double, gsl_vector>::operator/="));
    }
    
    gsl_vector_div(vector_, rhs.vector_);
  }
  
  return *this;
}

template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator/=(
  const VectorBase<complex, gsl_vector_complex>& rhs)
{
  if (rhs.vector_ != NULL)
  {
    size_t size = this->size();
    if (rhs.size() != size)
    {
      throw(Squeal(
        Squeal::recoverable,
        "Attempted to perform the product of two vectors of different sizes.",
        "MAUS::VectorBase<complex, gsl_vector_complex>::operator/="));
    }
    
    gsl_vector_complex_div(vector_, rhs.vector_);
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator*=(
  const double& rhs)
{
  if (vector_ != NULL)
  {
    gsl_vector_scale(vector_, rhs);
  }
  return *this;
}

template<>
VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator*=(
  const complex& rhs)
{
  if (vector_ != NULL)
  {
    gsl_vector_complex_scale(vector_, rhs);
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator/=(
  const double& rhs)
{
  if (vector_ != NULL)
  {
    gsl_vector_scale(vector_, 1./rhs);
  }
  return *this;
}
template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator/=(
  const complex& rhs)
{
  if (vector_ != NULL)
  {
    gsl_vector_complex_scale(vector_, 1./rhs);
  }
  return *this;
}

//*************************
// Algebraic Operators
//*************************

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator+(
  const VectorBase<StdType, GslType>&  rhs) const
{
  return VectorBase<StdType, GslType>(*this) += rhs;
}
template const VectorBase<double, gsl_vector>
VectorBase<double, gsl_vector>::operator+(
  const VectorBase<double, gsl_vector>&  rhs) const;
template
const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator+(
  const VectorBase<complex, gsl_vector_complex>&  rhs) const;

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator-(
  const VectorBase<StdType, GslType>&  rhs) const
{
  return VectorBase<StdType, GslType>(*this) -= rhs;
}
template const VectorBase<double, gsl_vector>
VectorBase<double, gsl_vector>::operator-(
  const VectorBase<double, gsl_vector>&  rhs) const;
template
const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator-(
  const VectorBase<complex, gsl_vector_complex>&  rhs) const;

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator*(
  const VectorBase<StdType, GslType>&  rhs) const
{
  return VectorBase<StdType, GslType>(*this) *= rhs;
}
template const VectorBase<double, gsl_vector>
VectorBase<double, gsl_vector>::operator*(
  const VectorBase<double, gsl_vector>&  rhs) const;
template const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator*(
  const VectorBase<complex, gsl_vector_complex>&  rhs) const;

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator/(
  const VectorBase<StdType, GslType>&  rhs) const
{
  return VectorBase<StdType, GslType>(*this) /= rhs;
}
template const VectorBase<double, gsl_vector>
VectorBase<double, gsl_vector>::operator/(
  const VectorBase<double, gsl_vector>&  rhs) const;
template const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator/(
  const VectorBase<complex, gsl_vector_complex>&  rhs) const;

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator*(
  const StdType& rhs) const
{
  return VectorBase<StdType, GslType>(*this) *= rhs;
}
template
const VectorBase<double, gsl_vector> VectorBase<double, gsl_vector>::operator*(
  const double& rhs) const;
template
const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator*(
  const complex& rhs) const;

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator/(
  const StdType& rhs)
  const
{
  return VectorBase<StdType, GslType>(*this) /= rhs;
}
template const VectorBase<double, gsl_vector>
VectorBase<double, gsl_vector>::operator/(const double& rhs) const;
template const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator/(const complex& rhs) const;

//*************************
// Comparison Operators
//*************************

template <>
const bool VectorBase<double, gsl_vector>::operator==(
  const VectorBase<double, gsl_vector>& rhs) const
{
  size_t this_size = size();
  if(this_size != rhs.size())
  {
    return false;
  }

  if (vector_ != NULL)
  {
    for(size_t index=0; index<this_size; ++index)
    {
      if(*gsl_vector_ptr(vector_, index) != *gsl_vector_ptr(rhs.vector_, index))
      {
        return false;
      }
    }
  }
  else if (rhs.vector_ != NULL)
  {
    return false;
  }

  return true;
}

template <>
const bool VectorBase<complex, gsl_vector_complex>::operator==(
  const VectorBase<complex, gsl_vector_complex>& rhs) const
{
  size_t this_size = size();
  if(this_size != rhs.size())
  {
    return false;
  }
  
  if (vector_ != NULL)
  {
    for(size_t index=0; index<this_size; ++index)
    {
      if(   *gsl_vector_complex_ptr(vector_, index)
         != *gsl_vector_complex_ptr(rhs.vector_, index))
      {
        return false;
      }
    }
  }
  else if (rhs.vector_ != NULL)
  {
    return false;
  }
  
  return true;
}

template <typename StdType, typename GslType>
const bool VectorBase<StdType, GslType>::operator!=(
  const VectorBase<StdType, GslType>& rhs) const
{
  return !((*this) == rhs);
}

//############################
// VectorBase (protected)
//############################

template <>
void VectorBase<double, gsl_vector>::delete_vector()
{
  if(vector_ != NULL)
  {
    gsl_vector_free(vector_);
  }
}

template <>
void VectorBase<complex, gsl_vector_complex>::delete_vector()
{
  if(vector_ != NULL)
  {
    gsl_vector_complex_free(vector_);
  }
}

template <>
void VectorBase<double, gsl_vector>::build_vector(size_t size)
{
  delete_vector();
  vector_ = gsl_vector_alloc(size);
}

template <>
void VectorBase<complex, gsl_vector_complex>::build_vector(size_t size)
{
  delete_vector();
  vector_ = gsl_vector_complex_alloc(size);
}

template <typename StdType, typename GslType>
void VectorBase<StdType, GslType>::build_vector(
  const StdType* data_begin, const StdType* data_end)
{
  VectorBase<StdType, GslType>::build_vector(data_end - data_begin);
  for(size_t i=0; i<this->size(); ++i)
  {
    (*this)[i] = data_begin[i];
  }
}
template void VectorBase<double, gsl_vector>::build_vector(
  const double* data_begin, const double* data_end);
template void VectorBase<complex, gsl_vector_complex>::build_vector(
  const complex* data_begin, const complex* data_end);

//############################
// Template Declarations
//############################

template class VectorBase<double, gsl_vector>;
template class VectorBase<MAUS::complex, gsl_vector_complex>;

template class Vector<double>;
template class Vector<MAUS::complex>;

//############################
// Free Functions
//############################

//****************************
// Conversion Functions
//****************************

VectorBase<double, gsl_vector> real(
  const VectorBase<complex, gsl_vector_complex>& complex_vector)
{
  VectorBase<double, gsl_vector> double_vector;

  if(complex_vector.vector_ != NULL)
  {
    double_vector = VectorBase<double, gsl_vector>(complex_vector.size());
    for(size_t i=1; i<=complex_vector.size(); ++i)
    {
      *gsl_vector_ptr(double_vector.vector_, i)
      = MAUS::Complex::real(*gsl_vector_complex_ptr(complex_vector.vector_, i));
    }
  }
  return double_vector;
}

VectorBase<double, gsl_vector> imag(
  const VectorBase<complex, gsl_vector_complex>& complex_vector)
{
  VectorBase<double, gsl_vector> double_vector;

  if(complex_vector.vector_ != NULL)
  {
    double_vector = VectorBase<double, gsl_vector>(complex_vector.size());
    for(size_t i=1; i<=complex_vector.size(); ++i)
    {
      *gsl_vector_ptr(double_vector.vector_, i)
      = MAUS::Complex::imag(*gsl_vector_complex_ptr(complex_vector.vector_, i));
    }
  }
  return double_vector;
}

} //namespace MAUS

//############################
// Global Operators
//############################

//*************************
// Unitary Operators
//*************************

template <typename StdType>
MAUS::Vector<StdType> operator-(const MAUS::Vector<StdType>& operand)
{
  size_t size = operand.size();
  Vector<StdType> vector(operand);
  for(size_t index=0; index<size; ++index)
  {
    vector[index] = -vector[index];
  }

  return vector;
}
template MAUS::Vector<double> operator-(const MAUS::Vector<double>& operand);
template MAUS::Vector<complex> operator-(const MAUS::Vector<complex>& operand);

//*************************
// Scalar Operators
//*************************

template <typename StdType>
MAUS::Vector<StdType> operator*(const StdType&                scalar,
                                const MAUS::Vector<StdType>&  vector) 
{
  return vector.operator*(scalar);
}
template MAUS::Vector<double> operator*(
  const double&               scalar,
  const MAUS::Vector<double>& vector);
template
MAUS::Vector<complex> operator*(
  const complex&                scalar,
  const MAUS::Vector<complex>&  vector);

//*************************
// Stream Operators
//*************************

template <typename StdType>
std::ostream& operator<<(std::ostream&          out,
                         const Vector<StdType>& vector)
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
template std::ostream& operator<<(std::ostream&           out,
                                  const Vector<double>&   vector);
template std::ostream& operator<<(std::ostream&           out,
                                  const Vector<complex>&  vector);

//TODO: need operator>>()
