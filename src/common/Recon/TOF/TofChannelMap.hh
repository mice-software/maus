
#ifndef TOFCHANNELMAP
#define TOFCHANNELMAP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Interface/VmeTdcHit.hh"
#include "Interface/VmefAdcHit.hh"


class TofChannelMap
{
 public :

   TofChannelMap();
   TofChannelMap(std::string);
   //~TofChannelMap();

   void Initialize(std::string);
   void Print();

   bool isThisTofTdc(VmeTdcHit*);
   bool isThisTofFadc(VmefAdcHit*);

   int findThePosition(VmeTdcHit*, int&, int&, int&, int&, std::string&);
   int findThePosition(VmeTdcHit*);

   int findThePosition(VmefAdcHit*, int&, int&, int&, int&, std::string&);
   int findThePosition(VmefAdcHit*);
   int findByName(std::string);

   int GetNumOfChannels() const { return numchans; };
   std::string GetName(int i)   { return pmtName[i]; };

 private :


   int numchans;

   std::vector<int> tdc_boards;
   std::vector<int> tdc_crates;
   std::vector<int> tdc_channels;

   std::vector<int> fadc_boards;
   std::vector<int> fadc_crates;
   std::vector<int> fadc_channels;

   std::vector<int> station;
   std::vector<int> plane;
   std::vector<int> slab;
   std::vector<int> pmt;
   std::vector<int> pmtNum;
   std::vector<std::string> pmtName;
};

#endif

