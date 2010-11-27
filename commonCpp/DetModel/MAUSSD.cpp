#include "MAUSSD.h"

namespace MAUS {

MAUSSD::MAUSSD(MiceModule* mod, bool dEdxCut)
    : G4VSensitiveDetector(mod->fullName()) {
  _dEdxCut = dEdxCut;
  _module  = mod;
  _isHit = false;
}

}  // ends MAUS namespace



