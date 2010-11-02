// @(#) $Id: MICEPrimaryGeneratorAction.cc,v 1.89 2010-10-22 08:51:33 rogers Exp $  $Name:  $
// Primary generator for MICE
// Yagmur Torun
// Rewrite by Chris Rogers May 2010

#include "Config/MiceModule.hh"
#include "MICEPrimaryGeneratorAction.hh"
#include "Interface/MICEEvent.hh"
#include "Config/BeamParameters.hh"
#include "Config/RFBackgroundParameters.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4ParticleTable.hh"
#include "G4UnitsTable.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "Randomize.hh"
#include "CLHEP/Random/Randomize.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "Interface/dataCards.hh"
#include "G4HEPEvtInterface.hh"
#include "BeamTools/BTPhaser.hh"
#include "Interface/Squeak.hh"
#include "Interface/For003Bank.hh"
#include "Interface/TurtleBank.hh"
#include "Interface/G4BLBank.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "Interface/Squeal.hh"
#include "Config/ModuleConverter.hh"

#include <iostream>
#include <fstream>
 

std::string MICEPrimaryGeneratorAction::stringType[10] = 
  {"Unknown", "Pencil", "G4MICE_PrimaryGenHit", "ICOOL", "G4BeamLine", "HEPEvt", "Gaussian", "TURTLE", "MiceModule", "None"};
int         MICEPrimaryGeneratorAction::nStringTypes  = 10;


MICEPrimaryGeneratorAction::MICEPrimaryGeneratorAction( MICEEvent* rfBGEvent, MICEEvent* _simEvent, MiceModule* root ) : fRF_BACKGROUND_ON(false), fBP(BeamParameters::getInstance()), _bgEvent( rfBGEvent ), simEvent(_simEvent),
                                                                         anEngine(new CLHEP::Hurd160Engine(fBP->RandomSeed()) )
{
  if (false) DefineBackground();
  genPid     = MyDataCards.fetchValueInt           ("BeamPid");
  double mass = ModuleConverter::PdgPidToMass(genPid);
  double mom  = MyDataCards.fetchValueDouble     ("BeamMomentum");

  forceNextEvent   = false;
  genKineticEnergy = sqrt(mass*mass+mom*mom)-mass;
  genSpin    = G4ThreeVector(0,0,0);
  genMomDir  = G4ThreeVector(0,0,1);
  genPos     = G4ThreeVector(0,0,MyDataCards.fetchValueDouble("ZOffsetStart"));
  genTime    = 0.;
  genWeight  = 1;
  genEvt     = 0;

  nextEventHasRandom = false;
  beamTp      = GetType(MyDataCards.fetchValueString("BeamType"));
  randomSeed  = MyDataCards.fetchValueInt           ("RandomSeed");
  zStart      = genPos[2];
  pid         = genPid;
  momentumStart = mom;
  gun         = new G4ParticleGun();
  rand        = CLHEP::HepSymMatrix(6,1);
  rand[0][0]  = MyDataCards.fetchValueDouble("SigmaX");
  rand[1][1]  = MyDataCards.fetchValueDouble("SigmaXPrime")*momentumStart;
  rand[2][2]  = MyDataCards.fetchValueDouble("SigmaY");
  rand[3][3]  = MyDataCards.fetchValueDouble("SigmaYPrime")*momentumStart;
  rand[4][4]  = MyDataCards.fetchValueDouble("SigmaT");
  rand[5][5]  = MyDataCards.fetchValueDouble("SigmaEOverE")*(genKineticEnergy+mass);
  /////////////////////////////////////////////////////////////////

  if(UsesTextFile(beamTp))
    SetupFile(MyDataCards.fetchValueString("InputBeamFileName"), beamTp);
  if(UsesG4MiceFile(beamTp))
    SetupG4MiceFile(MyDataCards.fetchValueString("InputBeamFileName"));
  if(root != NULL && beamTp == micemodule)
    SetModules(root);
}

MICEPrimaryGeneratorAction::~MICEPrimaryGeneratorAction()
{
  int size = bgGuns.size();
  for (int i = 0; i < size; i++) delete bgGuns[i];
  bgGuns.clear();
  delete gun;
  ClearG4MiceEvents();
}

MICEPrimaryGeneratorAction::beamType MICEPrimaryGeneratorAction::GetType(std::string textRepresentation)
{
	for(int i=0; i<nStringTypes; i++)
		if(stringType[i] == textRepresentation) return beamType(i);
  return unknown;
}

