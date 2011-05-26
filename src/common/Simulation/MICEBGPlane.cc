#include "Simulation/MICEBGPlane.hh"
#include "Interface/MICEEvent.hh"
// #include "PersistClasses.hh"
// #include "TextFileReader.hh"
// #include "Interface/MiceEventManager.hh"
#include "Config/RFBackgroundParameters.hh"
#include "Interface/SpecialHit.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "CLHEP/Random/Randomize.h"
#include "Interface/dataCards.hh"

#include <iostream>
#include <fstream>

MICEBGPlane::MICEBGPlane(G4String source):m_particleType(source),  m_fileIn(NULL)
{
  SetParameters();
  DefineBackground();
}

MICEBGPlane::MICEBGPlane(std::ifstream* fileIn)
  :m_nE(1), m_samplingMode(1), m_R(150), m_z(0), m_dir(0), m_timeWindow(0),
      m_timeDelay(0), m_particleType("e-"), m_n(1), m_model("Simple"),
      m_energyMode("User"), m_E(1), m_EWidth(1), m_fileIn(fileIn),m_scale(1)
{
  SetParameters(); // These parameters are only used if there is a problem reading the file
  DefineBackgroundFromFile();
}

MICEBGPlane::MICEBGPlane(std::ifstream* fileIn, G4int numberOfHits)
  :m_nE(1), m_samplingMode(1), m_R(150), m_z(0), m_dir(0), m_timeWindow(0),
      m_timeDelay(0), m_particleType("e-"), m_n(1), m_model("Simple"),
      m_energyMode("User"), m_E(1), m_EWidth(1), m_fileIn(fileIn), m_scale(1)
{
  SetParameters(); // Only scale and time delay are normally used.
  //The other parameters are only used if there is a problem reading the file

  //DefineBackgroundFromFile();
}

MICEBGPlane::MICEBGPlane(G4double R, G4double z, G4double dir, G4double timeWindow,
      G4double timeDelay, G4String particleType, G4int n, G4String model,
      G4String energyMode, G4double E, G4double EWidth, std::ifstream* fileIn)
      :m_R(R), m_z(z), m_dir(dir), m_timeWindow(timeWindow),
      m_timeDelay(timeDelay), m_particleType(particleType), m_n(n), m_model(model),
      m_energyMode(energyMode), m_E(E), m_EWidth(EWidth), m_fileIn(fileIn)
{
  G4cout << m_energyMode << G4endl;
  G4cout << m_model << G4endl;

  DefineBackground();
}

MICEBGPlane::~MICEBGPlane()
{
 delete m_eSpectrum;
}

void MICEBGPlane::SetParameters()
{
  static RFBackgroundParameters* fRFBackgroundParameters = RFBackgroundParameters::getInstance();
  m_R = fRFBackgroundParameters->GetBGRadius();
  m_z = fRFBackgroundParameters->GetBGZstart();
  m_dir = fRFBackgroundParameters->GetBGDirection();
  m_timeWindow = fRFBackgroundParameters->GetBGTimeWindow();
  m_timeDelay = fRFBackgroundParameters->GetBGTimeDelay();
  m_scale = fRFBackgroundParameters->GetBGScaleFactor();
  m_mode = fRFBackgroundParameters->GetBGMode();
  m_bankSamplingMode = fRFBackgroundParameters->GetBGSamplingMode();
  G4String fileIn;
  if (m_particleType == "e-") {
    m_n = fRFBackgroundParameters->GetBGNumberOfElectrons();
    m_model = fRFBackgroundParameters->GetBGElectronModel();
    m_energyMode = fRFBackgroundParameters->GetBGElectronSpectrum();
    m_E = fRFBackgroundParameters->GetBGElectronEnergy();
    m_EWidth = fRFBackgroundParameters->GetBGElectronEnergyWidth();
    fileIn = fRFBackgroundParameters->GetBGElectronSpectrumFile();
  } else if (m_particleType == "gamma") {
    m_n = fRFBackgroundParameters->GetBGNumberOfPhotons();
    m_model = fRFBackgroundParameters->GetBGPhotonModel();
    m_energyMode = fRFBackgroundParameters->GetBGPhotonSpectrum();
    m_E = fRFBackgroundParameters->GetBGPhotonEnergy();
    m_EWidth = fRFBackgroundParameters->GetBGPhotonEnergyWidth();
    fileIn = fRFBackgroundParameters->GetBGPhotonSpectrumFile();
  }

  std::ifstream specFile(fileIn.c_str());
// //   m_fileIn.open(fileIn.c_str());
  G4cout << "file opened" << G4endl;
  if (!specFile.is_open()) {
//   if (!m_fileIn.is_open()) {
    G4cout << "Couldn't open background spectrum file " << fileIn << G4endl;
  } else {
    m_fileIn = &specFile;
    //m_fileIn = specFile;
  }
}

