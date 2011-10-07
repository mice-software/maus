/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MICEPhysicsList_h
#define MICEPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4UImanager.hh"

#include "G4UserSpecialCuts.hh"
#include "G4StepLimiter.hh"

namespace MAUS {

/** MAUSPhysicsList, wrapper for G4VModularPhysicsList
 *
 *  MAUSPhysicsList is used to set up the physics process model for G4MICE.
 *  By default, we use some user define G4VModularPhysicsList (probably
 *  defined by G4PhysListFactory, for example) and then make modifications
 *  to get it to do what we want.
 *
 *  Physics list is controlled by two methods
 *  (i) Predefined commands to switch on and off stochastic processes, defined
 *  by datacards
 *  (ii) User-defined G4UI macros to modify processes by hand
 */

class MAUSPhysicsList: public G4VModularPhysicsList
{
  public:

    /** Controls EM scattering model - Multiple Coulomb Scattering or None
     */
    enum scat    {mcs, no_scat};

    /** Controls EM energy loss model - Energy Straggling, dedx (mean energy
     *  loss) or None
     */
    enum eloss   {energyStraggling, dedx, no_eloss};

    /** Controls hadronic processes - all or nothing
     */
    enum hadronic{all_hadronic, no_hadronic};

    /** Static initialiser gets the physics list. Uses datacard "PhysicsModel"
     *  to determine what the modular physics list is and G4PhysListFactory
     *  class
     */
    static MAUSPhysicsList* GetMAUSPhysicsList();

    /** Deep copies the modular physics list directly
     *
     *  /param physList 
     */
    MAUSPhysicsList(G4VModularPhysicsList* physList);

    /** Destructor 
     */
    ~MAUSPhysicsList();

    /** Convert from string to scat enumeration
     *
     *  "msc", "mcs" for multiple coulomb scattering
     *  "none" for no scattering 
     *  case insensitive
     */
    scat     ScatteringModel(std::string scatteringModel);

    /** Convert from string to eloss enumeration
     *  - "ionisation","ionization" for mean energy loss only
     *  - "estrag" for energy straggling and mean energy loss
     *  - "none" no energy loss at all
     *  Case insensitive
     */
    eloss    EnergyLossModel(std::string elossModel);

    /** Convert from string to hadronic enumeration 
     *
     *   \param hadronicModel Accepts "all" or "none" to turn on or off hadronic
     *          model case insensitive
     */
    hadronic HadronicModel  (std::string hadronicModel);

    /** Set up reference particle physics model according to reference datacards
     */
    void BeginOfReferenceParticleAction();

    /** Set up particle physics model according to normal datacards
     */
    void BeginOfRunAction();

    /** Construct the process table and add any MAUS specific processes 
     */
    void ConstructProcess();
    
    /** Does nothing - included to overload pure virtual base class function
     */
    void SetCuts() {}

    /** Get all relevant datacards from the json configuration
     */
    void Setup();

  private:
    /** Add some specific processes for MAUS
     *
     *  Only add a step limiter
     */
    void SetSpecialProcesses();

    /** Set g4 particle physics model using strings defined elsewhere
     *
     *  \param scatteringModel transverse scattering model 
     *  \param energyLossModel energy loss model 
     *  \param hadronicModel hadronic process model 
     *  \param decayModel are decays active 
     */
    void SetStochastics  (std::string scatteringModel,
                          std::string energyLossModel,
                          std::string hadronicModel,
                          bool decayModel);

    /** Set pion or muon half life (if specified)
     *
     *  \param pionHalfLife charged pion halfLife in ns
     *  \param muonHalfLife charged muon halfLife in ns
     */
    void SetHalfLife     (double pionHalfLife,  double muonHalfLife);

    /** Run UI macro with name filename
     *
     *  Can override these settings with a UI macro
     */
    void RunUserUICommand(std::string filename);

    /** Set scattering model for electromagnetic scattering processes
     *
     *  Modifies muBrems hBrems eBrems muPairProd ePairProd hPairProd
     *           ElectroNuclear msc G4 processes
     *
     *  \param scatteringModel if scatteringModel = no_scat, deactivates the
     *         processes; if scatteringModel = mcs, activates the processes
     */
    void SetScattering(scat     scatteringModel);

    /** Set energy loss model for electromagnetic processes
     *
     *  Modifies muBrems hBrems eBrems muPairProd ePairProd hPairProd muIoni
     *           hIoni eIoni GEANT4 processes
     *  \param energyLossModel If energyLoss = no_eloss, deactivates the
     *         processes; if energyLoss = dedx, activates the processes but sets
     *         cuts to 1000 km and deactivates fluctuations; if energyLoss = 
     *         energyStraggling, activates all processes and sets cuts to
     *         ProductionThreshold datacard
     */
    void SetEnergyLoss(eloss    energyLossModel);

    /** Set hadronic process model
     *
     *  Modifies all hadronic processes
     *  
     *  \param hadronicModel If hadronicModel is all_hadronic, activate all
     *         hadronic processes in the G4ProcessTable. If hadronicModel is
     *         no_hadronic, inactivate all hadronic processes in the
     *         G4ProcessTable
     */
    void SetHadronic  (hadronic hadronicModel);

    /** Set particle decays
     *  
     *  \param willDecay Set to true to activate particle decay for all
     *                   particles; set to false to inactivate decay
     */
    void SetDecay     (bool     willDecay);

    /** Set the half life for particle with G4Name particleName
     *
     *  \param particleName geant4 name of the particle
     *  \param halfLife half life [ns]
     */
    void SetParticleHalfLife  (std::string particleName, double halfLife);

  private:
    static const std::string _scatNames [];
    static const std::string _eLossNames[];
    static const int         _nScatNames;
    static const int         _nELossNames;

    std::string _refDEModel;
    std::string _refScript;
    std::string _runScript;
    std::string _msModel;
    std::string _dEModel;
    std::string _hadronicModel;
    bool        _partDecay;
    double      _piHalfLife;
    double      _muHalfLife;
    double      _productionThreshold;


};

}

#endif



