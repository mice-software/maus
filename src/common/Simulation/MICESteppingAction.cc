// MAUS WARNING: THIS IS LEGACY CODE.
#include "Simulation/MICESteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4TransportationManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4UserLimits.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4FieldManager.hh"
#include "G4Field.hh"
#include "G4UniformMagField.hh"
#include "BeamTools/BTPhaser.hh"
#include "Interface/dataCards.hh"
#include "Interface/Squeak.hh"
#include "Interface/KillHit.hh"
#include "Interface/MICEEvent.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4VVisManager.hh"
#include "G4Polyline.hh"


extern MICEEvent simEvent;

MICESteppingAction::MICESteppingAction(): m_StepStatistics(NULL)
{
  std::string rfType = "none"; //ME MyDataCards.fetchValueString("rfCellType");
  this->hasRF = (rfType  != "none");
  double rr = MyDataCards.fetchValueDouble("MaxRadiusInChannel");
  this->maxRadSqGlobal = rr*rr;
  this->maxZGlobal = MyDataCards.fetchValueDouble("MaxZInChannel");
  this->maxTGlobal = MyDataCards.fetchValueDouble("MaxTimeAllowed");
  theVirtualPlanes = VirtualPlaneManager::getVirtualPlaneManager();

  // added by rikard to debug steps and physicslist
  if (MyDataCards.fetchValueInt("StepStatisticsOutput"))
  {
    m_volumeIntr = MyDataCards.fetchValueString("StepStatisticsVolume");
    m_StepStatistics = new MICEStepStatistics();
  }
}

MICESteppingAction::~MICESteppingAction()
{
  if (m_StepStatistics) delete m_StepStatistics;
}

void MICESteppingAction::UserSteppingAction(const G4Step * aStep)
{
  G4Track* aTrack = aStep->GetTrack();
  G4String particleName = aTrack->GetDefinition()->GetParticleName();
  //Check whether I am stepping over a virtual plane; if "yes", set Virtual plane information
  //Nb: put before tracks are killed to make sure get output even on final step
//  theVirtualPlanes->VirtualPlanesSteppingAction(aStep);  comment out to make it compile

  // Readout code, special for debugging physicslist and cuts.
  // AssignElement must be called before the "return" command,
  // or we will never get statistics for the killing processes.

  if (m_StepStatistics) {
    if((m_volumeIntr == "All") | (m_volumeIntr == aTrack->GetVolume()->GetName()))
    {
      if (aStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL){
        m_StepStatistics->AssignElement(aStep);
      } else {
        m_StepStatistics->AssignElement(particleName, "UserLimit");
      }
    }
  }

  if (CheckStopAndKill(aTrack, aStep)) {
    aTrack->SetTrackStatus(fStopAndKill); // kill the track
    if(aTrack->GetTrackStatus()!=fAlive) { return; }
  }
  if (CheckStopButAlive(aTrack, aStep)) {
    aTrack->SetTrackStatus(fStopButAlive); // stop tracking it but allow rest processes to performed
  }
  // If we are tracing a reference particle, look for the
  // RF key word in the volume name, and the detector in the centre
  // of the r.f. cavity.
  if (!BTPhaser::IsPhaseSet())
    DoReferenceParticle(aTrack);
}


void MICESteppingAction::DoReferenceParticle(G4Track* aTrack)
{
  //Phasing model ... run reference particle, if tracks hit a cavity try to phase it
  //If phasing is outside tolerance run a new ref particle and rephase it
  //Etc until phasing is within tolerance (usually works on ~ 2nd iteration per cavity)

  //This is a minefield of flags - tread carefully, test thoroughly
  std::string  volumeName = aTrack->GetVolume()->GetName();
  G4StepPoint* PreStep    = aTrack->GetStep()->GetPreStepPoint();
  //if i am stepping into a cavity set the primary generator to run the ref particle from the start of the cavity
  //remember the initial energy
  if(!isInCavity && BTPhaser::IsCavity(volumeName) && PreStep->GetStepStatus()==fGeomBoundary && 0)
  {
     isInCavity   = true;
     refEnergyIn  = PreStep->GetTotalEnergy();
     cavityName   = volumeName;

  }
  //if i am stepping through the cavity detector set the position and time for phasing
  if(BTPhaser::IsCavityDetector(volumeName))
  {
     timeAtMiddle     = PreStep->GetGlobalTime();
     positionAtMiddle = PreStep->GetPosition();
     pastDetector     = true;
  }
  //if i am stepping out of a cavity attempt to set the phase
  //if this fails, kill the track; try again
  if(!BTPhaser::IsCavity(volumeName) && !BTPhaser::IsCavityDetector(volumeName) && isInCavity && pastDetector)
  {
    refEnergyOut = PreStep->GetTotalEnergy();
    if(!BTPhaser::SetThePhase(positionAtMiddle, timeAtMiddle, refEnergyOut - refEnergyIn))
    {
      aTrack->SetTrackStatus(fStopAndKill); // kill the track
      Squeak::mout(Squeak::debug) << "Trying again on cavity "+cavityName << std::endl;
      AddKillHit(aTrack, "Failed to phase cavity");
      pastDetector = false;

      //      if(m_eventAction) m_eventAction->SetBTPhaserDone(false);
    }
    else
    {
      Squeak::mout(Squeak::debug) << "Phased cavity "+cavityName << std::endl;
      isInCavity       = false; //continuing particle has now left the cavity
      pastDetector     = false;
      timeAtMiddle     = 0;
      positionAtMiddle = Hep3Vector();
      //      if(m_eventAction) m_eventAction->SetBTPhaserDone(true);
    }
  }
}

