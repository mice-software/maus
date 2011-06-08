// SciFiBadChans.hh
//
// M.Ellis 25/9/2005
//
// Code to hold a list of bad channels to be used during reconstruction to mask them from use

#ifndef RECON_SCIFIBADCHANS_H
#define RECON_SCIFIBADCHANS_H

#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

class SciFiBadChans
{
  public :

    SciFiBadChans();

    SciFiBadChans( std::string );

    bool	goodChannel( int, int, int, int ) const;

  private :

    bool m_good[2][5][3][1540];
};

#endif