std::string MICEPrimaryGeneratorAction::GetType(beamType type)
{
	return stringType[type];
}
 

void MICEPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  try
  {
    if(!forceNextEvent && !forceAllEvents) //if not forced by user, get event from datacards/mice modules
      switch(beamTp)
      {
        case none:
          return;
        case micemodule:
          if(currentModule<modules.end()) ModuleEvent(anEvent, !nextEventHasRandom);
          else Squeak::mout(Squeak::warning) << "Run out of MiceModule events" << std::endl;
          break;
        default:
          DatacardsEvent(anEvent, !nextEventHasRandom);
      }
    forceNextEvent     = false;
    FireNextEvent(anEvent);
    nextEventHasRandom = true;
    if(simEvent != NULL) G4PrimaryVertexToPrimaryGenHit(*anEvent, simEvent);
  }
  catch(Squeal squee) {throw(squee);}
	catch(std::exception exc)
	{
    if( beamTp == micemodule)
		  throw( Squeal( Squeal::recoverable, "Error <"+std::string(exc.what())+"> in beam file - generating beam in module "+(*currentModule)->name(), 
                     "MICEPrimaryGeneratorAction::ModuleEvent" ) ); 
    else
		  throw( Squeal( Squeal::recoverable, "Error <"+std::string(exc.what())+"> in beam file", "MICEPrimaryGeneratorAction::ModuleEvent" ) ); 
	}
}

void MICEPrimaryGeneratorAction::G4PrimaryVertexToPrimaryGenHit(const G4Event& anEvent, MICEEvent* event)
{

  for(int i=0; i<anEvent.GetNumberOfPrimaryVertex(); i++)
  {
    const G4PrimaryVertex& pv = *(anEvent.GetPrimaryVertex(i));
    for(int j=0; j<pv.GetNumberOfParticle(); j++)
    {
      PrimaryGenHit& hit = *(new PrimaryGenHit());
      
      hit.setPosition( pv.GetPosition() );
      hit.setTime    ( pv.GetT0() );

      const G4PrimaryParticle& pp = *pv.GetPrimary(j);
      hit.setTrackID ( pp.GetTrackID() );
      hit.setPdg     ( pp.GetPDGcode() );
      hit.setCharge  ( pp.GetCharge() );
      hit.setMass    ( pp.GetMass() );
      hit.setMomentum( pp.GetMomentum() );
      hit.setEdep    ( 0. );
      hit.setEnergy  ( sqrt(pp.GetMass()*pp.GetMass()+pp.GetMomentum().mag2()) );
      hit.setParticle( NULL ); 
      hit.setSpin    ( pp.GetPolarization() );
      hit.setSeed       (randomSeed);
      hit.setWeight     (pp.GetWeight());
      hit.setEventNumber(anEvent.GetEventID());

      event->pgHits.push_back(&hit);
    }
  }
}

void MICEPrimaryGeneratorAction::SetModules(const MiceModule* root)
{
	modules       = root->findModulesByPropertyExists("string","BeamType" );
	currentModule = modules.begin();
  SetupCurrentModule();
}

void MICEPrimaryGeneratorAction::NextModule()
{
	currentModule++;
	SetupCurrentModule();
}

void MICEPrimaryGeneratorAction::SetupCurrentModule()
{
	if(currentModule >= modules.end())
    throw(Squeal(Squeal::recoverable, "Run out of beam commands", "MICEPrimaryGeneratorAction::SetupCurrentModule()") );
  beamType type = GetType((*currentModule)->propertyString("BeamType"));
  if(UsesTextFile( type ) )
  {
    std::string filename = ReplaceVariables((*currentModule)->propertyString("BeamFile"));
    SetupFile(filename, type);
  }
  if(UsesG4MiceFile(type))
  {
    std::string filename = ReplaceVariables((*currentModule)->propertyString("BeamFile"));
    SetupG4MiceFile(filename);
  }
	numberOfEventsFromThisModule = 0;
}

