#include "MapCppTOFReconstruction.hh"

dataCards MyDataCards("Digitization");
MICEEvent simEvent;

bool MapCppTOFReconstruction::birth(std::string argJsonConfigDocument)
{
	cout<<"MapCppTOFReconstruction::birth"<<endl;

  _classname = "MapCppTOFReconstruction";

  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;
	  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  assert(configJSON.isMember("TOF_cabling_file"));
	string map_file_name = configJSON["TOF_cabling_file"].asString();
	char* pMAUS_ROOT_DIR = getenv( "MAUS_ROOT_DIR" );
  map.InitFromFile( string(pMAUS_ROOT_DIR) + map_file_name );

  return true;
}

bool MapCppTOFReconstruction::death()
{
	cout<<"MapCppTOFReconstruction::death"<<endl;

  return true;
}


std::string  MapCppTOFReconstruction::process(std::string document)
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

  Json::Value xDocTOF0, xDocTOF1, xDocTOF2;

  /// Check if the JSON _daq_data has a 'tof0' branch
  if (xDaqData.isMember("tof0"))
    xDocTOF0 = xDaqData.get("tof0",0);
  //cout << xDocTOF0 << endl;

  /// Check if the JSON _daq_data has a 'tof0' branch
  if (xDaqData.isMember("tof1"))
    xDocTOF1 = xDaqData.get("tof1",0);

  /// Check if the JSON _daq_data has a 'tof0' branch
  if (xDaqData.isMember("tof2"))
    xDocTOF2 = xDaqData.get("tof2",0);

  Json::Value xLevel1Rec, xLevel2Rec, xLevel3Rec, xLevel4Rec;

  level1_recon(xDocTOF0, xLevel1Rec["tof0"]);
  level1_recon(xDocTOF1, xLevel1Rec["tof1"]);
  level1_recon(xDocTOF2, xLevel1Rec["tof2"]);
  //cout << xLevel1Rec << endl;

  level2_recon(xLevel1Rec["tof0"], xLevel2Rec["tof0"]);
  level2_recon(xLevel1Rec["tof1"], xLevel2Rec["tof1"]);
  level2_recon(xLevel1Rec["tof2"], xLevel2Rec["tof2"]);
  cout << xLevel1Rec << endl;

  return writer.write(root);
}

bool MapCppTOFReconstruction::level1_recon(Json::Value xDocDetectorData, Json::Value &xDocLevel1Recon)
{
  if( xDocDetectorData.isArray() ){
    ///  Get the number of Particle events.
    int n_part_event = xDocDetectorData.size();
    for(int PartEvCount=0; PartEvCount<n_part_event; PartEvCount++){
			///  Get the Particle event.
      Json::Value xDocPartEvent = xDocDetectorData[PartEvCount];
	    //cout << xDocPartEvent << endl;
      Json::Value xDocTdc;
      ///  Check if the JSON doc has a 'V1290' branch. These are the TDC measurements.
      if (xDocPartEvent.isMember("V1290"))
        xDocTdc = xDocPartEvent.get("V1290",0);
      if( xDocTdc.isArray() ){
        ///  Get the number of TDC hits.
        int n_hits = xDocTdc.size();
        for(int HitCount=0; HitCount<n_hits; HitCount++){
          stringstream xConv;
          DAQChannelKey xDaqKey;
          string xDaqKey_str = xDocTdc[HitCount]["channel_key"].asString();
          xConv<<xDaqKey_str;
          xConv>>xDaqKey;
          TOFChannelKey* xTofKey = map.find(&xDaqKey);
          if(xTofKey)
            xDocTdc[HitCount]["tof_key"] = xTofKey->str();
          else cout<<"errore"<<endl;
          ///  Append the TDC measurement to the proper PMT branch.
          xDocLevel1Recon[PartEvCount][xTofKey->str()].append( xDocTdc[HitCount] );
					///  Add this PMT to the list of PMT keys.
					///  This list will be used in Level2.
          xDocLevel1Recon[PartEvCount]["key_list"].append(xTofKey->str());
        }
      }

      Json::Value xDocfAdc;
      ///  Check if the JSON doc has a 'V1724' branch. These are the fADC measurements.
      if (xDocPartEvent.isMember("V1724"))
        xDocfAdc = xDocPartEvent.get("V1724",0);
      if( xDocfAdc.isArray() ){
        ///  Get the number of fADC hits.
        int n_hits = xDocfAdc.size();
        for(int HitCount=0; HitCount<n_hits; HitCount++){
          stringstream xConv;
          DAQChannelKey xDaqKey;
          string xDaqKey_str = xDocfAdc[HitCount]["channel_key"].asString();
          xConv<<xDaqKey_str;
          xConv>>xDaqKey;
          TOFChannelKey* xTofKey = map.find(&xDaqKey);
          if(xTofKey)
            xDocfAdc[HitCount]["tof_key"] = xTofKey->str();
          else cout<<"errore"<<endl;
          ///  Append the fADC measurement to the proper PMT branch.
					///  If there is a TDC hit for this PMT the PMT key is already added to the list.
          xDocLevel1Recon[PartEvCount][xTofKey->str()].append( xDocfAdc[HitCount] );
        }
      }
    }
  }
  return true;
}

bool MapCppTOFReconstruction::level2_recon(Json::Value xDocLevel1Recon, Json::Value &xDocLevel2Recon)
{
  if( xDocLevel1Recon.isArray() ){
    /// Get the number of Particle events.
    unsigned int n_part_event = xDocLevel1Recon.size();
    for(int PartEvCount=0; PartEvCount<n_part_event; PartEvCount++){
			///  Get the Particle event
      Json::Value xDocPartEvent = xDocLevel1Recon[PartEvCount];
      ///  Get tne number of activ channels in this event, from the list of PMT keys.
			///  This list is filled in Level1.
      int n_channels = xDocLevel1Recon[PartEvCount]["key_list"].size();
      for(int channelCount=0; channelCount<n_channels; channelCount++){
				///  Get the PMT key from the list of keys.
        string key = xDocLevel1Recon[PartEvCount]["key_list"][channelCount].asString();
        ///  Get the number of measurements for this PMT
        int n_hits_in_ch = xDocPartEvent[key].size();
				  if( n_hits_in_ch == 2 ){
						Json::Value xDigit;
						for(int i=0;i<n_hits_in_ch;i++){
              ///  Get the measurement.
							Json::Value measurmet = xDocPartEvent[key][i];
						  if(measurmet["equip_type"].asInt()==102)
							  xDigit["time"] = measurmet["leading_time"].asInt();
						  if(measurmet["equip_type"].asInt()==120)
							  xDigit["charge"] = measurmet["charge_mm"].asInt();
						}
						xDocLevel2Recon[PartEvCount][key] = xDigit;
					  xDocLevel2Recon[PartEvCount]["key_list"].append(key);
					}
      }
    }
  }
}

bool MapCppTOFReconstruction::get_next_ParticleEvent()
{

}