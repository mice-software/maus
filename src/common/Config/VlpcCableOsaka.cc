// VlpcCableOsaka.cc
//
// Code to read in the connection information to allow the decoding of hit information
// from channel space (AFE board, MCM, channel) into physical space (station, plane, fibre)
//
// M.Ellis 28/8/2005

#include "VlpcCableOsaka.hh"
#include "tmUtil.hpp"

ViewCable::ViewCable( std::ifstream& inf )
{
  std::string line, tmp;

  // read in the number of connectors

  getline( inf, line );
  std::istringstream ist1( line.c_str() );
  ist1 >> tmp >> planeNum >> numConn;

  connectors.resize( numConn );
  waveguides.resize( numConn );
  cables.resize( numConn );
  reverse.resize( numConn );

  // read in the connector information

  for( int i = 0; i < numConn; ++i )
  {
    std::string tmp;
    getline( inf, line );
    std::istringstream ist( line.c_str() );
    ist >> connectors[i] >> waveguides[i] >> cables[i] >> tmp;
    if( tmp == "reversed" )
      reverse[i] = true;
    else
      reverse[i] = false;
  }
}

StationCable::StationCable( std::ifstream& inf )
{
  std::string line, tmp;

  // read in the station number and type;

  getline( inf, line );
  std::istringstream ist( line.c_str() );
  ist >> tmp >> statNum >> types;

  // read in the number of views

  getline( inf, line );
  std::istringstream ist1( line.c_str() );
  ist1 >> numView;

  // read in the view information

  for( int i = 0; i < numView; ++i )
  {
    ViewCable view( inf );
    views.push_back( view );
  }
}

VlpcCableOsaka::VlpcCableOsaka( std::string cable_file )
{
  miceMemory.addNew( Memory::VlpcCableOsaka ); 
  // first get the environment variable MICEFILES as this is where the model files are kept

  char* MICEFILES = getenv( "MICEFILES" );

  // the full name of the input file:

  std::string fnam = std::string( MICEFILES ) + "/Cabling/" + cable_file;

  osaka = false;

  std::ifstream inf( fnam.c_str() );

  std::string line;

  // read in the header line

  getline( inf, line );

  // read in the number of cassettes

  getline( inf, line );
  std::istringstream ist1( line.c_str() );
  ist1 >> numCassettes;

  // read in the cassette information

  cassette.resize( numCassettes );
  LHB.resize( numCassettes );
  RHB.resize( numCassettes );

  for( int i = 0; i < numCassettes; ++i )
  {
    getline( inf, line );
    std::istringstream ist( line.c_str() );
    ist >> cassette[i] >> LHB[i] >> RHB[i];
  }

  // read in the number of waveguides

  getline( inf, line );
  std::istringstream ist2( line.c_str() );
  ist2 >> numWaveGuides;

  // read in the waveguide information

  waveCass.resize( numWaveGuides );
  waveCassPos.resize( numWaveGuides );
  waveGuide.resize( numWaveGuides );
  waveType.resize( numWaveGuides );

  for( int i = 0; i < numWaveGuides; ++i )
  {
    getline( inf, line );
    std::istringstream ist( line.c_str() );
    ist >> waveCass[i] >> waveCassPos[i] >> waveGuide[i] >> waveType[i];
  }

  // read in the number of stations

  getline( inf, line );
  std::istringstream ist3( line.c_str() );
  ist3 >> numStations;

  // read in the station information

  for( int i = 0; i < numStations; ++i )
  {
    StationCable station( inf );
    stations.push_back( station );
  }
}

// constructor that takes the cable information in the format
// produced by the Osaka group

VlpcCableOsaka::VlpcCableOsaka( std::string cable_file, bool o )
{
  miceMemory.addNew( Memory::VlpcCableOsaka ); 
  // first get the environment variable MICEFILES as this is where the model files are kept

  char* MICEFILES = getenv( "MICEFILES" );

  // the full name of the input file:

  std::string fnam = std::string( MICEFILES ) + "/Cabling/SciFi/" + cable_file;

  osaka = true;

  // open the file

  std::ifstream inf( fnam.c_str() );

  if( ! inf )
    std::cerr << "vlpcCableOsaka - Unable to open file " << fnam << std::endl;

  std::string line;

  // read in the number of boards

  int numBoards;

  getline( inf, line );

  std::istringstream ist1( line.c_str() );

  ist1 >> numBoards;

  //ME - this needs to be fixed urgently!!!

  numCassettes = 0;

  // there are 512 channels per board

  numChannels = numBoards * 512;

  osakaBoard.resize( numChannels );
  osakaMcm.resize( numChannels );
  osakaChannel.resize( numChannels );
  osakaStation.resize( numChannels );
  osakaPlane.resize( numChannels );
  osakaFibre.resize( numChannels );

  int stationConvert[6] = { -1, 2, 1, 3, 4, 5 };

  for( int c = 0; c < numChannels; ++c )
  {
    // read in the board, MCM, channel, station, plane and fibre

    int board, mcm, chan, station, plane, fibre;

    // get the line for this channel

    getline( inf, line );

    std::istringstream ist2( line.c_str() );

    ist2 >> board >> mcm >> chan >> station >> plane >> fibre;

    if( fibre < 1 ) fibre = -1;

    osakaBoard[c] = board + 574000;
    osakaMcm[c] = mcm;
    osakaChannel[c] = chan;
    osakaStation[c] = stationConvert[ station ];
    osakaPlane[c] = plane;
    osakaFibre[c] = fibre;
  }   
}

