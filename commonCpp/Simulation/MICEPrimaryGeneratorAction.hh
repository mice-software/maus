// @(#) $Id: MICEPrimaryGeneratorAction.hh,v 1.36 2010-09-17 09:50:31 rogers Exp $  $Name:  $
// Primary generator for MICE
// Yagmur Torun
// Rewrite by Chris Rogers May 2010
//
// MPGA Constructor:
// Decide whether beam comes from datacards or micemods.
//   - If beam comes from datacards, I open a beam file if necessary, do some setup
//   - If beam comes from micemods, I find micemodules and setup the first beam
//        module, opening the first beam file if necessary. 
//
// MPGA::GeneratePrimaries:
//   - If beam comes from datacards, I parse the next line in the beam file and
//        convert to an event (DatacardEvent); or fire a random particle
//   - If beam comes from micemodules, I parse the next line in the beam file
//        and convert to an event; if there is a problem or the module has run out
//        of events, I move on to the next module. (MiceModuleEvent)
//   - If ForceNextEvent or ForceAllEvents is true, AND I have fired the initial reference particle
//        use SetNextEvent(...) to set event data instead until cavities phased
//


#ifndef MICEPrimaryGeneratorAction_h
#define MICEPrimaryGeneratorAction_h 1

#include "Interface/MICEEvent.hh"

#include <fstream>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"
#include "Randomize.hh"
#include "MICEBGPlane.hh"
#include "Config/MiceModule.hh"
#include "Config/ModuleConverter.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Random/RandEngine.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"

class G4ParticleGun;
class MICEEvent;

class BeamParameters;
class RFBackgroundParameters;

//////////// HACK //////////////
// This breaks the normal dependency tree and relies on Simulation.cc to handle methods
// That should really be handled inside MPGA. Needs fixing.
// I have left a gross memory leak in because I can't(!) delete MICEEvent* as the
// destructor doesn't clean up! What a horrible mess. Not a fan of persistency!
extern std::vector<MICEEvent*> ReadLoop( std::string filename, std::vector<std::string> classesToProcess );
////////////////////////////////


class MICEPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:

  MICEPrimaryGeneratorAction( MICEEvent* rfBGEvent, MICEEvent* simEvent=NULL, MiceModule* rootModule=NULL);

  ~MICEPrimaryGeneratorAction();

  //Set the modules to generate events using MiceModules
  void SetModules       (const MiceModule* root);
  //Called by GEANT4 - determine what the next particle is and fill the G4Event
  void GeneratePrimaries(G4Event* anEvent);

  //Force the next event with given time, position, energy, momentum; 
  //if pid == 0, it will default to the last pid fired; energy is total energy
  //uses momentum to set direction, but forces magnitude to satisft p^2+m^2=E^2
  void SetNextEvent(double time, G4ThreeVector position, double energy, G4ThreeVector momentum, int pid=0);
  //Set to true to use data from SetNextEvent for all following events, ignoring datacards and micemodules
  void ForceAllEvents(bool isForced) {forceAllEvents = isForced;}
  //Set tot true to use data from SetNextEvent for next event, ignoring datacards and micemodules
  void ForceNextEvent(bool isForced) {forceNextEvent = isForced;}
  //Set to false to force g4mice to ignore random dice on next event (use average instead)
  void NextEventHasRandom(bool hasRandom) {nextEventHasRandom = hasRandom;}

  //RF Background stuff (deprecated and probably does not work)
  G4bool GetRFBackgroundOn(){return fRF_BACKGROUND_ON;};
  void   SetRFBackgroundOn(G4bool rfb){fRF_BACKGROUND_ON=rfb;};

