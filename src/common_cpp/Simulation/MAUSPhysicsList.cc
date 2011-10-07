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

#include "globals.hh"

#include "G4UImanager.hh"
#include "G4ProcessTable.hh"
#include "G4ProcessVector.hh"
#include "G4PhysListFactory.hh"

#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"

#include "Interface/dataCards.hh"
#include "Interface/MICERun.hh"
#include "Interface/Squeak.hh"

namespace MAUS {

const std::string MAUSPhysicsList::scatNames [] = {"muBrems", "hBrems",
                  "eBrems", "muPairProd", "ePairProd", "hPairProd",
                  "ElectroNuclear", "msc"};
const std::string MAUSPhysicsList::eLossNames[] = {"muBrems", "hBrems",
                  "eBrems", "muPairProd", "ePairProd", "hPairProd", "muIoni",
                  "hIoni",  "eIoni"};
const int         MAUSPhysicsList::nScatNames   = 8;
const int         MAUSPhysicsList::nELossNames  = 9;

MAUSPhysicsList::MAUSPhysicsList(G4VModularPhysicsList* physList):
                                                G4VModularPhysicsList(*physList)
{}

MAUSPhysicsList::~MAUSPhysicsList()
{}


MAUSPhysicsList* MAUSPhysicsList::GetMAUSPhysicsList()
{
  dataCards& dc = *MICERun::getInstance()->DataCards;
  std::string physModel     = dc.fetchValueString("PhysicsModel");
  G4PhysListFactory fact;
  if(fact.IsReferencePhysList(physModel)) {
    return new MAUSPhysicsList(fact.GetReferencePhysList(physModel) );
  }
  else {
    throw(Squeal(Squeal::recoverable,
        "Failed to recognise physics list model "+physModel,
        "MAUSPhysicsList::GetMAUSPhysicsList()")
    );
  }
}

void MAUSPhysicsList::BeginOfReferenceParticleAction()
{
  dataCards& dc = *MICERun::getInstance()->DataCards;
  std::string refDEModel = dc.fetchValueString("ReferenceEnergyLossModel");
  std::string refScript  = dc.fetchValueString("ReferenceG4UIScript");
  Squeak::mout(Squeak::debug)
        << "MAUSPhysicsList::BeginOfReferenceParticleAction() with de model "
        << refDEModel << " script " << refScript << std::endl;
  SetStochastics("none", refDEModel, "none", "false");
  RunUserUICommand(refScript);
}

void MAUSPhysicsList:: BeginOfRunAction()
{
  dataCards& dc = *MICERun::getInstance()->DataCards;
  std::string runScript     = dc.fetchValueString("BeginOfRunG4UIScript");
  std::string msModel       = dc.fetchValueString("MultipleScatteringModel");
  std::string dEModel       = dc.fetchValueString("EnergyLossModel");
  std::string hadronicModel = dc.fetchValueString("HadronicModel");
  std::string partDecay     = dc.fetchValueString("ParticleDecay");
  double      piHalfLife    = dc.fetchValueDouble("ChargedPionHalfTime");
  double      muHalfLife    = dc.fetchValueDouble("MuonHalfTime");

  Squeak::mout(Squeak::debug) << "MAUSPhysicsList::BeginOfRunAction() with"
                              << "\n  de model " << dEModel
                              << "\n  msc model " << msModel
                              << "\n  hadronic model " << hadronicModel
                              << "\n  particle decay " << partDecay
                              << "\n  pi 1/2 life " << piHalfLife
                              << "\n  mu 1/2 life " << muHalfLife
                              << "\n  script " << runScript << std::endl;
  SetStochastics(msModel, dEModel, hadronicModel, partDecay);
  SetHalfLife   (piHalfLife, muHalfLife);
  RunUserUICommand(runScript);
}

MAUSPhysicsList::scat    MAUSPhysicsList::ScatteringModel(std::string scatteringModel)
{
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

MAUSPhysicsList::eloss   MAUSPhysicsList::EnergyLossModel(std::string elossModel)
{
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

MAUSPhysicsList::hadronic MAUSPhysicsList::HadronicModel  (std::string hadronicModel)
{
  for(size_t i=0; i<hadronicModel.size(); i++)
    hadronicModel[i] = tolower(hadronicModel[i]);
  if( hadronicModel=="all" ) return  all_hadronic;
  if( hadronicModel=="none" ) return no_hadronic;
  throw(Squeal(Squeal::recoverable,
        "Did not recognise hadronic model "+hadronicModel,
        "MAUSPhysicsList::HadronicModel")
  );
}

bool     MAUSPhysicsList::DecayModel     (std::string decayModel)
{
  for(size_t i=0; i<decayModel.size(); i++)
    decayModel[i] = tolower(decayModel[i]);
  if( decayModel=="true" )
    return  true;
  if( decayModel=="false" )
    return false;
  throw(Squeal(Squeal::recoverable,
        "Did not recognise decay model "+decayModel,
        "MAUSPhysicsList::DecayModel")
  );
}


void MAUSPhysicsList::SetStochastics(std::string scatteringModel,
                      std::string energyLossModel, std::string hadronicModel,
                      std::string decayModel)
{
  SetScattering( ScatteringModel(scatteringModel) );
  SetEnergyLoss( EnergyLossModel(energyLossModel) );
  SetHadronic  ( HadronicModel  (hadronicModel  ) );
  SetDecay     ( DecayModel     (decayModel     ) );
}

void MAUSPhysicsList::SetDecay(bool decay)
{
  G4UImanager* UI                = G4UImanager::GetUIpointer();
  if(!decay) UI->ApplyCommand("/process/inactivate Decay");
  else       UI->ApplyCommand("/process/activate   Decay");
}


void MAUSPhysicsList::SetEnergyLoss(eloss eLossModel)
{
  dataCards& dc = *MICERun::getInstance()->DataCards;
  G4UImanager* UI        = G4UImanager::GetUIpointer();
  double       cutDouble = dc.fetchValueDouble("ProductionThreshold");
  std::stringstream cutStream;
  std::string  elossActive = "activate";
  std::string  flucActive  = "true";
  switch(eLossModel) {
    case energyStraggling:
      elossActive = "activate";
      flucActive  = "true";
      cutDouble   = dc.fetchValueDouble("ProductionThreshold");
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
  for(int i=0; i<nELossNames; i++)
    uiCommand.push_back("/process/"+elossActive+" "+eLossNames[i]);

  for(size_t i=0; i<uiCommand.size(); i++) {
    Squeak::mout(Squeak::debug) << "Applying " << uiCommand[i] << std::endl;
    UI->ApplyCommand(uiCommand[i]);
  }
}


void MAUSPhysicsList::SetScattering(scat scatteringModel)
{
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

  for(int i=0; i<nScatNames; i++) {
    Squeak::mout(Squeak::debug) << "Applying " << activation+scatNames[i]
                                << std::endl;
    UI->ApplyCommand(activation+scatNames[i]);
  }
}

void MAUSPhysicsList::SetHadronic(hadronic hadronicModel)
{
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


void MAUSPhysicsList::SetSpecialProcesses()
{
  theParticleIterator->reset(); //from G4VUserPhysicsList
  while( (*theParticleIterator)() ) {
    G4ProcessManager* pmanager = theParticleIterator->value()->
                                                            GetProcessManager();
    pmanager->AddProcess( new G4StepLimiter,-1,-1,2);
  }
}

void MAUSPhysicsList::RunUserUICommand(std::string filename)
{
  Squeak::mout(Squeak::debug) << "Executing user macro " << filename
                              << std::endl;
  G4UImanager::GetUIpointer()->ApplyCommand("/control/execute "+filename);
}

void MAUSPhysicsList::SetHalfLife(double pionHalfLife,  double muonHalfLife)
{
  SetParticleHalfLife("pi+", pionHalfLife);
  SetParticleHalfLife("pi-", pionHalfLife);
  SetParticleHalfLife("mu+", muonHalfLife);
  SetParticleHalfLife("mu-", muonHalfLife);
}

void MAUSPhysicsList::SetParticleHalfLife(std::string particleName,
                                          double halfLife)
{
  if(halfLife <= 0.) return;
  G4UImanager* UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("particle/select "+particleName);
  Squeak::mout(Squeak::error) << "SET THE HALF LIFE " << particleName
                              << std::endl;
}

}

