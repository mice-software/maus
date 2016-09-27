// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: dataCards.cc,v 1.307 2011-05-20 14:28:40 reid Exp $  $Name:  $
/*
** modified from Yagmur Torun's MICE simulation package
** Ed McKigney - Aug 21 2002
*/

#include <Python.h>

#include "Interface/dataCards.hh"
#include "Utils/Squeak.hh"
#include "Utils/Exception.hh"
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::ms;
using CLHEP::mm;
using CLHEP::ns;
using CLHEP::MeV;
using CLHEP::megahertz;
using CLHEP::keV;
using CLHEP::coulomb;
using CLHEP::rad;
using CLHEP::picosecond;
using CLHEP::cm;

#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
using std::cout;
using std::cin;
using std::endl;
using std::vector;

dataCards::dataCards()
{
}

std::string dataCards::getClient()
{
  return client;
}

void dataCards::initializeApplicationCodes() {
  appCode["Simulation"]      = 0;
  appCode["Digitization"]    = 1;
  appCode["Reconstruction"]  = 2;
  appCode["Analysis"]        = 3;
}

dataCards::dataCards(const std::string& app) : client(app)
{
  initializeApplicationCodes();
  std::map<std::string,int>::iterator i = appCode.find(client);
  if (i == appCode.end()) {
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Unrecognized client application for datacards", "dataCards(...)"));
  }
  fillCards(i->second);
}

dataCards::dataCards(int app)
{
  fillCards(app);
}


void dataCards::AddDataCard( std::string aName, std::string aDefaultValue ){
	cs[ aName ] = aDefaultValue;
}

void dataCards::AddDataCard( std::string aName, int aDefaultValue ){
	ci[ aName ] = aDefaultValue;
}

void dataCards::AddDataCard( std::string aName, double aDefaultValue ){
	cd[ aName ] = aDefaultValue;
}

void dataCards::AddDataCard( std::string aName, vector< double > aDefaultValue ){
	cv[ aName ] = aDefaultValue;
}

void dataCards::AddDataCard( std::string aName, const unsigned int aSize, double * aDefaultValue ){
	vector<double> dVector;
	dVector.assign( aDefaultValue, aDefaultValue + aSize );
  cv[ aName ] = dVector;
}


