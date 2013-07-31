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

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <structmember.h>

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"

#define MAUS_PYPHASESPACEVECTOR_CC
#include "PyPhaseSpaceVector.hh"
#undef MAUS_PYPHASESPACEVECTOR_CC

namespace MAUS {
namespace PyPhaseSpaceVector {

static PyMemberDef _members[] = {
{NULL}
};

static PyMethodDef _methods[] = {
{NULL}
};

const char* module_docstring =
  "phase_space_vector module; merely a place holder for PhaseSpaceVector class";

const char* class_docstring =
  "PhaseSpaceVector provides bindings for particle tracks.";


static PyTypeObject PyPhaseSpaceVectorType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "phase_space_vector.PhaseSpaceVector",         /*tp_name*/
    sizeof(PyPhaseSpaceVector),           /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)_free, /*tp_dealloc*/
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
    class_docstring,           /* tp_doc */
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
    0,                  /* tp_new */
    (freefunc)_free, /* tp_free, called by dealloc */
};


PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems) {
    void* void_psv = malloc(sizeof(PyPhaseSpaceVector));
    PyPhaseSpaceVector* py_psv =
                                reinterpret_cast<PyPhaseSpaceVector*>(void_psv);
    py_psv->psv = NULL;
    py_psv->ob_refcnt = 1;
    py_psv->ob_type = type;
    return reinterpret_cast<PyObject*>(py_psv);
}

int _init(PyObject* self, PyObject *args, PyObject *kwds) {
    PyPhaseSpaceVector* py_psv = reinterpret_cast<PyPhaseSpaceVector*>(self);
    // failed to cast or self was not initialised - something horrible happened
    if (py_psv == NULL) {
        PyErr_SetString(PyExc_TypeError,
                    "Failed to resolve self as PyPhaseSpaceVector in __init__");
        return -1;
    }
    // legal python to call initialised_object.__init__() to reinitialise, so
    // handle this case
    if (py_psv->psv != NULL) {
        delete py_psv->psv;
        py_psv->psv = NULL;
    }
    /*
    // try to extract a numpy array from the arguments
    static char *kwlist[] = {(char*)"matrix"};
    PyObject* array = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &array)) {
        // error message is set in PyArg_Parse...
        return -1;
    }
    // now initialise the internal covariance matrix
    try {
        if (array == NULL)
            cm->cov_mat = new MAUS::CovarianceMatrix();
        else
            cm->cov_mat = create_from_numpy_matrix(array);
    } catch(Exception exc) {
        PyErr_SetString(PyExc_RuntimeError, exc.what());
        return -1;
    }
    */
    return 0;
}

void _free(PyPhaseSpaceVector * self) {
    if (self != NULL) {
        if (self->psv != NULL)
            delete self->psv;
        delete self;
    }
}


static PyMethodDef _keywdarg_methods[] = {
    {NULL,  NULL}   /* sentinel */
};

PyMODINIT_FUNC initphase_space_vector(void) {
    PyPhaseSpaceVectorType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyPhaseSpaceVectorType) < 0) return;

    PyObject* module = Py_InitModule("phase_space_vector", _keywdarg_methods); //, module_docstring);
    if (module == NULL) return;

    PyTypeObject* psv_type = &PyPhaseSpaceVectorType;
    Py_INCREF(psv_type);
    PyModule_AddObject(module, "PhaseSpaceVector", reinterpret_cast<PyObject*>(psv_type));

    // C API
    PyObject* psv_dict = PyModule_GetDict(module);
    PyObject* gpsv_c_api = PyCObject_FromVoidPtr((void *)C_API::get_phase_space_vector, NULL);
    PyObject* spsv_c_api = PyCObject_FromVoidPtr((void *)C_API::set_phase_space_vector, NULL);
    PyDict_SetItemString(psv_dict, "C_API_GET_PHASE_SPACE_VECTOR", gpsv_c_api);
    PyDict_SetItemString(psv_dict, "C_API_SET_PHASE_SPACE_VECTOR", spsv_c_api);
}

PhaseSpaceVector* C_API::get_phase_space_vector(PyObject* py_psv) {
    return reinterpret_cast<PyPhaseSpaceVector*>(py_psv)->psv;
}

void C_API::set_phase_space_vector(PyObject* py_psv_o, PhaseSpaceVector* psv) {
    PyPhaseSpaceVector* py_psv = reinterpret_cast<PyPhaseSpaceVector*>(py_psv_o);
    if (py_psv->psv != NULL) {
        delete py_psv->psv;
    }
    py_psv->psv = psv;
}


}
}

