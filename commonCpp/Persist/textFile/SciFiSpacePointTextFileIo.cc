// SciFiTrackTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "SciFiSpacePointTextFileIo.hh"
#include "SciFiDoubletClusterTextFileIo.hh"

SciFiSpacePointTextFileIo::SciFiSpacePointTextFileIo( SciFiSpacePoint* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiSpacePointTextFileIo );
	
  setTxtIoIndex( index );
  m_point = hit;
}

SciFiSpacePointTextFileIo::SciFiSpacePointTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::SciFiSpacePointTextFileIo );
  
  restoreObject( def );
}

std::string	SciFiSpacePointTextFileIo::storeObject()
{
  m_numClusters = m_point->getNumClusters();

  m_firstCluster = m_point->getDoubletCluster(0);
  m_secondCluster = m_point->getDoubletCluster(1);
  if(m_numClusters==3)
    m_thirdCluster = m_point->getDoubletCluster(2);
  else
    m_thirdCluster = NULL;

  // fill the line with the information about the class here, don't forget the index!

  m_first_index = m_second_index = m_third_index = -1;

  for( unsigned int i = 0; i < m_doubletClusters->size(); ++i )
  {
    if( (*m_doubletClusters)[i]->theCluster() == m_firstCluster )
    {
      m_first_index = (*m_doubletClusters)[i]->txtIoIndex();
    }
    else if( (*m_doubletClusters)[i]->theCluster() == m_secondCluster )
    {
      m_second_index = (*m_doubletClusters)[i]->txtIoIndex();
    }
    else if( (m_numClusters == 3) && (*m_doubletClusters)[i]->theCluster() == m_thirdCluster )
    {
      m_third_index = (*m_doubletClusters)[i]->txtIoIndex();
    }
  }  

  std::string fullLine;

  if( m_numClusters == 3 )
  {
    std::stringstream ss;
    
    ss << txtIoIndex() << " " << m_numClusters << " " << m_first_index << " " << m_second_index << " " << m_third_index;

    fullLine = ss.str();
  }
  else
  {
    std::stringstream ss;

    ss << txtIoIndex() << " " << m_numClusters << " " << m_first_index << " " << m_second_index;

    fullLine = ss.str();
  }

  return( fullLine );
}

void		SciFiSpacePointTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> m_numClusters >> m_first_index >> m_second_index; //  >> and many more variable here probably...

  if( m_numClusters == 3 )
    ist >> m_third_index;
  else
    m_third_index = -1;

  setTxtIoIndex( myindex );
}

SciFiSpacePoint*	SciFiSpacePointTextFileIo::makeSciFiSpacePoint()
{
  m_point = new SciFiSpacePoint();

  return m_point;
}

void		SciFiSpacePointTextFileIo::completeRestoration()
{
  m_firstCluster = NULL;
  m_secondCluster = NULL;
  m_thirdCluster = NULL;

  for(unsigned int i = 0; i< m_doubletClusters->size(); i++) 
  {
    if( (*m_doubletClusters)[i]->txtIoIndex() == m_first_index ) 
    {
      m_firstCluster = (*m_doubletClusters)[i]->theCluster();
    } 
    else if( (*m_doubletClusters)[i]->txtIoIndex() == m_second_index ) 
    {
      m_secondCluster = (*m_doubletClusters)[i]->theCluster();
    } 
    else if( m_numClusters == 3 && (*m_doubletClusters)[i]->txtIoIndex() == m_third_index )
    {
      m_thirdCluster = (*m_doubletClusters)[i]->theCluster();
    }
  }

  if( m_numClusters == 3 ) 
  {
    m_point->createCluster( m_firstCluster, m_secondCluster, m_thirdCluster );
  }
  else
  {
    m_point->createCluster( m_firstCluster, m_secondCluster );
  }
}

