#include "MICEStepStatistics.hh"

#include "G4SteppingManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MICEStepStatistics::MICEStepStatistics()
{
  // Make sure the array is defaulted to zero
  for (int i = 0; i<30; i++){
    for (int j = 0; j<20; j++){
      m_statistics[j][i] = 0;
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MICEStepStatistics::~MICEStepStatistics()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MICEStepStatistics::AssignElement(const G4Step* aStep){

  G4Track* aTrack = aStep->GetTrack();
  G4String particleName, processName;

  particleName = aTrack->GetDefinition()->GetParticleName();
  processName = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

  AssignElement(particleName, processName);
}

void MICEStepStatistics::AssignElement(G4String particleName, G4String processName)
{
  bool foundParticle = false;
  bool foundProcess = false;
  int size;
  int x, y;

  // assing particle
  size = m_particleList.size();
  for (y = 0; y < size; y++)
  {
    if (particleName == m_particleList[y]) {
      foundParticle = true;
      break;
    }
  }
  if (foundParticle == false) m_particleList.push_back(particleName);

  // assign process
  size = m_processList.size();
  for (x = 0; x < size; x++)
  {
    if (processName == m_processList[x]) {
      foundProcess = true;
      break;
    }
  }
  if (foundProcess == false) m_processList.push_back(processName);

  // count the step
  m_statistics[x+1][y+1]++;
}

void MICEStepStatistics::WriteStatistics(int numberOfEvents){
  const int magnitude = 9;
  char entry[magnitude+1];
  char row[20*(magnitude+1)+5];
  char yChar[3], xChar[magnitude+1];
  int sizeX, sizeY;

  sizeY = m_particleList.size();
  sizeX = m_processList.size();
  FILE* statFile = fopen("StepStat.out","w");

  // Write header
  fputs("*** Statistics file, particles and processes ***\n", statFile);
  fputs("* Particles and processes are automatically added in order of appearance.\n", statFile);
  fputs("* Each entry contains the quantity of the combination divided by number of events.\n", statFile);
  fputs("* Four number of precision is used, scientific notation used when necessary.\n", statFile);
  strcpy(row, "* Number of events: ");
  sprintf(entry, "%10.4g", (float)numberOfEvents);
  strcat(row, entry);
  fputs(row, statFile);

  // Write particle list
  strcpy(row, "\n* Row: ");
  for (int i = 1; i <= sizeY; i++)
  {
    sprintf(yChar, "%2d", i);
    strcat(row, yChar);
    strcat(row, "=");
    strcat(row, m_particleList[i-1]);
    if (i%10 == 0) {
      fputs(row, statFile);
      strcpy(row, "\n* Row: ");
    } else {
      strcat(row, ", ");
    }
  }
  strcat(row, "\n");
  fputs(row, statFile);

  // Write process list
  strcpy(row, "* Col: ");
  for (int i = 1; i <= sizeX; i++)
  {
    sprintf(yChar, "%2d", i);
    strcat(row, yChar);
    strcat(row, "=");
    strcat(row, m_processList[i-1]);
    if (i%10 == 0) {
      fputs(row, statFile);
      strcpy(row, "\n* Col: ");
    } else {
      strcat(row, ", ");
    }
  }
  strcat(row, "\n");
  fputs(row, statFile);

  // Write content and axis
  for (int y = 0; y <= sizeY; y++){
    sprintf(yChar, "%2d", y);
    for (int x = 0; x <= sizeX; x++){
      if (x == 0) {
        strcpy(row, yChar);
      } else {
        if (y == 0) {
          sprintf(xChar, "%10d", x);
          strcat(row, xChar);
        } else {
          //sprintf(entry, "%10d", m_statistics[x][y]);  //integers
          //sprintf(entry, "%10.4g", (float)m_statistics[x][y]);  //exps, total steps
          sprintf(entry, "%10.4g", (float)m_statistics[x][y]/numberOfEvents); //exps, steps/event
          strcat(row, entry);
        }
      }
    }
    strcat(row, "\n");
    fputs(row, statFile);
  }
  fclose(statFile);
}
