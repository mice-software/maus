// @(#) $Id: VersionInfo.cc,v 1.1 2004-11-28 22:41:19 torun Exp $ $Name:  $
// Definition of class VersionInfo

#include "VersionInfo.hh"

#include <string>
#include <map>

using namespace std;

map<string,const VersionInfo*> VersionInfo::registered;

void
VersionInfo::registerInfo(const VersionInfo* info) {
  registered[info->className] = info;
}


const VersionInfo*
VersionInfo::forClass(string name) {
  return registered[name];
}
