%module InputCppDAQData
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppDAQData.hh"

 %}
%include "std_string.i"

%feature("shadow") InputCppDAQData::emitter() %{
def emitter(self):
  spill = self.getNextSpill()
  print "START WHILE LOOP"
  while ( spill != ""):
    print "WHILE LOOP"
    yield spill
    spill = self.getNextSpill()
%}

%include "InputCppDAQData.hh"