// mcm is in the range {1-8} and chan in the range {1-64}

void	VlpcCableOsaka::statPlanFib( int board, int mcm, int chan, int& stat, int& plan, int& fib )
{
  stat = plan = fib = -1;

  if( board < 0 || mcm < 0 || chan < 0 )
    return;

  // use different access routine if the data is in the Osaka group format

  if( osaka )
    return statPlanFibOsaka( board, mcm, chan, stat, plan, fib );

  int cass = -1;
  int d0chan = -1;

  int wg = -1;
  int con = -1;
  int pos = -1;

  // {board, MCM, channel} -> {cassette, MCM, D0 channel number}

  CassD0Chan( board, mcm, chan, cass, d0chan );

  if( cass < 0 || d0chan < 0 )
    return;

  // {cassette, MCM, D0 channel number} -> {waveguide, connector, position}

  WaveConnPos( cass, mcm, d0chan, wg, con, pos );

  if( wg < 0 || con < 0 || pos < 0 )
    return;

  // {waveguide, connector, position} -> {station, plane, fibre}

  StatPlanFib( wg, con, pos, stat, plan, fib );

  return;
}

// D0 position map, go from chan number on a given type of board (Left or Right) and
// convert it to the channel number (0-127) on an MCM

  int left[64] = {  39,   7,  38,  53,   5,  21,   4,  52,  35,  19,  18,   2,  50,   1,  17,  16,
                    48,  32,   0,  33,  49,  34,   3,  51,  36,  20,  37,  54,   6,  22,  23,  55,
                    56,   8,  25,  41,  58,  10,  11,  59,  44,  28,  13,  61,  14,  31,  63,  47,
                    15,  30,  46,  62,  45,  29,  12,  60,  43,  27,  26,  42,  57,   9,  24,  40 };
                    
  int right[64] = { 88, 120,  89,  74, 122, 106, 123,  75,  92, 108, 109, 125,  77, 126, 110, 111,
                    79,  95, 127,  94,  78,  93, 124,  76,  91, 107,  90,  73, 121, 105, 104,  72,
                    71, 119, 102,  86,  69, 117, 116,  68,  83,  99, 114,  66, 113,  96,  64,  80,
                   112,  97,  81,  65,  82,  98, 115,  67,  84, 100, 101,  85,  70, 118, 103,  87 };

  int miceconnector[128] = { 0,  0,  1,  1,  2,  2,  3, -2, -2,  4,  4,  5,  5,  6,  6,  6,
                             0,  0,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  6,  6,  6,
                             0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  5,  5,  5,  6,  6,
                             0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  5,  5,  5,  6,  6,
                             0,  0,  1,  1,  1,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,
                             0,  0,  1,  1,  1,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,
                             0,  0,  0,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  6,  6,
                             0,  0,  0,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  6,  6 };

  int micepos[128] = {  8, 16,  6, 14,  4, 12,  2, -1, -1,  6, 14,  4, 12,  2, 10, 18,
                        7, 15,  5, 13,  3, 11,  1,  9, 16,  5, 13,  3, 11,  1,  9, 17,
                        6, 14,  4, 12,  2, 10, 18,  8, 15,  4, 12,  2, 10, 18,  8, 16,
                        5, 13,  3, 11,  1,  9, 17,  7, 14,  3, 11,  1,  9, 17,  7, 15,
                        4, 12,  2, 10, 18,  8, 16,  6, 13,  2, 10, 18,  8, 16,  6, 14,
                        3, 11,  1,  9, 17,  7, 15,  5, 12,  1,  9, 17,  7, 15,  5, 13,
                        2, 10, 18,  8, 16,  6, 14,  4, 11, 18,  8, 16,  6, 14,  4, 12,
                        1,  9, 17,  7, 15,  5, 13,  3, 10, 17,  7, 15,  5, 13,  3, 11 };

  int miceconnector_new[128] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                                 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                                 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                                 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 };

  int micepos_new[128] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                           1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                           1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                           1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                           1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                           1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };

  int miceconnector_mod[128] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                                 -1, 4, 4, 4, 4, 4, 4, 4, -1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                                 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                                 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 };

  int micepos_mod[128] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                           1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                           1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, -1,
                           1, 2, 3, 4, 5, 6, 7, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                           1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                           1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

  int afe2_to_afe1[128] = { 17, 16, 15, 18, 14, 19, 13, 12, 20, 11, 21,10, 22, 9, 23, 8, 24, 7, 25,
                            6, 5, 26, 4, 27, 3, 28, 2, 29, 1, 30, 0, 31, 32, 63, 33, 62, 34, 61, 35,
 			    60, 36, 59, 37, 58, 57, 38, 56, 39, 55, 40, 54, 41, 53, 42, 52, 43, 51,
			    50, 44, 49, 45, 48, 47, 46 };

