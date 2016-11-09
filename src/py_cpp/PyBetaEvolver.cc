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

#include <string>

#include "src/common_cpp/FieldTools/BetaEvolver.hh"

#include "src/py_cpp/PyBetaEvolver.hh"

namespace MAUS {
namespace PyBetaEvolver {
std::string ScaleFields_DocString = "";
std::string EvolveBeta_DocString = "";

static PyMethodDef methods[] = {
{"scale_step_4_fields", (PyCFunction)ScaleStep4Fields,
    METH_KEYWORDS, ScaleFields_DocString.c_str()},
{"check_step_4_names", (PyCFunction)CheckStep4Names,
    METH_KEYWORDS, ScaleFields_DocString.c_str()},
{"evolve_beta", (PyCFunction)EvolveBeta,
    METH_KEYWORDS, EvolveBeta_DocString.c_str()},
{NULL, NULL, 0, NULL}
};

PyObject* ScaleStep4Fields(PyObject *dummy, PyObject *args, PyObject *kwds) {
  static char *kwlist[] = {
    const_cast<char*>("EndCoil2_US"),
    const_cast<char*>("CenterCoil_US"),
    const_cast<char*>("EndCoil1_US"),
    const_cast<char*>("MatchCoil2_US"),
    const_cast<char*>("MatchCoil1_US"),
    const_cast<char*>("FocusCoil_US"),
    const_cast<char*>("FocusCoil_DS"),
    const_cast<char*>("MatchCoil1_DS"),
    const_cast<char*>("MatchCoil2_DS"),
    const_cast<char*>("EndCoil1_DS"),
    const_cast<char*>("CenterCoil_DS"),
    const_cast<char*>("EndCoil2_DS"),
    NULL
  };

  double e2_us, cc_us, e1_us, m2_us, m1_us, fc_us, fc_ds, m1_ds, m2_ds, e1_ds, cc_ds, e2_ds;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddddddddddd", kwlist,
      &e2_us, &cc_us, &e1_us, &m2_us, &m1_us, &fc_us, &fc_ds, &m1_ds, &m2_ds, &e1_ds, &cc_ds, &e2_ds)) {
      return NULL;
  }
  try {
    BetaEvolver::rescale_step_4_lattice(e2_us, cc_us, e1_us, m2_us, m1_us, fc_us,
                                        fc_ds, m1_ds, m2_ds, e1_ds, cc_ds, e2_ds);
  } catch (std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* CheckStep4Names(PyObject *dummy, PyObject *args, PyObject *kwds) {
  try {
    BetaEvolver::check_step_4_names();
  } catch (std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
  
}

PyObject* EvolveBeta(PyObject *dummy, PyObject *args, PyObject *kwds) {
  double target_z, z_in, beta_in, alpha_in, p_in, step_size;
  static char *kwlist[] = {
    const_cast<char*>("z_in"),
    const_cast<char*>("beta_in"),
    const_cast<char*>("alpha_in"),
    const_cast<char*>("p_in"),
    const_cast<char*>("step_size"),
    const_cast<char*>("target_z"),
    NULL
  };
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddddd", kwlist,
      &z_in, &beta_in, &alpha_in, &p_in, &step_size, &target_z)) {
      return NULL;
  }
  std::vector<double> b_bp;
  try {
    b_bp = BetaEvolver::integrate(target_z, z_in, beta_in, alpha_in, p_in, step_size);
  } catch (std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  PyObject* return_tuple = Py_BuildValue("ddd", b_bp[0], b_bp[1], b_bp[2]);
  Py_INCREF(return_tuple);
  return return_tuple;
}


PyMODINIT_FUNC initbeta_evolver(void) {
  Py_Initialize();
  PyObject* maus_module = Py_InitModule("beta_evolver", methods);
  if (maus_module == NULL) return;
}
}  // namespace PyBetaEvolver
}  // namespace MAUS

