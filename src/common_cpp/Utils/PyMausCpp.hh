#ifndef PYMAUSCPP
#define PYMAUSCPP

#include "Python.h"

PyMODINIT_FUNC initlibMausCpp(void);

static PyObject* CppErrorHandler_SetHandleExceptionFunction
                                              (PyObject *dummy, PyObject *args);

#endif

