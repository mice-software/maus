// VlpcCableOsaka3.cc
//
// Code to read in decoding format from Hideyuki for debugging tracker 2 readout
//
// M.Ellis October 2009

#include "Config/VlpcCableOsaka3.hh"     

VlpcCableOsaka3::VlpcCableOsaka3( std::string fname )
{
  char* MICEFILES = getenv( "MICEFILES" );

  // the full name of the input file:
  
  std::string fnam = std::string( MICEFILES ) + "/Cabling/" + fname; 

  std::ifstream inf( fnam.c_str() );

  if( ! inf )
  {
    std::cerr << "Unable to open the file " << fname << std::endl;
  }
  else
  {
    // some conversions that are required

    _afe_map[0][1][0] = 574062;
    _afe_map[0][1][1] = 574067;
    _afe_map[0][0][0] = 574296;
    _afe_map[0][0][1] = 574375;
    _afe_map[1][1][0] = 574218;
    _afe_map[1][1][1] = 574127;
    _afe_map[1][0][0] = 574256;
    _afe_map[1][0][1] = 574301;

    // read in the file and convert the output as required

    while ( inf )
    {
      int cryo, cass, afe, mcm, chan, tracker, station, view, fibre, extWG, intWG, WGfib;
      inf >> cryo >> cass >> afe >> mcm >> chan >> tracker >> station >> view >> fibre >> extWG >> intWG >> WGfib;
      _cryo.push_back( cryo );
      _cass.push_back( cass );
      _board.push_back( afe );
      _afe.push_back( _afe_map[ cryo - 1 ][ cass - 1 ][ afe - 1 ] );
      _mcm.push_back( mcm );
      _chan.push_back( chan );
      _tracker.push_back( tracker );
      _station.push_back( station );
      _view.push_back( view - 1 );
      _fibre.push_back( fibre );
      _extWG.push_back( extWG );
      _intWG.push_back( intWG );
      _WGfib.push_back( WGfib );
    }
  }
}

void	VlpcCableOsaka3::statPlanFib( int afe, int mcm, int chan, int& stat, int& plan, int& fib )
{
  bool found = false;
  stat = plan = fib = -1;

  for( unsigned int i = 0; ! found && i < _cryo.size(); ++i )
  {
    if( ( afe == _afe[i] ) && ( mcm == _mcm[i] ) && ( chan == _chan[i] ) )
    {
      stat = _station[i];
      plan = _view[i];
      fib = _fibre[i];
      found = true;
    }
  } 
}

void 	VlpcCableOsaka3::readout2cassette(int afe,
                          int mcm,
                          int channel,
                          int& cassette,
                          int& module,
                          int& channelCassette)
{
  cassette = module = channelCassette = -1;
}

void 	VlpcCableOsaka3::readout2waveguide(int afe,
                           int mcm,
                           int channel,
                           int& waveguide,
                           int& channelWaveguide)
{
  waveguide = channelWaveguide = -1;
}

void 	VlpcCableOsaka3::readout2patchpanel(int afe,
                            int mcm,
                            int channel,
                            int& patchPanelCon,
                            int& channelPatchPanel)
{
  patchPanelCon = channelPatchPanel = -1;
}

bool 	VlpcCableOsaka3::readouthaspatchpanel(int afe,
                            int mcm,
                            int channel )
{
  return true;
}

void 	VlpcCableOsaka3::readout2station(int afe,
                         int mcm,
                         int channel,
                         int& stat,
                         int& statCon,
                         int& channelStation)
{
  stat = statCon = channelStation = -1;
}

int 	VlpcCableOsaka3::cassettes() const
{
  return 4;
}

int 	VlpcCableOsaka3::cassetteNum( int ) const
{
  return -1;
}

int 	VlpcCableOsaka3::LeftHandBoard( int ) const
{
  return -1;
}

int 	VlpcCableOsaka3::RightHandBoard( int ) const
{
  return -1;
}

