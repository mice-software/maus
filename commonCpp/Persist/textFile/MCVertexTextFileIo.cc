// MCVertexTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "MCVertexTextFileIo.hh"
#include "MCParticleTextFileIo.hh"


MCVertexTextFileIo::MCVertexTextFileIo( MCVertex* hit, int index ) : TextFileIoBase(), incoming_indices(0), outgoing_indices(0)
{
  miceMemory.addNew( Memory::MCVertexTextFileIo ); 
	
  setTxtIoIndex( index );
  m_vertex = hit;
}

MCVertexTextFileIo::MCVertexTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::MCVertexTextFileIo ); 
  
  restoreObject( def );
}

std::string	MCVertexTextFileIo::storeObject()
{
  m_position = m_vertex->position();
  m_time = m_vertex->time();

  m_in_parts = m_vertex->incomingParticles();
  m_out_parts = m_vertex->outgoingParticles();

  // fill the line with the information about the class here, don't forget the index!

  incoming_indices.resize( m_in_parts.size() );

  for(unsigned int j=0; j<m_in_parts.size(); j++)
  {
    int index = -1;
    for(unsigned int i=0; i< m_particles->size(); i++)
       if((*m_particles)[i]->theParticle()==m_in_parts[j])
        index = (*m_particles)[i]->txtIoIndex();
    incoming_indices[j] = index;
  }

  outgoing_indices.resize(m_out_parts.size());

  for(unsigned int j=0; j<m_out_parts.size(); j++)
  {
    int index = -1;
    for(unsigned int i=0; i< m_particles->size(); i++)
      if((*m_particles)[i]->theParticle()==m_out_parts[j])
	 index = (*m_particles)[i]->txtIoIndex();
    outgoing_indices[j] = index;
  }
  
  std::string fullLine;

  std::stringstream ss1;

  ss1 << txtIoIndex() << " " << m_position.x() << " " << m_position.y() << " " << m_position.z() << " " << m_time << " ";

  fullLine += ss1.str();

  std::stringstream ss2;
  ss2 << incoming_indices.size() << " ";
  
  fullLine += ss2.str();

  for(unsigned int i=0; i<incoming_indices.size(); i++)
  {
      std::stringstream ss3;
      ss3 << incoming_indices[i] << " ";
      fullLine += ss3.str();
  }

  std::stringstream ss4;
  ss4 << outgoing_indices.size() << " ";
  fullLine += ss4.str();

  for(unsigned int i=0; i<outgoing_indices.size(); i++)
  {
      std::stringstream ss5;
      ss5 << outgoing_indices[i] << " ";
      fullLine += ss5.str();
  }

  return( fullLine );
}

void		MCVertexTextFileIo::restoreObject( std::string def )
{
  int myindex;
  double m_position_x, m_position_y, m_position_z;

  std::istringstream ist( def.c_str() );

  ist >> myindex //  >> and many more variable here probably...
      >> m_position_x >> m_position_y >>  m_position_z >>  m_time;
  m_position.set(m_position_x, m_position_y, m_position_z);

  int in_size, out_size;

  ist >> in_size;
  
  incoming_indices.resize(in_size);

  for(int i=0; i<in_size; i++)
    ist >> incoming_indices[i];

  ist >> out_size;

  outgoing_indices.resize(out_size);

  for(int i=0; i<out_size; i++)
    ist >> outgoing_indices[i];

  setTxtIoIndex( myindex );
}

MCVertex*	MCVertexTextFileIo::makeMCVertex()
{
  m_vertex = new MCVertex(m_position, m_time);

  // possibly you have to use some "set" methods here...

  return m_vertex;
}

void           MCVertexTextFileIo::setMCParticles(std::vector<MCParticleTextFileIo*>& particles)
{
  m_particles = & particles;
}
void		MCVertexTextFileIo::completeRestoration()
{
  m_in_parts.resize(incoming_indices.size());
  m_out_parts.resize(outgoing_indices.size());

  for( unsigned int i = 0; i < incoming_indices.size(); ++i )
    m_in_parts[i] = NULL;

  for( unsigned int i = 0; i < outgoing_indices.size(); ++i )
    m_out_parts[i] = NULL;

/*
  for(unsigned int i=0; i<m_in_parts.size(); i++)
    for(unsigned int j=0; j<(*m_particles).size(); j++)
      if((*m_particles)[j]->txtIoIndex() == incoming_indices[i])
	  m_in_parts[i] = (*m_particles)[j]->theParticle();

  for(unsigned int i=0; i<m_out_parts.size(); i++)
    for(unsigned int j=0; j<(*m_particles).size(); j++)
      if((*m_particles)[j]->txtIoIndex() == outgoing_indices[i])
	  m_out_parts[i] = (*m_particles)[j]->theParticle();
*/

  m_vertex->setIncomingParticles(m_in_parts);
  m_vertex->setOutgoingParticles(m_out_parts);
}