void MICEPrimaryGeneratorAction::ModuleEvent(G4Event* anEvent, bool isReference)
{
	try
	{
		int maxNumberOfEvents = -1;
		if( (*currentModule)->propertyExistsThis( "NumberOfEvents", "int") )
			maxNumberOfEvents = (*currentModule)->propertyInt("NumberOfEvents");
		while(numberOfEventsFromThisModule>=maxNumberOfEvents && maxNumberOfEvents > 0)
		{
			NextModule();
			maxNumberOfEvents = (*currentModule)->propertyInt("NumberOfEvents");
		}
		HepRotation rotation = (*currentModule)->globalRotation().inverse();
		Hep3Vector  position = (*currentModule)->globalPosition();
		beamType    type     = GetType( (*currentModule)->propertyString("BeamType") );
		switch(type)
		{
			case icool: case g4beamline: case turtle: case hepevt:
				GenerateFileEvent(type, anEvent, position, rotation);
				break;
			case gaussian:
				GenerateRandomEvent(*currentModule, anEvent, isReference);
				break;
			case pencil:
				GeneratePencilEvent(*currentModule, anEvent);
				break;
      case g4mice_primarygenhit:
        GenerateG4MicePrimaryGenEvent(anEvent, position, rotation);
        break;
			default:
				throw(Squeal(Squeal::recoverable, "Failed to recognise beam type "+(*currentModule)->propertyString("BeamType"),
				                                  "MICEPrimaryGeneratorAction::ModuleEvent(G4Event*)"));
		}
		numberOfEventsFromThisModule++;
	}
	catch(Squeal squee) {throw squee;} //squeal is a type of std::exception, so need to catch it first
}

void MICEPrimaryGeneratorAction::DatacardsEvent(G4Event* anEvent, bool isReference)
{
  CLHEP::Hep3Vector pos = CLHEP::Hep3Vector(0,0,zStart);
  CLHEP::HepVector means(6,0);
  means[5] = momentumStart;
  switch(beamTp)
  {
			case icool: case g4beamline: case turtle: case hepevt:
				GenerateFileEvent(beamTp, anEvent, pos, CLHEP::HepRotation());
				break;
			case pencil:
        //if isReference is true, generates pencil beam
				GenerateRandomEvent(anEvent, means, pos, pid, CLHEP::HepSymMatrix(6,1), P,  CLHEP::HepRotation(), true);
				break;
			case gaussian:
				GenerateRandomEvent(anEvent, means, pos, pid, rand, P,  CLHEP::HepRotation(), isReference);
				break;
      case g4mice_primarygenhit:
        GenerateG4MicePrimaryGenEvent(anEvent, CLHEP::Hep3Vector(), CLHEP::HepRotation());
        break;
      default:
        throw(Squeal(Squeal::recoverable, "BeamType not implemented", "DatacardsEvent"));
  }
}

void MICEPrimaryGeneratorAction::GenerateFileEvent(beamType type, G4Event* anEvent, CLHEP::Hep3Vector position, CLHEP::HepRotation rotation)
{
  switch(type)
  {
    case icool:
      GenerateFileEvent<For003Bank>(anEvent, position, rotation);
      break;
    case g4beamline:
      GenerateFileEvent<G4BLBank>(anEvent, position, rotation);
      break;
    case turtle:
      GenerateFileEvent<TurtleBank>(anEvent, position, rotation);
      break;
    default:
      throw(Squeal(Squeal::recoverable, "Not a beam file type that I recognise", "MICEPrimaryGeneratorAction::GenerateFileEvent"));
  }
}

void MICEPrimaryGeneratorAction::SetupFile(std::string filename, beamType type)
{
	if(fin.is_open()) fin.close();
	fin.clear();
	fin.open( filename.c_str() );
  if(!fin)
    throw(Squeal(Squeal::recoverable, "Failed to open beam file "+filename, "MICEPrimaryGeneratorAction::SetupFile"));
  switch(type)
  {
    case icool:
      For003Bank::ReadHeader(fin);
      break;
    case g4beamline:
      G4BLBank::ReadHeader(fin);
      break;
    case hepevt: //no header
      break;
    case turtle: //no header
      break;
    default:
      throw(Squeal(Squeal::recoverable, "Trying to setup beam file, but beamType doesn't use files", "MICEPrimaryGeneratorAction::SetupFile"));
  }

	if(fin)
		fin.exceptions ( std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit );
	else throw(Squeal(Squeal::recoverable, "Failed to open beam file "+(*currentModule)->propertyString("BeamFile"), 
	                                       "MICEPrimaryGeneratorAction::SetupCurrentModule()" ) );
}

void MICEPrimaryGeneratorAction::SetupG4MiceFile(std::string name)
{
  ClearG4MiceEvents();
  std::vector<MICEEvent*> events_forward = ReadLoop(name, std::vector<std::string>());
  for(std::vector<MICEEvent*>::reverse_iterator rit = events_forward.rbegin(); rit!=events_forward.rend(); rit++)
    g4mice_events.push_back(*rit); //reverse the order
}

