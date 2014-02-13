%module InputCppDAQData
 %{
 /* Includes the header in the wrapper code */
 #include "src/input/InputCppDAQData/InputCppDAQData.hh"

 %}
%include "std_string.i"

%feature("shadow") MAUS::InputCppDAQData::emitter() %{
def emitter(self):
  while (self.readNextEvent()):
    yield self.getCurEvent()
%}

%include "src/input/InputCppDAQData/InputCppDAQData.hh"

