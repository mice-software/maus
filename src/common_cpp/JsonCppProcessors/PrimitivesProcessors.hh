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

#include <string>

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"

namespace MAUS {

/** Processor to convert between C++ double and Json::numericValue
 */
class DoubleProcessor : public ProcessorBase<double> {
  public:
    /** Convert from Json::numericValue to double
     *
     *  If json_double cannot be converted to a double, throw a Exception
     */
    virtual double* JsonToCpp(const Json::Value& json_double);

    /** Convert from double to Json::numericValue
     */
    virtual Json::Value* CppToJson(const double& cpp_double);

    /** Convert from double to Json::numericValue passing path
     */
    virtual Json::Value* CppToJson(const double& cpp_double, std::string path);
};

/** Processor to convert between C++ std::string and Json::stringValue
 */
class StringProcessor : public ProcessorBase<std::string> {
  public:
    /** Convert from Json::numericValue to double
     *
     *  If json_string cannot be converted to a double, throw a Exception
     */
    virtual std::string* JsonToCpp(const Json::Value& json_string);

    /** Convert from std::string to Json::stringValue
     */
    virtual Json::Value* CppToJson(const std::string& cpp_string);


    /** Convert from std::string to Json::stringValue passing path information
     */
    virtual Json::Value* CppToJson
                              (const std::string& cpp_string, std::string path);
};

/** Processor to convert between C++ int and Json::intValue
 */
class IntProcessor : public ProcessorBase<int> {
  public:
    /** Convert from Json::intValue to int
     *
     *  If json_string cannot be converted to a int, throw a Exception
     */
    virtual int* JsonToCpp(const Json::Value& json_int);

    /** Convert from int to Json::intValue
     */
    virtual Json::Value* CppToJson(const int& cpp_int);

    /** Convert from int to Json::intValue passing path information
     */
    virtual Json::Value* CppToJson(const int& cpp_int, std::string path);
};

/** Processor to convert between C++ uint and Json::intValue or Json::uintValue
 */
class UIntProcessor : public ProcessorBase<unsigned int> {
  public:
    /** Convert from Json::intValue or Json::uintValue to unsigned int
     *
     *  If we have a negative Json::intValue or another type, throw an exception
     */
    virtual unsigned int* JsonToCpp(const Json::Value& json_uint);

    /** Convert from unsigned int to Json::uintValue
     */
    virtual Json::Value* CppToJson(const unsigned int& cpp_double);

    /** Convert from unsigned int to Json::uintValue passing path information
     */
    virtual Json::Value* CppToJson
                             (const unsigned int& cpp_double, std::string path);
};

/** Processor to convert between C++ nool and Json::boolValue
 */
class BoolProcessor : public ProcessorBase<bool> {
  public:
    /** Convert from Json::boolValue to bool
     *
     *  If json_bool cannot be converted to a bool, throw a Exception
     */
    virtual bool* JsonToCpp(const Json::Value& json_bool);


    /** Convert from bool to Json::boolValue passing
     */
    virtual Json::Value* CppToJson(const bool& cpp_bool);


    /** Convert from bool to Json::boolValue passing path information
     */
    virtual Json::Value* CppToJson(const bool& cpp_bool, std::string path);
};
}

#endif

