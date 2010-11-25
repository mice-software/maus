#include "MAUSSD.hh"

MAUSSD::MAUSSD( MiceModule* mod, bool dEdxCut )
 : G4VSensitiveDetector( mod->fullName()) 
{
  _dEdxCut = dEdxCut;
  _module  = mod;
  _isHit = false;
}



