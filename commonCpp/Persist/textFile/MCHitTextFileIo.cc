// MCHitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "MCHitTextFileIo.hh"
#include "MCParticleTextFileIo.hh"

MCHitTextFileIo::MCHitTextFileIo( MCHit* hit, int index ) : TextFileIoBase()
{
  setTxtIoIndex( index );
  m_hit = hit;
}

MCHitTextFileIo::MCHitTextFileIo( std::string def ) : TextFileIoBase()
{
  restoreObject( def );
}

std::string	MCHitTextFileIo::storeObject()
{
  m_trackID = m_hit->trackID();
  m_pdg = m_hit->pdg();
  m_charge = m_hit->charge();
  m_mass = m_hit->mass();
  m_position = m_hit->position();
  m_momentum = m_hit->momentum();
  m_time = m_hit->time();
  m_edep = m_hit->Edep();
  m_energy = m_hit->energy();
  m_particle = m_hit->particle();

  // fill the line with the information about the class here, don't forget the index!
  for(unsigned int i=0; i< m_particles->size(); i++)
    {
      if((*m_particles)[i]->theParticle()==m_particle){ 
	m_particle_index = (*m_particles)[i]->txtIoIndex();
      }
    }

  std::stringstream ss;

  ss << txtIoIndex() << " " << m_trackID << " " << m_pdg << " " << m_charge << " " << m_mass << " " << m_position.x() << " " << m_position.y() << " " << m_position.z() << " " << m_momentum.x() << " " << m_momentum.y() << " " << m_momentum.z() << " " << m_time << " " << m_edep << " " << m_energy << " " << m_particle_index;

  return( ss.str() );
}

void		MCHitTextFileIo::restoreObject( std::string def )
{
  int myindex;
  double pos_x, pos_y, pos_z; 
  double mom_x, mom_y, mom_z; 

  std::istringstream ist( def.c_str() );

  ist >> myindex >>  m_trackID >> m_pdg >>  m_charge >> m_mass
      >> pos_x >> pos_y >> pos_z >> mom_x >> mom_y >> mom_z
      >> m_time >>  m_edep >> m_energy>> m_particle_index;

  m_position.set(pos_x, pos_y, pos_z);
  m_momentum.set(mom_x, mom_y, mom_z);

  setTxtIoIndex( myindex );
}

MCHit*	MCHitTextFileIo::makeMCHit()
{
  m_hit = new MCHit(m_trackID, m_pdg, m_charge, m_mass, m_position, m_momentum, m_time, m_edep, m_energy);

  // possibly you have to use some "set" methods here...

  return m_hit;
}


void MCHitTextFileIo::setMCParticles(std::vector<MCParticleTextFileIo*>& particles)

{
  m_particles = &particles;
}

void		MCHitTextFileIo::completeRestoration()
{

  for(unsigned int j=0; j<(*m_particles).size(); j++){
    if((*m_particles)[j]->txtIoIndex() == m_particle_index){
      m_particle = (*m_particles)[j]->theParticle();
    }
  }

  m_hit->setParticle(m_particle);
}
