#include "globals.hh"

#include "G4UImanager.hh"
#include "G4ProcessTable.hh"
#include "G4ProcessVector.hh"
#include "G4PhysListFactory.hh"

#include "Interface/dataCards.hh"
#include "Interface/MICERun.hh"
#include "MICEPhysicsList.hh"
#include "Interface/Squeak.hh"


#include <iostream>

const std::string MICEPhysicsList::scatNames [] = {"muBrems", "hBrems", "eBrems", "muPairProd", "ePairProd", "hPairProd", "ElectroNuclear", "msc"};
const std::string MICEPhysicsList::eLossNames[] = {"muBrems", "hBrems", "eBrems", "muPairProd", "ePairProd", "hPairProd", "muIoni",  "hIoni",  "eIoni"};
const int         MICEPhysicsList::nScatNames   = 8;
const int         MICEPhysicsList::nELossNames  = 9;

MICEPhysicsList::MICEPhysicsList(G4VModularPhysicsList* physList): G4VModularPhysicsList(*physList)
{}

MICEPhysicsList::~MICEPhysicsList()
{}


MICEPhysicsList* MICEPhysicsList::GetMICEPhysicsList()
{
  dataCards& dc = *MICERun::getInstance()->DataCards;
  std::string physModel     = dc.fetchValueString("PhysicsModel");
  G4PhysListFactory fact;
  if(fact.IsReferencePhysList(physModel)) return new MICEPhysicsList(fact.GetReferencePhysList(physModel) );
  else throw(Squeal(Squeal::recoverable, "Failed to recognise physics list model "+physModel, "MICEPhysicsList::GetMICEPhysicsList()"));
}

void MICEPhysicsList::BeginOfReferenceParticleAction()
{
  dataCards& dc = *MICERun::getInstance()->DataCards;
  std::string refDEModel = dc.fetchValueString("ReferenceEnergyLossModel");
  std::string refScript  = dc.fetchValueString("ReferenceG4UIScript");
  Squeak::mout(Squeak::debug) << "MICEPhysicsList::BeginOfReferenceParticleAction() with de model " << refDEModel << " script " << refScript << std::endl;
  SetStochastics("none", refDEModel, "none", "false"); //scat, eloss, hadronic, decay
  RunUserUICommand(refScript);
}

void MICEPhysicsList:: BeginOfRunAction()
{
  dataCards& dc = *MICERun::getInstance()->DataCards;
  std::string runScript     = dc.fetchValueString("BeginOfRunG4UIScript");
  std::string msModel       = dc.fetchValueString("MultipleScatteringModel");
  std::string dEModel       = dc.fetchValueString("EnergyLossModel");
  std::string hadronicModel = dc.fetchValueString("HadronicModel");
  std::string muonDecay     = dc.fetchValueString("MuonDecay");
  std::string partDecay     = dc.fetchValueString("ParticleDecay");
  double      piHalfLife    = dc.fetchValueDouble("ChargedPionHalfTime");
  double      muHalfLife    = dc.fetchValueDouble("MuonHalfTime");
  
  Squeak::mout(Squeak::debug) << "MICEPhysicsList::BeginOfRunAction() with"
                              << "\n  de model " << dEModel
                              << "\n  msc model " << msModel
                              << "\n  hadronic model " << hadronicModel
                              << "\n  particle decay " << partDecay
                              << "\n  pi 1/2 life " << piHalfLife
                              << "\n  mu 1/2 life " << muHalfLife
                              << "\n  script " << runScript << std::endl;
  if(muonDecay != "") {
    partDecay = muonDecay; 
    Squeak::mout(Squeak::warning) << "Warning - using deprecated datacard MuonDecay. Please use ParticleDecay instead" << std::endl;
  }
  SetStochastics(msModel, dEModel, hadronicModel, partDecay);
  SetHalfLife   (piHalfLife, muHalfLife);
  RunUserUICommand(runScript);
}

MICEPhysicsList::scat    MICEPhysicsList::ScatteringModel(std::string scatteringModel)
{
  for(size_t i=0; i<scatteringModel.size(); i++) scatteringModel[i] = tolower(scatteringModel[i]);
  if(scatteringModel=="msc" || scatteringModel=="mcs") return mcs;
  if(scatteringModel=="none")   return no_scat;
  throw(Squeal(Squeal::recoverable, "Scattering model "+scatteringModel+" not recognised", "MICEPhysicsList::ScatteringModel(std::string)"));
}

