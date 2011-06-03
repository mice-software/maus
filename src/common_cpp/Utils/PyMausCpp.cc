#include "src/common_cpp/Utils/PyMausCpp.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

static PyMethodDef MausCpp_methods[] = {
{"SetHandleException", (PyCFunction)CppErrorHandler_SetHandleExceptionFunction,
    METH_VARARGS, "Set the python function that is called to handle exceptions"},
{NULL, NULL, 0, NULL}
};

PyObject* CppErrorHandler_SetHandleExceptionFunction(PyObject *dummy, PyObject *args) {
  std::cerr << "SetHandleExcveptionFunction" << std::endl;
  PyObject* err_handler = CppErrorHandler::GetPyErrorHandler();
  if (err_handler != NULL) {
    PyErr_SetString(PyExc_TypeError, "Attempt to set HandleExceptionFunction when it was already set previously");
    return NULL;
  }

  if (PyArg_ParseTuple(args, "O:HandleExceptionFunction", &err_handler)) {
    if (!PyCallable_Check(err_handler)) {
        PyErr_SetString(PyExc_TypeError, "Attempt to set HandleExceptionFunction to non-callable PyObject");
        return NULL;
    }
    Py_XINCREF(err_handler);
  }
  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC initlibMausCpp(void)
{
  PyObject* m = Py_InitModule("libMausCpp", MausCpp_methods);
  if (m == NULL) return;
}

