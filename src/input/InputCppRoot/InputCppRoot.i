%include std_string.i
%include cpointer.i
%include exception.i

// any exceptions thrown following this declaration should make a python
// exception
%exception {
  try {
    $function
  }
  catch (const std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
}

%{
#include "src/common_cpp/API/IModule.hh"
#include "src/common_cpp/API/ModuleBase.hh"
#include "src/common_cpp/API/IInput.hh"
#include "src/common_cpp/API/InputBase.hh"
using namespace std;
%}

%include "src/common_cpp/API/IModule.hh"
%include "src/common_cpp/API/ModuleBase.hh"
%include "src/common_cpp/API/IInput.hh"
%include "src/common_cpp/API/InputBase.hh"

%define INPUTBASE_WRAP(OUTPUT) 

    %template(IInput ## OUTPUT) MAUS::IInput<OUTPUT>;
    %template(InputBase ## OUTPUT) MAUS::InputBase<OUTPUT>;

    #ifndef OUTPUT ## p
    %pointer_functions(OUTPUT,  OUTPUT  ## p);
    #define OUTPUT ## p
    #endif

%enddef

%feature("shadow") MAUS::InputBase::emitter() %{
def emitter(self):
    yield self.emitter_cpp()
%}

using namespace std;
INPUTBASE_WRAP(string)

%module InputCppRoot
%{
#include "InputCppRoot.hh"
%}

%include "InputCppRoot.hh"


