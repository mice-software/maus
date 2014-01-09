%module InputCppDAQOfflineData
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppDAQOfflineData.hh"
 #include "src/input/InputCppDAQData/InputCppDAQData.hh"

 %}
%include "std_string.i"
%include "src/input/InputCppDAQData/InputCppDAQData.i"

%feature("shadow") MAUS::InputCppDAQOfflineData::emitter() %{
def emitter(self):
  while (self.readNextEvent()):
    yield self.getCurEvent()
%}

%include "InputCppDAQOfflineData.hh"
%include "src/input/InputCppDAQData/InputCppDAQData.hh"

