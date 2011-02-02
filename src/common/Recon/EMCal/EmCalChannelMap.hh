
#ifndef EMCALCHANNELMAP
#define EMCALCHANNELMAP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Interface/VmefAdcHit.hh"


class EmCalChannelMap
{
 public :
 
   EmCalChannelMap();
   EmCalChannelMap(std::string);
   //~TofChannelMap();
   
   void Initialize(std::string);
   void Print();
   
   bool isThisEmCalFadc(VmefAdcHit*);
      
   int findThePosition(VmefAdcHit*, int&, int&);
   int findThePosition(VmefAdcHit*);
        
   int GetNumOfChannels() const { return numchans; };
   
 private :
 
 
   int numchans;
   
   std::vector<int> fadc_boards;
   std::vector<int> fadc_crates;
   std::vector<int> fadc_channels;
      
   std::vector<int> cell;
   std::vector<int> pmt;
   std::vector<int> pmtNum;
   std::vector<std::string> pmtName;   
};

#endif

