// VlpcCalib.cc
//
// Class to contain the calibration of the VLPC readout for the SciFi Cosmic Test
//
// M.Ellis 28/08/2005

#include <cmath>

#include "VlpcCalib.hh"

BoardCalib::BoardCalib( std::ifstream& inf )
{
  std::string line;

  // read in the 512 calibrations

  for( int i = 0; i < 512; ++i )
  {
    int board, mcm, chan; //, status;
    double p, p1, pw, g, gainw, chion, chioff, mu;

    getline( inf, line );
    std::istringstream ist1( line.c_str() );
    ist1 >> board >> mcm >> chan >> p >> p1 >> pw >> g >> gainw >> chion >> chioff >> mu;

/*    if( chion > 5. )
    {
      p = 1024;
      g = 1.0;
    }
*/

    m_ped[mcm-1][chan-1] = p;
    m_gain[mcm-1][chan-1] = g;

    if( i == 0 )
      m_board = board;
    else if( m_board != board )
      std::cerr << "Error!!!!! inconsistent board numbers in input file" << std::endl;
  }
}

VlpcCalib::VlpcCalib( std::string file )
{
  // first get the environment variable MICEFILES as this is where the model files are kept

  char* MICEFILES = getenv( "MICEFILES" );

  // the full name of the input file:

  std::string fnam = std::string( MICEFILES ) + "/Calibration/" + file;

  std::ifstream inf( fnam.c_str() );
  std::string line;

  // read in the number of boards

  int numBoards;

  getline( inf, line );
  std::istringstream ist1( line.c_str() );
  ist1 >> numBoards;

  // now read in the board information

  for( int i = 0; i < numBoards; ++i )
  {
    BoardCalib board( inf );
    boards.push_back( board );
  }
}

double	VlpcCalib::pedestal( int board, int mcm, int chan ) const
{
  double ped = -1.0;

  for( unsigned int i = 0; i < boards.size(); ++i )
    if( boards[i].board() == board )
       ped = boards[i].pedestal( mcm-1, chan-1 );

  return ped;   
}

double	VlpcCalib::gain( int board, int mcm, int chan ) const
{
  double gain = -1.0;

  for( unsigned int i = 0; i < boards.size(); ++i )
    if( boards[i].board() == board )
      gain = boards[i].gain( mcm-1, chan-1 );

  return gain;
}

