// TextFileReader.cc
//
// M.Ellis 22/8/2005

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include "TextFileReader.hh"
#include "Interface/Squeal.hh"
#include "Interface/Squeak.hh"

#include "VlpcHitTextFileIo.hh"
#include "VlpcEventTimeTextFileIo.hh"
#include "CkovHitTextFileIo.hh"
#include "CkovDigitTextFileIo.hh"
#include "SciFiHitTextFileIo.hh"
#include "SciFiDigitTextFileIo.hh"
#include "SciFiDoubletClusterTextFileIo.hh"
#include "SciFiSpacePointTextFileIo.hh"
#include "EmCalHitTextFileIo.hh"
#include "EmCalDigitTextFileIo.hh"
#include "EmCalTrackTextFileIo.hh"
#include "VirtualHitTextFileIo.hh"
#include "SpecialHitTextFileIo.hh"
#include "TofHitTextFileIo.hh"
#include "TofDigitTextFileIo.hh"
#include "TofSlabHitTextFileIo.hh"
#include "TofSpacePointTextFileIo.hh"
#include "VmeAdcHitTextFileIo.hh"
#include "VmefAdcHitTextFileIo.hh"
#include "VmeTdcHitTextFileIo.hh"
#include "MCParticleTextFileIo.hh"
#include "MCVertexTextFileIo.hh"
#include "ZustandVektorTextFileIo.hh"
#include "RFDataTextFileIo.hh"
#include "KillHitTextFileIo.hh"
#include "PrimaryGenHitTextFileIo.hh"

TextFileReader::TextFileReader( std::string filename , MICEEvent& e, MICERun* r, PersistClasses& p ) :
  event( e ), run( r ), persist( p )
{
  isOk = false;

  miceMemory.addNew( Memory::TextFileReader ); 
	
  inf = gzopen( filename.c_str(), "r" );
  int err = Z_OK;
  gzerror(inf, &err);
  if( err != Z_OK )
    std::cerr << "Read error in text file reader " << filename << std::endl;
  else
    isOk = true;
}

