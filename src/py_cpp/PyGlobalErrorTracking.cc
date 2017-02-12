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

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#include <Python.h>
#include <structmember.h>

#include <string>
#include <vector>

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTracking.hh"
#include "src/common_cpp/Recon/Global/MaterialModel.hh"
#include "src/py_cpp/PyGlobalErrorTracking.hh"

namespace MAUS {
namespace PyGlobalErrorTracking {

using Kalman::Global::ErrorTracking;

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

std::string set_deviations_docstring =
std::string("Set the deviations used for calculating field map derivatives\n")+
std::string(" - dx: horizontal deviation\n")+
std::string(" - dy: vertical deviation\n")+
std::string(" - dz: longitudinal deviation\n")+
std::string(" - dt: time deviation\n")+
std::string("Returns None\n");


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
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    glet->SetDeviations(delta[0], delta[1], delta[2], delta[3]);
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_deviations_docstring =
std::string("Get the deviations used for calculating field map derivatives\n")+
std::string(" - Takes no arguments\n")+
std::string("Returns a tuple of the deviations in x, y, z, time\n");

static PyObject* get_deviations(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    std::vector<double> dev = glet->GetDeviations();
    PyObject* value = Py_BuildValue("dddd", dev[0], dev[1], dev[2], dev[3]);
    return value;
}

std::string enable_material_docstring =
std::string("Enable a material so that it exerts energy loss and scattering\n")+
std::string(" - material: string name of the material to be enabled. By\n")+
std::string("   default materials are disabled.\n")+
std::string("Returns None\n");

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

std::string disable_material_docstring =
std::string("Disable a material\n")+
std::string(" - material: string name of the material to be disabled. By\n")+
std::string("   default materials are disabled. Volumes with disabled\n")+
std::string("   materials do not affect the tracking.\n")+
std::string("Returns None\n");

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

std::string is_enabled_material_docstring =
std::string("Return true if a material is enabled\n")+
std::string(" - material: string name of the material to check.\n")+
std::string("Returns a boolean True if the material was enabled, else False.\n");

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

std::string set_max_step_size_docstring =
std::string("Set the maximum step size that will be used by the tracking\n")+
std::string(" - max_step_size: float corresponding to the maximum step\n")+
std::string("   size. Steps can be smaller if the step might intersect a\n")+
std::string("   physical volume. Always takes absolute value (tracking\n")+
std::string("   determines direction).\n")+
std::string("Returns None.\n");

static PyObject* set_max_step_size(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("max_step_size"),
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
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    try {
        glet->SetMaxStepSize(step_size);
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_max_step_size_docstring =
std::string("Returns the maximum step size that will be used by the tracking\n");

static PyObject* get_max_step_size(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    double step = glet->GetMaxStepSize();
    PyObject* value = PyFloat_FromDouble(step);
    Py_INCREF(value);
    return value;
}

std::string set_min_step_size_docstring =
std::string("Set the minimum step size that will be used by the tracking\n")+
std::string(" - min_step_size: float corresponding to the minimum step\n")+
std::string("   size. Steps cannot be smaller even if there is a nearby\n")+
std::string("   physical volume. Always takes absolute value (tracking\n")+
std::string("   determines direction).\n")+
std::string("Returns None.\n");

static PyObject* set_min_step_size(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("min_step_size"),
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
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    try {
        glet->SetMinStepSize(step_size);
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_min_step_size_docstring =
std::string("Returns the minimum step size that will be used by the tracking\n");

static PyObject* get_min_step_size(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    double step = glet->GetMinStepSize();
    PyObject* value = PyFloat_FromDouble(step);
    Py_INCREF(value);
    return value;
}

std::string set_charge_docstring =
std::string("Set the particle charge that will be used by the tracking\n")+
std::string(" - charge: float corresponding to the particle that will be\n")+
std::string("   used by the tracking.\n")+
std::string("Returns None.\n");


static PyObject* set_charge(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("charge"),
                             NULL};
    double charge = 1.;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "d", kwlist,
                                     &charge)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    glet->SetCharge(charge);
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_charge_docstring =
std::string("Returns the particle charge that will be used by the tracking\n");

static PyObject* get_charge(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    double charge = glet->GetCharge();
    PyObject* value = PyFloat_FromDouble(charge);
    Py_INCREF(value);
    return value;
}

std::string set_energy_loss_model_docstring =
std::string("Set the energy loss model that will be used by the tracking\n")+
std::string(" - model: string corresponding to the energy loss model.\n")+
std::string("   Options are:\n")+
std::string("     'no_eloss' particles never lose energy in material.\n")+
std::string("     'bethe_bloch_forwards' forwards-travelling particles lose\n")+
std::string("      energy in material; backwards-travelling particles gain\n")+
std::string("      energy in material.\n")+
std::string("     'bethe_bloch_backwards' backwards-travelling particles lose\n")+
std::string("      energy in material; forwards-travelling particles gain\n")+
std::string("      energy in material.\n")+
std::string("Returns None.\n");

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
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    // now set the eloss model
    ErrorTracking::ELossModel eloss_model = ErrorTracking::no_eloss;
    if (strcmp(eloss, "no_eloss") == 0) {
    } else if (strcmp(eloss, "bethe_bloch_forwards") == 0) {
        eloss_model = ErrorTracking::bethe_bloch_forwards;
    } else if (strcmp(eloss, "bethe_bloch_backwards") == 0) {
        eloss_model = ErrorTracking::bethe_bloch_backwards;
    } else {
        PyErr_SetString(PyExc_RuntimeError, "Did not recognise energy loss model");
        return NULL;
    }
    glet->SetEnergyLossModel(eloss_model);
    // return None
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_energy_loss_model_docstring =
std::string("Returns a string corresponding to the energy loss model that\n")+
std::string("will be used by the tracking\n");


static PyObject* get_energy_loss_model(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    ErrorTracking::ELossModel eloss_model = glet->GetEnergyLossModel();
    PyObject* py_eloss_model = NULL;
    switch (eloss_model) {
        case ErrorTracking::bethe_bloch_forwards:
            py_eloss_model = PyString_FromString("bethe_bloch_forwards");
            break;
        case ErrorTracking::bethe_bloch_backwards:
            py_eloss_model = PyString_FromString("bethe_bloch_backwards");
            break;
        case ErrorTracking::no_eloss:
            py_eloss_model = PyString_FromString("no_eloss");
            break;
    }
    if (py_eloss_model != NULL)
        Py_INCREF(py_eloss_model);
    return py_eloss_model;
}

std::string set_scattering_model_docstring =
std::string("Set the scattering model that will be used by the tracking.\n")+
std::string("Scattering results in a systematic change in the propagated\n")+
std::string("errors. The mean trajectory is not deviated.\n")+
std::string(" - model: string corresponding to the scattering model.\n")+
std::string("   Options are:\n")+
std::string("     'no_mcs' propagated errors are not affected by scattering.\n")+
std::string("     'moliere_forwards' error associated with forwards-travelling\n")+
std::string("      particles grows according to the pdg formula. Error\n")+
std::string("      associated with backwards-travelling particles shrinks\n")+
std::string("      according to the pdg formula\n")+
std::string("     'moliere_backwards' error associated with forwards-travelling\n")+
std::string("      particles shrinks according to the pdg formula. Error\n")+
std::string("      associated with backwards-travelling particles grows\n")+
std::string("      according to the pdg formula\n")+
std::string("Returns None.\n");


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
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    // now set the eloss model
    ErrorTracking::MCSModel scat_model = ErrorTracking::no_mcs;
    if (strcmp(scat_model_str, "no_mcs") == 0) {
    } else if (strcmp(scat_model_str, "moliere_forwards") == 0) {
        scat_model = ErrorTracking::moliere_forwards;
    } else if (strcmp(scat_model_str, "moliere_backwards") == 0) {
        scat_model = ErrorTracking::moliere_backwards;
    } else {
        PyErr_SetString(PyExc_RuntimeError, "Did not recognise scattering model");
        return NULL;
    }
    glet->SetMCSModel(scat_model);
    // return None
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_scattering_model_docstring =
std::string("Returns a string corresponding to the scattering model that\n")+
std::string("will be used by the tracking.\n");

static PyObject* get_scattering_model(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    ErrorTracking::MCSModel scat_model = glet->GetMCSModel();
    PyObject* py_scat_model = NULL;
    switch (scat_model) {
        case ErrorTracking::moliere_forwards:
            py_scat_model = PyString_FromString("moliere_forwards");
            break;
        case ErrorTracking::moliere_backwards:
            py_scat_model = PyString_FromString("moliere_backwards");
            break;
        case ErrorTracking::no_mcs:
            py_scat_model = PyString_FromString("no_mcs");
            break;
    }
    if (py_scat_model != NULL)
        Py_INCREF(py_scat_model);
    return py_scat_model;
}

std::string set_tracking_model_docstring =
std::string("Set the tracking model that will be used for integration.\n")+
std::string(" - model: string corresponding to the tracking model.\n")+
std::string("   Options are:\n")+
std::string("     'em_rk4_forwards_dynamic' integrate the particle\n")+
std::string("      trajectory downstream, choosing step size dynamically.\n")+
std::string("     'em_rk4_backwards_dynamic' integrate the particle\n")+
std::string("      trajectory upstream, choosing step size dynamically.\n")+
std::string("     'em_rk4_forwards_static' integrate the particle\n")+
std::string("      trajectory downstream, using a fixed step size.\n")+
std::string("     'em_rk4_backwards_static' integrate the particle\n")+
std::string("      trajectory upstream, choosing step size dynamically.\n")+
std::string("Returns None.\n");
// enum MCSModel {moliere_forwards, moliere_backwards, no_mcs};
static PyObject* set_tracking_model(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("model"),
                             NULL};
    char* track_model_str = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &track_model_str)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    // now set the eloss model
    ErrorTracking::TrackingModel track_model = ErrorTracking::em_forwards_dynamic;
    if (strcmp(track_model_str, "em_rk4_forwards_dynamic") == 0) {
    } else if (strcmp(track_model_str, "em_rk4_backwards_dynamic") == 0) {
        track_model = ErrorTracking::em_backwards_dynamic;
    } else if (strcmp(track_model_str, "em_rk4_forwards_static") == 0) {
        track_model = ErrorTracking::em_forwards_static;
    } else if (strcmp(track_model_str, "em_rk4_backwards_static") == 0) {
        track_model = ErrorTracking::em_backwards_static;
    } else {
        PyErr_SetString(PyExc_RuntimeError, "Did not recognise tracking model");
        return NULL;
    }
    glet->SetTrackingModel(track_model);
    // return None
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_tracking_model_docstring =
std::string("Returns a string corresponding to the tracking model.\n");

static PyObject* get_tracking_model(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    ErrorTracking::TrackingModel track_model = glet->GetTrackingModel();
    PyObject* py_track_model = NULL;
    switch (track_model) {
        case ErrorTracking::em_forwards_dynamic:
            py_track_model = PyString_FromString("em_rk4_forwards_dynamic");
            break;
        case ErrorTracking::em_backwards_dynamic:
            py_track_model = PyString_FromString("em_rk4_backwards_dynamic");
            break;
        case ErrorTracking::em_forwards_static:
            py_track_model = PyString_FromString("em_rk4_forwards_static");
            break;
        case ErrorTracking::em_backwards_static:
            py_track_model = PyString_FromString("em_rk4_backwards_static");
            break;
    }
    if (py_track_model != NULL)
        Py_INCREF(py_track_model);
    return py_track_model;
}

std::string set_geometry_model_docstring =
std::string("Set the geometry model that will be used for materials lookups.\n")+
std::string(" - model: string corresponding to the geometry model.\n")+
std::string("   Options are:\n")+
std::string("     'geant4' use the full geant4 geometry.\n")+
std::string("     'axial_lookup' use a lookup table constructed at runtime,\n")+
std::string("     corresponding to the on-axis materials assuming the world\n")+
std::string("     is made of infinite radius cylinders.\n")+
std::string("Returns None.\n");

// enum MCSModel {moliere_forwards, moliere_backwards, no_mcs};
static PyObject* set_geometry_model(PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("model"),
                             NULL};
    char* geometry_model_str = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &geometry_model_str)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    // now set the eloss model
    ErrorTracking::GeometryModel geometry_model = ErrorTracking::geant4;
    if (strcmp(geometry_model_str, "geant4") == 0) {
    } else if (strcmp(geometry_model_str, "axial_lookup") == 0) {
        geometry_model = ErrorTracking::axial_lookup;
    } else {
        PyErr_SetString(PyExc_RuntimeError, "Did not recognise geometry model");
        return NULL;
    }
    glet->SetGeometryModel(geometry_model);
    // return None
    Py_INCREF(Py_None);
    return Py_None;
}

std::string get_geometry_model_docstring =
std::string("Get the geometry model that will be used for materials lookups.");

static PyObject* get_geometry_model(PyObject *self, PyObject *args, PyObject *kwds) {
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
    if (glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "GlobalErrorTracking was not initialised properly");
        return NULL;
    }
    ErrorTracking::GeometryModel geometry_model = glet->GetGeometryModel();
    PyObject* py_geometry_model = NULL;
    switch (geometry_model) {
        case ErrorTracking::geant4:
            py_geometry_model = PyString_FromString("geant4");
            break;
        case ErrorTracking::axial_lookup:
            py_geometry_model = PyString_FromString("axial_lookup");
            break;
    }
    if (py_geometry_model != NULL)
        Py_INCREF(py_geometry_model);
    return py_geometry_model;
}

std::string propagate_errors_docstring =
std::string("Propagate a trajectory and associated errors.\n")+
std::string(" - centroid: list of length 8, corresponding to initial\n")+
std::string("   position of the trajectory. elements are:\n")+
std::string("   t, x, y, z, (total) energy, px, py, pz\n")+
std::string(" - ellipse: list of lists, corresponding to a 6x6 matrix with\n")+
std::string("   elements defining the initial error covariance matrix, given\n")+
std::string("   by Cov(u_i, u_j) with u = (t, x, y, energy, px, py) and\n")+
std::string("   Cov(u_i, u_j) is the covariance.\n")+
std::string(" - target_z: target z position to which trajectory should be\n")+
std::string("   integrated.\n")+
std::string("Returns None.\n");

static PyObject* propagate_errors
                              (PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("centroid"),
                             const_cast<char*>("ellipse"),
                             const_cast<char*>("target_z"),
                             NULL};
    PyObject* py_centroid = NULL;
    PyObject* py_ellipse = NULL;
    double target_z = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOd|", kwlist,
                                     &py_centroid, &py_ellipse, &target_z)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    PyGlobalErrorTracking* py_glet = reinterpret_cast<PyGlobalErrorTracking*>(self);
    if (py_glet == NULL) {
        PyErr_SetString(PyExc_TypeError, "Could not resolve global error tracking");
        return NULL;
    }
    ErrorTracking* glet = py_glet->tracking;
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
    } catch (Exceptions::Exception exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
    }
    // py_centroid/py_ellipse own all of their memory
    py_centroid = set_centroid(x_in);
    py_ellipse = set_variance(x_in);
    // initialises with a refcnt 1
    PyObject* ret_tuple = Py_BuildValue("OO", py_centroid, py_ellipse);
    // py_centroid and py_ellipse are initialised with a ref count = 1;
    // BuildValue then adds a ref count; but only ret_tuple holds a reference
    Py_DECREF(py_centroid);
    Py_DECREF(py_ellipse);
    return ret_tuple; //ret_tuple;
}

