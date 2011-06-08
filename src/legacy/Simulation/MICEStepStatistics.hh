// MAUS WARNING: THIS IS LEGACY CODE.

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef MICEStepStatistics_h
#define MICEStepStatistics_h 1

#include "G4Step.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MICEStepStatistics
{
 public:

  MICEStepStatistics();
 ~MICEStepStatistics();

  void AssignElement(const G4Step* aStep);
  void AssignElement(G4String particle, G4String process);
  void WriteStatistics(const int numberOfEvents);

 private:

  // The size of the array should be enough for detOnly mode.
  // The size could cause problem in Normal mode.
  // Solve either by making it sufficiently large or some fancy <vector> stuff.
  int m_statistics[20][30];
  std::vector<G4String> m_particleList;
  std::vector<G4String> m_processList;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
