/*!
 * \file DigitsProcessor.h
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 *
 * This file contains processor objects which handle the conversion of the "digits" Json values into a binary
 * data format. The processing of the digits branch of the data is broken up into two different
 * processors, \a DigitsProcessor and \a DigitProcessor, to improve modularity, readability (and thus
 * maintainability) of the code.
 */
#ifndef DIGITSPROCESSOR_H
#define DIGITSPROCESSOR_H
#include "json/json.h"
#include "MausEventStruct.h"

/*!
 * \class DigitsProcessor
 *
 * \brief Top level processor for the digits branch
 *
 * This processor deals with the top level of the digits branch and makes
 * sure that the \a DigitProcessor has the correct single Digit object to 
 * work with.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class DigitsProcessor {
 public:
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a MausData object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param MausData* a pointer to a \a MausData object
   */
  DigitsProcessor(MausData*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a MausData object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param MausData& a \a MausData object
   */
  DigitsProcessor(MausData&);
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a Json::Value object which it will extend with the 
   * correct number of elements from the MausData.
   *
   * \param Json::Value* a pointer to a \a Json::Value object
   */
  DigitsProcessor(Json::Value*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a Json::Value object which it will extend with the 
   * correct number of elements from the MausData.
   *
   * \param Json::Value& a \a Json::Value object
   */
  DigitsProcessor(Json::Value&);
  /*!
   * \brief Convert the given \a Json::Value
   *
   * Starts converting the \a Json::Value given.
   *
   * \param Json::Value& The Json representation of the Digits cpp data structure
   * \return a pointer to the converted Digits object stored within a MausData object
   */
  MausData* operator()(const Json::Value&);
  /*!
   * \brief Convert the given \a DigitsElement
   *
   * Starts converting the \a DigitsElement given.
   *
   * \param DigitsElement* a pointer to the DigitsElement object to convert to Json::Value
   * \return a pointer to the converted Json::Value object
   */
  Json::Value* operator()(const DigitsElement*);

 private:
  /*! \var MausData* m_md
   * \brief pointer to the output \a MausData object
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  MausData* m_md;
  /*! \var Json::Value* m_jv
   * \brief pointer to the output \a Json::Value object
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  Json::Value* m_jv;
};
////////////////////////////////////////////////////////////////////////////////////////////////


/*!
 * \class DigitProcessor
 *
 * \brief Processor for the individual Digits
 *
 * This processor handles the individual Digits and ensures that the values as they are
 * represented in the Json document are correctly translated into the binary structure.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class DigitProcessor {
 public:
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a DigitsElement object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param DigitsElement* a pointer to a \a DigitsElement object
   */
  DigitProcessor(DigitsElement*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a DigitsElement object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param DigitsElement& a \a DigitsElement object
   */
  DigitProcessor(DigitsElement&);
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a Json::Value object which it will extend with the 
   * correct number of elements from the DigitsElement.
   *
   * \param Json::Value* a pointer to a \a Json::Value object
   */
  DigitProcessor(Json::Value*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a Json::Value object which it will extend with the 
   * correct number of elements from the DigitsElement.
   *
   * \param Json::Value& a \a Json::Value object
   */
  DigitProcessor(Json::Value&);
  /*!
   * \brief Convert the given \a Json::Value
   *
   * Starts converting the \a Json::Value given.
   *
   * \param Json::Value& The individual Digit Json::Values
   * \return a pointer to the DigitsElement
   */
  DigitsElement* operator()(const Json::Value&);
  /*!
   * \brief Convert the given \a Digit
   *
   * Starts converting the \a Digit given.
   *
   * \param Digit* A pointer to the individual Digit being converted
   * \return a pointer to the Json::Value representation of the Digit
   */
  Json::Value* operator()(const Digit*);
 private:
  /*! \var DigitsElement* m_de
   * \brief pointer to a \a DigitsElement object that will be extended as part of building the output
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  DigitsElement* m_de;
  /*! \var Json::Value* m_jv
   * \brief pointer to a \a Json::Value object that will be extended as part of building the output
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  Json::Value* m_jv;
};
////////////////////////////////////////////////////////////////////////////////////////////////

#endif
