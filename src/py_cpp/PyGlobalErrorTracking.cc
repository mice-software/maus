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

#include <string>

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#include <Python.h>
#include <structmember.h>

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Recon/Kalman/GlobalErrorTracking.hh"
#include "src/common_cpp/Recon/Global/MaterialModel.hh"
#include "src/py_cpp/PyGlobalErrorTracking.hh"

namespace MAUS {
namespace PyGlobalErrorTracking {

int get_centroid(PyObject* py_centroid, std::vector<double>& x_in) {
    if (!PyList_Check(py_centroid)) {
        PyErr_SetString(PyExc_TypeError, "centroid was not a list");
        return 1;
    }
    if (PyList_Size(py_centroid) != 8) {
        PyErr_SetString(PyExc_ValueError, "centroid was not length 8");
        return 1;
    }
    for (Py_ssize_t i = 0; i < 8; ++i) {
        PyObject* value = PyList_GetItem(py_centroid, i);
        if (value == NULL) {
            return 1;
        }
        if (!PyFloat_Check(value)) {
            PyErr_SetString(PyExc_TypeError, "centroid had non-float");
            return 1;
        }
        x_in[i] = PyFloat_AsDouble(value);
    }
    return 0;
}

int get_ellipse(PyObject* py_ellipse,
                                  std::vector<double>& x_in) {
    if (!PyList_Check(py_ellipse)) {
        PyErr_SetString(PyExc_TypeError, "ellipse was not a list");
        return 1;
    }
    if (PyList_Size(py_ellipse) != 6) {
        PyErr_SetString(PyExc_ValueError, "ellipse was not length 6");
        return 1;
    }
    int index = 8;
    for (Py_ssize_t i = 0; i < 6; ++i) {
        PyObject* py_row = PyList_GetItem(py_ellipse, i);
        if (py_row == NULL) {
            return 1;
        }
        if (!PyList_Check(py_row)) {
            PyErr_SetString(PyExc_TypeError, "ellipse row was not a list");
            return 1;
        }
        if (PyList_Size(py_row) != 6) {
            PyErr_SetString(PyExc_ValueError, "ellipse row was not length 6");
            return 1;
        }
        for (Py_ssize_t j = i; j < 6; ++j) {
            PyObject* py_value = PyList_GetItem(py_row, j);
            if (!PyFloat_Check(py_value)) {
                PyErr_SetString(PyExc_TypeError, "ellipse had non-float");
                return 1;
            }
            x_in[index] = PyFloat_AsDouble(py_value);
            index++;
        }
    }
    return 0;
}

PyObject* set_centroid(std::vector<double> x_in) {
    PyObject* list = PyList_New(8);
    for (Py_ssize_t i = 0; i < 8; ++i) {
        PyObject* element = PyFloat_FromDouble(x_in[i]);
        PyList_SetItem(list, i, element);
    }
    return list;
}

PyObject* set_matrix(std::vector< std::vector<double> > matrix) {
    PyObject* ellipse = PyList_New(6);
    for (Py_ssize_t i = 0; i < 6; ++i) {
        PyObject* ellipse_row = PyList_New(6);
        for (Py_ssize_t j = 0; j < 6; ++j) {
            PyObject* element = PyFloat_FromDouble(matrix[i][j]);
            PyList_SetItem(ellipse_row, j, element);
        }
        PyList_SetItem(ellipse, i, ellipse_row);
    }
    return ellipse;
}

PyObject* set_variance(std::vector<double> x_in) {
    int index = 8;
    std::vector< std::vector<double> > matrix(6, std::vector<double>(6, 0.));
    for (size_t i = 0; i < 6; ++i)
        for (size_t j = i; j < 6; ++j) {
            matrix[i][j] = matrix[j][i] = x_in[index];
            index++;
        }
    return set_matrix(matrix);
}

std::string set_deviations_docstring = "DOCSTRING";

static PyObject* set_deviations(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("dx"),
                             const_cast<char*>("dy"),
                             const_cast<char*>("dz"),
                             const_cast<char*>("dt"),
                             NULL};
    std::vector<double> delta(4, 0.);
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddd", kwlist,
                                     &delta[0], &delta[1], &delta[2], &delta[3])) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    glet->SetDeviations(delta[0], delta[1], delta[2], delta[3]);
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_deviations_docstring = "DOCSTRING";

