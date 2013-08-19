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

#include <string>

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"

#define MAUS_PYPHASESPACEVECTOR_CC
#include "src/py_cpp/optics/PyPhaseSpaceVector.hh"
#undef MAUS_PYPHASESPACEVECTOR_CC

namespace MAUS {
namespace PyPhaseSpaceVector {

PyObject* get(PyObject* self,
              double (PhaseSpaceVector::*get_function)() const) {
    PhaseSpaceVector* psv = C_API::get_phase_space_vector(self);
    if (psv == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve self as a PhaseSpaceVector");
        return NULL;
    }
    double value = (psv->*get_function)();
    PyObject* py_value = Py_BuildValue("d", value);
    if (py_value == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "PyCovarianceMatrix failed to get value");
        return NULL;
    }
    Py_INCREF(py_value);
    return py_value;
}

PyObject* set(PyObject* self, PyObject *args, PyObject *kwds,
              void (PhaseSpaceVector::*set_function)(double value)) {
    PhaseSpaceVector* psv = C_API::get_phase_space_vector(self);
    if (psv == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve self as a PhaseSpaceVector");
        return NULL;
    }

    double value = 0;
    static char *kwlist[] = {const_cast<char*>("value"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "d|", kwlist, &value)) {
        return NULL;
    }
    (psv->*set_function)(value);
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_t_docstring =
std::string("Returns the time [ns]\n");

PyObject* get_t(PyObject* self, PyObject *args, PyObject *kwds) {
    return get(self, &PhaseSpaceVector::t);
}

std::string get_energy_docstring =
std::string("Returns the total energy (not kinetic energy) [MeV]\n");

PyObject* get_energy(PyObject* self, PyObject *args, PyObject *kwds) {
    return get(self, &PhaseSpaceVector::E);
}

std::string get_x_docstring =
std::string("Returns the horizontal position x [mm]\n");

PyObject* get_x(PyObject* self, PyObject *args, PyObject *kwds) {
    return get(self, &PhaseSpaceVector::x);
}

std::string get_px_docstring =
std::string("Returns the horizontal component of momentum px [MeV/c]\n");

PyObject* get_px(PyObject* self, PyObject *args, PyObject *kwds) {
    return get(self, &PhaseSpaceVector::Px);
}

std::string get_y_docstring =
std::string("Returns the vertical position y [mm]\n");

PyObject* get_y(PyObject* self, PyObject *args, PyObject *kwds) {
    return get(self, &PhaseSpaceVector::y);
}

std::string get_py_docstring =
std::string("Returns the vertical component of momentum py [MeV/c]\n");

PyObject* get_py(PyObject* self, PyObject *args, PyObject *kwds) {
    return get(self, &PhaseSpaceVector::Py);
}

std::string set_t_docstring =
std::string("Set the time\n\n")+
std::string("- value (float) phase space vector time [ns]\n")+
std::string("Returns None");

PyObject* set_t(PyObject* self, PyObject *args, PyObject *kwds) {
    return set(self, args, kwds, &PhaseSpaceVector::set_t);
}

std::string set_energy_docstring =
std::string("Set the total energy (not kinetic energy)\n\n")+
std::string("- value (float) phase space vector energy [MeV]\n")+
std::string("Returns None");

PyObject* set_energy(PyObject* self, PyObject *args, PyObject *kwds) {
    return set(self, args, kwds, &PhaseSpaceVector::set_energy);
}

std::string set_x_docstring =
std::string("Set the horizontal position\n\n")+
std::string("- value (float) horizontal position [mm]\n")+
std::string("Returns None");

PyObject* set_x(PyObject* self, PyObject *args, PyObject *kwds) {
    return set(self, args, kwds, &PhaseSpaceVector::set_x);
}

std::string set_px_docstring =
std::string("Set the horizontal component of momentum\n\n")+
std::string("- value (float) momentum px [MeV/c]\n")+
std::string("Returns None");

PyObject* set_px(PyObject* self, PyObject *args, PyObject *kwds) {
    return set(self, args, kwds, &PhaseSpaceVector::set_Px);
}

std::string set_y_docstring =
std::string("Set the vertical position\n\n")+
std::string("- value (float) vertical position [mm]\n")+
std::string("Returns None");

PyObject* set_y(PyObject* self, PyObject *args, PyObject *kwds) {
    return set(self, args, kwds, &PhaseSpaceVector::set_y);
}

std::string set_py_docstring =
std::string("Set the vertical component of momentum\n\n")+
std::string("- value (float) vertical momentum [MeV/c]\n")+
std::string("Returns None");

PyObject* set_py(PyObject* self, PyObject *args, PyObject *kwds) {
    return set(self, args, kwds, &PhaseSpaceVector::set_Py);
}

static PyMemberDef _members[] = {
{NULL}
};

static PyMethodDef _methods[] = {
{"get_t", (PyCFunction)get_t, METH_VARARGS|METH_KEYWORDS,
                                                       get_t_docstring.c_str()},
{"get_energy", (PyCFunction)get_energy, METH_VARARGS|METH_KEYWORDS,
                                                  get_energy_docstring.c_str()},
{"get_x", (PyCFunction)get_x, METH_VARARGS|METH_KEYWORDS,
                                                  get_x_docstring.c_str()},
{"get_px", (PyCFunction)get_px, METH_VARARGS|METH_KEYWORDS,
                                                  get_px_docstring.c_str()},
{"get_y", (PyCFunction)get_y, METH_VARARGS|METH_KEYWORDS,
                                                  get_y_docstring.c_str()},
{"get_py", (PyCFunction)get_py, METH_VARARGS|METH_KEYWORDS,
                                                  get_py_docstring.c_str()},

{"set_t", (PyCFunction)set_t, METH_VARARGS|METH_KEYWORDS,
                                                 set_t_docstring.c_str()},
{"set_energy", (PyCFunction)set_energy, METH_VARARGS|METH_KEYWORDS,
                                                 set_energy_docstring.c_str()},
{"set_x", (PyCFunction)set_x, METH_VARARGS|METH_KEYWORDS,
                                                 set_x_docstring.c_str()},
{"set_px", (PyCFunction)set_px, METH_VARARGS|METH_KEYWORDS,
                                                 set_px_docstring.c_str()},
{"set_y", (PyCFunction)set_y, METH_VARARGS|METH_KEYWORDS,
                                                 set_y_docstring.c_str()},
{"set_py", (PyCFunction)set_py, METH_VARARGS|METH_KEYWORDS,
                                                 set_py_docstring.c_str()},
{NULL}
};


static PyObject* _str(PyObject * self) {
    PhaseSpaceVector* psv = C_API::get_phase_space_vector(self);
    if (psv == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "PyPhaseSpaceVector not initialised properly");
        return NULL;
    }
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), " [%10g, %10g, %10g, %10g, %10g, %10g]",
      (*psv).t(), (*psv).energy(),
      (*psv).x(), (*psv).Px(),
      (*psv).y(), (*psv).Py());
    return PyString_FromString(buffer);
}



