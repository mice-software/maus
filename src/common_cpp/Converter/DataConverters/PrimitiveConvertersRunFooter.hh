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

#ifndef SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSRF_H
#define SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSRF_H
#include <string>

#include "src/common_cpp/Converter/DataConverters/JsonCppRunFooterConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonRunFooterConverter.hh"

namespace MAUS {
class RunFooterData;

/////////////////////////// X TO RunFooterData ////////////////////////////


class StringRunFooterConverter : public ConverterBase<std::string, RunFooterData> {
  public:
    StringRunFooterConverter()
      : ConverterBase<std::string, RunFooterData>("StringDataConverter") {}

  private:
    RunFooterData* _convert(const std::string* str) const {
        Json::Value* json = StringJsonConverter().convert(str);
        RunFooterData* jh = JsonCppRunFooterConverter().convert(json);
        delete json;
        return jh;
    }
};

class PyDictRunFooterConverter : public ConverterBase<PyObject, RunFooterData> {
  public:
    PyDictRunFooterConverter()
      : ConverterBase<PyObject, RunFooterData>("PyDictRunFooterConverter") {}

  private:
    RunFooterData* _convert(const PyObject* obj) const {
        std::string* str = PyDictStringConverter().convert(obj);
        RunFooterData* jh = StringRunFooterConverter().convert(str);
        delete str;
        return jh;
    }
};


/////////////////////////// RunFooterData TO X ////////////////////////////

class RunFooterStringConverter : public ConverterBase<RunFooterData, std::string> {
  public:
    RunFooterStringConverter()
      : ConverterBase<RunFooterData, std::string>("RunFooterStringConverter") {}

  private:
    std::string* _convert(const RunFooterData* jh) const {
        Json::Value* json = CppJsonRunFooterConverter().convert(jh);
        std::string* str = JsonStringConverter().convert(json);
        delete json;
        return str;
    }
};

class RunFooterRunFooterConverter : public ConverterBase<RunFooterData, RunFooterData> {
  public:
    RunFooterRunFooterConverter()
      : ConverterBase<RunFooterData, RunFooterData>("RunFooterRunFooterConverter") {}

  private:
    RunFooterData* _convert(const RunFooterData* jh) const {
        return new RunFooterData(*jh);
    }
};

class RunFooterPyDictConverter : public ConverterBase<RunFooterData, PyObject> {
  public:
    RunFooterPyDictConverter()
      : ConverterBase<RunFooterData, PyObject>("RunFooterPyDictConverter") {}

  private:
    PyObject* _convert(const RunFooterData* jh) const {
        std::string* str = RunFooterStringConverter().convert(jh);
        PyObject* obj = StringPyDictConverter().convert(str);
        delete str;
        return obj;
    }
};
}
#endif //  SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSRF_H

