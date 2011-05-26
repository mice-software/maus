// Memory.cc
//
// Class to keep track of how many times each of the G4MICE classes has been created and destroyed
//
// static const strings are used to refer to each class name and maps are used to keep track of new and delete
// operations.
//
// This class is used in MICETESTS but can also be used by user codes to check for memory leaks
//
// M.Ellis 1st November 2006

#include <iostream>

#include "Interface/Memory.hh"

Memory::Memory()
{
  _classes.push_back( Memory::VlpcHit );
  _classes.push_back( Memory::VlpcEventTime );
  _classes.push_back( Memory::CkovHit );
  _classes.push_back( Memory::CkovDigit );
  _classes.push_back( Memory::EMRHit );
  _classes.push_back( Memory::EMRDigit );
  _classes.push_back( Memory::KLHit );
  _classes.push_back( Memory::KLDigit );
  _classes.push_back( Memory::KLCellHit );
  _classes.push_back( Memory::SciFiHit );
  _classes.push_back( Memory::SciFiDigit );
  _classes.push_back( Memory::SciFiDoubletCluster );
  _classes.push_back( Memory::SciFiSpacePoint );
  _classes.push_back( Memory::SciFiTrack );
  _classes.push_back( Memory::SciFiKalTrack );
  _classes.push_back( Memory::SciFiExtrap );
  _classes.push_back( Memory::TofHit );
  _classes.push_back( Memory::TofDigit );
  _classes.push_back( Memory::TofSlabHit );
  _classes.push_back( Memory::TofSpacePoint );
  _classes.push_back( Memory::VmeAdcHit );
  _classes.push_back( Memory::VmefAdcHit );
  _classes.push_back( Memory::VmeTdcHit );
  _classes.push_back( Memory::VmeScalerData );
  _classes.push_back( Memory::EmCalHit );
  _classes.push_back( Memory::EmCalDigit );
  _classes.push_back( Memory::EmCalTrack ); 
  _classes.push_back( Memory::VirtualHit );
  _classes.push_back( Memory::SpecialHit );
  _classes.push_back( Memory::MCParticle );
  _classes.push_back( Memory::MCVertex );
  _classes.push_back( Memory::ZustandVektor );
  _classes.push_back( Memory::RFData );

  _classes.push_back( Memory::BeamParameters );
  _classes.push_back( Memory::BeamlineGeometry );
  _classes.push_back( Memory::BeamlineParameters );
  _classes.push_back( Memory::CoolingChannelGeom );
  _classes.push_back( Memory::MagnetParameters );
  _classes.push_back( Memory::MiceModule );
  _classes.push_back( Memory::RFBackgroundParameters );
  _classes.push_back( Memory::RFParameters );
  _classes.push_back( Memory::SciFiCableManager );
  _classes.push_back( Memory::TofCable );
  _classes.push_back( Memory::VirtualGeomParameters );
  _classes.push_back( Memory::VlpcCableImperial );
  _classes.push_back( Memory::VlpcCableOsaka );
  _classes.push_back( Memory::PidFits );

  _classes.push_back( Memory::CKOVSD );
  _classes.push_back( Memory::KLFiber );
  _classes.push_back( Memory::KLGlue );
  _classes.push_back( Memory::KLSD );
  _classes.push_back( Memory::EmCalKLFiber );
  _classes.push_back( Memory::EmCalKLGlue );
  _classes.push_back( Memory::EmCalSD );
  _classes.push_back( Memory::DoubletFiberParam );
  _classes.push_back( Memory::SciFiPlane );
  _classes.push_back( Memory::SciFiSD );
  _classes.push_back( Memory::TofSD );
  _classes.push_back( Memory::SpecialDummySD );
  _classes.push_back( Memory::SpecialVirtualSD );
  _classes.push_back( Memory::VirtualSD );
  _classes.push_back( Memory::VirtualStation );

  _classes.push_back( Memory::AbsorberVessel );
  _classes.push_back( Memory::MiceElectroMagneticField );
  _classes.push_back( Memory::MiceMagneticField );
  _classes.push_back( Memory::Polycone );
  _classes.push_back( Memory::Q35 );

  _classes.push_back( Memory::PersistClasses );
  _classes.push_back( Memory::TextFileWriter );
  _classes.push_back( Memory::TextFileReader );
  _classes.push_back( Memory::VlpcHitTextFileIo );
  _classes.push_back( Memory::VlpcEventTimeTextFileIo );
  _classes.push_back( Memory::CkovHitTextFileIo );
  _classes.push_back( Memory::CkovDigitTextFileIo );
  _classes.push_back( Memory::SciFiHitTextFileIo );
  _classes.push_back( Memory::SciFiDigitTextFileIo );
  _classes.push_back( Memory::SciFiDoubletClusterTextFileIo );
  _classes.push_back( Memory::SciFiSpacePointTextFileIo );
  _classes.push_back( Memory::SciFiTrackTextFileIo );
  _classes.push_back( Memory::SciFiKalTrackTextFileIo );
  _classes.push_back( Memory::SciFiExtrapTextFileIo );
  _classes.push_back( Memory::TofHitTextFileIo );
  _classes.push_back( Memory::TofDigitTextFileIo );
  _classes.push_back( Memory::TofSlabHitTextFileIo );
  _classes.push_back( Memory::TofSpacePointTextFileIo );
  _classes.push_back( Memory::VmeAdcHitTextFileIo );
  _classes.push_back( Memory::VmefAdcHitTextFileIo );
  _classes.push_back( Memory::VmeTdcHitTextFileIo );
  _classes.push_back( Memory::VmeScalerDataTextFileIo );
  _classes.push_back( Memory::KLHitTextFileIo );
  _classes.push_back( Memory::EmCalHitTextFileIo );
  _classes.push_back( Memory::EmCalDigitTextFileIo );
  _classes.push_back( Memory::EmCalTrackTextFileIo );
  _classes.push_back( Memory::VirtualHitTextFileIo );
  _classes.push_back( Memory::SpecialHitTextFileIo );
  _classes.push_back( Memory::MCParticleTextFileIo );
  _classes.push_back( Memory::MCVertexTextFileIo );
  _classes.push_back( Memory::ZustandVektorTextFileIo );
  _classes.push_back( Memory::RFDataTextFileIo );

	_classes.push_back( Memory::V1290DATEFileOut );
  _classes.push_back( Memory::V1724DATEFileOut );
	_classes.push_back( Memory::V1731DATEFileOut );
	_classes.push_back( Memory::V830DATEFileOut );
	
  reset();
}

