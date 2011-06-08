// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: VersionInfo.hh,v 1.1 2004-11-28 22:41:19 torun Exp $ $Name:  $
// Declaration of classes for versioning

#if !defined(VERSION_INFO_HH)
#define VERSION_INFO_HH

#include <string>
#include <map>

#define V_MAX_FIELDS 60
#define V_MAX_VERSIONS 10
#define V_MAX_FIELD_SIZE 10


struct Field {
  typedef enum {
    NOTHING = 0,
    INT = 1,
    DOUBLE = 2,
    HEP3VECTOR = 3
  } field_type;

  field_type type;
  std::string name;
  std::string extra;
};

struct Version {
  std::string id;
  Field field[V_MAX_FIELDS];
  std::string extra;
};

struct VersionInfo {
  std::string className;
  Version version[V_MAX_VERSIONS];
  std::string extra;

private:
  static std::map<std::string,const VersionInfo*> registered;

public:
  static void registerInfo(const VersionInfo* info);
  static const VersionInfo* forClass(std::string name);
};


#endif // VERSION_INFO_HH
