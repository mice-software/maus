/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_PROCESSORBASE_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_PROCESSORBASE_HH_

#include "json/json.h"

#include "src/legacy/Interface/Squeal.hh"

namespace MAUS {

/** @class IProcessor
 *  
 *  IProcessor is the processor interface class for processing json data to cpp
 *  data. Json data is represented by Json::Value while cpp data is represented
 *  by CppRepresentation type.
 */
template <class CppRepresentation>
class IProcessor {
  public:
    virtual ~IProcessor() {}

    /** Convert from the Json representation to the cpp representation
     *
     *  Memory for CppRepresentation should be allocated by this function. If
     *  the conversion fails, throw a Squeal - don't return NULL or allocate
     *  memory.
     */
    virtual CppRepresentation* JsonToCpp
                                   (const Json::Value& json_representation) = 0;

    /** Convert from the Cpp representation to the json representation
     *
     *  Memory for the json representation should be allocated by this function.
     *  If the conversion fails, throw a Squeal - don't return NULL or allocate
     *  memory.
     */
    virtual Json::Value* CppToJson
                              (const CppRepresentation& cpp_representation) = 0;
};

/** @class ProcessorBase
 *  
 *  ProcessorBase is the processor base class for processing json data to cpp
 *  data. Json data is represented by Json::Value while cpp data is represented
 *  by CppRepresentation type.
 *
 *  This class is provided because at some point I want to provide some more
 *  sophisticated error handling.
 *
 */
template <class CppRepresentation>
class ProcessorBase : IProcessor<CppRepresentation> {
  public:
    virtual CppRepresentation* JsonToCpp
                                   (const Json::Value& json_representation) = 0;
    virtual Json::Value* CppToJson
                              (const CppRepresentation& cpp_representation) = 0;

  protected:
};


}

#endif

/*
      nullValue = 0, ///< 'null' value
      intValue,      ///< signed integer value
      uintValue,     ///< unsigned integer value
      realValue,     ///< double value
      stringValue,   ///< UTF-8 string value
      booleanValue,  ///< bool value
      arrayValue,    ///< array value (ordered list)
      objectValue    ///< object value (collection of name/value pairs).
*/

