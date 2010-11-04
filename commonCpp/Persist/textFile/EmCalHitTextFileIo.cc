// EmCalHitTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "EmCalHitTextFileIo.hh"
#include "EmCalDigitTextFileIo.hh"
#include "MCParticleTextFileIo.hh"

EmCalHitTextFileIo::EmCalHitTextFileIo( EmCalHit* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::EmCalHitTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;
}

EmCalHitTextFileIo::EmCalHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::EmCalHitTextFileIo );
  
  restoreObject( def );
}

std::string	EmCalHitTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!
    _trackID = m_hit->GetTrackID();
    _pid = m_hit->GetPID();
    _copyNumber = m_hit->GetCopyNumber();
    _cellNumber = m_hit->GetCellNumber();
    _layerNumber = m_hit->GetLayerNumber();
    _position = m_hit->GetPosition() ;
    _time = m_hit->GetTime();
    _mom = m_hit->GetMomentum();
    _charge = m_hit->charge();
    _mass = m_hit->mass();
    _energy = m_hit->GetEnergy();
    _edep = m_hit->GetEdep();
    _digitVector = m_hit->GetDigits();

    _digitIndices.resize( _digitVector.size() );

    for( unsigned int i = 0; i < _digitVector.size(); ++i )
    {
      int din = -1;
      for( unsigned int j = 0; din < 0 && j < _digitIos->size(); ++j )
        if( (*_digitIos)[j]->theDigit() == _digitVector[i] )
            din = (*_digitIos)[j]->txtIoIndex();
      _digitIndices[i] = din;
    }

  m_particle = m_hit->particle();

  // fill the line with the information about the class here, don't forget the index!
  for(unsigned int i=0; i< m_particles->size(); i++)
    {
      if((*m_particles)[i]->theParticle()==m_particle){ 
	m_particle_index = (*m_particles)[i]->txtIoIndex();
      }
    }

  std::stringstream ss;

  ss << txtIoIndex() << " " << _trackID << " " << _pid << " " << _copyNumber << " " << _cellNumber << " " << _layerNumber << " " << _position.x() << " " << _position.y() << " " << _position.z() << " " << _time << " " << _mom.x() << " " << _mom.y() << " " << _mom.z() << " " << _energy << " " << _edep << " " << vectorToString(_digitIndices).c_str() << " " << m_particle_index << " " << _charge << " " << _mass;

  return( ss.str() );
}

void		EmCalHitTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  double x,y,z,px,py,pz;
  std::string string;
  ist >> myindex >> _trackID >> _pid >> _copyNumber >> _cellNumber >> _layerNumber >> 
    x >> y >> z >> _time >> 
    px >> py >> pz >> _energy >> _edep
      >> string >> m_particle_index >> _charge >> _mass;
  
  _digitIndices = stringToVectorInteger(string);
  _position.setX( x );
  _position.setY( y );
  _position.setZ( z );

  _mom.setX( px );
  _mom.setY( py );
  _mom.setZ( pz );

  setTxtIoIndex( myindex );
}

EmCalHit*	EmCalHitTextFileIo::makeEmCalHit()
{
  m_hit = new EmCalHit();
  
  m_hit->SetTrackID(_trackID );
  m_hit->SetPID(_pid);
  m_hit->SetCopyNumber(_copyNumber);
  m_hit->SetCellNumber(_cellNumber);
  m_hit->SetLayerNumber(_layerNumber);
  m_hit->SetPosition(_position) ;
  m_hit->SetTime(_time);
  m_hit->SetMomentum(_mom);
  m_hit->SetEnergy(_energy);
  m_hit->SetEdep(_edep);
  m_hit->setCharge( _charge );
  m_hit->setMass( _mass );

  return m_hit;
}

void EmCalHitTextFileIo::setMCParticles(std::vector<MCParticleTextFileIo*>& particles)

{
  m_particles = &particles;
}

void		EmCalHitTextFileIo::completeRestoration()
{
  std::vector<EmCalDigit*> digits;
  digits.resize( _digitIndices.size() );

  for( unsigned int j = 0; j < _digitIndices.size(); ++j )
  {
    EmCalDigit* digit = NULL;
    for( unsigned int i = 0; i < _digitIos->size(); ++i )
      if( (*_digitIos)[i]->txtIoIndex() == _digitIndices[j] )
        digit = (*_digitIos)[i]->theDigit();
    digits[j] = digit;
  }

  m_hit->SetDigits( digits );

  for(unsigned int j=0; j<(*m_particles).size(); j++){
    if((*m_particles)[j]->txtIoIndex() == m_particle_index){
      m_particle = (*m_particles)[j]->theParticle();
    }
  }

  m_hit->setParticle(m_particle);
}

void    EmCalHitTextFileIo::setEmCalDigits( std::vector<EmCalDigitTextFileIo*>* digits )
{
  _digitIos = digits;
}
