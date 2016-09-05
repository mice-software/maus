/* This file is part of MAUS: http:// micewww.pp.rl.ac.uk:8080/projects/maus
 * 
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http:// www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include "Maths/Vector.hh"

#include <stdio.h>

#include <complex>
#include <vector>
#include <iostream>

#include "CLHEP/Matrix/Vector.h"
#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_vector_complex_double.h"

#include "Maths/Complex.hh"
#include "Utils/Exception.hh"

namespace MAUS {

// ############################
//  VectorBase (public)
// ############################

// *************************
//  Constructors
// *************************

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase() : vector_(NULL) {}
template VectorBase<double, gsl_vector>::VectorBase();
template VectorBase<complex, gsl_vector_complex>::VectorBase();

template <>
VectorBase<double, gsl_vector>::VectorBase(
  const VectorBase<double, gsl_vector>& original_instance)
    : vector_(NULL) {
  if (original_instance.vector_ != NULL) {
    build_vector(original_instance.vector_->size);
    gsl_vector_memcpy(vector_, original_instance.vector_);
  }
}

template <>
VectorBase<complex, gsl_vector_complex>::VectorBase(
  const VectorBase<complex, gsl_vector_complex>& original_instance)
    : vector_(NULL) {
  if (original_instance.vector_ != NULL) {
    build_vector(original_instance.vector_->size);
    gsl_vector_complex_memcpy(vector_, original_instance.vector_);
  }
}

template <>
VectorBase<double, gsl_vector>::VectorBase(
  const ::CLHEP::HepVector& hep_vector) : vector_(NULL) {
  size_t size = hep_vector.num_row();

  build_vector(size);
  for (size_t index = 1; index <= size; ++index) {
    (*this)(index) = hep_vector(index);
  }
}

template <>
VectorBase<complex, gsl_vector_complex>::VectorBase(
  const ::CLHEP::HepVector& hep_vector) : vector_(NULL) {
  size_t size = hep_vector.num_row();

  build_vector(size);
  for (size_t index = 1; index <= size; ++index) {
    (*this)(index) = MAUS::Complex::complex(hep_vector(index));
  }
}

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase(const size_t i) : vector_(NULL) {
  build_vector(i);
}
template VectorBase<double, gsl_vector>::VectorBase(const size_t i);
template VectorBase<complex, gsl_vector_complex>::VectorBase(const size_t i);

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase(
    const size_t size, const StdType  value) : vector_(NULL) {
  VectorBase<StdType, GslType>::build_vector(size);
  for (size_t i = 0; i < size; ++i) {
    (*this)[i] = value;
  }
}
template VectorBase<double, gsl_vector>::VectorBase(
  const size_t size, const double  value);
template VectorBase<complex, gsl_vector_complex>::VectorBase(
  const size_t size, const complex  value);

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase(
    const StdType* data, const size_t size) : vector_(NULL) {
  build_vector(size, data);
}
template VectorBase<double, gsl_vector>::VectorBase(
    const double* data, const size_t size);
template VectorBase<complex, gsl_vector_complex>::VectorBase(
    const complex* data, const size_t size);

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::VectorBase(
    const std::vector<StdType, std::allocator<StdType> >& std_vector)
    : vector_(NULL) {
  build_vector(std_vector.size());
  typename std::vector<StdType>::const_iterator element;
  int index = 0;
  for (element = std_vector.begin(); element < std_vector.end(); ++element) {
    // TODO(plane1@iit.edu): create a MAUS::Vector iterator
    (*this)[index++] = *element;
  }
}
template VectorBase<double, gsl_vector>::VectorBase(
  const std::vector<double, std::allocator<double> >& std_vector);
template VectorBase<complex, gsl_vector_complex>::VectorBase(
  const std::vector<complex, std::allocator<complex> >& std_vector);

template <typename StdType, typename GslType>
VectorBase<StdType, GslType>::~VectorBase() {
  delete_vector();
}
template VectorBase<double, gsl_vector>::~VectorBase();
template VectorBase<complex, gsl_vector_complex>::~VectorBase();

// *************************
//  Indexing Operators
// *************************

template <>
const double& VectorBase<double, gsl_vector>::operator[](const size_t i)
    const {
  if (vector_ == NULL) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "Attempted to index an empty vector.",
      "MAUS::VectorBase<double, gsl_vector>::operator[]()"));
  }

  return *gsl_vector_ptr(vector_, i);
}

template <>
const complex& VectorBase<complex, gsl_vector_complex>::operator[](
    const size_t i) const {
  if (vector_ == NULL) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "Attempted to index an empty vector.",
      "MAUS::VectorBase<complex, gsl_vector_complex>::operator[]()"));
  }

  return *gsl_vector_complex_ptr(vector_, i);
}

template <>
double& VectorBase<double, gsl_vector>::operator[](const size_t i) {
  if (vector_ == NULL) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "Attempted to index an empty vector.",
      "MAUS::VectorBase<double, gsl_vector>::operator[]()"));
  }

  return *gsl_vector_ptr(vector_, i);
}

template <>
complex& VectorBase<complex, gsl_vector_complex>::operator[](const size_t i) {
  if (vector_ == NULL) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "Attempted to index an empty vector.",
      "MAUS::VectorBase<complex, gsl_vector_complex>::operator[]()"));
  }

  return *gsl_vector_complex_ptr(vector_, i);
}

template<typename StdType, typename GslType>
StdType& VectorBase<StdType, GslType>::operator()(const size_t i) {
  return (*this)[i-1];
}
template double& VectorBase<double, gsl_vector>::operator()(const size_t i);
template complex& VectorBase<complex, gsl_vector_complex>::operator()(
  const size_t i);

template<typename StdType, typename GslType>
const StdType& VectorBase<StdType, GslType>::operator()(const size_t i) const {
  return (*this)[i-1];
}
template const double& VectorBase<double, gsl_vector>::operator()(
  const size_t i) const;
template const complex& VectorBase<complex, gsl_vector_complex>::operator()(
  const size_t i) const;

// *************************
//  Size Functions
// *************************

template <typename StdType, typename GslType>
const size_t VectorBase<StdType, GslType>::size() const {
  if (vector_ != NULL) {
    return vector_->size;
  }

  return 0;
}
template const size_t VectorBase<double, gsl_vector>::size() const;
template const size_t VectorBase<complex, gsl_vector_complex>::size() const;

// *************************
//  Subvector Functions
// *************************

template <>
VectorBase<double, gsl_vector> VectorBase<double, gsl_vector>::subvector(
    size_t begin_index, size_t end_index) const {
  if (vector_ == NULL) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "Attempted to create a subvector from an empty vector.",
      "MAUS::VectorBase<double, gsl_vector>::subvector()"));
  }

  VectorBase<double, gsl_vector> subvector;

  size_t sub_size = end_index-begin_index;
  if (sub_size > 0) {
    subvector = VectorBase<double, gsl_vector>(sub_size);
    for (size_t index = 0; index < sub_size; ++index) {
      *gsl_vector_ptr(subvector.vector_, index)
        = *gsl_vector_ptr(vector_, begin_index+index);
    }
  }

  return subvector;
}

template <> VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::subvector(
    size_t begin_index,
    size_t end_index) const {
  if (vector_ == NULL) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "Attempted to create a subvector from an empty vector.",
      "MAUS::VectorBase<complex, gsl_vector_complex>::subvector()"));
  }

  VectorBase<complex, gsl_vector_complex> subvector;

  size_t sub_size = end_index-begin_index;
  if (sub_size > 0) {
    subvector = VectorBase<complex, gsl_vector_complex>(sub_size);
    for (size_t index = 0; index < sub_size; ++index) {
      *gsl_vector_complex_ptr(subvector.vector_, index)
      = *gsl_vector_complex_ptr(vector_, begin_index+index);
    }
  }

  return subvector;
}

// *************************
//  Assignment Operators
// *************************

template <>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator=(
    const VectorBase<double, gsl_vector>& rhs) {
  if (&rhs != this) {
    if (vector_ == NULL) {
      // special case (like for a copy constructor call) where a non-null
      // vector is assigned to a null vector
      build_vector(rhs.vector_->size);
    } else if ((rhs.vector_ == NULL) || (vector_->size != rhs.vector_->size)) {
      throw(Exceptions::Exception(Exceptions::recoverable,
                   "Attempted to assign a vector of a different size.",
                   "VectorBase<double>::operator=()"));
    }

    gsl_vector_memcpy(vector_, rhs.vector_);
  }

  return *this;
}

template <> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator=(
  const VectorBase<complex, gsl_vector_complex>& rhs) {
  if (&rhs != this) {
    if (vector_ == NULL) {
      // special case (like for a copy constructor call) where a non-null
      // vector is assigned to a null vector
      build_vector(rhs.vector_->size);
    } else if ((rhs.vector_ == NULL) || (vector_->size != rhs.vector_->size)) {
      throw(Exceptions::Exception(Exceptions::recoverable,
                   "Attempted to assign a vector of a different size.",
                   "VectorBase<complex>::operator=()"));
    }

    gsl_vector_complex_memcpy(vector_, rhs.vector_);
  }

  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator+=(
    const VectorBase<double, gsl_vector>& rhs) {
  if (rhs.vector_ != NULL) {
    gsl_vector_add(vector_, rhs.vector_);
  }
  return *this;
}

template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator+=(
    const VectorBase<complex, gsl_vector_complex>& rhs) {
  if (rhs.vector_ != NULL) {
    gsl_vector_complex_add(vector_, rhs.vector_);
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator-=(
    const VectorBase<double, gsl_vector>& rhs) {
  if (rhs.vector_ != NULL) {
    gsl_vector_sub(vector_, rhs.vector_);
  }
  return *this;
}

template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator-=(
    const VectorBase<complex, gsl_vector_complex>& rhs) {
  if (rhs.vector_ != NULL) {
    gsl_vector_complex_sub(vector_, rhs.vector_);
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator*=(
    const VectorBase<double, gsl_vector>& rhs) {
  if (rhs.vector_ != NULL) {
    size_t size = this->size();
    if (rhs.size() != size) {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Attempted to perform the product of two vectors of different sizes.",
        "MAUS::VectorBase<double, gsl_vector>::operator*="));
    }

    gsl_vector_mul(vector_, rhs.vector_);
  } else {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Attempted to multiply a vector by an empty vector.",
        "MAUS::VectorBase<double, gsl_vector>::operator*="));
  }

  return *this;
}

template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator*=(
    const VectorBase<complex, gsl_vector_complex>& rhs) {
  if (rhs.vector_ != NULL) {
    size_t size = this->size();
    if (rhs.size() != size) {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Attempted to perform the product of two vectors of different sizes.",
        "MAUS::VectorBase<complex, gsl_vector_complex>::operator*="));
    }

    gsl_vector_complex_mul(vector_, rhs.vector_);
  } else {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Attempted to multiply a vector by an empty vector.",
        "MAUS::VectorBase<complex, gsl_vector_complex>::operator*="));
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator/=(
    const VectorBase<double, gsl_vector>& rhs) {
  if (rhs.vector_ != NULL) {
    size_t size = this->size();
    if (rhs.size() != size) {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Attempted to perform the product of two vectors of different sizes.",
        "MAUS::VectorBase<double, gsl_vector>::operator/="));
    }

    gsl_vector_div(vector_, rhs.vector_);
  } else {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Attempted to divide a vector by an empty vector.",
        "MAUS::VectorBase<double, gsl_vector>::operator/="));
  }

  return *this;
}

template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator/=(
    const VectorBase<complex, gsl_vector_complex>& rhs) {
  if (rhs.vector_ != NULL) {
    size_t size = this->size();
    if (rhs.size() != size) {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Attempted to perform the product of two vectors of different sizes.",
        "MAUS::VectorBase<complex, gsl_vector_complex>::operator/="));
    }

    gsl_vector_complex_div(vector_, rhs.vector_);
  } else {
      throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Attempted to divide a vector by an empty vector.",
        "MAUS::VectorBase<complex, gsl_vector_complex>::operator/="));
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator+=(
    const double& rhs) {
  if (vector_ != NULL) {
    gsl_vector_add_constant(vector_, rhs);
  }
  return *this;
}

template<>
VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator+=(
    const complex& rhs) {
  if (vector_ != NULL) {
    gsl_vector_complex_add_constant(vector_, rhs);
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator-=(
    const double& rhs) {
  if (vector_ != NULL) {
    gsl_vector_add_constant(vector_, -rhs);
  }
  return *this;
}

template<>
VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator-=(
    const complex& rhs) {
  if (vector_ != NULL) {
    gsl_vector_complex_add_constant(vector_, -rhs);
  }
  return *this;
}

template<>
VectorBase<double, gsl_vector>& VectorBase<double, gsl_vector>::operator*=(
    const double& rhs) {
  if (vector_ != NULL) {
    gsl_vector_scale(vector_, rhs);
  }
  return *this;
}

template<>
VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator*=(
    const complex& rhs) {
  if (vector_ != NULL) {
    gsl_vector_complex_scale(vector_, rhs);
  }
  return *this;
}

template<> VectorBase<double, gsl_vector>&
VectorBase<double, gsl_vector>::operator/=(const double& rhs) {
  if (vector_ != NULL) {
    gsl_vector_scale(vector_, 1./rhs);
  }
  return *this;
}

template<> VectorBase<complex, gsl_vector_complex>&
VectorBase<complex, gsl_vector_complex>::operator/=(const complex& rhs) {
  if (vector_ != NULL) {
    gsl_vector_complex_scale(vector_, 1./rhs);
  }
  return *this;
}

// *************************
//  Algebraic Operators
// *************************

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator+(
    const VectorBase<StdType, GslType>&  rhs) const {
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
    const VectorBase<StdType, GslType>&  rhs) const {
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
    const VectorBase<StdType, GslType>&  rhs) const {
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
    const VectorBase<StdType, GslType>&  rhs) const {
  return VectorBase<StdType, GslType>(*this) /= rhs;
}
template const VectorBase<double, gsl_vector>
VectorBase<double, gsl_vector>::operator/(
    const VectorBase<double, gsl_vector>&  rhs) const;
template const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator/(
    const VectorBase<complex, gsl_vector_complex>&  rhs) const;

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator+(
    const StdType& rhs) const {
  return VectorBase<StdType, GslType>(*this) += rhs;
}
template
const VectorBase<double, gsl_vector> VectorBase<double, gsl_vector>::operator+(
    const double& rhs) const;
template
const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator+(
    const complex& rhs) const;

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator-(
    const StdType& rhs) const {
  return VectorBase<StdType, GslType>(*this) -= rhs;
}
template
const VectorBase<double, gsl_vector> VectorBase<double, gsl_vector>::operator-(
    const double& rhs) const;
template
const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator-(
    const complex& rhs) const;

template <typename StdType, typename GslType>
const VectorBase<StdType, GslType> VectorBase<StdType, GslType>::operator*(
    const StdType& rhs) const {
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
    const StdType& rhs) const {
  return VectorBase<StdType, GslType>(*this) /= rhs;
}
template const VectorBase<double, gsl_vector>
VectorBase<double, gsl_vector>::operator/(const double& rhs) const;
template const VectorBase<complex, gsl_vector_complex>
VectorBase<complex, gsl_vector_complex>::operator/(const complex& rhs) const;

// *************************
//  Comparison Operators
// *************************

template <>
const bool VectorBase<double, gsl_vector>::operator==(
    const VectorBase<double, gsl_vector>& rhs) const {
  size_t this_size = size();
  if (this_size != rhs.size()) {
    return false;
  }

  if (vector_ != NULL) {
    for (size_t index = 0; index < this_size; ++index) {
      if (   *gsl_vector_ptr(vector_, index)
          != *gsl_vector_ptr(rhs.vector_, index)) {
        return false;
      }
    }
  } else if (rhs.vector_ != NULL) {
    return false;
  }

  return true;
}

template <>
const bool VectorBase<complex, gsl_vector_complex>::operator==(
    const VectorBase<complex, gsl_vector_complex>& rhs) const {
  size_t this_size = size();
  if (this_size != rhs.size()) {
    return false;
  }

  if (vector_ != NULL) {
    for (size_t index = 0; index < this_size; ++index) {
      if (   *gsl_vector_complex_ptr(vector_, index)
          != *gsl_vector_complex_ptr(rhs.vector_, index)) {
        return false;
      }
    }
  } else if (rhs.vector_ != NULL) {
    return false;
  }

  return true;
}

template <typename StdType, typename GslType>
const bool VectorBase<StdType, GslType>::operator!=(
    const VectorBase<StdType, GslType>& rhs) const {
  return !((*this) == rhs);
}

// ############################
//  VectorBase (protected)
// ############################

template <>
void VectorBase<double, gsl_vector>::delete_vector() {
  if (vector_ != NULL) {
    gsl_vector_free(vector_);
  }

  vector_ = NULL;
}

template <>
void VectorBase<complex, gsl_vector_complex>::delete_vector() {
  if (vector_ != NULL) {
    gsl_vector_complex_free(vector_);
  }

  vector_ = NULL;
}

template <>
void VectorBase<double, gsl_vector>::build_vector(const size_t size,
                                                  const bool initialize) {
  delete_vector();
  if (initialize) {
    vector_ = gsl_vector_calloc(size);
  } else {
    vector_ = gsl_vector_alloc(size);
  }
}

template <>
void VectorBase<complex, gsl_vector_complex>::build_vector(
    const size_t size, const bool initialize) {
  delete_vector();

  if (initialize) {
    vector_ = gsl_vector_complex_calloc(size);
  } else {
    vector_ = gsl_vector_complex_alloc(size);
  }
}

template <typename StdType, typename GslType>
void VectorBase<StdType, GslType>::build_vector(
    const size_t size, const StdType* data) {
  build_vector(size, false);
  for (size_t i = 0; i < this->size(); ++i) {
    (*this)[i] = data[i];
  }
}
template void VectorBase<double, gsl_vector>::build_vector(
    const size_t size, const double* data);
template void VectorBase<complex, gsl_vector_complex>::build_vector(
    const size_t size, const complex* data);

template <typename StdType, typename GslType>
GslType * VectorBase<StdType, GslType>::vector() {
  return vector_;
}
template gsl_vector * VectorBase<double, gsl_vector>::vector();
template gsl_vector_complex * VectorBase<complex, gsl_vector_complex>::vector();

// ############################
//  Vector (public)
// ############################

const Vector<double> Vector<double>::operator+(const Vector<double>& rhs)
    const {
  return VectorBase<double, gsl_vector>::operator+(rhs);
}

const Vector<double> Vector<double>::operator-(const Vector<double>& rhs)
    const {
  return VectorBase<double, gsl_vector>::operator-(rhs);
}

const Vector<double> Vector<double>::operator*(const Vector<double>& rhs)
    const {
  return VectorBase<double, gsl_vector>::operator*(rhs);
}

const Vector<double> Vector<double>::operator/(const Vector<double>& rhs)
    const {
  return VectorBase<double, gsl_vector>::operator/(rhs);
}

const Vector<double> Vector<double>::operator+(const double& rhs) const {
  return VectorBase<double, gsl_vector>::operator+(rhs);
}

const Vector<double> Vector<double>::operator-(const double& rhs) const {
  return VectorBase<double, gsl_vector>::operator-(rhs);
}

const Vector<double> Vector<double>::operator*(const double& rhs) const {
  return VectorBase<double, gsl_vector>::operator*(rhs);
}

const Vector<double> Vector<double>::operator/(const double& rhs) const {
  return VectorBase<double, gsl_vector>::operator/(rhs);
}

const Vector<complex> Vector<complex>::operator+(const Vector<complex>& rhs)
    const {
  return VectorBase<complex, gsl_vector_complex>::operator+(rhs);
}

const Vector<complex> Vector<complex>::operator-(const Vector<complex>& rhs)
    const {
  return VectorBase<complex, gsl_vector_complex>::operator-(rhs);
}

const Vector<complex> Vector<complex>::operator*(const Vector<complex>& rhs)
    const {
  return VectorBase<complex, gsl_vector_complex>::operator*(rhs);
}

const Vector<complex> Vector<complex>::operator/(const Vector<complex>& rhs)
    const {
  return VectorBase<complex, gsl_vector_complex>::operator/(rhs);
}

const Vector<complex> Vector<complex>::operator+(const complex& rhs) const {
  return VectorBase<complex, gsl_vector_complex>::operator+(rhs);
}

const Vector<complex> Vector<complex>::operator-(const complex& rhs) const {
  return VectorBase<complex, gsl_vector_complex>::operator-(rhs);
}

const Vector<complex> Vector<complex>::operator*(const complex& rhs) const {
  return VectorBase<complex, gsl_vector_complex>::operator*(rhs);
}

const Vector<complex> Vector<complex>::operator/(const complex& rhs) const {
  return VectorBase<complex, gsl_vector_complex>::operator/(rhs);
}

// ############################
//  Template Declarations
// ############################

template class VectorBase<double, gsl_vector>;
template class VectorBase<complex, gsl_vector_complex>;

template class Vector<double>;
template class Vector<complex>;

// ############################
//  Free Functions
// ############################

// ****************************
//  Conversion Functions
// ****************************

VectorBase<double, gsl_vector> real(
    const VectorBase<complex, gsl_vector_complex>& complex_vector) {
  VectorBase<double, gsl_vector> double_vector;

  if (complex_vector.vector_ != NULL) {
    double_vector = VectorBase<double, gsl_vector>(complex_vector.size());
    for (size_t i = 1; i <= complex_vector.size(); ++i) {
      double_vector(i) = real(complex_vector(i));
    }
  }
  return double_vector;
}

VectorBase<double, gsl_vector> imag(
    const VectorBase<complex, gsl_vector_complex>& complex_vector) {
  VectorBase<double, gsl_vector> double_vector;

  if (complex_vector.vector_ != NULL) {
    double_vector = VectorBase<double, gsl_vector>(complex_vector.size());
    for (size_t i = 1; i <= complex_vector.size(); ++i) {
      double_vector(i) = imag(complex_vector(i));
    }
  }
  return double_vector;
}

// ############################
//  Global Operators
// ############################

// *************************
//  Unitary Operators
// *************************

template <typename StdType>
Vector<StdType> operator-(const Vector<StdType>& operand) {
  size_t size = operand.size();
  Vector<StdType> vector(operand);
  for (size_t index = 0; index < size; ++index) {
    vector[index] = -vector[index];
  }

  return vector;
}
template Vector<double> operator-(const Vector<double>& operand);
template Vector<complex> operator-(const Vector<complex>& operand);

// *************************
//  Scalar Operators
// *************************

template <typename StdType>
Vector<StdType> operator*(const StdType&                scalar,
                                const Vector<StdType>&  vector)  {
  return vector.operator*(scalar);
}
template Vector<double> operator*(
    const double&         scalar,
    const Vector<double>& vector);
template
Vector<complex> operator*(
    const complex&          scalar,
    const Vector<complex>&  vector);

// *************************
//  Stream Operators
// *************************

template <typename StdType, typename GslType>
std::ostream& operator<<(
    std::ostream&                       out,
    const VectorBase<StdType, GslType>& vector) {
  size_t vector_size = vector.size();
  out << vector_size << " : ";
  for (size_t i = 0; i < vector_size; ++i) {
    out << " " << vector[i];
  }
  return out;
}
template std::ostream& operator<<(
  std::ostream&                                         out,
  const VectorBase<double, gsl_vector>&                 vector);
template std::ostream& operator<<(
  std::ostream&                                         out,
  const VectorBase<complex, gsl_vector_complex>&        vector);

template <typename StdType, typename GslType>
std::istream& operator>>(std::istream&                  in,
                         VectorBase<StdType, GslType>&  vector) {
  size_t n;
  in >> n;
  vector = VectorBase<StdType, GslType>(n);

  char dummy;
  in >> dummy;

  for (size_t i = 0; i < n; ++i) {
    in >> vector[i];
  }
  return in;
}
template std::istream& operator>>(
  std::istream&                                         in,
  VectorBase<double, gsl_vector>&                       vector);
template std::istream& operator>>(
  std::istream&                                         in,
  VectorBase<complex, gsl_vector_complex>&              vector);

}  // namespace MAUS
