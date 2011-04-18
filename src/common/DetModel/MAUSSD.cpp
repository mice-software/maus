#include "MAUSSD.h"

namespace MAUS {

MAUSSD::MAUSSD(MiceModule* mod)
    : G4VSensitiveDetector(mod->fullName()) {
  _module  = mod;
  _isHit = false;
}

}  // ends MAUS namespace



