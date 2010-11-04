// TofHit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_TofHitTEXTFILEIO_H
#define PERSIST_TofHitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/TofHit.hh"
#include "Interface/Memory.hh"

class MCParticleTextFileIo;

class TofHitTextFileIo : public TextFileIoBase
{
  public :

    TofHitTextFileIo( TofHit*, int );
    TofHitTextFileIo( std::string );

    virtual ~TofHitTextFileIo() { miceMemory.addDelete( Memory::TofHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    TofHit*		theHit() const { return m_hit; };

    TofHit*		makeTofHit();

    void                setMCParticles(std::vector<MCParticleTextFileIo*>& );


    void		completeRestoration();

  private :

    TofHit*		m_hit;

  int                 _trackID;
  int                 _pid;
  double              _mass;
  double              _edep;
  Hep3Vector          _pos;
  double              _time;
  double              _charge;
  Hep3Vector          _momentum;
  double              _energy;
  double              _pathLength;
  int                 _stationNumber;
  int                 _planeNumber;
  int                 _stripNumber;
  std::string         _volName;
  MCParticle*	m_particle;

  int m_particle_index;

  std::vector<MCParticleTextFileIo*>* m_particles;
};

#endif


