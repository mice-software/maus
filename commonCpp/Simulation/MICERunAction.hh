#ifndef MICERunAction_H
#define MICERunAction_H 1

#include "globals.hh"
#include "G4UserRunAction.hh"

class MICERunAction : public G4UserRunAction
{
  public:
    MICERunAction();
    virtual ~MICERunAction();

  public:
    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

};

#endif

