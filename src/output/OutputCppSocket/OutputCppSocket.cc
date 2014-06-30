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

#include "TPython.h"

#include "src/common_cpp/DataStructure/ImageData.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/output/OutputCppSocket/OutputCppSocket.hh"

namespace MAUS {

OutputCppSocket::OutputCppSocket()
    : OutputBase<ImageData*>("OutputCppSocket"), _doc_id(0) {
}

void OutputCppSocket::_birth(const std::string& json_datacards) {
    if (_root_doc_store != NULL) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                              "Call death before rebirthing OutputCppSocket",
                              "MAUS::OutputCppSocket");
    }
    Json::Value cards = JsonWrapper::StringToJson(json_datacards);
    std::string host = cards["output_cpp_socket_host"].asString();
    int port = cards["output_cpp_socket_port"].asInt();
    std::string collection = cards["output_cpp_socket_collection"].asString();
    int maxsize = cards["root_document_store_maxsize"].asInt();
    double timeout = cards["root_document_store_timeout"].asDouble();
    double poll_time = cards["root_document_store_poll_time"].asDouble();
    _get_root_document_store(timeout, poll_time);
    _connect(host, port);
    _create_collection(collection, maxsize);

}

void OutputCppSocket::_death() {
    // _root_doc_store == NULL stops us DECREFing twice
    if (_root_doc_store != NULL) {
        Py_DECREF(_root_doc_store); // python garbage collector can clean up
    }
    _root_doc_store = NULL;
}

bool OutputCppSocket::_save(JobHeaderData* jh) {
    return false;
}

bool OutputCppSocket::_save(JobFooterData* jf) {
    return false;
}

bool OutputCppSocket::_save(RunHeaderData* rh) {
    return false;
}

bool OutputCppSocket::_save(RunFooterData* rf) {
    return false;
}

bool OutputCppSocket::_save(Data* spill) {
    return false;
}


bool OutputCppSocket::_save(ImageData* image) {
    if (image == NULL) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                              "Trying to save NULL pointer",
                              "OutputCppSocket::save(ImageData* image)");
    }
    void* image_void = dynamic_cast<void*>(image);
    // who owns image_void? I think it is python...
    PyObject* py_image = TPython::ObjectProxy_FromVoidPtr(image_void,
                                                          "Image",
                                                          kTRUE);
    if (py_image == NULL) {
        throw MAUS::Exception(MAUS::Exception::recoverable,
                              "Failed to convert Image to PyObject",
                              "OutputCppSocket::save(ImageData* image)");
    }
    return _save_py_object(py_image);
}

bool OutputCppSocket::_save_py_object(PyObject* object) {
    if (_root_doc_store == NULL)
        throw MAUS::Exception(MAUS::Exception::recoverable,
                        "No RootDocumentStore when creating collection",
                        "OutputCppSocket::_connect");
    PyObject* ret = PyObject_CallMethod(_root_doc_store,
                                        const_cast<char*>("put"),
                                        const_cast<char*>("Oi"),
                                        object, _doc_id++);
    if (ret == NULL) {
        PyErr_Print();
        throw MAUS::Exception(MAUS::Exception::recoverable,
                        "Failed to create collection on RootDocumentStore",
                        "OutputCppSocket::_connect");
    }
    Py_INCREF(ret);
    return true;
}

PyObject* OutputCppSocket::_get_root_document_store(double timeout,
                                                    double poll_time) {
    PyObject* rds_module = PyImport_ImportModule(
                          "docstore.root_document_store._root_document_store");
    if (rds_module == NULL) {
        PyErr_SetString(PyExc_ImportError,
                        "Failed to import root document store");
        return NULL;
    }
    Py_INCREF(rds_module);
    PyObject *rds_dict  = PyModule_GetDict(rds_module);
    if (rds_dict == NULL) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to get rds module dictionary");
        return NULL;
    }
    Py_INCREF(rds_dict);
    PyObject* rds_init = PyDict_GetItemString(rds_dict, "_initialise_from_c");
    if (rds_init == NULL || PyCallable_Check(rds_init) == 0) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to get rds module dictionary");
        return NULL;
    }
    PyObject* args = Py_BuildValue("dd", timeout, poll_time);
    if (args == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to parse args");
        return NULL;
    }
    Py_INCREF(args);
    PyObject* rds = PyObject_Call(rds_init, args, NULL);
    if (rds == NULL) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to initialise RootDocumentStore");
        return NULL;
    }
    _root_doc_store = rds;
    Py_INCREF(rds);
    return rds;
}

void OutputCppSocket::_connect(std::string host, int port) {
    PyObject* py_host = Py_BuildValue("s", host.c_str());
    if (py_host == NULL) {
        PyErr_Print();
        throw MAUS::Exception(MAUS::Exception::recoverable,
                        "Failed to parse host as a string",
                        "OutputCppSocket::_connect");
    }
    Py_INCREF(py_host);
    PyObject* py_port = Py_BuildValue("i", port);
    if (py_port == NULL) {
        PyErr_Print();
        throw MAUS::Exception(MAUS::Exception::recoverable,
                        "Failed to parse port as an int",
                        "OutputCppSocket::_connect");
        Py_INCREF(py_port);
    }
    PyObject* params = PyDict_New();
    if (params == NULL) {
        PyErr_Print();
        throw MAUS::Exception(MAUS::Exception::recoverable,
                        "Failed to make dictionary",
                        "OutputCppSocket::_connect");
        Py_INCREF(params);
    }
    PyDict_SetItemString(params, const_cast<char*>("host"), py_host);
    PyDict_SetItemString(params, const_cast<char*>("port"), py_port);
    PyObject* ret = PyObject_CallMethod(_root_doc_store,
                                        const_cast<char*>("connect"),
                                        const_cast<char*>("O"),
                                        params);
    if (ret == NULL) {
        PyErr_Print();
        throw MAUS::Exception(MAUS::Exception::recoverable, 
                        "Failed to connect to RootDocumentStore",
                        "OutputCppSocket::_connect");
    }
    Py_INCREF(ret);
}

void OutputCppSocket::_create_collection(std::string collection, int maxsize) {
    if (_root_doc_store == NULL)
        throw MAUS::Exception(MAUS::Exception::recoverable,
                        "No RootDocumentStore when creating collection",
                        "OutputCppSocket::_connect");
    PyObject* ret = PyObject_CallMethod(_root_doc_store,
                                        const_cast<char*>("create_collection"),
                                        const_cast<char*>("si"),
                                        collection.c_str(),
                                        maxsize);
    if (ret == NULL) {
        PyErr_Print();
        throw MAUS::Exception(MAUS::Exception::recoverable,
                        "Failed to create collection on RootDocumentStore",
                        "OutputCppSocket::_connect");
    }
    Py_INCREF(ret);
}

} // namespace MAUS