void dataCards::fillCards(int app)
{
  if (app == 0)
  {        // app = 0 for Simulation
    cs["OutputFile"] = "Sim.out.gz";
    cs["InputFile"] = "none";
  }
  else if (app == 1)
  { // app = 1 for Digitization
    cs["InputFile"] = "Sim.out.gz";
    cs["OutputFile"] = "Digits.out.gz";
  }
  else if (app == 2)
  { // app = 2 for Reconstruction
    cs["InputFile"] = "Digits.out.gz";
    cs["OutputFile"] = "Rec.out.gz";
  }
  else if (app == 3)
  { // app = 3 for Analysis
    cs["InputFile"] = "Rec.out.gz";
    cs["OutputFile"] = "Ana.out";
  }
  else
  {
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Unrecognized client application for datacards", "dataCards::fillCards"));
  }
  cs["FileTag"] = ""; //Can be used as a component of output filenames for various applications, being descriptive of the beam and analysis, e.g. 400MeVpions. Convenient for batch scripts.

    double QuadFringeParams[6] = {0.296471, 4.533219, -2.270982, 1.068627, -0.036391, 0.022261}; //THIS IS PUT UP HERE TO STOP IT INTERFERING WITH DOC SCRIPT

  //***Start Doc Script***


    // <head>Global run control</head>
    ci["numEvts"] = 100; // Number of events in the run
    ci["runNumber"] = 1; // Run number listed in header of some output files
    cs["Description"] = "G4MICE events"; // Description of the run listed in the header of some output files
    cs["DataType"] = "TextFile"; //The data type of the dataCards file, set by user and used in global reader, TextFileReader etc

    // <head>Modelling and Configuration</head>
    cs["GeometrySchema"] = "http://service-spi.web.cern.ch/service-spi/app/releases/GDML/GDML_3_0_0/schema/gdml.xsd";
    // GDML schema (built-in default will be used if not found)
    cs["ReadGeometryURL"]  = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometry";  // To be overridden
    cs["WriteGeometryURL"] = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse";  // ...so it can be different
    cs["GeometrySource"] = "MiceModelTextFile"; //Or "RunNumber", "CurrentGeometry", "GeometryID", "GDMLTextFile"
    cs["MiceModel"] = "MICEStage6/Stage6.dat"; // The MiceModel, as defined in $MICEFILES/Models/Configurations/<MiceModel>
    cs["SciFiCalib"] = ""; //SciFi detector calibration file, found in ${MICEFILES}/Calibration/<SciFiCalib>
    cs["TofCalib"] = "tofcalib"; //TOF detector calibration file, found in ${MICEFILES}/Calibration/<TofCalib>
    cs["SciFiCable"] = "";  //SciFi detector cabling file, found in ${MICEFILES}/Cabling/SciFi/<SciFiCable>
    cs["SciFiCableType"] = ""; //Set to "Imperial" or "Osaka" for different cabling types
    cs["TofCable"] = "tofcabling.txt";  //Tof detector cabling file, found in ${MICEFILES}/Cabling/<TofCable>
    ci["FieldGridX"]             = 2; //Number of points in the field grid in x. As an optimisation, G4MICE divides the world into a grid. At each step, G4MICE only looks at fields in the local grid. The number of points in each dimension should roughly match the size of the problem in each dimension for a reasonable option, and FieldGridX*FieldGridY*FieldGridZ should be small, say<1e6.
    ci["FieldGridY"]             = 2; //Number of points in the field grid in y
    ci["FieldGridZ"]             = 100; //Number of points in the field grid in z

    // <head>Simulation Application run control</head>
    cd["MaxRadiusInChannel"] = 1.e9 * mm; // Events with r = sqrt(x^2 + y^2) > MaxRadius in global coordinates are killed
    cd["MaxZInChannel"] = 1.e9 * mm; // Events with z > MaxZ are killed in Simulation
    cd["MaxTimeAllowed"] = 1. * ms; // Events with t > MaxT are killed in Simulation
    ci["RandomSeed"] = 0; // Random seed for stochastic algorithms
    cs["PhysicsModel"] = "QGSP_BERT"; //Physics package loaded by G4MICE to set default values; modifications can be made as per following
    cs["MultipleScatteringModel"] = "mcs"; //"mcs" (multiple coulomb scattering) or "none"
    cs["EnergyLossModel"] = "estrag"; //"estrag" (full energy loss), "ionisation" (mean energy loss only) or "none"
    cs["HadronicModel"] = "All"; //"All" (All hadronic interactions) or "None" (no hadronic interactions)
    cs["MuonDecay"] = ""; //Deprecated; please use ParticleDecay datacard
    cs["ParticleDecay"] = "True"; //Set to "True" or "False" to determine whether muons decay
    cs["ReferenceEnergyLossModel"] = "ionisation"; //Energy loss model of reference particle; set to "none" or "ionisation". Reference particle can never have stochastic processes (e.g. estrag, mcs) enabled.
    cs["ReferenceG4UIScript"]    = ""; //If a filename is given, G4MICE will run this as a macro to G4UI after setting up ReferenceEnergyLossModel but before firing the reference particle. Enables any extra set-up that may be required.
    cs["BeginOfRunG4UIScript"]   = ""; //If a filename is given, G4MICE will run this as a macro to G4UI after firing the reference particle but before firing the beam. Enables any extra set-up that may be required.
    cs["EverythingSpecialVirtual"] = ""; //Set to "true" to make every physical volume a special virtual detector
    ci["WriteReferenceParticle"] = 0; // Set to 1 to write reference particle hits during simulation; or 0 to ignore them
    cd["ProductionThreshold"]    = 0.5 * mm; //I *think* threshold for delta ray production
    cd["MuonHalfTime"]           = -1 * ns;  //Set the muon half life; use g4 default if negative.
    cd["ChargedPionHalfTime"]    = -1 * ns;  //Set the pi+ and pi- half life; use g4 default if negative.
    cs["TrackOnlyMuons"]         = "False"; //Set to true to kill non-muons on production
    ci["MaxStepsWOELoss"]        = 2000;  //Maximum number of steps taken without energy loss - particles killed afterwards
    ci["StepStatisticsOutput"]   = 0; //I *think* set to 1 to see physics processes output
    cs["StepStatisticsVolume"]   = "All"; //I *think* selects which volumes to write physics processes output from
    ci["CheckVolumeOverlaps"]    = 0; //Set to 1 to check for volume overlaps in GEANT4 physical volumes

    // <head>Simulation Application beam control</head>
    cs["BeamType"] = "Pencil"; //Type of beam; "Pencil" generates a pencil beam using parameters below, "Gaussian" generates a beam with gaussian distribution using parameters below, "ICOOL", "G4BeamLine", "TURTLE",  "G4MICE_PrimaryGenHit" set G4MICE to read from a beam file with the appropriate format and file name defined in datacard "InputBeamFileName"; "MiceModule" uses the beam definition taken from MiceModules (see mice module docs), and allows arbitrary placement of beams with arbitrary multivariate gaussian distributions; in general, MiceModules are the preferred method for setting up beams. The reference particle is the first particle created. If BeamType is Random, the first particle will have no random distribution.
    cd["BeamMomentum"] = 200.0 * MeV; //When BeamType is Pencil or Random, controls momentum of the beam
    cd["ZOffsetStart"] = -6011. * mm; //Z-position of input beam when not in MiceModule mode. x and y are 0. Added to any z-position in beam file.
    ci["BeamPid"]      = -13;         //Particle ID of pencil beam and TURTLE beam (11 is e-, -13 is mu+, 211 is pi+, 2212 is proton)
    cs["InputBeamFileName"] = "none"; //Name of beam file for BeamTypes that use files, as specified in BeamType above
    cd["SigmaX"]       = 0.*mm;  //If beam type is "Gaussian", set RMS of x
    cd["SigmaXPrime"]  = 0.;     //If beam type is "Gaussian", set RMS of x' (px/p)
    cd["SigmaY"]       = 0.*mm;  //If beam type is "Gaussian", set RMS of y
    cd["SigmaYPrime"]  = 0.;     //If beam type is "Gaussian", set RMS of y' (py/p)
    cd["SigmaT"]       = 0.*ns;  //If beam type is "Gaussian", set RMS of time distribution
    cd["SigmaEOverE"]  = 0.;     //If beam type is "Gaussian", set RMS of (E-E_ref)/E_ref where E is total energy and E_ref is total energy corresponding to BeamMomentum

    // <head>Beam Matcher Application Control</head>
    cs["MatchAlgorithm"] = "EigenvalueSeed"; //The algorithm used for matching; "Eigenvalue" calculates the transfer map and then attempts to automatically derive the matched covariance matrix; "EigenvalueSeed" uses this automatic calculation as the seed for a numerical for a periodic solution, including e.g. some effects of material; "UserDefined" uses user input for the Twiss parameters at the LatticeStart; "UserDefinedSeed" uses user input for the seed Twiss parameters, and then iterates through an optimisation algorithm to find a periodic solution.
    cd["NormalisedCanonicalAngularMomentum"] = 0; //Normalised canonical angular momentum of beam at lattice start
    cd["LatticeStart"] = -2750 * mm; //Start of lattice over which beta is periodic
    cd["LatticeEnd"] = 2750 * mm; //End of lattice over which beta is periodic
    cd["EnergyAtLatticeStart"] = 226 * MeV; //Desired energy at start of the lattice
    cd["AlphaBetaScaleFactor"] = 11/0.05; //Weighting factor between alpha and beta for "Seed" algorithms
    cd["TransverseEmittance"] = 6 * mm; //Transverse emittance of generated beam
    cd["LongitudinalEmittance"] = 0.1 * ns; //Longitudinal emittance of generated beam
    cd["AmplitudeMomentumCorrelation"] = 0.; //amplitude momentum correlation introduces factor like p *= (1+(A-<A>)*corr) to matched beam
    ci["NumberOfEnergyCoordinates"] = 1; // Calculate matched beta function for various energies
    cd["EnergyRange"] = 0. * MeV; // full energy range over which the multiple beta functions will be calculated
    cd["LongitudinalBeta"] = 10.; //longitudinal beta at z = LatticeStart; set to < 0. to auto-detect
    cd["LongitudinalAlpha"] = 0.; //longitudinal alpha at z = LatticeStart; only used if auto-detect is off
    cd["TransverseBeta"] = 420. * mm; //transverse beta at z = LatticeStart; set to < 0. to auto-detect
    cd["TransverseAlpha"] = 0.; //transverse alpha at z = LatticeStart; only used if auto-detect is off
    cd["DispersionX"] = 0. * mm; //Future development: x dispersion at z = LatticeStart for matcher
    cd["DispersionY"] = 0. * mm; //Future development: y dispersion at z = LatticeStart for matcher


    // <head>Solenoids Field Controls</head>
    cs["FieldMode"] = "Full";     //"Full" = use G4MICE field definitions; "Constant" = use G4 constant fields
    cs["SolDataFiles"] = "Write"; //"Write" = write solenoid field maps; "Read" = read solenoid field maps
    ci["NumberNodesZGrid"] = 100; //Default number of nodes in Z in the solenoid field map grid
    ci["NumberNodesRGrid"] = 150; //Default number of nodes in R in the solenoid field map grid
    cd["SolzMapExtendFactor"] = 10.0;             // Set the Z extent of the solenoid field maps; z_extent of the field map = coil_length + z_factor*coil_inner_radius
    cd["SolrMapExtendFactor"] = 2.01828182845905; // Set the radial extent of solenoid field maps; r_extent of the field map = r_factor * coil_inner_radius
    ci["DefaultNumberOfSheets"] = 10;             // number of sheets for solenoids.

    // <head>Quadrupole Field Controls</head>
    cd["QuadrupoleFringeMaxZFactor"] = 5.; //Maximum z extent of quadrupole fringe fields (as a fraction of quad length)
    ci["QuadrupoleUseFringeFields"] = 1; //Set to 1 to use fringe fields in quadrupoles
    fillVector("QuadrupoleFringeParameters",6,QuadFringeParams); /*comment below is included for documentation - don't delete*/
//    double QuadFringeParams[6] = {0.296471, 4.533219, -2.270982, 1.068627, -0.036391, 0.022261}; //Default parameters for Enge function, that controls multipole fringe fields

    // <head>RF Cavity Field Controls</head>
    cd["rfAccelerationPhase"]      = 1.57 * rad; //Default RF phase of all cavities. May go buggy if this is > pi/2 so be careful
    cd["OpticsRfFrequency"]        = 201.25 * megahertz; //Global RF frequency assumed by optics module.
    cd["PhaseTolerance"]           = 1e-5 * ns; //Tolerance on RF phase
    cd["RFDeltaEnergyTolerance"]   = 1e-5 * MeV; //Tolerance on RF energy gain (peak field)

    //<head>Simulation Application stepping parameters</head>
    /*See G4 Docs on ElectroMagnetic fields*/
    cd["DeltaOneStep"]      =  1. * mm;         // Estimate of error in end point in free space
    cd["DeltaIntersection"] =  1.e-3 * mm;      // Estimate of error in step end point when a track intersects a new physical volume
    cd["EpsilonMin"]        =  1.e-10 * mm;     // Minimum on DeltaOneStep? (unclear in G4 docs)
    cd["EpsilonMax"]        =  1.e+10 * mm;     // Maximum on DeltaOneStep? (unclear in G4 docs)
    cd["MissDistance"]      =  1.e-2 * mm;      // Maximum miss distance when a track passes near a physical volume
    cs["FieldTrackStepper"] = "Classic"; // Set the integration algorithm for tracking. Options are for electromagnetic: "Classic" (= "ClassicalRK4"); "SimpleHeum"; "ImplicitEuler"; "SimpleRunge"; "ExplicitEuler"; and for magnetic only: "CashKarpRKF45"; "HelixImplicitEuler"; "HelixHeum"; "HelixSimpleRunge"; "HelixExplicitEuler".  see G4 documentation for further details.
    cd["VirtualRelativeError"] = 1.e-4;        //Relative error for virtual detector stepping
    cd["VirtualAbsoluteError"] = 1.e-4;        //Absolute error for virtual detector stepping
    cd["MaxStepSizeInChannel"] = 10000.0 * mm; //Deprecated - does nothing

    // <head>Simulation Application RF induced background controls - Deprecated do not use</head>
    cs["rfBGMode"] = "none"; //Deprecated - does nothing
    cs["rfBGSource"] = "none"; //Deprecated - does nothing
    cs["rfBGElectronModel"] = "Simple"; //Deprecated - does nothing
    cs["rfBGPhotonModel"] = "Uniform"; //Deprecated - does nothing
    cs["rfBGEmitterMode"] = "Random"; //Deprecated - does nothing
    ci["rfBGNumberOfEmitters"] = 10; //Deprecated - does nothing
    ci["rfBGNumberOfElectrons"] = 1; //Deprecated - does nothing
    ci["rfBGNumberOfPhotons"] = 1; //Deprecated - does nothing
    ci["rfBGDirection"] = 0; //Deprecated - does nothing
    cd["rfBGElectronEnergy"] = .01 * MeV; //Deprecated - does nothing
    cd["rfBGPhotonEnergy"] = .1 * MeV; //Deprecated - does nothing
    cd["rfBGElectronEnergyWidth"] = .001 * MeV; //Deprecated - does nothing
    cd["rfBGPhotonEnergyWidth"]   = .01 * MeV; //Deprecated - does nothing
    cs["rfBGElectronSpectrum"] = "Fixed"; //Deprecated - does nothing
    cs["rfBGPhotonSpectrum"]   = "Fixed"; //Deprecated - does nothing
    cs["rfBGElectronSpectrumFile"] = ""; //Deprecated - does nothing
    cs["rfBGPhotonSpectrumFile"]   = ""; //Deprecated - does nothing
    cs["rfBGMultipleSpectraFile"]   = "None"; //Deprecated - does nothing
    cs["rfBGBankFile"] = "None"; //Deprecated - does nothing
    cs["rfBGSamplingMode"] = "Loop";  //Deprecated - does nothing
    cd["rfBGTimeWindow"] = 0.2 * ns; //Deprecated - does nothing
    cd["rfBGTimeDelay"] = 0.0 * ns; //Deprecated - does nothing
    cd["rfBGZstart"] = 0.0 * ns; //Deprecated - does nothing
    cd["rfBGRadius"] = 150.0 * mm; //Deprecated - does nothing
    cd["rfBGScaleFactor"] = 1.0; //Deprecated - does nothing

    // <head>Digitization Application Calorimeter Controls</head>
    cd["EMCalAttLengthLong"]          = 2400.0 * mm; //The EM Calorimeter models the number of photoelectrons entering the PMT using Interface/src/EMCalDigit.cc
    cd["EMCalAttLengthShort"]         =  200.0 * mm;
    cd["EMCalAttLengthLongNorm"]      = 0.655 * mm;
    cd["EMCalAttLengthShortNorm"]     = 0.345 * mm;
    cd["EMCalLightCollectionEff"]     = 0.031;
    cd["EMCalEnergyPerPhoton"]        = 0.125 * keV;
    cd["EMCalLightSpeed"]             =  170.0 * mm / ns;
    cd["EMCalLightGuideEff"]          =  0.85;
    cd["EMCalQuantumEff"]             =  0.18;
    cd["EMCalAmplificationCof"]       =1000000.;
    cd["EMCalAmplificationFluct"]     = 1000.;
    cd["EMCalAdcConversionFactor"]    =  0.25*1e-12*coulomb;  // pC per count
    cd["EMCalTdcConversionFactor"]    = 0.025 * ns;
    ci["EMCalAdcThreshold"]           =     2;    // adc counts
    cd["EMCalTdcThreshold"]           =  1.84*1e-12*coulomb;  // pC
    cd["EMCalTdcResolution"]          =  0.05 * ns;
    ci["EMCalTdcBits"]                =    12;
    cs["EMCalHardCodedTrigger"]       = "False";
    cd["EMCalSamplingTimeStart"]      =  100.0 * ns;
    cd["EMCalSamplingTimeWindow"]     =  100.0 * ns;
    cd["EMCalElectronicsDecayTime"]   =    8.0 * ns;

    // <head>Digitization Application SciFi controls</head>
    cs["SciFiDeadChanFName"] = "";
    ci["SciFiMUXNum"] = 7;
    cd["SciFiFiberDecayConst"] = 2.7;
    cd["SciFiFiberConvFactor"] =  3047.1;
    cd["SciFiFiberRefractiveIndex"] = 1.6;
    cd["SciFiFiberCriticalAngle"] = 0.4782 * rad;
    cd["SciFiFiberTrappingEff"] = 0.056;
    cd["SciFiFiberMirrorEff"] = 0.6;
    cd["SciFiFiberTransmissionEff"] = 0.8;
    cd["SciFiMUXTransmissionEff"] = 1.0;
    cd["SciFiFiberRunLength"] = 5000.0 * mm; //ME this number is wrong
    cd["SciFivlpcQE"] = 0.8;
    cd["SciFivlpcEnergyRes"] = 4.0 * MeV;
    cd["SciFivlpcTimeRes"] = 0.2 * ns;
    ci["SciFiadcBits"] = 8;
    cd["SciFiadcFactor"] = 6.0;
    ci["SciFitdcBits"] = 16;
    cd["SciFitdcFactor"] = 1.0;
    ci["SciFinElecChanPerPlane"] = 1429;
    ci["SciFinPlanes"] = 3;
    ci["SciFinStations"] = 5;
    ci["SciFinTrackers"] = 2;

    // <head>Digitization Application TOF controls</head>

    cd["TOFconversionFactor"] = 10 * keV;
    cd["TOFpmtTimeResolution"] = 100.0 * picosecond;
    cd["TOFattenuationLength"] = 140 * cm;
    cd["TOFadcConversionFactor"] = 0.125;
    cd["TOFtdcConversionFactor"] = 25.0 * picosecond;
    cd["TOFpmtQuantumEfficiency"] = 0.25;
    cd["TOFscintLightSpeed"] =  170.0 * mm / ns;

    // <head>Digitization Application Ckov controls</head>

    cd["CKOVadcConversionFactor"] = 0.125;
    cd["CKOVtdcConversionFactor"] = 25.0 * picosecond;
    cd["CKOVpmtTimeResolution"] = 100.0 * picosecond;

    // <head>Reconstruction Application controls</head>

    cd["BeamCharge"] = 1.0;

    cd["SciFiTrackRecStatusCut"] = 2.0;
    cd["SciFiTrackRecRCut"] = 0.0;
    cd["SciFiTrackRecNHitCut"] = 4.0;
    cd["SciFiTrackRecNPECut"] = 2.0;
    cd["SciFiTrackRecChi2Cut"] = 10.0;
    cd["SciFiTripletChi2Cut"] = 10.0;

    cd["SciFiadcFactor"] = 6.0;
    cd["SciFitdcFactor"] = 1.0;
    cd["SciFiCombineWidth"] = 0.0 * mm;
    cd["SciFiCombineTime"] = 2.0 * ns;
    cd["SciFiCombineRadius"] = 1.5 * mm;

    cd["RecP"] = -1 * MeV;

    /*ME - new ones*/
	 cs["DBServerHostName"] = "micewww.pp.rl.ac.uk"; //Database server host name
	 ci["DBServerPort"] = 4443;  //Database server port
    ci["TofTriggerStation"] = 1; // The Station which is giving the trigger
    ci["RecLevel"] = 5;		// 1 = clusters, 2 = points, 3 = tracks, 4 = matching, 5 = pid
    ci["TofRecLevel"] = 1;	// 1 = standard track reconstruction
    ci["TofCalibrationMode"] = 0;	// 0 = reconstruct data, 1 = calculate calibration
    ci["BeamMakerMode"] = 1;	//Matched in tracker/trace ellipse of constant amplitude/uniform distribution... see application for details
    ci["BeamMakerPID"] = +2;	//1=e, 2=mu, 3=pi, 4=K, 5=p, sign=charge (same as ICOOL for003.dat definition)
    cd["ZStart"] = 0.;	//Initial z (absolute) for the BeamMaker application
    cd["Q7Gradient"] = 1.;	//|g0| in T/m for Quadrupole 7 (assumed focussing in x)
    cd["Q8Gradient"] = 1.;	//|g0| in T/m for Quadrupole 8 (assumed defocussing in x)
    cd["Q9Gradient"] = 1.;	//|g0| in T/m for Quadrupole 9 (assumed focussing in x)
    cd["Q7Current"] = 174.09;	//|I| in Amps for Quadrupole 7 (assumed focussing in x, default implies |g0| = 1 T/m)
    cd["Q8Current"] = 174.09;	//|I| in Amps for Quadrupole 8 (assumed defocussing in x, default implies |g0| = 1 T/m)
    cd["Q9Current"] = 174.09;	//|I| in Amps for Quadrupole 9 (assumed focussing in x, default implies |g0| = 1 T/m)
    cd["Iperg0"] = 174.09;	//The current required to get a central gradient of 1 T/m in Quadrupoles 7, 8, and 9
    ci["PlotFrequency"] = 100;	//For applications which plot histograms to the screen: how many events between updating the plots on the screen?
    cd["MinimumTOF"] = 0. * ns;	 //Temporary PID
    cd["MaximumTOF"] = 100. * ns;//Temporary PID

    ci["UseRecCuts"] = 0; //If set to 0, do not use the reconstruction cuts defined below. Otherwise assume all events outside cuts are bad.
    cd["RecMinPX"] = -200 * MeV; //Cut value for reconstructed x momentum
    cd["RecMaxPX"] =  200 * MeV; //Cut value for reconstructed x momentum
    cd["RecMinPY"] = -200 * MeV; //Cut value for reconstructed y momentum
    cd["RecMaxPY"] =  200 * MeV; //Cut value for reconstructed y momentum
    cd["RecMinPT"] =    0 * MeV; //Cut value for reconstructed transverse momentum
    cd["RecMaxPT"] =  200 * MeV; //Cut value for reconstructed transverse momentum
    cd["RecMinPZ"] =   50 * MeV; //Cut value for reconstructed z momentum
    cd["RecMaxPZ"] =  500 * MeV; //Cut value for reconstructed z momentum
    ci["WriteDST"] = 0; //Set to 1 to write only TOF data and Zustand vectors after recontruction. Else write everything.

    ci["NavigatorVerbosity"] = 0; //Set Kalman verbosity level
    ci["FitterVerbosity"] = 0; //Set Kalman verbosity level
    ci["SetupVerbosity"] = 0; //Set Kalman verbosity level
    ci["ModelVerbosity"] = 0; //Set Kalman verbosity level
    ci["MatchingVerbosity"] = 0; //Set Kalman verbosity level

    cd["KalmanStepSize"] = 10. * cm; //Step size for Kalman fitter; used if fields are not constant
    ci["KalmanUseFieldMap"] = 0; //Set to 0 to use a constant field for track fits; set to 1 to use BeamTools (real) fields for reconstruction

    // <head>Analysis Application controls</head>
    cs["AnalysisParametersFile"] = "AnaParams.dat"; //Auxiliary file for analysis; see $MICESRC/Analysis/doc/
    cs["ICOOLfor003StyleFile"]   = "for003.dat"; //icool beam input data file
    cs["ICOOLfor009StyleFile"]   = "for009.dat"; //icool beam output data file
    ci["AnalysisParticlePdgId"]  = -13; //PDG Particle ID; other events will not be used in the analysis
    ci["StartEvent"]             = 0; //Start event - ignores events before this
    cd["TransferMapAbsoluteError"] = 1e-5; //Absolute error when Optics package calculates transfer maps
    cd["TransferMapRelativeError"] = 1e-5; //Relative error when Optics package calculates transfer maps
    cd["OpticsStepSize"]         = 100 * mm; //Step size for Optics package
    cs["OpticsFileName"]         = "Optics.out"; //Output file name for Optics package
    cs["OpticsdEdXModel"]        = "BetheBloch"; //dEdx model for optics package; "MinimumIonising" or "BetheBloch"
    ci["OpticsCalculationOrder"] = 1; //Default order for calculation of optics transfer maps (>1 still doesn't work I'm afraid)
    cs["PidInputFile"]           = "nnReadStage6.root"; //Input PID data file for PIDAnalysis application
    cs["PidTreeName"]            = "eventTree;7"; //Input Tree name for PIDAnalysis application
    cd["PidBGWeight"]            = 1.; // Weighting given to background events in PidAnalysis application
    ci["PidAnalysisPlanes"]      = 1; // Planes used in PidAnalysis application 1 = tracker ref, 2 = tof ref central, 3 = tof ref outer, negative = same with smearing

    //***End Doc Script***

    // Example of use of InputDataCardsVector
    // double avtest[4]             ={1.,2.,3.,4.};
    // fillVector("VectorTest",4,avtest);
    // The equivalent entry in the input file is:
    // VectorTest 4 1. 2. 3. 4.
    // An alternative for the case where default values are all zero is
    // cv["VectorTest"]             = vector<double>(4);
}

