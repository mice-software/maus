/*!
 * \file MCProcessor.h
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 *
 * This file contains processor objects which handle the conversion of the "mc" Json values into a binary
 * data format. The processing of the mc branch of the data is broken up into three different
 * processors, \a MCProcessor, \a MCHitProcessor and \a MCPrimaryProcessor, to improve modularity, readability
 * (and thus maintainability) of the code.
 */
#ifndef MCPROCESSOR_H
#define MCPROCESSOR_H
#include "json/json.h"
#include "src/common_cpp/DataStructure/MausEventStruct.hh"

/*!
 * \class MCProcessor
 *
 * \brief Top level processor for the mc branch
 *
 * This processor deals with the top level of the mc branch and makes
 * sure that \a MCHitProcessor and \a MCPrimaryProcessor have the correct
 * single Hit and Primary objects to work with.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class MCProcessor{
 public:
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a MausData object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param MausData* a pointer to a \a MausData object
   */
  MCProcessor(MausData*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a MausData object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param MausData& a \a MausData object
   */
  MCProcessor(MausData&);
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a Json::Value object which it will extend with the 
   * correct number of elements from the MCElement given in operator().
   *
   * \param Json::Value* a pointer to a \a Json::Value object
   */
  MCProcessor(Json::Value*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a Json::Value object which it will extend with the 
   * correct number of elements from the MCElement given in operator().
   *
   * \param Json::Value& a \a Json::Value object
   */
  MCProcessor(Json::Value&);
  /*!
   * \brief Convert the given \a Json::Value
   *
   * Starts converting the \a Json::Value given.
   *
   * \param Json::Value& The top level "mc" Json::Value
   * \return a pointer to a MausData object containing the mc Json::Value information
   */
  MausData* operator()(const Json::Value&);
  /*!
   * \brief Convert the given \a MCElement
   *
   * Starts converting the \a MCElement given.
   *
   * \param MCElement* pointer to a MCElement object to be converted into Json::Value object
   * \return a pointer to a Json::Value object containing the MCElement information
   */
  Json::Value* operator()(const MCElement*);
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
 * \class MCHitProcessor
 *
 * \brief Processor for the individual mc hits
 *
 * This processor handles the individual hits and ensures that the values as they are
 * represented in the Json document are correctly translated into the binary structure.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class MCHitProcessor{
 public:
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a MCHits object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param MCHits* a pointer to a \a MCHits object
   */
  MCHitProcessor(MCHits*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a MCHits object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param MCHits& a \a MCHits object
   */
  MCHitProcessor(MCHits&);
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a Json::Value object which it will extend with the 
   * correct number of elements from the MCHitsElement given in operator().
   *
   * \param Json::Value* a pointer to a \a Json::Value object
   */
  MCHitProcessor(Json::Value*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a Json::Value object which it will extend with the 
   * correct number of elements from the MCHitsElement given in operator().
   *
   * \param Json::Value& a \a Json::Value object
   */
  MCHitProcessor(Json::Value&);
  /*!
   * \brief Convert the given \a Json::Value
   *
   * Starts converting the \a Json::Value given.
   *
   * \param Json::Value& The individual mc hit Json::Values
   * \return a pointer to the filled MCHits object
   */
  MCHits* operator()(const Json::Value&);
  /*!
   * \brief Convert the given \a MCHitsElement
   *
   * Starts converting the \a MCHitsElement given.
   *
   * \param MCHitsElement* pointer to a MCHitsElement object to be converted into Json::Value object
   * \return a pointer to a Json::Value object containing the MCHitsElement information
   */
  Json::Value* operator()(const MCHitsElement*);

private:
  /*! \var MCHits* m_mch
   * \brief pointer to a \a MCHits object that will be extended as part of building the output
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  MCHits* m_mch;
  /*! \var Json::Value* m_jv
   * \brief pointer to the output \a Json::Value object
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  Json::Value* m_jv;
};
////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \class MCPrimaryProcessor
 *
 * \brief Processor for the individual mc primaries
 *
 * This processor handles the individual mc primaries and ensures that the values as they are
 * represented in the Json document are correctly translated into the binary structure.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class MCPrimaryProcessor {
 public:
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a MCPrimary object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param MCPrimary* a pointer to a \a MCPrimary object
   */
  MCPrimaryProcessor(MCPrimary*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a MCPrimary object which it will extend with the 
   * correct number of elements from the Json data.
   *
   * \param MCPrimary& a \a MCPrimary object
   */
  MCPrimaryProcessor(MCPrimary&);
  /*!
   * \brief Constructor
   *
   * Constructor takes a pointer to a \a Json::Value object which it will extend with the 
   * correct number of elements from the MCPrimary given in operator().
   *
   * \param Json::Value* a pointer to a \a Json::Value object
   */
  MCPrimaryProcessor(Json::Value*);
  /*!
   * \brief Constructor
   *
   * Constructor takes a \a Json::Value object which it will extend with the 
   * correct number of elements from the MCPrimary given in operator().
   *
   * \param Json::Value& a \a Json::Value object
   */
  MCPrimaryProcessor(Json::Value&);
  /*!
   * \brief Convert the given \a Json::Value
   *
   * Starts converting the \a Json::Value given.
   *
   * \param Json::Value& The individual mc primary Json::Values
   * \return a pointer to the cpp MCPrimary representation of the Json::Value information
   */
  MCPrimary* operator()(const Json::Value&);
  /*!
   * \brief Convert the given \a MCPrimary
   *
   * Starts converting the \a MCPrimary given.
   *
   * \param MCPrimary* pointer to the MCPrimary object to convert
   * \return a pointer to a Json::Value object containing the MCPrimary information
   */
  Json::Value* operator()(const MCPrimary*);
 private:
  /*! \var MCPrimary* m_mcp
   * \brief pointer to a \a MCPrimary object that will be extended as part of building the output
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  MCPrimary* m_mcp;
  /*! \var Json::Value* m_jv
   * \brief pointer to the output \a Json::Value object
   * \note the memory from this variable is assumed under the control of the user
   * and therefore they assume responsibility for it's cleanup
   */
  Json::Value* m_jv;
};
////////////////////////////////////////////////////////////////////////////////////////////////

#endif
