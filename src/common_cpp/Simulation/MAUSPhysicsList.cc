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

#include <limits>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "json/json.h"

#include "Geant4/G4ProcessManager.hh"
#include "Geant4/G4ProcessTable.hh"
#include "Geant4/G4ProcessVector.hh"

#include "Geant4/G4ParticleTypes.hh"
#include "Geant4/G4ParticleTable.hh"

#include "Geant4/G4PionDecayMakeSpin.hh"
#include "Geant4/G4MuonDecayChannelWithSpin.hh"
#include "Geant4/G4MuonRadiativeDecayChannelWithSpin.hh"
#include "Geant4/G4DecayWithSpin.hh"
#include "Geant4/G4DecayPhysics.hh"
#include "Geant4/G4DecayTable.hh"

#include "Geant4/globals.hh"
#include "Geant4/G4StepLimiter.hh"
#include "Geant4/G4UserSpecialCuts.hh"
#include "Geant4/G4ProductionCuts.hh"
#include "Geant4/G4UImanager.hh"
#include "Geant4/G4PhysListFactory.hh"

#include "Geant4/G4Region.hh"
#include "Geant4/G4RegionStore.hh"

#include "Utils/Squeak.hh"
#include "Interface/STLUtils.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"
#include "src/common_cpp/Simulation/DetectorConstruction.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

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

double MAUSPhysicsList::_defaultChargedPiHalfLife = -1;
double MAUSPhysicsList::_defaultChargedMuHalfLife = -1;


