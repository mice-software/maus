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


