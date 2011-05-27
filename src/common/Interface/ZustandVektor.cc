// ZustandVektor.cc

#include "Interface/ZustandVektor.hh"
#include "Interface/MCHit.hh"

ZustandVektor::ZustandVektor()
{
  _xLorentz.set(0.,0.,0.,0.);
  _pLorentz.set(0.,0.,0.,0.);
  _matrix = HepSymMatrix(8,0); // 8x8 initialized to zero
  _truth = false;
  _pid = 0;
}

ZustandVektor::ZustandVektor( MCHit* aHit)
{
  SetPosition(aHit->position());
  SetTime(aHit->time());
  SetMomentum(aHit->momentum());
  SetEnergy(aHit->energy());
  _matrix = HepSymMatrix(8,0); // 8x8 initialized to zero
  _truth = true; // by default this is mc truth from hit
  _pid = aHit->pdg();
}


