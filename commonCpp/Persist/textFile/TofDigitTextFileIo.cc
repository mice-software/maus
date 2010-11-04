// TofDigitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "TofDigitTextFileIo.hh"
#include "TofSlabHitTextFileIo.hh"
#include "VmeAdcHitTextFileIo.hh"
#include "VmefAdcHitTextFileIo.hh"
#include "VmeTdcHitTextFileIo.hh"


TofDigitTextFileIo::TofDigitTextFileIo( TofDigit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::TofDigitTextFileIo );
	
  setTxtIoIndex( index );
  m_digit = hit;
}

TofDigitTextFileIo::TofDigitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::TofDigitTextFileIo );
  
  restoreObject( def );
}

std::string	TofDigitTextFileIo::storeObject()
{
  //get the hits from the digit object

  m_tofHit = m_digit->tofHit();
  m_adc = m_digit->vmeAdcHit();
  m_fadc = m_digit->vmefAdcHit();  
  m_tdc = m_digit->vmeTdcHit();
  m_slabHit = m_digit->slabHit();


		//set the data that we are going to write (variables)
  m_stationNumber = m_digit->stationNumber();
  m_planeNumber = m_digit->planeNumber();
  m_good = m_digit->isGood();
  m_pmt = m_digit->pmt();
  m_adc_index = -1;
  m_tdc_index = -1;
  m_slab_index = -1;
  m_slab = m_digit->slab();
  _pe = m_digit->pe();
  _time = m_digit->time();
  _adc = m_digit->adc();
  _tdc = m_digit->tdc();


  //now set the pointers that we are going to write as indices
  if( m_adc )
    for(unsigned int i = 0; i<m_vmeAdcHits->size(); i++)
      if((*m_vmeAdcHits)[i]->theHit() == m_adc)
	      m_adc_index = (*m_vmeAdcHits)[i]->txtIoIndex();

  if( m_fadc )
    for(unsigned int i = 0; i<m_vmefAdcHits->size(); i++)
      if((*m_vmefAdcHits)[i]->theHit() == m_fadc)
	      m_fadc_index = (*m_vmefAdcHits)[i]->txtIoIndex();
	      
  if( m_tdc )
    for(unsigned int i = 0; i<m_vmeTdcHits->size(); i++)
      if((*m_vmeTdcHits)[i]->theHit() == m_tdc)
	      m_tdc_index = (*m_vmeTdcHits)[i]->txtIoIndex();

  if(m_slabHit)
    for(unsigned int i = 0; i<m_tofSlabHits->size(); i++)
      if((*m_tofSlabHits)[i]->theHit() == m_slabHit)
	      m_slab_index = (*m_tofSlabHits)[i]->txtIoIndex();

  if(m_tofHit)
    for(unsigned int i = 0; i<m_tofHits->size(); i++)
      if((*m_tofHits)[i]->theHit() == m_tofHit)
		  m_hit_index = (*m_tofHits)[i]->txtIoIndex();

  std::stringstream ss;

		//now write it all to string output
  ss << txtIoIndex() << " " << m_stationNumber << " " << m_planeNumber << " "
                       << m_pmt << " " << m_slab << " " << _pe << " " << _time << " "
                       << _adc << " " << _tdc << " " << m_good << " "
                       << m_adc_index << " " << m_fadc_index << " " << m_tdc_index << " " << m_slab_index << " "
                       << m_hit_index;

    return( ss.str() );
}

void		TofDigitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> m_stationNumber >> m_planeNumber >> m_pmt >> m_slab >> _pe >> _time
      >> _adc >> _tdc >> m_good >> m_adc_index >> m_fadc_index >> m_tdc_index >> m_slab_index
      >> m_hit_index;

  setTxtIoIndex( myindex );

}

TofDigit*	TofDigitTextFileIo::makeTofDigit()
{
  m_digit = new TofDigit(m_pmt);

  m_digit->setAdc( _adc );
  m_digit->setTdc( _tdc );
  m_digit->setPE( _pe );
  m_digit->setTime( _time );

  m_digit->setStationNumber(m_stationNumber);
  m_digit->setPlaneNumber(m_planeNumber);
  m_digit->setPmt(m_pmt);
  m_digit->setSlab(m_slab);
  m_digit->setIsGood(m_good);

  m_digit->setVmeAdcHit(m_adc);
  m_digit->setVmefAdcHit(m_fadc);
  m_digit->setVmeTdcHit(m_tdc);
  m_digit->setTofHit(m_tofHit);
  m_digit->setSlabHit(m_slabHit);

  return m_digit;
}

void		TofDigitTextFileIo::completeRestoration()
{
  m_adc = NULL;
  m_fadc = NULL;  
  m_tdc = NULL;

  for(unsigned int i =0; i<m_vmeAdcHits->size(); i++)
    if( (*m_vmeAdcHits)[i]->txtIoIndex() == m_adc_index )
      m_adc = (*m_vmeAdcHits)[i]->theHit();

  for(unsigned int i =0; i<m_vmefAdcHits->size(); i++)
    if( (*m_vmefAdcHits)[i]->txtIoIndex() == m_fadc_index )
      m_fadc = (*m_vmefAdcHits)[i]->theHit();
      
  for(unsigned int i =0; i<m_vmeTdcHits->size(); i++)
    if((*m_vmeTdcHits)[i]->txtIoIndex() == m_tdc_index)
      m_tdc = (*m_vmeTdcHits)[i]->theHit();

  m_slabHit = NULL;

  for(unsigned int i =0; i<m_tofSlabHits->size(); i++)
    if((*m_tofSlabHits)[i]->txtIoIndex() == m_slab_index)
      m_slabHit = (*m_tofSlabHits)[i]->theHit();

  m_tofHit = NULL;

  for(unsigned int i =0; i<m_tofHits->size(); i++)
    if((*m_tofHits)[i]->txtIoIndex() == m_hit_index)
      m_tofHit = (*m_tofHits)[i]->theHit();

  if( m_fadc ) m_digit->assignVmeHits(m_fadc, m_tdc);
  else         m_digit->assignVmeHits(m_adc, m_tdc); 
  
  m_digit->assignSlabHit(m_slabHit);
  m_digit->assignTofHit( m_tofHit );

  }
