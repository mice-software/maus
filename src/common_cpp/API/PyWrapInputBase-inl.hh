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

#ifndef _SRC_COMMON_CPP_API_PYWRAPINPUTBASE_INL_
#define _SRC_COMMON_CPP_API_PYWRAPINPUTBASE_INL_

#include <string>

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"
#include "structmember.h"

#include "src/common_cpp/API/PyWrapInputBaseEmitter.hh"

#include <iostream>

namespace MAUS {
template <class INPUTCLASS>
PyObject* PyWrapInputBase<INPUTCLASS>::birth(PyObject* self,
                                       PyObject *args,
                                       PyObject *kwds) {
  PyWrappedInput* py_input = reinterpret_cast<PyWrappedInput*>(self);
  if (!py_input->input) {
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
      py_input->input->birth(config);
  } catch (std::exception& exc) {
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
      return NULL;
  } catch (...) {
      PyErr_SetString(PyExc_SystemError, "Caught an unknown error during birth");
      return NULL;
  }

  Py_RETURN_NONE;
}

template <class INPUTCLASS>
PyObject* PyWrapInputBase<INPUTCLASS>::death(PyObject* self,
                                       PyObject *args,
                                       PyObject *kwds) {
  PyWrappedInput* py_input = reinterpret_cast<PyWrappedInput*>(self);
  if (!py_input->input) {
      PyErr_SetString(PyExc_ValueError, "self was not initialised properly");
      return NULL;
  }
  try {
      py_input->input->death();
  } catch (std::exception& exc) {
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
      return NULL;
  } catch (...) {
      PyErr_SetString(PyExc_SystemError, "Caught an unknown error during birth");
      return NULL;
  }

  Py_RETURN_NONE;
}

template <class INPUTCLASS>
PyObject* PyWrapInputBase<INPUTCLASS>::emitter(PyObject* self,
                                       PyObject *args,
                                       PyObject *kwds) {
  typedef PyWrapInputBaseEmitter<INPUTCLASS> Emitter;
  typedef typename Emitter::PyWrappedInputEmitter PyEmitter;
  PyTypeObject& type = Emitter::_class_type;
  PyEmitter* emitter = reinterpret_cast<PyEmitter*>(type.tp_alloc(&type, 0));
  PyWrappedInput* self_input = reinterpret_cast<PyWrappedInput*>(self);
  emitter->input = self_input->input;
  return reinterpret_cast<PyObject*>(emitter);
}

template <class INPUTCLASS>
PyObject* PyWrapInputBase<INPUTCLASS>::_new(PyTypeObject* type, PyObject *args, PyObject *kwds) {
    PyWrappedInput* self = reinterpret_cast<PyWrappedInput*>(type->tp_alloc(type, 0));
    self->input = NULL;
    return reinterpret_cast<PyObject*>(self);
}

template <class INPUTCLASS>
std::string PyWrapInputBase<INPUTCLASS>::_birth_docstring =
  std::string("Initialise the inputter based on datacards\n\n")+
  std::string(" - datacards: string representation of the control variables\n");

template <class INPUTCLASS>
std::string PyWrapInputBase<INPUTCLASS>::_emitter_docstring =
  std::string("Emit some data\n\n");

template <class INPUTCLASS>
std::string PyWrapInputBase<INPUTCLASS>::_death_docstring =
  std::string("Deinitialise the inputter ready for the next run\n");

template <class INPUTCLASS>
std::string PyWrapInputBase<INPUTCLASS>::_class_docstring =
  std::string("Class for generating MAUS data.\n\n")+
  std::string("  def __init__(self)\n")+
  std::string("    Initialise the class\n");


template <class INPUTCLASS>
int PyWrapInputBase<INPUTCLASS>::_init(PyWrappedInput* self, PyObject *args, PyObject *kwds) {
    if (self->input == NULL)
        self->input = new INPUTCLASS();
    return 0;
}

template <class INPUTCLASS>
void PyWrapInputBase<INPUTCLASS>::_dealloc(PyWrappedInput* self) {
    if (self->input != NULL)
        delete self->input;
}

template <class INPUTCLASS>
PyMemberDef PyWrapInputBase<INPUTCLASS>::_members[] = {
  {NULL}  /* Sentinel */
};

// static PyMemberDef _members[] = {
// {NULL}
// };


template <class INPUTCLASS>
PyMethodDef PyWrapInputBase<INPUTCLASS>::_methods[] = {
    {"birth", (PyCFunction)birth,
      METH_VARARGS|METH_KEYWORDS, _birth_docstring.c_str()},
    {"death", (PyCFunction)death,
      METH_VARARGS|METH_KEYWORDS, _death_docstring.c_str()},
    {"emitter", (PyCFunction)emitter,
      METH_VARARGS|METH_KEYWORDS, _emitter_docstring.c_str()},
    {NULL}  // Sentinel
};

template <class INPUTCLASS>
PyMethodDef PyWrapInputBase<INPUTCLASS>::_module_methods[] = {
    {NULL}  /* Sentinel */
};

template <class INPUTCLASS>
std::string PyWrapInputBase<INPUTCLASS>::_class_name = "";
template <class INPUTCLASS>
std::string PyWrapInputBase<INPUTCLASS>::_module_name = "";
template <class INPUTCLASS>
std::string PyWrapInputBase<INPUTCLASS>::_path_name = "";


template <class INPUTCLASS>
PyTypeObject PyWrapInputBase<INPUTCLASS>::_class_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    _path_name.c_str(),             /*tp_name*/
    sizeof(PyWrappedInput),             /*tp_basicsize*/
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
    PyWrapInputBase<INPUTCLASS>::_methods,             /* tp_methods */
    PyWrapInputBase<INPUTCLASS>::_members,             /* tp_members */
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

template <class INPUTCLASS>
void PyWrapInputBase<INPUTCLASS>::PyWrapInputBaseModInit(
                std::string class_name,
                std::string class_docstring,
                std::string birth_docstring,
                std::string death_docstring,
                std::string emitter_docstring) {
    if (class_docstring != "")
        _class_docstring = class_docstring;
    if (birth_docstring != "")
        _birth_docstring = birth_docstring;
    if (death_docstring != "")
        _death_docstring = death_docstring;
    if (emitter_docstring != "")
        _emitter_docstring = emitter_docstring;
    _methods[0].ml_doc = _birth_docstring.c_str();
    _methods[1].ml_doc = _emitter_docstring.c_str();
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

    // Now the iterator object
    typedef PyWrapInputBaseEmitter<INPUTCLASS> Emitter;
    Emitter::_path_name = _module_name+"."+Emitter::_class_name;
    Emitter::_class_type.tp_name = Emitter::_path_name.c_str();
    Emitter::_class_type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&Emitter::_class_type) < 0)
        return;
    PyTypeObject* emitter_class_type = &Emitter::_class_type;
    Py_INCREF(emitter_class_type);
    PyModule_AddObject(module,
                       Emitter::_class_name.c_str(),
                       reinterpret_cast<PyObject*>(emitter_class_type));
}
}  // ~MAUS
#endif

