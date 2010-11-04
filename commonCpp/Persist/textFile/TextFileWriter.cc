// TextFileWriter.cc
//
// M.Ellis 22/8/2005


#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include "Interface/Squeak.hh"
#include "TextFileWriter.hh"
#include "VlpcHitTextFileIo.hh"
#include "VlpcEventTimeTextFileIo.hh"
#include "VmeAdcHitTextFileIo.hh"
#include "VmefAdcHitTextFileIo.hh"
#include "VmeTdcHitTextFileIo.hh"
#include "CkovHitTextFileIo.hh"
#include "CkovDigitTextFileIo.hh"
#include "SciFiHitTextFileIo.hh"
#include "SciFiDigitTextFileIo.hh"
#include "SciFiDoubletClusterTextFileIo.hh"
#include "SciFiSpacePointTextFileIo.hh"
#include "SciFiTrackTextFileIo.hh"
#include "SciFiKalTrackTextFileIo.hh"
#include "SciFiExtrapTextFileIo.hh"
#include "KLHitTextFileIo.hh"
#include "EmCalHitTextFileIo.hh"
#include "EmCalDigitTextFileIo.hh"
#include "EmCalTrackTextFileIo.hh"
#include "VirtualHitTextFileIo.hh"
#include "SpecialHitTextFileIo.hh"
#include "TofHitTextFileIo.hh"
#include "TofDigitTextFileIo.hh"
#include "TofSlabHitTextFileIo.hh"
#include "TofSpacePointTextFileIo.hh"
#include "MCParticleTextFileIo.hh"
#include "MCVertexTextFileIo.hh"
#include "ZustandVektorTextFileIo.hh"
#include "RFDataTextFileIo.hh"
#include "KillHitTextFileIo.hh"
#include "PrimaryGenHitTextFileIo.hh"

extern int system( char* );

TextFileWriter::TextFileWriter( std::string filename , MICEEvent& e, PersistClasses& p ) :
  event( e ), persist( p )
{
  miceMemory.addNew( Memory::TextFileWriter ); 
  
  maxWrite = 2000000000;
  fileName = filename;
  fileNum = 0;
  outf = gzopen( filename.c_str(), "w" );
}

