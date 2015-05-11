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
 *
 */

#ifndef SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERS_H
#define SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERS_H
#include <string>

#include "json/json.h"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Converter/ConverterBase.hh"

// Tested in ConverterFactoryTest

namespace MAUS {

/////////////////////////// STRING TO X ////////////////////////////

class StringJsonConverter : public ConverterBase<std::string, Json::Value> {
  public:
    StringJsonConverter()
      : ConverterBase<std::string, Json::Value>("StringJsonConverter") {}

  private:
    Json::Value* _convert(const std::string* json) const {
        return new Json::Value(JsonWrapper::StringToJson(*json));
    }
};

class StringStringConverter : public ConverterBase<std::string, std::string> {
  public:
    StringStringConverter()
      : ConverterBase<std::string, std::string>("SelfConverter") {}

  private:
    std::string* _convert(const std::string* str) const {
        return new std::string(*str);
    }
};

class StringPyDictConverter : public ConverterBase<std::string, PyObject> {
  public:
    StringPyDictConverter()
      : ConverterBase<std::string, PyObject>("StringPyObjectConverter") {}

  private:
    inline PyObject* _convert(const std::string* str) const;
};

/////////////////////////// JSON TO X ////////////////////////////

class JsonStringConverter : public ConverterBase<Json::Value, std::string> {
  public:
    JsonStringConverter()
      : ConverterBase<Json::Value, std::string>("JsonStringConverter") {}

  private:
    std::string* _convert(const Json::Value* json) const {
        return new std::string(JsonWrapper::JsonToString(*json));
    }
};

class JsonJsonConverter : public ConverterBase<Json::Value, Json::Value> {
  public:
    JsonJsonConverter()
      : ConverterBase<Json::Value, Json::Value>("JsonJsonConverter") {}

  private:
    Json::Value* _convert(const Json::Value* json) const {
        return new Json::Value(*json);
    }
};

class JsonPyDictConverter : public ConverterBase<Json::Value, PyObject> {
  public:
    JsonPyDictConverter()
      : ConverterBase<Json::Value, PyObject>("JsonJsonConverter") {}

  private:
    PyObject* _convert(const Json::Value* json) const {
        std::string* str = JsonStringConverter().convert(json);
        PyObject* py_dict = StringPyDictConverter().convert(str);
        delete str;
        return py_dict;
    }
};

/////////////////////////// PYDICT TO X ////////////////////////////

class PyDictStringConverter : public ConverterBase<PyObject, std::string> {
  public:
    PyDictStringConverter()
      : ConverterBase<PyObject, std::string>("PyDictStringConverter") {}

  private:
    inline std::string* _convert(const PyObject* obj) const;
};

class PyDictJsonConverter : public ConverterBase<PyObject, Json::Value> {
  public:
    PyDictJsonConverter()
      : ConverterBase<PyObject, Json::Value>("PyDictJsonConverter") {}

  private:
    Json::Value* _convert(const PyObject* obj) const {
        std::string* str = PyDictStringConverter().convert(obj);
        Json::Value* json = StringJsonConverter().convert(str);
        delete str;
        return json;
    }
};


class PyDictPyDictConverter : public ConverterBase<PyObject, PyObject> {
  public:
    PyDictPyDictConverter()
      : ConverterBase<PyObject, PyObject>("PyDictPyDictConverter") {}

  private:
    PyObject* _convert(const PyObject* obj) const {
        PyObject* obj_non_const = const_cast<PyObject*>(obj);
        return PyDict_Copy(obj_non_const);
    }
};

PyObject* StringPyDictConverter::_convert(const std::string* str) const {
        if (!str)
            throw Exception(Exception::recoverable,
                            "string was NULL",
                            "StringPyDictConverter::_convert");
        // import json
        PyObject* json_module = PyImport_ImportModule("json");
        if (!json_module) {
            throw Exception(Exception::recoverable,
                            "Failed to import json",
                            "StringPyDictConverter::_convert");
        }
        // json.getattr("loads")
        PyObject* loads_function = PyObject_GetAttrString(json_module, "loads");
        Py_DECREF(json_module);
        if (!PyCallable_Check(loads_function))
            throw Exception(Exception::recoverable,
                            "Failed to get loads function",
                            "StringPyDictConverter::_convert");
        // json.loads(str)
        char* c_string = const_cast<char*>(str->c_str());
        char* format = const_cast<char*>("s");
        PyObject* py_dict = PyObject_CallFunction(loads_function,
                                                  format,
                                                  c_string);
        Py_DECREF(loads_function);
        if (!py_dict || !PyDict_Check(py_dict)) {
            if (py_dict)
                Py_DECREF(py_dict);
            throw Exception(Exception::recoverable,
                            "Failed to call loads and extract a dict",
                            "StringPyDictConverter::_convert");
        }
        return py_dict;
}

std::string* PyDictStringConverter::_convert(const PyObject* py_dict) const {
        if (!PyDict_Check(py_dict))
            throw Exception(Exception::recoverable,
                            "PyDict was NULL",
                            "PyDictStringConverter::_convert");
        // import json
        PyObject* json_module = PyImport_ImportModule("json");
        if (!json_module) {
            throw Exception(Exception::recoverable,
                            "Failed to import json",
                            "PyDictStringConverter::_convert");
        }
        // json.dumps(py_dict)
        PyObject* dumps_function = PyObject_GetAttrString(json_module, "dumps");
        Py_DECREF(json_module);
        if (!PyCallable_Check(dumps_function))
            throw Exception(Exception::recoverable,
                            "Failed to get dumps function",
                            "PyDictStringConverter::_convert");
        char* format = const_cast<char*>("O");
        PyObject* py_str = PyObject_CallFunction(dumps_function,
                                                  format,
                                                  py_dict);
        Py_DECREF(dumps_function);
        // memory owned by py_str; py_str checking done as part of this call
        char* c_string = PyString_AsString(py_str);
        if (!c_string)
            throw Exception(Exception::recoverable,
                            "Failed to extract string from dumps output",
                            "PyDictStringConverter::_convert");
        std::string* cpp_string = new std::string(c_string);
        Py_DECREF(py_str);
        return cpp_string;
}
}



#endif // SRC_COMMON_CPP_CONVERTERS_DATACONVERTERS_PRIMITIVECONVERTERS_H

