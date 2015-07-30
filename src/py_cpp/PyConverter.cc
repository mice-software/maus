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

#include <string>
#include "src/py_cpp/PyConverter.hh"

namespace MAUS {
namespace PyConverter {
template <class DATAREPR>
PyObject* py_wrap(PyObject* self, PyObject* args) {
  PyObject* py_data_in;
  if (!PyArg_ParseTuple(args, "O", &py_data_in)) {
    return NULL;
  }
  DATAREPR* data_out = NULL;
  PyObject* py_data_out = NULL;
  try {
      // for a moment MAUS owns this memory
      data_out = PyObjectWrapper::unwrap<DATAREPR>(py_data_in);
      // now python owns this memory
      py_data_out = PyObjectWrapper::wrap(data_out);
  } catch (Exception& exc) {
      if (data_out != NULL && py_data_out == NULL)
          throw; // memory was lost, raise it up...
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
  } catch (std::exception& exc) {
      if (data_out != NULL && py_data_out == NULL)
          throw; // memory was lost, raise it up...
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
  }
  return py_data_out;
}

PyObject* py_wrap_data(PyObject* self, PyObject* args) {
    return py_wrap<Data>(self, args);
}

PyObject* py_wrap_job_header(PyObject* self, PyObject* args) {
    return py_wrap<JobHeaderData>(self, args);
}

PyObject* py_wrap_job_footer(PyObject* self, PyObject* args) {
    return py_wrap<JobFooterData>(self, args);
}

PyObject* py_wrap_run_header(PyObject* self, PyObject* args) {
    return py_wrap<RunHeaderData>(self, args);
}

PyObject* py_wrap_run_footer(PyObject* self, PyObject* args) {
    return py_wrap<RunFooterData>(self, args);
}

PyObject* py_wrap_string(PyObject* self, PyObject* args) {
    return py_wrap<std::string>(self, args);
}

PyObject* py_wrap_pyobject(PyObject* self, PyObject* args) {
    return py_wrap<PyObject>(self, args);
}

PyObject* py_del_data_repr(PyObject* self, PyObject* args) {
    PyObject* py_data_in;
    if (!PyArg_ParseTuple(args, "O", &py_data_in)) {
      return NULL;
    }
    PyObject* name = PyObject_CallMethod(py_data_in,
                                         const_cast<char*>("Class_Name"),
                                         NULL);
    if (!name) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve object as a ROOT type");
        return NULL;
    }
    char* c_string = NULL;
    if (!PyArg_Parse(name, "s", &c_string)) {
        PyErr_SetString(PyExc_TypeError,
           "Could not resolve object as a ROOT type - Class_Name() should return string");
    }
    if (c_string == NULL || strcmp(c_string, "MAUS::Data") != 0) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve object as a MAUS::Data type");
    }
    py_data_in->ob_refcnt++;
    void * vptr = static_cast<void*>(TPyReturn(py_data_in));
    Data* data = static_cast<Data*>(vptr);
    delete data;
    Py_INCREF(Py_None);
    return Py_None;
}

std::string py_del_data_repr_docstring =
  std::string("Free memory owned by MAUS.Data\n")+
  std::string("  - data_representation: MAUS.Data representation of spill data\n")+
  std::string("Returns None\n")+
  std::string("Raises TypeError if data_representation is of incorrect type\n\n")+
  std::string("Explicit free must be called on MICE data when in MAUS.Data\n")+
  std::string("representation as underlying ROOT library does not respect\n")+
  std::string("Python reference counting. Otherwise a memory leak will \n")+
  std::string("ensue. Once freed, calling the referenced memory will result\n")+
  std::string("in a python crash. I'm very sorry about that, I can't see a\n")+
  std::string("better way.\n");

std::string json_repr_docstring =
  std::string("Represent data as a (py)json object\n")+
  std::string(" - data: MICE data in any representation.");

std::string string_repr_docstring =
  std::string("Represent data as a string object\n")+
  std::string(" - data: MICE data in any representation.");

std::string data_repr_docstring =
  std::string("Represent data as a MAUS.Data object\n")+
  std::string(" - data: MICE spill data in any representation.");

std::string job_header_repr_docstring =
  std::string("Represent data as a MAUS.JobHeaderData object\n")+
  std::string(" - data: MICE job header data in any representation.");

std::string job_footer_repr_docstring =
  std::string("Represent data as a MAUS.JobFooterData object\n")+
  std::string(" - data: MICE job footer data in any representation.");

std::string run_header_repr_docstring =
  std::string("Represent data as a MAUS.RunHeaderData object\n")+
  std::string(" - data: MICE run header data in any representation.");

std::string run_footer_repr_docstring =
  std::string("Represent data as a MAUS.RunFooterData object\n")+
  std::string(" - data: MICE run footer data in any representation.");

PyMethodDef methods[] = {
    {"json_repr", (PyCFunction)py_wrap_pyobject, METH_VARARGS,
                                           json_repr_docstring.c_str()},
    {"string_repr", (PyCFunction)py_wrap_string, METH_VARARGS,
                                           string_repr_docstring.c_str()},
    {"data_repr", (PyCFunction)py_wrap_data, METH_VARARGS,
                                           data_repr_docstring.c_str()},
    {"job_header_repr", (PyCFunction)py_wrap_job_header, METH_VARARGS,
                                           job_header_repr_docstring.c_str()},
    {"job_footer_repr", (PyCFunction)py_wrap_job_footer, METH_VARARGS,
                                           job_footer_repr_docstring.c_str()},
    {"run_header_repr", (PyCFunction)py_wrap_run_header, METH_VARARGS,
                                           run_header_repr_docstring.c_str()},
    {"run_footer_repr", (PyCFunction)py_wrap_run_footer, METH_VARARGS,
                                           run_footer_repr_docstring.c_str()},
    {"del_data_repr", (PyCFunction)py_del_data_repr, METH_VARARGS,
                                           py_del_data_repr_docstring.c_str()},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initconverter(void) {
  Py_Initialize();
  PyObject* converter_module = Py_InitModule3("converter", methods,
  "Handle data conversions between different representations of the MICE data");
  if (converter_module == NULL) return;
}
}
}

