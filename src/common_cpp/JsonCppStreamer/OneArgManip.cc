#include "OneArgManip.hh"
#include "RStream.hh"

oneArgManip<const char*>* branchName(const char* name){
  return new oneArgManip<const char*>(rstream::setBranch,name);
}