Memory::~Memory()
{
}

void  Memory::reset()
{
  for( unsigned int i = 0; i < _classes.size(); ++i )
  {
    _news[ _classes[i] ] = 0;
    _deletes[ _classes[i] ] = 0;
  }
}

void  Memory::addNew( std::string cl )
{
  std::map<std::string,int>::iterator it = _news.find( cl );

  if( it != _news.end() )
    _news[ cl ] = _news[ cl ] + 1;
  else
    std::cerr << "Memory::new - does not know about the class " << cl << std::endl;
}

void  Memory::addDelete( std::string cl )
{
  std::map<std::string,int>::iterator it = _deletes.find( cl );

  if( it != _deletes.end() )
    _deletes[ cl ] = _deletes[ cl ] + 1;
  else
    std::cerr << "Memory::delete - does not know about the class " << cl << std::endl;
}

int Memory::numNews( std::string cl ) const
{
  int ret = -1;

  std::map<std::string,int>::const_iterator it = _news.find( cl );

  if( it != _news.end() )
    ret = it->second;
  else
    std::cerr << "Memory::new - does not know about the class " << cl << std::endl;

  return ret;
}

int   Memory::numDeletes( std::string cl ) const
{
  int ret = -1;

  std::map<std::string,int>::const_iterator it = _deletes.find( cl );

  if( it != _deletes.end() )
    ret = it->second;
  else
    std::cerr << "Memory::delete - does not know about the class " << cl << std::endl;

  return ret;
}

bool  Memory::isLeaking( std::string cl ) const
{
  int nnews = numNews( cl );
  int ndels = numDeletes( cl );

  bool leak = false;

  if( nnews < 0 || ndels < 0 || nnews != ndels )
    leak = true;

  return leak;
}