static PyObject* get_deviations(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    std::vector<double> dev = glet->GetDeviations();
    PyObject* value = Py_BuildValue("dddd", dev[0], dev[1], dev[2], dev[3]);
    return value;
}

std::string enable_material_docstring = "DOCSTRING";

static PyObject* enable_material(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("material"),
                             NULL};
    char* material = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &material)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    MaterialModel::EnableMaterial(std::string(material));
    Py_INCREF(Py_None);
    return Py_None;
}

std::string disable_material_docstring = "DOCSTRING";

static PyObject* disable_material(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("material"),
                             NULL};
    char* material = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &material)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    MaterialModel::DisableMaterial(std::string(material));
    Py_INCREF(Py_None);
    return Py_None;
}

std::string is_enabled_material_docstring = "DOCSTRING";

static PyObject* is_enabled_material(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("material"),
                             NULL};
    char* material = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &material)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    bool enabled = MaterialModel::IsEnabledMaterial(std::string(material));
    if (enabled) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

std::string set_step_size_docstring = "DOCSTRING";

static PyObject* set_step_size(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("step_size"),
                             NULL};
    double step_size = 1.;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "d", kwlist,
                                     &step_size)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    glet->SetStepSize(step_size);
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_step_size_docstring = "DOCSTRING";

static PyObject* get_step_size(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    double step = glet->GetStepSize();
    PyObject* value = PyFloat_FromDouble(step);
    Py_INCREF(value);
    return value;
}

std::string set_energy_loss_model_docstring = "DOCSTRING";

static PyObject* set_energy_loss_model(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("model"),
                             NULL};
    char* eloss = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &eloss)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    // now set the eloss model
    GlobalErrorTracking::ELossModel eloss_model = GlobalErrorTracking::no_eloss;
    if (strcmp(eloss, "no_eloss") == 0) {
    } else if (strcmp(eloss, "bethe_bloch_forwards") == 0) {
        eloss_model = GlobalErrorTracking::bethe_bloch_forwards;
    } else if (strcmp(eloss, "bethe_bloch_backwards") == 0) {
        eloss_model = GlobalErrorTracking::bethe_bloch_backwards;
    } else {
        PyErr_SetString(PyExc_RuntimeError, "Did not recognise energy loss model");
        return NULL;
    }
    glet->SetEnergyLossModel(eloss_model);
    // return None
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_energy_loss_model_docstring = "DOCSTRING";

static PyObject* get_energy_loss_model(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    GlobalErrorTracking::ELossModel eloss_model = glet->GetEnergyLossModel();
    PyObject* py_eloss_model = NULL;
    switch (eloss_model) {
        case GlobalErrorTracking::bethe_bloch_forwards:
            py_eloss_model = PyString_FromString("bethe_bloch_forwards");
            break;
        case GlobalErrorTracking::bethe_bloch_backwards:
            py_eloss_model = PyString_FromString("bethe_bloch_backwards");
            break;
        case GlobalErrorTracking::no_eloss:
            py_eloss_model = PyString_FromString("no_eloss");
            break;
    }
    if (py_eloss_model != NULL)
        Py_INCREF(py_eloss_model);
    return py_eloss_model;
}

