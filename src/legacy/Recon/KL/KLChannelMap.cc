#include "KLChannelMap.hh"

KLChannelMap::KLChannelMap()
{
   fadc_boards.resize(0);
   fadc_crates.resize(0);
   fadc_channels.resize(0);

   cell.resize(0);
   pmt.resize(0 );
   pmtNum.resize(0);
   pmtName.resize(0);
   numchans = 0;
}

KLChannelMap::KLChannelMap(std::string fileName)
{
   fadc_boards.resize(0);
   fadc_crates.resize(0);
   fadc_channels.resize(0);
   
   cell.resize(0);
   pmt.resize(0 );
   pmtNum.resize(0);
   pmtName.resize(0);
   numchans = 0;   
   Initialize(fileName);
}

void KLChannelMap::Initialize(std::string fileName)
{
   std::ifstream inf( fileName.c_str() );

   if( ! inf )
   {
      std::cerr << "Can't open cabling file " << fileName << std::endl;
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
   
   cell.resize( numchans );
   pmt.resize( numchans );
   pmtNum.resize( numchans );
   pmtName.resize( numchans );
     
   ss.clear();
   int chan = 0;
   while( chan < numchans )
   {
      std::getline(inf, lbuf); 
      if(lbuf[0] == '#') continue;
      
      int fadc_bd, fadc_crate, fadc_ch, cel, p, pNum;
      std::string pName;

      ss.str( lbuf );  
      ss >> fadc_crate >> fadc_bd >>  fadc_ch >> cel >> p >> pNum >> pName;
      ss.clear(); 
           
      fadc_boards[chan] = fadc_bd;
      fadc_crates[chan] = fadc_crate;
      fadc_channels[chan] = fadc_ch;
      cell[chan] = cel;
      pmt[chan] = p;
      pmtNum[chan] = pNum;
      pmtName[chan] = pName; 
      //std::cout<<pName<<std::endl; 
      chan ++;   
   }

   inf.close();
}

bool KLChannelMap::isThisKLFadc(VmefAdcHit* theHit)
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

int KLChannelMap::findThePosition(VmefAdcHit* theHit, int& cel, int& p)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();
   
   for( int j = 0; j < numchans; j++ )
      if( fadc_crates[j] == crate 
	  && fadc_boards[j] == board 
	  && fadc_channels[j] == chan) 
      {
         cel = cell[j];
         p = pmt[j];
	  
         return pmtNum[j];
      }
       
      return -99;
	  
}

int KLChannelMap::findThePosition(VmefAdcHit* theHit)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();
   
   for( int j = 0; j < numchans; j++ )
      if( fadc_crates[j] == crate 
	  && fadc_boards[j] == board 
	  && fadc_channels[j] == chan)
	  
          return pmtNum[j];	  
       
      return -99;
	  
}

void KLChannelMap::Print()
{
   std::cout<<"===== KLChannelMap ====="<<std::endl;
   for( int j = 0; j < numchans; j++)
   {
      std::cout << "Pmt " << pmtName[j] << " " << pmtNum[j] << " fADC" 
	   << fadc_crates[j] << fadc_boards[j] << fadc_channels[j];
      std::cout << " KL " << cell[j] << pmt[j] << std::endl;
   }
}
