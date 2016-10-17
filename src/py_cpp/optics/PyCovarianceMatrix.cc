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
#include <string.h>
#include <stdio.h>

#include <string>

#include "numpy/arrayobject.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Maths/SymmetricMatrix.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"

#define MAUS_PYCOVARIANCEMATRIX_CC
#include "src/py_cpp/optics/PyCovarianceMatrix.hh"
#undef MAUS_PYCOVARIANCEMATRIX_CC

namespace MAUS {
namespace PyCovarianceMatrix {

std::string get_element_docstring =
std::string("Get an element of the covariance matrix\n\n")+
std::string(" - row (int) Row from which to get the element, indexed from 1\n")+
std::string("   to 6 inclusive\n")+
std::string(" - column (int) Column from which to get the element, indexed\n")+
std::string("   from 1 to 6 inclusive\n")+
std::string("Covariances are for vector U with variables ordered like\n")+
std::string("   (t, energy, x, px, y, py)\n")+
std::string("with standard Geant4 system of units\n")+
std::string("   ([ns], [MeV], [mm], [MeV/c], [mm], [MeV/c])\n")+
std::string("Returns the corresponding covariance (float).");

PyObject* get_element(PyObject* self, PyObject *args, PyObject *kwds) {
    CovarianceMatrix* cm = C_API::get_covariance_matrix(self);
    if (cm == NULL)
        return NULL;

    int row = 0;
    int col = 0;
    double value = 0;
    static char *kwlist[] = {const_cast<char*>("row"),
                             const_cast<char*>("column"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii|", kwlist, &row, &col)) {
        return NULL;
    }

    try {
        value = (*cm)(row, col);
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_IndexError, (&exc)->what());
        return NULL;
    }
    PyObject* py_value = Py_BuildValue("d", value);
    if (py_value == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "PyCovarianceMatrix failed to get value");
        return NULL;
    }
    Py_INCREF(py_value);
    return py_value;
}

std::string set_element_docstring =
std::string("Set an element of the covariance matrix\n\n")+
std::string(" - row (int) Row from which to get the element, indexed from 1\n")+
std::string("   to 6 inclusive\n")+
std::string(" - column (int) Column from which to get the element, indexed\n")+
std::string("   from 1 to 6 inclusive\n")+
std::string(" - value (float) Column from which to get the element\n")+
std::string("Covariances are for vector U with variables ordered like\n")+
std::string("   (t, energy, x, px, y, py)\n")+
std::string("with standard Geant4 system of units\n")+
std::string("   ([ns], [MeV], [mm], [MeV/c], [mm], [MeV/c])\n")+
std::string("Returns None");

PyObject* set_element(PyObject* self, PyObject *args, PyObject *kwds) {
    CovarianceMatrix* cm = C_API::get_covariance_matrix(self);
    if (cm == NULL)
        return NULL;

    int row = 0;
    int col = 0;
    double value = 0;
    static char *kwlist[] = {const_cast<char*>("row"),
                             const_cast<char*>("column"),
                             const_cast<char*>("value"),
                             NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iid|", kwlist, &row, &col,
                                                                      &value)) {
        return NULL;
    }

