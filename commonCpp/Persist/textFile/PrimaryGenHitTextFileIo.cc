// MCHitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "PrimaryGenHitTextFileIo.hh"

PrimaryGenHitTextFileIo::PrimaryGenHitTextFileIo( PrimaryGenHit* hit, int index ) : TextFileIoBase()
{
  setTxtIoIndex( index );
  m_hit      = hit;
  m_trackID  = hit->trackID();
  m_pdg      = hit->pdg();
  m_charge   = hit->charge();
  m_mass     = hit->mass();
  m_position = hit->position();
  m_momentum = hit->momentum();
  m_time     = hit->time();
  m_edep     = hit->Edep();
  m_energy   = hit->energy();
  m_weight   = hit->weight();
  m_seed     = hit->seed();
  m_eventID  = hit->eventNumber();
  m_particle_index = 0;

}

PrimaryGenHitTextFileIo::PrimaryGenHitTextFileIo( std::string def ) : TextFileIoBase()
{
  restoreObject( def );
}

std::string	PrimaryGenHitTextFileIo::storeObject()
{
  std::stringstream ss;

  ss << txtIoIndex() << " " << m_trackID << " " << m_pdg << " " << m_charge << " " << m_mass << " " << m_position.x() << " " 
     << m_position.y() << " " << m_position.z() << " " << m_momentum.x() << " " << m_momentum.y() << " " << m_momentum.z() << " " 
     << m_time << " " << m_edep << " " << m_energy << " " << m_weight << " " << m_seed << " " << m_eventID << " " << m_particle_index;

  return( ss.str() );
}

void		PrimaryGenHitTextFileIo::restoreObject( std::string def )
{
  int myindex;
  double pos_x, pos_y, pos_z; 
  double mom_x, mom_y, mom_z; 

  std::istringstream ist( def.c_str() );

  ist >> myindex >>  m_trackID >> m_pdg >>  m_charge >> m_mass
      >> pos_x >> pos_y >> pos_z >> mom_x >> mom_y >> mom_z
      >> m_time >>  m_edep >> m_energy >> m_weight >> m_seed >> m_eventID >> m_particle_index;

  m_position.set(pos_x, pos_y, pos_z);
  m_momentum.set(mom_x, mom_y, mom_z);

  setTxtIoIndex( myindex );
}

PrimaryGenHit*	PrimaryGenHitTextFileIo::makePGHit()
{
  m_hit = new PrimaryGenHit();
  m_hit->setTrackID(m_trackID);
  m_hit->setPdg(m_pdg);
  m_hit->setCharge(m_charge);
  m_hit->setMass(m_mass);
  m_hit->setPosition(m_position);
  m_hit->setMomentum(m_momentum);
  m_hit->setTime(m_time);
  m_hit->setEdep(m_edep);
  m_hit->setEnergy(m_energy);
  m_hit->setWeight(m_weight);
  m_hit->setSeed(m_seed);
  m_hit->setEventNumber(m_eventID);  // possibly you have to use some "set" methods here...

  return m_hit;
}


void		PrimaryGenHitTextFileIo::completeRestoration()
{
  m_hit->setParticle(NULL);
}
