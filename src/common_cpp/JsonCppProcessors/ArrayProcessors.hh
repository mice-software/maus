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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_ARRAYPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_ARRAYPROCESSOR_HH_

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"

namespace MAUS {

template <class ArrayContents>
class PointerArrayProcessor : public ProcessorBase<std::vector<ArrayContents*> > {
  public:
    PointerArrayProcessor(ProcessorBase<ArrayContents>* contents_processor);
    ~PointerArrayProcessor();
    std::vector<ArrayContents*>* JsonToCpp(const Json::Value& json_array);
    Json::Value* CppToJson(const std::vector<ArrayContents*>& cpp_array);
  private:
    ProcessorBase<ArrayContents>* _proc;
};

template <class ArrayContents>
class ValueArrayProcessor : public ProcessorBase<std::vector<ArrayContents> > {
  public:
    ValueArrayProcessor(ProcessorBase<ArrayContents>* ArrayContentsProcessor);
    ~ValueArrayProcessor();
    std::vector<ArrayContents>* JsonToCpp(const Json::Value& json_array);
    Json::Value* CppToJson(const std::vector<ArrayContents>& cpp_array);
  private:
    ProcessorBase<ArrayContents>* _proc;
};

}

#include "src/common_cpp/JsonCppProcessors/ArrayProcessors-inl.hh"

#endif

