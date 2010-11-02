#ifndef MICEPhysicsList_h
#define MICEPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4UImanager.hh"

//! MICEPhysicsList, wrapper for G4VModularPhysicsList

//! MICEPhysicsList is used to set up the physics process model for G4MICE.
//! By default, we use some user define G4VModularPhysicsList (probably
//! defined by G4PhysListFactory, for example) and then make modifications
//! to get it to do what we want.
//!
//! Physics list is controlled by two methods
//! (i) Predefined commands to switch on and off stochastic processes, defined
//! by datacards
//! (ii) User-defined G4UI macros to modify processes by hand
//!

class MICEPhysicsList: public G4VModularPhysicsList
{
  public:
    enum scat    {mcs, no_scat};
    enum eloss   {energyStraggling, dedx, no_eloss};
    enum hadronic{all_hadronic, no_hadronic};

    static MICEPhysicsList* GetMICEPhysicsList();
    MICEPhysicsList(G4VModularPhysicsList* physList);
    ~MICEPhysicsList();

    scat     ScatteringModel(std::string scatteringModel);
    eloss    EnergyLossModel(std::string elossModel);
    hadronic HadronicModel  (std::string hadronicModel);
    bool     DecayModel     (std::string decayModel);

    void BeginOfReferenceParticleAction();
    void BeginOfRunAction();

    void SetStochastics  (std::string scatteringModel, std::string energyLossModel, std::string hadronicModel, std::string decayModel);
    void SetHalfLife     (double pionHalfLife,  double muonHalfLife);
    void RunUserUICommand(std::string filename);

    void SetScattering(scat     scatteringModel);
    void SetEnergyLoss(eloss    energyLossModel);
    void SetHadronic  (hadronic hadronicModel);
    void SetDecay     (bool     willDecay);
    void SetParticleHalfLife  (std::string particleName, double halfLife);

    void SetCuts() {}
  private:
    static const std::string scatNames [];
    static const std::string eLossNames[];
    static const int         nScatNames;
    static const int         nELossNames;

};

#endif



