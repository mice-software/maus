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
 */

#ifndef _SRC_COMMON_CPP_API_PYWRAPOUTPUTBASE_INL_
#define _SRC_COMMON_CPP_API_PYWRAPOUTPUTBASE_INL_
#include <string>

#include "Python.h"
#include "structmember.h"  // python PyMemberDef

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {
template <class OUTPUTCLASS>
PyObject* PyWrapOutputBase<OUTPUTCLASS>::birth(PyObject* self,
                                       PyObject *args,
                                       PyObject *kwds) {
  PyWrappedOutput* py_output = reinterpret_cast<PyWrappedOutput*>(self);
  if (!py_output->output) {
      PyErr_SetString(PyExc_ValueError, "self was not initialised properly");
      return NULL;
  }

  static char *kwlist[] = {const_cast<char*>("datacards"), NULL};
  char* cards;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &cards)) {
    return NULL;
  }
  std::string config(cards);
  try {
      py_output->output->birth(config);
  } catch (std::exception& exc) {
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
      return NULL;
  } catch (...) {
      PyErr_SetString(PyExc_SystemError, "Caught an unknown error during birth");
      return NULL;
  }

  Py_RETURN_NONE;
}

template <class OUTPUTCLASS>
PyObject* PyWrapOutputBase<OUTPUTCLASS>::death(PyObject* self,
                                       PyObject *args,
                                       PyObject *kwds) {
  PyWrappedOutput* py_output = reinterpret_cast<PyWrappedOutput*>(self);
  if (!py_output->output) {
      PyErr_SetString(PyExc_ValueError, "self was not initialised properly");
      return NULL;
  }
  try {
      py_output->output->death();
  } catch (std::exception& exc) {
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
      return NULL;
  } catch (...) {
      PyErr_SetString(PyExc_SystemError, "Caught an unknown error during birth");
      return NULL;
  }

  Py_RETURN_NONE;
}

template <class OUTPUTCLASS>
PyObject* PyWrapOutputBase<OUTPUTCLASS>::save(PyObject* self,
                                       PyObject *args,
                                       PyObject *kwds) {
  PyWrappedOutput* py_output = reinterpret_cast<PyWrappedOutput*>(self);
  if (!py_output->output) {
      PyErr_SetString(PyExc_ValueError, "self was not initialised properly");
      return NULL;
  }

  static char *kwlist[] = {const_cast<char*>("data"), NULL};
  PyObject* data_in;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &data_in)) {
    return NULL;
  }
  // data_out should be Py_NONE or NULL (error)
  PyObject* data_out = py_output->output->save_pyobj(data_in);
  return data_out;
}

template <class OUTPUTCLASS>
PyObject* PyWrapOutputBase<OUTPUTCLASS>::_new(PyTypeObject* type, PyObject *args, PyObject *kwds) {
    PyWrappedOutput* self = reinterpret_cast<PyWrappedOutput*>(type->tp_alloc(type, 0));
    self->output = NULL;
    return reinterpret_cast<PyObject*>(self);
}

template <class OUTPUTCLASS>
std::string PyWrapOutputBase<OUTPUTCLASS>::_birth_docstring =
  std::string("Initialise the outputter based on datacards\n\n")+
  std::string(" - datacards: string representation of the control variables\n");

template <class OUTPUTCLASS>
std::string PyWrapOutputBase<OUTPUTCLASS>::_save_docstring =
  std::string("Save the data\n\n")+
  std::string(" - data: a parsable event, either in json, string or MAUS\n")+
  std::string(" native representation. Can be a MAUS.Data, MAUS.JobHeader,\n")+
  std::string(" MAUS.RunHeader, MAUS.RunFooter or MAUS.JobFooter object\n");

template <class OUTPUTCLASS>
std::string PyWrapOutputBase<OUTPUTCLASS>::_death_docstring =
  std::string("Deinitialise the outputter ready for the next run\n");

template <class OUTPUTCLASS>
std::string PyWrapOutputBase<OUTPUTCLASS>::_class_docstring =
  std::string("Class for saving the MAUS data.\n\n")+
  std::string("  def __init__(self)\n")+
  std::string("    Initialise the class\n");


