// MCVertex.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_MCVertexTEXTFIILEIO_H
#define PERSIST_MCVertexTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/MCVertex.hh"

#include "CLHEP/Vector/ThreeVector.h"

#include "Interface/Memory.hh" 

class MCParticleTextFileIo;

class MCVertexTextFileIo : public TextFileIoBase
{
  public :

    MCVertexTextFileIo( MCVertex*, int );
    MCVertexTextFileIo( std::string );

    virtual ~MCVertexTextFileIo() { miceMemory.addDelete( Memory::MCVertexTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    MCVertex*		theVertex() const { return m_vertex; };

    MCVertex*		makeMCVertex();

    void		completeRestoration();

    void                setMCParticles(std::vector<MCParticleTextFileIo*>&);

  private :

    MCVertex*		m_vertex;
    std::vector<MCParticle*> m_in_parts;
    std::vector<MCParticle*> m_out_parts;
    Hep3Vector m_position;
    double m_time;
  
    std::vector<int> incoming_indices;
    std::vector<int> outgoing_indices;

    std::vector<MCParticleTextFileIo*>* m_particles;
};

#endif