void MICEBGPlane::DefineBackgroundFromFile(){
  m_energyMode = "User"; // only supported option now
  G4String dummy;
  *m_fileIn >> dummy >> dummy;
  G4cout   << "  " << dummy << " " << dummy << G4endl;
  *m_fileIn >> dummy >> m_particleType;
  G4cout   << "  " << dummy << " " << m_particleType << G4endl;
  *m_fileIn >> dummy >> m_n;
  G4cout   << "  " << dummy << " " << m_n << G4endl;
  *m_fileIn >> dummy >> m_z;
  G4cout   << "  " << dummy << " " << m_z << G4endl;
  *m_fileIn >> dummy >> m_dir;
  G4cout   << "  " << dummy << " " << m_dir << G4endl;
  *m_fileIn >> dummy >> m_R;
  G4cout   << "  " << dummy << " " << m_R << G4endl;
  *m_fileIn >> dummy >> m_timeWindow;
  G4cout   << "  " << dummy << " " << m_timeWindow << G4endl;
  *m_fileIn >> dummy >> m_timeDelay;
  G4cout   << "  " << dummy << " " << m_timeDelay << G4endl;
  *m_fileIn >> dummy >> m_samplingMode;
  G4cout   << "  " << dummy << " " << m_samplingMode << G4endl;
  *m_fileIn >> dummy >> m_model;
  G4cout   << "  " << dummy << " " << m_model << G4endl;
  *m_fileIn >> dummy >> m_nE;
  G4cout   << "  " << dummy << " " << m_nE << G4endl;

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  this->SetParticleDefinition(particleTable->FindParticle(m_particleType));

  ReadHistogram();
}

void MICEBGPlane::DefineBackground(){

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  this->SetParticleDefinition(particleTable->FindParticle(m_particleType));

  if (m_energyMode == "Fixed") {
    G4cout << "Setting dark current energy to "
          << G4BestUnit(m_E, "Energy") << G4endl;
  } else if (m_energyMode == "Flat") {
    G4cout << "Setting Flat dark current energy distribution "
          << G4BestUnit(m_E-m_EWidth, "Energy") << " - "
          << G4BestUnit(m_E+m_EWidth, "Energy") << G4endl;
  } else if (m_energyMode == "User") {
    if (!m_fileIn->is_open()) {
      G4cout << "Couldn't open background spectrum file for " << m_particleType << G4endl
          << "Using Fixed energy instead" << G4endl;
      m_energyMode = "Fixed";
    } else {
      ReadHistogram();
    }
  } else {
    G4cout << "I don't understand the spectrum parameter = " << m_energyMode << G4endl
          << "Using Fixed " << G4BestUnit(m_E, "Energy")
          << " instead"     << G4endl;
    m_energyMode = "Fixed";
  }

  G4cout << "Defined background" << G4endl;
}

void MICEBGPlane::ReadHistogram()
{
  G4double thispE;
  std::pair<G4double, G4double> thisPair;
  std::vector<G4double> pEArray;
  for (int iE = 0; iE < m_nE; iE++) {
    *m_fileIn >> thisPair.first >> thisPair.second >> thispE;
    G4cout << "  " << thisPair.first << " " << thisPair.second << " " << thispE << G4endl;
    m_EArray.push_back(thisPair);
    pEArray.push_back(thispE);
  }

  G4cout << "Using histogram for dark current energy distribution" << G4endl;
  m_EWidth = m_EArray[m_nE-1].first - m_EArray[0].first; // check this!
  int arraySize = pEArray.size();
  double* pE;
  pE = new double[m_nE];
  for (int index = 0; index < arraySize; index++) {
    pE[index] = pEArray[index];
  }
  m_eSpectrum = new CLHEP::RandGeneral(pE, m_nE, m_samplingMode);
  delete[] pE;
}


