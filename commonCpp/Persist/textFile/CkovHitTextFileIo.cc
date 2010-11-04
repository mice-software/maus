// CkovHitTextFileIo.cc
//
// M.Ellis 16/8/2006

#include <sstream>
#include <iostream>
#include "Interface/CkovDigit.hh"
#include "CkovHitTextFileIo.hh"
#include "MCParticleTextFileIo.hh"


CkovHitTextFileIo::CkovHitTextFileIo( CkovHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::CkovHitTextFileIo );

  setTxtIoIndex( index );
  m_hit = hit;
}

CkovHitTextFileIo::CkovHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::CkovHitTextFileIo );
  	
  restoreObject( def );
}

std::string	CkovHitTextFileIo::storeObject()
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
  planeNo = m_hit->GetPlaneNo();
  volumeName = m_hit->GetVolumeName();

  wavelenght = m_hit->GetWavelenght();
  Iangle = m_hit->GetAngle();
  Pangle = m_hit->GetPAngle();
  pmtNo = m_hit->GetPmtNo();
  polarization = m_hit->GetPol();

  m_particle = m_hit->particle();

  // fill the line with the information about the class here, don't forget the index!
  for(unsigned int i=0; i< m_particles->size(); i++)
    {
      if((*m_particles)[i]->theParticle()==m_particle){
	m_particle_index = (*m_particles)[i]->txtIoIndex();
      }
    }

  std::stringstream ss;

  ss << txtIoIndex() << " " << trackID << " " << charge << " " << pid << " " << mass << " " << edep << " " << pos.x() << " " << pos.y() << " " << pos.z() << " " << time << " " << mom.x() << " " << mom.y() << " " << mom.z() << " " << energy << " " << planeNo << " " << volumeName.c_str() << " " << m_particle_index << " " << wavelenght <<  " " << Iangle <<  " " << Pangle <<  " " <<  pmtNo << " " << polarization.x() << " " << polarization.y() << " " << polarization.z();

  return( ss.str() );

}

void		CkovHitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  double x, y, z, px, py, pz, polx, poly, polz;

  ist >> myindex >> trackID >> charge >> pid >> mass >> edep >> x >> y >> z >> time
      >> px >> py >> pz >> energy >> planeNo >> volumeName >> m_particle_index >>wavelenght >> Iangle >> Pangle >> pmtNo >> polx >> poly >> polz;

  pos.setX( x );
  pos.setY( y );
  pos.setZ( z );

  mom.setX( px );
  mom.setY( py );
  mom.setZ( pz );

  polarization.setX( polx );
  polarization.setY( poly );
  polarization.setZ( polz );

  setTxtIoIndex( myindex );
}

CkovHit*	CkovHitTextFileIo::makeCkovHit()
{
  m_hit = new CkovHit();

  m_hit->SetTrackID( trackID );
  m_hit->SetCharge( charge );
  m_hit->SetPID( pid );
  m_hit->SetMass( mass );
  m_hit->SetEdep( edep );
  m_hit->SetPos( pos );
  m_hit->SetTime( time );
  m_hit->SetMom( mom );
  m_hit->SetEnergy( energy );
  m_hit->SetPlaneNo( planeNo );
  m_hit->SetVolumeName( volumeName );

  m_hit->SetWavelenght( wavelenght );
  m_hit->SetAngle( Iangle );
  m_hit->SetPAngle( Pangle );
  m_hit->SetPmtNo( pmtNo );
  m_hit->SetPol( polarization );

  return m_hit;
}

void CkovHitTextFileIo::setMCParticles(std::vector<MCParticleTextFileIo*>& particles)

{
  m_particles = &particles;
}

void		CkovHitTextFileIo::completeRestoration()
{

  for(unsigned int j=0; j<(*m_particles).size(); j++){
    if((*m_particles)[j]->txtIoIndex() == m_particle_index){
      m_particle = (*m_particles)[j]->theParticle();
    }
  }

  m_hit->setParticle(m_particle);

}