    try {
        cm->set(row, col, value);
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_IndexError, (&exc)->what());
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* _str(PyObject * self) {
    CovarianceMatrix* cm = C_API::get_covariance_matrix(self);
    if (cm == NULL)
        return NULL;
    char buffer[1024];
    std::string row =  "[%10g, %10g, %10g, %10g, %10g, %10g]";
    std::string matrix = " ["+row+",\n  "+row+",\n   "+row+",\n   "+row+
                            ",\n   "+row+",\n   "+row+"]";
    snprintf(buffer, sizeof(buffer), matrix.c_str(),
      (*cm)(1, 1), (*cm)(1, 2), (*cm)(1, 3), (*cm)(1, 4), (*cm)(1, 5), (*cm)(1, 6),
      (*cm)(2, 1), (*cm)(2, 2), (*cm)(2, 3), (*cm)(2, 4), (*cm)(2, 5), (*cm)(2, 6),
      (*cm)(3, 1), (*cm)(3, 2), (*cm)(3, 3), (*cm)(3, 4), (*cm)(3, 5), (*cm)(3, 6),
      (*cm)(4, 1), (*cm)(4, 2), (*cm)(4, 3), (*cm)(4, 4), (*cm)(4, 5), (*cm)(4, 6),
      (*cm)(5, 1), (*cm)(5, 2), (*cm)(5, 3), (*cm)(5, 4), (*cm)(5, 5), (*cm)(5, 6),
      (*cm)(6, 1), (*cm)(6, 2), (*cm)(6, 3), (*cm)(6, 4), (*cm)(6, 5), (*cm)(6, 6));
    return PyString_FromString(buffer);
}

const char* module_docstring =
  "covariance_matrix module for the CovarianceMatrix class";

std::string class_docstring =
std::string("CovarianceMatrix provides bindings for beam ellipses.\n\n")+
std::string("__init__()\n")+
std::string("    Takes no arguments. Initialises the covariance matrix to a\n")+
std::string("    6x6 matrix with elements all 0.\n");

static PyMemberDef _members[] = {
{NULL}
};

static PyMethodDef _methods[] = {
{"get_element", (PyCFunction)get_element,
  METH_VARARGS|METH_KEYWORDS, get_element_docstring.c_str()},
{"set_element", (PyCFunction)set_element,
  METH_VARARGS|METH_KEYWORDS, set_element_docstring.c_str()},
{NULL}
};

static PyTypeObject PyCovarianceMatrixType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "covariance_matrix.CovarianceMatrix",         /*tp_name*/
    sizeof(PyCovarianceMatrix),           /*tp_basicsize*/
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
    void* void_cm = malloc(sizeof(PyCovarianceMatrix));
    PyCovarianceMatrix* cm = reinterpret_cast<PyCovarianceMatrix*>(void_cm);
    cm->cov_mat = NULL;
    cm->ob_refcnt = 1;
    cm->ob_type = type;
    return reinterpret_cast<PyObject*>(cm);
}

CovarianceMatrix* create_from_numpy_matrix(PyObject* numpy_array) {
    PyArrayObject* array = reinterpret_cast<PyArrayObject*>(numpy_array);
    if (array == NULL) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                        "Attempting to pass an object that is not a numpy array",
                        "PyCovarianceMatrix::create_from_numpy_matrix"));
    }
    if (PyArray_NDIM(array) != 2) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                        "numpy_array had wrong dimension - should be matrix",
                        "PyCovarianceMatrix::create_from_numpy_matrix"));
    }
    if (PyArray_DIM(array, 0) != 6 || PyArray_DIM(array, 1) != 6) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                        "numpy_array had wrong size - should be 6x6 matrix",
                        "PyCovarianceMatrix::create_from_numpy_matrix"));
    }
    SymmetricMatrix raw_mat(6);
    for (size_t i = 0; i < 6; ++i)
        for (size_t j = i; j < 6; ++j) {
            double* value =
                  reinterpret_cast<double*>(PyArray_GETPTR2(numpy_array, i, j));
            if (value == NULL) {
                throw(Exceptions::Exception(Exceptions::recoverable,
                               "numpy_array had wrong data type",
                               "PyCovarianceMatrix::create_from_numpy_matrix"));
            }
            raw_mat.set(i+1, j+1, *value);
        }
    return new CovarianceMatrix(raw_mat);
}

int _init(PyObject* self, PyObject *args, PyObject *kwds) {
    PyCovarianceMatrix* cm = reinterpret_cast<PyCovarianceMatrix*>(self);
    // failed to cast or self was not initialised - something horrible happened
    if (cm == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve self as PyCovarianceMatrix in __init__");
        return -1;
    }
    // legal python to call initialised_object.__init__() to reinitialise, so
    // handle this case
    if (cm->cov_mat == NULL) {
        cm->cov_mat = new MAUS::CovarianceMatrix();
    }
    return 0;
}

void _free(PyCovarianceMatrix * self) {
    if (self != NULL) {
        if (self->cov_mat != NULL)
            delete self->cov_mat;
        free(self);
    }
}

