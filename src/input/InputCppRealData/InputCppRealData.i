%module InputCppRealData
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppRealData.hh"

 %}
%include "std_string.i"

%feature("shadow") InputCppRealData::emitter() %{
def emitter(self):
  while (self.readNextEvent()):
    yield self.getCurEvent()
%}

%include "InputCppRealData.hh"

