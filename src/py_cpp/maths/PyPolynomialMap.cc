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

#include <string>

#include "src/common_cpp/Maths/Matrix.hh"
#include "src/common_cpp/Maths/PolynomialMap.hh"

#include "src/py_cpp/maths/PyPolynomialMap.hh"

namespace MAUS {
namespace PyPolynomialMap {

std::string get_coefficients_as_matrix_docstring =
std::string("Return the coefficients of the matrix. Takes no arguments\n\n")+
std::string("Return value is a list of lists, with polynomial coefficients\n")+
std::string("for y_i = a_{ij}*x_i^j forming the i, j term in the matrix.\n");

PyObject* get_coefficients_as_matrix(PyObject *self, PyObject *args, PyObject *kwds) {
    PyPolynomialMap* py_map = reinterpret_cast<PyPolynomialMap*>(self);
    // failed to cast or self was not initialised - something horrible happened
    if (py_map == NULL) {
        PyErr_SetString(PyExc_TypeError,
                "Failed to resolve self as PolynomialMap");
        return NULL;
    }
    if (py_map->map == NULL) {
        PyErr_SetString(PyExc_TypeError,
                "PolynomialMap not properly initialised");
        return NULL;
    }
    Matrix<double> coefficients = py_map->map->GetCoefficientsAsMatrix();
    PyObject* py_coefficients = PyList_New(coefficients.number_of_rows());
    // not safe from out of memory errors (etc)
    for (size_t i = 0; i < coefficients.number_of_rows(); ++i) {
        PyObject* py_row = PyList_New(coefficients.number_of_columns());
        for (size_t j = 0; j < coefficients.number_of_columns(); ++j) {
            PyObject* py_value = PyFloat_FromDouble(coefficients(i+1, j+1));
            Py_INCREF(py_value);
            PyList_SetItem(py_row, j, py_value);
        }
        PyList_SetItem(py_coefficients, i, py_row);
        Py_INCREF(py_row);
    }
    Py_INCREF(py_coefficients);
    return py_coefficients;
}

std::string evaluate_docstring =
std::string("Apply the mapping to a point.\n\n")+
std::string("  - point: list of floats with length equal to the point\n")+
std::string("    dimension of the mapping, corresponding to the abscissa.\n")+
std::string("Return value is a list of floats, with length equal to the\n")+
std::string("value dimension of the mapping; corresponding to the ordinates\n");

PyObject* evaluate(PyObject *self, PyObject *args, PyObject *kwds) {
    PyPolynomialMap* py_map = reinterpret_cast<PyPolynomialMap*>(self);
    // failed to cast or self was not initialised - something horrible happened
    if (py_map == NULL) {
        PyErr_SetString(PyExc_TypeError,
                "Failed to resolve self as PolynomialMap");
        return NULL;
    }
    if (py_map->map == NULL) {
        PyErr_SetString(PyExc_TypeError,
                "PolynomialMap not properly initialised");
        return NULL;
    }
    PyObject* py_point;
    static char *kwlist[] = {const_cast<char*>("point"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &py_point)) {
        return NULL;
    }

    if (!PyList_Check(py_point)) {
        PyErr_SetString(PyExc_TypeError, "point was not a list");
        return NULL;
    }
    if (PyList_Size(py_point) != py_map->map->PointDimension()) {
        PyErr_SetString(PyExc_TypeError, "point had wrong size");
        return NULL;
    }
    std::vector<double> point(py_map->map->PointDimension());
    for (size_t i = 0; i < point.size(); ++i) {
        PyObject* point_i = PyList_GetItem(py_point, i);
        point[i] = PyFloat_AsDouble(point_i);
    }
    if (PyErr_Occurred()) // probably not a double in the list
        return NULL;
    std::vector<double> value(py_map->map->ValueDimension());
    py_map->map->F(&point[0], &value[0]);
    PyObject* py_value = PyList_New(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        PyObject* value_i = PyFloat_FromDouble(value[i]);
        PyList_SetItem(py_value, i, value_i);
        Py_INCREF(value_i);
    }
    Py_INCREF(py_value);
    return py_value;
}

std::string least_squares_fit_docstring =
std::string("Find a mapping by performing a least squares fit.\n\n")+
std::string("  - points: list, each entry containing a list of floats\n")+
std::string("  corresponding to abscissa, with length ValueDimension\n")+
std::string("  - values: list, each entry containing a list of floats\n")+
std::string("  corresponding to ordinates, with length PointDimension\n")+
std::string("  - polynomial_order: integer, >= 0, corresponding to the\n")+
std::string("  polynomial fit.\n")+
std::string("Returns a polynomial map.\n");

std::vector<std::vector<double> > get_vectors(PyObject* py_floats) {
    // convert from list of list of floats to std::vector< std::vector<double> >
    // first check validity of coefficients
     std::vector< std::vector<double> > data;
    if (!PyList_Check(py_floats)) {
        return std::vector<std::vector<double> >();
    }
    size_t num_rows = PyList_Size(py_floats);
    if (num_rows == 0) {
        return std::vector<std::vector<double> >();
    }
    size_t num_cols = 0;
    // now loop over the rows
    for (size_t i = 0; i < num_rows; ++i) {
        PyObject* row = PyList_GetItem(py_floats, i);
        if (!PyList_Check(row)) {
            return std::vector<std::vector<double> >();
        }
        // do some initialisation in the first row
        if (i == 0) {
            num_cols = PyList_Size(row);
            if (num_cols == 0) {
                return std::vector<std::vector<double> >();
            }
            data = std::vector< std::vector<double> >(num_rows, std::vector<double>(num_cols));
        }
        // now loop over columns
        if (PyList_Size(row) != int(num_cols)) {
            return std::vector<std::vector<double> >();
        }
        for (size_t j = 0; j < num_cols; ++j) {
            PyObject* py_value = PyList_GetItem(row, j);
            data.at(i).at(j) = PyFloat_AsDouble(py_value);
            if (PyErr_Occurred() != NULL) // not a float
                return std::vector<std::vector<double> >();
        }
    }
    return data;
}

PyObject* least_squares_fit(PyObject *py_class, PyObject *args, PyObject *kwds) {
    PyTypeObject* py_map_type = reinterpret_cast<PyTypeObject*>(py_class);
    // failed to cast or self was not initialised - something horrible happened
    if (py_map_type == NULL) {
        PyErr_SetString(PyExc_TypeError,
                "Failed to resolve self as PolynomialMapType");
        return NULL;
    }

    PyObject* py_points = NULL;
    PyObject* py_values = NULL;
    int polynomial_order = 0;
    static char *kwlist[] = {
        const_cast<char*>("points"),
        const_cast<char*>("values"),
        const_cast<char*>("polynomial_order"),
        NULL
    };
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOi", kwlist, &py_points,
            &py_values, &polynomial_order)) {
        return NULL;
    }
    if (polynomial_order < 0) {
        PyErr_SetString(PyExc_ValueError, "polynomial_order should be >= 0");
        return NULL;
    }
    std::vector<std::vector<double> > points = get_vectors(py_points);
    if (points.size() == 0) {
        PyErr_SetString(PyExc_ValueError, "Failed to evaluate points");
        return NULL;
    }
    std::vector<std::vector<double> > values = get_vectors(py_values);
    if (values.size() == 0) {
        PyErr_SetString(PyExc_ValueError, "Failed to evaluate values");
        return NULL;
    }
    if (points.size() != values.size()) {
        PyErr_SetString(PyExc_ValueError, "points misaligned with values");
        return NULL;
    }
    for (size_t i = 0; i < points.size(); ++i)
        if (points[i].size() != values[i].size()) {
            PyErr_SetString(PyExc_ValueError,
                            "points row misaligned with values row");
            return NULL;
        }
    PolynomialMap* test_map = NULL;
    try {
        test_map = PolynomialMap::PolynomialLeastSquaresFit(points, values, polynomial_order);
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_ValueError, exc.what());
        return NULL;
    }
    PyObject* py_map_obj = _alloc(py_map_type, 0);
    PyPolynomialMap* py_map = reinterpret_cast<PyPolynomialMap*>(py_map_obj);
    py_map->map = test_map;
    return py_map_obj;
}


