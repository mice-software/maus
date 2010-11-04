//VmeScalerDataTextFileIo.cc
//
// V.Verguilov 2 February 2009

#include <sstream>

#include "VmeScalerDataTextFileIo.hh"

VmeScalerDataTextFileIo::VmeScalerDataTextFileIo( VmeScalerData* scaler, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VmeScalerDataTextFileIo );	
  setTxtIoIndex( index );
  m_scaler = scaler;
}

VmeScalerDataTextFileIo::VmeScalerDataTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VmeScalerDataTextFileIo );
  restoreObject( def );
}

std::string	VmeScalerDataTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!
	
	m_crate = m_scaler->crate();
	m_board = m_scaler->board();
	m_module_type = m_scaler->moduleType();
	m_channel = m_scaler->channel();
	m_data = m_scaler->GetData();

  std::stringstream ss;
  ss << txtIoIndex() << " " << m_crate << " " << m_board << " " << m_channel << " " << m_data << " " << m_module_type.c_str();
  return( ss.str() );
}

void VmeScalerDataTextFileIo::restoreObject( std::string def )
{
  int myindex;
  std::istringstream ist( def.c_str() );
	ist >> myindex >> m_crate >> m_board >> m_channel >> m_data >> m_module_type;
  setTxtIoIndex( myindex );
}

VmeScalerData*	VmeScalerDataTextFileIo::makeVmeScalerData()
{
  m_scaler = new VmeScalerData( m_crate, m_board, m_module_type, m_channel, m_data );
  // possibly you have to use some "set" methods here...
  return m_scaler;
}

void	VmeScalerDataTextFileIo::completeRestoration()
{

}
