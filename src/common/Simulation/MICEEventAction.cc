#include "Simulation/MICEEventAction.hh"
#include "Config/BeamParameters.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "Interface/dataCards.hh"
#include <iostream>
#include <fstream>
#include <unistd.h>
using std::string;
using std::endl;

#include "Interface/MICEEvent.hh"
#include "Interface/MCParticle.hh"
#include "BeamTools/BTPhaser.hh"

extern MICEEvent simEvent;

//extern void writeEvent();

MICEEventAction::MICEEventAction() :_btPhaserDone(true) { theVirtualPlanes = VirtualPlaneManager::getVirtualPlaneManager();  }

MICEEventAction::~MICEEventAction() { }

void MICEEventAction::BeginOfEventAction(const G4Event* currentEvent)
{
  theVirtualPlanes->StartOfEvent();
  SetRandomSeed((long)currentEvent->GetEventID() + (long)BeamParameters::getInstance()->RandomSeed());
}

void MICEEventAction::EndOfEventAction(const G4Event* currentEvent)
{
  // take the G4Trajectory information and extract the points, adding them to each MCParticle

  G4TrajectoryContainer* trajs = currentEvent->GetTrajectoryContainer();
  std::cout<<"number of trajs: "<<trajs->entries()<<std::endl;
  if( trajs )
    for( G4int i = 0; i < trajs->entries(); ++i )
    {
      G4VTrajectory* traj = (*trajs)[i];
      std::cout<<traj->GetParticleName()<<endl;
      traj->ShowTrajectory();
      MCParticle* found = NULL;

       for( unsigned int j = 0; j < simEvent.mcParticles.size(); ++j )
          if( simEvent.mcParticles[j]->trackID() == traj->GetTrackID() )
             found = simEvent.mcParticles[j];

      if( found )
      {
        for( int j = 0; j < traj->GetPointEntries(); ++j )
          found->addPoint( traj->GetPoint( j )->GetPosition() );
      }
  }

  //  writeEvent();

  G4String drawFlag = "all";
  if (G4VVisManager::GetConcreteInstance()) 
  {
    G4TrajectoryContainer* trajectoryContainer = currentEvent->GetTrajectoryContainer();
    G4int n_trajectories = 0;
    if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

    for (G4int i=0; i<n_trajectories; i++) 
    {
      G4Trajectory* trj = (G4Trajectory*)
                              ((*(currentEvent->GetTrajectoryContainer()))[i]);
      if (drawFlag == "all") trj->DrawTrajectory(50);
      else if ((drawFlag == "charged")&&(trj->GetCharge() != 0.))
                              trj->DrawTrajectory(50);
      else if ((drawFlag == "neutral")&&(trj->GetCharge() == 0.))
                              trj->DrawTrajectory(50);
    }
  }
  
  BTPhaser::IsPhaseSet(_btPhaserDone);
}

void MICEEventAction::SetRandomSeed(long seed)
{
  CLHEP::HepRandom::setTheSeed(seed);
  //Changing the seed by a small amount should generate different events
  long reseed = CLHEP::RandFlat::shootInt(2147483647);
  CLHEP::HepRandom::setTheSeed(reseed);
  CLHEP::RandGauss::setFlag(false);
  //Fire some values so we don't reuse values that might have been seen
  //in primary generator for physics - hmm...
  for(int i=0; i<16; i++)
    CLHEP::RandGauss::shoot();
}

