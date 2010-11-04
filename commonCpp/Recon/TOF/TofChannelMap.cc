#include "TofChannelMap.hh"

TofChannelMap::TofChannelMap()
{
   fadc_boards.resize(0);
   fadc_crates.resize(0);
   fadc_channels.resize(0);
   tdc_boards.resize(0);
   tdc_crates.resize(0);
   tdc_channels.resize(0);

   station.resize(0);
   plane.resize(0);
   slab.resize(0);
   pmt.resize(0 );
   pmtNum.resize(0);
   pmtName.resize(0);
   numchans = 0;
}

TofChannelMap::TofChannelMap(std::string fileName)
{
   fadc_boards.resize(0);
   fadc_crates.resize(0);
   fadc_channels.resize(0);
   tdc_boards.resize(0);
   tdc_crates.resize(0);
   tdc_channels.resize(0);

   station.resize(0);
   plane.resize(0);
   slab.resize(0);
   pmt.resize(0 );
   pmtNum.resize(0);
   pmtName.resize(0);
   numchans = 0;
   Initialize(fileName);
}

void TofChannelMap::Initialize(std::string fileName)
{
   std::ifstream inf( fileName.c_str() );

   if( ! inf )
   {
      std::cerr << "Can't open cablilng file " << fileName << std::endl;
      exit(1);
   }

   std::string lbuf = "";
   do { std::getline( inf , lbuf);}
   while (lbuf[0] == '#');
   std::istringstream ss;
   ss.str(lbuf);
   ss >> numchans;

   fadc_boards.resize( numchans );
   fadc_crates.resize( numchans );
   fadc_channels.resize( numchans );
   tdc_boards.resize( numchans );
   tdc_crates.resize( numchans );
   tdc_channels.resize( numchans );

   station.resize( numchans );
   plane.resize( numchans );
   slab.resize( numchans );
   pmt.resize( numchans );
   pmtNum.resize( numchans );
   pmtName.resize( numchans );

   ss.clear();
   int chan = 0;
   while( chan < numchans )
   {
      std::getline(inf, lbuf); 
      if(lbuf[0] == '#') continue;

      int fadc_bd, fadc_crate=0, fadc_ch, tdc_bd, tdc_crate, tdc_ch, st , pl, sl, p, pNum;
      std::string pName;

      ss.str( lbuf );  
      ss >> fadc_crate >> fadc_bd >>  fadc_ch >> tdc_crate >> tdc_bd >>  tdc_ch >> st >> pl >> sl >> p >> pNum >> pName;
      ss.clear(); 

      fadc_boards[chan] = fadc_bd;
      fadc_crates[chan] = fadc_crate;
      fadc_channels[chan] = fadc_ch;
      tdc_boards[chan] = tdc_bd;
      tdc_crates[chan] = tdc_crate;
      tdc_channels[chan] = tdc_ch;
      station[chan] = st;
      plane[chan] = pl;
      slab[chan] = sl;
      pmt[chan] = p;
      pmtNum[chan] = pNum;
      pmtName[chan] = pName; 
      //std::cout<<pName<<std::endl; 
      chan ++;
   }

   inf.close();
}

bool TofChannelMap::isThisTofTdc(VmeTdcHit* theHit)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();
   for( int j = 0; j < numchans; j++ )
      if( tdc_crates[j] == crate 
          && tdc_boards[j] == board 
          && tdc_channels[j] == chan) 
      {
          return true;
      }

      return false;

}

bool TofChannelMap::isThisTofFadc(VmefAdcHit* theHit)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();
   for( int j = 0; j < numchans; j++ )
      if( fadc_crates[j] == crate 
          && fadc_boards[j] == board 
          && fadc_channels[j] == chan) 
      {
          return true;
      }

      return false;

}
int TofChannelMap::findThePosition(VmeTdcHit* theHit, int& st, int& pl, int& sl, int& p, std::string& n)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();

   for( int j = 0; j < numchans; j++ )
      if( tdc_crates[j] == crate
          && tdc_boards[j] == board
          && tdc_channels[j] == chan)
      {
         st = station[j];
         pl = plane[j];
         sl = slab[j];
         p = pmt[j];
         n = pmtName[j];

         return pmtNum[j];
          //return j;
      }

      return -99;

}

int TofChannelMap::findThePosition(VmeTdcHit* theHit)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();

   for( int j = 0; j < numchans; j++ )
      if( tdc_crates[j] == crate 
          && tdc_boards[j] == board 
          && tdc_channels[j] == chan)
      {
          return pmtNum[j];
          //return j;
      }

      return -99;

}

int TofChannelMap::findThePosition(VmefAdcHit* theHit, int& st, int& pl, int& sl, int& p, std::string& n)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();

   for( int j = 0; j < numchans; j++ )
      if( fadc_crates[j] == crate 
          && fadc_boards[j] == board 
          && fadc_channels[j] == chan) 
      {
         st = station[j];
         pl = plane[j];
         sl = slab[j];
         p = pmt[j];
         n = pmtName[j];

         return pmtNum[j];
         //return j;
      }

      return -99;
 
}

int TofChannelMap::findThePosition(VmefAdcHit* theHit)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();

   for( int j = 0; j < numchans; j++ )
      if( fadc_crates[j] == crate 
          && fadc_boards[j] == board 
          && fadc_channels[j] == chan)
      {
          return pmtNum[j];
          //return j;
      }

      return -99;

}

int TofChannelMap::findByName(std::string name)
{
    for( int j = 0; j < numchans; j++ )
    if(name == pmtName[j])
        return j;

    return -99;
}

void TofChannelMap::Print()
{
   std::cout<<"===== TofChannelMap ====="<<std::endl;
   for( int j = 0; j < numchans; j++)
   {
      std::cout<<"Pmt "<<pmtName[j]<<" "<<pmtNum[j]<<" fADC "<<fadc_crates[j]<<fadc_boards[j]<<fadc_channels[j];
      std::cout<<" TDC "<<tdc_crates[j]<<tdc_boards[j]<<tdc_channels[j];
      std::cout<<" TOF "<<station[j]<<plane[j]<<slab[j]<<pmt[j]<<std::endl;
   }
}
