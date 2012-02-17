#ifndef JSONCPPCONVERTER_H
#define JSONCPPCONVERTER_H
#include "json/json.h"

#include "src/common_cpp/JsonCppStreamer/ConverterBase.hh"
#include "src/common_cpp/DataStructure/Spill.hh"

namespace MAUS {

/*!
 * \class JsonCppConverter
 *
 * \brief Converts JSON documents into corresponding binary Cpp fromat
 *
 * JsonCppConverter is build on \a ConverterBase as a specialisation to
 * handle the conversion of data from \a Json::Value to binary \a MausData
 * format. Since Json documents often come in the form of a string, these
 * too are accepted and parsed on the fly.
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class JsonCppConverter : public ConverterBase<Json::Value, Spill>{
public:
  JsonCppConverter() : ConverterBase<Json::Value, Spill>(static_cast<Spill*>(NULL)) {}

  /*!
   * \brief Convert Json value
   * Overloaded process initiate the conversion process converting the 
   * \a Json::Value given as the argument into the output type \a MausData
   *
   * \param Json::Value& The root \a Json::Value object from the Json data file 
   * \return a pointer to the MausData object
   */
  Spill* operator()(const Json::Value&);

  /*!
   * \brief Convert MausData
   * Overloaded process initiates the conversion process converting the 
   * \a MausData given as the argument into the output type \a Json::Value
   *
   * \param MausData& The root \a MausData object from the cpp data structure
   * \return a pointer to the Json::Value object 
   */
  Json::Value* operator()(const Spill&);

private:
};

}

#endif
