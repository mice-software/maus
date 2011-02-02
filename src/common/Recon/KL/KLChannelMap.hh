
#ifndef KLCHANNELMAP
#define KLCHANNELMAP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Interface/VmefAdcHit.hh"


class KLChannelMap
{
 public :
 
   KLChannelMap();
   KLChannelMap(std::string);
   
   void Initialize(std::string);
   void Print();
   
   bool isThisKLFadc(VmefAdcHit*);
      
   int findThePosition(VmefAdcHit*, int&, int&);
   int findThePosition(VmefAdcHit*);
        
   int GetNumOfChannels() const { return numchans; };
   std::string GetName(int i)   { return pmtName[i]; };
   
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

