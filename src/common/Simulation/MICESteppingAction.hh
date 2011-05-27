#ifndef MICESteppingAction_h
#define MICESteppingAction_h 1

#include "globals.hh"
#include "G4UserSteppingAction.hh"
#include "Simulation/MICEStepStatistics.hh"
#include "Simulation/VirtualPlanes.hh"
#include "Simulation/MICEPrimaryGeneratorAction.hh"
#include "Simulation/MICEEventAction.hh"

class MICESteppingAction : public G4UserSteppingAction
{
  public:
    MICESteppingAction();
    ~MICESteppingAction();

    virtual void UserSteppingAction(const G4Step*);
    void CleanEmptyTraces();
    //added by rikard
    inline MICEStepStatistics* GetStepStatistics(){return m_StepStatistics;};
  //    void SetPrimaryGenerator(MICEPrimaryGeneratorAction * primaryGenerator) {m_generator=primaryGenerator;}
  //    void SetEventAction     (MICEEventAction            * eventAction)      {m_eventAction=eventAction;}

  private:
   void AddKillHit(const G4Track* aTrack, std::string reasonForDying) const;
   VirtualPlaneManager *theVirtualPlanes;
   bool hasRF;
   double maxRadSqGlobal;
   double maxTGlobal;
   double maxZGlobal;

   MICEStepStatistics* m_StepStatistics;
  //   MICEPrimaryGeneratorAction* m_generator;
   G4String m_volumeIntr;
  //   MICEEventAction* m_eventAction;

   //phasing information
   bool        isInCavity;
   bool        pastDetector;
   double      refEnergyIn;
   double      refEnergyOut;
   double      timeAtMiddle;
   Hep3Vector  positionAtMiddle;
   std::string cavityName;

   void DoReferenceParticle(G4Track* aTrack);
   bool CheckStopAndKill(const G4Track* aTrack, const G4Step* aStep); /// kill the track
   bool CheckStopButAlive(const G4Track* aTrack, const G4Step* aStep); /// stop the track and invoke rest processes


#ifdef ROOTFLAG
   TNtuple **rt_Traces, *rt_TraceRefPart; // rt_Traces is a pointer to an
                                          // array of pointers
#endif
};

#endif
