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
#include <ctype.h>

#include <sstream>

#include "src/legacy/Config/MiceModule.hh"

#include "src/common_cpp/Utils/Exception.hh"

#define MAUS_PYMICEMODULE_CC
#include "src/py_cpp/PyMiceModule.hh"
#undef MAUS_PYMICEMODULE_CC

namespace MAUS {
namespace PyMiceModule {

MiceModule* C_API::get_mice_module(PyObject* self) {
    PyMiceModule* py_mod = reinterpret_cast<PyMiceModule*>(self);
    if (py_mod == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve self as PyMiceModule");
        return NULL;
    }
    return py_mod->mod;   
}

std::string get_property_docstring =
std::string("Returns the value of a particular MiceModule property\n\n")+
std::string("  - name (string) name of the property\n")+
std::string("  - type (string) type of the property\n")+
std::string("Returns a value of the appropriate type");

PyObject* get_bool(MiceModule* mod, std::string name) {
    try {
        bool out = mod->propertyBoolThis(name);
        PyObject* py_out = Py_BuildValue("b", int(out));
        Py_INCREF(py_out);
        return py_out;
    } catch(Exception exc) {
        PyErr_SetString(PyExc_KeyError, exc.what());
        return NULL;
    } 
}

PyObject* get_int(MiceModule* mod, std::string name) {
    try {
        int out = mod->propertyIntThis(name);
        PyObject* py_out = Py_BuildValue("i", out);
        Py_INCREF(py_out);
        return py_out;
    } catch(Exception exc) {
        PyErr_SetString(PyExc_KeyError, exc.what());
        return NULL;
    } 
}

PyObject* get_double(MiceModule* mod, std::string name) {
    try {
        double out = mod->propertyDoubleThis(name);
        PyObject* py_out = Py_BuildValue("d", out);
        Py_INCREF(py_out);
        return py_out;
    } catch(Exception exc) {
        PyErr_SetString(PyExc_KeyError, exc.what());
        return NULL;
    } 
}

PyObject* get_string(MiceModule* mod, std::string name) {
    try {
        std::string out = mod->propertyStringThis(name);
        PyObject* py_out = Py_BuildValue("s", out.c_str());
        Py_INCREF(py_out);
        return py_out;
    } catch(Exception exc) {
        PyErr_SetString(PyExc_KeyError, exc.what());
        return NULL;
    } 
}

PyObject* get_hep3vector(MiceModule* mod, std::string name) {
    try {
        CLHEP::Hep3Vector out = mod->propertyHep3VectorThis(name);
        PyObject* py_out = PyDict_New();
        Py_INCREF(py_out);
        PyObject* x = Py_BuildValue("d", out[0]);
        Py_INCREF(x);
        PyObject* y = Py_BuildValue("d", out[1]);
        Py_INCREF(y);
        PyObject* z = Py_BuildValue("d", out[2]);
        Py_INCREF(z);
        PyDict_SetItemString(py_out, "x", x);
        PyDict_SetItemString(py_out, "y", y);
        PyDict_SetItemString(py_out, "z", z);
        return py_out;
    } catch(Exception exc) {
        PyErr_SetString(PyExc_KeyError, exc.what());
        return NULL;
    } 
}

PyObject *get_property(PyObject* self, PyObject *args, PyObject *kwds) {
    MiceModule* mod = C_API::get_mice_module(self);
    if (mod == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "MiceModule was not properly initialised");
        return NULL;
    }
    const char* name = NULL;
    const char* c_type = NULL;
    static char *kwlist[] = {(char*)"name", (char*)"type", NULL};
    if (!PyArg_ParseTupleAndKeywords
                                  (args, kwds, "ss|", kwlist, &name, &c_type)) {
        return NULL;
    }
    // convert type to lower case
    std::string type(c_type);
    for (size_t i = 0; i < type.size(); ++i) {
        type[i] = std::tolower(type[i]);
    }

