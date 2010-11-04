// SciFiHitTextFileIo.cc
//
// M.Ellis 22/8/2005

#include <sstream>

#include "SciFiHitTextFileIo.hh"
#include "MCParticleTextFileIo.hh"

SciFiHitTextFileIo::SciFiHitTextFileIo( SciFiHit* hit, int index ) : TextFileIoBase(), m_particles(NULL)
{
  miceMemory.addNew( Memory::SciFiHitTextFileIo );
  
  setTxtIoIndex( index );
  m_hit = hit;
}

SciFiHitTextFileIo::SciFiHitTextFileIo( std::string def ) : TextFileIoBase(), m_particles(NULL)
{
  miceMemory.addNew( Memory::SciFiHitTextFileIo );
  
  restoreObject( def );
}

std::string	SciFiHitTextFileIo::storeObject()
{
  trackID = m_hit->GetTrackID();
  charge = m_hit->GetCharge();
  pid = m_hit->GetPID();
  mass = m_hit->GetMass();
  edep = m_hit->GetEdep();
  pos = m_hit->GetPos();
  time = m_hit->GetTime();
  mom = m_hit->GetMom();
  energy = m_hit->GetEnergy();
  trackerNo = m_hit->GetTrackerNo();
  stationNo = m_hit->GetStationNo();
  planeNo = m_hit->GetPlaneNo();
  fiberNo = m_hit->GetFiberNo();
  volumeName = m_hit->GetVolumeName();

  m_particle = m_hit->particle();

  m_particle_index = -1;

  // fill the line with the information about the class here, don't forget the index!
  for(unsigned int i=0; i< m_particles->size(); i++)
    {
      if( (*m_particles)[i]->theParticle() == m_particle )
      { 
	m_particle_index = (*m_particles)[i]->txtIoIndex();
      }
    }

  std::stringstream ss;

  ss << txtIoIndex() << " " << trackID << " " << charge << " " << pid << " " << mass << " " << edep << " " << pos.x() << " " << pos.y() << " " << pos.z() << " " << time << " " << mom.x() << " " << mom.y() << " " << mom.z() << " " << energy << " " << trackerNo << " " << stationNo << " " << planeNo << " " << fiberNo << " " << volumeName.c_str() << " " << m_particle_index;

  return( ss.str() );
}

void		SciFiHitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  double x, y, z, px, py, pz;

  ist >> myindex >> trackID >> charge >> pid >> mass >> edep >> x >> y >> z >> time
      >> px >> py >> pz >> energy >> trackerNo >> stationNo >> planeNo >> fiberNo >> volumeName >> m_particle_index;

  pos.setX( x );
  pos.setY( y );
  pos.setZ( z );

  mom.setX( px );
  mom.setY( py );
  mom.setZ( pz );

  setTxtIoIndex( myindex );
}

SciFiHit*	SciFiHitTextFileIo::makeSciFiHit()
{
  m_hit = new SciFiHit();

  m_hit->SetTrackID( trackID );
  m_hit->SetCharge( charge );
  m_hit->SetPID( pid );
  m_hit->SetMass( mass );
  m_hit->SetEdep( edep );
  m_hit->SetPos( pos );
  m_hit->SetTime( time );
  m_hit->SetMom( mom );
  m_hit->SetEnergy( energy );
  m_hit->SetTrackerNo( trackerNo );
  m_hit->SetStationNo( stationNo );
  m_hit->SetPlaneNo( planeNo );
  m_hit->SetFiberNo( fiberNo );
  m_hit->SetVolumeName( volumeName );

  return m_hit;
}

void SciFiHitTextFileIo::setMCParticles(std::vector<MCParticleTextFileIo*>& particles)

{
  m_particles = &particles;
}

void		SciFiHitTextFileIo::completeRestoration()
{
  m_particle = NULL;

  for(unsigned int j=0; j<(*m_particles).size(); j++){
    if((*m_particles)[j]->txtIoIndex() == m_particle_index){
      m_particle = (*m_particles)[j]->theParticle();
    }
  }

  m_hit->setParticle(m_particle);
}
