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

#ifndef _SRC_COMMON_CPP_API_PYWRAPINPUTBASEEMITTER_
#define _SRC_COMMON_CPP_API_PYWRAPINPUTBASEEMITTER_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"
#include "structmember.h"


namespace MAUS {

template <class INPUTCLASS>
class PyWrapInputBaseEmitter {
 public:
  typedef struct {
    PyObject_HEAD;
    INPUTCLASS* input;
  } PyWrappedInputEmitter;

 private:
    static PyObject* Iter(PyObject *self);
    static PyObject* Next(PyObject *self);
    static PyTypeObject _class_type;

    static std::string _class_name;  // Class
    static std::string _path_name;  // module.Class

    // only PyWrapInputBase can initialise/destruct PyWrapInputBaseEmitter
    friend class PyWrapInputBase<INPUTCLASS>;
};


template <class INPUTCLASS>
PyObject* PyWrapInputBaseEmitter<INPUTCLASS>::Iter(PyObject *self) {
  Py_INCREF(self);
  return self;
}

template <class INPUTCLASS>
PyObject* PyWrapInputBaseEmitter<INPUTCLASS>::Next(PyObject *self) {
  PyWrappedInputEmitter *py_wrap_emitter =
                               reinterpret_cast<PyWrappedInputEmitter*>(self);
  if (py_wrap_emitter) {
      PyObject* out = py_wrap_emitter->input->emit_pyobj();
      if (out) {
          return out;
      } else {
          /* Raising of standard StopIteration exception with empty value. */
          PyErr_SetNone(PyExc_StopIteration);
          return NULL;
      }
  } else {
      PyErr_SetString(PyExc_RuntimeError, "Failed to get the emitter");
      return NULL;
  }
}

template <class INPUTCLASS>
PyTypeObject PyWrapInputBaseEmitter<INPUTCLASS>::_class_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "PyWrapInputBaseIterator",            /*tp_name*/
    sizeof(PyWrappedInputEmitter),       /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER,
      /* tp_flags: Py_TPFLAGS_HAVE_ITER tells python to
         use tp_iter and tp_iternext fields. */
    "PyWrapInputBase iterator object.",           /* tp_doc */
    0,  /* tp_traverse */
    0,  /* tp_clear */
    0,  /* tp_richcompare */
    0,  /* tp_weaklistoffset */
    PyWrapInputBaseEmitter<INPUTCLASS>::Iter,  /* tp_iter: __iter__() method */
    PyWrapInputBaseEmitter<INPUTCLASS>::Next  /* tp_iternext: next() method */
};

template <class INPUTCLASS>
std::string PyWrapInputBaseEmitter<INPUTCLASS>::_class_name = "";  // Class
template <class INPUTCLASS>
std::string PyWrapInputBaseEmitter<INPUTCLASS>::_path_name = "";  // module.Class

} // namespace MAUS
#endif

