// TofSlabHitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "TofSlabHitTextFileIo.hh"
#include "TofDigitTextFileIo.hh"
#include "VmeAdcHitTextFileIo.hh"
#include "VmeTdcHitTextFileIo.hh"


TofSlabHitTextFileIo::TofSlabHitTextFileIo( TofSlabHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::TofSlabHitTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

TofSlabHitTextFileIo::TofSlabHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::TofSlabHitTextFileIo );
  
  restoreObject( def );
}

std::string	TofSlabHitTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!

    m_station = m_hit->station();
    m_plane = m_hit->plane();
    m_slabNumber = m_hit->slabNumber();

	r_time = m_hit->RawTime();
	c_time = m_hit->CorrectedTime();

    m_digit1 = m_hit->digit1();
    m_digit2 = m_hit->digit2();


	if(m_digit1)
		for(unsigned int i = 0; i< m_tofDigits->size(); i++){
			if((*m_tofDigits)[i]->theDigit() == m_digit1)
				m_digit1_index = (*m_tofDigits)[i]->txtIoIndex();
    }

	if(m_digit2)
		for(unsigned int i = 0; i< m_tofDigits->size(); i++){
			if((*m_tofDigits)[i]->theDigit() == m_digit2)
				m_digit2_index = (*m_tofDigits)[i]->txtIoIndex();
    }

  std::stringstream ss;

  ss << txtIoIndex() << " " << r_time << " " << c_time << " " <<
  m_station << " " << m_plane << " " << m_slabNumber << " " << m_digit1_index << " " << m_digit2_index;

  return( ss.str() );
}

void		TofSlabHitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> r_time >> c_time >> m_station >> m_plane >> m_slabNumber >> m_digit1_index >>  m_digit2_index;

  setTxtIoIndex( myindex );
}

TofSlabHit*	TofSlabHitTextFileIo::makeTofSlabHit()
{
	m_hit = new TofSlabHit();
	m_hit->SetStation(m_station);
	m_hit->SetPlane(m_plane);
	m_hit->SetSlab(m_slabNumber);
	m_hit->SetRawTime(r_time);
	m_hit->SetCorrectedTime(c_time);
	m_hit->SetGood(true);

  return m_hit;
}

void		TofSlabHitTextFileIo::completeRestoration()
{
	m_digit1 = NULL;
	m_digit2 = NULL;

  	for(unsigned int i = 0; i < m_tofDigits->size(); i++){
    	if((*m_tofDigits)[i]->txtIoIndex() == m_digit1_index){
			m_digit1 = (*m_tofDigits)[i]->theDigit();
    	}
	}

  	for(unsigned int i = 0; i < m_tofDigits->size(); i++){
    	if((*m_tofDigits)[i]->txtIoIndex() == m_digit2_index){
			m_digit2 = (*m_tofDigits)[i]->theDigit();
    	}
	}

	m_hit->SetDigit1( m_digit1 );
	m_hit->SetDigit2( m_digit2 );
}
