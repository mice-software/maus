#include "Recon/Ckov/CkovChannelMap.hh"



CkovChannelMap::CkovChannelMap()
{
   Create();
}


CkovChannelMap::CkovChannelMap(std::string fileName)
{
	Create();
	Initialize(fileName);
}

void CkovChannelMap::Create()
{
   m_fadc_boards.resize( 0 );
   m_fadc_crates.resize( 0 );
   m_fadc_channels.resize( 0 );
   m_tdc_boards.resize( 0 );
   m_tdc_crates.resize( 0 );
   m_tdc_channels.resize( 0 );
   m_plane.resize( 0 );
   m_pmt.resize( 0 );
   m_pmtNum.resize( 0 );
   m_pmtName.resize( 0 );
   m_numchans = 0;
}

void CkovChannelMap::Initialize(std::string fileName)
{
   std::ifstream inf( fileName.c_str() );

   if( ! inf ){
      std::cerr << "Can't open cablilng file " << fileName << std::endl;
      exit( 1 );
   }
 
   std::string lbuf = "";
   do 
   { 
	   std::getline( inf , lbuf);
   }while ( lbuf[0] == '#' );

   std::istringstream ss;
   ss.str( lbuf );    
   ss >> m_numchans;

   /// FDCs
   m_fadc_boards.resize( m_numchans );
   m_fadc_crates.resize( m_numchans );
   m_fadc_channels.resize( m_numchans );

   /// TDCs
   m_tdc_boards.resize( m_numchans );
   m_tdc_crates.resize( m_numchans );
   m_tdc_channels.resize( m_numchans );   

   /// CKOV
   m_plane.resize( m_numchans );
   m_pmt.resize( m_numchans );
   m_pmtNum.resize( m_numchans );
   m_pmtName.resize( m_numchans );

   ss.clear();
   int chan = 0;
   while( chan < m_numchans )
   {
      std::getline( inf, lbuf ); 
      if( lbuf[ 0 ] == '#') continue;

      int fadc_bd, fadc_crate = 0, fadc_ch, tdc_bd, tdc_crate, tdc_ch, pl, p, pNum;

	  std::string pName;

      ss.str( lbuf );
      ss >> fadc_crate >> fadc_bd >>  fadc_ch >> tdc_crate >> tdc_bd >> tdc_ch >> pl >> p >> pNum >> pName;
      ss.clear(); 

      m_fadc_boards[ chan ] = fadc_bd;
      m_fadc_crates[ chan ] = fadc_crate;
      m_fadc_channels[ chan ] = fadc_ch;

      m_tdc_boards[ chan ] = tdc_bd;
      m_tdc_crates[ chan ] = tdc_crate;
      m_tdc_channels[ chan ] = tdc_ch;

	  m_plane[ chan ] = pl;
      m_pmt[ chan ] = p;
      m_pmtNum[ chan ] = pNum;
      m_pmtName[ chan ] = pName; 
      chan++;   
   }

   inf.close();
}

bool CkovChannelMap::isThisCkovTdc(VmeTdcHit* theHit)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();

   for( int j = 0; j < m_numchans; ++j )
      if( ( m_tdc_crates[j] == crate ) && ( m_tdc_boards[j] == board ) &&  ( m_tdc_channels[j] == chan ) ) 
          return true;
   return false;
}

bool CkovChannelMap::isThisCkovFadc(VmefAdcHit* theHit)
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();
   for( int j = 0; j < m_numchans; j++ )
      if( ( m_fadc_crates[ j ] == crate ) && ( m_fadc_boards[ j ] == board ) && ( m_fadc_channels[ j ] == chan ) ) 
          return true;
   return false;
}

int CkovChannelMap::findThePosition( VmeTdcHit* theHit, int& plane, int& pmt )
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();
   for( int j = 0; j < m_numchans; ++j )
      if( ( m_tdc_crates[ j ] == crate ) && ( m_tdc_boards[ j ] == board ) && ( m_tdc_channels[ j ] == chan ) ) 
      {
         plane = m_plane [ j ];
         pmt = m_pmt[ j ];
         return m_pmtNum[ j ];
      }
   return -99;
}

int CkovChannelMap::findThePosition( VmeTdcHit* theHit )
{
	int crate = theHit->crate();
	int board = theHit->board();
	int chan = theHit->channel();
 
	for( int j = 0; j < m_numchans; ++j )
		if( ( m_tdc_crates[ j ] == crate ) && ( m_tdc_boards[ j ] == board ) && ( m_tdc_channels[ j ] == chan ) )
			return m_pmtNum[ j ];
	return -99;
}

int CkovChannelMap::findThePosition( VmefAdcHit* theHit, int& plane, int& pmt )
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();
   
   for( int j = 0; j < m_numchans; j++ )
      if( ( m_fadc_crates[j] == crate ) && ( m_fadc_boards[j] == board ) && ( m_fadc_channels[j] == chan ) ) 
      {
         plane = m_plane[j];
         pmt = m_pmt[j];
         return m_pmtNum[j];
	 //return j;
      }
      return -99;
	  
}

int CkovChannelMap::findThePosition( VmefAdcHit* theHit )
{
   int crate = theHit->crate();
   int board = theHit->board();
   int chan = theHit->channel();
   
   for( int j = 0; j < m_numchans; j++ )
      if( m_fadc_crates[j] == crate 
	  && m_fadc_boards[j] == board 
	  && m_fadc_channels[j] == chan)
          return m_pmtNum[j];
   return -99; 
}

void CkovChannelMap::Print()
{
   std::cout<<"===== CkovChannelMap ====="<<std::endl;
   for( int j = 0; j < m_numchans; j++)
   {
      std::cout << "Pmt " << m_pmtName[ j ] << " " << m_pmtNum[ j ] << " fADC " << m_fadc_crates[ j ] << m_fadc_boards[ j ] << m_fadc_channels[ j ];
      std::cout << " TDC " << m_tdc_crates[ j ] << m_tdc_boards[ j ] << m_tdc_channels[ j ];
      std::cout << " CKOV " << m_plane[ j ] << m_pmt[ j ] << std::endl;
   }
}