//  create_from_twiss_parameters doc string
std::string create_from_twiss_parameters_docstring =
std::string("Create a CovarianceMatrix from Penn parameters.\n\n")+
std::string("Penn defines a parameterisation for cylindrically symmetric\n")+
std::string("beam ellipses that is often followed in solenodial beam\n")+
std::string("optics. Following parameters are mandatory:\n")+
std::string(" - mass (float): nominal particle mass for beam particles\n")+
std::string("   [MeV/c^2]\n")+
std::string(" - momentum (float): nominal particle momentum for beam\n")+
std::string("   particles [MeV/c]\n")+
std::string(" - emittance_x (float): horizontal emittance [mm]\n")+
std::string(" - beta_x (float): horizontal optical beta function [mm]\n")+
std::string(" - emittance_y (float): vertical emittance [mm]\n")+
std::string(" - beta_y (float): vertical optical beta function [mm]\n")+
std::string(" - emittance_l (float): longitudinal emittance [?]\n")+
std::string(" - beta_l (float): longitudinal optical beta function [?]\n\n")+
std::string("Following parameters are optional, taking given default if not\n")+
std::string("specified by the user:\n")+
std::string(" - alpha_x (float, 0): horizontal optical alpha function\n")+
std::string(" - alpha_y (float, 0): vertical optical alpha function\n")+
std::string(" - alpha_l (float, 0): longitudinal optical alpha function\n")+
std::string(" - dispersion_x (float, 0): dispersion in x direction [?]\n")+
std::string(" - dispersion_prime_x (float, 0): dispersion prime in\n")+
std::string("   horizontal direction (derivative with respect to s) [?]\n")+
std::string(" - dispersion_y (float, 0): dispersion in y direction [?]\n")+
std::string(" - dispersion_prime_y (float, 0): dispersion prime in\n")+
std::string("   vertical direction (derivative with respect to s) [?]\n");

PyObject* create_from_twiss_parameters
                              (PyObject *self, PyObject *args, PyObject *kwds) {
    double mass = 0.;
    double momentum = 0.;
    double emittance_x = 0.;
    double beta_x = 0.;
    double alpha_x = 0.;
    double emittance_y = 0.;
    double beta_y = 0.;
    double alpha_y = 0.;
    double emittance_l = 0.;
    double beta_l = 0.;
    double alpha_l = 0.;
    double dispersion_x = 0.;
    double dispersion_prime_x = 0.;
    double dispersion_y = 0.;
    double dispersion_prime_y = 0.;
    static char *kwlist[] = {const_cast<char*>("mass"),
                             const_cast<char*>("momentum"),
                const_cast<char*>("emittance_x"), const_cast<char*>("beta_x"),
                const_cast<char*>("emittance_y"), const_cast<char*>("beta_y"),
                const_cast<char*>("emittance_l"), const_cast<char*>("beta_l"),
                const_cast<char*>("alpha_x"), const_cast<char*>("alpha_y"),
                const_cast<char*>("alpha_l"),
                const_cast<char*>("dispersion_x"),
                const_cast<char*>("dispersion_prime_x"),
                const_cast<char*>("dispersion_y"),
                const_cast<char*>("dispersion_prime_y"), NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddddddd|ddddddd", kwlist,
        &mass, &momentum,
        &emittance_x, &beta_x,
        &emittance_y, &beta_y,
        &emittance_l, &beta_l,
        &alpha_x, &alpha_y, &alpha_l,
        &dispersion_x, &dispersion_prime_x,
        &dispersion_y, &dispersion_prime_y)) {
        return NULL;
    }

    // note that there is some reordering here in order to provide default
    // values for python api
    CovarianceMatrix* cm = new CovarianceMatrix(
        CovarianceMatrix::CreateFromTwissParameters(
            mass,
            momentum,
            emittance_x,
            beta_x,
            alpha_x,
            emittance_y,
            beta_y,
            alpha_y,
            emittance_l,
            beta_l,
            alpha_l,
            dispersion_x,
            dispersion_prime_x,
            dispersion_y,
            dispersion_prime_y));
    // Allocate a PyCovarianceMatrix
    // I *think* the INCREF on the TypeObject here is correct
    PyObject* py_cm = C_API::create_empty_matrix();
    C_API::set_covariance_matrix(py_cm, cm);
    Py_INCREF(py_cm);
    return py_cm;
}

