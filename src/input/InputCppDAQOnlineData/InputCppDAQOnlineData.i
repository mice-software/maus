%module InputCppDAQOnlineData
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppDAQOnlineData.hh"
 #include "src/input/InputCppDAQData/InputCppDAQData.hh"

 %}
%include "std_string.i"
%include "src/input/InputCppDAQData/InputCppDAQData.i"

%feature("shadow") MAUS::InputCppDAQOnlineData::emitter() %{
def emitter(self):
  while (self.readNextEvent()):
	print self.getCurEvent()
    yield self.getCurEvent()
%}

%include "InputCppDAQOnlineData.hh"
%include "src/input/InputCppDAQData/InputCppDAQData.hh"

