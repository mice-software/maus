// EmCalDigitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "EmCalDigitTextFileIo.hh"
#include "EmCalHitTextFileIo.hh"

EmCalDigitTextFileIo::EmCalDigitTextFileIo( EmCalDigit* digit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::EmCalDigitTextFileIo );
	
  setTxtIoIndex( index );
  m_digit = digit;
}

EmCalDigitTextFileIo::EmCalDigitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::EmCalDigitTextFileIo );
  
  restoreObject( def );
}

std::string	EmCalDigitTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!
  _side      = m_digit->GetSide();
  _adc       = m_digit->GetADC();
  _hitVector = m_digit->GetMotherHits();  
  _tdcVector = m_digit->GetTDCVector(); 

  // sometimes number of hits is way too large to write to a single line, this is a quick fix:
  int hitsToKeep = _hitVector.size();
  if (hitsToKeep > 500) hitsToKeep = 500;
  _hitIndices.resize( hitsToKeep );

  for( unsigned int i = 0; i < _hitVector.size(); ++i )
  {
    int hin = -1;
    for( unsigned int j = 0; hin < 0 && j < _hitIos->size(); ++j )
      if( (*_hitIos)[j]->theHit() == _hitVector[i] )
          hin = (*_hitIos)[j]->txtIoIndex();
    _hitIndices[i] = hin;
  }
 
  std::stringstream ss;

  ss << txtIoIndex() << " " << _side << " " << _adc << " " << vectorToString(_tdcVector).c_str() << " " << vectorToString(_hitIndices).c_str();

  return( ss.str() );
}

void		EmCalDigitTextFileIo::restoreObject( std::string def )
{
  int myindex;
  std::string tdcString, hitString;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> _side >> _adc;

  _tdcVector = streamToVectorInteger(ist);
  _hitIndices = streamToVectorInteger(ist);

  setTxtIoIndex( myindex );
}

EmCalDigit*	EmCalDigitTextFileIo::makeEmCalDigit()
{
  m_digit = new EmCalDigit();

  // possibly you have to use some "set" methods here...
  m_digit->SetSide(_side);
  m_digit->SetADC(_adc);
  m_digit->SetTDCVector(_tdcVector); 

  return m_digit;
}

void		EmCalDigitTextFileIo::completeRestoration()
{
  std::vector<EmCalHit*> hits;
  hits.resize( _hitIndices.size() );

  for( unsigned int j = 0; j < _hitIndices.size(); ++j )
  {
    EmCalHit* hit = NULL;
    for( unsigned int i = 0; i < _hitIos->size(); ++i )
      if( (*_hitIos)[i]->txtIoIndex() == _hitIndices[j] )
        hit = (*_hitIos)[i]->theHit();
    hits[j] = hit;
  }
  m_digit->SetMotherHits( hits );
}

void    EmCalDigitTextFileIo::setEmCalHits( std::vector<EmCalHitTextFileIo*>* hits )
{
  _hitIos = hits;
}