std::string get_transfer_matrix_docstring =
std::string("Calculate the infinitesimal transfer matrix.\n")+
std::string("    - centroid: The central trajectory around which the transfer\n")+
std::string("      matrix is calculated. List of length 8, as per propagate.\n")+
std::string("    - direction: Either +1 or -1; if +1, calculate the matrix\n")+
std::string("      for a forwards-going particle. If -1 calculate the matrix\n")+
std::string("      for a backwards-going particle.\n")+
std::string("Returns a 6x6 matrix formatted as a list of lists.\n");

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
    ErrorTracking* glet = py_glet->tracking;
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
    } catch (Exceptions::Exception exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
    }
}

std::string class_docstring_str =
std::string("The GlobalErrorTracking module provides routines to propagate\n")+
std::string("tracks and associated errors through an arbitrary MAUS\n")+
std::string("geometry. A track is represented by a vector of time, position,\n")+
std::string("energy and momentum. Errors are represented by a covariance\n")+
std::string("matrix in time, transverse position, energy and transverse\n")+
std::string("momentum. Routines are provided to propagate tracks through EM\n")+
std::string("fields and materials.\n");

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
        glet->tracking = new ErrorTracking();
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
{"set_charge", (PyCFunction)set_charge,
  METH_VARARGS|METH_KEYWORDS, set_charge_docstring.c_str()},
{"get_charge", (PyCFunction)get_charge,
  METH_VARARGS|METH_KEYWORDS, get_charge_docstring.c_str()},
{"set_max_step_size", (PyCFunction)set_max_step_size,
  METH_VARARGS|METH_KEYWORDS, set_max_step_size_docstring.c_str()},
{"get_max_step_size", (PyCFunction)get_max_step_size,
  METH_VARARGS|METH_KEYWORDS, get_max_step_size_docstring.c_str()},
{"set_min_step_size", (PyCFunction)set_min_step_size,
  METH_VARARGS|METH_KEYWORDS, set_min_step_size_docstring.c_str()},
{"get_min_step_size", (PyCFunction)get_min_step_size,
  METH_VARARGS|METH_KEYWORDS, get_min_step_size_docstring.c_str()},
{"set_energy_loss_model", (PyCFunction)set_energy_loss_model,
  METH_VARARGS|METH_KEYWORDS, set_energy_loss_model_docstring.c_str()},
{"get_energy_loss_model", (PyCFunction)get_energy_loss_model,
  METH_VARARGS|METH_KEYWORDS, get_energy_loss_model_docstring.c_str()},
{"set_scattering_model", (PyCFunction)set_scattering_model,
  METH_VARARGS|METH_KEYWORDS, set_scattering_model_docstring.c_str()},
{"get_scattering_model", (PyCFunction)get_scattering_model,
  METH_VARARGS|METH_KEYWORDS, get_scattering_model_docstring.c_str()},
{"set_tracking_model", (PyCFunction)set_tracking_model,
  METH_VARARGS|METH_KEYWORDS, set_tracking_model_docstring.c_str()},
{"get_tracking_model", (PyCFunction)get_tracking_model,
  METH_VARARGS|METH_KEYWORDS, get_tracking_model_docstring.c_str()},
{"get_geometry_model", (PyCFunction)get_geometry_model,
  METH_VARARGS|METH_KEYWORDS, get_geometry_model_docstring.c_str()},
{"set_geometry_model", (PyCFunction)set_geometry_model,
  METH_VARARGS|METH_KEYWORDS, set_geometry_model_docstring.c_str()},
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

std::string module_docstring =
std::string("The global_error_tracking module provides routines for\n")+
std::string("propagating tracks and errors through an arbitrary MAUS geometry.\n");

PyMODINIT_FUNC initglobal_error_tracking(void) {
    if (PyType_Ready(&PyGlobalErrorTrackingType) < 0)
        return;

    PyObject* module = Py_InitModule3("global_error_tracking",
                                      _keywdarg_methods,
                                      module_docstring.c_str());
    if (module == NULL) return;

    PyTypeObject* tracking_type = &PyGlobalErrorTrackingType;
    Py_INCREF(tracking_type);
    PyModule_AddObject(module,
                       "GlobalErrorTracking",
                       reinterpret_cast<PyObject*>(tracking_type));
}
}  // namespace PyGlobalErrorTracking
}  // namespace MAUS

