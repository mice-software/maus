// MCVertex.hh

#ifndef INTERFACE_MCVERTEX_HH
#define INTERFACE_MCVERTEX_HH 1

#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;
#include "Memory.hh"

class MCParticle;

class MCVertex
{
  public :

  MCVertex();

  MCVertex(Hep3Vector position, double time);

  ~MCVertex() 		{ miceMemory.addDelete( Memory::MCVertex ); };

  void AddParticle(MCParticle* inParticle);

  const std::vector<MCParticle*>& incomingParticles() const			{ return m_in_parts; };
  const std::vector<MCParticle*>& outgoingParticles() const			{ return m_out_parts; };
  void setIncomingParticles( const std::vector<MCParticle*>& in)		{ m_in_parts=in; };
  void setOutgoingParticles( const std::vector<MCParticle*>& out)		{ m_out_parts=out; };
  Hep3Vector position() const							{ return m_position; };
  double time()const								{ return m_time; };

  private :

  std::vector<MCParticle*> m_in_parts;
  std::vector<MCParticle*> m_out_parts;
  Hep3Vector m_position;
  double m_time;
};

#endif

