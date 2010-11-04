// PersistClasses.cc

#include <iostream>
#include <sstream>

#include "PersistClasses.hh"

PersistClasses::PersistClasses()
{
  miceMemory.addNew( Memory::PersistClasses );

  fillClasses();

  processAll();
}

PersistClasses::PersistClasses( PersistClasses& rhs )
{
  miceMemory.addNew( Memory::PersistClasses ); 
  
  fillClasses();

  processNone();

  for( unsigned int i = 0; i < classes.size(); ++i )
    setClassProcessing( classes[i], rhs.processClass( classes[i] ) );
}

PersistClasses::PersistClasses( bool allnone )
{
  miceMemory.addNew( Memory::PersistClasses ); 
  
  fillClasses();

  if( allnone )
    processAll();
  else
    processNone();
}

void  PersistClasses::processNone()
{
  fillClasses();

  for( unsigned int i = 0; i < classes.size(); ++i )
    setClassProcessing( classes[i], false );
}

void  PersistClasses::processAll()
{
  fillClasses();

  for( unsigned int i = 0; i < classes.size(); ++i )
    setClassProcessing( classes[i], true );
}

void  PersistClasses::setClassProcessing( std::string cl )
{
  if( classKnown( cl ) )
    processing[cl] = true;
}

void  PersistClasses::setClassProcessing( std::string cl, bool proc )
{
  if( classKnown( cl ) )
     processing[cl] = proc;
}

bool  PersistClasses::processClass( std::string cl )
{
  bool proc = false;

  if( classKnown( cl ) )
     proc = processing[cl];

  return proc;
}

void  PersistClasses::fillClasses()
{
  if( classes.size() == 0 )
  {
    classes.push_back( PersistClasses::VlpcHit );
    classes.push_back( PersistClasses::VlpcEventTime );
    classes.push_back( PersistClasses::CkovHit );
    classes.push_back( PersistClasses::CkovDigit );
    classes.push_back( PersistClasses::SciFiHit );
    classes.push_back( PersistClasses::SciFiDigit );
    classes.push_back( PersistClasses::SciFiDoubletCluster );
    classes.push_back( PersistClasses::SciFiSpacePoint );
    classes.push_back( PersistClasses::SciFiTrack );
    classes.push_back( PersistClasses::SciFiKalTrack );
    classes.push_back( PersistClasses::SciFiExtrap );
    classes.push_back( PersistClasses::TofHit );
    classes.push_back( PersistClasses::TofDigit );
    classes.push_back( PersistClasses::TofSlabHit );
    classes.push_back( PersistClasses::TofSpacePoint );
    classes.push_back( PersistClasses::VmeAdcHit );
    classes.push_back( PersistClasses::VmefAdcHit );
    classes.push_back( PersistClasses::VmeTdcHit );
    classes.push_back( PersistClasses::EmCalHit );
    classes.push_back( PersistClasses::EmCalDigit );
    classes.push_back( PersistClasses::EmCalTrack );
    classes.push_back( PersistClasses::KLHit );
    classes.push_back( PersistClasses::KLDigit );
    classes.push_back( PersistClasses::KLCellHit );
    classes.push_back( PersistClasses::VirtualHit );
    classes.push_back( PersistClasses::SpecialHit );
    classes.push_back( PersistClasses::MCParticle );
    classes.push_back( PersistClasses::MCVertex );
    classes.push_back( PersistClasses::ZustandVektor );
    classes.push_back( PersistClasses::RFData );
    classes.push_back( PersistClasses::KillHit );
    classes.push_back( PersistClasses::PrimaryGenHit );
  }
}

bool  PersistClasses::classKnown( std::string name )
{
  bool found = false;

  for( unsigned int i = 0; ! found && i < classes.size(); ++i )
    if( classes[i] == name )
      found = true;

  return found;
}

void  PersistClasses::dumpProcessing()
{
  for( unsigned int i = 0; i < classes.size(); ++i )
    std::cout << "Processing for class " << classes[i] << " = " 
              << processClass( classes[i] ) << std::endl;
}

const std::string PersistClasses::VlpcHit = "VlpcHit";
const std::string PersistClasses::VlpcEventTime = "VlpcEventTime";
const std::string PersistClasses::CkovHit = "CkovHit";
const std::string PersistClasses::CkovDigit = "CkovDigit";
const std::string PersistClasses::SciFiHit = "SciFiHit";
const std::string PersistClasses::SciFiDigit = "SciFiDigit";
const std::string PersistClasses::SciFiDoubletCluster = "SciFiDoubletCluster";
const std::string PersistClasses::SciFiSpacePoint = "SciFiSpacePoint";
const std::string PersistClasses::SciFiTrack = "SciFiTrack";
const std::string PersistClasses::SciFiKalTrack = "SciFiKalTrack";
const std::string PersistClasses::SciFiExtrap = "SciFiExtrap";
const std::string PersistClasses::TofHit = "TofHit";
const std::string PersistClasses::TofDigit = "TofDigit";
const std::string PersistClasses::TofSlabHit = "TofSlabHit";
const std::string PersistClasses::TofSpacePoint = "TofSpacePoint";
const std::string PersistClasses::VmeAdcHit = "VmeAdcHit";
const std::string PersistClasses::VmefAdcHit = "VmefAdcHit";
const std::string PersistClasses::VmeTdcHit = "VmeTdcHit";
const std::string PersistClasses::VmeScalerData = "VmeScalerData";
const std::string PersistClasses::EmCalHit = "EmCalHit";
const std::string PersistClasses::EmCalDigit = "EmCalDigit";
const std::string PersistClasses::EmCalTrack = "EmCalTrack";
const std::string PersistClasses::KLHit = "KLHit";
const std::string PersistClasses::KLDigit = "KLDigit";
const std::string PersistClasses::KLCellHit = "KLCellHit";
const std::string PersistClasses::VirtualHit = "VirtualHit";
const std::string PersistClasses::SpecialHit = "SpecialHit";
const std::string PersistClasses::MCParticle = "MCParticle";
const std::string PersistClasses::MCVertex = "MCVertex";
const std::string PersistClasses::ZustandVektor = "ZustandVektor";
const std::string PersistClasses::RFData = "RFData";
const std::string PersistClasses::KillHit = "KillHit";
const std::string PersistClasses::PrimaryGenHit = "PrimaryGenHit";


