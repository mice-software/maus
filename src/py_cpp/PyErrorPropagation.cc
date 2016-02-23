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

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Recon/Global/Tracking.hh"
#include "src/py_cpp/PyErrorPropagation.hh"

namespace MAUS {
namespace PyErrorPropagation {

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

std::string propagate_errors_docstring = "DOCSTRING";

static PyObject* propagate_errors
                              (PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("centroid"),
                             const_cast<char*>("ellipse"),
                             const_cast<char*>("target_z"),
                             const_cast<char*>("step_size"),
                             NULL};
    PyObject* py_centroid = NULL;
    PyObject* py_ellipse = NULL;
    double target_z = 0;
    double step_size = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOdd|", kwlist, &py_centroid, &py_ellipse, &target_z, &step_size)) {
        // error message is set in PyArg_Parse...
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
        TrackingZ tz;
        tz.SetStepSize(step_size);
        tz.SetDeviations(0.001, 0.001, 0.001, 0.001);
        tz.SetEnergyLossModel(TrackingZ::no_eloss);
        tz.SetMCSModel(TrackingZ::no_mcs);
        tz.SetEStragModel(TrackingZ::no_estrag);
        tz.SetField(Globals::GetInstance()->GetMCFieldConstructor());
        tz.Propagate(&x_in[0], target_z);
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
        NULL};
    PyObject* py_centroid = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &py_centroid)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    try {
        std::vector<double> x_in(8, 0.);
        if (get_centroid(py_centroid, x_in)) {
            return NULL;
        }
        TrackingZ tz;
        tz.SetStepSize(10.);
        tz.SetDeviations(0.001, 0.001, 0.001, 0.001);
        tz.SetField(Globals::GetInstance()->GetMCFieldConstructor());
        tz.UpdateTransferMatrix(&x_in[0]);
        std::vector<std::vector<double> > matrix = tz.GetMatrix();
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

static PyMethodDef _keywdarg_methods[] = {
    {"propagate_errors", (PyCFunction)propagate_errors,
    METH_VARARGS|METH_KEYWORDS, propagate_errors_docstring.c_str()},
    {"get_transfer_matrix", (PyCFunction)get_transfer_matrix,
    METH_VARARGS|METH_KEYWORDS, get_transfer_matrix_docstring.c_str()},
    {NULL,  NULL}   /* sentinel */
};

std::string module_docstring = "DOCSTRING";

PyMODINIT_FUNC initerror_propagation(void) {
    PyObject* module = Py_InitModule3("error_propagation", _keywdarg_methods,
                                                     module_docstring.c_str());
    if (module == NULL) return;
}
}  // namespace PyErrorPropagation
}  // namespace MAUS