void MICEPrimaryGeneratorAction::ClearG4MiceEvents()
{
  ////////// BUG!!!!!!!!! ////////// 
  /// GROSS MEMORY LEAK AAARR //////
  for(unsigned int i=0; i<g4mice_events.size(); i++) delete g4mice_events[i]; //does not delete member data
  g4mice_events = std::vector<MICEEvent*>();
}


template <class IO_Object> void MICEPrimaryGeneratorAction::GenerateFileEvent(G4Event* anEvent, Hep3Vector position, HepRotation rotation)
{
	IO_Object event_IO;
	std::string line; //more robust - if there is an error in a particular line, will keep going
	getline(fin, line);
  if(fin.eof())
    Squeak::mout(Squeak::warning) << "Ran out of events in beam file" << std::endl;
	std::stringstream ss_in(line);
	ss_in >> event_IO;
  if( (!fin && !fin.eof() ) || !ss_in) 
		throw(Squeal(Squeal::recoverable, "Failed to parse beam file. Line was\n"+line, "MICEPrimaryGeneratorAction::GenerateFileEvent"));
  int f_pid = event_IO.ParticleId();
  if(f_pid == 0) f_pid = pid; //default to datacards value if pid not defined
	G4ParticleDefinition* def  = G4ParticleTable::GetParticleTable()->FindParticle(f_pid);
	double                mass = def->GetPDGMass();
  genPid           = f_pid;
	genMomDir        = rotation*event_IO.Momentum();
  genKineticEnergy = sqrt(mass*mass+event_IO.Momentum().mag2())-mass;
  genPos           = rotation*event_IO.Position()+position;
	genTime          = event_IO.Time();
	genSpin          = rotation*event_IO.Spin();
  genEvt           = event_IO.EventNumber();
  genWeight        = event_IO.Weight();
}

void MICEPrimaryGeneratorAction::GeneratePencilEvent(const MiceModule* module, G4Event* anEvent)
{
  MCHit               hit  = ModuleConverter::ModuleToHit(module);
  CLHEP::HepVector    means(6,0);
  means[0] = 0.; means[1] = 0.; 
  means[2] = 0.; means[3] = 0.; 
  means[4] = hit.time();        means[5] = hit.energy();
	GenerateRandomEvent(anEvent, means, module->globalPosition(), hit.pdg(), CLHEP::HepSymMatrix(6,1), E, module->globalRotation(), true);
}

void MICEPrimaryGeneratorAction::GenerateRandomEvent(const MiceModule* module, G4Event* anEvent, bool isReference)
{
  MCHit               hit  = ModuleConverter::ModuleToHit(module);
  //Need to change order from t,E,x,px,y,py to x,px,y,py,t,E
  CLHEP::HepSymMatrix cov1 = ModuleConverter::ModuleToBeamEllipse(module, hit); 
  CLHEP::HepSymMatrix cov2 = cov1;
  for(int i=0; i<4; i++)
    for(int j=i; j<4; j++) cov1[i][j] = cov2[i+2][j+2];
  for(int i=0; i<2; i++)
    for(int j=i; j<2; j++) cov1[i+4][j+4]     = cov2[i][j];
  for(int i=0; i<4; i++)
    for(int j=4; j<6; j++) cov1[i][j] = cov2[i+2][j-4];

  CLHEP::HepVector    means(6,0);
  means[0] = 0.; means[1] = 0.;
  means[2] = 0.; means[3] = 0.; 
  means[4] = hit.time();        means[5] = hit.energy();
	GenerateRandomEvent(anEvent, means, module->globalPosition(), hit.pdg(), cov1, E, module->globalRotation(), isReference);
}

