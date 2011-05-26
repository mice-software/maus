/**
 * @author      Takashi Matsushita
 * @date        $Date: 2007-05-29 01:55:09 $
 * @version     $Revision: 1.22 $
 */
/*
 * Copyright:   (c) 2006 Takashi Matsushita
 * Licence:     All rights reserved.
 * Created:     1 Jan 2006
 * Credits:
 */

/** @todo
    debug
    check existence of key for map
    check range of array index */
/** @warnings */

/*====================================================================*
 * declarations
 *====================================================================*/
#define test_VlpcCableImperial_cc
#undef test_VlpcCableImperial_cc

#define NDEBUG
#undef NDEBUG

/*--------------------------------------------------------------------*
 * headers
 *--------------------------------------------------------------------*/
#include "Config/VlpcCableImperial.hh"
#include "Config/tmUtil.hpp"

/*====================================================================*
 * implementation
 *====================================================================*/
/*--------------------------------------------------------------------*
 * Decode
 *--------------------------------------------------------------------*/
// ---------------------------------------------------------------------
// Constructors and Destructor
// ---------------------------------------------------------------------
VlpcCableImperial::VlpcCableImperial(const std::string configFileName)
      : isValid_(false),
        configFileName_(configFileName)
{
  miceMemory.addNew( Memory::VlpcCableImperial ); 
  // add the MICEFILES path to the configFileName

  configFileName_ = std::string( getenv( "MICEFILES" ) ) + "/Cabling/SciFi/" + configFileName;

//std::cout << "configFileName_ = " << configFileName_ << std::endl;

  this->parse();
  this->showSetup();
}

// ---------------------------------------------------------------------
// methods
// ---------------------------------------------------------------------
void
VlpcCableImperial::setConfigFileName(const std::string& name)
{
  miceMemory.addNew( Memory::VlpcCableImperial ); 
  // add the MICEFILES path to the configFileName

  configFileName_ = std::string( getenv( "MICEFILES" ) ) + "/Cabling/SciFi/" + name;
}

const std::string
VlpcCableImperial::getConfigFileName()
{
  return configFileName_;
}

bool
VlpcCableImperial::parse()
{
  bool rc = true;

  rc &= parseConfig();
  rc &= parseReadout(config_.roLeft, readout_.left);
  rc &= parseReadout(config_.roRight, readout_.right);
  rc &= parseConnection();
  rc &= parseFibremap();

  isValid_ = rc;
  showSetup();

  return rc;
}

void
VlpcCableImperial::showSetup()
{
  if (not isValid_) {
    //TM_LOG_WAR("fibre mapping is not ready");
  } else {
    showConfig();
  }
}

bool
VlpcCableImperial::isValid()
{
  return isValid_;
}

bool
VlpcCableImperial::readout2fibre(int afe,
                      int mcm,
                      int channel,
                      int& stationId,
                      int& view,
                      int& fibre)
{
  stationId = view = fibre = -1;

  if (not isValid_) {
    //TM_LOG_WAR("fibre mapping is not ready");
    return false;
  }

#if 0
  //TM_LOG_INF("afe = " << afe);
  //TM_LOG_INF("mcm = " << mcm);
  //TM_LOG_INF("channel = " << channel);
#endif

  // TODO: key check [afe] and range check [mcm]
  int external = intAfePtrMap_[afe]->externalWaveguide[mcm];
  int d0hole;

  // TODO: range check [channel]
  if (intAfePtrMap_[afe]->isLeft) {
    d0hole = readout_.left[channel];
  } else {
    d0hole = readout_.right[channel];
  }

  int key = encodeExternal(external, d0hole);

  if( externalWaveguide_.find(key) == externalWaveguide_.end() )
  {
    stationId = view = fibre = -1;
    return false;
  }

  int internal = externalWaveguide_[key];
  int station = internalWaveguide_[internal];

  fibre = stationConnector_[station];

  decodeStation(station, stationId, view);

  if( fibre < 1 )
  {
    stationId = view = fibre = -1;
    return false;  
  }

  return true;
}

