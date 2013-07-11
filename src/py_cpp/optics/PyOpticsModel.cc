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

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

#include <structmember.h>
#include <string.h>
#include <stdio.h>

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Optics/OpticsModel.hh"
#include "src/common_cpp/Optics/LinearApproximationOpticsModel.hh"

#include "src/py_cpp/optics/PyOpticsModel.hh"

namespace MAUS {
namespace PyOpticsModel {

static PyMemberDef _members[] = {
{NULL}
};

OpticsModel* get_optics_model(PyOpticsModel* py_model) {
    return py_model->model;
}

/// Dummy function
static PyObject* get(PyObject* self, PyObject* args) {
    return NULL;
}

/// Dummy function
static PyObject* get_static(PyObject* self, PyObject* args) {
    return NULL;
}


static PyMethodDef _methods[] = {
{"get", (PyCFunction)get,           METH_VARARGS, "Docstring"},
{"get_static", (PyCFunction)get_static, METH_STATIC, "Docstring"},
{NULL}
};

static PyTypeObject PyOpticsModelType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "optics_model.OpticsModel",         /*tp_name*/
    sizeof(PyOpticsModel),           /*tp_basicsize*/
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
    "optics.OpticsModel docstring",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    _methods,           /* tp_methods */
    _members,           /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)_init,      /* tp_init */
    (allocfunc)_alloc,    /* tp_alloc, called by new */
    (newfunc)_new,   /* tp_new */
    (freefunc)_free, /* tp_free, called by dealloc */
};

PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems) {
    PyOpticsModel* optics = (PyOpticsModel*)malloc(sizeof(PyOpticsModel));
    optics->model = NULL;
    optics->ob_refcnt = 1;
    optics->ob_type = type;
    return (PyObject*)optics;
}

int _init(PyObject* self, PyObject *args, PyObject *kwds) {
    PyOpticsModel* optics = (PyOpticsModel*)self;
    if (optics == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve self as OpticsModel in __init__");
        return 1;
    }
    if (optics->model != NULL) {
        PyErr_SetString(PyExc_RuntimeError,
                        "C++ OpticsModel was unexpectedly initialised already");
        return 1;
    }
    optics->model = new MAUS::LinearApproximationOpticsModel(
                                              Globals::GetConfigurationCards());
    return 0;
}

PyObject *_new(PyTypeObject *type, Py_ssize_t nitems) {
    PyOpticsModel* optics = (PyOpticsModel*)_alloc(type, nitems);
    return (PyObject*)optics;
}

void _dealloc(PyOpticsModel * self) {
    _free(self);
}

void _free(PyOpticsModel * self) {
    if (self != NULL) {
        if (self->model != NULL)
            delete self->model;
        delete self;
    }
}

PyMODINIT_FUNC initoptics_model(void) {
    PyOpticsModelType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyOpticsModelType) < 0) return;

    PyObject* module = Py_InitModule3("optics_model", NULL, "optics model docstring");
    if (module == NULL) return;

    PyTypeObject* optics_model_type = &PyOpticsModelType;
    Py_INCREF(optics_model_type);
    PyModule_AddObject(module, "OpticsModel", (PyObject*)optics_model_type);
}

}  // namespace PyOpticsModel
}  // namespace MAUS

