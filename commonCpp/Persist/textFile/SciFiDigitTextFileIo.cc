// SciFiDigitTextFileIo.cc
//
// M.Ellis 22/8/2005

#include <sstream>

#include "SciFiDigitTextFileIo.hh"
#include "SciFiHitTextFileIo.hh"

SciFiDigitTextFileIo::SciFiDigitTextFileIo( SciFiDigit* digit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiDigitTextFileIo );
	
  setTxtIoIndex( index );
  m_digit = digit;
}

SciFiDigitTextFileIo::SciFiDigitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiDigitTextFileIo );
  
  restoreObject( def );
}

std::string	SciFiDigitTextFileIo::storeObject()
{
  trackerNo = m_digit->getTrackerNo();
  stationNo = m_digit->getStationNo();
  planeNo = m_digit->getPlaneNo();
  fiberNo = m_digit->getChanNo();
  adcCounts = m_digit->getAdcCounts();
  tdcCounts = m_digit->getTdcCounts();
  used = m_digit->isUsed();
  m_mc_hit = m_digit->mcHit();
  m_pe = m_digit->getNPE();

  mchit_index = -1;

  for( unsigned int i = 0; i < m_mchits->size(); ++i )
    if( (*m_mchits)[i]->theHit() == m_mc_hit )
      mchit_index = (*m_mchits)[i]->txtIoIndex();

  std::stringstream ss;

  ss << txtIoIndex() << " " << trackerNo << " " << stationNo << " " << planeNo << " " << fiberNo << " " << adcCounts << " " << tdcCounts << " " << used << " " << m_pe << " " << mchit_index;

  return( ss.str() );
}

void		SciFiDigitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> trackerNo >> stationNo >> planeNo >> fiberNo 
      >> adcCounts >> tdcCounts >> used >> m_pe >> mchit_index;

  setTxtIoIndex( myindex );
}

SciFiDigit*	SciFiDigitTextFileIo::makeSciFiDigit()
{
  m_digit = new SciFiDigit();

  m_digit->setTrackerNo( trackerNo );
  m_digit->setStationNo( stationNo );
  m_digit->setPlaneNo( planeNo );
  m_digit->setChanNo( fiberNo );
  m_digit->setAdcCounts( adcCounts );
  m_digit->setTdcCounts( tdcCounts );
  m_digit->setUsed( used );
  m_digit->setPhotoElectrons( m_pe );

  return m_digit;
}

void		SciFiDigitTextFileIo::setSciFiHits( std::vector<SciFiHitTextFileIo*>& mchits )
{
  m_mchits = & mchits;
}

void		SciFiDigitTextFileIo::completeRestoration()
{
  m_digit->setMcHit( NULL );

  for( unsigned int i = 0; i < m_mchits->size(); ++i )
    if( (*m_mchits)[i]->txtIoIndex() == mchit_index )
      m_digit->setMcHit( (*m_mchits)[i]->theHit() );
}