void
VlpcCableImperial::statPlanFib(int afe,
                    int mcm,
                    int channel,
                    int& station,
                    int& view,
                    int& fibre)
{
  if (not readout2fibre(afe, mcm-1, channel-1, station, view, fibre)) {
  // mcmId and channelId start from ONE instead of ZERO, that's why we subtract one from the value
  // before calling readout2fibre
  }

  return;
}

bool VlpcCableImperial::testExternalCoding( int waveguideId, int holeId )
  {
     int encoded = encodeExternal( waveguideId, holeId );
     int newWgID, newHoleID;
     decodeExternal( encoded, newWgID, newHoleID ); 

     return( ( waveguideId == newWgID ) && ( holeId == newHoleID ) );
  }

bool VlpcCableImperial::testInternalCoding( int waveguideId, int connectorId, int holeId )
  {
     int encoded = encodeInternal( waveguideId, connectorId, holeId );
     int newWgID, newConID, newHoleID;
     decodeInternal( encoded, newWgID, newConID, newHoleID ); 

     return( ( waveguideId == newWgID ) && ( connectorId == newConID ) && ( holeId == newHoleID ) );
  }

bool VlpcCableImperial::testStationCoding( int stationId, int viewId, int connectorId, int holeId )
  {
     int encoded = encodeStation( stationId, viewId, connectorId, holeId );
     int newStatID, newViewID, newConID, newHoleID;
     decodeStation( encoded, newStatID, newViewID, newConID, newHoleID ); 

     return( ( stationId == newStatID ) && ( viewId == newViewID ) && ( connectorId == newConID )&& ( holeId == newHoleID ) );
  }

void VlpcCableImperial::decodeExternal( int encodedExternal, int& waveguideId, int& holeId )
  {
    waveguideId = encodedExternal/1000;
    holeId = encodedExternal%1000;
  }

void VlpcCableImperial::decodeInternal( int encodedInternal, int& waveguideId, int& connectorId, int& holeId )
  {
    waveguideId = encodedInternal/10000;
    connectorId = (encodedInternal%10000)/100;
    holeId = encodedInternal%100;
  }

void VlpcCableImperial::decodeStation( int encodedStation, int& stationId, int& viewId, int& connectorId, int& holeId )
  {
    stationId = encodedStation/100000;
    viewId = (encodedStation%100000)/10000;
    connectorId = (encodedStation%10000)/100;
    holeId = encodedStation%100;
  }

void VlpcCableImperial::readout2cassette(int afe,
                                         int mcm,
                     	                 int channel,
                                         int& cassette, 
                                         int& module,
                                         int& channelCassette)
{
  cassette = module = channelCassette = -1;
      
  //find the cassette number from the board number

  bool isLeft = true;
  
  for( int i = 0; i < cassettes(); ++i )
    if( LeftHandBoard(i) == afe || RightHandBoard(i) == afe )
    {   
      cassette = cassetteNum(i);
      if( RightHandBoard(i) == afe )
        isLeft = false;
    }

  // find the module number

  module = mcm;
  
  if( ! isLeft )
    module = 9 - mcm;
 
  // find the channel number on the cassette
  
  if (intAfePtrMap_[afe]->isLeft) 
    channelCassette = readout_.left[channel-1];
  else
    channelCassette = readout_.right[channel-1];
  
  return;
}

void VlpcCableImperial::readout2waveguide(int afe,
                                          int mcm,
                     	                  int channel,
                                          int& waveguide,
                                          int& channelWaveguide)
{
  waveguide = channelWaveguide = -1;
      
  // find the waveguide number
      
    waveguide  = intAfePtrMap_[afe]->externalWaveguide[mcm-1];
     
  // find channel number on the waveguide
   
    if (intAfePtrMap_[afe]->isLeft) {
      channelWaveguide = readout_.left[channel-1];
    } else {
      channelWaveguide = readout_.right[channel-1];
    }
 
      return;
}

