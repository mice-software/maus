// TofHitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "TofHitTextFileIo.hh"
#include "MCParticleTextFileIo.hh"

TofHitTextFileIo::TofHitTextFileIo( TofHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::TofHitTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

TofHitTextFileIo::TofHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::TofHitTextFileIo );
	
  restoreObject( def );
}

std::string	TofHitTextFileIo::storeObject()
{
  _trackID = m_hit->GetTrackID();
  _pid = m_hit->GetPID();
  _mass = m_hit->GetMass();
  _edep = m_hit->GetEdep();
  _pos = m_hit->GetPosition();
  _time = m_hit->GetTime();
  _charge = m_hit->charge();
  _momentum = m_hit->GetMomentum();
  _energy = m_hit->GetEnergy();
  _pathLength = m_hit->GetPathLength();
  _stationNumber = m_hit->GetStationNo();
  _planeNumber = m_hit->GetPlaneNo();
  _stripNumber = m_hit->GetStripNo();
  _volName = m_hit->GetVolumeName();

  m_particle = m_hit->particle();

  // fill the line with the information about the class here, don't forget the index!

  for(unsigned int i=0; i< m_particles->size(); i++)
    if( (*m_particles)[i]->theParticle() == m_particle ) 
      m_particle_index = (*m_particles)[i]->txtIoIndex();

  std::stringstream ss;

  ss << txtIoIndex() << " " << _trackID << " " << _pid << " " << _mass << " " << _edep << " "
                  << _pos << " " << _time << " " << _charge << " " << _momentum << " " 
                  << _energy << " " << _pathLength << " " << _stationNumber << " " 
                  << _planeNumber << " " << _stripNumber << " " << _volName << " " 
                  << m_particle_index;

  return( ss.str() );
}

void		TofHitTextFileIo::restoreObject( std::string def )
{
  //std::cout << "THTFIO restored" << std::endl;
  int myindex;

  std::istringstream ist( def );

  ist >> myindex >> _trackID >> _pid >> _mass >> _edep >> _pos
      >> _time >> _charge >> _momentum >> _energy >> _pathLength >> _stationNumber 
      >> _planeNumber >> _stripNumber >> _volName >> m_particle_index; 

  setTxtIoIndex( myindex );
}

TofHit*	TofHitTextFileIo::makeTofHit()
{
  //std::cout << "TofHit made from THTFIO" << std::endl;
  m_hit = new TofHit();
   // possibly you have to use some "set" methods here...
  m_hit->SetTrackID( _trackID );
  m_hit->SetPID( _pid );
  m_hit->SetMass( _mass );
  m_hit->SetEdep( _edep );
  m_hit->SetPosition( _pos );
  m_hit->SetTime( _time );
  m_hit->SetMomentum( _momentum );
  m_hit->setCharge( _charge );
  m_hit->SetEnergy( _energy );
  m_hit->SetPathLength( _pathLength );
  m_hit->SetStationNo( _stationNumber );
  m_hit->SetPlaneNo( _planeNumber );
  m_hit->SetStripNo( _stripNumber );
  m_hit->SetVolumeName( _volName );
  return m_hit;
}

void TofHitTextFileIo::setMCParticles(std::vector<MCParticleTextFileIo*>& particles)

{
  m_particles = &particles;
}

void		TofHitTextFileIo::completeRestoration()
{
  for( unsigned int j = 0; j < (*m_particles).size(); ++j )
    if( (*m_particles)[j]->txtIoIndex() == m_particle_index )
      m_particle = (*m_particles)[j]->theParticle();

  m_hit->setParticle( m_particle );
}