MAUSPhysicsList::MAUSPhysicsList(G4VModularPhysicsList* physList)
                                                       : G4VUserPhysicsList(), _polDecay(false) {
  _list = physList;
  _list->RegisterPhysics(new G4DecayPhysics());
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
    catch (Exceptions::Exception exc) {
        delete mpl;
        throw exc;
    }
  } else {
    throw(Exceptions::Exception(Exceptions::recoverable,
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
                              << "\n polarised muons " << _polDecay
                              << std::endl;
  SetStochastics(_msModel, _dEModel, _hadronicModel, _partDecay);
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
  if (decay) {
      SetHalfLife(_piHalfLife, _muHalfLife);
  } else {
      // note that disabling decays makes a G4Exception, as per #1404
      // This is a G4 bug.
      //
      // Here we just set the lifetime to be very long (this could cause a
      // problem if user sets simulation time to be even longer, default
      // max time is 1e9 nanoseconds
      double life = std::numeric_limits<double>::max()/10.;
      SetHalfLife(life, life);
  }
}


void MAUSPhysicsList::SetEnergyLoss(eloss eLossModel) {
  double cutDouble = _productionThreshold;
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
  std::vector<std::string> uiCommand;
  if (eLossModel != energyStraggling)
    uiCommand.push_back("/run/setCut "+STLUtils::ToString(cutDouble));
  uiCommand.push_back("/process/eLoss/fluct "+flucActive);
  for (int i = 0; i < _nELossNames; i++)
    uiCommand.push_back("/process/"+elossActive+" "+_eLossNames[i]);

  for (size_t i = 0; i < uiCommand.size(); i++) {
    UIApplyCommand(uiCommand[i]);
  }

  // set by volume
  MAUSGeant4Manager* g4man = MAUSGeant4Manager::GetInstance();
  std::vector<std::string> regions = g4man->GetGeometry()->GetRegions();
  for (size_t i = 0; i < regions.size() && eLossModel == energyStraggling; ++i) {
      std::map<std::string, double> thresholds;
      if (_fineGrainedProductionThreshold.find(regions[i]) !=
                                        _fineGrainedProductionThreshold.end()) {
          thresholds = _fineGrainedProductionThreshold[regions[i]];
      }
      SetProductionThresholdByVolume(regions[i], cutDouble, thresholds);
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
    if (_polDecay == true) {
        G4DecayTable* MuonPlusDecayTable = new G4DecayTable();
        MuonPlusDecayTable -> Insert(new G4MuonDecayChannelWithSpin
                                                                ("mu+", 0.986));
        MuonPlusDecayTable -> Insert(new G4MuonRadiativeDecayChannelWithSpin
                                                                ("mu+", 0.014));
        G4MuonPlus::MuonPlusDefinition()->SetDecayTable(MuonPlusDecayTable);

        G4DecayTable* MuonMinusDecayTable = new G4DecayTable();
        MuonMinusDecayTable -> Insert(new
                               G4MuonDecayChannelWithSpin("mu-", 0.986));
        MuonMinusDecayTable -> Insert(new G4MuonRadiativeDecayChannelWithSpin
                                                                ("mu-", 0.014));
        G4MuonMinus::MuonMinusDefinition()->SetDecayTable(MuonMinusDecayTable);
    }
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
        G4ProcessManager* pmanager = NULL;
        G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();
        G4VProcess* decay = NULL;

        ////////// MUON PLUS ///////////
        pmanager = G4MuonPlus::MuonPlus()->GetProcessManager();
        G4DecayWithSpin* decayWithSpinPlus = new G4DecayWithSpin();
        decay = processTable->FindProcess("Decay", G4MuonPlus::MuonPlus());
        if (pmanager) {
            if (decay)
                pmanager->RemoveProcess(decay);
            pmanager->AddProcess(decayWithSpinPlus);
            processTable->Insert(decayWithSpinPlus,
                                 G4MuonPlus::MuonPlus()->GetProcessManager());
            // set ordering for PostStepDoIt and AtRestDoIt
            pmanager ->SetProcessOrdering(decayWithSpinPlus, idxPostStep);
            pmanager ->SetProcessOrdering(decayWithSpinPlus, idxAtRest);
        }

        ////////// MUON MINUS ///////////
        pmanager = G4MuonMinus::MuonMinus()->GetProcessManager();
        G4DecayWithSpin* decayWithSpinMinus = new G4DecayWithSpin();
        decay = processTable->FindProcess("Decay", G4MuonMinus::MuonMinus());
        if (pmanager) {
            if (decay)
                pmanager->RemoveProcess(decay);
            pmanager->AddProcess(decayWithSpinMinus);
            processTable->Insert(decayWithSpinMinus,
                                 G4MuonMinus::MuonMinus()->GetProcessManager());
            // set ordering for PostStepDoIt and AtRestDoIt
            pmanager->SetProcessOrdering(decayWithSpinMinus, idxPostStep);
            pmanager->SetProcessOrdering(decayWithSpinMinus, idxAtRest);
        }
    }
}

void MAUSPhysicsList::SetHalfLife(double pionHalfLife,  double muonHalfLife) {
  if (_defaultChargedPiHalfLife < 0 || _defaultChargedMuHalfLife < 0) {
      G4ParticleTable* p_table = G4ParticleTable::GetParticleTable();
      _defaultChargedPiHalfLife = p_table->FindParticle(211)->GetPDGLifeTime();
      _defaultChargedMuHalfLife = p_table->FindParticle(13)->GetPDGLifeTime();
  }
  if (pionHalfLife <= 0.)
      pionHalfLife = _defaultChargedPiHalfLife;
  if (muonHalfLife <= 0.)
      muonHalfLife = _defaultChargedMuHalfLife;
  SetParticleHalfLife("pi+", pionHalfLife);
  SetParticleHalfLife("pi-", pionHalfLife);
  SetParticleHalfLife("mu+", muonHalfLife);
  SetParticleHalfLife("mu-", muonHalfLife);
}

void MAUSPhysicsList::SetParticleHalfLife(std::string particleName,
                                          double halfLife) {
  if (halfLife <= 0.)
      throw Exceptions::Exception(Exceptions::recoverable,
                      "Negative half life",
                      "MAUSPhysicsList::SetParticleHalfLife");
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
    Json::Value jsonFineGrained = JsonWrapper::GetProperty(dc,
                                          "fine_grained_production_threshold",
                                          JsonWrapper::objectValue);
    // loop over json {region1:{pid1:cut1, pid2:cut2}, region2:{...}, ...}
    std::vector<std::string> names = jsonFineGrained.getMemberNames();
    for (size_t i = 0; i < names.size(); ++i) {
        Json::Value jsonCuts = JsonWrapper::GetProperty(
                              jsonFineGrained,
                              names[i],
                              JsonWrapper::objectValue);
        std::vector<std::string> pidStrings = jsonCuts.getMemberNames();
        std::map<std::string, double> cuts;
        for (size_t j = 0; j < pidStrings.size(); ++j) {
            cuts[pidStrings[j]] = JsonWrapper::GetProperty(jsonCuts,
                                          pidStrings[j],
                                          JsonWrapper::realValue).asDouble();
        }
        _fineGrainedProductionThreshold[names[i]] = cuts;
    }
}

void MAUSPhysicsList::UIApplyCommand(std::string command) {
    Squeak::mout(Squeak::debug)
        << "Apply G4UI command: " << command << std::endl;
    G4UImanager::GetUIpointer()->ApplyCommand(command);
}

void MAUSPhysicsList::SetProductionThresholdByVolume(
                          std::string volumeName,
                          double defaultProductionThreshold,
                          std::map<std::string, double> particleIdToThreshold) {
    G4Region* region = G4RegionStore::GetInstance()->GetRegion(volumeName);
    if (region == NULL) {
        throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                              "Failed to find region "+volumeName+" for G4Cuts",
                              "MAUSPhysicsList::SetProductionThresholdByVolume");
    }
    G4ProductionCuts* cuts = new G4ProductionCuts();
    Squeak::mout(Squeak::debug) << "Cuts for Volume " << volumeName << std::endl;
    G4ParticleTable* ptable = G4ParticleTable::GetParticleTable();
    if (particleIdToThreshold.find("default") != particleIdToThreshold.end()) {
        defaultProductionThreshold = particleIdToThreshold["default"];
    }

    for (int i = 0; i < ptable->size(); ++i) {
        int pid = ptable->GetParticle(i)->GetPDGEncoding();
        std::string nameString = ptable->GetParticle(i)->GetParticleName();
        std::string pidString = STLUtils::ToString(pid);
        double cut = 0.;
        if (particleIdToThreshold.find(pidString) != particleIdToThreshold.end()) {
            cut = particleIdToThreshold[pidString];
        } else if (particleIdToThreshold.find(nameString) != particleIdToThreshold.end()) {
            cut = particleIdToThreshold[nameString];
        } else {
            cut = defaultProductionThreshold;
        }
        if (cut > 0.) {
            Squeak::mout(Squeak::debug) << "    " << pid << " " << cut << std::endl;
            cuts->SetProductionCut(cut, pid);
        } else {
            Squeak::mout(Squeak::debug) << "    " << pid
                                        << " -ve so not set" << std::endl;
        }
    }
    region->SetProductionCuts(cuts);
}
}