void VlpcCableImperial::readout2patchpanel(int afe,
                                           int mcm,
                     	                   int channel,
                                           int& patchPanelCon, 
                                           int& channelPatchPanel)
{
      patchPanelCon = channelPatchPanel = -1;
    
  // find the patch panel connector number
/*  
  for (std::map<int, Afe *>::iterator it = intAfePtrMap_.begin();
                                      it != intAfePtrMap_.end(); it++) {
    int afe = it->first;

    for (int mcm = 0; mcm < NUMBER_OF_MODULES; ++mcm ) {
*/
      patchPanelCon = intAfePtrMap_[afe]->externalWaveguide[mcm-1];

  // find channel number on the patch panel

//      for (int channel = 0; channel < NUMBER_OF_CHANNELS_PER_MCM; ++channel ) {
        if (intAfePtrMap_[afe]->isLeft) {
          channelPatchPanel = readout_.left[channel-1];
        } else {
          channelPatchPanel = readout_.right[channel-1];

        }
//      }
//    }
//  }
  return;
}

bool VlpcCableImperial::readouthaspatchpanel(int afe,
                                             int mcm,
                     	                     int channel )
    {
	//ME - at the moment, it does not look like the current
	// data structure can actually distinguish between stations
	// that had a patch panel interface and those that did not.
	// Do we need this to be implemented correctly?

    return false;
    }

void VlpcCableImperial::readout2station(int afe,
                                        int mcm,
                     	                int channel,
                                        int& stat,
                                        int& statCon,
                                        int& channelStation)
{
  stat = statCon = channelStation = -1;

  // find station number
  
  int external = intAfePtrMap_[afe]->externalWaveguide[mcm-1];
  int d0hole;

  if (intAfePtrMap_[afe]->isLeft) {
    d0hole = readout_.left[channel-1];
  } else {
    d0hole = readout_.right[channel-1];
  }

  int key = encodeExternal(external, d0hole);

  if( externalWaveguide_.find(key) == externalWaveguide_.end() )
  {
    return;
  }

  int internal = externalWaveguide_[key];
  int estat = internalWaveguide_[internal];
  int view;

   decodeStation( estat, stat, view );
/*
  int external = intAfePtrMap_[afe]->externalWaveguide[mcm];
  int d0hole;

  if (intAfePtrMap_[afe]->isLeft) {
    d0hole = readout_.left[channel-1];
  } else {
    d0hole = readout_.right[channel-1];
  }


//  ExternalWaveguideMap::iterator it = externalWaveguide_.find(encodeExternal(external, d0hole));
//  if (it == externalWaveguide_.end()) {
//    return false;
//  }

  for (std::map<int, int>::iterator it = externalWaveguide_.begin();
                                    it != externalWaveguide_.end(); it++) {

  int internal = externalWaveguide_[encodeExternal(external, d0hole)];
  stat = internalWaveguide_[internal];
*/

  // find station connector number
 
  int wg;

  decodeInternal( estat, wg, statCon, channelStation );

  return;
}


int 	VlpcCableImperial::cassettes() const
{
  return( intAfePtrMap_.size() / 2 );				// AfePtrMap map has 2 boards per cassette
}

int 	VlpcCableImperial::cassetteNum( int i ) const
{
  int pos = 0;

  for( std::map<int,Afe*>::const_iterator it = intAfePtrMap_.begin(); it != intAfePtrMap_.end(); ++it )
    if( it->second->isLeft )
    {
      if( pos == i )
        return it->second->cassette;
      ++pos;
    }

  // haven't found the correct cassette

  return -1;
}

int 	VlpcCableImperial::LeftHandBoard( int i  ) const
{
  int pos = 0;

  for( std::map<int,Afe*>::const_iterator it = intAfePtrMap_.begin(); it != intAfePtrMap_.end(); ++it )
    if( it->second->isLeft )
    {
      if( pos == i )
        return it->first;
      ++pos;
    }

  // haven't found the correct board

  return -1;
}

