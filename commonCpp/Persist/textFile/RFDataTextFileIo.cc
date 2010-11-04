// RFDataTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "RFDataTextFileIo.hh"

RFDataTextFileIo::RFDataTextFileIo( RFData* data, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::RFDataTextFileIo );
	
  setTxtIoIndex( index );
  _data = data;
}

RFDataTextFileIo::RFDataTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::RFDataTextFileIo );
  
  restoreObject( def );
}

std::string	RFDataTextFileIo::storeObject()
{
		//set the data that we are going to write (variables)
  _cavityPos   = _data->GetCavityPosition();
  _phase       = _data->GetPhase();
  _peakField   = _data->GetPeakField();
  _frequency   = _data->GetFrequency();
  _phaseError  = _data->GetPhaseError();
  _energyError = _data->GetEnergyError();

  std::stringstream ss;

		//now write it all to string output
  ss << txtIoIndex() << " " << _cavityPos << " " << _phase << " "
     << _peakField  << " " << _frequency << " " << _phaseError << " " << _energyError;

  return( ss.str() );
}

void		RFDataTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> _cavityPos >> _phase 
      >> _peakField >> _frequency >> _phaseError >> _energyError;

  setTxtIoIndex( myindex );
}

RFData*	RFDataTextFileIo::makeRFData()
{
  _data = new RFData(_cavityPos, _phase, _peakField, _frequency, _phaseError, _energyError);

  return _data;
}

void		RFDataTextFileIo::completeRestoration()
{
}
