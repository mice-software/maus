// VlpcCalib.hh
//
// Class to contain the calibration of the VLPC readout for the SciFi tracker
//
// M.Ellis 28/8/2003

#ifndef VLPCCALIB_H
#define VLPCCALIB_H

#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

class BoardCalib
{
  public :

    BoardCalib( std::ifstream& );

    int			board() const 				{ return m_board; };
    double		pedestal( int mcm, int chan ) const	{ return m_ped[mcm][chan]; };
    double		gain( int mcm, int chan ) const		{ return m_gain[mcm][chan]; };

  private :

    int		m_board;
    double	m_ped[8][64];
    double	m_gain[8][64];
};

class VlpcCalib
{
  public :

    VlpcCalib( std::string );

    unsigned int 	numBoards() const			{ return boards.size(); };
    int			boardNumber( int i ) const 		{ return boards[i].board(); };
    double		pedestal( int, int, int ) const;
    double		gain( int, int, int ) const;

  private :

    std::vector<BoardCalib>	boards;
};

#endif