int 	VlpcCableImperial::RightHandBoard( int i ) const
{
  int pos = 0;

  for( std::map<int,Afe*>::const_iterator it = intAfePtrMap_.begin(); it != intAfePtrMap_.end(); ++it )
    if( ! it->second->isLeft )
    {
      if( pos == i )
        return it->first;
      ++pos;
    }

  // haven't found the correct board

  return -1;
}

bool
VlpcCableImperial::toOsaka()
{
  for (std::map<int, Afe *>::iterator it = intAfePtrMap_.begin();
                                      it != intAfePtrMap_.end(); it++) {
    int afe = it->first;

    for (int mcm = 0; mcm < NUMBER_OF_MODULES; mcm++) {
      int external = intAfePtrMap_[afe]->externalWaveguide[mcm];
      int d0holeId;

      for (int channel = 0; channel < NUMBER_OF_CHANNELS_PER_MCM; channel++) {
        if (intAfePtrMap_[afe]->isLeft) {
          d0holeId = readout_.left[channel];
        } else {
          d0holeId = readout_.right[channel];

        }

        int key = encodeExternal(external, d0holeId);
        if (externalWaveguide_.find(key) == externalWaveguide_.end()) {
          //TM_LOG_WAR("skip (ext., hole.) = " << external << "," << d0holeId);
          continue;
        }

        int internal = externalWaveguide_[key];
        int station = internalWaveguide_[internal];
        int stationId, viewId;

        decodeStation(station, stationId, viewId);
        int fibre = stationConnector_[station];

        // convert to osaka convention
        int afeOsaka = 0;

        if (afe == 0) {
          afeOsaka = 104;
        } else if  (afe == 1) {
          afeOsaka = 107;
        } else if  (afe == 2) {
          afeOsaka = 106;
        } else if  (afe == 3) {
          afeOsaka = 105;
        } else {
          //TM_NOT_IMPLEMENTED_ERROR();
        }

        if (stationId == 0) {
          stationId = 1;
        } else if  (stationId == 1) {
          stationId = 2;
        } else if  (stationId == 2) {
          stationId = 3;
        } else if  (stationId == 3) {
          stationId = 4;
        } else {
          //TM_NOT_IMPLEMENTED_ERROR();
        }

        if (viewId == 0) {
          viewId = 1;
        } else if  (viewId == 1) {
          viewId = 3;
        } else if  (viewId == 2) {
          viewId = 2;
        } else {
          //TM_NOT_IMPLEMENTED_ERROR();
        }

        std::cout << afeOsaka << "\t" << mcm+1 << "\t" << channel+1 << "\t"
                  << stationId << "\t" << viewId << "\t" << fibre+1 << std::endl;
      }
    }
  }

  return true;
}


bool
VlpcCableImperial::parseConfig()
{
  //TM_LOG_DBG("parseConfig()");

  bool rc = true;

  std::ifstream file;
  file.open(configFileName_.c_str());
  if (!file.is_open()) {
    //TM_FATAL_ERROR("could not open a file = " << configFileName_);
  }

  while (!file.eof()) {
    std::vector<std::string> tokens;
    std::string line;

    getline(file, line);
    line = TM::trim(line);
    if (line.length() == 0) {
      continue;
    }
    if (line[0] == COMMENT_CHAR) {
      continue;
    }

    tokens.clear();
    TM::tokenise(line, tokens, "=");
    for (std::vector<std::string>::iterator it = tokens.begin();
                                            it != tokens.end(); it++) {
      std::string key = TM::trim(*it);
      if (key == "FIBRE_MAP") {
        config_.fibreMap = TM::trim(*++it);
      } else if (key == "CONNECTION") {
        config_.connection = TM::trim(*++it);
      } else if (key == "RO_RIGHT") {
        config_.roRight = TM::trim(*++it);
      } else if (key == "RO_LEFT") {
        config_.roLeft = TM::trim(*++it);
      } else {
        rc = false;
      }
    }
  }

  return rc;
}

