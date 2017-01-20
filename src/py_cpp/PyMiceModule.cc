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
#include <vector>

#include "src/legacy/Config/MiceModule.hh"

#include "src/common_cpp/Utils/Exception.hh"

#define MAUS_PYMICEMODULE_CC
#include "src/py_cpp/PyMiceModule.hh"
#undef MAUS_PYMICEMODULE_CC

namespace MAUS {
namespace PyMiceModule {


std::string get_name_docstring =
std::string("Get the name of this MiceModule\n\n")+
std::string("  Takes no arguments.\n")+
std::string("Returns a python string containing the module name.");

PyObject *get_name(PyObject* self, PyObject *args, PyObject *kwds) {
    MiceModule* mod = C_API::get_mice_module(self);
    PyObject* py_string = PyString_FromString(mod->name().c_str());
    Py_INCREF(py_string);
    return py_string;
}

std::string get_property_docstring =
std::string("Returns the value of a particular MiceModule property\n\n")+
std::string("  - name (string) name of the property\n")+
std::string("  - type (string) type of the property\n")+
std::string("Returns a value of the appropriate type");

PyObject* get_bool(MiceModule* mod, std::string name) {
    try {
        bool out = mod->propertyBoolThis(name);
        int out_int = out ? 1 : 0; //  ooh I feel so dirty
        PyObject* py_out = Py_BuildValue("b", out_int);
        Py_INCREF(py_out);
        return py_out;
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_KeyError, (&exc)->what());
        return NULL;
    }
}

PyObject* get_int(MiceModule* mod, std::string name) {
    try {
        int out = mod->propertyIntThis(name);
        PyObject* py_out = Py_BuildValue("i", out);
        Py_INCREF(py_out);
        return py_out;
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_KeyError, (&exc)->what());
        return NULL;
    }
}

PyObject* get_double(MiceModule* mod, std::string name) {
    try {
        double out = mod->propertyDoubleThis(name);
        PyObject* py_out = Py_BuildValue("d", out);
        Py_INCREF(py_out);
        return py_out;
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_KeyError, (&exc)->what());
        return NULL;
    }
}

PyObject* get_string(MiceModule* mod, std::string name) {
    try {
        std::string out = mod->propertyStringThis(name);
        PyObject* py_out = Py_BuildValue("s", out.c_str());
        Py_INCREF(py_out);
        return py_out;
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_KeyError, (&exc)->what());
        return NULL;
    }
}

PyObject* hep3vector_to_dict(CLHEP::Hep3Vector out) {
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
}

PyObject* get_hep3vector(MiceModule* mod, std::string name) {
    try {
        CLHEP::Hep3Vector out = mod->propertyHep3VectorThis(name);
        return hep3vector_to_dict(out);
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_KeyError, (&exc)->what());
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
    static char *kwlist[] = {const_cast<char*>("name"),
                             const_cast<char*>("type"), NULL};
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

std::string get_global_position_docstring =
std::string("Returns the position of the MiceModule in the global (top\n")+
std::string("level) coordinates\n\n")+
std::string("Takes no arguments. Returns a dict like {'x':x, 'y':y, 'z':z}\n")+
std::string("corresponding to the global position of the module.");

static PyObject *get_global_position(PyObject* self, PyObject *args, PyObject *kwds) {
    MiceModule* mod = C_API::get_mice_module(self);
    if (mod == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "MiceModule was not properly initialised");
        return NULL;
    }
    try {
        CLHEP::Hep3Vector out = mod->globalPosition();
        return hep3vector_to_dict(out);
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_KeyError, (&exc)->what());
        return NULL;
    }
}

std::string get_global_rotation_docstring =
std::string("Returns the rotation of the MiceModule in the global (top\n")+
std::string("level) coordinates\n\n")+
std::string("Takes no arguments. Returns a dict like\n")+
std::string("{'x':x, 'y':y, 'z':z, 'angle':angle} where x,y,z is the axis\n")+
std::string("of rotation and angle is the angle of rotation [radians], in \n")+
std::string("the global (top level) coordinate system.");

