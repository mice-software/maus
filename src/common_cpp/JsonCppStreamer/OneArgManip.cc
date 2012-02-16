#include "JsonCppStreamer/OneArgManip.hh"
#include "JsonCppStreamer/RStream.hh"

oneArgManip<const char*>* branchName(const char* name) {
  return new oneArgManip<const char*>(rstream::setBranch, name);
}