bool
VlpcCableImperial::parseReadout(const std::string& fileName,
                     int *array)
{
  //TM_LOG_DBG("parseReadout()");

  bool rc = true;

  std::string fnam = std::string( getenv( "MICEFILES" ) ) + "/Cabling/SciFi/" + fileName;

  std::ifstream file;
  file.open(fnam.c_str());

  if (!file.is_open()) {
    TM_FATAL_ERROR("could not open a file = " << fileName);
  }

  //TM_LOG_INF(fileName);

  for (int ii = 0; ii < NUMBER_OF_CHANNELS_PER_MCM; ii++) {
    array[ii] = -1;
  }

  while (!file.eof()) {
    std::vector<std::string> tokens;
    std::string line;

    getline(file, line);
    line = TM::trim(line);
    if (line.length() == 0) {
      continue;
    }
    if (line[0] == COMMENT_CHAR) {
      continue;
    }

    tokens.clear();
    TM::tokenise(line, tokens, "=");
    for (std::vector<std::string>::iterator it = tokens.begin();
                                            it != tokens.end(); it++) {
      std::string key = TM::trim(*it);
      if (key == "TYPE") {
        //TM_LOG_INF("   TYPE = " << 
TM::trim(*++it);
      } else if (key == "VERSION") {
        //TM_LOG_INF("   VERSION = " << 
TM::trim(*++it);
      } else {
        int index, value;
        index = atoi((*it).c_str()) - 1;
        value = atoi((*++it).c_str()) - 1;

        if (index < 0 || index >= NUMBER_OF_CHANNELS_PER_MCM) {
//          TM_FATAL_ERROR("index error: (" << index << "," << value << ")");
        }
        if (value < 0 || value >= NUMBER_OF_CHANNELS_PER_MCM*2) {
//          TM_FATAL_ERROR("value error: (" << index << "," << value << ")");
        }

        array[index] = value;
      }
    }
  }

  return rc;
}

bool
VlpcCableImperial::parseConnection()
{
  //TM_LOG_DBG("parseConnection()");

  bool rc = true;

  std::string fnam = std::string( getenv( "MICEFILES" ) ) + "/Cabling/SciFi/" + config_.connection;

  std::ifstream file;
  file.open( fnam.c_str() );

  if (!file.is_open()) {
    TM_FATAL_ERROR("could not open a file = " << config_.connection);
  }

  //TM_LOG_INF(config_.connection);

  while (!file.eof()) {
    std::vector<std::string> tokens;
    std::string line;

    getline(file, line);
    line = TM::trim(line);
    if (line.length() == 0) {
      continue;
    }
    if (line[0] == COMMENT_CHAR) {
      continue;
    }

    tokens.clear();
    TM::tokenise(line, tokens, "=");
    for (std::vector<std::string>::iterator it = tokens.begin();
                                            it != tokens.end(); it++) {
      std::string key = TM::trim(*it);
      if (key == "TYPE") {
        //TM_LOG_INF("   TYPE = " << 
TM::trim(*++it);
      } else if (key == "VERSION") {
        //TM_LOG_INF("   VERSION = " << 
TM::trim(*++it);
      } else {
        std::vector<std::string> subTokens;

        int waveguide = -1;
        int cassette = -1;
        int module = -1;
        int afeLeft = -1;
        int afeRight = -1;

        waveguide = atoi((*it).c_str());

        subTokens.clear();
        TM::tokenise(*++it, subTokens, " ");
        for (std::vector<std::string>::iterator its = subTokens.begin();
                                                its != subTokens.end(); its++) {

          cassette = atoi((*its).c_str());
          module = atoi((*++its).c_str()) - 1;
        }

        subTokens.clear();
        TM::tokenise(*++it, subTokens, " ");
        for (std::vector<std::string>::iterator its = subTokens.begin();
                                                its != subTokens.end(); its++) {

          afeLeft = atoi((*its).c_str());
          afeRight = atoi((*++its).c_str());
        }

#if 0
        //TM_LOG_DBG("  waveguide = " << waveguide);
        //TM_LOG_DBG("  cassette  = " << cassette);
        //TM_LOG_DBG("  module    = " << module);
        //TM_LOG_DBG("  afeLeft   = " << afeLeft);
        //TM_LOG_DBG("  afeRight  = " << afeRight);
#endif

        int mcm = module;

#if 0
        //TM_LOG_DBG("  count  = " << intAfePtrMap_.count(afeLeft));
        //TM_LOG_DBG("  mcm  = " << mcm);
#endif
        if (intAfePtrMap_.count(afeLeft) == 0) {
          Afe *afe = new Afe();
          intAfePtrMap_.insert(std::make_pair(afeLeft, afe));
        }
        intAfePtrMap_[afeLeft]->isLeft = true;
        intAfePtrMap_[afeLeft]->externalWaveguide[mcm] = waveguide;
        intAfePtrMap_[afeLeft]->cassette = cassette;

        mcm = NUMBER_OF_MODULES - module - 1;
        key = afeRight;

#if 0
        //TM_LOG_DBG("  count  = " << intAfePtrMap_.count(afeRight));
        //TM_LOG_DBG("  mcm  = " << mcm);
#endif
        if (intAfePtrMap_.count(afeRight) == 0) {
          Afe *afe = new Afe();
          intAfePtrMap_.insert(std::make_pair(afeRight, afe));
        }
        intAfePtrMap_[afeRight]->isLeft = false;
        intAfePtrMap_[afeRight]->externalWaveguide[mcm] = waveguide;
        intAfePtrMap_[afeRight]->cassette = cassette;

      }
    }
  }

  return rc;
}

