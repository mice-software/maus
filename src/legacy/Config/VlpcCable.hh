// MAUS WARNING: THIS IS LEGACY CODE.
// VlpcCable.hh
//
// Code to read in the connection information to allow the decoding of hit information
// from channel space (AFE board, MCM, channel) into physical space (station, plane, fibre)
//
// M.Ellis 28/8/2005

#ifndef VLPCCABLE_HH
#define VLPCCABLE_HH

#include <cstdlib>

class VlpcCable
{
  public:
 
   virtual ~VlpcCable() {};
 
   virtual void statPlanFib(int afe,
                            int mcm,
                            int channel,
                            int& station, 
                            int& plane,
                            int& fibre) = 0;
   
   virtual void readout2cassette(int afe,
                                 int mcm,
                                 int channel,
                                 int& cassette,
                                 int& module,
                                 int& channelCassette) = 0;

    virtual void readout2waveguide(int afe,
                                   int mcm,
                                   int channel,
                                   int& waveguide,
                                   int& channelWaveguide) = 0;

    virtual void readout2patchpanel(int afe,
                                    int mcm,
                                    int channel,
                                    int& patchPanelCon,
                                    int& channelPatchPanel) = 0;

    virtual bool readouthaspatchpanel(int afe,
                                      int mcm,
                                      int channel ) = 0;

    virtual void readout2station(int afe,
                                 int mcm,
                                 int channel,
                                 int& stat,
                                 int& statCon,
                                 int& channelStation) = 0;

   virtual int cassettes() const = 0;
   virtual int cassetteNum( int ) const = 0;
   virtual int LeftHandBoard( int ) const = 0;
   virtual int RightHandBoard( int ) const = 0;

};

#endif
