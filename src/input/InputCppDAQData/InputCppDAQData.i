%module InputCppDAQData
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppDAQData.hh"

 %}
%include "std_string.i"

%feature("shadow") InputCppDAQData::emitter() %{
def emitter(self):
  while (self.readNextEvent()):
    yield self.getCurEvent()
%}

%include "InputCppDAQData.hh"