bool
VlpcCableImperial::parseFibremap()
{
  //TM_LOG_DBG("parseFibremap()");

  bool rc = true;

  std::string fnam = std::string( getenv( "MICEFILES" ) ) + "/Cabling/SciFi/" + config_.fibreMap;

  std::ifstream file;
  file.open( fnam.c_str() );

  if (!file.is_open()) {
    TM_FATAL_ERROR("could not open a file = " << config_.fibreMap);
  }

  //TM_LOG_INF(config_.fibreMap);

  while (!file.eof()) {
    std::vector<std::string> tokens;
    std::string line;

    getline(file, line);
    line = TM::trim(line);
    if (line.length() == 0) {
      continue;
    }
    if (line[0] == COMMENT_CHAR) {
      continue;
    }

    tokens.clear();
    TM::tokenise(line, tokens, "=");
    for (std::vector<std::string>::iterator it = tokens.begin();
                                            it != tokens.end(); it++) {

      std::string key = TM::trim(*it);
      if (key == "TYPE") {
        //TM_LOG_INF("   TYPE = " << 
TM::trim(*++it);
      } else if (key == "VERSION") {
//        TM_LOG_INF("   VERSION = " << 
TM::trim(*++it);
      } else {
        std::vector<std::string> subTokens;

        int station = -1;
        int view = -1;
        int fibre = -1;
        int stConnId = -1;
        int stHoleId = -1;
        int inGuideId = -1;
        int inConnId = -1;
        int  inHoleId = -1; 
        int inPpId = -1;
        int exGuideId = -1; 
        int exHoleId = -1;
        int d0holeId = -1;

        subTokens.clear();
        TM::tokenise(*it, subTokens, " ");
        for (std::vector<std::string>::iterator its = subTokens.begin();
                                                its != subTokens.end(); its++) {
          station = atoi((*its).c_str());
          view = atoi((*++its).c_str());
          fibre = atoi((*++its).c_str());
        }
#if 0
        //TM_LOG_DBG("  station = " << station);
        //TM_LOG_DBG("  view = " << view);
        //TM_LOG_DBG("  fibre = " << fibre);
#endif

        subTokens.clear();
        TM::tokenise(*++it, subTokens, " ");
        for (std::vector<std::string>::iterator its = subTokens.begin();
                                                its != subTokens.end(); its++) {
          stConnId = atoi((*its).c_str()) - 1;
          stHoleId = atoi((*++its).c_str()) - 1;
        }
#if 0
        //TM_LOG_DBG("  stConnId = " << stConnId);
        //TM_LOG_DBG("  stHoleId = " << stHoleId);
#endif

        subTokens.clear();
        TM::tokenise(*++it, subTokens, " ");
        for (std::vector<std::string>::iterator its = subTokens.begin();
                                                its != subTokens.end(); its++) {
          inGuideId = atoi((*its).c_str());
          inConnId = atoi((*++its).c_str()) - 1;
          inHoleId = atoi((*++its).c_str()) - 1;
        }
#if 0
        //TM_LOG_DBG("  inGuideId = " << inGuideId);
        //TM_LOG_DBG("  inConnId = " << inConnId);
        //TM_LOG_DBG("  inHoleId = " << inHoleId);
#endif

        inPpId = atoi((*++it).c_str());
        // TM_LOG_DBG("  inPpId = " << inPpId);

        subTokens.clear();
        TM::tokenise(*++it, subTokens, " ");
        for (std::vector<std::string>::iterator its = subTokens.begin();
                                                its != subTokens.end(); its++) {
          exGuideId = atoi((*its).c_str());
          exHoleId = atoi((*++its).c_str()) - 1;
        }
#if 0
        //TM_LOG_DBG("  exGuideId = " << exGuideId);
        //TM_LOG_DBG("  exHoleId = " << exHoleId);
#endif

        d0holeId = atoi((*++it).c_str()) - 1;
        // TM_LOG_DBG("  d0holeId = " << d0holeId);
        if (d0holeId < 0) {
          //TM_LOG_WAR("skip (ext., hole.) = " << exGuideId << "," << d0holeId);
          continue;
        }

        externalWaveguide_.insert(std::make_pair(
                                  encodeExternal(exGuideId, d0holeId),
                                  encodeInternal(inGuideId, inConnId, inHoleId)));
        internalWaveguide_.insert(std::make_pair(
                                  encodeInternal(inGuideId, inConnId, inHoleId),
                                  encodeStation(station, view, stConnId, stHoleId)));

        stationConnector_.insert(std::make_pair(
                                 encodeStation(station, view, stConnId, stHoleId),
                    fibre));
      }
    }
  }

  return rc;
}

