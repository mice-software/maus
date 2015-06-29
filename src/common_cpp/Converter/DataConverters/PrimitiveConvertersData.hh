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

#ifndef SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSDATA_H
#define SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSDATA_H
#include <string>

#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"

namespace MAUS {
class Data;

/////////////////////////// X TO DATA ////////////////////////////


class StringDataConverter : public ConverterBase<std::string, MAUS::Data> {
  public:
    StringDataConverter()
      : ConverterBase<std::string, MAUS::Data>("StringDataConverter") {}

  private:
    MAUS::Data* _convert(const std::string* str) const {
        Json::Value* json = StringJsonConverter().convert(str);
        MAUS::Data* data = JsonCppSpillConverter().convert(json);
        delete json;
        return data;
    }
};

class PyDictDataConverter : public ConverterBase<PyObject, Data> {
  public:
    PyDictDataConverter()
      : ConverterBase<PyObject, MAUS::Data>("PyDictDataConverter") {}

  private:
    MAUS::Data* _convert(const PyObject* obj) const {
        std::string* str = PyDictStringConverter().convert(obj);
        MAUS::Data* data = StringDataConverter().convert(str);
        delete str;
        return data;
    }
};


/////////////////////////// DATA TO X ////////////////////////////

class DataStringConverter : public ConverterBase<MAUS::Data, std::string> {
  public:
    DataStringConverter()
      : ConverterBase<MAUS::Data, std::string>("DataStringConverter") {}

  private:
    std::string* _convert(const MAUS::Data* data) const {
        Json::Value* json = CppJsonSpillConverter().convert(data);
        std::string* str = JsonStringConverter().convert(json);
        delete json;
        return str;
    }
};

class DataDataConverter : public ConverterBase<Data, Data> {
  public:
    DataDataConverter()
      : ConverterBase<MAUS::Data, MAUS::Data>("DataDataConverter") {}

  private:
    MAUS::Data* _convert(const MAUS::Data* data) const {
        return new Data(*data);
    }
};

class DataPyDictConverter : public ConverterBase<Data, PyObject> {
  public:
    DataPyDictConverter()
      : ConverterBase<Data, PyObject>("DataPyDictConverter") {}

  private:
    PyObject* _convert(const Data* data) const {
        std::string* str = DataStringConverter().convert(data);
        PyObject* obj = StringPyDictConverter().convert(str);
        delete str;
        return obj;
    }
};
}
#endif //  SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSDATA_H


