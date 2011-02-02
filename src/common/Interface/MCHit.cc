// MCHit.cc

#include "MCHit.hh"

MCHit::MCHit()
{
  _trackID = -1;
}


MCHit::MCHit( int id, int pdg, double q, double m, Hep3Vector pos, Hep3Vector mom, double t, double de, double e )
{
  _trackID = id;
  _pdg = pdg;
  _charge = q;
  _mass = m;
  _position = pos;
  _momentum = mom;
  _time = t;
  _edep = de;
  _energy = e;
}