void
VlpcCableImperial::showConfig()
{
/* ME
  std::cout << "Decoding configuration: "<< std::endl;
  std::cout << "  Fibre map: " << config_.fibreMap << std::endl;
  std::cout << "  Fibre to read-out connection map: " << config_.connection << std::endl;
  std::cout << "  read-out map (right): " << config_.roRight << std::endl;
  std::cout << "  read-out map (left): " << config_.roLeft << std::endl;
*/
}

void
VlpcCableImperial::showReadout()
{

  std::cout << "  read-out map (left): " << std::endl;
  for (int ii = 0; ii < NUMBER_OF_CHANNELS_PER_MCM; ii++) {
    std::cout << ii << " = " << readout_.left[ii] << std::endl;
  }

  std::cout << "  read-out map (right): " << std::endl;
  for (int ii = 0; ii < NUMBER_OF_CHANNELS_PER_MCM; ii++) {
    std::cout << ii << " = " << readout_.right[ii] << std::endl;
  }
}

void
VlpcCableImperial::showConnection()
{
  for (std::map<int, Afe *>::iterator it = intAfePtrMap_.begin();
                                      it != intAfePtrMap_.end(); it++) {
    std::cout << " afe board    = " << it->first << std::endl;
    std::cout << "     cassette = " << it->second->cassette << std::endl;
    for (int ii = 0; ii < NUMBER_OF_MODULES; ii++) {
      std::cout << "     mcm = " << ii;
      std::cout << " <-> waveguide = " << it->second->externalWaveguide[ii]
                << std::endl;
    }
  }
}


