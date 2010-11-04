// TofSpacePointTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "TofSpacePointTextFileIo.hh"

TofSpacePointTextFileIo::TofSpacePointTextFileIo( TofSpacePoint* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::TofSpacePointTextFileIo );
	
  setTxtIoIndex( index );
  m_hit = hit;

}

TofSpacePointTextFileIo::TofSpacePointTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::TofSpacePointTextFileIo );
  
  restoreObject( def );
}

std::string	TofSpacePointTextFileIo::storeObject()
{
		//get the hits from the digit object
	m_slabHit_A = m_hit->hitA(); 
	m_slabHit_B = m_hit->hitB();

		//set the data that we are going to write (variables)
	_station = m_hit->station(); 
   _pos = m_hit->position();
   _time = m_hit->time();
	m_good = m_hit->isGood();

  if( m_slabHit_A )
    for(unsigned int i = 0; i<m_tofSlabHits->size(); i++)
      if((*m_tofSlabHits)[i]->theHit() == m_slabHit_A)
	      m_slabA_index = (*m_tofSlabHits)[i]->txtIoIndex();

  if( m_slabHit_B )
    for(unsigned int i = 0; i<m_tofSlabHits->size(); i++)
      if((*m_tofSlabHits)[i]->theHit() == m_slabHit_B)
	      m_slabB_index = (*m_tofSlabHits)[i]->txtIoIndex();

  std::stringstream ss;

		//now write it all to string output

  ss << txtIoIndex() << " " << _station << " " << _pos  << " " << _time << " " << m_good << " "
                       << m_slabA_index << " " << m_slabB_index ;

  return( ss.str() );
}

void		TofSpacePointTextFileIo::restoreObject( std::string def )
{
  int myindex;

  std::istringstream ist( def.c_str() );

  ist >> myindex >> _station >> _pos >> _time >> m_good
						>> m_slabA_index >> m_slabB_index ;

  setTxtIoIndex( myindex );
}

TofSpacePoint*	TofSpacePointTextFileIo::makeTofSpacePoint()
{
	m_hit = new TofSpacePoint();
/*
	std::vector<const MiceModule*> tofModules = theRun->miceModule->findModulesByPropertyString( "Detector", "TOF" );
	for(unsigned int i=0;i<tofModules.size(); ++i)
		if(tofModules[i]->propertyInt( "Station" ) == _station)
		m_hit->SetModule( tofModules[i] );
*/
	m_hit->SetTime( _time );
	m_hit->SetPosition( _pos );
	m_hit->SetGood( m_good );

	return m_hit;
}

void		TofSpacePointTextFileIo::completeRestoration()
{
	m_slabHit_A = NULL;
	m_slabHit_B = NULL;

	for( unsigned int j = 0; j < m_tofSlabHits->size(); ++j )
	if( (*m_tofSlabHits)[j]->txtIoIndex() == m_slabA_index)
		m_slabHit_A =  (*m_tofSlabHits)[j]->theHit();

	for( unsigned int j = 0; j < m_tofSlabHits->size(); ++j )
	if( (*m_tofSlabHits)[j]->txtIoIndex() == m_slabB_index)
		m_slabHit_B =  (*m_tofSlabHits)[j]->theHit();

	m_hit->SetHitA(m_slabHit_A);

	m_hit->SetHitB(m_slabHit_B);
}
