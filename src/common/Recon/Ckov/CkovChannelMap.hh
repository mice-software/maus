// V. Verguilov Oct 2008
#ifndef CKOVCHANNELMAP
#define CKOVCHANNELMAP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Interface/VmeTdcHit.hh"
#include "Interface/VmefAdcHit.hh"


class CkovChannelMap
{
 public :
 
   CkovChannelMap();
   CkovChannelMap( std::string );
   virtual ~CkovChannelMap(){};
   
   void Initialize(std::string);
   void Print();
   
   bool isThisCkovTdc( VmeTdcHit* );
   bool isThisCkovFadc( VmefAdcHit* );
      
   int findThePosition( VmeTdcHit* theHit, int&, int& );
   int findThePosition( VmeTdcHit* theHit);
    
   int findThePosition( VmefAdcHit* theHit, int& plane, int& pmt );
   int findThePosition( VmefAdcHit* theHit );
        
   int GetNumOfChannels() const { return m_numchans; };
   
 private :
   void Create();
 
   int m_numchans;
    
   std::vector<int> m_tdc_boards;
   std::vector<int> m_tdc_crates;
   std::vector<int> m_tdc_channels;
   
   std::vector<int> m_fadc_boards;
   std::vector<int> m_fadc_crates;
   std::vector<int> m_fadc_channels;
   std::vector<int> m_plane;
   std::vector<int> m_pmt;
   std::vector<int> m_pmtNum;
   std::vector<std::string> m_pmtName;   
};

#endif