std::string set_scattering_model_docstring = "DOCSTRING";
// enum MCSModel {moliere_forwards, moliere_backwards, no_mcs};
static PyObject* set_scattering_model(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("model"),
                             NULL};
    char* scat_model_str = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &scat_model_str)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    // now set the eloss model
    GlobalErrorTracking::MCSModel scat_model = GlobalErrorTracking::no_mcs;
    if (strcmp(scat_model_str, "no_mcs") == 0) {
    } else if (strcmp(scat_model_str, "moliere_forwards") == 0) {
        scat_model = GlobalErrorTracking::moliere_forwards;
    } else if (strcmp(scat_model_str, "moliere_backwards") == 0) {
        scat_model = GlobalErrorTracking::moliere_backwards;
    } else {
        PyErr_SetString(PyExc_RuntimeError, "Did not recognise scattering model");
        return NULL;
    }
    glet->SetMCSModel(scat_model);
    // return None
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_scattering_model_docstring = "DOCSTRING";

static PyObject* get_scattering_model(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    GlobalErrorTracking::MCSModel scat_model = glet->GetMCSModel();
    PyObject* py_scat_model = NULL;
    switch (scat_model) {
        case GlobalErrorTracking::moliere_forwards:
            py_scat_model = PyString_FromString("moliere_forwards");
            break;
        case GlobalErrorTracking::moliere_backwards:
            py_scat_model = PyString_FromString("moliere_backwards");
            break;
        case GlobalErrorTracking::no_mcs:
            py_scat_model = PyString_FromString("no_mcs");
            break;
    }
    if (py_scat_model != NULL)
        Py_INCREF(py_scat_model);
    return py_scat_model;
}


std::string propagate_errors_docstring = "DOCSTRING";

static PyObject* propagate_errors
                              (PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("centroid"),
                             const_cast<char*>("ellipse"),
                             const_cast<char*>("target_z"),
                             NULL};
    PyObject* py_centroid = NULL;
    PyObject* py_ellipse = NULL;
    double target_z = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOd|", kwlist, &py_centroid, &py_ellipse, &target_z)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }

    std::vector<double> x_in(29, 0.);
    if (get_centroid(py_centroid, x_in)) {
        return NULL;
    }
    if (get_ellipse(py_ellipse, x_in)) {
        return NULL;
    }
    try {
        glet->SetField(Globals::GetInstance()->GetMCFieldConstructor());
        glet->Propagate(&x_in[0], target_z);
    } catch (MAUS::Exception exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
    }
    // who owns memory?
    py_centroid = set_centroid(x_in);
    py_ellipse = set_variance(x_in);
    PyObject* ret_tuple = Py_BuildValue("OO", py_centroid, py_ellipse);
    return ret_tuple;
}

std::string get_transfer_matrix_docstring = "DOCSTRING";

static PyObject* get_transfer_matrix
                              (PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {
        const_cast<char*>("centroid"),
        const_cast<char*>("direction"),
        NULL};
    PyObject* py_centroid = NULL;
    double direction = 1.;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "Od|", kwlist,
                                     &py_centroid, &direction)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    GlobalErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }

    try {
        std::vector<double> x_in(8, 0.);
        if (get_centroid(py_centroid, x_in)) {
            return NULL;
        }
        glet->UpdateTransferMatrix(&x_in[0], direction);
        std::vector<std::vector<double> > matrix = glet->GetMatrix();
        PyObject* ellipse = set_matrix(matrix);
        if (ellipse == NULL) {
            PyErr_SetString(PyExc_RuntimeError, "Error calculating matrix");
            return NULL;
        }
        return ellipse;
    } catch (MAUS::Exception exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
    }
}

std::string class_docstring_str = "CLASS DOCUMENTATION";
const char* class_docstring = class_docstring_str.c_str();

PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems) {
    void* void_glet = malloc(sizeof(PyGlobalErrorTracking));
    PyGlobalErrorTracking* glet = reinterpret_cast<PyGlobalErrorTracking*>(void_glet);
    glet->tracking = NULL;
    glet->ob_refcnt = 1;
    glet->ob_type = type;
    return reinterpret_cast<PyObject*>(glet);
}

