#ifndef MICEStackingAction_H
#define MICEStackingAction_H 1

#include "G4UserStackingAction.hh"

class G4Track;

class MICEStackingAction : public G4UserStackingAction
{
  public:
    MICEStackingAction();
    virtual ~MICEStackingAction();

  public:
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();

  private:
    

      
  public:
};

#endif