void	VlpcCableOsaka::CassD0Chan( int board, int& mcm, int chan, int& cass, int& d0chan ) const
{
  cass = d0chan = -1;

  // find the cassette number from the board number

  bool isLHB = true;

  for( int i = 0; i < numCassettes; ++i )
    if( LHB[i] == board || RHB[i] == board )
    {
      cass = cassette[i];
      if( RHB[i] == board )
        isLHB = false;
    }

  // find the D0 channel number on the 128 way connector

  if( isLHB )
    d0chan = left[ afe2_to_afe1[ chan-1 ] ] + 1;
  else
    d0chan = right[ afe2_to_afe1[ chan-1 ] ] + 1;

  // finally, fix the MCM number if it is a right hand board

  if( ! isLHB )
    mcm = 9 - mcm;
}

void	VlpcCableOsaka::WaveConnPos( int cass, int mcm, int d0chan, int& wg, int& con, int& pos ) const
{
  wg = con = pos = -1;

  // find the waveguide number from the cassette and MCM number

  std::string type;

  for( int i = 0; i < numWaveGuides; ++i )
    if( waveCass[i] == cass && waveCassPos[i] == mcm )
    {
      wg = waveGuide[i];
      type = waveType[i];
    }

  if( wg < 0 )
    return;

  // find the MICE connector number and fibre bundle number within the waveguide

  if( type == "old" ) 	// old style connector from the first prototype
  {
    con = miceconnector[d0chan-1] + 1;
    pos = micepos[d0chan-1];
  }
  else if( type == "modified" ) // modified old waveguides to fit new scheme
  {
   con = miceconnector_mod[d0chan-1];
   pos = micepos_mod[d0chan-1];
  }
  else if( type == "new" ) // new waveguide design
  {
    con = miceconnector_new[d0chan-1];
    pos = micepos_new[d0chan-1];
  }

  return;
}

int	VlpcCableOsaka::StatPlanFib( int wg, int con, int pos, 
                                int& stat, int& plan, int& fib, bool rev ) const
{
  stat = plan = fib = -1;

  // find the waveguide type

  std::string type;

  for( int i = 0; i < numWaveGuides; ++i )
    if( waveGuide[i] == wg )
      type = waveType[i];

  // find the station plane and fibre bundle numbers

  int num_bundles;

  if( type == "old" )
    num_bundles = 18;
  else
    num_bundles = 22;

  int statc = -1;

  for( int i = 0; i < numStations; ++i )
  {
    StationCable sc = stations[i];

    for( int j = 0; j < sc.numViews(); ++j )
    {
      ViewCable vc = sc.view(j);

      for( int k = 0; k < vc.numConnectors(); ++k )
        if( vc.cable(k) == con && vc.waveguide(k) == wg ) // found the correct connector
        {
          stat = sc.number();
          plan = vc.number();
          fib = ( vc.connector(k) - 1 ) * num_bundles;

	  statc = vc.connector(k);

          if( ( vc.reversed(k) && ! rev ) || ( ! vc.reversed(k) && rev ) )
            fib += num_bundles + 1 - pos;
          else
            fib += pos;
        }
    }
  }
 
  return statc;
}

void 	VlpcCableOsaka::statPlanFibOsaka( int board, int mcm, int chan, int& stat, int& plan, int& fib ) const
{
  stat = plan = fib = -1;

  for( int c = 0; fib == -1 && c < numChannels; ++c )
    if( osakaBoard[c] == board && osakaMcm[c] == mcm && osakaChannel[c] == chan )
    {
      stat = osakaStation[c];
      plan = osakaPlane[c] - 1;
      fib = osakaFibre[c];
    }

  return;
}

void VlpcCableOsaka::readout2cassette(int afe,
                                         int mcm,
                                         int channel,
                                         int& cassette,
                                         int& module,
                                         int& channelCassette)
    {
      cassette = module = channelCassette = -1;
      return;
    }

void VlpcCableOsaka::readout2waveguide(int afe,
                                          int mcm,
                                          int channel,
                                          int& waveguide,
                                          int& channelWaveguide)
    {   
      waveguide = channelWaveguide = -1;
      return;
    }
  
void VlpcCableOsaka::readout2patchpanel(int afe,
                                           int mcm,
                                           int channel,
                                           int& patchPanelCon, 
                                           int& channelPatchPanel)
    {
      patchPanelCon = channelPatchPanel = -1;
      return;
    }

bool VlpcCableOsaka::readouthaspatchpanel(int afe,
                                             int mcm,
                                             int channel )
    {   
    return false;
    } 

void VlpcCableOsaka::readout2station(int afe,
                                            int mcm,
                                            int channel,
                                            int& stat,
                                            int& statCon,
                                            int& channelStation)
    {
      stat = statCon = channelStation = -1;
      return;
    }

