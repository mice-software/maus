#include "src/common_cpp/Utils/PyMausCpp.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

static PyMethodDef MausCpp_methods[] = {
{"SetHandleException", (PyCFunction)CppErrorHandler_SetHandleExceptionFunction,
    METH_VARARGS, "Set the python function that is called to handle exceptions"},
{NULL, NULL, 0, NULL}
};

PyObject* CppErrorHandler_SetHandleExceptionFunction(PyObject *dummy, PyObject *args) {
  PyObject* temp = NULL;
  if (PyArg_ParseTuple(args, "O:HandleExceptionFunction", &temp)) {
    if (!PyCallable_Check(temp)) {
        PyErr_SetString(PyExc_TypeError, "Attempt to set HandleExceptionFunction to non-callable PyObject");
        return NULL;
    }
    Py_XINCREF(temp);
    CppErrorHandler::SetPyErrorHandler(temp);
  }
  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC initlibMausCpp(void) {
  Py_Initialize();
  PyObject* m = Py_InitModule("libMausCpp", MausCpp_methods);
  if (m == NULL) return;
}