dataCards::~dataCards()
{
}

void dataCards::fillVector(const char* st, const int& nentries, const double* entry)
{
  vector<double> cvec(nentries);
  for(int i=0; i<nentries; i++)
    {  cvec[i]=entry[i];
    }
  cv[st] = cvec;
}

int dataCards::readKeys(const char* fileName)
{
  std::ifstream in(fileName);
  if (!in) return 0;
  else     return readKeys(in);
}

int dataCards::readKeys(std::istream& in)
{
  std::string s;
  std::string t;
  std::string c;

  while (in) {
    c="";
    t="";
    getline(in,c);
    std::istringstream is(c);
    is >> t;
    if (t.length() == 0) continue;
    if (t[0] == '#') continue; // Comments cards ignored
    if (t[0] == '!') continue; // MCFast style comment
    int ok = 0;
    InputDataCardsDouble::iterator i = cd.find(t);
    if ( i != cd.end()) {
      double d;
      is >> d;
      cd[t] = d;
      ok = 1;
    }
    if( ok == 0)
    {
      InputDataCardsInt::iterator i1 = ci.find(t);
      if(i1 != ci.end()){
        int ii;
        is >> ii;
        ci[t] = ii;
        ok =1;
      }
    }
    if (ok == 0) {
      std::istringstream is2(c);
      is2 >> t;
      InputDataCardsString::iterator i2 = cs.find(t);
      if ( i2 != cs.end()) {
        std::string s2;
        is>> s2;
        cs[t] = s2;
        ok = 1;
      }
    }
    if (ok == 0) {
      std::istringstream iv(c);
      iv >> t;
      InputDataCards3Vector::iterator i2 = c3v.find(t);
      if ( i2 != c3v.end()) {
        Hep3Vector v;
        iv >> v;
        c3v[t] = v;
        ok = 1;
      }
    }
    if (ok == 0) {
      InputDataCardsVector::iterator i2 = cv.find(t);
      if ( i2 != cv.end()) {
        int nentries;
        is >> nentries;
  vector<double> cvec(nentries);
  for(int i=0; i<nentries; i++) {
    double vd;
    if ( is.good() ) {
      is >> vd;
    }
    else {
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error reading key "+t+". Bad data format", "dataCards::readKeys"));
    }
    cvec[i] = vd;
  }
  cv[t] = cvec;
        ok = 1;
      }
    }
    if (ok != 1)
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Failed to recognise datacard "+t+".", "dataCards::readKeys"));
  }
  return 1;
}

