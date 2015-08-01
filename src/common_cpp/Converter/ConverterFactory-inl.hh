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
#include <string>

#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Converter/DataConverters/PrimitiveConverters.hh"
#include "src/common_cpp/Converter/DataConverters/PrimitiveConvertersData.hh"
#include "src/common_cpp/Converter/DataConverters/PrimitiveConvertersJobHeader.hh"
#include "src/common_cpp/Converter/DataConverters/PrimitiveConvertersJobFooter.hh"
#include "src/common_cpp/Converter/DataConverters/PrimitiveConvertersRunHeader.hh"
#include "src/common_cpp/Converter/DataConverters/PrimitiveConvertersRunFooter.hh"
#include "src/common_cpp/Converter/DataConverters/PrimitiveConvertersImage.hh"

namespace Json {
  class Value;
}

namespace MAUS {
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
  inline IConverter<std::string, JobHeaderData>*
  ConverterFactory::getConverter<std::string, JobHeaderData>() const {
    return new StringJobHeaderConverter();
  }

  template <>
  inline IConverter<std::string, JobFooterData>*
  ConverterFactory::getConverter<std::string, JobFooterData>() const {
    return new StringJobFooterConverter();
  }

  template <>
  inline IConverter<std::string, RunHeaderData>*
  ConverterFactory::getConverter<std::string, RunHeaderData>() const {
    return new StringRunHeaderConverter();
  }

  template <>
  inline IConverter<std::string, RunFooterData>*
  ConverterFactory::getConverter<std::string, RunFooterData>() const {
    return new StringRunFooterConverter();
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
  inline IConverter<Json::Value, JobHeaderData>*
  ConverterFactory::getConverter<Json::Value, JobHeaderData>() const {
    return new JsonCppJobHeaderConverter();
  }

  template <>
  inline IConverter<Json::Value, JobFooterData>*
  ConverterFactory::getConverter<Json::Value, JobFooterData>() const {
    return new JsonCppJobFooterConverter();
  }

  template <>
  inline IConverter<Json::Value, RunHeaderData>*
  ConverterFactory::getConverter<Json::Value, RunHeaderData>() const {
    return new JsonCppRunHeaderConverter();
  }

  template <>
  inline IConverter<Json::Value, RunFooterData>*
  ConverterFactory::getConverter<Json::Value, RunFooterData>() const {
    return new JsonCppRunFooterConverter();
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
  ConverterFactory::getConverter<Data, PyObject>() const {
    return new DataPyDictConverter();
  }

  template <>
  inline IConverter<Data, std::string>*
  ConverterFactory::getConverter<Data, std::string>() const {
    return new DataStringConverter();
  }

  /// JobHeaderData
  template <>
  inline IConverter<JobHeaderData, Json::Value>*
  ConverterFactory::getConverter<JobHeaderData, Json::Value>() const {
    return new CppJsonJobHeaderConverter();
  }

  template <>
  inline IConverter<JobHeaderData, JobHeaderData>*
  ConverterFactory::getConverter<JobHeaderData, JobHeaderData>() const {
    return new JobHeaderJobHeaderConverter();
  }

  template <>
  inline IConverter<JobHeaderData, PyObject>*
  ConverterFactory::getConverter<JobHeaderData, PyObject>() const {
    return new JobHeaderPyDictConverter();
  }

  template <>
  inline IConverter<JobHeaderData, std::string>*
  ConverterFactory::getConverter<JobHeaderData, std::string>() const {
    return new JobHeaderStringConverter();
  }

  /// JobFooterData
  template <>
  inline IConverter<JobFooterData, Json::Value>*
  ConverterFactory::getConverter<JobFooterData, Json::Value>() const {
    return new CppJsonJobFooterConverter();
  }

  template <>
  inline IConverter<JobFooterData, JobFooterData>*
  ConverterFactory::getConverter<JobFooterData, JobFooterData>() const {
    return new JobFooterJobFooterConverter();
  }

  template <>
  inline IConverter<JobFooterData, PyObject>*
  ConverterFactory::getConverter<JobFooterData, PyObject>() const {
    return new JobFooterPyDictConverter();
  }

  template <>
  inline IConverter<JobFooterData, std::string>*
  ConverterFactory::getConverter<JobFooterData, std::string>() const {
    return new JobFooterStringConverter();
  }

  /// RunHeaderData
  template <>
  inline IConverter<RunHeaderData, Json::Value>*
  ConverterFactory::getConverter<RunHeaderData, Json::Value>() const {
    return new CppJsonRunHeaderConverter();
  }

  template <>
  inline IConverter<RunHeaderData, RunHeaderData>*
  ConverterFactory::getConverter<RunHeaderData, RunHeaderData>() const {
    return new RunHeaderRunHeaderConverter();
  }

  template <>
  inline IConverter<RunHeaderData, PyObject>*
  ConverterFactory::getConverter<RunHeaderData, PyObject>() const {
    return new RunHeaderPyDictConverter();
  }

  template <>
  inline IConverter<RunHeaderData, std::string>*
  ConverterFactory::getConverter<RunHeaderData, std::string>() const {
    return new RunHeaderStringConverter();
  }

  /// RunFooterData
  template <>
  inline IConverter<RunFooterData, Json::Value>*
  ConverterFactory::getConverter<RunFooterData, Json::Value>() const {
    return new CppJsonRunFooterConverter();
  }

  template <>
  inline IConverter<RunFooterData, RunFooterData>*
  ConverterFactory::getConverter<RunFooterData, RunFooterData>() const {
    return new RunFooterRunFooterConverter();
  }

  template <>
  inline IConverter<RunFooterData, PyObject>*
  ConverterFactory::getConverter<RunFooterData, PyObject>() const {
    return new RunFooterPyDictConverter();
  }

  template <>
  inline IConverter<RunFooterData, std::string>*
  ConverterFactory::getConverter<RunFooterData, std::string>() const {
    return new RunFooterStringConverter();
  }

  /// ImageData
  template <>
  inline IConverter<ImageData, ImageData>*
  ConverterFactory::getConverter<ImageData, ImageData>() const {
    return new ImageImageConverter();
  }

  /// PyDICT to X
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
  inline IConverter<PyObject, JobHeaderData>*
  ConverterFactory::getConverter<PyObject, JobHeaderData>() const {
    return new PyDictJobHeaderConverter();
  }

  template <>
  inline IConverter<PyObject, JobFooterData>*
  ConverterFactory::getConverter<PyObject, JobFooterData>() const {
    return new PyDictJobFooterConverter();
  }

  template <>
  inline IConverter<PyObject, RunHeaderData>*
  ConverterFactory::getConverter<PyObject, RunHeaderData>() const {
    return new PyDictRunHeaderConverter();
  }

  template <>
  inline IConverter<PyObject, RunFooterData>*
  ConverterFactory::getConverter<PyObject, RunFooterData>() const {
    return new PyDictRunFooterConverter();
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

  // DEFAULT - just throw an exception
  template <typename INPUT, typename OUTPUT>
  IConverter<INPUT, OUTPUT>* ConverterFactory::getConverter() const {
    return new DisallowedConverter<INPUT, OUTPUT>();
  }
} // end of namespace

#endif
