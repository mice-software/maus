#include "OneArgManip.hh"

oneArgManip<const char*>* branchName(const char* name){
  return new oneArgManip<const char*>(rstream::setBranch,name);
}
