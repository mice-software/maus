%module InputCppData
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppData.hh"

 %}
%include "std_string.i"

%feature("shadow") InputCppData::emitter() %{
def emitter(self):
  while (self.readNextEvent()):
    yield self.getCurEvent()
%}

%include "InputCppData.hh"

