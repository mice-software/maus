// EmCalHit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_EmCalHitTEXTFIILEIO_H
#define PERSIST_EmCalHitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/EmCalHit.hh"
#include <vector>
#include "Interface/Memory.hh" 

class EmCalDigit;
class EmCalDigitTextFileIo;
class MCParticleTextFileIo;

class EmCalHitTextFileIo : public TextFileIoBase
{
  public :

    EmCalHitTextFileIo( EmCalHit*, int );
    EmCalHitTextFileIo( std::string );

    virtual ~EmCalHitTextFileIo() { miceMemory.addDelete( Memory::EmCalHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    EmCalHit*		theHit() const { return m_hit; };

    EmCalHit*		makeEmCalHit();

    void		completeRestoration();

    void    setEmCalDigits( std::vector<EmCalDigitTextFileIo*>* );

    void    setMCParticles(std::vector<MCParticleTextFileIo*>& );


  private :

    EmCalHit*   m_hit;
    std::vector<EmCalDigitTextFileIo*>* _digitIos;
    std::vector<int>  _digitIndices;

    // ***** variables go here ******
    std::vector<EmCalDigit*> _digitVector;
    double      _edep;
    Hep3Vector  _position;
    int         _copyNumber;
    int         _cellNumber;
    int         _layerNumber;
    int         _trackID;
    int         _pid;
    double      _time;
    Hep3Vector  _mom;
    double      _energy;
  MCParticle*	m_particle;

  int m_particle_index;

  double _charge;
  double _mass;

  std::vector<MCParticleTextFileIo*>* m_particles;
};

#endif