bool MICESteppingAction::CheckStopAndKill(const G4Track* aTrack, const G4Step* aStep)
{
  bool killIt = false;
  // This code kills everything but muons if datacard is set
  static bool onlyMuons = ((MyDataCards.fetchValueString("TrackOnlyMuons") == "True")
                  |(MyDataCards.fetchValueString("TrackOnlyMuons") == "true"));
  G4ThreeVector position = aTrack->GetPosition();
  double rrSq = (position[0]*position[0] + position[1]*position[1]);
  int pid = aTrack->GetDefinition()->GetPDGEncoding();
  double tGlobal = aTrack->GetGlobalTime();

  // kill all neutrinos
  if ((pid == 12) | (pid==-12) | (pid==14) | (pid==-14) | (pid==16) | (pid==-16)) 
  {
    killIt = true;
    AddKillHit(aTrack, "Never track neutrinos");
  }
  else if ( onlyMuons && ( abs( pid ) != 13 ) )
  {
    killIt = true;
    AddKillHit(aTrack, "Track only muons set");
  }
  else 
  {
    if( rrSq > this->maxRadSqGlobal )
    {
      killIt = true;
      AddKillHit(aTrack, "R > MaxRadiusInChannel");
    }
    else if(position[2] > this->maxZGlobal)
    {
      killIt = true;
      AddKillHit(aTrack, "Z > MaxZInChannel");
    }
    else if(tGlobal > this->maxTGlobal )
    {
      killIt = true;
      AddKillHit(aTrack, "Time > MaxTimeAllowed");
    }
  }
  return killIt;
}

bool MICESteppingAction::CheckStopButAlive(const G4Track* aTrack, const G4Step* aStep)
{
  bool stopIt = false;
    // This is to prevent a particle from being tracked indefinetely.
  int stepNr = aTrack->GetCurrentStepNumber();
  if (stepNr%MyDataCards.fetchValueInt("MaxStepsWOELoss") == 0){
    if ((stepNr > 50000) | (aStep->GetTotalEnergyDeposit() == 0.)){
      Squeak::mout(Squeak::info) << aTrack->GetDefinition()->GetParticleName() << " exceeded MaxNumberOfStepsWOELoss inside " <<  aTrack->GetVolume()->GetName() << std::endl;
      Squeak::mout(Squeak::debug) << "After steps: " << stepNr << ", kinetic energy: " << aTrack->GetKineticEnergy()/MeV << " MeV" << G4endl;
      Squeak::mout(Squeak::debug) << "x: " << aTrack->GetPosition() << "  p: " << aTrack->GetMomentum() << " t: " << aTrack->GetGlobalTime() << std::endl;
      //Stop particle
      stopIt = true;
      AddKillHit(aTrack, "Number of steps > MaxStepsWOELoss");
    }
  }
  return stopIt;
}

void MICESteppingAction::AddKillHit(const G4Track* aTrack, std::string reason) const
{
  G4StepPoint*          Step = aTrack->GetStep()->GetPostStepPoint();
  G4ParticleDefinition* Def  = aTrack->GetDefinition();
  simEvent.killHits.push_back( new KillHit(aTrack->GetTrackID(), Def->GetPDGEncoding(), Def->GetPDGCharge(), Def->GetPDGMass(), 
                                           Step->GetPosition(), Step->GetMomentum(), Step->GetGlobalTime(), Step->GetTotalEnergy(), reason) );
}


