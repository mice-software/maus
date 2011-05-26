// MousePid.hh - interface describing a generic matching of PID detector information with a track

#ifndef REC_MOUSEPID_HH
#define REC_MOUSEPID_HH

#include "Recon/Global/Extrapolation.hh"

class MousePid
{
  public : 

  virtual ~MousePid() {};

  virtual CLHEP::Hep3Vector	position() const = 0;
  virtual double		time() const = 0;
  virtual const Extrapolation&	extrapolation() const = 0; 
};

#endif