template <class OUTPUTCLASS>
int PyWrapOutputBase<OUTPUTCLASS>::_init(PyWrappedOutput* self, PyObject *args, PyObject *kwds) {
    if (self->output == NULL)
        self->output = new OUTPUTCLASS();
    if (self->can_convert == NULL) {
        self->can_convert = Py_True;
        Py_INCREF(self->can_convert);
    }
    return 0;
}

template <class OUTPUTCLASS>
void PyWrapOutputBase<OUTPUTCLASS>::_dealloc(PyWrappedOutput* self) {
    if (self->output != NULL)
        delete self->output;
    Py_DECREF(self->can_convert);
}

template <class OUTPUTCLASS>
PyMemberDef PyWrapOutputBase<OUTPUTCLASS>::_members[] = {
  {const_cast<char*>("can_convert"),
  T_OBJECT_EX, offsetof(PyWrappedOutput, can_convert), 0,
  const_cast<char*>(
  "Returns true if the module can do conversions, else false (or non-existent)"
  )},
  {NULL}  /* Sentinel */
};


template <class OUTPUTCLASS>
PyMethodDef PyWrapOutputBase<OUTPUTCLASS>::_methods[] = {
    {"birth", (PyCFunction)birth,
      METH_VARARGS|METH_KEYWORDS, _birth_docstring.c_str()},
    {"death", (PyCFunction)death,
      METH_VARARGS|METH_KEYWORDS, _death_docstring.c_str()},
    {"save", (PyCFunction)save,
      METH_VARARGS|METH_KEYWORDS, _save_docstring.c_str()},
    {NULL}  // Sentinel
};

template <class OUTPUTCLASS>
PyMethodDef PyWrapOutputBase<OUTPUTCLASS>::_module_methods[] = {
    {NULL}  /* Sentinel */
};

template <class OUTPUTCLASS>
std::string PyWrapOutputBase<OUTPUTCLASS>::_class_name = "";
template <class OUTPUTCLASS>
std::string PyWrapOutputBase<OUTPUTCLASS>::_module_name = "";
template <class OUTPUTCLASS>
std::string PyWrapOutputBase<OUTPUTCLASS>::_path_name = "";


template <class OUTPUTCLASS>
PyTypeObject PyWrapOutputBase<OUTPUTCLASS>::_class_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    _path_name.c_str(),             /*tp_name*/
    sizeof(PyWrappedOutput),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    _class_docstring.c_str(),           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyWrapOutputBase<OUTPUTCLASS>::_methods,             /* tp_methods */
    PyWrapOutputBase<OUTPUTCLASS>::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)_init,      /* tp_init */
    0,                         /* tp_alloc */
    _new,                 /* tp_new */
};

template <class OUTPUTCLASS>
void PyWrapOutputBase<OUTPUTCLASS>::PyWrapOutputBaseModInit(
                std::string class_name,
                std::string class_docstring,
                std::string birth_docstring,
                std::string death_docstring,
                std::string save_docstring) {
    if (class_docstring != "")
        _class_docstring = class_docstring;
    if (birth_docstring != "")
        _birth_docstring = birth_docstring;
    if (death_docstring != "")
        _death_docstring = death_docstring;
    if (save_docstring != "")
        _save_docstring = save_docstring;
    _methods[0].ml_doc = _birth_docstring.c_str();
    _methods[1].ml_doc = _save_docstring.c_str();
    _methods[2].ml_doc = _death_docstring.c_str();

    _class_type.tp_doc = _class_docstring.c_str();
    // Static so allocates c_str() memory for lifetime of the program
    _class_name = class_name;
    _module_name = "_"+_class_name;
    _path_name = "_"+_class_name+"."+_class_name;
    _class_type.tp_name = _path_name.c_str();
    PyObject* module;

    if (PyType_Ready(&_class_type) < 0)
        return;

    module = Py_InitModule3(_module_name.c_str(), _module_methods,
                      "Please import the class directly from the MAUS module.");

    if (module == NULL)
      return;
    PyTypeObject* obj_class_type = &_class_type;
    Py_INCREF(obj_class_type);
    PyModule_AddObject(module,
                       _class_name.c_str(),
                       reinterpret_cast<PyObject*>(obj_class_type));
}
}  // ~MAUS
#endif