const char* module_docstring =
  "phase_space_vector module; merely a place holder for PhaseSpaceVector class";

std::string class_docstring =
std::string("PhaseSpaceVector provides bindings for particle tracks.\n\n")+
std::string("__init__()\n")+
std::string("    Takes no arguments. Returns a PhaseSpaceVector initialised\n")+
std::string("    0. Note that PhaseSpaceVector coordinates are considered\n")+
std::string("    relative to 0, not relative to the\n")+
std::string("    'simulation_reference_particle'.\n");


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
    _str,                      /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    _str,                      /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    class_docstring.c_str(),           /* tp_doc */
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
    // legal to call __init__ on an existing object
    if (py_psv->psv == NULL)
        py_psv->psv = new PhaseSpaceVector(0., 0., 0., 0., 0., 0.);
    return 0;
}

std::string create_from_coordinates_docstring =
std::string("Create a new phase space vector given coordinates\n\n")+
std::string(" - t (float) time [ns]\n")+
std::string(" - energy (float) energy [MeV]\n")+
std::string(" - x (float) horizontal position [mm]\n")+
std::string(" - px (float) horizontal component of momentum [MeV/c]\n")+
std::string(" - y (float) vertical position [mm]\n")+
std::string(" - py (float) vertical component of momentum [MeV/c]\n")+
std::string("Returns the new phase space vector");

