// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef MICEPhysicsList_h
#define MICEPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4UImanager.hh"

#include "G4UserSpecialCuts.hh"
#include "G4StepLimiter.hh"

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
    //! Controls EM scattering model - Multiple Coulomb Scattering or None
    enum scat    {mcs, no_scat};
    //! Controls EM energy loss model - Energy Straggling dedx (mean energy loss) or None
    enum eloss   {energyStraggling, dedx, no_eloss};
    //! Controls hadronic processes - all or nothing
    enum hadronic{all_hadronic, no_hadronic};

    //! Static initialiser gets the physics list using datacard "PhysicsModel" to determine
    //! what the modular physics list is and G4PhysListFactory class
    static MICEPhysicsList* GetMICEPhysicsList();

    //! Just copies the modular physics list directly
    MICEPhysicsList(G4VModularPhysicsList* physList);

    //! Destructor 
    ~MICEPhysicsList();

    //! Convert from string to scat enumeration - accepts "msc", "mcs", "none"; case insensitive
    scat     ScatteringModel(std::string scatteringModel);
    //! Convert from string to eloss enumeration - accepts "ionisation", "ionization", "estrag", "none"; case insensitive
    eloss    EnergyLossModel(std::string elossModel);
    //! Convert from string to hadronic enumeration - accepts "all", "none"; case insensitive
    hadronic HadronicModel  (std::string hadronicModel);
    //! Convert from string to DecayModel - accepts "true", "false"; case insensitive
    bool     DecayModel     (std::string decayModel);

    //! Set up reference particle physics model according to reference particle datacards
    void BeginOfReferenceParticleAction();
    //! Set up normal particle physics model according to normal particle datacards
    void BeginOfRunAction();

    //! Add some G4MICE specific processes
    void ConstructProcess() {G4VModularPhysicsList::ConstructProcess(); SetSpecialProcesses();}
    void SetSpecialProcesses();

    //! Set particle physics model using strings defined and mappings defined elsewhere
    void SetStochastics  (std::string scatteringModel, std::string energyLossModel, std::string hadronicModel, std::string decayModel);
    //! Set pion or muon half life (if specified)
    void SetHalfLife     (double pionHalfLife,  double muonHalfLife);
    //! Run UI macro with name filename
    void RunUserUICommand(std::string filename);

    //! Set scattering model for electromagnetic scattering processes

    //! Modifies muBrems hBrems eBrems muPairProd ePairProd hPairProd ElectroNuclear msc G4 processes
    //! If scatteringModel = no_scat, deactivates the processes; if scatteringModel = mcs, activates the processes
    void SetScattering(scat     scatteringModel);

    //! Set energy loss model for electromagnetic processes

    //! Modifies muBrems hBrems eBrems muPairProd ePairProd hPairProd muIoni hIoni eIoni GEANT4 processes
    //! If energyLoss = no_eloss, deactivates the processes; if energyLoss = dedx, activates the processes 
    //! but sets cuts to 1000 km and deactivates fluctuations; if energyLoss = energyStraggling, activates
    //! all processes and sets cuts to ProductionThreshold datacard
    void SetEnergyLoss(eloss    energyLossModel);

    //! Set hadronic process model

    //! Modifies all hadronic processes
    //! If hadronicModel is all_hadronic, activate all hadronic processes in the G4ProcessTable
    //! If hadronicModel is no_hadronic, inactivate all hadronic processes in the G4ProcessTable
    void SetHadronic  (hadronic hadronicModel);

    //! Set to true to activate particle decay for all particles; set to false to inactivate decay
    void SetDecay     (bool     willDecay);

    //! Set the half life for particle with G4Name particleName
    void SetParticleHalfLife  (std::string particleName, double halfLife);
    
    //! Does nothing - included as part of the inheritance model
    void SetCuts() {}

  private:
    static const std::string scatNames [];
    static const std::string eLossNames[];
    static const int         nScatNames;
    static const int         nELossNames;

};

#endif



