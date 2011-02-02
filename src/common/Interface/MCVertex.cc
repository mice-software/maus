// MCVertex.cc

#include "MCVertex.hh"
#include "MCParticle.hh"

MCVertex::MCVertex()
{
  miceMemory.addNew( Memory::MCVertex );
  
  std::cout << "MCVertex Constructor 1" << std::endl;
}

MCVertex::MCVertex(Hep3Vector position, double time)
{
  miceMemory.addNew( Memory::MCVertex );
  
  m_position = position;
  m_time = time;
  //   std::cout << "vertex created " << position.x() << " " << position.y() << " " << position.z() << " " << time << std::endl;
   m_out_parts.clear();
   m_in_parts.clear();

//  std::cout << "vertex created " << position.x() << " " << position.y() << " " << position.z() << " " << time << std::endl;
}

void MCVertex::AddParticle(MCParticle* particle)
{
  // decide whether particle is incoming or outgoing
  // and add to appropriate vector
  
  //std::cout << "adding particle " << std::endl; 
  if( particle->initialPosition() == m_position )
  {
    //m_out_parts.push_back(particle);
    m_out_parts.resize(m_out_parts.size()+1);
    m_out_parts[m_out_parts.size()-1]= particle;
    //std::cout << "outgoing " << std::endl;
  }
  else if( particle->finalPosition() == m_position )
    {
      m_in_parts.resize(m_in_parts.size()+1);
      m_in_parts[m_in_parts.size()-1]= particle;
      //m_in_parts.push_back(particle);
      //std::cout << "incoming " << std::endl;
    }
  else
    {
      std::cerr << "Oh dear! particle doesn't seem to be incoming or outgoing!!" << std::endl;
    }
}
