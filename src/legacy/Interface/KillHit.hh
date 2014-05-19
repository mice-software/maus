// MAUS WARNING: THIS IS LEGACY CODE.
// KillHit.hh

#ifndef INTERFACE_KILLHIT_HH
#define INTERFACE_KILLHIT_HH 1

#include "Interface/MCHit.hh"
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;

class KillHit : public MCHit
{
  public :

    KillHit() : MCHit(), _reason("") {;}
    KillHit( int trackID, int pdgID, double q, double m, Hep3Vector pos, Hep3Vector mom, double t, double energy,  Hep3Vector spin, std::string reasonForDying) 
           : MCHit(trackID, pdgID, q, m, pos, mom, t, 0, energy, spin), _reason(reasonForDying)
	{;}
    ~KillHit() {};

    std::string reasonForDying   ()                   {return _reason;}
    void        setReasonForDying(std::string reason) {_reason = reason;}

  private :
    std::string _reason;
};

#endif

