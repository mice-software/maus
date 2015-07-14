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

#ifndef SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSJH_H
#define SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSJH_H
#include <string>

#include "src/common_cpp/Converter/DataConverters/JsonCppJobHeaderConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonJobHeaderConverter.hh"

namespace MAUS {
class JobHeaderData;

/////////////////////////// X TO JobHeaderData ////////////////////////////


class StringJobHeaderConverter : public ConverterBase<std::string, JobHeaderData> {
  public:
    StringJobHeaderConverter()
      : ConverterBase<std::string, JobHeaderData>("StringDataConverter") {}

  private:
    JobHeaderData* _convert(const std::string* str) const {
        Json::Value* json = StringJsonConverter().convert(str);
        JobHeaderData* jh = JsonCppJobHeaderConverter().convert(json);
        delete json;
        return jh;
    }
};

class PyDictJobHeaderConverter : public ConverterBase<PyObject, JobHeaderData> {
  public:
    PyDictJobHeaderConverter()
      : ConverterBase<PyObject, JobHeaderData>("PyDictJobHeaderConverter") {}

  private:
    JobHeaderData* _convert(const PyObject* obj) const {
        std::string* str = PyDictStringConverter().convert(obj);
        JobHeaderData* jh = StringJobHeaderConverter().convert(str);
        delete str;
        return jh;
    }
};


/////////////////////////// JobHeaderData TO X ////////////////////////////

class JobHeaderStringConverter : public ConverterBase<JobHeaderData, std::string> {
  public:
    JobHeaderStringConverter()
      : ConverterBase<JobHeaderData, std::string>("JobHeaderStringConverter") {}

  private:
    std::string* _convert(const JobHeaderData* jh) const {
        Json::Value* json = CppJsonJobHeaderConverter().convert(jh);
        std::string* str = JsonStringConverter().convert(json);
        delete json;
        return str;
    }
};

class JobHeaderJobHeaderConverter : public ConverterBase<JobHeaderData, JobHeaderData> {
  public:
    JobHeaderJobHeaderConverter()
      : ConverterBase<JobHeaderData, JobHeaderData>("JobHeaderJobHeaderConverter") {}

  private:
    JobHeaderData* _convert(const JobHeaderData* jh) const {
        return new JobHeaderData(*jh);
    }
};

class JobHeaderPyDictConverter : public ConverterBase<JobHeaderData, PyObject> {
  public:
    JobHeaderPyDictConverter()
      : ConverterBase<JobHeaderData, PyObject>("JobHeaderPyDictConverter") {}

  private:
    PyObject* _convert(const JobHeaderData* jh) const {
        std::string* str = JobHeaderStringConverter().convert(jh);
        PyObject* obj = StringPyDictConverter().convert(str);
        delete str;
        return obj;
    }
};
}
#endif //  SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERSJH_H


