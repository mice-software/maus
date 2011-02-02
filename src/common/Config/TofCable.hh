// TofCable.hh
//
// Code to read in the connection information to allow the decoding of hit information
// from channel space (ADC & TDC, Channel) into physical space (station, plane, slab, pmt)
//
//Based on M.Ellis VlpcCable.hh
//
// A.Fish 2nd October 2005

#ifndef TOFCABLE_HH
#define TOFCABLE_HH

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

#include "Interface/Memory.hh" 

class TofCable
{
  public :

    TofCable( std::string );
    ~TofCable() 	{ miceMemory.addDelete( Memory::TofCable ); };
    
    int			variables() const 		{ return numVariables; };

    int			modType      ( int i ) const	{ return moduleType[i]; };
    int			crateNumber  ( int i ) const	{ return crateNum[i]; };
    int			moduleNumber ( int i ) const	{ return moduleNum[i]; };
    int			channel      ( int i ) const	{ return chan[i]; };
    int			stationNumber( int i ) const	{ return stationNum[i]; };
    int			planeNumber  ( int i ) const	{ return planeNum[i]; };
    int			slab         ( int i ) const	{ return slabNum[i]; };
    int			pmt          ( int i ) const	{ return pmtNum[i]; };
 
  private :

    int 		numVariables;

    std::vector<int>	moduleType;
    std::vector<int>	crateNum;
    std::vector<int>	moduleNum;
    std::vector<int>	chan;
    std::vector<int>	stationNum;
    std::vector<int>	planeNum;
    std::vector<int>	slabNum;
    std::vector<int>	pmtNum;
    
}; 

#endif

