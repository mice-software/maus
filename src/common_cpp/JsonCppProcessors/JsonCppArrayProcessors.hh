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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPARRAYPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPARRAYPROCESSORBASE_HH_

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/JsonCppProcessorBase.hh"

namespace MAUS {

template <class ArrayContents>
class JsonCppPointerArrayProcessor : public JsonCppProcessorBase<std::vector<ArrayContents*> > {
  public:
    JsonCppPointerArrayProcessor(JsonCppProcessorBase<ArrayContents>* contents_processor);
    ~JsonCppPointerArrayProcessor();
    std::vector<ArrayContents*>* JsonToCpp(const Json::Value& json_array);
    Json::Value* CppToJson(const std::vector<ArrayContents*>& cpp_array);
  private:
    JsonCppProcessorBase<ArrayContents>* _proc;
};

template <class ArrayContents>
class JsonCppValueArrayProcessor : public JsonCppProcessorBase<std::vector<ArrayContents> > {
  public:
    JsonCppValueArrayProcessor(JsonCppProcessorBase<ArrayContents>* ArrayContentsProcessor);
    ~JsonCppValueArrayProcessor();
    std::vector<ArrayContents>* JsonToCpp(const Json::Value& json_array);
    Json::Value* CppToJson(const std::vector<ArrayContents>& cpp_array);
  private:
    JsonCppProcessorBase<ArrayContents>* _proc;
};

}

#include "src/common_cpp/JsonCppProcessors/JsonCppArrayProcessors-inl.hh"

#endif