PyObject* create_from_coordinates(PyObject* self,
                                  PyObject *args,
                                  PyObject *kwds) {
    PyPhaseSpaceVector* py_psv =
           reinterpret_cast<PyPhaseSpaceVector*>(C_API::create_empty_vector());
    if (py_psv == NULL) {
        PyErr_SetString(PyExc_TypeError,
                    "Failed to initialise PyPhaseSpaceVector");
        return NULL;
    }
    double t(0), E(0), x(0), px(0), y(0), py(0);
    // try to extract a numpy array from the arguments
    static char *kwlist[] = {const_cast<char*>("t"),
                             const_cast<char*>("energy"),
                             const_cast<char*>("x"),
                             const_cast<char*>("px"),
                             const_cast<char*>("y"),
                             const_cast<char*>("py"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddddd|", kwlist, &t, &E,
                                                                    &x, &px,
                                                                    &y, &py)) {
        // error message is set in PyArg_Parse...
        free(py_psv);
        return NULL;
    }
    // now initialise the internal phase space vector
    try {
        py_psv->psv = new PhaseSpaceVector(t, E, x, px, y, py);
    } catch(std::exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        free(py_psv);
        return NULL;
    }
    Py_INCREF(py_psv);
    return reinterpret_cast<PyObject*>(py_psv);
}

void _free(PyPhaseSpaceVector * self) {
    if (self != NULL) {
        if (self->psv != NULL)
            delete self->psv;
        free(self);
    }
}

static PyMethodDef _keywdarg_methods[] = {
    {"create_from_coordinates", (PyCFunction)create_from_coordinates,
    METH_VARARGS|METH_KEYWORDS, create_from_coordinates_docstring.c_str()},
    {NULL,  NULL}   /* sentinel */
};

PyMODINIT_FUNC initphase_space_vector(void) {
    PyPhaseSpaceVectorType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyPhaseSpaceVectorType) < 0) return;

    PyObject* module = Py_InitModule3
                    ("phase_space_vector", _keywdarg_methods, module_docstring);
    if (module == NULL) return;

    PyTypeObject* psv_type = &PyPhaseSpaceVectorType;
    Py_INCREF(psv_type);
    PyModule_AddObject
            (module, "PhaseSpaceVector", reinterpret_cast<PyObject*>(psv_type));

    // C API
    PyObject* psv_dict = PyModule_GetDict(module);
    PyObject* cev_c_api = PyCObject_FromVoidPtr
                    (reinterpret_cast<void*>(C_API::create_empty_vector), NULL);
    PyObject* gpsv_c_api = PyCObject_FromVoidPtr
                 (reinterpret_cast<void*>(C_API::get_phase_space_vector), NULL);
    PyObject* spsv_c_api = PyCObject_FromVoidPtr
                 (reinterpret_cast<void*>(C_API::set_phase_space_vector), NULL);
    PyDict_SetItemString(psv_dict, "C_API_CREATE_EMPTY_VECTOR", cev_c_api);
    PyDict_SetItemString(psv_dict, "C_API_GET_PHASE_SPACE_VECTOR", gpsv_c_api);
    PyDict_SetItemString(psv_dict, "C_API_SET_PHASE_SPACE_VECTOR", spsv_c_api);
}

PyObject *C_API::create_empty_vector() {
    return _alloc(&PyPhaseSpaceVectorType, 0);
}

PhaseSpaceVector* C_API::get_phase_space_vector(PyObject* py_psv) {
    if (py_psv == NULL || py_psv->ob_type != &PyPhaseSpaceVectorType) {
        PyErr_SetString(PyExc_TypeError,
                        "Could not resolve variable into a PhaseSpaceVector");
        return NULL;
    }
    PyPhaseSpaceVector* psv_ = reinterpret_cast<PyPhaseSpaceVector*>(py_psv);
    return psv_->psv;
}

int C_API::set_phase_space_vector(PyObject* py_psv_o, PhaseSpaceVector* psv) {
    if (py_psv_o == NULL || py_psv_o->ob_type != &PyPhaseSpaceVectorType) {
        PyErr_SetString(PyExc_TypeError,
                        "Could not resolve variable into a PhaseSpaceVector");
        return 0;
    }
    PyPhaseSpaceVector* py_psv =
                                reinterpret_cast<PyPhaseSpaceVector*>(py_psv_o);
    if (py_psv->psv != NULL) {
        delete py_psv->psv;
    }
    py_psv->psv = psv;
    return 1;
}
}
}