private:

  //RF background ///////////////////
  bool hasRF;
  G4bool fRF_BACKGROUND_ON;
  std::vector<MICEBGPlane*> bgGuns;
  G4ParticleGun* rfElectronGun;
  G4ParticleGun* rfPhotonGun;
  BeamParameters* fBP;
  RFBackgroundParameters* fRFBackgroundParameters;
  MICEEvent*  _bgEvent;
  std::ifstream m_bankFile;
  void DefineBackground();
  ///////////////////////////////////

  //The point of using these enumerations is that, so long as I set up the mapping right to start with, everything
  //is checked at compile time
	enum               beamType{unknown, pencil, g4mice_primarygenhit, icool, g4beamline, hepevt, gaussian, turtle, micemodule, none};
	enum               longVar {E, P, Pz, Ek};
	static std::string stringType[];
	static int         nStringTypes;

  ////////// Particle data for next hit; by default reads from datacards/micemodules 
  // can be forced by the user with SetNextEvent(...) or ForceAllEvents(...)
  bool          forceNextEvent;
  bool          forceAllEvents;
  G4ThreeVector genMomDir;
  G4ThreeVector genPos;
  G4ThreeVector genSpin;
  double        genTime;
  double        genKineticEnergy;
  double        genWeight;
  int           genPid;
  int           genEvt;
  /////////////////////////////////////////////////////////////////

  /////////// General data  
  beamType                beamTp;
  double                  randomSeed;
  bool                    nextEventHasRandom; //set to false to ignore RMS etc
  MICEEvent*              simEvent;
	std::ifstream           fin;
  G4ParticleGun*          gun;
  std::vector<MICEEvent*> g4mice_events;

  //Reference particle is special - if in reference particle mode, shoot
  void               FireNextEvent(G4Event* anEvent);
  //Setup
  void               SetMemberVariables();
  //Put PrimaryVertex data into mice event
  void               G4PrimaryVertexToPrimaryGenHit(const G4Event& anEvent, MICEEvent* event);
  //Return true if type needs a file
	bool               UsesTextFile  (beamType type) {return type==icool || type==g4beamline || type==hepevt || type==turtle;}
	bool               UsesG4MiceFile(beamType type) {return type==g4mice_primarygenhit;}
  //Convert from string to enumeration and vice versa
	beamType           GetType(std::string textRepresentation);
	std::string        GetType(beamType type);
  //Open file, strips header (if there is any)
  void               SetupFile        (std::string name, beamType type);
  //Environment variable substitution BUG - should be defined in Interface somewhere
  std::string        ReplaceVariables( std::string in);
  //G4MICE files are special
  void               SetupG4MiceFile  (std::string name);
  void               ClearG4MiceEvents();
  //Get event from file (formatting type dependent)
  void               GenerateFileEvent(beamType type, G4Event* anEvent, CLHEP::Hep3Vector position, CLHEP::HepRotation rotation);
  //IO_Object must have methods ParticleId(), Momentum(), Position(), Time(), Spin() and operator >>
  //I use a template, should really do this with inheritance model...
  template <class IO_Object> 
  void               GenerateFileEvent(G4Event* anEvent, CLHEP::Hep3Vector position, CLHEP::HepRotation rotation);
  //Generate particles travelling on axis
  void               GeneratePencilEvent(G4Event* anEvent, CLHEP::HepVector means, CLHEP::Hep3Vector position, int pid, 
                                         longVar momentumVariable, CLHEP::HepRotation rotation);
  //Generate particles with some distribution
  //Set isReference to true to generate a pencil beam
	void               GenerateRandomEvent(G4Event* anEvent, CLHEP::HepVector means, CLHEP::Hep3Vector position, int pid,
                         CLHEP::HepSymMatrix covariances, longVar momentumVariable, CLHEP::HepRotation rotation, bool isReference);
  //Extract from G4MICE PrimaryGenHit; basically inverse of PrimaryVertexToPrimaryGenHit
  void               GenerateG4MicePrimaryGenEvent(G4Event* anEvent, CLHEP::Hep3Vector position, CLHEP::HepRotation rotation);

  //// Datacards beams specific data and functions //////
  int            pid; //also used by turtle beam
  double         zStart;
  double         momentumStart;
  CLHEP::HepSymMatrix rand; //holds RMS data for beam
  void           DatacardsEvent     (G4Event* anEvent, bool isReference);
  ////////////////////////////////////////////////////


  ////// MiceModules function calls and data /////////////////
	void        ModuleEvent       (G4Event* anEvent, bool isReference=false);
	void        NextModule();
	void        SetupCurrentModule();
  void        findModulesByPropertyExistsTree();
	void        GenerateRandomEvent    (const MiceModule* module, G4Event* anEvent, bool isReference);
	void        GeneratePencilEvent    (const MiceModule* module, G4Event* anEvent);

	int                                      numberOfEventsFromThisModule;
	std::vector<const MiceModule*>           modules;
	std::vector<const MiceModule*>::iterator currentModule;
  /////////////////////////////////////////



	CLHEP::HepRandomEngine*                  anEngine;
};

#endif