double dataCards::fetchValueDouble(const std::string& key)
{
  InputDataCardsDouble::iterator i=cd.find(key);
  if (i == cd.end())
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error fetching unknown datacard "+key+".", "dataCards::fetchValueDouble"));
  return i->second;
}

int dataCards::fetchValueInt(const std::string& key){
 InputDataCardsInt::iterator i=ci.find(key);
 if( i == ci.end())
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error fetching unknown datacard "+key+".", "dataCards::fetchValueInt"));
 else
   return i->second;
 InputDataCardsDouble::iterator i2=cd.find(key); // only gets here if not
 if( i2 == cd.end())                             // stored as int
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error fetching unknown datacard "+key+".", "dataCards::fetchValueInt"));
 return (int) i->second;
}

Hep3Vector dataCards::fetchValue3Vector(const std::string& key)
{
  InputDataCards3Vector::iterator i=c3v.find(key);
  if (i == c3v.end())
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error fetching unknown datacard "+key+".", "dataCards::fetchValue3Vector"));
  return i->second;
}

std::string dataCards::fetchValueString(const std::string& key)
{
  InputDataCardsString::iterator i=cs.find(key);
  if (i == cs.end())
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error fetching unknown datacard "+key+".", "dataCards::fetchValueString"));
  return i->second;
}

