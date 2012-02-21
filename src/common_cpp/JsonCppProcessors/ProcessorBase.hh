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

template <class CppRepresentation>
class IProcessor {
  public:
    virtual ~IProcessor() {}
    virtual CppRepresentation* JsonToCpp(const Json::Value& json_representation) = 0;
    virtual Json::Value* CppToJson(const CppRepresentation& cpp_representation) = 0;
};

template <class CppRepresentation>
class ProcessorBase : IProcessor<CppRepresentation> {
  public:
    virtual CppRepresentation* JsonToCpp(const Json::Value& json_representation) = 0;
    virtual Json::Value* CppToJson(const CppRepresentation& cpp_representation) = 0;

    virtual void SetIsStrict(bool isStrict) {
        _isStrict = isStrict;
    }
    virtual bool GetIsStrict() {
        return _isStrict;
    }
  protected:
    bool _isStrict;
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

