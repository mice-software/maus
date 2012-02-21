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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPPRIMITIVESPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPPRIMITIVESPROCESSOR_HH_

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"

namespace MAUS {

class DoubleProcessor : public ProcessorBase<double> {
  public:
    virtual double* JsonToCpp(const Json::Value& json_double);
    virtual Json::Value* CppToJson(const double& cpp_double);
};

class StringProcessor : public ProcessorBase<std::string> {
  public:
    virtual std::string* JsonToCpp(const Json::Value& json_string);
    virtual Json::Value* CppToJson(const std::string& cpp_string);
};

class IntProcessor : public ProcessorBase<int> {
  public:
    virtual int* JsonToCpp(const Json::Value& json_int);
    virtual Json::Value* CppToJson(const int& cpp_int);
};

class UIntProcessor : public ProcessorBase<unsigned int> {
  public:
    virtual unsigned int* JsonToCpp(const Json::Value& json_uint);
    virtual Json::Value* CppToJson(const unsigned int& cpp_double);
};

class BoolProcessor : public ProcessorBase<bool> {
  public:
    virtual bool* JsonToCpp(const Json::Value& json_bool);
    virtual Json::Value* CppToJson(const bool& cpp_bool);
};





}

#endif

