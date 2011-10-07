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

#include "json/json.h"

#include "globals.hh"

#include "G4UImanager.hh"
#include "G4ProcessTable.hh"
#include "G4ProcessVector.hh"
#include "G4PhysListFactory.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"

#include "Interface/dataCards.hh"
#include "Interface/MICERun.hh"
#include "Interface/Squeak.hh"

namespace MAUS {

const std::string MAUSPhysicsList::_scatNames [] = {"muBrems", "hBrems",
                  "eBrems", "muPairProd", "ePairProd", "hPairProd",
                  "ElectroNuclear", "msc"};
const std::string MAUSPhysicsList::_eLossNames[] = {"muBrems", "hBrems",
                  "eBrems", "muPairProd", "ePairProd", "hPairProd", "muIoni",
                  "hIoni",  "eIoni"};
const int         MAUSPhysicsList::_nScatNames   = 8;
const int         MAUSPhysicsList::_nELossNames  = 9;

MAUSPhysicsList::MAUSPhysicsList(G4VModularPhysicsList* physList):
                                                G4VModularPhysicsList(*physList)
{}

MAUSPhysicsList::~MAUSPhysicsList()
{}

MAUSPhysicsList* MAUSPhysicsList::GetMAUSPhysicsList() {
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;
  std::string physModel = JsonWrapper::GetProperty
                     (dc, "physics_model", JsonWrapper::stringValue).asString();
  G4PhysListFactory fact;
  if(fact.IsReferencePhysList(physModel)) {
    MAUSPhysicsList* mpl =
                     new MAUSPhysicsList(fact.GetReferencePhysList(physModel) );
    try {
        mpl->Setup();
        return mpl;
    }
    catch(Squeal squee) {
        delete mpl;
        throw squee;
    }
  }
  else {
    throw(Squeal(Squeal::recoverable,
        "Failed to recognise physics list model "+physModel,
        "MAUSPhysicsList::GetMAUSPhysicsList()")
    );
  }
}

void MAUSPhysicsList::BeginOfReferenceParticleAction() {
  std::cerr //Squeak::mout(Squeak::debug)
        << "MAUSPhysicsList::BeginOfReferenceParticleAction() with de model "
        << _refDEModel << " script " << _refScript << std::endl;
  SetStochastics("none", _refDEModel, "none", "false");
  RunUserUICommand(_refScript);
}

void MAUSPhysicsList:: BeginOfRunAction() {
  Squeak::mout(Squeak::debug) << "MAUSPhysicsList::BeginOfRunAction() with"
                              << "\n  de model " << _dEModel
                              << "\n  msc model " << _msModel
                              << "\n  hadronic model " << _hadronicModel
                              << "\n  particle decay " << _partDecay
                              << "\n  pi 1/2 life " << _piHalfLife
                              << "\n  mu 1/2 life " << _muHalfLife
                              << "\n  script " << _runScript << std::endl;
  SetStochastics(_msModel, _dEModel, _hadronicModel, _partDecay);
  SetHalfLife   (_piHalfLife, _muHalfLife);
  RunUserUICommand(_runScript);
}

MAUSPhysicsList::scat    MAUSPhysicsList::ScatteringModel
                                                (std::string scatteringModel) {
  for(size_t i=0; i<scatteringModel.size(); i++) {
    scatteringModel[i] = tolower(scatteringModel[i]);
  }
  if(scatteringModel=="msc" || scatteringModel=="mcs") {
    return mcs;
  }
  if(scatteringModel=="none") {
    return no_scat;
  }
  throw(Squeal(Squeal::recoverable,
      "Scattering model "+scatteringModel+" not recognised",
      "MAUSPhysicsList::ScatteringModel(std::string)")
  );
}

MAUSPhysicsList::eloss   MAUSPhysicsList::EnergyLossModel
                                                      (std::string elossModel) {
  for(size_t i=0; i<elossModel.size(); i++) {
    elossModel[i] = tolower(elossModel[i]);
  }
  if(elossModel=="ionization" || elossModel=="ionisation" || elossModel=="dedx")
    return dedx;
  if(elossModel=="estrag")
    return energyStraggling;
  if(elossModel=="none")
    return no_eloss;
  throw(Squeal(Squeal::recoverable,
        "Energy loss model "+elossModel+" not recognised",
        "MAUSPhysicsList::EnergyLossModel(std::string)")
  );
}

MAUSPhysicsList::hadronic MAUSPhysicsList::HadronicModel
                                                   (std::string hadronicModel) {
  for(size_t i=0; i<hadronicModel.size(); i++)
    hadronicModel[i] = tolower(hadronicModel[i]);
  if( hadronicModel=="all" ) return  all_hadronic;
  if( hadronicModel=="none" ) return no_hadronic;
  throw(Squeal(Squeal::recoverable,
        "Did not recognise hadronic model "+hadronicModel,
        "MAUSPhysicsList::HadronicModel")
  );
}

void MAUSPhysicsList::SetStochastics(std::string scatteringModel,
                      std::string energyLossModel, std::string hadronicModel,
                      bool decayModel) {
  SetScattering( ScatteringModel(scatteringModel) );
  SetEnergyLoss( EnergyLossModel(energyLossModel) );
  SetHadronic  ( HadronicModel  (hadronicModel  ) );
  SetDecay     ( decayModel );
}

void MAUSPhysicsList::SetDecay(bool decay) {
  G4UImanager* UI                = G4UImanager::GetUIpointer();
  if(!decay) UI->ApplyCommand("/process/inactivate Decay");
  else       UI->ApplyCommand("/process/activate   Decay");
}


void MAUSPhysicsList::SetEnergyLoss(eloss eLossModel) {
  G4UImanager* UI        = G4UImanager::GetUIpointer();
  double       cutDouble = _productionThreshold;
  std::stringstream cutStream;
  std::string  elossActive = "activate";
  std::string  flucActive  = "true";
  switch(eLossModel) {
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
  for(int i=0; i<_nELossNames; i++)
    uiCommand.push_back("/process/"+elossActive+" "+_eLossNames[i]);

  for(size_t i=0; i<uiCommand.size(); i++) {
    Squeak::mout(Squeak::debug) << "Applying " << uiCommand[i] << std::endl;
    UI->ApplyCommand(uiCommand[i]);
  }
}


void MAUSPhysicsList::SetScattering(scat scatteringModel) {
  G4UImanager* UI          = G4UImanager::GetUIpointer();
  std::string activation;
  switch(scatteringModel) {
    case mcs:
      activation  = "/process/activate ";
      break;
    case no_scat:
      activation  = "/process/inactivate ";
      break;
  }

  for(int i=0; i<_nScatNames; i++) {
    Squeak::mout(Squeak::debug) << "Applying " << activation+_scatNames[i]
                                << std::endl;
    UI->ApplyCommand(activation+_scatNames[i]);
  }
}

void MAUSPhysicsList::SetHadronic(hadronic hadronicModel) {
  std::string activation;
  switch(hadronicModel) {
    case all_hadronic:
      activation  = "/process/activate ";
      break;
    case no_hadronic:
      activation  = "/process/inactivate ";
      break;
  }

  G4UImanager*       UI    = G4UImanager::GetUIpointer();
  G4ProcessVector*   pvec  = G4ProcessTable::GetProcessTable()->
                                                       FindProcesses(fHadronic);
  for(int i=0; i<pvec->size(); i++) {
    std::string pname = (*pvec)[i]->GetProcessName();
    Squeak::mout(Squeak::debug) << "Applying " << activation+pname << std::endl;
    UI->ApplyCommand( activation+pname );
  }
}

void MAUSPhysicsList::ConstructProcess() {
    G4VModularPhysicsList::ConstructProcess();
    SetSpecialProcesses();
}


void MAUSPhysicsList::SetSpecialProcesses() {
  theParticleIterator->reset(); //from G4VUserPhysicsList
  while( (*theParticleIterator)() ) {
    G4ProcessManager* pmanager = theParticleIterator->value()->
                                                            GetProcessManager();
    pmanager->AddProcess( new G4StepLimiter,-1,-1,2);
  }
}

void MAUSPhysicsList::RunUserUICommand(std::string filename) {
  Squeak::mout(Squeak::debug) << "Executing user macro " << filename
                              << std::endl;
  G4UImanager::GetUIpointer()->ApplyCommand("/control/execute "+filename);
}

void MAUSPhysicsList::SetHalfLife(double pionHalfLife,  double muonHalfLife) {
  SetParticleHalfLife("pi+", pionHalfLife);
  SetParticleHalfLife("pi-", pionHalfLife);
  SetParticleHalfLife("mu+", muonHalfLife);
  SetParticleHalfLife("mu-", muonHalfLife);
}

void MAUSPhysicsList::SetParticleHalfLife(std::string particleName,
                                          double halfLife) {
  if(halfLife <= 0.) return;
  G4UImanager* UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("particle/select "+particleName);
}

void MAUSPhysicsList::Setup() {
    Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;
    _refDEModel = JsonWrapper::GetProperty(dc, "reference_energy_loss_model",
                                          JsonWrapper::stringValue).asString();
    _refScript = JsonWrapper::GetProperty(dc, "reference_g4ui_script",
                                          JsonWrapper::stringValue).asString();
    _runScript = JsonWrapper::GetProperty(dc, "begin_of_run_g4ui_script",
                                          JsonWrapper::stringValue).asString();
    _msModel = JsonWrapper::GetProperty(dc, "multiple_scattering_model",
                                          JsonWrapper::stringValue).asString();
    _dEModel = JsonWrapper::GetProperty(dc, "energy_loss_model",
                                          JsonWrapper::stringValue).asString();
    _hadronicModel = JsonWrapper::GetProperty(dc, "hadronic_model",
                                          JsonWrapper::stringValue).asString();
    _partDecay = JsonWrapper::GetProperty(dc, "particle_decay",
                                          JsonWrapper::booleanValue).asBool();
    _piHalfLife = JsonWrapper::GetProperty(dc, "charged_pion_half_time",
                                          JsonWrapper::realValue).asDouble();
    _muHalfLife = JsonWrapper::GetProperty(dc, "muon_half_time",
                                          JsonWrapper::realValue).asDouble();
    _productionThreshold = JsonWrapper::GetProperty(dc, "production_threshold",
                                          JsonWrapper::realValue).asDouble();

}

}

