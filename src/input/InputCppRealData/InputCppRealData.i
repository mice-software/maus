%module InputCppRealData
 %{
 /* Includes the header in the wrapper code */
 #include "InputCppRealData.hpp"

 %}
%include "std_string.i"

/* Create the iterator functions with some smarter python code ! */
/*
%feature("shadow") UnpackTest::__init__() %{
def __iter__(self):
  self._i = 0
  return self
%}

%feature("shadow") UnpackTest::next() %{
def next(self):
  self._i = self._i + 1
  if (self._i > 5):
    raise StopIteration
  return self._i
%}
*/

%feature("shadow") InputCppRealData::Emitter() %{
def Emitter(self):
  while (self.readNextEvent()):
    yield self.getCurEvent()
%}

%include "InputCppRealData.hpp"