static PyObject *get_global_rotation(PyObject* self, PyObject *args, PyObject *kwds) {
    MiceModule* mod = C_API::get_mice_module(self);
    if (mod == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "MiceModule was not properly initialised");
        return NULL;
    }
    try {
        CLHEP::HepRotation rot = mod->globalRotation();
        PyObject* py_rotation = hep3vector_to_dict(rot.getAxis());
        PyObject* angle = Py_BuildValue("d", rot.getDelta());
        Py_INCREF(angle);
        PyDict_SetItemString(py_rotation, "angle", angle);
        return py_rotation;
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_KeyError, (&exc)->what());
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
    static char *kwlist[] = {const_cast<char*>("name"),
                             const_cast<char*>("type"),
                             const_cast<char*>("value"), NULL};
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

std::string get_children_docstring =
std::string("Get child modules of this MiceModule\n\n")+
std::string("  Takes no arguments.\n")+
std::string("Returns a python list containing a deep copy of all child\n")+
std::string("MiceModules.");

PyObject *get_children(PyObject* self, PyObject *args, PyObject *kwds) {
    MiceModule* mod = C_API::get_mice_module(self);
    std::vector<MiceModule*> daughter_list = mod->allDaughters();
    PyObject* py_list = PyList_New(daughter_list.size());
    Py_INCREF(py_list);
    for (size_t i = 0; i < daughter_list.size(); ++i) {
        if (daughter_list[i] == NULL) {
            PyErr_SetString(PyExc_RuntimeError, "Could not find MiceModule");
            return NULL;
        }
        PyObject* py_mod = C_API::create_empty_module();
        Py_INCREF(py_mod);
        MiceModule* new_child = MiceModule::deepCopy(*daughter_list[i], false);
        C_API::set_mice_module(py_mod, new_child);
        PyList_SetItem(py_list, i, py_mod);
    }
    return py_list;
}


bool will_circle(const MiceModule* ancestor, const MiceModule* child) {
    if (ancestor == NULL)
        return false;
    else if (ancestor == child)
        return true;
    else
        return will_circle(ancestor->mother(), child);
}

std::string set_children_docstring =
std::string("Set child modules of this MiceModule\n\n")+
std::string("  - children (list) list of MiceModule objects. The existing\n")+
std::string("  children will be replaced by deep copies of those in the\n")+
std::string("  list. The parent module of the children is updated to this\n")+
std::string("  MiceModule.\n")+
std::string("Returns None.");