std::vector<double> dataCards::fetchValueVector(const std::string& key)
{
  InputDataCardsVector::iterator i=cv.find(key);
  if (i == cv.end())
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error fetching unknown datacard "+key+".", "dataCards::fetchValueVector"));
  return i->second;
}

void dataCards::printValue(const std::string& key)
{
  InputDataCardsDouble::iterator id=cd.find(key);
  if (id != cd.end()) {
    std::cout << id->second;
    return;
  }
  InputDataCardsInt::iterator in=ci.find(key);
  if (in != ci.end()){
    std::cout << in->second;
    return;
  }
  InputDataCards3Vector::iterator i3v=c3v.find(key);
  if (i3v != c3v.end()) {
    std::cout << i3v->second;
    return;
  }
  InputDataCardsString::iterator is=cs.find(key);
  if (is != cs.end()) {
    std::cout << is->second;
    return;
  }
  InputDataCardsVector::iterator iv=cv.find(key);
  if (iv != cv.end()) {
    std::vector<double> v = iv->second;
    for (unsigned int i = 0; i < v.size(); i++) {
      if (i==0) std::cout << "(";
      else std::cout << ",";
      std::cout << v[i];
    }
    std::cout << ")";
    return;
  }
  throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "datacard "+key+" not found.", "dataCards::printValue"));
}

