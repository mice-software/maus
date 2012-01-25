%module InputCppRoot
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppRoot.hh"

 %}
%include "std_string.i"

%feature("shadow") InputCppRoot::emitter() %{
def emitter(self):
  next_event = "a"
  while (next_event != ""):
    next_event = self.getNextEvent()
    yield next_event
%}

%include "InputCppRoot.hh"

