#include "src/common_cpp/Simulation/MAUSStackingActionKillNonMuons.hh"

G4ClassificationOfNewTrack
MAUSStackingActionKillNonMuons::ClassifyNewTrack(const G4Track * aTrack) {
  if (abs(aTrack->GetDefinition()->GetPDGEncoding()) != 13) return fKill;
  return fWaiting;
}



