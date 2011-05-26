// VlpcCableOsaka.hh
//
// Code to read in the connection information to allow the decoding of hit information
// from channel space (AFE board, MCM, channel) into physical space (station, plane, fibre)
//
// M.Ellis 28/8/2005

#ifndef VLPCCABLEOSAKA_HH
#define VLPCCABLEOSAKA_HH

#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "CLHEP/Units/SystemOfUnits.h"
#include "Config/VlpcCable.hh"     

#include "Interface/Memory.hh" 

class ViewCable
{
  public :

    ViewCable( std::ifstream& );

    int			numConnectors() const 		{ return numConn; };
    int			number() const			{ return planeNum; };
    int			connector( int i ) const	{ return connectors[i]; };
    int			waveguide( int i ) const	{ return waveguides[i]; };
    int			cable( int i ) const		{ return cables[i]; };
    bool		reversed( int i ) const		{ return reverse[i]; };

  private :

    int 		numConn;
    int			planeNum;
    std::vector<int>	connectors;
    std::vector<int>	waveguides;
    std::vector<int>	cables;
    std::vector<bool>	reverse;
};

class StationCable
{
  public :

    StationCable( std::ifstream& );

    int			numViews() const		{ return numView; };
    int			number() const			{ return statNum; };
    std::string		type() const			{ return types; };
    ViewCable		view( int i ) const		{ return views[i]; };

  private :

    int				numView;
    int				statNum;
    std::string			types;
    std::vector<ViewCable>	views;
};

class VlpcCableOsaka: public VlpcCable
{
  public :

   VlpcCableOsaka( std::string );

   VlpcCableOsaka( std::string, bool );

   ~VlpcCableOsaka() { miceMemory.addDelete( Memory::VlpcCableOsaka ); };

   int			cassettes() const 			{ return numCassettes; };
   int			cassetteNum( int i ) const		{ return cassette[i]; };
   int			LeftHandBoard( int i ) const		{ return LHB[i]; };
   int			RightHandBoard( int i ) const		{ return RHB[i]; };

   int			waveGuides() const			{ return numWaveGuides; };
   int			waveGuideCassette( int i ) const	{ return waveCass[i]; };
   int			waveGuideCassPos( int i) const		{ return waveCassPos[i]; };
   int			waveGuideNum( int i ) const		{ return waveGuide[i]; };
   std::string		waveGuideType( int i ) const		{ return waveType[i]; };

   int			numstations() const			{ return numStations; };
   StationCable		station( int i ) const			{ return stations[i]; };

   void			statPlanFib( int, int, int, int&, int&, int& );

    void		CassD0Chan( int, int&, int, int&, int& ) const;

    void		WaveConnPos( int, int, int, int&, int&, int& ) const;

    int			StatPlanFib( int, int, int, int&, int&, int&, bool = false ) const;

    void		statPlanFibOsaka( int, int, int, int&, int&, int& ) const;

    bool		isOsaka() const		{ return osaka; };

   std::vector<int>	osakaBoards() const	{ return osakaBoard; };

    void readout2cassette(int afe,
                          int mcm,
                          int channel,
                          int& cassette,
                          int& module,
                          int& channelCassette);

    void readout2waveguide(int afe,
                           int mcm,
                           int channel,
                           int& waveguide,
                           int& channelWaveguide);

    void readout2patchpanel(int afe,
                            int mcm,
                            int channel,
                            int& patchPanelCon,
                            int& channelPatchPanel);

    bool readouthaspatchpanel(int afe,
                            int mcm,
                            int channel );

    void readout2station(int afe,
                         int mcm,
                         int channel,
                         int& stat,
                         int& statCon,
                         int& channelStation);
private :

    int				numCassettes;
    std::vector<int>		cassette;
    std::vector<int>		LHB;
    std::vector<int>		RHB;

    int				numWaveGuides;
    std::vector<int>		waveCass;
    std::vector<int>		waveCassPos;
    std::vector<int>		waveGuide;
    std::vector<std::string>	waveType;

    int				numStations;
    std::vector<StationCable>	stations;

    bool 			osaka;

    int				numChannels;
    std::vector<int>		osakaBoard;
    std::vector<int>		osakaMcm;
    std::vector<int>		osakaChannel;
    std::vector<int>		osakaStation;
    std::vector<int>		osakaPlane;
    std::vector<int>		osakaFibre;
};

#endif