int _init(PyObject* self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    // failed to cast or self was not initialised - something horrible happened
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve self as PyGlobalErrorTracking in __init__");
        return -1;
    }
    // legal python to call initialised_object.__init__() to reinitialise, so
    // handle this case
    if (glet->tracking != NULL) {
        delete glet->tracking;
    }

    try {
        glet->tracking = new GlobalErrorTracking();
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_ValueError, (&exc)->what());
        return -1;
    }

    return 0;
}

void _free(PyGlobalErrorTracking * self) {
    if (self != NULL) {
        if (self->tracking != NULL)
            delete self->tracking;
        free(self);
    }
}

PyObject* _str(PyObject * self) {
    return PyString_FromString("GlobalErrorTracking object");
}


static PyMemberDef _members[] = {
{NULL}
};

static PyMethodDef _methods[] = {
{"propagate_errors", (PyCFunction)propagate_errors,
  METH_VARARGS|METH_KEYWORDS, propagate_errors_docstring.c_str()},
{"get_transfer_matrix", (PyCFunction)get_transfer_matrix,
  METH_VARARGS|METH_KEYWORDS, get_transfer_matrix_docstring.c_str()},
{"set_deviations", (PyCFunction)set_deviations,
  METH_VARARGS|METH_KEYWORDS, set_deviations_docstring.c_str()},
{"get_deviations", (PyCFunction)get_deviations,
  METH_VARARGS|METH_KEYWORDS, get_deviations_docstring.c_str()},
{"set_step_size", (PyCFunction)set_step_size,
  METH_VARARGS|METH_KEYWORDS, set_step_size_docstring.c_str()},
{"get_step_size", (PyCFunction)get_step_size,
  METH_VARARGS|METH_KEYWORDS, get_step_size_docstring.c_str()},
{"set_energy_loss_model", (PyCFunction)set_energy_loss_model,
  METH_VARARGS|METH_KEYWORDS, set_energy_loss_model_docstring.c_str()},
{"get_energy_loss_model", (PyCFunction)get_energy_loss_model,
  METH_VARARGS|METH_KEYWORDS, get_energy_loss_model_docstring.c_str()},
{"set_scattering_model", (PyCFunction)set_scattering_model,
  METH_VARARGS|METH_KEYWORDS, set_scattering_model_docstring.c_str()},
{"get_scattering_model", (PyCFunction)get_scattering_model,
  METH_VARARGS|METH_KEYWORDS, get_scattering_model_docstring.c_str()},
{NULL}
};

static PyTypeObject PyGlobalErrorTrackingType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "global_error_tracking.GlobalErrorTracking",         /*tp_name*/
    sizeof(PyGlobalErrorTracking),           /*tp_basicsize*/
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
    PyType_GenericNew,                  /* tp_new */
    (freefunc)_free, /* tp_free, called by dealloc */
};

static PyMethodDef _keywdarg_methods[] = {
    {"enable_material", (PyCFunction)enable_material,
      METH_VARARGS|METH_KEYWORDS, enable_material_docstring.c_str()},
    {"disable_material", (PyCFunction)disable_material,
      METH_VARARGS|METH_KEYWORDS, disable_material_docstring.c_str()},
    {"is_enabled_material", (PyCFunction)is_enabled_material,
      METH_VARARGS|METH_KEYWORDS, is_enabled_material_docstring.c_str()},
    {NULL,  NULL}   /* sentinel */
};

const char* module_docstring = "MODULE DOCSTRING";

PyMODINIT_FUNC initglobal_error_tracking(void) {
    if (PyType_Ready(&PyGlobalErrorTrackingType) < 0)
        return;

    PyObject* module = Py_InitModule3("global_error_tracking",
                                      _keywdarg_methods,
                                      module_docstring);
    if (module == NULL) return;

    PyTypeObject* tracking_type = &PyGlobalErrorTrackingType;
    Py_INCREF(tracking_type);
    PyModule_AddObject(module,
                       "GlobalErrorTracking",
                       reinterpret_cast<PyObject*>(tracking_type));
}
}  // namespace PyGlobalErrorTracking
}  // namespace MAUS