void MICEBGPlane::GenerateBackground(G4Event* anEvent, MICEEvent* bgEvent)
{
  G4int nbrParticles;
  if (m_mode == "SimBank") { // use a bank of predefined particles
//     int numberOfHits;
    bool done = false;
    static G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    if( ! done )
    {
      for( unsigned int i = 0; i < bgEvent->specialHits.size(); ++i )
      {
        int stationNum = bgEvent->specialHits[i]->GetStationNo();
        if ((stationNum == 9001) | (stationNum == 9002)) {
          this->SetParticlePosition(bgEvent->specialHits[i]->GetPosition());
          // shift the time to allow for different starting point of muon
          this->SetParticleTime(bgEvent->specialHits[i]->GetTime()+m_timeDelay);
          this->SetParticleDefinition(particleTable
            ->FindParticle(bgEvent->specialHits[i]->GetPID()));
          this->SetParticleMomentumDirection(bgEvent->specialHits[i]->GetMomentum());;
          this->SetParticleEnergy(bgEvent->specialHits[i]->GetEnergy()
            - bgEvent->specialHits[i]->GetMass()); // E_kin

          // generate the particle and start tracking it
          this->GeneratePrimaryVertex(anEvent);
        }
      }
    }

//     while (active) {
//       // Get an input line
//       getline(*m_fileIn, tbuff);
//       if (m_fileIn->eof()) { // do something smart when the end is reached, loop?
//         G4cout << "I have reached end of background bank file!" << G4endl;
//         if (m_bankSamplingMode == "Linear"){
//           G4cout << "Giving up, no more background for you." << G4endl;
//           break;
//         } else if (m_bankSamplingMode == "Loop") {
//           G4cout << "Looping the background file, starting from beginning again." << G4endl;
//           m_fileIn->clear();
//           m_fileIn->seekg(0, std::ios::beg);
//         } else if (m_bankSamplingMode == "Random"){
//           G4cout << "To be implemented..." << G4endl;
//         } else {
//           G4cout << "I dont understand so I won't give you more background." << G4endl;
//           break;
//         }
//       }
//       std::stringstream tok;
//       tok << tbuff;
//       // Check first word to see if it is a token, and act accordingly
//       tok >> tbuff;
//       if(tbuff == "nspech") {
//         tok >> tbuff >> numberOfHits;
//         active = false; // tell it to stop reading
//       }
//     nbrParticles = (G4int)floor(m_scale*numberOfHits);
    // G4cout << "Number of particles = " << nbrParticles << G4endl;
    //static G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//     if(nbrParticles > 0) {
//       for (G4int k = 0; k < nbrParticles; k++) {
        // read in the specs for this particle
        //virtHB->ReadSim(m_fileIn);
        //ME - this should come from the MICEEvent through persistency from now on!
        // SpecialHit* aHit = new SpecialHit(*m_fileIn);

/*
        this->SetParticlePosition(aHit->GetPosition());

        // shift the time to allow for different starting point of muon
        this->SetParticleTime(aHit->GetTime()+m_timeDelay);
        this->SetParticleDefinition(particleTable
          ->FindParticle(aHit->GetPID()));
        this->SetParticleMomentumDirection(aHit->GetMomentum());;
        this->SetParticleEnergy(aHit->GetEnergy()-aHit->GetMass()); // E_kin

        // generate the particle and start tracking it
        this->GeneratePrimaryVertex(anEvent);
        delete aHit;
*/
//       }
//     }
  } else { // do not use the bank, but use some random model to generate particles
    nbrParticles = m_n;
    if (m_model == "Simple") nbrParticles = CLHEP::RandPoisson::shoot(m_n);
    if(nbrParticles > 0) {
      for (G4int k = 0; k < nbrParticles; k++) {
        G4double r = m_R * sqrt(G4UniformRand());
        G4double phi = twopi * G4UniformRand(); // check this distribution
        G4double x = r * cos(phi);
        G4double y = r * sin(phi);
        G4double z = m_z;
        G4double t = G4UniformRand()*m_timeWindow + m_timeDelay;
        G4double nz = m_dir;
        if (m_model == "Uniform" && m_dir == 0) nz = 2. * CLHEP::RandBit::shoot() - 1.;
        this->SetParticlePosition(G4ThreeVector(x, y, z));
        this->SetParticleMomentumDirection(G4ThreeVector(0, 0, nz));
        if (m_energyMode == "Flat") {
          this->SetParticleEnergy(m_E + m_EWidth * (2*G4UniformRand() - 1));
        } else if (m_energyMode == "User") {
          G4double rand = m_eSpectrum->shoot();
          if (m_samplingMode == 1){
            // take the time corresponding to energy (only works in sampling mode 1)
            t = m_EArray[int(m_nE*rand+0.5)].second
            + G4UniformRand()*m_timeWindow + m_timeDelay;
            this->SetParticleEnergy(m_EArray[int(m_nE*rand+0.5)].first);
          } else {
            this->SetParticleEnergy(m_EArray[0].first+m_EWidth*rand);
          }
        }
        this->SetParticleTime(t);
        this->GeneratePrimaryVertex(anEvent);
      }
    }
  }
}
