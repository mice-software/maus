%include "std_string.i"
%include cpointer.i

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

using namespace std;
INPUTBASE_WRAP(string)

%module InputCppRoot
%{
#include "InputCppRoot.hh"
%}

%feature("shadow") MAUS::InputCppRoot::emitter() %{
def emitter(self):
  next_event = "No Event"
  while (next_event != ""):
    next_event = self.emitter_cpp()
    if next_event == "":
        return
    yield next_event
%}



%include "InputCppRoot.hh"


