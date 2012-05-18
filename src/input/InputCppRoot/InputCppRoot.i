%module InputCppRoot
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppRoot.hh"

 %}
%include "std_string.i"

%feature("shadow") MAUS::InputCppRoot::emitter() %{
def emitter(self):
  next_event = "No Event"
  while (next_event != ""):
    next_event = self.getNextEvent()
    if next_event == "":
        return
    yield next_event
%}

%include "InputCppRoot.hh"