    if (type == "bool" || type == "boolean") {
        return get_bool(mod, name);
    } else if (type == "int") {
        return get_int(mod, name);
    } else if (type == "string") {
        return get_string(mod, name);
    } else if (type == "double") {
        return get_double(mod, name);
    } else if (type == "hep3vector") {
        return get_hep3vector(mod, name);
    } else {
        std::stringstream message;
        message << "Did not recognise type '" << type << "' ";
        message << " - should be one of bool, int, string, double, hep3vector";
        PyErr_SetString(PyExc_TypeError, message.str().c_str());
        return NULL;
    }
}

bool set_property_hep3vector_one
                          (PyObject* py_dict, std::string dim, double* value) {
    //  py_value is borrowed ref
    PyObject* py_value = PyDict_GetItemString(py_dict, dim.c_str());
    if (!py_value) {
        PyErr_SetString(PyExc_KeyError,
                      "could not find x, y and z in hep3vector dictionary");
        return false;
    }
    if (!PyArg_Parse(py_value, "d", value)) {
        std::string err = "value['"+dim+"'] could not be converted to a number";
        PyErr_SetString(PyExc_TypeError, err.c_str());
        return false;
    }
    return true;
}

PyObject* set_property_hep3vector
                      (MiceModule* mod, std::string name, PyObject* py_value) {
    CLHEP::Hep3Vector value;
    if (!PyDict_Check(py_value)) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve Hep3Vector as a dict");
        return NULL;
    }
    if (!set_property_hep3vector_one(py_value, "x", &value[0])) return NULL;
    if (!set_property_hep3vector_one(py_value, "y", &value[1])) return NULL;
    if (!set_property_hep3vector_one(py_value, "z", &value[2])) return NULL;
    mod->setProperty(name, value);
    Py_INCREF(Py_None);
    return Py_None;  // all is well
}

std::string set_property_docstring =
std::string("Sets the value of a particular MiceModule property\n\n")+
std::string("  - name (string) name of the property\n")+
std::string("  - type (string) type of the property\n")+
std::string("  - value (type) value to be set - should be convertible to\n")+
std::string("    the appropriate type\n")+
std::string("Returns None");

PyObject *set_property(PyObject* self, PyObject *args, PyObject *kwds) {
    MiceModule* mod = C_API::get_mice_module(self);
    if (mod == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "MiceModule was not properly initialised");
        return NULL;
    }
    const char* name = NULL;
    const char* c_type = NULL;
    PyObject* py_value = NULL;
    static char *kwlist[] =
                          {(char*)"name", (char*)"type", (char*)"value", NULL};
    if (!PyArg_ParseTupleAndKeywords
                      (args, kwds, "ssO|", kwlist, &name, &c_type, &py_value)) {
        return NULL;
    }
    // convert type to lower case
    std::string type(c_type);
    for (size_t i = 0; i < type.size(); ++i) {
        type[i] = std::tolower(type[i]);
    }

    if (type == "bool" || type == "boolean") {
        int value = 0;
        if (PyArg_Parse(py_value, "i", &value))
            mod->setProperty(name, value != 0);
        else
            return NULL;
    } else if (type == "int") {
        int value = 0;
        if (PyArg_Parse(py_value, "i", &value))
            mod->setProperty(name, value);
        else
            return NULL;
    } else if (type == "string") {
        char* value = NULL;
        if (PyArg_Parse(py_value, "s", &value))
            mod->setProperty(name, std::string(value));
        else
            return NULL;
    } else if (type == "double") {
        double value = 0;
        if (PyArg_Parse(py_value, "d", &value))
            mod->setProperty(name, value);
        else
            return NULL;
    } else if (type == "hep3vector") {
        return set_property_hep3vector(mod, name, py_value);
    } else {
        std::stringstream message;
        message << "Did not recognise type '" << type << "' ";
        message << " - should be one of bool, int, string, double, hep3vector";
        PyErr_SetString(PyExc_TypeError, message.str().c_str());
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}


PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems) {
    void* void_mod = malloc(sizeof(PyMiceModule));
    PyMiceModule* mod = reinterpret_cast<PyMiceModule*>(void_mod);
    mod->mod = NULL;
    mod->ob_refcnt = 1;
    mod->ob_type = type;
    return reinterpret_cast<PyObject*>(mod);
}