int _init(PyObject* self, PyObject *args, PyObject *kwds) {
    PyPolynomialMap* py_map = reinterpret_cast<PyPolynomialMap*>(self);
    // failed to cast or self was not initialised - something horrible happened
    if (py_map == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve self as PolynomialMap in __init__");
        return -1;
    }
    // legal python to call initialised_object.__init__() to reinitialise, so
    // handle this case
    if (py_map->map != NULL) {
        delete py_map->map;
        py_map->map = NULL;
    }
    // read in arguments
    int point_dim;
    PyObject* py_coefficients;
    static char *kwlist[] = {const_cast<char*>("point_dimension"),
                             const_cast<char*>("coefficients"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iO", kwlist,
                                         &point_dim, &py_coefficients)) {
        return -1;
    }

    // convert from list of list of floats to Matrix<double>
    // first check validity of coefficients
    if (!PyList_Check(py_coefficients)) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve coefficients as a list");
        return -1;
    }
    size_t num_rows = PyList_Size(py_coefficients);
    if (num_rows == 0) {
        PyErr_SetString(PyExc_ValueError, "coefficients was empty");
        return -1;
    }
    size_t num_cols = 0;
    std::vector<double> data;
    // now loop over the rows
    for (size_t i = 0; i < num_rows; ++i) {
        PyObject* row = PyList_GetItem(py_coefficients, i);
        if (!PyList_Check(py_coefficients)) {
            PyErr_SetString(PyExc_TypeError,
                            "Failed to resolve coefficients row as a list");
            return -1;
        }
        // do some initialisation in the first row
        if (i == 0) {
            num_cols = PyList_Size(row);
            if (num_cols == 0) {
                PyErr_SetString(PyExc_ValueError, "coefficients row was empty");
                return -1;
            }
            data = std::vector<double>(num_rows*num_cols);
        }
        // now loop over columns
        if (PyList_Size(row) != int(num_cols)) {
                PyErr_SetString(PyExc_ValueError,
                                "coefficients row had wrong number of elements");
        }
        for (size_t j = 0; j < num_cols; ++j) {
            PyObject* py_value = PyList_GetItem(row, j);
            data.at(i*num_cols+j) = PyFloat_AsDouble(py_value);
            if (PyErr_Occurred() != NULL) // not a float
                return -1;
        }
    }
    Matrix<double> coefficients(num_rows, num_cols, &data[0]);

    // now initialise the internal map
    try {
        py_map->map = new MAUS::PolynomialMap(point_dim, coefficients);
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return -1;
    }
    return 0;
}

PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems) {
    void* void_map = malloc(sizeof(PyPolynomialMap));
    PyPolynomialMap* map = reinterpret_cast<PyPolynomialMap*>(void_map);
    map->map = NULL;
    map->ob_refcnt = 1;
    map->ob_type = type;
    return reinterpret_cast<PyObject*>(map);
}

PyObject *_new(PyTypeObject *type, Py_ssize_t nitems) {
    return _alloc(type, nitems);
}

void _dealloc(PyPolynomialMap * self) {
    _free(self);
}

void _free(PyPolynomialMap * self) {
    if (self != NULL) {
        if (self->map != NULL)
            delete self->map;
        free(self);
    }
}

static PyMemberDef _members[] = {
{NULL}
};
static PyMethodDef _methods[] = {
{"get_coefficients_as_matrix", (PyCFunction)get_coefficients_as_matrix,
  METH_VARARGS|METH_KEYWORDS, get_coefficients_as_matrix_docstring.c_str()},
{"evaluate", (PyCFunction)evaluate,
  METH_VARARGS|METH_KEYWORDS, evaluate_docstring.c_str()},
{"least_squares_fit", (PyCFunction)least_squares_fit,
  METH_CLASS|METH_VARARGS|METH_KEYWORDS, least_squares_fit_docstring.c_str()},
{NULL}
};

std::string class_docstring =
std::string("PolynomialMap provides routines to calculate multivariate \n")+
std::string("polynomials.\n\n")+
std::string("__init__()\n")+
std::string("    Takes two arguments.\n")+
std::string("    - point_dim: integer which defines the dimension of the\n")+
std::string("      points (abscissa)\n")+
std::string("    - coefficients: list of lists of floats which define the\n")+
std::string("      polynomial\n")+
std::string("The value dimension of the PolynomialMap is the number of rows\n")+
std::string("coefficients matrix\n");

static PyTypeObject PyPolynomialMapType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "polynomial_map.PolynomialMap",         /*tp_name*/
    sizeof(PyPolynomialMap),           /*tp_basicsize*/
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

const char* module_docstring =
                       "polynomial_map module contains the PolynomialMap class";


PyMODINIT_FUNC initpolynomial_map(void) {
    PyPolynomialMapType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyPolynomialMapType) < 0)
        return;

    PyObject* module = Py_InitModule3("polynomial_map", NULL, module_docstring);
    if (module == NULL)
        return;

    PyTypeObject* polynomial_map_type = &PyPolynomialMapType;
    Py_INCREF(polynomial_map_type);
    PyModule_AddObject(module, "PolynomialMap",
                       reinterpret_cast<PyObject*>(polynomial_map_type));
}


}  // namespace PyPolynomialMap
}  // namespace MAUS

