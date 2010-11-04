// SpecialHit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_SpecialHitTEXTFIILEIO_H
#define PERSIST_SpecialHitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/SpecialHit.hh"

#include "Interface/Memory.hh" 

class SpecialHitTextFileIo : public TextFileIoBase
{
  public :

    SpecialHitTextFileIo( SpecialHit*, int );
    SpecialHitTextFileIo( std::string );

    virtual ~SpecialHitTextFileIo() { miceMemory.addDelete( Memory::SpecialHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    SpecialHit*		theHit() const { return m_hit; };

    SpecialHit*		makeSpecialHit();

    void		completeRestoration();

  private :

    SpecialHit*		m_hit;

    double  _edep;
    int     _iax, _iaz, _irad;
    int     _stepType;
    double  _stepLength;
    double  _pathLength;
    double  _properTime;

    int _trackID;
    int _stationNumber;
    int _cellNumber;
    Hep3Vector _pos;
    Hep3Vector _momentum;
    std::string _volName;
    double _time;
    double _energy;
    int    _pid;
    double _mass;
    double _charge;
    Hep3Vector _bfield;
    Hep3Vector _efield;
    int    _parentrackID;
    int    _ancestrackID;
    Hep3Vector _vtxpos;
    double _vtxtime;
    Hep3Vector _vtxmom;
};

#endif

