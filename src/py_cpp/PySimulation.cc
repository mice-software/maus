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

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/py_cpp/PySimulation.hh"

namespace MAUS {
namespace PySimulation {
std::string module_docstring =
std::string("simulation module contains tools for tracking particles\n")+
std::string("the Geant4 geometry\n");

std::string track_particles_docstring =
std::string("Track a list of particles.\n\n")+
std::string("- primary_list: Json-formatted string encoding a list of\n")+
std::string("  primaries. Each primary in the list should be a json-\n")+
std::string("  formatted string like\n")+
std::string("    {'primary':\n      {\n")+
std::string("        'position':{'x':<x pos>, 'y':<y pos>, 'z':<z pos>},\n")+
std::string("        'momentum':{'x':<x mom>, 'y':<y mom>, 'z':<z mom>},\n")+
std::string("        'particle_id':<pdg integer particle id>,\n")+
std::string("        'energy':<total energy [MeV]>,\n")+
std::string("        'time':<initial time [ns]>,\n")+
std::string("        'random_seed':<integer random seed>\n")+
std::string("      }\n    }\n")+
std::string("  'position' is a three vector with dimensions of mm.\n")+
std::string("  'momentum' is a dimensionless three vector giving the\n")+
std::string("  momentum direction\n\n")+
std::string("Return value is a json-formatted string containing a list of\n")+
std::string("resultant Monte Carlo events. New keys will be added to the\n")+
std::string("dictionary with Hit data, track data and so on. For details on\n")+
std::string("the structure of Monte Carlo events see the chapter\n")+
std::string("'Using and Modifying the Data Structure' in the MAUS\n")+
std::string("documentation.\n\n")+
std::string("Hint: Conversions to and from json can be done by calling\n")+
std::string("   json.dumps([{<primary dict>}, {<primary dict>}])\n")+
std::string("and\n")+
std::string("   json.loads('<my string>')\n")+
std::string("to dump to a string and load from a string respectively.\n");

static PyObject* track_particles
                              (PyObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {const_cast<char*>("primary_list"), NULL};
    char* primaries = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &primaries)) {
        // error message is set in PyArg_Parse...
        return NULL;
    }
    if (primaries == NULL) {
        // I think this should be handled by PyArg_Parse... but let's be sure
        return NULL;
    }
    try {
        std::string primaries_str(primaries);
        Json::Value primaries_json = JsonWrapper::StringToJson(primaries_str);
        MAUSGeant4Manager* g4man = Globals::GetInstance()->GetGeant4Manager();
        Json::Value mc_event_json = g4man->RunManyParticles(primaries_json);
        std::string mc_event_str = JsonWrapper::JsonToString(mc_event_json);
        PyObject* mc_event_py = Py_BuildValue("s", mc_event_str.c_str());
        return mc_event_py;
    } catch (MAUS::Exceptions::Exception exc) {
        PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
        return NULL;
    }
}

static PyMethodDef _keywdarg_methods[] = {
    {"track_particles", (PyCFunction)track_particles,
    METH_VARARGS|METH_KEYWORDS, track_particles_docstring.c_str()},
    {NULL,  NULL}   /* sentinel */
};


PyMODINIT_FUNC initsimulation(void) {
    PyObject* module = Py_InitModule3("simulation", _keywdarg_methods,
                                                     module_docstring.c_str());
    if (module == NULL) return;
}
}  // namespace PySimulation
}  // namespace MAUS