int _init(PyObject* self, PyObject *args, PyObject *kwds) {
    PyMiceModule* mod = reinterpret_cast<PyMiceModule*>(self);
    // failed to cast or self was not initialised - something horrible happened
    if (mod == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve self as PyMiceModule in __init__");
        return -1;
    }
    // legal python to call initialised_object.__init__() to reinitialise, so
    // handle this case
    if (mod->mod != NULL) {
        delete mod->mod;
    }
    char* file_name;
    static char *kwlist[] = {(char*)"file_name", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &file_name)) {
        return -1;
    }

    try {
        mod->mod = new MiceModule(std::string(file_name));
    } catch (Exception exc) {
        PyErr_SetString(PyExc_ValueError, exc.what());
        return -1;
    }
    return 0;
}

void _free(PyMiceModule * self) {
    if (self != NULL) {
        if (self->mod != NULL)
            free(self->mod);
        free(self);
    }
}

PyObject* _str(PyObject * self) {
    MiceModule* mod = C_API::get_mice_module(self);
    if (mod == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "MiceModule was not properly initialised");
        return NULL;
    }
    std::stringstream ss_in;
    mod->printThis(ss_in);
    return PyString_FromString(ss_in.str().c_str());
}

const char* module_docstring =
  "mice_module for geometry and field definitions including MiceModule class";

std::string class_docstring_str = 
std::string("MiceModule provides bindings for defining geometries.\n\n")+
std::string("For guidance on setting up a MiceModule, see MAUS\n")+
std::string("documentation chapter \"How to Define a Geometry\".\n")+
std::string("\n")+
std::string("__init__(file_name)\n")+
std::string("  Constructor for a new MiceModule.\n")+
std::string("    - file_name (string) name of the file from which to read\n")+
std::string("    the MiceModule file. Either a path relative to the current\n")+
std::string("    directory or relative to the")+
std::string("         ${MICEFILES}/Models/Configurations/\n")+
std::string("    environment variable\n")+
std::string("  Returns a MiceModule object\n");
const char* class_docstring = class_docstring_str.c_str();

static PyMemberDef _members[] = {
{NULL}
};

static PyMethodDef _methods[] = {
{"get_property", (PyCFunction)get_property,
 METH_VARARGS|METH_KEYWORDS, get_property_docstring.c_str()},
{"set_property", (PyCFunction)set_property,
 METH_VARARGS|METH_KEYWORDS, set_property_docstring.c_str()},
//{"set_element", (PyCFunction)set_element,
// METH_VARARGS|METH_KEYWORDS, set_element_docstring.c_str()},
{NULL}
};

static PyTypeObject PyMiceModuleType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "mice_module.MiceModule",         /*tp_name*/
    sizeof(PyMiceModule),           /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)_free, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                      /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                      /*tp_str*/
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
    {NULL,  NULL}   /* sentinel */
};

PyMODINIT_FUNC initmice_module(void) {
    if (PyType_Ready(&PyMiceModuleType) < 0)
        return;

    PyObject* module = Py_InitModule("mice_module", _keywdarg_methods); //, module_docstring);
    if (module == NULL) return;

    PyTypeObject* mm_type = &PyMiceModuleType;
    Py_INCREF(mm_type);
    PyModule_AddObject(module, "MiceModule", reinterpret_cast<PyObject*>(mm_type));

    // C API
    /*
    PyObject* mm_dict = PyModule_GetDict(module);
    PyObject* cem_c_api = PyCObject_FromVoidPtr((void *)C_API::create_empty_matrix, NULL);
    PyObject* gcm_c_api = PyCObject_FromVoidPtr((void *)C_API::get_covariance_matrix, NULL);
    PyObject* scm_c_api = PyCObject_FromVoidPtr((void *)C_API::set_covariance_matrix, NULL);
    PyDict_SetItemString(cov_mat_dict, "C_API_CREATE_EMPTY_MATRIX_1", cem_c_api);
    PyDict_SetItemString(cov_mat_dict, "C_API_GET_COVARIANCE_MATRIX_1", gcm_c_api);
    PyDict_SetItemString(cov_mat_dict, "C_API_SET_COVARIANCE_MATRIX_1", scm_c_api);
    */
}
}
}
