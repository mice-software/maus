// VirtualHitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>
#include <iomanip>

#include "VirtualHitTextFileIo.hh"
#include "MCParticleTextFileIo.hh"

VirtualHitTextFileIo::VirtualHitTextFileIo( VirtualHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VirtualHitTextFileIo );
  m_particle = NULL;	
  setTxtIoIndex( index );
  m_hit = hit;
}

VirtualHitTextFileIo::VirtualHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VirtualHitTextFileIo );
  m_particle = NULL;	
  
  restoreObject( def );
}

std::string	VirtualHitTextFileIo::storeObject()
{
  trackID = m_hit->GetTrackID();
  charge = m_hit->GetCharge();
  pid = m_hit->GetPID();
  mass = m_hit->GetMass();
  pos = m_hit->GetPos();
  time = m_hit->GetTime();
  momentum = m_hit->GetMomentum();
  energy = m_hit->GetEnergy();
  stationNumber = m_hit->GetStationNumber();
  bfield = m_hit->GetBField();
  efield = m_hit->GetEField();
  edep = m_hit->Edep();

  m_particle = m_hit->particle();

  // fill the line with the information about the class here, don't forget the index!
  m_particle_index = -1; //if i cant find the particle index assume it was NULL
  for(unsigned int i=0; i< m_particles->size(); i++) //m_particles is a pointer to a vector of MCParticleTextFileIo*
    {
      if((*m_particles)[i]->theParticle()==m_particle){ 
	m_particle_index = (*m_particles)[i]->txtIoIndex();
      }
    }
  
  std::stringstream ss;
  ss << std::setprecision(10);
//  ss << std::fixed;

  ss << txtIoIndex() << " " << stationNumber << " " << trackID << " " << pid << " " << mass << " " << charge << " " << pos.x() << " " << pos.y() << " " << pos.z() << " " << time << " " << momentum.x() << " " << momentum.y() << " " << momentum.z() << " " << energy << " " << bfield.x() << " " << bfield.y() << " " << bfield.z() << " " << efield.x() << " " << efield.y() << " " << efield.z() << " " << edep << " " << m_particle_index;

  return( ss.str() );
}

void		VirtualHitTextFileIo::restoreObject( std::string def )
{
  int myindex;
	double x, y, z, px, py, pz, bx, by, bz, ex, ey, ez;

  std::istringstream ist( def.c_str() );
  ist >> myindex >> stationNumber >> trackID >> pid >> mass >> charge >> x >> y >> z >> time
      >> px >> py >> pz >> energy >> bx >> by >> bz >> ex >> ey >> ez >> edep >> m_particle_index;

  pos.setX(x);
  pos.setY(y);
  pos.setZ(z);
  momentum.setX(px);
  momentum.setY(py);
  momentum.setZ(pz);
  bfield.setX(bx);
  bfield.setY(by);
  bfield.setZ(bz);
  efield.setX(ex);
  efield.setY(ey);
  efield.setZ(ez);

  setTxtIoIndex( myindex );
}

VirtualHit*	VirtualHitTextFileIo::makeVirtualHit()
{
  m_hit = new VirtualHit();

  m_hit->SetTrackID(trackID);
  m_hit->SetCharge(charge);
  m_hit->SetPID(pid);
  m_hit->SetMass(mass);
  m_hit->SetPos(pos);
  m_hit->SetTime(time);
  m_hit->SetMomentum(momentum);
  m_hit->SetEnergy(energy);
  m_hit->SetStationNumber(stationNumber);
  m_hit->SetBField(bfield);
  m_hit->SetEField(efield);
  m_hit->setEdep( edep );

  return m_hit;
}

void VirtualHitTextFileIo::setMCParticles(std::vector<MCParticleTextFileIo*>& particles)
{
  m_particles = &particles;
}

void		VirtualHitTextFileIo::completeRestoration()
{

  for(unsigned int j=0; j<(*m_particles).size(); j++){
    if((*m_particles)[j]->txtIoIndex() == m_particle_index){
      m_particle = (*m_particles)[j]->theParticle();
    }
  }

  m_hit->setParticle(m_particle);

}
