#ifndef MICEBGPlane_HH
#define MICEBGPlane_HH

#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include <vector>
#include <map>
//#include <pair.h>

class G4Event;
class MICEEvent;

class MICEBGPlane : G4ParticleGun{
  public:
  
    /// Create a background plane given the source particle.
    /// Other parameters are given from dataCards.
    MICEBGPlane(G4String source);
    
    /// Create a background plane from an input file, 
    /// which can contain an arbitraty amount of such planes.
    /// Only in case of a problem will the dataCards parameters be used.
    MICEBGPlane(std::ifstream* fileIn);
    
    /// Create a background plane from an input file, 
    /// which in this case is a preexisting bank of background particles.
    /// A parameter used to scale the amount of background
    /// per event to desired level must also be set. 
    /// Only in case of a problem will the dataCards parameters be used.
    MICEBGPlane(std::ifstream* fileIn, G4int numberOfHits);
    
    /// Create a background plane given the parameters explicitly.
    MICEBGPlane(G4double R, G4double z, G4double dir, G4double timeWindow,
      G4double timeDelay, G4String particleType, G4int n, G4String model,
      G4String energyMode, G4double E, G4double EWidth, std::ifstream* fileIn);
      
    /// Destructor  
    ~MICEBGPlane();

  public:    
    /// Generate the background.
    void GenerateBackground(G4Event* anEvent, MICEEvent* bgEvent);

  private:
    /// Set the parameters according to dataCards.
    void SetParameters();
    
    /// Define the background from an input file.
    /// Only in case of a problem will the dataCards parameters be used.
    void DefineBackgroundFromFile();
    
    /// Define background from predefined values.
    void DefineBackground();
    
    /// Read in an energy and time histogram
    void ReadHistogram();

  private:

  // general
  G4int m_nE;
  G4int m_samplingMode;
  G4double m_R;
  G4double m_z;
  G4double m_dir;
  G4double m_timeWindow;
  G4double m_timeDelay;
  G4String m_particleType;
  G4int m_n;
  G4String m_model;
  G4String m_energyMode;
  G4String m_mode;
  G4String m_bankSamplingMode;
  G4double m_E;
  G4double m_EWidth;
  std::ifstream* m_fileIn;
  G4double m_scale;
  
  CLHEP::RandGeneral* m_eSpectrum;
  
  std::vector< std::pair<G4double, G4double> > m_EArray;
};

#endif