PyObject *set_children(PyObject* self, PyObject *args, PyObject *kwds) {
    MiceModule* mod = C_API::get_mice_module(self);
    if (mod == NULL)
        return NULL;

    PyObject* py_children = NULL;
    static char *kwlist[] = {const_cast<char*>("children"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &py_children)) {
        return NULL;
    }
    if (!PyList_Check(py_children)) {
        PyErr_SetString(PyExc_TypeError, "Argument should be a list");
        return NULL;
    }
    std::vector<MiceModule*> children;
    for (int i = 0; i < PyList_Size(py_children); ++i) {
        PyObject* py_child = PyList_GetItem(py_children, i);
        MiceModule* child = C_API::get_mice_module(py_child);
        if (child == NULL) {
            PyErr_SetString(PyExc_TypeError,
                            "List object was not a MiceModule");
            return NULL;  // no memory allocated and module unchanged
        }
        children.push_back(MiceModule::deepCopy(*child, false));
    }
    while (mod->allDaughters().size() > 0) {
        mod->allDaughters()[0]->setMother(NULL);
        delete mod->allDaughters()[0];
        mod->removeDaughter(mod->allDaughters()[0]);
    }
    for (size_t i = 0; i < children.size(); ++i) {
        mod->addDaughter(children[i]);
        children[i]->setMother(mod);
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
    static char *kwlist[] = {const_cast<char*>("file_name"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &file_name)) {
        return -1;
    }

    try {
        mod->mod = new MiceModule(std::string(file_name));
    } catch (std::exception& exc) {
        PyErr_SetString(PyExc_ValueError, (&exc)->what());
        return -1;
    }
    return 0;
}

void _free(PyMiceModule * self) {
    if (self != NULL) {
        if (self->mod != NULL && self->mod->mother() == NULL)
            delete self->mod;
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
std::string("\n")+
std::string("MiceModules are representations of the MAUS geometry and.\n")+
std::string("fields. They are structured as a tree; for now the tree is\n")+
std::string("mono-directional. Each level of the tree holds properties that\n")+
std::string("reflect the geometry objects at that level and child modules\n")+
std::string("that can be used to access lower level geometry objects. \n")+
std::string("\n")+
std::string("See MAUS documentation chapter \"How to Define a Geometry\".\n")+
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

/* Comment on why global position and rotation are hard:
 * Python owns the memory allocated to the module. Because python can delete
 * things without warning, we make a deep copy of the module when it gets taken
 * over by python and cut it out of the module tree (mother points to NULL).
 * So just calling globalPosition() doesn't work because the module doesn't know
 * who its mother is. 
 *
 * I toyed with the idea of making a temporary pointer to the mother, but here
 * we can run into trouble. Say we do (python) 
 * mod_1 = mod_0.get_children()[0]
 * mod_2 = mod_1.get_children()[0]
 * we have no way to guarantee that mod_1 has not been deleted. So 
 * mod_2.get_global_position() would try to get the global position of mod_1;
 * by which point mod_1 has been deleted, and we get a segv. I guess child 
 * module has to hold a INCREF and it would work okay... more work than I want
 * right now. I left the code in.
 */

static PyMethodDef _methods[] = {
{"get_name", (PyCFunction)get_name,
  METH_VARARGS|METH_KEYWORDS, get_name_docstring.c_str()},
{"set_children", (PyCFunction)set_children,
  METH_VARARGS|METH_KEYWORDS, set_children_docstring.c_str()},
{"get_children", (PyCFunction)get_children,
  METH_VARARGS|METH_KEYWORDS, get_children_docstring.c_str()},
{"get_property", (PyCFunction)get_property,
  METH_VARARGS|METH_KEYWORDS, get_property_docstring.c_str()},
{"set_property", (PyCFunction)set_property,
  METH_VARARGS|METH_KEYWORDS, set_property_docstring.c_str()},
// {"get_global_position", (PyCFunction)get_global_position,
//  METH_VARARGS|METH_KEYWORDS, get_global_position_docstring.c_str()},
// {"get_global_rotation", (PyCFunction)get_global_rotation,
//  METH_VARARGS|METH_KEYWORDS, get_global_rotation_docstring.c_str()},
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
    {NULL,  NULL}   /* sentinel */
};

PyMODINIT_FUNC initmice_module(void) {
    if (PyType_Ready(&PyMiceModuleType) < 0)
        return;

    PyObject* module = Py_InitModule3
                           ("mice_module", _keywdarg_methods, module_docstring);
    if (module == NULL) return;

    PyTypeObject* mm_type = &PyMiceModuleType;
    Py_INCREF(mm_type);
    PyModule_AddObject
                   (module, "MiceModule", reinterpret_cast<PyObject*>(mm_type));

    // C API
    PyObject* mod_dict = PyModule_GetDict(module);
    PyObject* cem_c_api = PyCObject_FromVoidPtr(reinterpret_cast<void *>
                                            (C_API::create_empty_module), NULL);
    PyObject* gmm_c_api = PyCObject_FromVoidPtr(reinterpret_cast<void *>
                                                (C_API::get_mice_module), NULL);
    PyObject* smm_c_api = PyCObject_FromVoidPtr(reinterpret_cast<void *>
                                                (C_API::set_mice_module), NULL);
    PyDict_SetItemString(mod_dict, "C_API_CREATE_EMPTY_MODULE", cem_c_api);
    PyDict_SetItemString(mod_dict, "C_API_GET_MICE_MODULE", gmm_c_api);
    PyDict_SetItemString(mod_dict, "C_API_SET_MICE_MODULE", smm_c_api);
}


PyObject* C_API::create_empty_module() {
    return _alloc(&PyMiceModuleType, 0);
}

MiceModule* C_API::get_mice_module(PyObject* self) {
    if (self->ob_type != &PyMiceModuleType) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve object as MiceModule");
        return NULL;
    }
    PyMiceModule* py_mod = reinterpret_cast<PyMiceModule*>(self);
    return py_mod->mod;
}

int C_API::set_mice_module(PyObject* self, MiceModule* mod) {
    if (self->ob_type != &PyMiceModuleType) {
        PyErr_SetString(PyExc_TypeError,
                        "Failed to resolve object as MiceModule");
        return 0;
    }
    PyMiceModule* py_mod = reinterpret_cast<PyMiceModule*>(self);
    if (py_mod->mod != NULL) {
        delete mod;
    }
    // I can't keep memory consistent if I allow access back up the tree.
    mod->setMother(NULL);
    py_mod->mod = mod;
    return 1;
}
}
}
