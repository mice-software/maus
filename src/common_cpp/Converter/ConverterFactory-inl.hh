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
#include "src/common_cpp/Converter/DataConverters/PrimitiveConverters.hh"

namespace Json {
  class Value;
}

namespace MAUS {

  class Spill;



  // Template implementation
  // /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  template <typename INPUT, typename OUTPUT>
  OUTPUT* ConverterFactory::convert(INPUT* i) const {
    // not necessary if each converter does this check
    // but only if inheriting from converter base, not necessary as long as
    // implement the IConverter interface so check here anyway
    IConverter<INPUT, OUTPUT>* c = getConverter<INPUT, OUTPUT>();
    OUTPUT* o = NULL;
    try {
        o = c->convert(i);
    } catch (...) {
        delete c;
        throw;
    }
    delete c;
    return o;
  }

  /// STRING
  template <>
  inline IConverter<std::string, Json::Value>*
  ConverterFactory::getConverter<std::string, Json::Value>() const {
    return new StringJsonConverter();
  }

  template <>
  inline IConverter<std::string, Data>*
  ConverterFactory::getConverter<std::string, Data>() const {
    return new StringDataConverter();
  }

  template <>
  inline IConverter<std::string, PyObject>*
  ConverterFactory::getConverter<std::string, PyObject>() const {
    return new StringPyDictConverter();
  }

  template <>
  inline IConverter<std::string, std::string>*
  ConverterFactory::getConverter<std::string, std::string>() const {
    return new StringStringConverter();
  }

  /// JSON
  template <>
  inline IConverter<Json::Value, Json::Value>*
  ConverterFactory::getConverter<Json::Value, Json::Value>() const {
    return new JsonJsonConverter();
  }

  template <>
  inline IConverter<Json::Value, Data>*
  ConverterFactory::getConverter<Json::Value, Data>() const {
    return new JsonCppSpillConverter();
  }

  template <>
  inline IConverter<Json::Value, PyObject>*
  ConverterFactory::getConverter<Json::Value, PyObject>() const {
    return new JsonPyDictConverter();
  }

  template <>
  inline IConverter<Json::Value, std::string>*
  ConverterFactory::getConverter<Json::Value, std::string>() const {
    return new JsonStringConverter();
  }

  /// Data
  template <>
  inline IConverter<Data, Json::Value>*
  ConverterFactory::getConverter<Data, Json::Value>() const {
    return new CppJsonSpillConverter();
  }

  template <>
  inline IConverter<Data, Data>*
  ConverterFactory::getConverter<Data, Data>() const {
    return new DataDataConverter();
  }

  template <>
  inline IConverter<Data, PyObject>*
  ConverterFactory::getConverter<Data, PyObject>() const
    throw(ConverterNotFoundException) {
    return new DataPyDictConverter();
  }

  template <>
  inline IConverter<Data, std::string>*
  ConverterFactory::getConverter<Data, std::string>() const {
    return new DataStringConverter();
  }

  ///PyDICT to X
  template <>
  inline IConverter<PyObject, Json::Value>*
  ConverterFactory::getConverter<PyObject, Json::Value>() const {
    return new PyDictJsonConverter();
  }

  template <>
  inline IConverter<PyObject, Data>*
  ConverterFactory::getConverter<PyObject, Data>() const {
    return new PyDictDataConverter();
  }

  template <>
  inline IConverter<PyObject, PyObject>*
  ConverterFactory::getConverter<PyObject, PyObject>() const {
    return new PyDictPyDictConverter();
  }

  template <>
  inline IConverter<PyObject, std::string>*
  ConverterFactory::getConverter<PyObject, std::string>() const {
    return new PyDictStringConverter();
  }
} // end of namespace

#endif