MICEPhysicsList::eloss   MICEPhysicsList::EnergyLossModel(std::string elossModel)
{
  for(size_t i=0; i<elossModel.size(); i++) elossModel[i] = tolower(elossModel[i]);
  if(elossModel=="ionization" || elossModel=="ionisation" || elossModel=="dedx") return dedx;
  if(elossModel=="estrag") return energyStraggling;
  if(elossModel=="none")   return no_eloss;
  throw(Squeal(Squeal::recoverable, "Energy loss model "+elossModel+" not recognised", "MICEPhysicsList::EnergyLossModel(std::string)"));
}

MICEPhysicsList::hadronic MICEPhysicsList::HadronicModel  (std::string hadronicModel)
{
  for(size_t i=0; i<hadronicModel.size(); i++) hadronicModel[i] = tolower(hadronicModel[i]);
  if( hadronicModel=="all" ) return  all_hadronic;
  if( hadronicModel=="none" ) return no_hadronic;
  throw(Squeal(Squeal::recoverable, "Did not recognise hadronic model "+hadronicModel, "MICEPhysicsList::HadronicModel"));
}

bool     MICEPhysicsList::DecayModel     (std::string decayModel)
{
  for(size_t i=0; i<decayModel.size(); i++) decayModel[i] = tolower(decayModel[i]);
  if( decayModel=="true" ) return  true;
  if( decayModel=="false" ) return false;
  throw(Squeal(Squeal::recoverable, "Did not recognise decay model "+decayModel, "MICEPhysicsList::DecayModel"));
}


void MICEPhysicsList::SetStochastics(std::string scatteringModel, std::string energyLossModel, std::string hadronicModel, std::string decayModel)
{
  SetScattering( ScatteringModel(scatteringModel) );
  SetEnergyLoss( EnergyLossModel(energyLossModel) );
  SetHadronic  ( HadronicModel  (hadronicModel  ) );
  SetDecay     ( DecayModel     (decayModel     ) );
}

void MICEPhysicsList::SetDecay(bool decay)
{
  G4UImanager* UI                = G4UImanager::GetUIpointer();
  if(!decay) UI->ApplyCommand("/process/inactivate Decay");
  else       UI->ApplyCommand("/process/activate   Decay");
}


void MICEPhysicsList::SetEnergyLoss(eloss eLossModel)
{ 
  G4UImanager* UI        = G4UImanager::GetUIpointer();
  double       cutDouble = MyDataCards.fetchValueDouble("ProductionThreshold");
  std::stringstream cutStream;
  std::string  elossActive = "activate";
  std::string  flucActive  = "true";
  switch(eLossModel) {
    case energyStraggling:
      elossActive = "activate";
      flucActive  = "true";
      cutDouble   = MyDataCards.fetchValueDouble("ProductionThreshold");
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


void MICEPhysicsList::SetScattering(scat scatteringModel)
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
    Squeak::mout(Squeak::debug) << "Applying " << activation+scatNames[i] << std::endl;
    UI->ApplyCommand(activation+scatNames[i]);
  }
}

void MICEPhysicsList::SetHadronic(hadronic hadronicModel)
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
  G4ProcessVector*   pvec  = G4ProcessTable::GetProcessTable()->FindProcesses(fHadronic);
  for(int i=0; i<pvec->size(); i++) {
    std::string pname = (*pvec)[i]->GetProcessName();
    Squeak::mout(Squeak::debug) << "Applying " << activation+pname << std::endl;
    UI->ApplyCommand( activation+pname );
  }
}

void MICEPhysicsList::SetSpecialProcesses()
{
  theParticleIterator->reset(); //from G4VUserPhysicsList
  while( (*theParticleIterator)() ) {
    G4ProcessManager* pmanager = theParticleIterator->value()->GetProcessManager();
    pmanager->AddProcess( new G4StepLimiter,-1,-1,2);
  }  
}

void MICEPhysicsList::RunUserUICommand(std::string filename)
{
  Squeak::mout(Squeak::debug) << "Executing user macro " << filename << std::endl;
  G4UImanager::GetUIpointer()->ApplyCommand("/control/execute "+filename);
}

void MICEPhysicsList::SetHalfLife(double pionHalfLife,  double muonHalfLife)
{
  SetParticleHalfLife("pi+", pionHalfLife);
  SetParticleHalfLife("pi-", pionHalfLife);
  SetParticleHalfLife("mu+", muonHalfLife);
  SetParticleHalfLife("mu-", muonHalfLife);
}

void MICEPhysicsList::SetParticleHalfLife(std::string particleName, double halfLife)
{
  if(halfLife <= 0.) return;
  G4UImanager* UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("particle/select "+particleName);
  Squeak::mout(Squeak::error) << "SET THE HALF LIFE " << particleName << std::endl; 
}

