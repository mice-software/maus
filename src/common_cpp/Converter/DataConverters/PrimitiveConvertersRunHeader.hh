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

#ifndef SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSRH_H
#define SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSRH_H
#include <string>

#include "src/common_cpp/Converter/DataConverters/JsonCppRunHeaderConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonRunHeaderConverter.hh"

namespace MAUS {
class RunHeaderData;

/////////////////////////// X TO RunHeaderData ////////////////////////////

class StringRunHeaderConverter : public ConverterBase<std::string, RunHeaderData> {
  public:
    StringRunHeaderConverter()
      : ConverterBase<std::string, RunHeaderData>("StringRunHeaderConverter") {}

  private:
    RunHeaderData* _convert(const std::string* str) const {
        Json::Value* json = StringJsonConverter().convert(str);
        RunHeaderData* jh = JsonCppRunHeaderConverter().convert(json);
        delete json;
        return jh;
    }
};

class PyDictRunHeaderConverter : public ConverterBase<PyObject, RunHeaderData> {
  public:
    PyDictRunHeaderConverter()
      : ConverterBase<PyObject, RunHeaderData>("PyDictRunHeaderConverter") {}

  private:
    RunHeaderData* _convert(const PyObject* obj) const {
        std::string* str = PyDictStringConverter().convert(obj);
        RunHeaderData* jh = StringRunHeaderConverter().convert(str);
        delete str;
        return jh;
    }
};


/////////////////////////// RunHeaderData TO X ////////////////////////////

class RunHeaderStringConverter : public ConverterBase<RunHeaderData, std::string> {
  public:
    RunHeaderStringConverter()
      : ConverterBase<RunHeaderData, std::string>("RunHeaderStringConverter") {}

  private:
    std::string* _convert(const RunHeaderData* jh) const {
        Json::Value* json = CppJsonRunHeaderConverter().convert(jh);
        std::string* str = JsonStringConverter().convert(json);
        delete json;
        return str;
    }
};

class RunHeaderRunHeaderConverter : public ConverterBase<RunHeaderData, RunHeaderData> {
  public:
    RunHeaderRunHeaderConverter()
      : ConverterBase<RunHeaderData, RunHeaderData>("RunHeaderRunHeaderConverter") {}

  private:
    RunHeaderData* _convert(const RunHeaderData* jh) const {
        return new RunHeaderData(*jh);
    }
};

class RunHeaderPyDictConverter : public ConverterBase<RunHeaderData, PyObject> {
  public:
    RunHeaderPyDictConverter()
      : ConverterBase<RunHeaderData, PyObject>("RunHeaderPyDictConverter") {}

  private:
    PyObject* _convert(const RunHeaderData* jh) const {
        std::string* str = RunHeaderStringConverter().convert(jh);
        PyObject* obj = StringPyDictConverter().convert(str);
        delete str;
        return obj;
    }
};
}
#endif //  SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSRH_H