bool  TextFileReader::readEvent()
{
  unsigned int i;

  // Make TextFileIo objects for each transient object

  std::vector<VlpcHitTextFileIo*>   vlpcHitIo;
  VlpcEventTimeTextFileIo*      vlpcEventTimeIo = NULL;
  std::vector<CkovHitTextFileIo*>   ckovHitIo;
  std::vector<CkovDigitTextFileIo*>   ckovDigitIo;
  std::vector<SciFiHitTextFileIo*>    sciFiHitIo;
  std::vector<SciFiDigitTextFileIo*>    sciFiDigitIo;
  std::vector<SciFiDoubletClusterTextFileIo*> sciFiClusterIo;
  std::vector<SciFiSpacePointTextFileIo*> sciFiSpacePointIo;
  std::vector<EmCalHitTextFileIo*> emCalHitIo;
  std::vector<EmCalDigitTextFileIo*> emCalDigitIo;
  std::vector<EmCalTrackTextFileIo*> emCalTrackIo;
  std::vector<VirtualHitTextFileIo*> virtualHitIo;
  std::vector<SpecialHitTextFileIo*> specialHitIo;
  std::vector<TofHitTextFileIo*> tofHitIo;
  std::vector<TofDigitTextFileIo*> tofDigitIo;
  std::vector<TofSlabHitTextFileIo*> tofSlabHitIo;
  std::vector<TofSpacePointTextFileIo*> tofSpacePointIo;
  std::vector<VmeAdcHitTextFileIo*> vmeAdcHitIo;
  std::vector<VmefAdcHitTextFileIo*> vmefAdcHitIo;
  std::vector<VmeTdcHitTextFileIo*> vmeTdcHitIo;
  std::vector<MCParticleTextFileIo*> mcParticleIo;
  std::vector<MCVertexTextFileIo*> mcVertexIo;
  std::vector<ZustandVektorTextFileIo*> zsVectIo;
  std::vector<RFDataTextFileIo*> rfDataIo;
  std::vector<KillHitTextFileIo*> killHitIo;
  std::vector<PrimaryGenHitTextFileIo*> primaryHitIo;

  std::string type;
  int number;
  int count;

  number = 0;

  int err = Z_OK;
  gzerror(inf, &err);
  if( err != Z_OK )
  {
    isOk = false;
    return false;
  }

  while( number != -1 && err == Z_OK && !gzeof(inf))
  {
    std::istringstream ist( readOneLine( inf ).c_str() );
    ist >> number >> type;

    if( type == "VlpcHits" )
    {
      for( count = 0; count < number; ++count )
      {
        VlpcHitTextFileIo* hit = new VlpcHitTextFileIo( readOneLine( inf ) );
        vlpcHitIo.push_back( hit );
      }
    }
    else if( type == "CkovHits" )
    {

      for( count = 0; count < number; ++count )
      {
        CkovHitTextFileIo* hit = new CkovHitTextFileIo( readOneLine( inf ) );
        ckovHitIo.push_back( hit );
      }
    }
    else if( type == "CkovDigits" )
    {
      for( count = 0; count < number; ++count )
      {
        CkovDigitTextFileIo* digit = new CkovDigitTextFileIo( readOneLine( inf ) );
        ckovDigitIo.push_back( digit );
      }
    }
    else if( type == "SciFiHits" )
    {
      for( count = 0; count < number; ++count )
      {
        SciFiHitTextFileIo* hit = new SciFiHitTextFileIo( readOneLine( inf ) );
        sciFiHitIo.push_back( hit );
      }
    }
    else if( type == "SciFiDigits" )
    {
      for( count = 0; count < number; ++count )
      {
        SciFiDigitTextFileIo* hit = new SciFiDigitTextFileIo( readOneLine( inf ) );
        sciFiDigitIo.push_back( hit );
      }
    }
    else if( type == "SciFiDoubletClusters" )
    {
      for( count = 0; count < number; ++count )
      {
        SciFiDoubletClusterTextFileIo* cluster = new SciFiDoubletClusterTextFileIo( readOneLine( inf ) );
        sciFiClusterIo.push_back( cluster );
      }
    }
    else if( type == "SciFiSpacePoints" )
    {
      for( count = 0; count < number; ++count )
      {
        SciFiSpacePointTextFileIo* cluster = new SciFiSpacePointTextFileIo( readOneLine( inf ) );
        sciFiSpacePointIo.push_back( cluster );
      }
    }
    else if( type == "SciFiKalTracks" )
    {
      for( count = 0; count < number; ++count ) 
      {
        double dummy; // this should be filled in
        ist >> dummy;
      }
    }
    else if( type == "EmCalHits" )
    {
      for( count = 0; count < number; ++count )
      {
        EmCalHitTextFileIo* hit = new EmCalHitTextFileIo( readOneLine( inf ) );
        emCalHitIo.push_back( hit );
      }
    }
    else if( type == "EmCalDigits" )
    {
      for( count = 0; count < number; ++count )
      {
        EmCalDigitTextFileIo* digit = new EmCalDigitTextFileIo( readOneLine( inf ) );
        emCalDigitIo.push_back( digit );
      }
    }
    else if( type == "EmCalTracks" )
    {
      for( count = 0; count < number; ++count )
      {
        EmCalTrackTextFileIo* track = new EmCalTrackTextFileIo( readOneLine( inf ) );
        emCalTrackIo.push_back( track );
      }
    }
    else if( type == "VirtualHits" )
    {
      for( count = 0; count < number; ++count )
      {
        VirtualHitTextFileIo* hit = new VirtualHitTextFileIo( readOneLine( inf ) );
        virtualHitIo.push_back( hit );
      }
    }
    else if( type == "SpecialHits" )
    {
      for( count = 0; count < number; ++count )
      {
        SpecialHitTextFileIo* hit = new SpecialHitTextFileIo( readOneLine( inf ) );
        specialHitIo.push_back( hit );
      }
    }
   else if( type == "TofHits" )
    {
      for( count = 0; count < number; ++count )
      {
        TofHitTextFileIo* hit = new TofHitTextFileIo( readOneLine( inf ) );
        tofHitIo.push_back( hit );
      }
    }
   else if( type == "TofSlabHits" )
    {
      for( count = 0; count < number; ++count )
      {
        TofSlabHitTextFileIo* hit = new TofSlabHitTextFileIo( readOneLine( inf ) );
        tofSlabHitIo.push_back( hit );
      }
    }
   else if( type == "TofDigits" )
    {
      for( count = 0; count < number; ++count )
      {
        TofDigitTextFileIo* hit = new TofDigitTextFileIo( readOneLine( inf ) );
        tofDigitIo.push_back( hit );
      }
    }
   else if( type == "TofSpacePoints" )
    {
      for( count = 0; count < number; ++count )
      {
        TofSpacePointTextFileIo* hit = new TofSpacePointTextFileIo( readOneLine( inf ) );
        tofSpacePointIo.push_back( hit );
      }
    }
   else if( type == "VmeAdcHits" )
    {
      for( count = 0; count < number; ++count )
      {
        VmeAdcHitTextFileIo* hit = new VmeAdcHitTextFileIo( readOneLine( inf ) );
        vmeAdcHitIo.push_back( hit );
      }
    }
   else if( type == "VmefAdcHits" )
    {
      for( count = 0; count < number; ++count )
      {
        VmefAdcHitTextFileIo* hit = new VmefAdcHitTextFileIo( readOneLine( inf ) );
        vmefAdcHitIo.push_back( hit );
      }
    }
   else if( type == "VmeTdcHits" )
    {
      for( count = 0; count < number; ++count )
      {
        VmeTdcHitTextFileIo* hit = new VmeTdcHitTextFileIo( readOneLine( inf ) );
        vmeTdcHitIo.push_back( hit );
      }
    }
   else if( type == "VlpcEventTimes" )
    {
      // there should only ever be one of these classes!

      vlpcEventTimeIo = new VlpcEventTimeTextFileIo( readOneLine( inf ) );
    }
   else if( type == "MCParticles" )
    {
//      mcParticleIo.resize(((number - 1)> 0 ? (number-1) : 0));//to avoid negative numbers and seg fault
      mcParticleIo.resize(number);

      for( count = 0; count < number; ++count )
      {
        MCParticleTextFileIo* hit = new MCParticleTextFileIo( readOneLine( inf ) );
        mcParticleIo[count] = hit;
      }
    }
   else if( type == "MCVertices" )
    {
      mcVertexIo.resize( number );
      for( count = 0; count < number; ++count )
      {
        MCVertexTextFileIo* hit = new MCVertexTextFileIo( readOneLine( inf ) );
        mcVertexIo[count] = hit;
      }
    }
   else if( type == "ZustandVektors" )
    {
      zsVectIo.resize( number );
      for( count = 0; count < number; ++count )
      {
        ZustandVektorTextFileIo* vec = new ZustandVektorTextFileIo( readOneLine( inf ) );
        zsVectIo[count] = vec;
      }
    }
    else if( type == "RFData" )
    {
      rfDataIo.resize( number );
      for( count = 0; count < number; ++count )
      {
        rfDataIo[count] = new RFDataTextFileIo( readOneLine( inf ) );
      }
    }
    else if( type == "KillHits" )
    {
      killHitIo.resize( number );
      for( count = 0; count < number; ++count )
      {
        killHitIo[count] = new KillHitTextFileIo( readOneLine( inf ) );
      }
    }
    else if( type == "PrimaryGenHits" )
    {
      primaryHitIo.resize( number );
      for( count = 0; count < number; ++count )
      {
        primaryHitIo[count] = new PrimaryGenHitTextFileIo( readOneLine( inf ) );
      }
    }

    else if( type != "STOP" )
    {
      if (number>0){ // no need to kill the event if it is empty
        std::cerr << "Don't know about the class \"" << type << "\", stopping attempts to read this event" << std::endl;
        number = -1;
      }
    }
    gzerror(inf, &err);
  }

  // now make transient objects

  if( persist.processClass( PersistClasses::VlpcHit ) )
    for( i = 0; i < vlpcHitIo.size(); ++i )
    {
      VlpcHit* hit = vlpcHitIo[i]->makeVlpcHit();
      event.vlpcHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::VlpcEventTime ) && vlpcEventTimeIo )
      event.vlpcEventTime =  vlpcEventTimeIo->makeVlpcEventTime();
  else
    event.vlpcEventTime = NULL;

  if( persist.processClass( PersistClasses::CkovHit ) )
    for( i = 0; i < ckovHitIo.size(); ++i )
    {
      CkovHit* hit = ckovHitIo[i]->makeCkovHit();
      event.ckovHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::CkovDigit ) )
    for( i = 0; i < ckovDigitIo.size(); ++i )
    {
      CkovDigit* digit = ckovDigitIo[i]->makeCkovDigit();
      event.ckovDigits.push_back( digit );
    }

  if( persist.processClass( PersistClasses::SciFiHit ) )
    for( i = 0; i < sciFiHitIo.size(); ++i )
    {
      SciFiHit* hit = sciFiHitIo[i]->makeSciFiHit();
      event.sciFiHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::SciFiDigit ) )
    for( i = 0; i < sciFiDigitIo.size(); ++i )
    {
      SciFiDigit* hit = sciFiDigitIo[i]->makeSciFiDigit();
      event.sciFiDigits.push_back( hit );
    }

  if( persist.processClass(PersistClasses::SciFiDoubletCluster ) )
    for( i = 0; i < sciFiClusterIo.size(); ++i )
    {
      SciFiDoubletCluster* clust = sciFiClusterIo[i]->makeSciFiDoubletCluster();
      event.sciFiClusters.push_back( clust );
    }
  if( persist.processClass( PersistClasses::SciFiSpacePoint ) )
    for( i = 0; i < sciFiSpacePointIo.size(); ++i )
    {
      SciFiSpacePoint* point = sciFiSpacePointIo[i]->makeSciFiSpacePoint();
      point->setRun( run );
      event.sciFiSpacePoints.push_back( point );
    }

  if( persist.processClass( PersistClasses::EmCalHit ) )
    for( i = 0; i < emCalHitIo.size(); ++i )
    {
      EmCalHit* hit = emCalHitIo[i]->makeEmCalHit();
      event.emCalHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::EmCalDigit ) )
    for( i = 0; i < emCalDigitIo.size(); ++i )
    {
      EmCalDigit* hit = emCalDigitIo[i]->makeEmCalDigit();
      event.emCalDigits.push_back( hit );
    }
    
  if( persist.processClass( PersistClasses::EmCalTrack ) )
      for( i = 0; i < emCalTrackIo.size(); ++i )
    {
      EmCalTrack* track = emCalTrackIo[i]->makeEmCalTrack();
      event.emCalTracks.push_back( track );
    }

  if( persist.processClass( PersistClasses::VirtualHit ) )
    for( i = 0; i < virtualHitIo.size(); ++i )
    {
      VirtualHit* hit = virtualHitIo[i]->makeVirtualHit();
      event.virtualHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::SpecialHit ) )
    for( i = 0; i < specialHitIo.size(); ++i )
    {
      SpecialHit* hit = specialHitIo[i]->makeSpecialHit();
      event.specialHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::TofHit ) )
    for( i = 0; i < tofHitIo.size(); ++i )
    {
      TofHit* hit = tofHitIo[i]->makeTofHit();
      event.tofHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::TofDigit ) )
    for( i = 0; i < tofDigitIo.size(); ++i )
    {
      TofDigit* digit = tofDigitIo[i]->makeTofDigit();
      event.tofDigits.push_back( digit );
    }

  if( persist.processClass( PersistClasses::TofSpacePoint ) )
    for( i = 0; i < tofSpacePointIo.size(); ++i )
    {
      TofSpacePoint* spacePoint = tofSpacePointIo[i]->makeTofSpacePoint();
      event.tofSpacePoints.push_back( spacePoint );
    }

  if( persist.processClass( PersistClasses::TofSlabHit ) )
    for( i = 0; i < tofSlabHitIo.size(); ++i )
    {
      TofSlabHit* hit = tofSlabHitIo[i]->makeTofSlabHit();
      event.tofSlabHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::VmeAdcHit ) )
    for( i = 0; i < vmeAdcHitIo.size(); ++i )
    {
      VmeAdcHit* hit = vmeAdcHitIo[i]->makeVmeAdcHit();
      event.vmeAdcHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::VmefAdcHit ) )
    for( i = 0; i < vmefAdcHitIo.size(); ++i )
    {
      VmefAdcHit* hit = vmefAdcHitIo[i]->makeVmefAdcHit();
      event.vmefAdcHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::VmeTdcHit ) )
    for( i = 0; i < vmeTdcHitIo.size(); ++i )
    {
      VmeTdcHit* hit = vmeTdcHitIo[i]->makeVmeTdcHit();
      event.vmeTdcHits.push_back( hit );
    }

  if( persist.processClass( PersistClasses::MCParticle ) )
    for( i = 0; i < mcParticleIo.size(); ++i )
    {
      MCParticle* particle = mcParticleIo[i]->makeMCParticle();
      event.mcParticles.push_back( particle );
    }

  if( persist.processClass( PersistClasses::MCVertex ) )
    for( i = 0; i < mcVertexIo.size(); ++i )
    {
      MCVertex* vertex = mcVertexIo[i]->makeMCVertex();
      event.mcVertices.push_back( vertex );
    }

  if( persist.processClass( PersistClasses::ZustandVektor ) )
    for( i = 0; i < zsVectIo.size(); ++i )
    {
      ZustandVektor* vec = zsVectIo[i]->makeZustandVektor();
      event.zustandVektors.push_back( vec );
    }

  if( persist.processClass( PersistClasses::RFData ) )
    for( i = 0; i < rfDataIo.size(); ++i )
    {
      RFData* data = rfDataIo[i]->makeRFData();
      event.rfData.push_back( data );
    }

  if( persist.processClass( PersistClasses::PrimaryGenHit ) )
    for( i = 0; i < primaryHitIo.size(); ++i )
    {
      PrimaryGenHit* hit = primaryHitIo[i]->makePGHit();
      event.pgHits.push_back( hit );
    }

  // next resolve the links to other transient objects

  if( persist.processClass( PersistClasses::VlpcHit ) )
    for( i = 0; i < vlpcHitIo.size(); ++i )
      vlpcHitIo[i]->completeRestoration();

  if( persist.processClass( PersistClasses::VlpcEventTime ) && vlpcEventTimeIo )
      vlpcEventTimeIo->completeRestoration();

  if( persist.processClass( PersistClasses::SciFiHit ) ){
    for( i = 0; i < sciFiHitIo.size(); ++i ){
      sciFiHitIo[i]->setMCParticles( mcParticleIo );
      sciFiHitIo[i]->completeRestoration();
    }
  }

  if( persist.processClass( PersistClasses::CkovHit ) ){
    for( i = 0; i < ckovHitIo.size(); ++i ){
      ckovHitIo[i]->setMCParticles( mcParticleIo );
      ckovHitIo[i]->completeRestoration();
    }
  }

  if( persist.processClass( PersistClasses::CkovDigit ) ){
    for( i = 0; i < ckovDigitIo.size(); ++i ){
      ckovDigitIo[i]->setCkovHits( &ckovHitIo );
      ckovDigitIo[i]->setVmeAdcHits( vmeAdcHitIo );
      ckovDigitIo[i]->setVmeTdcHits( vmeTdcHitIo );
      ckovDigitIo[i]->setVmefAdcHits( vmefAdcHitIo );
      ckovDigitIo[i]->completeRestoration();
    }
  }

  if( persist.processClass( PersistClasses::SciFiDigit ) )
    for( i = 0; i < sciFiDigitIo.size(); ++i )
    {
      sciFiDigitIo[i]->setSciFiHits( sciFiHitIo );
      sciFiDigitIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::SciFiDoubletCluster ) )
    for( i = 0; i < sciFiClusterIo.size(); ++i )
    {
      sciFiClusterIo[i]->setSciFiDigits( sciFiDigitIo );
      sciFiClusterIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::SciFiSpacePoint ) )
    for( i = 0; i < sciFiSpacePointIo.size(); ++i )
    {
      sciFiSpacePointIo[i]->setSciFiDoubletClusters( sciFiClusterIo );
      sciFiSpacePointIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::EmCalHit ) )
    for( i = 0; i < emCalHitIo.size(); ++i )
    {
      emCalHitIo[i]->setMCParticles( mcParticleIo );
      emCalHitIo[i]->setEmCalDigits( &emCalDigitIo );
      emCalHitIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::EmCalDigit ) )
    for( i = 0; i < emCalDigitIo.size(); ++i )
    {
      emCalDigitIo[i]->setEmCalHits( &emCalHitIo );
      emCalDigitIo[i]->completeRestoration();
    }
    
  if( persist.processClass( PersistClasses::EmCalTrack ) )
      for( i = 0; i < emCalTrackIo.size(); ++i )
    {
      emCalTrackIo[i]->setEmCalDigits( &emCalDigitIo );
      emCalTrackIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::VirtualHit ) )
    for( i = 0; i < virtualHitIo.size(); ++i )
    {
      virtualHitIo[i]->setMCParticles( mcParticleIo );
      virtualHitIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::SpecialHit ) )
    for( i = 0; i < specialHitIo.size(); ++i )
      specialHitIo[i]->completeRestoration();

  if( persist.processClass( PersistClasses::TofHit ) )
    for( i = 0; i < tofHitIo.size(); ++i )
    {
      tofHitIo[i]->setMCParticles( mcParticleIo );
      tofHitIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::TofDigit ) )
    for( i = 0; i < tofDigitIo.size(); ++i )
    {
      tofDigitIo[i]->setVmeAdcHits( vmeAdcHitIo );
      tofDigitIo[i]->setVmefAdcHits( vmefAdcHitIo );      
      tofDigitIo[i]->setVmeTdcHits( vmeTdcHitIo );
      tofDigitIo[i]->setTofSlabHits( tofSlabHitIo );
      tofDigitIo[i]->setTofHits( tofHitIo );
      tofDigitIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::TofSlabHit ) )
    for( i = 0; i < tofSlabHitIo.size(); ++i )
    {
      tofSlabHitIo[i]->setTofDigits( tofDigitIo );
      tofSlabHitIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::TofSpacePoint ) )
    for( i = 0; i < tofSpacePointIo.size(); ++i )
    {
      tofSpacePointIo[i]->setTofSlabHits( tofSlabHitIo );
      tofSpacePointIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::VmeAdcHit ) )
    for( i = 0; i < vmeAdcHitIo.size(); ++i )
      vmeAdcHitIo[i]->completeRestoration();

  if( persist.processClass( PersistClasses::VmefAdcHit ) )
    for( i = 0; i < vmefAdcHitIo.size(); ++i )
      vmefAdcHitIo[i]->completeRestoration();

  if( persist.processClass( PersistClasses::VmeTdcHit ) )
    for( i = 0; i < vmeTdcHitIo.size(); ++i )
      vmeTdcHitIo[i]->completeRestoration();

  if( persist.processClass( PersistClasses::MCParticle ) )
    for( i = 0; i < mcParticleIo.size(); ++i )
    {
      mcParticleIo[i]->setMCVertices( mcVertexIo );
      mcParticleIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::MCVertex ) )
    for( i = 0; i < mcVertexIo.size(); ++i )
    {
      mcVertexIo[i]->setMCParticles( mcParticleIo );
      mcVertexIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::ZustandVektor ) )
    for( i = 0; i < zsVectIo.size(); ++i )
    {
      zsVectIo[i]->completeRestoration();
    }

  if( persist.processClass( PersistClasses::KillHit ) )
    for( i = 0; i < killHitIo.size(); ++i )
      killHitIo[i]->completeRestoration();

  // delete convertor classes

  for( i = 0; i < vlpcHitIo.size(); ++i )
    delete vlpcHitIo[i];

  if( vlpcEventTimeIo )
    delete vlpcEventTimeIo;

  for( i = 0; i < ckovHitIo.size(); ++i )
    delete ckovHitIo[i];

  for( i = 0; i < ckovDigitIo.size(); ++i )
    delete ckovDigitIo[i];

  for( i = 0; i < sciFiHitIo.size(); ++i )
    delete sciFiHitIo[i];

  for( i = 0; i < sciFiDigitIo.size(); ++i )
    delete sciFiDigitIo[i];

  for( i = 0; i < sciFiClusterIo.size(); ++i )
    delete sciFiClusterIo[i];

  for( i = 0; i < sciFiSpacePointIo.size(); ++i )
    delete sciFiSpacePointIo[i];

  for( i = 0; i < emCalHitIo.size(); ++i )
    delete emCalHitIo[i];

  for( i = 0; i < emCalDigitIo.size(); ++i )
    delete emCalDigitIo[i];
  
  for( i = 0; i < emCalTrackIo.size(); ++i )
    delete emCalTrackIo[i];

  for( i = 0; i < virtualHitIo.size(); ++i )
    delete virtualHitIo[i];

  for( i = 0; i < specialHitIo.size(); ++i )
    delete specialHitIo[i];

  for( i = 0; i < tofHitIo.size(); ++i )
    delete tofHitIo[i];

  for( i = 0; i < tofDigitIo.size(); ++i )
    delete tofDigitIo[i];

  for( i = 0; i < tofSlabHitIo.size(); ++i )
    delete tofSlabHitIo[i];

  for( i = 0; i < vmeAdcHitIo.size(); ++i )
    delete vmeAdcHitIo[i];

  for( i = 0; i < vmefAdcHitIo.size(); ++i )
    delete vmefAdcHitIo[i];

  for( i = 0; i < vmeTdcHitIo.size(); ++i )
    delete vmeTdcHitIo[i];

  for( i = 0; i < mcParticleIo.size(); ++i )
    delete mcParticleIo[i];

  for( i = 0; i < mcVertexIo.size(); ++i )
    delete mcVertexIo[i];

  for( i = 0; i < zsVectIo.size(); ++i )
    delete zsVectIo[i];

  for( i = 0; i < rfDataIo.size(); ++i )
    delete rfDataIo[i];

  for( i = 0; i < killHitIo.size(); ++i )
    delete killHitIo[i];

  for( i = 0; i < primaryHitIo.size(); ++i )
    delete primaryHitIo[i];

  return true;
}

void  TextFileReader::closeInputFile()
{
  gzclose( inf );
}

std::string TextFileReader::readOneLine( gzFile fp )
{
  char line[50000];

  gzgets( fp, line, 50000 );

  return ( std::string( line ) );
}

void    TextFileReader::skipEvents( int numSkip )
{
  for( int skip = 0; skip < numSkip; ++skip )
  {
    // read through the input file until we find the keyword STOP

    bool stop = false;

    while( ! stop )
    {
      int number;
      std::string type;

      std::istringstream ist( readOneLine( inf ).c_str() );
      ist >> number >> type;
      if( number == -1 )  // this is the end of event marker
        stop = true;
      else
        for( int line = 0; line < number; ++line ) // read in the information for this class and ignore it
          readOneLine( inf );
    }
  }
}

bool TextFileReader::IsOk() const
{
 return isOk;
}