bool  Memory::anyLeaking() const
{
  bool leak = false;

  for( unsigned int i = 0; i < _classes.size(); ++i )
    leak |= isLeaking( _classes[i] );

  return leak;
}

void  Memory::writeReport( std::ostream& out ) const
{
  out << "Memory use report:" << std::endl;

  for( unsigned int i = 0; i < _classes.size(); ++i )
  {
    out << "  " << _classes[i] << " was newed " << numNews( _classes[i] ) << " times and was deleted " << numDeletes( _classes[i] ) << " times";
    if( isLeaking( _classes[i] ) )
      out << " - this is a LEAK!" << std::endl;
    else
      out << std::endl;
  }
}

void  Memory::printReport() const
{
  writeReport( std::cout );
}

const std::string Memory::VlpcHit = "VlpcHit";
const std::string Memory::VlpcEventTime = "VlpcEventTime";
const std::string Memory::CkovHit = "CkovHit";
const std::string Memory::CkovDigit = "CkovDigit";
const std::string Memory::EMRHit = "EMRHit";
const std::string Memory::EMRDigit = "EMRDigit";
const std::string Memory::KLHit = "KLHit";
const std::string Memory::KLDigit = "KLDigit";
const std::string Memory::KLCellHit = "KLCellHit";
const std::string Memory::SciFiHit = "SciFiHit";
const std::string Memory::SciFiDigit = "SciFiDigit";
const std::string Memory::SciFiDoubletCluster = "SciFiDoubletCluster";
const std::string Memory::SciFiSpacePoint = "SciFiSpacePoint";
const std::string Memory::SciFiTrack = "SciFiTrack";
const std::string Memory::SciFiKalTrack = "SciFiKalTrack";
const std::string Memory::SciFiExtrap = "SciFiExtrap";
const std::string Memory::TofHit = "TofHit";
const std::string Memory::TofDigit = "TofDigit";
const std::string Memory::TofSlabHit = "TofSlabHit";
const std::string Memory::TofSpacePoint = "TofSpacePoint";
const std::string Memory::VmeAdcHit = "VmeAdcHit";
const std::string Memory::VmefAdcHit = "VmefAdcHit";
const std::string Memory::VmeTdcHit = "VmeTdcHit";
const std::string Memory::VmeScalerData = "VmeScalerData";
const std::string Memory::EmCalHit = "EmCalHit";
const std::string Memory::EmCalDigit = "EmCalDigit";
const std::string Memory::EmCalTrack = "EmCalTrack";
const std::string Memory::VirtualHit = "VirtualHit";
const std::string Memory::SpecialHit = "SpecialHit";
const std::string Memory::MCParticle = "MCParticle";
const std::string Memory::MCVertex = "MCVertex";
const std::string Memory::ZustandVektor = "ZustandVektor";
const std::string Memory::RFData = "RFData";

const std::string Memory::BeamParameters = "BeamParameters";
const std::string Memory::BeamlineGeometry = "BeamlineGeometry";
const std::string Memory::BeamlineParameters = "BeamlineParameters";
const std::string Memory::CoolingChannelGeom = "CoolingChannelGeom";
const std::string Memory::MagnetParameters = "MagnetParameters";
const std::string Memory::MiceModule = "MiceModule";
const std::string Memory::RFBackgroundParameters = "RFBackgroundParameters";
const std::string Memory::RFParameters = "RFParameters";
const std::string Memory::SciFiCableManager = "SciFiCableManager";
const std::string Memory::TofCable = "TofCable";
const std::string Memory::VirtualGeomParameters = "VirtualGeomParameters";
const std::string Memory::VlpcCableImperial = "VlpcCableImperial";
const std::string Memory::VlpcCableOsaka = "VlpcCableOsaka";
const std::string Memory::PidFits = "PidFits";

