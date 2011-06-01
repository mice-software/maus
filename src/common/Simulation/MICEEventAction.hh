// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef MICEEventAction_H
#define MICEEventAction_H 1

#include "globals.hh"
#include "G4UserEventAction.hh"
#include "src/cpp_core/Simulation/VirtualPlanes.hh"

//MICEEventAction controls what happens at the beginning and end of every event
//GEANT4 calls BeginOfEventAction and EndOfEventAction at the beginning and end of the event
//Used for clearing virtual planes, setting random seed, setting visualisation, ...
//Need to have an extern writeEvent() that writes the event to some file (gak!)
class MICEEventAction : public G4UserEventAction
{
  public:
    MICEEventAction();
    virtual ~MICEEventAction();

    //Runs VirtualPlanes BeginOfEventAction
    //Resets the RandomSeed
    virtual void BeginOfEventAction(const G4Event* currentEvent);
    //Does something I don't understand with G4Traj
    //Calls writeEvent() extern that writes detector hits etc to file
    //Tells visualisation to draw trajectories
    //Tells BTPhaser whether phase was set
    virtual void EndOfEventAction(const G4Event* currentEvent);

    //Set the random seed for the event
    void SetRandomSeed(long seed);
    //Set to true if _btPhaserDone is true and isDone is true
    //Trying to make _btPhaserDone AND of each attempt to phase a cavity 
    void SetBTPhaserDone(bool isDone) {_btPhaserDone = isDone;}

  private:
    VirtualPlaneManager *theVirtualPlanes;
    bool _btPhaserDone; //if btphaserdone is true at the end of event, I assume all phases were set correctly
};

#endif

