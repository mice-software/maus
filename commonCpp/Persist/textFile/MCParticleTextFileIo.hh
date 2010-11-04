// MCParticle.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_MCParticleTEXTFIILEIO_H
#define PERSIST_MCParticleTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/MCParticle.hh"

#include "CLHEP/Vector/ThreeVector.h"

#include "Interface/Memory.hh"

class MCVertexTextFileIo;

class MCParticleTextFileIo : public TextFileIoBase
{
  public :

    MCParticleTextFileIo( MCParticle*, int );
    MCParticleTextFileIo( std::string );

    virtual ~MCParticleTextFileIo() { miceMemory.addDelete( Memory::MCParticleTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    MCParticle*		theParticle() const { return m_particle; };

    MCParticle*		makeMCParticle();

    void		completeRestoration();
  
    void                setMCVertices(std::vector<MCVertexTextFileIo*>& );

  private :

    MCParticle*		m_particle;

    int m_trackID;
    double m_charge;
    int m_pdg;
    double m_mass;
    double m_ini_kinE;
    double m_fin_kinE;
    Hep3Vector m_ini_pos;
    Hep3Vector m_fin_pos;
    Hep3Vector m_ini_mom;
    Hep3Vector m_fin_mom;
    double m_ini_time;
    double m_fin_time;
    bool m_ref_part;

    MCVertex* m_ini_vertex;
    MCVertex* m_fin_vertex;

   int ini_index;
   int fin_index;

   std::vector<MCVertexTextFileIo*>* m_vertices;

   std::vector<Hep3Vector> m_points;

};

#endif

