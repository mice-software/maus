// EmCalTrackTextFileIo.cc

#include <sstream>

#include "EmCalTrackTextFileIo.hh"
#include "EmCalDigitTextFileIo.hh"

EmCalTrackTextFileIo::EmCalTrackTextFileIo( EmCalTrack* track, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::EmCalTrackTextFileIo );
  
  setTxtIoIndex( index );
  m_track = track;
}

EmCalTrackTextFileIo::EmCalTrackTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::EmCalTrackTextFileIo );
  
  restoreObject( def );
}

std::string EmCalTrackTextFileIo::storeObject()
{
  _tdcPeaks = m_track->getTDCPeaks();
  _totalProdADC = m_track->getTotalProdADC();
  _barycenter = m_track->getBarycenter();
  _maxADClay = m_track->getMaxADClay();
  _range = m_track->getRange();
  _rangeH = m_track->getRangeH(); 
  _holesQ = m_track->getHolesQ(); 
  _highQ = m_track->getHighQ();
  _maxLSubR = m_track->getMaxLSubR(); 
  _adcProd = m_track->getADCProd();
  _adcAdj = m_track->getADCAdj();
  _adcQ = m_track->getADCQ();
  _n = m_track->getN();
  _nH = m_track->getNH();
  _digitVector = m_track->GetMotherDigits();  

  _digitIndices.resize( _digitVector.size() );
  for( unsigned int i = 0; i < _digitVector.size(); ++i )
  {
    int din = -1;
    for( unsigned int j = 0; din < 0 && j < _digitIos->size(); ++j ){
      if( (*_digitIos)[j]->theDigit() == _digitVector[i] )
        din = (*_digitIos)[j]->txtIoIndex();
    }
    _digitIndices[i] = din;
  }
  
  std::stringstream ss;

  ss << txtIoIndex() << " " << _tdcPeaks << " " << _maxADClay << " " <<_rangeH << " " << _range <<  " " << _maxLSubR << " " <<_totalProdADC << " " << _barycenter << " " << _holesQ << " " << _highQ << " " << vectorToString(_adcProd).c_str() << " " << vectorToString(_adcAdj).c_str() << " " << vectorToString(_adcQ).c_str() << " " << vectorToString(_n).c_str() << " " << vectorToString(_nH).c_str() << " " << vectorToString(_digitIndices).c_str();

  return( ss.str() );
}

void    EmCalTrackTextFileIo::restoreObject( std::string def )
{
  int myindex;
  
  std::string adcProdString, adcAdjString, adcQString, nString, nHString;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> _tdcPeaks >> _maxADClay >> _rangeH >> _range >> _maxLSubR >> _totalProdADC >> _barycenter >> _holesQ >> _highQ;

  _adcProd = streamToVectorDouble(ist);
  _adcAdj = streamToVectorDouble(ist);
  _adcQ = streamToVectorDouble(ist);
  _n = streamToVectorInteger(ist);
  _nH = streamToVectorInteger(ist);
  _digitIndices = streamToVectorInteger(ist);

  setTxtIoIndex( myindex );
}

EmCalTrack* EmCalTrackTextFileIo::makeEmCalTrack()
{
  m_track = new EmCalTrack();
  m_track->setTDCPeaks(_tdcPeaks);
  m_track->setTotalProdADC(_totalProdADC);
  m_track->setBarycenter(_barycenter);
  m_track->setMaxADClay(_maxADClay);
  m_track->setRange(_range);
  m_track->setRangeH(_rangeH); 
  m_track->setHolesQ(_holesQ); 
  m_track->setHighQ(_highQ);
  m_track->setMaxLSubR(_maxLSubR); 
  m_track->setADCProd(_adcProd);
  m_track->setADCAdj(_adcAdj);
  m_track->setADCQ(_adcQ);
  m_track->setN(_n);
  m_track->setNH(_nH);

  return m_track;
}

void    EmCalTrackTextFileIo::completeRestoration()
{
  std::vector<EmCalDigit*> digits;
  digits.resize( _digitIndices.size() );

  for( unsigned int j = 0; j < _digitIndices.size(); ++j )
  {
    EmCalDigit* digit = NULL;
    for( unsigned int i = 0; i < _digitIos->size(); ++i )
      if( (*_digitIos)[i]->txtIoIndex() == _digitIndices[j] )
        digit = (*_digitIos)[i]->theDigit();
    digits[j] = digit;
  }
  m_track->SetMotherDigits( digits );
}

void    EmCalTrackTextFileIo::setEmCalDigits( std::vector<EmCalDigitTextFileIo*>* digits )
{
  _digitIos = digits;
}