const std::string Memory::CKOVSD = "CKOVSD";
const std::string Memory::EMRSD = "EMRSD";
const std::string Memory::KLFiber = "KLFiber";
const std::string Memory::KLGlue = "KLGlue";
const std::string Memory::KLSD = "KLSD";
const std::string Memory::EmCalKLFiber = "EmCalKLFiber";
const std::string Memory::EmCalKLGlue = "EmCalKLGlue";
const std::string Memory::EmCalSD = "EmCalSD";
const std::string Memory::DoubletFiberParam = "DoubletfiberParam";
const std::string Memory::SciFiPlane = "SciFiPlane";
const std::string Memory::SciFiSD = "SciFiSD";
const std::string Memory::TofSD = "TofSD";
const std::string Memory::SpecialDummySD = "SpecialDummySD";
const std::string Memory::SpecialVirtualSD = "SpecialVirtualSD";
const std::string Memory::VirtualSD = "VirtualSD";
const std::string Memory::VirtualStation = "VirtualStation";

const std::string Memory::AbsorberVessel = "AbsorberVessel";
const std::string Memory::MiceElectroMagneticField = "MiceElectroMagneticField";
const std::string Memory::MiceMagneticField = "MiceMagneticField";
const std::string Memory::Polycone = "Polycone";
const std::string Memory::Q35 = "Q35";

const std::string Memory::PersistClasses = "PersistClasses";
const std::string Memory::TextFileReader = "TextFileReader";
const std::string Memory::TextFileWriter = "TextFileWriter";
const std::string Memory::VlpcHitTextFileIo = "VlpcHitTextFileIo";
const std::string Memory::VlpcEventTimeTextFileIo = "VlpcEventTimeTextFileIo";
const std::string Memory::CkovHitTextFileIo = "CkovHitTextFileIo";
const std::string Memory::CkovDigitTextFileIo = "CkovDigitTextFileIo";
const std::string Memory::SciFiHitTextFileIo = "SciFiHitTextFileIo";
const std::string Memory::SciFiDigitTextFileIo = "SciFiDigitTextFileIo";
const std::string Memory::SciFiDoubletClusterTextFileIo = "SciFiDoubletClusterTextFileIo";
const std::string Memory::SciFiSpacePointTextFileIo = "SciFiSpacePointTextFileIo";
const std::string Memory::SciFiTrackTextFileIo = "SciFiTrackTextFileIo";
const std::string Memory::SciFiKalTrackTextFileIo = "SciFiKalTrackTextFileIo";
const std::string Memory::SciFiExtrapTextFileIo = "SciFiExtrapTextFileIo";
const std::string Memory::TofHitTextFileIo = "TofHitTextFileIo";
const std::string Memory::TofDigitTextFileIo = "TofDigitTextFileIo";
const std::string Memory::TofSlabHitTextFileIo = "TofSlabHitTextFileIo";
const std::string Memory::TofSpacePointTextFileIo = "TofSpacePointTextFileIo";
const std::string Memory::VmeAdcHitTextFileIo = "VmeAdcHitTextFileIo";
const std::string Memory::VmefAdcHitTextFileIo = "VmefAdcHitTextFileIo";
const std::string Memory::VmeTdcHitTextFileIo = "VmeTdcHitTextFileIo";
const std::string Memory::VmeScalerDataTextFileIo = "VmeScalerDataTextFileIo";
const std::string Memory::KLHitTextFileIo = "KLHitTextFileIo";
const std::string Memory::EmCalHitTextFileIo = "EmCalHitTextFileIo";
const std::string Memory::EmCalDigitTextFileIo = "EmCalDigitTextFileIo";
const std::string Memory::EmCalTrackTextFileIo = "EmCalTrackTextFileIo";
const std::string Memory::VirtualHitTextFileIo = "VirtualHitTextFileIo";
const std::string Memory::SpecialHitTextFileIo = "SpecialHitTextFileIo";
const std::string Memory::MCParticleTextFileIo = "MCParticleTextFileIo";
const std::string Memory::MCVertexTextFileIo = "MCVertexTextFileIo";
const std::string Memory::ZustandVektorTextFileIo = "ZustandVektorTextFileIO";
const std::string Memory::RFDataTextFileIo = "RFDataTextFileIO";

const std::string Memory::V1290DATEFileOut = "V1290DATEFileOut";
const std::string Memory::V1724DATEFileOut = "V1724DATEFileOut";
const std::string Memory::V1731DATEFileOut = "V1731DATEFileOut";
const std::string Memory::V830DATEFileOut = "V830DATEFileOut";

Memory miceMemory;

