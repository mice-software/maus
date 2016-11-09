
#ifndef _SRC_PY_CPP_PYBETAEVOLVER_HH_
#define _SRC_PY_CPP_PYBETAEVOLVER_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

namespace MAUS {

namespace PyBetaEvolver {

PyObject* CheckStep4Names(PyObject *dummy, PyObject *args, PyObject *kwds);
PyObject* ScaleStep4Fields(PyObject *dummy, PyObject *args, PyObject *kwds);
PyObject* EvolveBeta(PyObject *dummy, PyObject *args, PyObject *kwds);
}  // namespace PyBetaEvolver
}  // namespace MAUS

#endif  // _SRC_PY_CPP_PYBETAEVOLVER_HH_
