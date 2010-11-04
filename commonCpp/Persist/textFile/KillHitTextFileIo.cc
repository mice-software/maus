#include <sstream>

#include "KillHitTextFileIo.hh"

KillHitTextFileIo::KillHitTextFileIo( KillHit* hit, int index ) : TextFileIoBase()
{
  setTxtIoIndex( index );
  m_hit = hit;
}

KillHitTextFileIo::KillHitTextFileIo( std::string def ) : TextFileIoBase()
{
  restoreObject( def );
}

std::string	KillHitTextFileIo::storeObject()
{
  m_trackID = m_hit->trackID();
  m_pdg = m_hit->pdg();
  m_charge = m_hit->charge();
  m_mass = m_hit->mass();
  m_position = m_hit->position();
  m_momentum = m_hit->momentum();
  m_time = m_hit->time();
  m_energy = m_hit->energy();
  m_reason   = m_hit->reasonForDying();

  std::stringstream ss;

  ss << txtIoIndex() << " " << m_trackID << " " << m_pdg << " " << m_charge << " " << m_mass << " " << m_position.x() << " " << m_position.y() << " " << m_position.z() << " " << m_momentum.x() << " " << m_momentum.y() << " " << m_momentum.z() << " " << m_time << " " << m_energy << " " << m_reason;

  return( ss.str() );
}

void		KillHitTextFileIo::restoreObject( std::string def )
{
  int myindex;
  double pos_x, pos_y, pos_z; 
  double mom_x, mom_y, mom_z; 

  std::istringstream ist( def.c_str() );

  ist >> myindex >>  m_trackID >> m_pdg >>  m_charge >> m_mass
      >> pos_x >> pos_y >> pos_z >> mom_x >> mom_y >> mom_z
      >> m_time >> m_energy >> m_reason;

  m_position.set(pos_x, pos_y, pos_z);
  m_momentum.set(mom_x, mom_y, mom_z);

  setTxtIoIndex( myindex );
}

KillHit*	KillHitTextFileIo::makeKillHit()
{
  m_hit = new KillHit(m_trackID, m_pdg, m_charge, m_mass, m_position, m_momentum, m_time, m_energy, m_reason);
  return m_hit;
}


