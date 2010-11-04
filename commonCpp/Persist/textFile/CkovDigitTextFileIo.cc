// CkovDigitTextFileIo.cc
//
// M.Ellis 7/10/2006

#include <sstream>

#include "CkovDigitTextFileIo.hh"
#include "CkovHitTextFileIo.hh"
#include "VmeAdcHitTextFileIo.hh"
#include "VmefAdcHitTextFileIo.hh"
#include "VmeTdcHitTextFileIo.hh"

CkovDigitTextFileIo::CkovDigitTextFileIo( CkovDigit* digit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::CkovDigitTextFileIo );
	
  setTxtIoIndex( index );
  m_digit = digit;

  m_vmeAdcHits = NULL;
  m_vmeTdcHits = NULL;
  m_vmefAdcHits = NULL;
  m_adcHit = NULL;
  m_tdcHit = NULL;
  m_fadcHit = NULL;
}

CkovDigitTextFileIo::CkovDigitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::CkovDigitTextFileIo );
  
  restoreObject( def );
}

std::string	CkovDigitTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!
	m_adcHit  = m_digit->vmeAdcHit();
	m_fadcHit = m_digit->vmefAdcHit(); 
	m_tdcHit  = m_digit->vmeTdcHit();

  _pmt       = m_digit->GetPmt();
  _plane     = m_digit->GetPlane();
  _tdc       = m_digit->GetTDC();
  _adc       = m_digit->GetADC();
  _pe	     = m_digit->GetPe();
  _good		 = m_digit->isGood();
  _hitVector = m_digit->GetMotherHits();

  _hitIndices.resize( _hitVector.size() );

  for( unsigned int i = 0; i < _hitVector.size(); ++i )
  {
    int hin = -1;
    for( unsigned int j = 0; hin < 0 && j < _hitIos->size(); ++j )
      if( (*_hitIos)[j]->theHit() == _hitVector[i] )
          hin = (*_hitIos)[j]->txtIoIndex();
    _hitIndices[i] = hin;
  }
 
  m_adc_index = -1;

  if( m_adcHit && m_vmeAdcHits )
    for( unsigned int i = 0; i < m_vmeAdcHits->size(); ++i )
      if( ( *m_vmeAdcHits )[ i ]->theHit() == m_adcHit )
	      m_adc_index = ( *m_vmeAdcHits )[ i ]->txtIoIndex();

  m_fadc_index = -1;

  if( m_fadcHit && m_vmefAdcHits )
    for(unsigned int i = 0; i<m_vmefAdcHits->size(); ++i )
      if((*m_vmefAdcHits)[i]->theHit() == m_fadcHit )
	      m_fadc_index = (*m_vmefAdcHits)[i]->txtIoIndex();

  m_tdc_index = -1;

  if( m_tdcHit && m_vmeTdcHits )
    for( unsigned int i = 0; i < m_vmeTdcHits->size(); ++i )
      if( ( *m_vmeTdcHits )[ i ]->theHit() == m_tdcHit )
	      m_tdc_index = ( *m_vmeTdcHits )[ i ]->txtIoIndex();

  std::stringstream ss;
  
  ss << txtIoIndex() << " " << _pe << " " << _pmt << " " << _plane << " " << _tdc <<   " " << _adc << " " 
		  			 << vectorToString(_hitIndices).c_str() << " " << _good << " " 
		  			 << m_adc_index << " " << m_fadc_index << " " << m_tdc_index ;

  return( ss.str() );
}

void	CkovDigitTextFileIo::restoreObject( std::string def )
{
  int myindex;
  std::string hitString;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> _pe >> _pmt >> _plane >> _tdc >> _adc;

  _hitIndices = streamToVectorInteger( ist );

  ist >> _good >> m_adc_index >> m_fadc_index >> m_tdc_index;

  setTxtIoIndex( myindex );
}

CkovDigit*	CkovDigitTextFileIo::makeCkovDigit()
{
  m_digit = new CkovDigit();
  
  m_digit->SetPmt(_pmt);
  m_digit->SetPlane(_plane);
  m_digit->SetTDC(_tdc);
  m_digit->SetADC(_adc);
  m_digit->SetPe(_pe);
  m_digit->setIsGood( _good );

  return m_digit;
}

void	CkovDigitTextFileIo::completeRestoration()
{
  m_adcHit = NULL;
  m_fadcHit = NULL;
  m_tdcHit = NULL;

  std::vector<CkovHit*> hits;
  hits.resize( _hitIndices.size() );

  for( unsigned int j = 0; j < _hitIndices.size(); ++j )
  {
    CkovHit* hit = NULL;
    for( unsigned int i = 0; i < _hitIos->size(); ++i )
      if( (*_hitIos)[i]->txtIoIndex() == _hitIndices[j] )
        hit = (*_hitIos)[i]->theHit();
    hits[j] = hit;
  }
  m_digit->SetMotherHits( hits );

  if( m_vmeAdcHits )
  for( unsigned int i = 0; i< m_vmeAdcHits->size(); ++i )
    if( (*m_vmeAdcHits)[i]->txtIoIndex() == m_adc_index )
      m_adcHit = (*m_vmeAdcHits)[i]->theHit();

  if( m_vmefAdcHits )
  for( unsigned int i = 0; i < m_vmefAdcHits->size(); ++i )
    if( (*m_vmefAdcHits)[i]->txtIoIndex() == m_fadc_index )
      m_fadcHit = (*m_vmefAdcHits)[i]->theHit();

  if( m_vmeTdcHits )
  for( unsigned int i =0; i < m_vmeTdcHits->size(); ++i )
    if(( *m_vmeTdcHits )[ i ]->txtIoIndex() == m_tdc_index)
      m_tdcHit = (*m_vmeTdcHits)[ i ]->theHit();

  m_digit->setVmeAdcHit( m_adcHit );
  m_digit->setVmeTdcHit( m_tdcHit );
  m_digit->setVmefAdcHit( m_fadcHit );
}

void    CkovDigitTextFileIo::setCkovHits( std::vector<CkovHitTextFileIo*>* hits )
{
  _hitIos = hits;
}
