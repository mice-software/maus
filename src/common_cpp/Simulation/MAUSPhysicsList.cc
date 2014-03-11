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

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "json/json.h"


    
#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4ParticleTypes.hh"
#include "Geant4/G4ParticleTable.hh"
   
#include "Geant4/G4DecayPhysics.hh"
#include "Geant4/G4ProcessTable.hh"
 
#include "Geant4/G4PionDecayMakeSpin.hh"
#include "Geant4/G4DecayWithSpin.hh"
    
#include "Geant4/G4DecayTable.hh"
#include "Geant4/G4MuonDecayChannelWithSpin.hh"
#include "Geant4/G4MuonRadiativeDecayChannelWithSpin.hh"




#include "Geant4/globals.hh"
#include "Geant4/G4StepLimiter.hh"
#include "Geant4/G4UserSpecialCuts.hh"
#include "Geant4/G4UImanager.hh"
#include "Geant4/G4ProcessTable.hh"
#include "Geant4/G4ProcessVector.hh"
#include "Geant4/G4PhysListFactory.hh"

#include "Interface/Squeak.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"

namespace MAUS {

// Note muMsc and CoulombScat make an error message from G4.9.2 - but are needed
// for G4.9.5 so we leave them in... I could do some logic checking for G4
// version if it is a problem (Rogers)
const std::string MAUSPhysicsList::_scatNames[] = {"muBrems", "hBrems",
                  "eBrem", "muPairProd", "hPairProd",
                  "ElectroNuclear", "msc", "muMsc", "CoulombScat"};
const std::string MAUSPhysicsList::_eLossNames[] = {"muBrems", "hBrems",
                  "eBrem", "muPairProd", "hPairProd",
                  "muIoni", "hIoni",  "eIoni"};
const int         MAUSPhysicsList::_nScatNames   = 9;
const int         MAUSPhysicsList::_nELossNames  = 8;

MAUSPhysicsList::MAUSPhysicsList(G4VModularPhysicsList* physList)
                                                       : G4VUserPhysicsList(), _polDecay(false) {
  _list = physList;
}

MAUSPhysicsList::~MAUSPhysicsList() {
    for (size_t i = 0; i < _limits.size(); ++i) {
        delete _limits[i];
    }
    for (size_t i = 0; i < _specialCuts.size(); ++i) {
        delete _specialCuts[i];
    }
    delete _list;
}

//MAUSPhysicsList::MAUSPhysicsList() : G4VModularPhysicsList() 
 //   {
 //       RegisterPhysics(new G4DecayPhysics());
 //       RegisterPhysics(new MAUSExtraPhysics());
 //   }
   
//MAUSPhysicsList::~MAUSPhysicsList() {;}
    
    




MAUSPhysicsList* MAUSPhysicsList::GetMAUSPhysicsList() {
  Json::Value& dc = *Globals::GetInstance()->GetConfigurationCards();
  std::string physModel = JsonWrapper::GetProperty
                     (dc, "physics_model", JsonWrapper::stringValue).asString();
  G4PhysListFactory fact;
  if (fact.IsReferencePhysList(physModel)) {
    MAUSPhysicsList* mpl =
                     new MAUSPhysicsList(fact.GetReferencePhysList(physModel));
    try {
        mpl->Setup();
        return mpl;
    }
    catch (Exception exc) {
        delete mpl;
        throw exc;
    }
  } else {
    throw(Exception(Exception::recoverable,
        "Failed to recognise physics list model "+physModel,
        "MAUSPhysicsList::GetMAUSPhysicsList()")
    );
  }
}

void MAUSPhysicsList::BeginOfReferenceParticleAction() {
  Squeak::mout(Squeak::debug)
        << "MAUSPhysicsList::BeginOfReferenceParticleAction() with de model "
        << _refDEModel << std::endl;
  SetStochastics(no_scat, _refDEModel, no_hadronic, false);
}

void MAUSPhysicsList:: BeginOfRunAction() {
  Squeak::mout(Squeak::debug) << "MAUSPhysicsList::BeginOfRunAction() with"
                              << "\n  de model " << _dEModel
                              << "\n  msc model " << _msModel
                              << "\n  hadronic model " << _hadronicModel
                              << "\n  particle decay " << _partDecay
                              << "\n  pi 1/2 life " << _piHalfLife
                              << "\n  mu 1/2 life " << _muHalfLife 
                              << "\n polarised muons true/false ? "<< _polDecay<<std::endl; //added
  SetStochastics(_msModel, _dEModel, _hadronicModel, _partDecay);
  SetHalfLife(_piHalfLife, _muHalfLife);
}

void MAUSPhysicsList::SetStochastics(scat scatteringModel,
                      eloss energyLossModel, hadronic hadronicModel,
                      bool decayModel) {
  SetEnergyLoss(energyLossModel);
  SetScattering(scatteringModel);
  SetHadronic(hadronicModel);
  SetDecay(decayModel);
}

void MAUSPhysicsList::SetDecay(bool decay) {
  if (!decay) UIApplyCommand("/process/inactivate DecayWithSpin");
  else        UIApplyCommand("/process/activate DecayWithSpin");
}


void MAUSPhysicsList::SetEnergyLoss(eloss eLossModel) {
  double       cutDouble = _productionThreshold;
  std::stringstream cutStream;
  std::string  elossActive = "activate";
  std::string  flucActive  = "true";
  switch (eLossModel) {
    case energyStraggling:
      elossActive = "activate";
      flucActive  = "true";
      cutDouble   =  _productionThreshold;
      break;
    case dedx:
      elossActive = "activate";
      flucActive  = "false";
      cutDouble   = 1e12*mm;
      break;
    case no_eloss:
      elossActive = "inactivate";
      flucActive  = "false";
      cutDouble   = 1e12*mm;
      break;
  }
  cutStream << cutDouble;
  std::vector<std::string> uiCommand;
  uiCommand.push_back("/run/setCut "+cutStream.str());
  uiCommand.push_back("/process/eLoss/fluct "+flucActive);
  for (int i = 0; i < _nELossNames; i++)
    uiCommand.push_back("/process/"+elossActive+" "+_eLossNames[i]);

  for (size_t i = 0; i < uiCommand.size(); i++) {
    UIApplyCommand(uiCommand[i]);
  }
}


void MAUSPhysicsList::SetScattering(scat scatteringModel) {
  std::string activation;
  switch (scatteringModel) {
    case mcs:
      activation  = "/process/activate ";
      break;
    case no_scat:
      activation  = "/process/inactivate ";
      break;
  }

  for (int i = 0; i < _nScatNames; i++) {
     UIApplyCommand(activation+_scatNames[i]);
  }
}

void MAUSPhysicsList::SetHadronic(hadronic hadronicModel) {
  std::string activation;
  switch (hadronicModel) {
    case all_hadronic:
      activation  = "/process/activate ";
      break;
    case no_hadronic:
      activation  = "/process/inactivate ";
      break;
  }

  G4ProcessVector*   pvec  = G4ProcessTable::GetProcessTable()->
                                                       FindProcesses(fHadronic);
  for (int i = 0; i < pvec->size(); i++) {
    std::string pname = (*pvec)[i]->GetProcessName();
    UIApplyCommand(activation+pname);
  }
  delete pvec;
}

void MAUSPhysicsList::ConstructParticle() {
    _list->ConstructParticle();

}


void MAUSPhysicsList::ConstructProcess() {
    _list->ConstructProcess();  // BUG: Memory leak on G4 side
    SetSpecialProcesses();
}




void MAUSPhysicsList::SetSpecialProcesses() {
  theParticleIterator->reset();  // from G4VUserPhysicsList
  while ( (*theParticleIterator)() ) {
    G4ProcessManager* fmanager = theParticleIterator->value()->
                                                            GetProcessManager();
    // step limiter for G4StepMax parameter; _limits exists for memory cleanup
    _limits.push_back(new G4StepLimiter);
    fmanager->AddProcess(_limits.back(), -1, -1, 2);
    // track limiter for G4KinMax, etc
    _specialCuts.push_back(new G4UserSpecialCuts());
    fmanager->AddProcess(_specialCuts.back(), -1, -1, 3);
  }
  if (_polDecay == true) {
        std::cerr << "pol_decay" <<std::endl;
        G4ProcessManager* pmanager;
        pmanager = G4MuonPlus::MuonPlus()->GetProcessManager();
        G4DecayWithSpin* decayWithSpin = new G4DecayWithSpin();
    
        G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();
        G4ProcessVector* p = processTable->FindProcesses();
        for(int j=0; j<p->size() ; j++){
            std::cout<<(*p)[j]->GetProcessName()<<std::endl;
        }
        std::cerr<<"pol_decay_vector "<< p<<std::endl;
        G4VProcess* decay;
        decay = processTable->FindProcess("Decay",G4MuonPlus::MuonPlus());
        
      
   
        if (pmanager) {
            if (decay) pmanager->RemoveProcess(decay);
            pmanager->AddProcess(decayWithSpin);
            // set ordering for PostStepDoIt and AtRestDoIt
            pmanager ->SetProcessOrdering(decayWithSpin, idxPostStep);
            pmanager ->SetProcessOrdering(decayWithSpin, idxAtRest);
            }
        decay = processTable->FindProcess("Decay",G4MuonPlus::MuonPlus());
        std::cerr << "Decay " << decay << std::endl;
        decay = processTable->FindProcess("DecayWithSpin",G4MuonPlus::MuonPlus());
        std::cerr << "DecayWithSpin " << decay << std::endl;

        decay = processTable->FindProcess("Garbage",G4MuonPlus::MuonPlus());
        std::cerr << "Garbage " << decay << std::endl;
   
        decay = processTable->FindProcess("Decay",G4MuonMinus::MuonMinus());
   
        pmanager = G4MuonMinus::MuonMinus()->GetProcessManager();
   
        if (pmanager) {
            if (decay) pmanager->RemoveProcess(decay);
            pmanager->AddProcess(decayWithSpin);
            // set ordering for PostStepDoIt and AtRestDoIt
            pmanager ->SetProcessOrdering(decayWithSpin, idxPostStep);
            pmanager ->SetProcessOrdering(decayWithSpin, idxAtRest);
          }
 
        G4PionDecayMakeSpin* poldecay = new G4PionDecayMakeSpin();
 
       decay = processTable->FindProcess("Decay",G4PionPlus::PionPlus());
 
        pmanager = G4PionPlus::PionPlus()->GetProcessManager();
 
        if (pmanager) {
            if (decay) pmanager->RemoveProcess(decay);
            pmanager->AddProcess(poldecay);
              // set ordering for PostStepDoIt and AtRestDoIt
            pmanager ->SetProcessOrdering(poldecay, idxPostStep);
            pmanager ->SetProcessOrdering(poldecay, idxAtRest);
        }
 
        decay = processTable->FindProcess("Decay",G4PionMinus::PionMinus());

 
        pmanager = G4PionMinus::PionMinus()->GetProcessManager();
 
        if (pmanager) {
            if (decay) pmanager->RemoveProcess(decay);
                pmanager->AddProcess(poldecay);
                // set ordering for PostStepDoIt and AtRestDoIt
                pmanager ->SetProcessOrdering(poldecay, idxPostStep);
                pmanager ->SetProcessOrdering(poldecay, idxAtRest);
            }
    for(int j=0; j<p->size() ; j++){
            std::cout<<(*p)[j]->GetProcessName()<<std::endl;
        }
    }

  }


/*
  void MAUSPhysicsList::ConstructParticle() {
    

    MAUSPhysicsList::ConstructParticle();
    
    G4GenericIon::GenericIonDefinition();
  
    G4DecayTable* MuonPlusDecayTable = new G4DecayTable();
    MuonPlusDecayTable -> Insert(new
                              G4MuonDecayChannelWithSpin("mu+",0.986));
    MuonPlusDecayTable -> Insert(new
                              G4MuonRadiativeDecayChannelWithSpin("mu+",0.014));
    G4MuonPlus::MuonPlusDefinition() -> SetDecayTable(MuonPlusDecayTable);
   
    G4DecayTable* MuonMinusDecayTable = new G4DecayTable();
    MuonMinusDecayTable -> Insert(new
                       G4MuonDecayChannelWithSpin("mu-",0.986));
    MuonMinusDecayTable -> Insert(new
                           G4MuonRadiativeDecayChannelWithSpin("mu-",0.014));
    G4MuonMinus::MuonMinusDefinition() -> SetDecayTable(MuonMinusDecayTable);


    }

  void MAUSPhysicsList::ConstructProcess(){
     
    MAUSPhysicsList::ConstructProcess();
    
    G4DecayWithSpin* decayWithSpin = new G4DecayWithSpin();
    
    G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();
   
    G4VProcess* decay;
    decay = processTable->FindProcess("Decay",G4MuonPlus::MuonPlus());
    
    G4ProcessManager* pmanager;
    pmanager = G4MuonPlus::MuonPlus()->GetProcessManager();
   
    if (pmanager) {
        if (decay) pmanager->RemoveProcess(decay);
        pmanager->AddProcess(decayWithSpin);
            // set ordering for PostStepDoIt and AtRestDoIt
        pmanager ->SetProcessOrdering(decayWithSpin, idxPostStep);
        pmanager ->SetProcessOrdering(decayWithSpin, idxAtRest);
        }
   
    decay = processTable->FindProcess("Decay",G4MuonMinus::MuonMinus());
   
    pmanager = G4MuonMinus::MuonMinus()->GetProcessManager();
   
    if (pmanager) {
        if (decay) pmanager->RemoveProcess(decay);
        pmanager->AddProcess(decayWithSpin);
        // set ordering for PostStepDoIt and AtRestDoIt
        pmanager ->SetProcessOrdering(decayWithSpin, idxPostStep);
        pmanager ->SetProcessOrdering(decayWithSpin, idxAtRest);
      }
 
  G4PionDecayMakeSpin* poldecay = new G4PionDecayMakeSpin();
 
  decay = processTable->FindProcess("Decay",G4PionPlus::PionPlus());
 
  pmanager = G4PionPlus::PionPlus()->GetProcessManager();
 
  if (pmanager) {
      if (decay) pmanager->RemoveProcess(decay);
      pmanager->AddProcess(poldecay);
          // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(poldecay, idxPostStep);
      pmanager ->SetProcessOrdering(poldecay, idxAtRest);
  }
 
  decay = processTable->FindProcess("Decay",G4PionMinus::PionMinus());

 
  pmanager = G4PionMinus::PionMinus()->GetProcessManager();
 
  if (pmanager) {
      if (decay) pmanager->RemoveProcess(decay);
          pmanager->AddProcess(poldecay);
          // set ordering for PostStepDoIt and AtRestDoIt
          pmanager ->SetProcessOrdering(poldecay, idxPostStep);
          pmanager ->SetProcessOrdering(poldecay, idxAtRest);
      }
 
 }*/




void MAUSPhysicsList::SetHalfLife(double pionHalfLife,  double muonHalfLife) {
  SetParticleHalfLife("pi+", pionHalfLife);
  SetParticleHalfLife("pi-", pionHalfLife);
  SetParticleHalfLife("mu+", muonHalfLife);
  SetParticleHalfLife("mu-", muonHalfLife);
}

void MAUSPhysicsList::SetParticleHalfLife(std::string particleName,
                                          double halfLife) {
  if (halfLife <= 0.) return;
  std::stringstream ss_in;
  ss_in << halfLife;
  UIApplyCommand("/particle/select "+particleName);
  UIApplyCommand("/particle/property/lifetime "+ss_in.str()+" ns");
  UIApplyCommand("/particle/property/dump");
}

void MAUSPhysicsList::Setup() {
    Json::Value& dc = *Globals::GetInstance()->GetConfigurationCards();
    std::string refPhysicsModel = JsonWrapper::GetProperty(dc,
            "reference_physics_processes", JsonWrapper::stringValue).asString();
    std::string physicsModel = JsonWrapper::GetProperty(dc, "physics_processes",
                                          JsonWrapper::stringValue).asString();
    
    if (refPhysicsModel == "none")
      _refDEModel = no_eloss;
    if (refPhysicsModel == "mean_energy_loss")
      _refDEModel = dedx;

    if (physicsModel == "none") {
      _msModel = no_scat;
      _dEModel = no_eloss;
      _hadronicModel = no_hadronic;
    }

    if (physicsModel == "mean_energy_loss") {
      _msModel = no_scat;
      _dEModel = dedx;
      _hadronicModel = no_hadronic;
    }

    if (physicsModel == "standard") {
      _msModel = mcs;
      _dEModel = energyStraggling;
      _hadronicModel = all_hadronic;
    }

    _partDecay = JsonWrapper::GetProperty(dc, "particle_decay",
                                          JsonWrapper::booleanValue).asBool();
    _polDecay = JsonWrapper::GetProperty(dc, "polarised_decay",
                                          JsonWrapper::booleanValue).asBool();
    _piHalfLife = JsonWrapper::GetProperty(dc, "charged_pion_half_life",
                                          JsonWrapper::realValue).asDouble();
    _muHalfLife = JsonWrapper::GetProperty(dc, "muon_half_life",
                                          JsonWrapper::realValue).asDouble();
    _productionThreshold = JsonWrapper::GetProperty(dc, "production_threshold",
                                          JsonWrapper::realValue).asDouble();
}

void MAUSPhysicsList::UIApplyCommand(std::string command) {
    Squeak::mout(Squeak::debug)
        << "Apply G4UI command: " << command << std::endl;
    G4UImanager::GetUIpointer()->ApplyCommand(command);
}
}

