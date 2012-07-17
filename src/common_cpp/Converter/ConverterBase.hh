/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef CONVERTERBASE_H
#define CONVERTERBASE_H

/*! \class ConverterBase
 * \brief Abstract base class for converters.
 *
 * ConverterBase is a functor (function object) class template which acts
 * to ensure that all converters define as a minimum the correct operator().
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 *
 * 
 */
template<class TYPE1, class TYPE2>
  class ConverterBase {
 public:
    // ! Destructor
    virtual ~ConverterBase() {}
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a TYPE1 object which it will store and construct with the 
   * correct number of elements from the TYPE2 object passed into the operator() method.
   *
   * \param TYPE1* a pointer to a \a TYPE1 object
   */
  explicit ConverterBase(TYPE1* d1) : m_d1(d1), m_d2(0) {}
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a TYPE1 object which it will store and construct with the 
   * correct number of elements from the TYPE2 object passed into the operator() method.
   *
   * \param TYPE1& a \a TYPE1 object
   */
  explicit ConverterBase(TYPE1& d1) : m_d1(&d1), m_d2(0) {}
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a TYPE2 object which it will store and construct with the 
   * correct number of elements from the TYPE1 object passed into the operator() method.
   *
   * \param TYPE2* a pointer to a \a TYPE2 object
   */
  explicit ConverterBase(TYPE2* d2) : m_d1(0), m_d2(d2) {}
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a TYPE2 object which it will store and construct with the 
   * correct number of elements from the TYPE1 object passed into the operator() method.
   *
   * \param TYPE2& a \a TYPE2 object
   */
  explicit ConverterBase(TYPE2& d2) : m_d1(0), m_d2(&d2) {}

  /*!
   * \brief Perform conversion
   *
   * The \a operator() member function takes a parameter of type 
   * \a TYPE2 and performs the conversion before returning
   * a type \a TYPE1. In this abstract base class this is a pure
   * virtual function.
   *
   * \param DATA_TYPE_2 The input data to be converted
   * \return The output data after conversion
   */
    virtual TYPE1* operator()(const TYPE2&) = 0;
  /*!
   * \brief Perform conversion
   *
   * The \a operator() member function takes a parameter of type 
   * \a TYPE1 and performs the conversion before returning
   * a type \a TYPE2. In this abstract base class this is a pure
   * virtual function.
   *
   * \param DATA_TYPE_1 The input data to be converted
   * \return The output data after conversion
   */
    virtual TYPE2* operator()(const TYPE1&) = 0;
 protected:
  /*! \var int m_evtcnt
   * \brief Event counter
   *
   * Event counting functionality provided by the abstract base class.
   */
  int m_evtcnt;
  /*! \var TYPE1* m_d1
   * \brief pointer to the TYPE1 object being constructed
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  TYPE1* m_d1;
  /*! \var TYPE2* m_d2
   * \brief pointer to the TYPE2 object being constructed
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  TYPE2* m_d2;
};


#endif