void MICEPrimaryGeneratorAction::GenerateRandomEvent(G4Event* anEvent, CLHEP::HepVector means, CLHEP::Hep3Vector position, int pid,
                         CLHEP::HepSymMatrix covariances, longVar momentumVariable, CLHEP::HepRotation rotation, bool isReference)
{
  if(covariances.determinant() <= 0.)
    throw(Squeal(Squeal::recoverable, "Badly conditioned Random event matrix - beam has negative or 0 emittance",
                 "MICEPrimaryGeneratorAction::GenerateRandomEvent"));
	CLHEP::RandMultiGauss * multiGauss = new CLHEP::RandMultiGauss( anEngine, means, covariances );
	CLHEP::HepVector        kin(means); //BUG rotation may be applied twice! CHECK
  if(!isReference)        kin        = multiGauss->fire( );
	double                  mass       = ModuleConverter::PdgPidToMass(pid);
	double                  energy     = 0;
	CLHEP::Hep3Vector       momentum;
	switch(momentumVariable)
	{
		case E:
		{
			energy = kin[5];
			double pz = sqrt(kin[5]*kin[5]-mass*mass-kin[3]*kin[3]-kin[1]*kin[1]);
			if(pz!=pz) pz = 0;
			momentum = Hep3Vector(kin[1], kin[3], pz);
			break;
		}
		case P:
		{
			energy = sqrt(kin[5]*kin[5] + mass*mass);
			double pz = sqrt(kin[5]*kin[5]-kin[3]*kin[3]-kin[1]*kin[1]);
			if(pz!=pz) pz = 0;
			momentum = Hep3Vector(kin[1], kin[3], pz);
			break;
		}
		case Pz:
		{
			energy = sqrt(kin[1]*kin[1]+kin[3]*kin[3]+kin[5]*kin[5]+mass*mass);
			momentum = Hep3Vector(kin[1], kin[3], kin[5]);
			break;
		}
		case Ek:
		{
			energy = kin[5]+mass;
			double pz = sqrt( (kin[5]+mass)*(kin[5]+mass)-mass*mass-kin[3]*kin[3]-kin[1]*kin[1] );
			if(pz!=pz) pz = 0;
			momentum = Hep3Vector(kin[1], kin[3], pz);
			break;
		}
	}

  if(pid!=0) 
    genPid         = pid;
	genMomDir        = rotation*momentum;
  genPos           = rotation*Hep3Vector(kin[0], kin[2], 0) +position;
	genTime          = kin[4];
	genKineticEnergy = energy-mass;
  if(genKineticEnergy < 0.) 
    GenerateRandomEvent(anEvent, means, position, pid, covariances, momentumVariable, rotation, isReference);
	genSpin          = G4ThreeVector(0,0,0);
  genEvt          += 1;
  genWeight        = 1.;
}

void MICEPrimaryGeneratorAction::GenerateG4MicePrimaryGenEvent(G4Event* anEvent, CLHEP::Hep3Vector position, CLHEP::HepRotation rotation)
{
  //BUG - if multiple pghits in one event, it will turn into multiple events
  //May fail depending on how (if) spill structure is implemented
  PrimaryGenHit* hit;
  if(g4mice_events.size() == 0) 
    throw( Squeal(Squeal::recoverable, "Didn't find any g4mice events at all", "MICEPrimaryGeneratorAction::GenerateG4MicePrimaryGenEvent") );
  while(g4mice_events.back()->pgHits.size() == 0 ) 
  {
    delete g4mice_events.back();
    g4mice_events.pop_back();
    if(g4mice_events.size() == 0) 
      throw( Squeal(Squeal::recoverable, "No more g4mice events", "MICEPrimaryGeneratorAction::GenerateG4MicePrimaryGenEvent") );
  }    
  hit = g4mice_events.back()->pgHits.back();

	double mass      = ModuleConverter::PdgPidToMass(hit->pdg());
  genPid           = hit->pdg();
	genMomDir        = rotation*hit->momentum();
  genPos           = rotation*hit->position() +position;
	genTime          = hit->time();
	genKineticEnergy = sqrt(mass*mass+hit->momentum().mag2())-mass;
	genSpin          = rotation*hit->spin();
  genEvt           = hit->eventNumber();
  genWeight        = hit->weight();
  delete hit;
  g4mice_events.back()->pgHits.pop_back();



}

