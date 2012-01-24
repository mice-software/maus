#include "src/legacy/Interface/Squeal.hh"

#include "src/common_cpp/DataStructureBase/ORStream.hh"
#include "src/common_cpp/DataStructure/JsonCppConverter.hh"

#include "OutputCppRoot.hh"

namespace MAUS {

bool OutputCppRoot::birth(std::string json_datacards) {
  _outfile = new orstream("test.root", "MausData");
  Digits* d = new Digits();
  MC* mc = new MC();
  _md = new MausData(d, mc);
  (*_outfile) << oneArgManip<const char*>::branchName("digits") << d;
  (*_outfile) << oneArgManip<const char*>::branchName("mc") << mc;
  _jsonCppConverter = new JsonCppConverter(_md);
  return true;
}

bool OutputCppRoot::save(std::string json_spill_document) {
  try {
    (*_jsonCppConverter)(json_spill_document);
    (*_outfile) << fillEvent;
  } catch (Squeal squeal) {
    return false;
  } catch (std::exception exc) {
    return false;
  }
  return true;
}

bool OutputCppRoot::death() {
  _outfile->close();
  delete _md;
  return true;
}

}

