// MCParticleTextFileIo.cc
//
// M.Ellis 27/8/2005

#include <sstream>

#include "MCParticleTextFileIo.hh"
#include "MCVertexTextFileIo.hh"

MCParticleTextFileIo::MCParticleTextFileIo( MCParticle* hit, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::MCParticleTextFileIo );
	
  setTxtIoIndex( index );
  m_particle = hit;
}

MCParticleTextFileIo::MCParticleTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::MCParticleTextFileIo );
  
  restoreObject( def );
}

std::string	MCParticleTextFileIo::storeObject()
{
    m_trackID = m_particle->trackID();
    m_charge = m_particle->charge();
    m_pdg = m_particle->pdg();
    m_mass = m_particle->mass();
    m_ini_kinE = m_particle->initialKinE();
    m_fin_kinE = m_particle->finalKinE();
    m_ini_pos = m_particle->initialPosition();
    m_fin_pos = m_particle->finalPosition();
    m_ini_mom = m_particle->initialMomentum();
    m_fin_mom = m_particle->finalMomentum();
    m_ini_time = m_particle->initialTime();
    m_fin_time = m_particle->finalTime();
    m_ini_vertex = m_particle->initialVertex();
    m_fin_vertex = m_particle->finalVertex();
    m_ref_part   = m_particle->isRefPart();

  // fill the line with the information about the class here, don't forget the index!
      for(unsigned int i=0; i< m_vertices->size(); i++)
	{
	  if((*m_vertices)[i]->theVertex()==m_ini_vertex){ 
	    ini_index = (*m_vertices)[i]->txtIoIndex();
	  }
	  if((*m_vertices)[i]->theVertex()==m_fin_vertex){ 
	    fin_index = (*m_vertices)[i]->txtIoIndex();
	  }	  
	}

    ini_index = fin_index = -1;	//ME this needs to be filled!

    for( unsigned int i = 0; i < m_particle->numPoints(); ++i )
      m_points.push_back( m_particle->point(i) );

    std::stringstream lines;

    lines << txtIoIndex() << " ";
    lines <<  m_trackID << " ";
    lines <<  m_charge << " ";
    lines << m_pdg << " ";
    lines <<  m_mass << " ";
    lines <<  m_ini_kinE << " ";
    lines <<  m_fin_kinE << " ";
    lines <<  m_ini_pos.x() << " ";
    lines <<  m_ini_pos.y() << " ";
    lines <<  m_ini_pos.z()  << " ";
    lines <<  m_fin_pos.x() << " ";
    lines <<  m_fin_pos.y() << " ";
    lines <<  m_fin_pos.z() << " ";
    lines <<  m_ini_mom.x() << " ";
    lines <<  m_ini_mom.y() << " ";
    lines <<  m_ini_mom.z() << " ";
    lines <<  m_fin_mom.x() << " ";
    lines <<  m_fin_mom.y() << " ";
    lines <<  m_fin_mom.z() << " ";
    lines <<  m_ini_time << " ";
    lines <<  m_fin_time << " ";
    lines << m_ref_part << " ";
    lines <<  ini_index << " ";
    lines <<  fin_index << " ";
    lines <<  m_points.size();

  for( unsigned int i = 0; i < m_points.size(); ++i )
    lines << " " <<  m_points[i];
 
  return( lines.str() );
}

void		MCParticleTextFileIo::restoreObject( std::string def )
{
  int myindex;
  double m_ini_pos_x, m_ini_pos_y, m_ini_pos_z; 
  double m_fin_pos_x, m_fin_pos_y, m_fin_pos_z;
  double m_ini_mom_x, m_ini_mom_y, m_ini_mom_z; 
  double m_fin_mom_x, m_fin_mom_y, m_fin_mom_z;
  int num_pts = 0;

  std::istringstream ist( def.c_str() );

  ist >> myindex //  >> and many more variable here probably...
      >> m_trackID >>  m_charge >>  m_pdg >> m_mass 
      >>  m_ini_kinE >> m_fin_kinE
      >> m_ini_pos_x >>  m_ini_pos_y >> m_ini_pos_z 
      >> m_fin_pos_x >>  m_fin_pos_y >> m_fin_pos_z
      >> m_ini_mom_x >>  m_ini_mom_y >> m_ini_mom_z 
      >> m_fin_mom_x >>  m_fin_mom_y >> m_fin_mom_z
      >> m_ini_time  >>  m_fin_time  >> m_ref_part 
      >> ini_index   >> fin_index 
      >> num_pts;

  m_points = std::vector<Hep3Vector>(num_pts);

  for( unsigned int i = 0; i < m_points.size(); ++i )
  {
    Hep3Vector tmp;
    ist >> tmp;
    m_points[i] = tmp;
  }

  m_ini_pos.set(m_ini_pos_x, m_ini_pos_y, m_ini_pos_z);
  m_fin_pos.set(m_fin_pos_x, m_fin_pos_y, m_fin_pos_z);
  m_ini_mom.set(m_ini_mom_x, m_ini_mom_y, m_ini_mom_z);
  m_fin_mom.set(m_fin_mom_x, m_fin_mom_y, m_fin_mom_z);

  setTxtIoIndex( myindex );
}

MCParticle*	MCParticleTextFileIo::makeMCParticle()
{
  m_particle = new MCParticle(m_trackID, m_charge, m_pdg, m_mass, m_ini_kinE, m_fin_kinE, m_ini_pos, m_fin_pos, m_ini_mom, m_fin_mom, m_ini_time, m_fin_time, m_ref_part );

  for( unsigned int i = 0; i < m_points.size(); ++i )
    m_particle->addPoint( m_points[i] );

  return m_particle;
}


void MCParticleTextFileIo::setMCVertices(std::vector<MCVertexTextFileIo*>& vertices)

{
  m_vertices = &vertices;
}

void		MCParticleTextFileIo::completeRestoration()
{

  m_ini_vertex = NULL;
  m_fin_vertex = NULL;

  for(unsigned int j=0; j<(*m_vertices).size(); j++){
    if((*m_vertices)[j]->txtIoIndex() == ini_index){
      m_ini_vertex = (*m_vertices)[j]->theVertex();
    }
    
   if((*m_vertices)[j]->txtIoIndex() == fin_index){
      m_fin_vertex = (*m_vertices)[j]->theVertex();
    }
  }

  m_particle->SetVertices(m_ini_vertex, m_fin_vertex);
}
