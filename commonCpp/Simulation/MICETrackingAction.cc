#include "MICETrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4SteppingManager.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "Interface/dataCards.hh"

#include "Interface/MICEEvent.hh"
#include "BeamTools/BTPhaser.hh"

extern MICEEvent simEvent;

void MICETrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
/* ME
  if(aTrack->GetParentID()==0 && aTrack->GetUserInformation()==0) {
    G4Track* theTrack = (G4Track*)aTrack;
  }
*/
}

void MICETrackingAction::PostUserTrackingAction(const G4Track* atrack)
{
  // see if we have a new particle first

  MCParticle* found = NULL;

  for( unsigned int i = 0; i < simEvent.mcParticles.size(); ++i )
    if( simEvent.mcParticles[i]->trackID() == atrack->GetTrackID() )
      found = simEvent.mcParticles[i];

  if( ! found ) 
  {
    // make a MCParticle from the G4Track

    double pi = sqrt( atrack->GetVertexKineticEnergy() * ( 2 * atrack->GetDefinition()->GetPDGMass() + atrack->GetVertexKineticEnergy() ) );

    MCParticle* particle = new MCParticle( atrack->GetTrackID(),
    atrack->GetDefinition()->GetPDGCharge()/eplus,
    atrack->GetDefinition()->GetPDGEncoding(),
    atrack->GetDefinition()->GetPDGMass(),
    atrack->GetVertexKineticEnergy(),
    atrack->GetKineticEnergy(),
    atrack->GetVertexPosition(),
    atrack->GetPosition(),
    pi * atrack->GetVertexMomentumDirection(),
    atrack->GetMomentum(),
    atrack->GetGlobalTime(),
    atrack->GetGlobalTime(),
    BTPhaser::IsRefPart() );

    found = particle;

    simEvent.mcParticles.push_back( particle );
  }

/*
  // now ensure that a MCVertex exists for the start and end of this particle

  bool start = true, end = true;

  for( unsigned int i = 0; i <  simEvent.mcVertices.size(); ++i )
  {
    // check whether particle vertices already exist
    //if yes add them

    if( particle->initialPosition() == simEvent.mcVertices[i]->position()
     && particle->initialTime() == simEvent.mcVertices[i]->time() )
    {
      simEvent.mcVertices[i]->AddParticle(particle);
      particle->SetInitialVertex(simEvent.mcVertices[i]);
      start=false;
    }
    else if( particle->finalPosition() == simEvent.mcVertices[i]->position()
          && particle->finalTime() == simEvent.mcVertices[i]->time() )
    {
      simEvent.mcVertices[i]->AddParticle(particle);
      particle->SetFinalVertex(simEvent.mcVertices[i]);
      end=false;
    }
  }

  if( start )
  {
    MCVertex* vertex1 = new MCVertex( particle->initialPosition(), particle->initialTime() );
    vertex1->AddParticle( particle );
    particle->SetInitialVertex(vertex1);
    simEvent.mcVertices.push_back(vertex1);
  }

  if( end )
  {
    MCVertex* vertex2 = new MCVertex( particle->finalPosition(), particle->finalTime() );
    vertex2->AddParticle(particle);
    particle->SetFinalVertex(vertex2);
    simEvent.mcVertices.push_back(vertex2);
  }
*/

  G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
  if(secondaries)
  {
    size_t n2 = secondaries->size();
    if(n2>0)    {
      for(size_t i=0; i < n2; i++)
      {
      }
    }
  }

}