void
VlpcCableImperial::showFibremap()
{
  for (std::map<int, int>::iterator it = externalWaveguide_.begin();
                                    it != externalWaveguide_.end(); it++) {
    printExternal(it->first);
    printInternal(it->second);
    printStation(internalWaveguide_[it->second]);
    std::cout << " ch = " << stationConnector_[internalWaveguide_[it->second]] << std::endl;
  }
}

int
VlpcCableImperial::encodeExternal(int waveguideId,
                       int d0holeId)
{
  if (d0holeId < 0 || d0holeId > 127) {
    TM_LOG_WAR("d0holeId = " << d0holeId);
    TM_NOT_IMPLEMENTED_ERROR();
  }

  return waveguideId*1000 + d0holeId;
}

int
VlpcCableImperial::encodeInternal(int waveguideId,
                       int connectorId,
                       int holeId)
{
  return waveguideId*10000 + connectorId*100 + holeId;
}

int
VlpcCableImperial::encodeStation(int stationId,
                      int viewId,
                      int connectorId,
                      int holeId)
{
  return stationId*100000 + viewId*10000 + connectorId*100 + holeId;
}

void
VlpcCableImperial::printExternal(int encoded)
{
  std::cout << " ex = " << encoded/1000 << " " <<  encoded % 1000;
}

void
VlpcCableImperial::printInternal(int encoded)
{
  int waveguide = encoded/1000;
  int connector = (encoded%1000)/100;
  int hole = (encoded%1000)%100;

  std::cout << " in = " << waveguide << " " << connector << " " << hole;
}

void
VlpcCableImperial::printStation(int encoded)
{
  int station = encoded/100000;
  int view = (encoded%100000)/10000;
  int connector = ((encoded%100000)%10000)/100;
  int hole = ((encoded%100000)%10000)%100;

  std::cout << " st = " << station << " " << view << " " << connector << " " << hole;
  
}

void
VlpcCableImperial::decodeStation(int encoded,
                                 int& station,
                                 int& view)
{
  station = encoded/100000;
  view = (encoded%100000)/10000;
}

//-------------------------------------------------------------------------------
// functions
//-------------------------------------------------------------------------------
namespace TM {
std::string
trim(std::string const& source,
     char const* whites)
{
  std::string result(source);
  std::string::size_type index = result.find_last_not_of(whites);
  if (index != std::string::npos) {
    result.erase(++index);
  }

  index = result.find_first_not_of(whites);
  if (index != std::string::npos) {
    result.erase(0, index);
  } else {
    result.erase();
  }

  return result;
}

void
tokenise(const std::string& str,
         std::vector<std::string>& tokens,
         const std::string& delimiters)
{
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos) {
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
  }
}
} // namespace TM

#ifdef test_VlpcCableImperial_cc
#include <unistd.h>

#include <iterator>

using namespace std;

void
usage(char *argv[]) {
  cout << "usage: " << argv[0]
       << " -i <decode configuration file>"
       << " [-h (for this message)]"
       << endl;
  return;
}

int
main (int argc, char *argv[]) {
  string fileName(CONFIG_FILE);

  int c;
  while ((c = getopt(argc, argv, "hi:")) != -1) {
    switch (c) {
      case 'h':
        usage(argv);
        return EXIT_SUCCESS;
        break;
      case 'i':
        fileName = optarg;
        break;
      default:
        usage(argv);
        return EXIT_FAILURE;
    }
  }

  VlpcCableImperial decode;
  decode.setConfigFileName(fileName);

  //TM_LOG_INF("parse()");
  decode.parse();

  // showSetup();
  // showReadout();
  // showConnection();
  // showFibremap();

  //TM_LOG_INF("toOsaka()");
  decode.toOsaka();


  //decode.readout2fibre(afe, mcm, channel, station, plane, fibre);

  //int station, plane, fibre;
  //decode.readout2fibre(1, 7, 63, station, plane, fibre);
  //std::cout << "(2 8 64) -> (" <<
              //station << " " << plane << " " << fibre << ")" << std::endl;

  // sleep(10);


  return EXIT_SUCCESS;
}


#endif // test_VlpcCableImperial_cc
/* eof */
