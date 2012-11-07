%include "std_string.i"
%include cpointer.i

%{
#include "src/common_cpp/API/IModule.hh"
#include "src/common_cpp/API/ModuleBase.hh"
#include "src/common_cpp/API/IOutput.hh"
#include "src/common_cpp/API/OutputBase.hh"
using namespace std;
%}

%include "src/common_cpp/API/IModule.hh"
%include "src/common_cpp/API/ModuleBase.hh"
%include "src/common_cpp/API/IOutput.hh"
%include "src/common_cpp/API/OutputBase.hh"

%define OUTPUTBASE_WRAP(INPUT) 

    %template(IOutput ## INPUT) MAUS::IOutput<INPUT>;
    %template(OutputBase ## INPUT) MAUS::OutputBase<INPUT>;

    #ifndef INPUT ## p
    %pointer_functions(INPUT,  INPUT  ## p);
    #define INPUT ## p
    #endif

%enddef

using namespace std;
OUTPUTBASE_WRAP(string)

%module OutputCppRoot
%{
#include "OutputCppRoot.hh"
%}

%include "OutputCppRoot.hh"