void  TextFileWriter::writeEvent()
{
  unsigned int i;

  // Make TextFileIo objects for each transient object

  std::vector<VlpcHitTextFileIo*>               vlpcHitIo;
  VlpcEventTimeTextFileIo*                      vlpcEventTimeIo = NULL;
  std::vector<CkovHitTextFileIo*>               ckovHitIo;
  std::vector<CkovDigitTextFileIo*>             ckovDigitIo;
  std::vector<SciFiHitTextFileIo*>              sciFiHitIo;
  std::vector<SciFiDigitTextFileIo*>            sciFiDigitIo;
  std::vector<SciFiDoubletClusterTextFileIo*>   sciFiClusterIo;
  std::vector<SciFiSpacePointTextFileIo*>       sciFiSpacePointIo;
  std::vector<SciFiTrackTextFileIo*>            sciFiTrackIo;
  std::vector<SciFiKalTrackTextFileIo*>         sciFiKalTrackIo;
  std::vector<SciFiExtrapTextFileIo*>           sciFiExtrapIo;
  std::vector<KLHitTextFileIo*>                 klHitIo;
  std::vector<EmCalHitTextFileIo*>              emCalHitIo;
  std::vector<EmCalDigitTextFileIo*>            emCalDigitIo;
  std::vector<EmCalTrackTextFileIo*>            emCalTrackIo;
  std::vector<VirtualHitTextFileIo*>            virtualHitIo;
  std::vector<SpecialHitTextFileIo*>            specialHitIo;
  std::vector<TofHitTextFileIo*>                tofHitIo;
  std::vector<TofDigitTextFileIo*>              tofDigitIo;
  std::vector<TofSlabHitTextFileIo*>            tofSlabHitIo;
  std::vector<TofSpacePointTextFileIo*>         tofSpacePointIo;
  std::vector<VmeAdcHitTextFileIo*>             vmeAdcHitIo;
  std::vector<VmefAdcHitTextFileIo*>            vmefAdcHitIo;
  std::vector<VmeTdcHitTextFileIo*>             vmeTdcHitIo;
  std::vector<MCParticleTextFileIo*>            mcParticleIo;
  std::vector<MCVertexTextFileIo*>              mcVertexIo;
  std::vector<ZustandVektorTextFileIo*>         zustandVektorIo;
  std::vector<RFDataTextFileIo*>                rfDataIo;
  std::vector<KillHitTextFileIo*>               killHitIo;
  std::vector<PrimaryGenHitTextFileIo*>         primaryGenHitIo;

  for( i = 0; i < event.vlpcHits.size(); ++i )
    vlpcHitIo.push_back( new VlpcHitTextFileIo( event.vlpcHits[i], i ) );

  if( event.vlpcEventTime )
    vlpcEventTimeIo = new VlpcEventTimeTextFileIo( event.vlpcEventTime, 0 );

  for( i = 0; i < event.ckovHits.size(); ++i )
    ckovHitIo.push_back( new CkovHitTextFileIo( event.ckovHits[i], i ) );

  for( i = 0; i < event.ckovDigits.size(); ++i )
    ckovDigitIo.push_back( new CkovDigitTextFileIo( event.ckovDigits[i], i ) );

  for( i = 0; i < event.sciFiHits.size(); ++i )
    sciFiHitIo.push_back( new SciFiHitTextFileIo( event.sciFiHits[i], i ) );

  for( i = 0; i < event.sciFiDigits.size(); ++i )
    sciFiDigitIo.push_back( new SciFiDigitTextFileIo( event.sciFiDigits[i], i ) );

  for( i = 0; i < event.sciFiClusters.size(); ++i )
    sciFiClusterIo.push_back( new SciFiDoubletClusterTextFileIo( event.sciFiClusters[i], i ) );

  for( i = 0; i < event.sciFiSpacePoints.size(); ++i )
    sciFiSpacePointIo.push_back( new SciFiSpacePointTextFileIo( event.sciFiSpacePoints[i], i ) );

  for( i = 0; i < event.sciFiTracks.size(); ++i )
    sciFiTrackIo.push_back( new SciFiTrackTextFileIo( event.sciFiTracks[i], i ) );

  for( i = 0; i < event.sciFiKalTracks.size(); ++i )
    sciFiKalTrackIo.push_back( new SciFiKalTrackTextFileIo( event.sciFiKalTracks[i], i ) );

  for( i = 0; i < event.sciFiExtraps.size(); ++i )
    sciFiExtrapIo.push_back( new SciFiExtrapTextFileIo( event.sciFiExtraps[i], i ) );

  for( i = 0; i < event.klHits.size(); ++i )
    klHitIo.push_back( new KLHitTextFileIo( event.klHits[i], i ) );

  for( i = 0; i < event.emCalHits.size(); ++i )
    emCalHitIo.push_back( new EmCalHitTextFileIo( event.emCalHits[i], i ) );

  for( i = 0; i < event.emCalDigits.size(); ++i )
    emCalDigitIo.push_back( new EmCalDigitTextFileIo( event.emCalDigits[i], i ) );

  for( i = 0; i < event.emCalTracks.size(); ++i )
    emCalTrackIo.push_back( new EmCalTrackTextFileIo( event.emCalTracks[i], i ) );

  for( i = 0; i < event.virtualHits.size(); ++i )
    virtualHitIo.push_back( new VirtualHitTextFileIo( event.virtualHits[i], i ) );

  for( i = 0; i < event.specialHits.size(); ++i )
    specialHitIo.push_back( new SpecialHitTextFileIo( event.specialHits[i], i ) );

  for( i = 0; i < event.tofHits.size(); ++i )
    tofHitIo.push_back( new TofHitTextFileIo( event.tofHits[i], i ) );

  for( i = 0; i < event.tofDigits.size(); ++i )
    tofDigitIo.push_back( new TofDigitTextFileIo( event.tofDigits[i], i ) );

  for( i = 0; i < event.tofSlabHits.size(); ++i )
    tofSlabHitIo.push_back( new TofSlabHitTextFileIo( event.tofSlabHits[i], i ) );

  for( i = 0; i < event.tofSpacePoints.size(); ++i )
    tofSpacePointIo.push_back( new TofSpacePointTextFileIo( event.tofSpacePoints[i], i ) );

  for( i = 0; i < event.vmeAdcHits.size(); ++i )
    vmeAdcHitIo.push_back( new VmeAdcHitTextFileIo( event.vmeAdcHits[i], i ) );

  for( i = 0; i < event.vmefAdcHits.size(); ++i )
    vmefAdcHitIo.push_back( new VmefAdcHitTextFileIo( event.vmefAdcHits[i], i ) );

  for( i = 0; i < event.vmeTdcHits.size(); ++i )
    vmeTdcHitIo.push_back( new VmeTdcHitTextFileIo( event.vmeTdcHits[i], i ) );

  mcParticleIo.resize( event.mcParticles.size() );
  for( i = 0; i < event.mcParticles.size(); ++i )
    mcParticleIo[i] = new MCParticleTextFileIo( event.mcParticles[i], i );

  for( i = 0; i < event.mcVertices.size(); ++i )
    mcVertexIo.push_back( new MCVertexTextFileIo( event.mcVertices[i], i ) );

  for( i = 0; i < event.zustandVektors.size(); ++i )
    zustandVektorIo.push_back( new ZustandVektorTextFileIo( event.zustandVektors[i], i ) );

  for( i = 0; i < event.rfData.size(); ++i )
    rfDataIo.push_back( new RFDataTextFileIo( event.rfData[i], i ) );

  for( i = 0; i < event.killHits.size(); ++i )
    killHitIo.push_back( new KillHitTextFileIo( event.killHits[i], i ) );

  for( i = 0; i < event.pgHits.size(); ++i )
    primaryGenHitIo.push_back( new PrimaryGenHitTextFileIo( event.pgHits[i], i ) );


  // now write classes to output file
  if( persist.processClass( PersistClasses::RFData ) )
  {
    gzprintf( outf, "%d RFData\n", rfDataIo.size() );

    for( i = 0; i < rfDataIo.size(); ++i )
      gzprintf( outf, "%s\n", rfDataIo[i]->storeObject().c_str() );
  }

  if( persist.processClass( PersistClasses::VlpcHit ) )
  {
    gzprintf( outf, "%d VlpcHits\n", vlpcHitIo.size() );

    for( i = 0; i < vlpcHitIo.size(); ++i )
      gzprintf( outf, "%s\n", vlpcHitIo[i]->storeObject().c_str() );
  }

  if( persist.processClass( PersistClasses::VlpcEventTime ) && vlpcEventTimeIo )
  {
    gzprintf( outf, "%d VlpcEventTimes\n", 1 );
    gzprintf( outf, "%s\n", vlpcEventTimeIo->storeObject().c_str() );
  }

  if( persist.processClass( PersistClasses::CkovHit ) )
  {
    gzprintf( outf, "%d CkovHits\n", ckovHitIo.size() );

    for( i = 0; i < ckovHitIo.size(); ++i )
    {
      ckovHitIo[i]->setMCParticles( mcParticleIo);
      gzprintf( outf, "%s\n", ckovHitIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::CkovDigit ) )
  {
    gzprintf( outf, "%d CkovDigits\n", ckovDigitIo.size() );

    for( i = 0; i < ckovDigitIo.size(); ++i )
    {
      ckovDigitIo[i]->setCkovHits( &ckovHitIo );
      gzprintf( outf, "%s\n", ckovDigitIo[i]->storeObject().c_str() );

    }
  }

  if( persist.processClass( PersistClasses::SciFiHit ) )
  {
    gzprintf( outf, "%d SciFiHits\n", sciFiHitIo.size() );

    for( i = 0; i < sciFiHitIo.size(); ++i ) 
    {
      sciFiHitIo[i]->setMCParticles( mcParticleIo );
      gzprintf( outf, "%s\n", sciFiHitIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::SciFiDigit ) )
  {
    gzprintf( outf, "%d SciFiDigits\n", sciFiDigitIo.size() );

    for( i = 0; i < sciFiDigitIo.size(); ++i )
    {
      sciFiDigitIo[i]->setSciFiHits( sciFiHitIo );
      gzprintf( outf, "%s\n", sciFiDigitIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::SciFiDoubletCluster ) )
  {
    gzprintf( outf, "%d SciFiDoubletClusters\n", sciFiClusterIo.size() );

    for( i = 0; i < sciFiClusterIo.size(); ++i )
    {
      sciFiClusterIo[i]->setSciFiDigits( sciFiDigitIo );
      gzprintf( outf, "%s\n", sciFiClusterIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::SciFiSpacePoint ) )
  {
    gzprintf( outf, "%d SciFiSpacePoints\n", sciFiSpacePointIo.size() );

    for( i = 0; i < sciFiSpacePointIo.size(); ++i )
    {
      sciFiSpacePointIo[i]->setSciFiDoubletClusters( sciFiClusterIo );
      gzprintf( outf, "%s\n", sciFiSpacePointIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::SciFiTrack ) )
  {
    gzprintf( outf, "%d SciFiTracks\n", sciFiTrackIo.size() );

    for( i = 0; i < sciFiTrackIo.size(); ++i )
    {
      //sciFiTrackIo[i]->setSciFiDigits( sciFiDigitIo );
      gzprintf( outf, "%s\n", sciFiTrackIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::SciFiKalTrack ) )
  {
    gzprintf( outf, "%d SciFiKalTracks\n", sciFiKalTrackIo.size() );

    for( i = 0; i < sciFiKalTrackIo.size(); ++i )
    {
      //sciFiKalTrackIo[i]->setSciFiDigits( sciFiDigitIo );
      gzprintf( outf, "%s\n", sciFiKalTrackIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::SciFiExtrap ) )
  {
    gzprintf( outf, "%d SciFiExtraps\n", sciFiExtrapIo.size() );

    for( i = 0; i < sciFiExtrapIo.size(); ++i )
    {
      //sciFiExtrapIo[i]->setSciFiDigits( sciFiDigitIo );
      gzprintf( outf, "%s\n", sciFiExtrapIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::KLHit ) )
  {
    gzprintf( outf, "%d KLHits\n", klHitIo.size() );

    for( i = 0; i < klHitIo.size(); ++i )
    {
      klHitIo[i]->setMCParticles( mcParticleIo);
      gzprintf( outf, "%s\n", klHitIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::EmCalHit ) )
  {
    gzprintf( outf, "%d EmCalHits\n", emCalHitIo.size() );

    for( i = 0; i < emCalHitIo.size(); ++i )
    {
      emCalHitIo[i]->setEmCalDigits( &emCalDigitIo );
      emCalHitIo[i]->setMCParticles( mcParticleIo);
      gzprintf( outf, "%s\n", emCalHitIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::EmCalDigit ) )
  {
    gzprintf( outf, "%d EmCalDigits\n", emCalDigitIo.size() );

    for( i = 0; i < emCalDigitIo.size(); ++i )
    {
      emCalDigitIo[i]->setEmCalHits( &emCalHitIo );
      gzprintf( outf, "%s\n", emCalDigitIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::EmCalTrack ) )
  {
    gzprintf( outf, "%d EmCalTracks\n", emCalTrackIo.size() );

    for( i = 0; i < emCalTrackIo.size(); ++i )
    {
      emCalTrackIo[i]->setEmCalDigits( &emCalDigitIo );
      gzprintf( outf, "%s\n", emCalTrackIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::VirtualHit ) )
  {
    gzprintf( outf, "%d VirtualHits\n", virtualHitIo.size() );

    for( i = 0; i < virtualHitIo.size(); ++i ){
      virtualHitIo[i]->setMCParticles( mcParticleIo);
      gzprintf( outf, "%s\n", virtualHitIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::SpecialHit ) )
  {
    gzprintf( outf, "%d SpecialHits\n", specialHitIo.size() );

    for( i = 0; i < specialHitIo.size(); ++i )
      gzprintf( outf, "%s\n", specialHitIo[i]->storeObject().c_str() );
  }

  if( persist.processClass( PersistClasses::TofHit ) )
  {
    gzprintf( outf, "%d TofHits\n", tofHitIo.size() );

    for( i = 0; i < tofHitIo.size(); ++i ){
      tofHitIo[i]->setMCParticles( mcParticleIo );
      gzprintf( outf, "%s\n", tofHitIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::TofDigit ) )
  {
    gzprintf( outf, "%d TofDigits\n", tofDigitIo.size() );

    for( i = 0; i < tofDigitIo.size(); ++i )
      {
		tofDigitIo[i]->setVmeAdcHits( vmeAdcHitIo );
		tofDigitIo[i]->setVmefAdcHits( vmefAdcHitIo );		
		tofDigitIo[i]->setVmeTdcHits( vmeTdcHitIo );
		tofDigitIo[i]->setTofSlabHits( tofSlabHitIo );
		tofDigitIo[i]->setTofHits( tofHitIo );
		gzprintf( outf, "%s\n", tofDigitIo[i]->storeObject().c_str() );
      }
  }

  if( persist.processClass( PersistClasses::TofSlabHit ) )
  {
    gzprintf( outf, "%d TofSlabHits\n", tofSlabHitIo.size() );

    for( i = 0; i < tofSlabHitIo.size(); ++i ){
		tofSlabHitIo[i]->setTofDigits( tofDigitIo );
		gzprintf( outf, "%s\n", tofSlabHitIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::TofSpacePoint ) )
  {
    gzprintf( outf, "%d TofSpacePoints\n", tofSpacePointIo.size() );

    for( i = 0; i < tofSpacePointIo.size(); ++i ){
	tofSpacePointIo[i]->setTofSlabHits( tofSlabHitIo );
      	gzprintf( outf, "%s\n", tofSpacePointIo[i]->storeObject().c_str() );
    }
  }

  if( persist.processClass( PersistClasses::VmeAdcHit ) )
  {
    gzprintf( outf, "%d VmeAdcHits\n", vmeAdcHitIo.size() );

    for( i = 0; i < vmeAdcHitIo.size(); ++i )
      gzprintf( outf, "%s\n", vmeAdcHitIo[i]->storeObject().c_str() );
  }

  if( persist.processClass( PersistClasses::VmefAdcHit ) )
  {
    gzprintf( outf, "%d VmefAdcHits\n", vmefAdcHitIo.size() );

    for( i = 0; i < vmefAdcHitIo.size(); ++i )
      gzprintf( outf, "%s\n", vmefAdcHitIo[i]->storeObject().c_str() );
  }


 if( persist.processClass( PersistClasses::VmeTdcHit ) )
  {
    gzprintf( outf, "%d VmeTdcHits\n", vmeTdcHitIo.size() );

    for( i = 0; i < vmeTdcHitIo.size(); ++i )
      gzprintf( outf, "%s\n", vmeTdcHitIo[i]->storeObject().c_str() );
  }

 if( persist.processClass( PersistClasses::MCParticle ) )
  {
    gzprintf( outf, "%d MCParticles\n", mcParticleIo.size() );

    for( i = 0; i < mcParticleIo.size(); ++i )
      {
  mcParticleIo[i]->setMCVertices( mcVertexIo );
  gzprintf( outf, "%s\n", mcParticleIo[i]->storeObject().c_str() );
      }
  }

 if( persist.processClass( PersistClasses::MCVertex ) )
  {
    gzprintf( outf, "%d MCVertices\n", mcVertexIo.size() );

    for( i = 0; i < mcVertexIo.size(); ++i )
      {
  mcVertexIo[i]->setMCParticles( mcParticleIo);
  gzprintf( outf, "%s\n", mcVertexIo[i]->storeObject().c_str() );
      }
  }

 if( persist.processClass( PersistClasses::ZustandVektor ) )
  {
    gzprintf( outf, "%d ZustandVektors\n", zustandVektorIo.size() );

    for( i = 0; i < zustandVektorIo.size(); ++i )
      gzprintf( outf, "%s\n", zustandVektorIo[i]->storeObject().c_str() );
  }

 if( persist.processClass( PersistClasses::KillHit ) )
  {
    gzprintf( outf, "%d KillHits\n", killHitIo.size() );

    for( i = 0; i < killHitIo.size(); ++i )
      gzprintf( outf, "%s\n", killHitIo[i]->storeObject().c_str() );
  }

 if( persist.processClass( PersistClasses::PrimaryGenHit ) )
  {
    gzprintf( outf, "%d PrimaryGenHits\n", primaryGenHitIo.size() );

    for( i = 0; i < primaryGenHitIo.size(); ++i )
      gzprintf( outf, "%s\n", primaryGenHitIo[i]->storeObject().c_str() );
  }

  // write end of event line

  gzprintf( outf, "%d STOP\n", -1 );

  int num_written = gztell( outf );

  if( num_written > maxWrite )
    openNewFile();

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

  for( i = 0; i < sciFiTrackIo.size(); ++i )
    delete sciFiTrackIo[i];

  for( i = 0; i < sciFiKalTrackIo.size(); ++i )
    delete sciFiKalTrackIo[i];

  for( i = 0; i < sciFiExtrapIo.size(); ++i )
    delete sciFiExtrapIo[i];

  for( i = 0; i < klHitIo.size(); ++i )
    if (klHitIo[i]) delete klHitIo[i];

  for( i = 0; i < emCalHitIo.size(); ++i )
    if (emCalHitIo[i]) delete emCalHitIo[i];

  for( i = 0; i < emCalDigitIo.size(); ++i )
    if (emCalDigitIo[i]) delete emCalDigitIo[i];

  for( i = 0; i < emCalTrackIo.size(); ++i )
    if (emCalTrackIo[i]) delete emCalTrackIo[i];

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

  for( i = 0; i < tofSpacePointIo.size(); ++i )
    delete tofSpacePointIo[i];

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

  for( i = 0; i < zustandVektorIo.size(); ++i )
    delete zustandVektorIo[i];

  for( i = 0; i < rfDataIo.size(); ++i )
    delete rfDataIo[i];

  for( i = 0; i < killHitIo.size(); ++i )
    delete killHitIo[i];
}

void  TextFileWriter::closeOutputFile()
{
  gzclose( outf );
}

void  TextFileWriter::openNewFile()
{
  // close the existing file

  gzclose( outf );

  // increment the file counter

  ++fileNum;

  // create a new file name

  std::stringstream strm;
  strm << fileNum;

  std::string newName = fileName + "." + strm.str();

  // open the new file

  outf = gzopen( newName.c_str(), "w" );
}
