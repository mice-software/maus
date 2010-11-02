#ifndef MICEStackingAction_H
#define MICEStackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"
#include "G4ThreeVector.hh"

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
    int ih_NTu_PionPlus;
    int ih_NTu_MuonPlus;
    double momentumInit[3];
    bool newPion;
    bool inclusiveTracking;
    

      
  public:
};

#endif

