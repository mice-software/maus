#ifndef OUTPUTCPPROOT
#define OUTPUTCPPROOT

class MausData;
class JsonCppConverter;
class orstream;

namespace MAUS {

class OutputCppRoot {
 public:
  OutputCppRoot() {;}
  ~OutputCppRoot() {;}

  bool birth(std::string json_datacards);

  bool save(std::string json_spill_document);

  bool death();

 private:
  orstream* _outfile;
  MausData* _md;
  JsonCppConverter* _jsonCppConverter;
};

}

#endif


