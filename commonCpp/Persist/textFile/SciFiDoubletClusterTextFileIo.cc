// SciFiDoubletClusterTextFileIo.cc
//
// M.Ellis 22/8/2005

#include <sstream>

#include "SciFiDoubletClusterTextFileIo.hh"
#include "SciFiDigitTextFileIo.hh"
#include "Recon/SciFi/SciFiDoubletCluster.hh"

SciFiDoubletClusterTextFileIo::SciFiDoubletClusterTextFileIo( SciFiDoubletCluster* cluster, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiDoubletClusterTextFileIo );
	
  setTxtIoIndex( index );
  m_cluster = cluster;
}

SciFiDoubletClusterTextFileIo::SciFiDoubletClusterTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiDoubletClusterTextFileIo );
  
  restoreObject( def );
}

std::string	SciFiDoubletClusterTextFileIo::storeObject()
{
  seed = m_cluster->seedDigi();
  neighbour = NULL;			//ME this is not used anymore
  digits = m_cluster->allDigits();
  aveChanNoWeighted = m_cluster->getChanNoU();
  aveChanNoUnweighted = m_cluster->getChanNoW();
  lowerChanNo = m_cluster->getLowerChanNo();
  upperChanNo = m_cluster->getUpperChanNo();
  doubletNo = m_cluster->getDoubletNo();
  stationNo = m_cluster->getStationNo();
  trackerNo = m_cluster->getTrackerNo();
  nMu = m_cluster->getNMuChans();
  time = m_cluster->getTime();
  nPE = m_cluster->getNPE();
  truePos = m_cluster->getTruePosition();
  trueMom = m_cluster->getTrueMomentum();
  m_used = m_cluster->used();
  pos = m_cluster->position();
  dir = m_cluster->direction();

  seed_index = -1;

  for( unsigned int i = 0; i < m_digits->size(); ++i )
    if( (*m_digits)[i]->theDigit() == seed )
      seed_index = (*m_digits)[i]->txtIoIndex();

  neighbour_index = -1;

  for( unsigned int i = 0; i < m_digits->size(); ++i )
    if( (*m_digits)[i]->theDigit() == neighbour )
      neighbour_index = (*m_digits)[i]->txtIoIndex();

  digits_indices.resize( digits.size() );

  for( unsigned int i = 0; i < digits.size(); ++i )
  {
    int index = -1;

    for( unsigned int j = 0; j < m_digits->size(); ++j )
      if( (*m_digits)[j]->theDigit() == digits[i] )
        index = (*m_digits)[j]->txtIoIndex();

    digits_indices[i] = index;
  }

  std::stringstream ss;

  ss << txtIoIndex() << " " << aveChanNoWeighted << " " << aveChanNoUnweighted << " " << lowerChanNo << " " << upperChanNo << " " << doubletNo << " " << stationNo << " " << trackerNo << " " << time << " " << nPE << " " << truePos.x() << " " << truePos.y() << " " << truePos.z() << " " << trueMom.x() << " " << trueMom.y() << " " << trueMom.z() << " " << m_used << " " << seed_index << " " << neighbour_index << " " << pos.x() << " " << pos.y() << " " << pos.z() << " " << dir.x() << " " << dir.y() << " " << dir.z() << " " << digits_indices.size();

  std::string fullLine = ss.str();

  for( unsigned int i = 0; i < digits_indices.size(); ++i )
  {
    std::stringstream ss1;
    ss1 << digits_indices[i] << " ";
    fullLine += ss1.str();
  }

  return( fullLine );
}

void		SciFiDoubletClusterTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  double x, y, z, px, py, pz;
  double fx, fy, fz, dx, dy, dz;
  int num_digits;

  ist >> myindex >> aveChanNoWeighted >> aveChanNoUnweighted >> lowerChanNo >> upperChanNo >> doubletNo
      >> stationNo >> trackerNo >> time >> nPE >> x >> y >> z >> px >> py >> pz >> m_used 
      >> seed_index >> neighbour_index >> fx >> fy >> fz >> dx >> dy >> dz >> num_digits;

  truePos.setX( x );
  truePos.setY( y );
  truePos.setZ( z );
  
  trueMom.setX( px );
  trueMom.setY( py );
  trueMom.setZ( pz );

  pos.setX( fx );
  pos.setY( fy );
  pos.setZ( fz );

  dir.setX( dx );
  dir.setY( dy );
  dir.setZ( dz );

  digits_indices.resize( num_digits );

  for( int i = 0; i < num_digits; ++i )
    ist >> digits_indices[i];

  setTxtIoIndex( myindex );
}

SciFiDoubletCluster*	SciFiDoubletClusterTextFileIo::makeSciFiDoubletCluster()
{
  m_cluster = new SciFiDoubletCluster();

  return m_cluster;
}

void	SciFiDoubletClusterTextFileIo::setSciFiDigits( std::vector<SciFiDigitTextFileIo*>& digits )
{
  m_digits = & digits;
}

void		SciFiDoubletClusterTextFileIo::completeRestoration()
{
  seed = NULL;
  neighbour = NULL;

  digits.resize( digits_indices.size() );

  for( unsigned int i = 0; i < digits_indices.size(); ++i )
    digits[i] = NULL;

  for( unsigned int i = 0; i < m_digits->size(); ++i )
    if( (*m_digits)[i]->txtIoIndex() == seed_index )
      seed = (*m_digits)[i]->theDigit();

  for( unsigned int i = 0; i < m_digits->size(); ++i )
    if( (*m_digits)[i]->txtIoIndex() == neighbour_index )
      neighbour = (*m_digits)[i]->theDigit();

  for( unsigned int i = 0; i < digits.size(); ++i )
    for( unsigned int j = 0; j < m_digits->size(); ++j )
      if( (*m_digits)[j]->txtIoIndex() == digits_indices[i] )
        digits[i] = (*m_digits)[j]->theDigit();

  SciFiDoubletCluster* temp = new SciFiDoubletCluster( seed );

  for( unsigned int i = 1; i < digits.size(); ++i )
    temp->addDigit( digits[i] );

  m_cluster->copy( *temp );

  m_cluster->setPosition( pos );
  m_cluster->setDirection( dir );
  m_cluster->setNPE( nPE );

  delete temp;
}
