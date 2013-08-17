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

#include <string>
#include <limits>

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "src/common_cpp/Optics/PolynomialOpticsModel.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/OpticsModel.hh"

#include "src/py_cpp/optics/PyCovarianceMatrix.hh"
#include "src/py_cpp/optics/PyPhaseSpaceVector.hh"
#include "src/py_cpp/optics/PyOpticsModel.hh"

namespace MAUS {
namespace PyOpticsModel {

std::string transport_covariance_matrix_docstring =
std::string("Transport a CovarianceMatrix.\n\n")+
std::string(" - cov_matrix (covariance_matrix): CovarianceMatrix object\n")+
std::string("   that represents the start CovarianceMatrix\n")+
std::string(" - end_position (float): z position for tracking. Optics\n")+
std::string("   will attempt to track to the nearest VirtualPlane to this\n")+
std::string("   z position.\n")+
std::string("Returns a new transported covariance_matrix object");

// note we don't use Transport(cov_matrix, start, end) as this can make a
// segmentation fault
PyObject* transport_covariance_matrix(PyObject *self, PyObject *args,
                                                               PyObject *kwds) {
    PyOpticsModel* py_optics_model = reinterpret_cast<PyOpticsModel*>(self);
    if (py_optics_model == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to interpret self as an optics_model");
        return NULL;
    }
    OpticsModel* optics_model = get_optics_model(py_optics_model);

    PyObject* py_cm_in = NULL;
    double end_plane = std::numeric_limits<double>::max()/10.;
    static char *kwlist[] = {const_cast<char*>("cov_matrix"),
                             const_cast<char*>("end_position"), NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "Od", kwlist,
                                         &py_cm_in, &end_plane)) {
        return NULL;
    }
    CovarianceMatrix* cm_in =
                            PyCovarianceMatrix::get_covariance_matrix(py_cm_in);
    if (cm_in == NULL) {
        PyErr_SetString(PyExc_TypeError,
                       "Failed to extract a covariance matrix from cov_matrix");
        return NULL;
    }
    CovarianceMatrix* cm_out = NULL;
    try {
        cm_out = new CovarianceMatrix(optics_model->Transport
                                                           (*cm_in, end_plane));
    }
    catch(Exception exc) {
        PyErr_SetString(PyExc_RuntimeError, exc.what());
        return NULL;
    }
    PyObject* py_cm_out = PyCovarianceMatrix::create_empty_matrix();
    PyCovarianceMatrix::set_covariance_matrix(py_cm_out, cm_out);
    Py_INCREF(py_cm_out);
    return py_cm_out;
}


std::string transport_phase_space_vector_docstring =
std::string("Transport a PhaseSpaceVector.\n\n")+
std::string(" - phase_space_vector (PhaseSpaceVector): phase space vector\n")+
std::string("   object that represents the start coordinate\n")+
std::string(" - end_position (float): z position for tracking. Optics\n")+
std::string("   will attempt to track to the nearest VirtualPlane to this\n")+
std::string("   z position.\n")+
std::string("Returns a new transported PhaseSpaceVector");

// note we don't use Transport(cov_matrix, start, end) as this can make a
// segmentation fault
PyObject* transport_phase_space_vector(PyObject *self, PyObject *args,
                                                               PyObject *kwds) {
    PyOpticsModel* py_optics_model = reinterpret_cast<PyOpticsModel*>(self);
    if (py_optics_model == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to interpret self as an optics_model");
        return NULL;
    }
    OpticsModel* optics_model = get_optics_model(py_optics_model);

    PyObject* py_psv_in = NULL;
    double end_plane = std::numeric_limits<double>::max()/10.;
    static char *kwlist[] = {const_cast<char*>("phase_space_vector"),
                             const_cast<char*>("end_position"), NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "Od", kwlist,
                                         &py_psv_in, &end_plane)) {
        return NULL;
    }
    PhaseSpaceVector* psv_in =
                          PyPhaseSpaceVector::get_phase_space_vector(py_psv_in);
    if (psv_in == NULL) {
        PyErr_SetString(PyExc_TypeError,
                "Failed to extract a PhaseSpaceVector from phase_space_vector");
        return NULL;
    }
    PhaseSpaceVector* psv_out = NULL;
    try {
        psv_out = new PhaseSpaceVector(optics_model->Transport
                                                          (*psv_in, end_plane));
    }
    catch(Exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, exc.what());
        return NULL;
    }
    PyObject* py_psv_out = PyPhaseSpaceVector::create_empty_vector();
    PyPhaseSpaceVector::set_phase_space_vector(py_psv_out, psv_out);
    Py_INCREF(py_psv_out);
    return py_psv_out;
}

PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems) {
    void* void_optics = malloc(sizeof(PyOpticsModel));
    PyOpticsModel* optics = reinterpret_cast<PyOpticsModel*>(void_optics);
    optics->model = NULL;
    optics->ob_refcnt = 1;
    optics->ob_type = type;
    return reinterpret_cast<PyObject*>(optics);
}

bool is_built = false;

int _init(PyObject* self, PyObject *args, PyObject *kwds) {
    PyOpticsModel* optics = reinterpret_cast<PyOpticsModel*>(self);
    // failed to cast or self was not initialised - something horrible happened
    if (optics == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve self as OpticsModel in __init__");
        return -1;
    }
    // legal python to call initialised_object.__init__() to reinitialise, so
    // handle this case
    if (optics->model != NULL) {
        delete optics->model;
        optics->model = NULL;
    }
    // now initialise the internal optics model; for now hardcoded to
    // PolynomialOpticsModel
    try {
        Json::Value* cards = Globals::GetConfigurationCards();
        // just gets deltas (dx, dy, ...) and polynomial order
        optics->model = new MAUS::PolynomialOpticsModel(*cards);
        // uses MAUS::Globals::MAUSGeant4Manager for geometry, etc
        optics->model->Build();
        is_built = true;  // done by constructor
    } catch(Exception exc) {
        PyErr_SetString(PyExc_RuntimeError, exc.what());
        return -1;
    }
    return 0;
}

PyObject *_new(PyTypeObject *type, Py_ssize_t nitems) {
    return _alloc(type, nitems);
}

void _dealloc(PyOpticsModel * self) {
    _free(self);
}

void _free(PyOpticsModel * self) {
    if (self != NULL) {
        if (self->model != NULL)
            delete self->model;
        free(self);
    }
}

static PyMemberDef _members[] = {
{NULL}
};

OpticsModel* get_optics_model(PyOpticsModel* py_model) {
    return py_model->model;
}

static PyMethodDef _methods[] = {
{"transport_covariance_matrix", (PyCFunction)transport_covariance_matrix,
  METH_VARARGS|METH_KEYWORDS, transport_covariance_matrix_docstring.c_str()},
{"transport_phase_space_vector", (PyCFunction)transport_phase_space_vector,
  METH_VARARGS|METH_KEYWORDS, transport_phase_space_vector_docstring.c_str()},
{NULL}
};

const char* module_docstring =
  "optics_model module; merely a place holder for OpticsModel";

std::string class_docstring =
std::string("OpticsModel transports particles and beam ellipses.\n\n")+
std::string("OpticsModel provides bindings to transport particles and beam\n")+
std::string("ellipses.\n\n")+
std::string("__init__()\n")+
std::string("    Takes no arguments. Sets up the OpticsModel with datacards\n")+
std::string("    and geometry stored in globals. Transport is initialised\n")+
std::string("    by expansion relative to 'simulation_reference_particle'\n")+
std::string("    datacard which also defines the start position for all\n")+
std::string("    transport.");

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
    0, // (newfunc)_new,   /* tp_new */
    (freefunc)_free, /* tp_free, called by dealloc */
};

PyMODINIT_FUNC initoptics_model(void) {
    PyOpticsModelType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyOpticsModelType) < 0)
        return;

    int success = PyCovarianceMatrix::import_PyCovarianceMatrix();
    if (success != 1)
        return;
    success = PyPhaseSpaceVector::import_PyPhaseSpaceVector();
    if (success != 1)
        return;

    PyObject* module = Py_InitModule3("optics_model", NULL, module_docstring);
    if (module == NULL)
        return;

    PyTypeObject* optics_model_type = &PyOpticsModelType;
    Py_INCREF(optics_model_type);
    PyModule_AddObject(module, "OpticsModel",
                       reinterpret_cast<PyObject*>(optics_model_type));
}
}  // namespace PyOpticsModel
}  // namespace MAUS

