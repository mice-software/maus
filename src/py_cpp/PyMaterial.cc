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

#include <string>

#include "src/common_cpp/Simulation/GeometryNavigator.hh"

#include "src/common_cpp/Utils/Globals.hh"

#include "src/py_cpp/PyMaterial.hh"

namespace MAUS {
  namespace PyField {

    std::string GetMaterialData_DocString =
      std::string("get_material_data()\n\n")+
      std::string("Get a python dict containing data on the material found at the")+
      std::string("current position. The dict has the entries: \"name\", \"A\", \"Z\", ")+
      std::string("\"radlen\", \"interlen\" and \"density\". Note that if the material ")+
      std::string("is a mixture of elements, Z and A are not properly defined, hence t")+
      std::string("hey are set to 0.0 by default.");
    PyObject* GetMaterialData(PyObject *dummy, PyObject *args) {
      std::string name;
      double A;
      double Z;
      double radlen;
      double interlen;
      double density;

      if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,
               "Unexpected get_material_data arguments");
        return NULL;
      }
      try {

        GeometryNavigator* nav = Globals::GetInstance()->GetMCGeometryNavigator();
        name = nav->GetMaterialName();
        if (nav->IsMixture()) {
          A = 0.0;
          Z = 0.0;
        } else {
          A = nav->GetA();
          Z = nav->GetZ();
        }
        radlen = nav->GetRadiationLength();
        interlen = nav->GetNuclearInteractionLength();
        density = nav->GetDensity();
      } catch (std::exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
      }
      PyObject* py_dict = Py_BuildValue("{s:s,s:d,s:d,s:d,s:d,s:d}", "name", name.c_str(), "A", A,
                              "Z", Z, "radlen", radlen, "interlen", interlen, "density", density );
//      PyObject* py_dict = Py_BuildValue("{s:s,s:d,s:d,s:d}", "name", name.c_str(),
//          "radlen", radlen, "interlen", interlen, "density", density );
      return py_dict;
    }


    std::string SetPosition_DocString =
      std::string("set_position(x, y, z)\n\n")+
      std::string("Set the current position of the geometry navigator.");
    PyObject* SetPosition(PyObject *dummy, PyObject *args) {
      double point[] = {0.0, 0.0, 0.0};

      if (!PyArg_ParseTuple(args, "ddd", &point[0], &point[1], &point[2])) {
        PyErr_SetString(PyExc_TypeError,
               "Failed to process arguments of \"set_position\" as x,y,z");
        return NULL;
      }
      try {

        ThreeVector pos(point[0], point[1], point[2]);
        GeometryNavigator* nav = Globals::GetInstance()->GetMCGeometryNavigator();
        nav->SetPoint(pos);
      } catch (std::exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
      }
      PyObject* none = Py_BuildValue("");
      return none;
    }


    std::string GetPosition_DocString =
      std::string("get_position()\n\n")+
      std::string("Returns the current position of the geometry navigator.");
    PyObject* GetPosition(PyObject *dummy, PyObject *args) {
      double point[] = {0., 0., 0.};

      if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError, "Unexpected arguments for \"get_position\"");
        return NULL;
      }
      try {
        GeometryNavigator* nav = Globals::GetInstance()->GetMCGeometryNavigator();
        ThreeVector pos = nav->GetCurrentPoint();
        point[0] = pos.x();
        point[1] = pos.y();
        point[2] = pos.z();
      } catch (std::exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
      }
      PyObject* position = Py_BuildValue("ddd", point[0], point[1], point[2]);
      return position;
    }


    std::string Step_DocString =
      std::string("step(x, y, z)\n\n")+
      std::string("Move the current position of the navigator by the supplied ")+
      std::string("displacement.");
    PyObject* Step(PyObject *dummy, PyObject *args) {
      double point[] = {0.0, 0.0, 0.0};

      if (!PyArg_ParseTuple(args, "ddd", &point[0], &point[1], &point[2])) {
        PyErr_SetString(PyExc_TypeError,
               "Failed to process arguments of \"Step\" as x,y,z");
        return NULL;
      }
      try {
        ThreeVector disp(point[0], point[1], point[2]);
        GeometryNavigator* nav = Globals::GetInstance()->GetMCGeometryNavigator();
        nav->Step(disp);
      } catch (std::exception& exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
      }
      PyObject* none = Py_BuildValue("");
      return none;
    }


    static PyMethodDef methods[] = {
    {"get_material_data", (PyCFunction)GetMaterialData,
                             METH_VARARGS, GetMaterialData_DocString.c_str()},
    {"set_position", (PyCFunction)SetPosition,
                             METH_VARARGS, SetPosition_DocString.c_str()},
    {"get_position", (PyCFunction)GetPosition,
                             METH_VARARGS, GetPosition_DocString.c_str()},
    {"step", (PyCFunction)Step,
                             METH_VARARGS, Step_DocString.c_str()},
    {NULL, NULL, 0, NULL}
    };


    PyMODINIT_FUNC initmaterial(void) {
      Py_Initialize();
      PyObject* maus_module = Py_InitModule("material", methods);
      if (maus_module == NULL) return;
    }
  }
}