void MICEPrimaryGeneratorAction::DefineBackground()
{
  G4String source = fRFBackgroundParameters->GetBGSource();

  // this might need to be set by another parameter

  G4String bgMode = fRFBackgroundParameters->GetBGMode();
  bool useBank        = (bgMode == "SimBank");
  bool useMultiPlane  = (bgMode == "MultiPlane");
  bool useSinglePlane = (bgMode == "SinglePlane");
  fRF_BACKGROUND_ON = ((useSinglePlane) | (useMultiPlane) | (useBank));

  if (fRF_BACKGROUND_ON) {
    // First select what input to use
    // If more than one is "turned on", the priority is
    // 1. bank
    // 2. multiplane
    // 3. datacards
    if (useBank) { // use predefined background from file
      G4String bgFile = fRFBackgroundParameters->GetBGBankFile();
      G4cout << "RF background specified by bank file " << bgFile << G4endl;
        m_bankFile.open(bgFile.c_str());
      if (!m_bankFile.is_open()) { // could not open the file
        G4cout << "Couldn't open BG bank file " << bgFile << G4endl;
      } else {
        MICEBGPlane* aBGPlane = new MICEBGPlane(&m_bankFile, 0);
        G4cout << "Defined BG plane from bank " << G4endl;
        bgGuns.push_back(aBGPlane);
      }
    } else if (useMultiPlane){ // use multiple bg planes defined in file
      G4String bgFile = fRFBackgroundParameters->GetBGMultipleSpectraFile();
      G4cout << "RF background specified by multiple spectra file " << bgFile << G4endl;
      std::ifstream bgSpecFile(bgFile.c_str());
      if (!bgSpecFile.is_open()) {
        G4cout << "Couldn't open multiple BG spectra file " << bgFile << G4endl;
      } else {
        int numberOfPlanes;
        bgSpecFile.ignore(256,' '); // ignore until it finds a space, then read in value
        bgSpecFile >> numberOfPlanes;
        for (int i = 0; i < numberOfPlanes; i++){
          MICEBGPlane* aBGPlane = new MICEBGPlane(&bgSpecFile);
          G4cout << "Defined BG plane " << i << G4endl;
          bgGuns.push_back(aBGPlane);
        }
        bgSpecFile.close();
      }
    } else if (useSinglePlane) { // use defaults or datacards to set up one bg plane
      G4cout << "RF background specified by datacard parameters" << G4endl;
      G4String source = fRFBackgroundParameters->GetBGSource();
      if (source == "e" || source == "e+gamma") {
        MICEBGPlane* aBGPlane = new MICEBGPlane("e-");
        bgGuns.push_back(aBGPlane);
      }
      if (source == "gamma" || source == "e+gamma") {
        MICEBGPlane* aBGPlane = new MICEBGPlane("gamma");
        bgGuns.push_back(aBGPlane);
      }
    } else {
      G4cout << "I don't understand what kind of background to simulate" << G4endl;
    }
  }
}

void MICEPrimaryGeneratorAction::FireNextEvent(G4Event* anEvent)
{
  if(genKineticEnergy<0.) 
    throw(Squeal(Squeal::recoverable, "Error - particle with negative kinetic energy", "MICEPrimaryGeneratorAction::FireNextEvent"));
  if(genPid == 0)
    throw(Squeal(Squeal::recoverable, "Error - particle with pid 0", "MICEPrimaryGeneratorAction::FireNextEvent"));
  G4ParticleTable*      particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle      = particleTable->FindParticle(genPid);
  gun->SetParticleDefinition(particle);
//  Squeak::mout() << "firing " << genPid << std::endl;
  Squeak::mout(Squeak::debug) << "Firing "+particle->GetParticleName()+" at t: " << genTime << " pos: " << genPos 
            << " momentum direction: " << genMomDir << " kinetic energy: " << genKineticEnergy << "\n";
  gun->SetParticlePosition(genPos);
  gun->SetParticleEnergy(genKineticEnergy);
  gun->SetParticleMomentumDirection(genMomDir);
  gun->SetParticleTime(genTime);
  gun->GeneratePrimaryVertex(anEvent);
}

void MICEPrimaryGeneratorAction::SetNextEvent(double time, G4ThreeVector position, double energy, G4ThreeVector momentum, int pid)
{
  forceNextEvent = true;
  double p    = momentum.r();
  double mass = sqrt(energy*energy - p*p);
  if(pid!=0) genPid  = pid;
  genTime = time;
  genPos  = position;
  genMomDir  = momentum/p;
  genKineticEnergy = energy - mass;
}

std::string	MICEPrimaryGeneratorAction::ReplaceVariables( std::string fileName )
{
  unsigned int pos = 0;
  std::string fullName;

  while( pos < fileName.size() )
  {
    if( fileName[pos] == '$' ) 	// we've found an environment variable 
    {
      ++pos;
      if( fileName[pos] == '{' )
	++pos;
      int end = pos +1;
      while( fileName[end] != '}' )
        ++end;
      std::string variable;
      for( int vpos = pos; vpos < end; ++vpos )
        variable += fileName[vpos];
      if(getenv( variable.c_str() ) == NULL) 
          throw(Squeal(Squeal::recoverable, "Error - "+variable+" environment variable was not defined", "MICEPrimaryGeneratorAction::ReplaceVariables"));
      fullName += std::string( getenv( variable.c_str() ) );
      pos = end + 1;
    }
    else
    {
      fullName += fileName[pos];
      ++pos;
    }
  }

  return fullName;
}

