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

#ifndef _SRC_COMMON_CPP_API_PYWRAPMAPBASE_INL_
#define _SRC_COMMON_CPP_API_PYWRAPMAPBASE_INL_

#include "Python.h"
#include "structmember.h"  // python PyMemberDef

#include <string>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {
template <class MAPCLASS>
PyObject* PyWrapMapBase<MAPCLASS>::birth(PyObject* self,
                                       PyObject *args,
                                       PyObject *kwds) {
  PyWrappedMap* py_map = reinterpret_cast<PyWrappedMap*>(self);
  if (!py_map->map) {
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
      py_map->map->birth(config);
  } catch (std::exception& exc) {
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
      return NULL;
  } catch (...) {
      PyErr_SetString(PyExc_SystemError, "Caught an unknown error during birth");
      return NULL;
  }

  Py_RETURN_NONE;
}

template <class MAPCLASS>
PyObject* PyWrapMapBase<MAPCLASS>::death(PyObject* self,
                                       PyObject *args,
                                       PyObject *kwds) {
  PyWrappedMap* py_map = reinterpret_cast<PyWrappedMap*>(self);
  if (!py_map->map) {
      PyErr_SetString(PyExc_ValueError, "self was not initialised properly");
      return NULL;
  }
  try {
      py_map->map->death();
  } catch (std::exception& exc) {
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
      return NULL;
  } catch (...) {
      PyErr_SetString(PyExc_SystemError, "Caught an unknown error during birth");
      return NULL;
  }

  Py_RETURN_NONE;
}

template <class MAPCLASS>
PyObject* PyWrapMapBase<MAPCLASS>::process(PyObject* self,
                                       PyObject *args,
                                       PyObject *kwds) {
  PyWrappedMap* py_map = reinterpret_cast<PyWrappedMap*>(self);
  if (!py_map->map) {
      PyErr_SetString(PyExc_ValueError, "self was not initialised properly");
      return NULL;
  }

  static char *kwlist[] = {const_cast<char*>("data"), NULL};
  PyObject* data_in;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &data_in)) {
    return NULL;
  }
  PyObject* data_out = py_map->map->process_pyobj(data_in);
  // PyObject* data_tuple_out = Py_BuildValue("(O)", &data_out);
  return data_out;
}

template <class MAPCLASS>
PyObject* PyWrapMapBase<MAPCLASS>::_new(PyTypeObject* type, PyObject *args, PyObject *kwds) {
    PyWrappedMap* self = reinterpret_cast<PyWrappedMap*>(type->tp_alloc(type, 0));
    self->map = NULL;
    return reinterpret_cast<PyObject*>(self);
}

template <class MAPCLASS>
std::string PyWrapMapBase<MAPCLASS>::_birth_docstring =
  std::string("Initialise the mapper based on datacards\n\n")+
  std::string(" - datacards: string representation of the control variables\n");

template <class MAPCLASS>
std::string PyWrapMapBase<MAPCLASS>::_process_docstring =
  std::string("Process the data\n\n")+
  std::string(" - data: a MAUS::Data event, either in json, string or MAUS\n")+
  std::string(" native representation.\n");

template <class MAPCLASS>
std::string PyWrapMapBase<MAPCLASS>::_death_docstring =
  std::string("Deinitialise the mapper ready for the next run\n");

template <class MAPCLASS>
std::string PyWrapMapBase<MAPCLASS>::_class_docstring =
  std::string("Class for processing the MAUS data.\n\n")+
  std::string("  def __init__(self)\n")+
  std::string("    Initialise the class\n");


template <class MAPCLASS>
int PyWrapMapBase<MAPCLASS>::_init(PyWrappedMap* self, PyObject *args, PyObject *kwds) {
    if (self->map == NULL)
        self->map = new MAPCLASS();
    return 0;
}

template <class MAPCLASS>
void PyWrapMapBase<MAPCLASS>::_dealloc(PyWrappedMap* self) {
    if (self->map != NULL)
        delete self->map;
}

template <class MAPCLASS>
PyMemberDef PyWrapMapBase<MAPCLASS>::_members[] = {
    {NULL}  /* Sentinel */
};


template <class MAPCLASS>
PyMethodDef PyWrapMapBase<MAPCLASS>::_methods[] = {
    {"birth", (PyCFunction)birth,
      METH_VARARGS|METH_KEYWORDS, _birth_docstring.c_str()},
    {"death", (PyCFunction)death,
      METH_VARARGS|METH_KEYWORDS, _death_docstring.c_str()},
    {"process", (PyCFunction)process,
      METH_VARARGS|METH_KEYWORDS, _process_docstring.c_str()},
    {NULL}  // Sentinel
};

template <class MAPCLASS>
PyMethodDef PyWrapMapBase<MAPCLASS>::_module_methods[] = {
    {NULL}  /* Sentinel */
};

template <class MAPCLASS>
std::string PyWrapMapBase<MAPCLASS>::_class_name = "";
template <class MAPCLASS>
std::string PyWrapMapBase<MAPCLASS>::_module_name = "";
template <class MAPCLASS>
std::string PyWrapMapBase<MAPCLASS>::_path_name = "";


template <class MAPCLASS>
PyTypeObject PyWrapMapBase<MAPCLASS>::_class_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    _path_name.c_str(),             /*tp_name*/
    sizeof(PyWrappedMap),             /*tp_basicsize*/
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
    PyWrapMapBase<MAPCLASS>::_methods,             /* tp_methods */
    PyWrapMapBase<MAPCLASS>::_members,             /* tp_members */
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

template <class MAPCLASS>
void PyWrapMapBase<MAPCLASS>::PyWrapMapBaseModInit(
                std::string class_docstring,
                std::string birth_docstring,
                std::string death_docstring,
                std::string process_docstring) {
    if (class_docstring != "")
        _class_docstring = class_docstring;
    if (birth_docstring != "")
        _birth_docstring = birth_docstring;
    if (death_docstring != "")
        _death_docstring = death_docstring;
    if (process_docstring != "")
        _process_docstring = process_docstring;
    _class_name = MAPCLASS().get_classname();
    _module_name = "_"+_class_name;
    _path_name = "_"+_class_name+"."+_class_name;
    _class_type.tp_name = _path_name.c_str();
    PyObject* module;

    if (PyType_Ready(&_class_type) < 0)
        return;

    module = Py_InitModule3(_module_name.c_str(), _module_methods,
                       "Better to import the class directly from MAUS.");

    if (module == NULL)
      return;

    Py_INCREF(&_class_type);
    PyModule_AddObject(module, _class_name.c_str(), (PyObject *)&_class_type);
}
}  // ~MAUS
#endif

