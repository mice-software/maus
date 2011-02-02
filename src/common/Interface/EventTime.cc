// EventTime.cc - class to hold the time of an event as retrieved from the LDC Event information
//
// M. Ellis - 20th May 2009

#include "EventTime.hh"
#include "CLHEP/Units/SystemOfUnits.h"

double	EventTime::time() const
{
  double t = (double)_sec * CLHEP::second + (double)_usec * CLHEP::microsecond;
  return t;
}

