// KLHitTextFileIo.cc
//

#include <sstream>

#include "KLHitTextFileIo.hh"
#include "MCParticleTextFileIo.hh"

KLHitTextFileIo::KLHitTextFileIo( KLHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::KLHitTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

KLHitTextFileIo::KLHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::KLHitTextFileIo );
  
  restoreObject( def );
}

std::string	KLHitTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!
    _trackID = m_hit->GetTrackID();
    _pid = m_hit->GetPID();
    _copyNumber = m_hit->GetCopyNumber();
    _cellNumber = m_hit->GetCellNumber();
    _position = m_hit->GetPosition() ;
    _time = m_hit->GetTime();
    _mom = m_hit->GetMomentum();
    _charge = m_hit->charge();
    _mass = m_hit->mass();
    _energy = m_hit->GetEnergy();
    _edep = m_hit->GetEdep();


  m_particle = m_hit->particle();

  // fill the line with the information about the class here, don't forget the index!
  for(unsigned int i=0; i< m_particles->size(); i++)
    {
      if((*m_particles)[i]->theParticle()==m_particle){ 
	m_particle_index = (*m_particles)[i]->txtIoIndex();
      }
    }

  std::stringstream ss;

  ss << txtIoIndex() << " " << _trackID << " " << _pid << " " << _copyNumber << " " << _cellNumber << " " <<  " " << _position.x() << " " << _position.y() << " " << _position.z() << " " << _time << " " << _mom.x() << " " << _mom.y() << " " << _mom.z() << " " << _energy << " " << _edep << " " << m_particle_index << " " << _charge << " " << _mass;

  return( ss.str() );
}

void		KLHitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  double x,y,z,px,py,pz;
  ist >> myindex >> _trackID >> _pid >> _copyNumber >> _cellNumber >> 
    x >> y >> z >> _time >> 
    px >> py >> pz >> _energy >> _edep
      >> m_particle_index >> _charge >> _mass;
  
  _position.setX( x );
  _position.setY( y );
  _position.setZ( z );

  _mom.setX( px );
  _mom.setY( py );
  _mom.setZ( pz );

  setTxtIoIndex( myindex );
}

KLHit*	KLHitTextFileIo::makeKLHit()
{
  m_hit = new KLHit();
  
  m_hit->SetTrackID(_trackID );
  m_hit->SetPID(_pid);
  m_hit->SetCopyNumber(_copyNumber);
  m_hit->SetCellNumber(_cellNumber);
  m_hit->SetPosition(_position) ;
  m_hit->SetTime(_time);
  m_hit->SetMomentum(_mom);
  m_hit->SetEnergy(_energy);
  m_hit->SetEdep(_edep);
  m_hit->setCharge( _charge );
  m_hit->setMass( _mass );

  return m_hit;
}

void KLHitTextFileIo::setMCParticles(std::vector<MCParticleTextFileIo*>& particles)

{
  m_particles = &particles;
}

void		KLHitTextFileIo::completeRestoration()
{

  for(unsigned int j=0; j<(*m_particles).size(); j++){
    if((*m_particles)[j]->txtIoIndex() == m_particle_index){
      m_particle = (*m_particles)[j]->theParticle();
    }
  }

  m_hit->setParticle(m_particle);
}

