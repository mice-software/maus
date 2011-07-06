#include "MapCppEMRMonitor.hh"

bool MapCppEMRMonitor::birth(string argJsonConfigDocument)
{}

bool MapCppEMRMonitor::death()
{}

string MapCppEMRMonitor::process(std::string document)
{
  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(document, root);
  if (!parsingSuccessful) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
		return false;
  }
	//cout<<root<<endl;

  Json::Value xDaqData;
  // Check if the JSON document has a 'daq_data' branch, else return error
  if (!root.isMember("daq_data")) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "I need a Daq Data branch.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
		return false;
  }else xDaqData = root.get("daq_data", 0);
  //cout<<xDaqData<<endl;

  Json::Value xDocEMR;

  /// Check if the JSON _daq_data has a 'tof0' branch
  if (xDaqData.isMember("emr"))
    xDocEMR = xDaqData.get("emr",0);
  cout << xDocEMR << endl;

  return writer.write(root);
}


