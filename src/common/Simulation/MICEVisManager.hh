#ifndef MICEVISMANAGER_HH
#define MICEVISMANAGER_HH

#ifdef G4VIS_USE

#include "G4VisManager.hh"

class MICEVisManager: public G4VisManager {

public:

  MICEVisManager ();

private:

  void RegisterGraphicsSystems ();

};

#endif

#endif
