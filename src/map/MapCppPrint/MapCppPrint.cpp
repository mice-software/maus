#include "MapCppPrint.h"

bool MapCppPrint::Birth(std::string argJsonConfigDocument) {
  _classname = "MapCppPrint";
  return true;
}

bool MapCppPrint::Death() {
  return true;
}

std::string  MapCppPrint::Process(std::string document) {
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(document, root);
  if (!parsingSuccessful) {
      Json::Value errors;
      std::stringstream ss;

      ss << _classname << " says:" << reader.getFormatedErrorMessages();
      errors["bad_json_document"] = ss.str();
      root["errors"] = errors;

      Json::FastWriter writer;
      return writer.write(root);
  }
  std::cout << root;

  return document;
}
