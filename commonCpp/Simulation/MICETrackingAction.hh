#ifndef MICETrackingAction_h
#define MICETrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "Interface/MCParticle.hh"
#include "Interface/MCVertex.hh"
#include "Interface/SciFiHit.hh"
#include "Interface/TofHit.hh"
#include "Interface/VirtualHit.hh"
#include "Interface/EmCalHit.hh"
#include <vector>

class MICETrackingAction : public G4UserTrackingAction {

  public:
    MICETrackingAction(){
      //particles = new std::vector<MCParticle*>;
      //vertices = new std::vector<MCVertex*>;
    };
    ~MICETrackingAction(){};
   
    void PreUserTrackingAction(const G4Track*);
    void PostUserTrackingAction(const G4Track*);

  //std::vector<MCParticle*>* particles;
  //std::vector<MCVertex*>* vertices;
  

};

#endif
