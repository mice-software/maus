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
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a TYPE1 object which it will store and construct with the 
   * correct number of elements from the TYPE2 object passed into the operator() method.
   *
   * \param TYPE1* a pointer to a \a TYPE1 object
   */
  ConverterBase(TYPE1* d1):m_d1(d1),m_d2(0){}
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a TYPE1 object which it will store and construct with the 
   * correct number of elements from the TYPE2 object passed into the operator() method.
   *
   * \param TYPE1& a \a TYPE1 object
   */
  ConverterBase(TYPE1& d1):m_d1(&d1),m_d2(0){}
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a TYPE2 object which it will store and construct with the 
   * correct number of elements from the TYPE1 object passed into the operator() method.
   *
   * \param TYPE2* a pointer to a \a TYPE2 object
   */
  ConverterBase(TYPE2* d2):m_d1(0),m_d2(d2){}
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a TYPE2 object which it will store and construct with the 
   * correct number of elements from the TYPE1 object passed into the operator() method.
   *
   * \param TYPE2& a \a TYPE2 object
   */
  ConverterBase(TYPE2& d2):m_d1(0),m_d2(&d2){}

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
    virtual TYPE1* operator()(const TYPE2&) =0;
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
    virtual TYPE2* operator()(const TYPE1&) =0;
 protected:
  /*! \var long m_evtcnt
   * \brief Event counter
   *
   * Event counting functionality provided by the abstract base class.
   */
  long m_evtcnt;
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