//  create_from_penn_parameters doc string
std::string create_from_penn_parameters_docstring =
std::string("Create a CovarianceMatrix from Penn parameters.\n\n")+
std::string("Penn defines a parameterisation for cylindrically symmetric\n")+
std::string("beam ellipses that is often followed in solenodial beam\n")+
std::string("optics. Following parameters are mandatory:\n")+
std::string(" - mass (float): nominal particle mass for beam particles\n")+
std::string("   [MeV/c^2]\n")+
std::string(" - momentum (float): nominal particle momentum for beam\n")+
std::string("   particles [MeV/c]\n")+
std::string(" - emittance_t (float): transverse emittance [mm]\n")+
std::string(" - beta_t (float): transverse optical beta function [mm]\n")+
std::string(" - emittance_l (float): longitudinal emittance [?]\n")+
std::string(" - beta_l (float): longitudinal optical beta function [?]\n\n")+
std::string("Following parameters are optional, taking given default if not\n")+
std::string("specified by the user:\n")+
std::string(" - alpha_t (float, 0.): transverse optical alpha function\n")+
std::string(" - alpha_l (float, 0.): longitudinal optical alpha function\n")+
std::string(" - charge (float): nominal charge of particles [e+ charge]\n")+
std::string(" - bz (float): longitudinal magnetic field. If non-zero,\n")+
std::string("   the beam kinetic angular momentum. Note units are [kT]\n")+
std::string(" - ltwiddle (float): normalised canonical angular momentum\n")+
std::string(" - dispersion_x (float): dispersion in x direction [?]\n")+
std::string(" - dispersion_prime_x (float): dispersion prime in x\n")+
std::string("   direction (derivative with respect to s) [?]\n")+
std::string(" - dispersion_y (float): dispersion in y direction [?]\n")+
std::string(" - dispersion_prime_y (float): dispersion prime in y\n")+
std::string("   direction (derivative with respect to s) [?]\n");

PyObject* create_from_penn_parameters
                              (PyObject *self, PyObject *args, PyObject *kwds) {
    double mass = 0.;
    double momentum = 0.;
    double charge = 1.;
    double Bz = 0.;
    double Ltwiddle_t = 0.;
    double emittance_t = 0.;
    double beta_t = 0.;
    double alpha_t = 0.;
    double emittance_l = 0.;
    double beta_l = 0.;
    double alpha_l = 0.;
    double dispersion_x = 0.;
    double dispersion_prime_x = 0.;
    double dispersion_y = 0.;
    double dispersion_prime_y = 0.;
    static char *kwlist[] = {const_cast<char*>("mass"),
                             const_cast<char*>("momentum"),
                const_cast<char*>("emittance_t"), const_cast<char*>("beta_t"),
                const_cast<char*>("emittance_l"), const_cast<char*>("beta_l"),
                const_cast<char*>("alpha_t"), const_cast<char*>("alpha_l"),
                const_cast<char*>("charge"),
                const_cast<char*>("bz"), const_cast<char*>("ltwiddle"),
                const_cast<char*>("dispersion_x"),
                const_cast<char*>("dispersion_prime_x"),
                const_cast<char*>("dispersion_y"),
                const_cast<char*>("dispersion_prime_y"), NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddddd|ddddddddd", kwlist,
        &mass, &momentum, &emittance_t, &beta_t, &emittance_l, &beta_l,
        &alpha_t, &alpha_l, &charge, &Bz, &Ltwiddle_t,
        &dispersion_x, &dispersion_prime_x, &dispersion_y,
        &dispersion_prime_y)) {
        return NULL;
    }

    // note that there is some reordering here in order to provide default
    // values for python api
    CovarianceMatrix* cm = new CovarianceMatrix(
        CovarianceMatrix::CreateFromPennParameters(
          mass,
          momentum,
          charge,
          Bz,
          Ltwiddle_t,
          emittance_t,
          beta_t,
          alpha_t,
          emittance_l,
          beta_l,
          alpha_l,
          dispersion_x,
          dispersion_prime_x,
          dispersion_y,
          dispersion_prime_y));
    // Allocate a PyCovarianceMatrix
    // I *think* the INCREF on the TypeObject here is correct
    PyObject* py_cm = C_API::create_empty_matrix();
    C_API::set_covariance_matrix(py_cm, cm);
    Py_INCREF(py_cm);
    return py_cm;
}

std::string create_from_matrix_docstring =
std::string("Create a covariance matrix from a numpy matrix\n\n")+
std::string(" - matrix (numpy matrix) 6x6 matrix [various units] containing\n")+
std::string("   covariances with variables ordered like\n")+
std::string("   (t, energy, x, px, y, py)\n")+
std::string("Returns the constructed covariance matrix");

