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

#ifndef SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSJF_H
#define SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSJF_H
#include <string>


#include "src/common_cpp/Converter/DataConverters/JsonCppJobFooterConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonJobFooterConverter.hh"

namespace MAUS {
class JobFooterData;

/////////////////////////// X TO JobFooterData ////////////////////////////


class StringJobFooterConverter : public ConverterBase<std::string, JobFooterData> {
  public:
    StringJobFooterConverter()
      : ConverterBase<std::string, JobFooterData>("StringDataConverter") {}

  private:
    JobFooterData* _convert(const std::string* str) const {
        Json::Value* json = StringJsonConverter().convert(str);
        JobFooterData* jh = JsonCppJobFooterConverter().convert(json);
        delete json;
        return jh;
    }
};

class PyDictJobFooterConverter : public ConverterBase<PyObject, JobFooterData> {
  public:
    PyDictJobFooterConverter()
      : ConverterBase<PyObject, JobFooterData>("PyDictJobFooterConverter") {}

  private:
    JobFooterData* _convert(const PyObject* obj) const {
        std::string* str = PyDictStringConverter().convert(obj);
        JobFooterData* jh = StringJobFooterConverter().convert(str);
        delete str;
        return jh;
    }
};

/////////////////////////// JobFooterData TO X ////////////////////////////

class JobFooterStringConverter : public ConverterBase<JobFooterData, std::string> {
  public:
    JobFooterStringConverter()
      : ConverterBase<JobFooterData, std::string>("JobFooterStringConverter") {}

  private:
    std::string* _convert(const JobFooterData* jh) const {
        Json::Value* json = CppJsonJobFooterConverter().convert(jh);
        std::string* str = JsonStringConverter().convert(json);
        delete json;
        return str;
    }
};

class JobFooterJobFooterConverter : public ConverterBase<JobFooterData, JobFooterData> {
  public:
    JobFooterJobFooterConverter()
      : ConverterBase<JobFooterData, JobFooterData>("JobFooterJobFooterConverter") {}

  private:
    JobFooterData* _convert(const JobFooterData* jh) const {
        return new JobFooterData(*jh);
    }
};

class JobFooterPyDictConverter : public ConverterBase<JobFooterData, PyObject> {
  public:
    JobFooterPyDictConverter()
      : ConverterBase<JobFooterData, PyObject>("JobFooterPyDictConverter") {}

  private:
    PyObject* _convert(const JobFooterData* jh) const {
        std::string* str = JobFooterStringConverter().convert(jh);
        PyObject* obj = StringPyDictConverter().convert(str);
        delete str;
        return obj;
    }
};
}
#endif //  SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSJF_H


