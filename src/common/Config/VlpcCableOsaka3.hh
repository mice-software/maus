// VlpcCableOsaka3.hh
//
// Code to read in decoding format from Hideyuki for debugging tracker 2 readout
//
// M.Ellis December 2009

#ifndef VLPCCABLEOSAKA3_HH
#define VLPCCABLEOSAKA3_HH

#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "CLHEP/Units/SystemOfUnits.h"
#include "Config/VlpcCable.hh"     
#include "Config/VlpcCableOsaka2.hh"

class VlpcCableOsaka3: public VlpcCable
{
  public :

   VlpcCableOsaka3( std::string );

   ~VlpcCableOsaka3() { };

   void			statPlanFib( int, int, int, int&, int&, int& );

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

    int cassettes() const;

    int cassetteNum( int ) const;

    int LeftHandBoard( int ) const;

    int RightHandBoard( int ) const;

private :

    std::vector<int>	_cryo;
    std::vector<int>	_cass;
    std::vector<int>	_board;
    std::vector<int>	_afe;
    std::vector<int>	_mcm;
    std::vector<int>	_chan;
    std::vector<int>	_tracker;
    std::vector<int>	_station;
    std::vector<int>	_view;
    std::vector<int>	_fibre;
    std::vector<int>	_extWG;
    std::vector<int>	_intWG;
    std::vector<int>	_WGfib;

    int			_afe_map[2][2][2];
};

#endif