PyObject* create_from_matrix(PyObject* self, PyObject *args, PyObject *kwds) {
    PyCovarianceMatrix* cm =
            reinterpret_cast<PyCovarianceMatrix*>(C_API::create_empty_matrix());
    // failed to cast or self was not initialised - something horrible happened
    if (cm == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to allocate memory for PyCovarianceMatrix");
        free(cm);
        return NULL;
    }
    // try to extract a numpy array from the arguments
    static char *kwlist[] = {const_cast<char*>("matrix")};
    PyObject* array = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &array)) {
        // error message is set in PyArg_Parse...
        free(cm);
        return NULL;
    }
    // now initialise the internal covariance matrix
    try {
        cm->cov_mat = create_from_numpy_matrix(array);
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        free(cm);
        return NULL;
    }
    Py_INCREF(cm);
    return reinterpret_cast<PyObject*>(cm);
}

static PyMethodDef _keywdarg_methods[] = {
    {"create_from_matrix", (PyCFunction)create_from_matrix,
    METH_VARARGS|METH_KEYWORDS, create_from_matrix_docstring.c_str()},
    {"create_from_penn_parameters", (PyCFunction)create_from_penn_parameters,
    METH_VARARGS|METH_KEYWORDS, create_from_penn_parameters_docstring.c_str()},
    {"create_from_twiss_parameters", (PyCFunction)create_from_twiss_parameters,
    METH_VARARGS|METH_KEYWORDS, create_from_twiss_parameters_docstring.c_str()},
    {NULL,  NULL}   /* sentinel */
};

PyMODINIT_FUNC initcovariance_matrix(void) {
    PyCovarianceMatrixType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyCovarianceMatrixType) < 0) return;

    PyObject* module = Py_InitModule3("covariance_matrix", _keywdarg_methods,
                                                           module_docstring);
    if (module == NULL) return;

    PyTypeObject* cm_type = &PyCovarianceMatrixType;
    Py_INCREF(cm_type);
    PyModule_AddObject(module, "CovarianceMatrix",
                                          reinterpret_cast<PyObject*>(cm_type));

    // C API
    PyObject* cov_mat_dict = PyModule_GetDict(module);
    PyObject* cem_c_api = PyCObject_FromVoidPtr(reinterpret_cast<void*>
                                            (C_API::create_empty_matrix), NULL);
    PyObject* gcm_c_api = PyCObject_FromVoidPtr(reinterpret_cast<void*>
                                          (C_API::get_covariance_matrix), NULL);
    PyObject* scm_c_api = PyCObject_FromVoidPtr(reinterpret_cast<void*>
                                          (C_API::set_covariance_matrix), NULL);
    PyDict_SetItemString(cov_mat_dict, "C_API_CREATE_EMPTY_MATRIX", cem_c_api);
    PyDict_SetItemString(cov_mat_dict, "C_API_GET_COVARIANCE_MATRIX", gcm_c_api);
    PyDict_SetItemString(cov_mat_dict, "C_API_SET_COVARIANCE_MATRIX", scm_c_api);
}

CovarianceMatrix* C_API::get_covariance_matrix(PyObject* py_cm) {
    if (py_cm == NULL || py_cm->ob_type != &PyCovarianceMatrixType) {
        PyErr_SetString(PyExc_TypeError,
                        "Could not resolve object to a CovarianceMatrix");
        return NULL;
    }
    return reinterpret_cast<PyCovarianceMatrix*>(py_cm)->cov_mat;
}

int C_API::set_covariance_matrix(PyObject* py_cm_o, CovarianceMatrix* cm) {
    if (py_cm_o == NULL || py_cm_o->ob_type != &PyCovarianceMatrixType) {
        PyErr_SetString(PyExc_TypeError,
                        "Could not resolve object to a CovarianceMatrix");
        return 0;
    }
    PyCovarianceMatrix* py_cm = reinterpret_cast<PyCovarianceMatrix*>(py_cm_o);
    if (py_cm->cov_mat != NULL) {
        delete py_cm->cov_mat;
    }
    py_cm->cov_mat = cm;
    return 1;
}

PyObject *C_API::create_empty_matrix() {
    return _alloc(&PyCovarianceMatrixType, 0);
}
}
}
