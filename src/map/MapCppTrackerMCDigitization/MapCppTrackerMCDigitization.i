%module MapCppTrackerMCDigitization
 %{
 /* Includes the header in the wrapper code */
 #include "MapCppTrackerMCDigitization.hh"
 %}
%include "std_string.i"
%include "typemaps.i"
%include "std_vector.i"

namespace std
{
  %template(JsonValueVector) vector<Json::Value>;
}
 
%include "MapCppTrackerMCDigitization.hh"

