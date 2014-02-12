/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 *
 */
#ifndef _SRC_COMMON_CPP_CONVERTER_CONVERTERFACTORY_INL_
#define _SRC_COMMON_CPP_CONVERTER_CONVERTERFACTORY_INL_
#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"

namespace Json {
  class Value;
}

namespace MAUS {

  class Spill;



  // Template implementation
  // /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  template <typename INPUT, typename OUTPUT>
  OUTPUT* ConverterFactory::convert(INPUT* i) const throw(NullInputException, UnhandledException) {
    // not necessary if each converter does this check
    // but only if inheriting from converter base, not necessary as long as
    // implement the IConverter interface so check here anyway
    if (!i) { throw NullInputException("ConverterFactory"); }

    // Get converter
    IConverter<INPUT, OUTPUT>* c = 0;
    try {
      c = getConverter<INPUT, OUTPUT>();
    }
    catch (ConverterNotFoundException& e) {
      if (c) { delete c; }
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, "ConverterFactory");
      return 0;
    }

    // Convert input using converter.
    // Still have to catch all as user may have custom converter inheriting
    // from IConverter not ConverterBase
    OUTPUT* o = 0;
    try {
      o = c->convert(i);
    }
    catch (Exception& s) {
      CppErrorHandler::getInstance()->HandleExceptionNoJson(s, "ConverterFactory");
    }
    catch (std::exception& e) {
//       if(c){ delete c; }
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, "ConverterFactory");
    }
    catch (...) {
      if (c) { delete c; }
      throw UnhandledException("ConverterFactory");
    }

    delete c;
    return o;
  }

  template <typename INPUT, typename OUTPUT>
  IConverter<INPUT, OUTPUT>* ConverterFactory::getConverter() const
    throw(ConverterNotFoundException) {

    throw ConverterNotFoundException("ConverterFactory");
  }
  template <>
  IConverter<Json::Value, Data>* ConverterFactory::getConverter<Json::Value, Data>() const
    throw(ConverterNotFoundException) {

    return new JsonCppSpillConverter();
  }

  // DAMN cant do this as JsonCppSpillConverter only inherits from one type of Converter base
  // Must split it up
  template <>
  IConverter<Data, Json::Value>* ConverterFactory::getConverter<Data, Json::Value>() const
    throw(ConverterNotFoundException) {

    return new CppJsonSpillConverter();
  }
//   template <>
//   IConverter<int,double>* ConverterFactory::getConverter<double, int>() const
//     throw (ConverterNotFoundException){
//     return new myDoubleIntConverter();
//   }

} // end of namespace

#endif
