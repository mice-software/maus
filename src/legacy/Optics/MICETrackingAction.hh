#ifndef MICETrackingAction_h
#define MICETrackingAction_h 1

#include "Geant4/G4UserTrackingAction.hh"

class MICETrackingAction : public G4UserTrackingAction {

  public:
    MICETrackingAction(){
    };
    ~MICETrackingAction(){};
   
    void PreUserTrackingAction(const G4Track*);
    void PostUserTrackingAction(const G4Track*);

  //std::vector<MCParticle*>* particles;
  //std::vector<MCVertex*>* vertices;
  

};

#endif