std::ostream& operator<<(std::ostream& o, const dataCards& d)
{

  o << " ============== DataCards ==============" << endl;

  InputDataCardsString::const_iterator si = d.cs.begin();
  for (; si != d.cs.end(); ++si) {
    o << "  " << si->first << " = " << si->second << endl;
  }

  o << " ---------------------------------------" << endl;

  InputDataCardsInt::const_iterator ii = d.ci.begin();
  for (; ii != d.ci.end(); ++ii) {
    o << "  " << ii->first << " = " << ii->second << endl;
  }

  o << " ---------------------------------------" << endl;

  InputDataCardsDouble::const_iterator di = d.cd.begin();
  for (; di != d.cd.end(); ++di) {
    o << "  " << di->first << " = " << di->second << endl;
  }

  o << " ---------------------------------------" << endl;

  InputDataCards3Vector::const_iterator v3i = d.c3v.begin();
  for (; v3i != d.c3v.end(); ++v3i) {
    o << "  " << v3i->first << " = " << v3i->second << endl;
  }
  o << " ---------------------------------------" << endl;

  InputDataCardsVector::const_iterator vi = d.cv.begin();
  for (; vi != d.cv.end(); ++vi) {
    o << "  " << vi->first << " = (";
    std::vector<double> v = vi->second;
    for (unsigned int i = 0; i < v.size(); i++) {
      if (i>0) o << ",";
      o << v[i];
    }
    o << ")" << endl;
  }

  o << " =======================================" << endl;

  return o;

}


