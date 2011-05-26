// SciFiBadChans.hh
//
// M.Ellis 25/9/2005
//
// Code to hold a list of bad channels to be used during reconstruction to mask them from use

#include "Recon/SciFi/SciFiBadChans.hh"

SciFiBadChans::SciFiBadChans()
{
  // use all channels

  for( int tr = 0; tr < 2; ++tr )
    for( int st = 0; st < 5; ++st )
      for( int pl = 0; pl < 3; ++pl )
        for( int fib = 0; fib < 1540; ++fib )
          m_good[tr][st][pl][fib] = true;
}

SciFiBadChans::SciFiBadChans( std::string filename )
{
  // by default, we use all channels

  for( int tr = 0; tr < 2; ++tr )
    for( int st = 0; st < 5; ++st )
      for( int pl = 0; pl < 3; ++pl )
        for( int fib = 0; fib < 1540; ++fib )
          m_good[tr][st][pl][fib] = true;

  // now open file which contains the list of bad channels

  std::ifstream inf( filename.c_str() );

  // read in the number of bad channels

  int numBad;

  inf >> numBad;

  for( int i = 0; i < numBad; ++i )
  {
    int tr, st, pl, fib;

    inf >> tr >> st >> pl >> fib;

    m_good[tr][st][pl][fib] = false;
  }
  
}

bool	SciFiBadChans::goodChannel( int tr, int st, int pl, int fib ) const
{
  return m_good[tr][st][pl][fib];
}

