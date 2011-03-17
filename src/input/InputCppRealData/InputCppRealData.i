%module InputCppRealData
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppRealData.h"

 %}
%include "std_string.i"

%feature("shadow") InputCppRealData::Emitter() %{
def emitter(self):
  while (self.readNextEvent()):
    yield self.getCurEvent()
%}

%include "InputCppRealData.h"

