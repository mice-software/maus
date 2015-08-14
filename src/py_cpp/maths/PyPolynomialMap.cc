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

std::string set_coefficients_docstring =
std::string("");

// in the end this is not in the public interface; because Matrix<double> does
// not support assignment very well.
PyObject* set_coefficients(PyObject *self, PyObject *args, PyObject *kwds) {
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
    int point_dim;
    PyObject* py_coefficients;
    static char *kwlist[] = {const_cast<char*>("point_dimension"),
                             const_cast<char*>("coefficients"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iO", kwlist,
                                         &point_dim, &py_coefficients)) {
        return NULL;
    }
    if (!PyList_Check(py_coefficients)) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve coefficients as a list");
        return NULL;
    }
    size_t num_rows = PyList_Size(py_coefficients);
    if (num_rows == 0) {
        PyErr_SetString(PyExc_ValueError, "coefficients was empty");
        return NULL;
    }
    size_t num_cols = 0;
    std::vector<double> data;
    for (size_t i = 0; i < num_rows; ++i) {
        PyObject* row = PyList_GetItem(py_coefficients, i);
        if (!PyList_Check(py_coefficients)) {
            PyErr_SetString(PyExc_TypeError,
                            "Failed to resolve coefficients row as a list");
            return NULL;
        }
        if (i == 0) {
            num_cols = PyList_Size(row);
            if (num_cols == 0) {
                PyErr_SetString(PyExc_ValueError, "coefficients row was empty");
            }
            data = std::vector<double>(num_rows*num_cols);
        }
        if (PyList_Size(row) != int(num_cols)) {
                PyErr_SetString(PyExc_ValueError,
                                "coefficients row had wrong number of elements");
        }
        for (size_t j = 0; j < num_cols; ++j) {
            PyObject* py_value = PyList_GetItem(row, j);
            data.at(i*num_cols+j) = PyFloat_AsDouble(py_value);
            if (PyErr_Occurred() != NULL) // not a float
                return NULL;
        }
    }
    std::cerr << "!" << std::endl;
    Matrix<double> coefficients(num_rows, num_cols, &data[0]);
    std::cerr << "&" << std::endl;
    py_map->map->SetCoefficients(point_dim, coefficients);
    std::cerr << "#" << std::endl;
    Py_INCREF(Py_None);
    return Py_None; 
}

std::string get_coefficients_as_matrix_docstring =
std::string("");

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
    PyObject* py_coefficients = PyList_New(coefficients.number_of_columns());
    // not safe from out of memory errors (etc)
    for (size_t i = 0; i < coefficients.number_of_columns(); ++i) {
        PyObject* py_row = PyList_New(coefficients.number_of_rows());
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
std::string("");

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
        PyFloat_AsDouble(point_i, point[i]);
    }
    if (PyErr_Occurred()) // probably not a double in the list
        return NULL;
    std::vector<double> value(py_map->map->ValueDimension());
    py_map->map->F(&point[0], &value[0]);
    PyObject* py_value = PyList_New(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        PyObject* value_i = PyFloat_FromDouble(value[i]);
        PyList_SetItem(py_value, value_i, i);
        Py_INCREF(value_i);
    }
    Py_INCREF(py_value);
    return py_value;
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
    // now initialise the internal map
    try {
        Matrix<double> matrix(1, 1, 0.);
        py_map->map = new MAUS::PolynomialMap(1, matrix);
        set_coefficients(self, args, kwds);
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
std::string("      polynomial");

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

