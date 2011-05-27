// @(#) $Id: RFParameters.hh,v 1.16 2010-03-10 08:14:10 rogers Exp $  $Name:  $
//
//  MICE RF parameters
//

#ifndef RFPARAMETERS_H
#define RFPARAMETERS_H 1

#include <string>
#include <vector>
#include "CLHEP/Units/SystemOfUnits.h"

class RFParameters {

  double  frfAccelerationPhase;
  double  frfRefEnergy;
  double  frfRefCharge;
  double  frfFrequency;
  double  frfPhaseTolerance;
  double  frfEnergyTolerance;
  std::string fphaseModel;

  double  frfOffsetZ;
  std::string  frfCellType;
  double  frfPeakField;
  double  frfCellLength;
  double  frfCavityRadius;
  int     frfNumCellPerLinac;
  std::string  frfWindowType;
  double  frfWindowThick;
  double  frfWindowRadius;
  double  frfCellSkinDepth;
  double  frfWallThick;
  std::string  frfWindowMaterial;
  double  fStepSizeInRf;
  double  fNominalEGainPerLinac;
  double  frfRiseTime;
  double  frfPulseLength;
//  std::string  frfPhaseInputFile;
//  double  frfGridCavOption;
//  std::string  fEMFieldTest;
  bool    frfInstrumentCavities;
  bool    frfInstrumentWindows;
  double frfCellGap;
  std::vector<double> frfTimeDelays;

  static RFParameters* fInstance;

public:

  static RFParameters* getInstance();
  RFParameters();
  
  double  rfAccelerationPhase()  { return frfAccelerationPhase; }
  double  opticsRfFrequency()    { return frfFrequency; }
//  double  rfRefEnergy()          { return frfRefEnergy; }
//  double  rfRefCharge()          { return frfRefCharge; }
  double  rfPhaseTolerance()     { return frfPhaseTolerance; }
  double  rfEnergyTolerance()    { return frfEnergyTolerance; }
  std::string phaseModel()       { return fphaseModel; }

	//THESE DO NOTHING, INCLUDED FOR BACKWARDS COMPATIBILITY
  double  rfOffsetZ()            { return frfOffsetZ; }
  std::string  rfCellType()           { return frfCellType; }
  double  rfFrequency()          { return frfFrequency; }
  double  rfPeakField()          { return frfPeakField; }
  double  rfCellLength()         { return frfCellLength; }
  double  rfCavityRadius()       { return frfCavityRadius; }
  int     rfNumCellPerLinac()    { return frfNumCellPerLinac; }
  std::string  rfWindowType()         { return frfWindowType; }
  double  rfWindowThick()        { return frfWindowThick; }
  double  rfWindowRadius()       { return frfWindowRadius; }
  double  rfCellSkinDepth()      { return frfCellSkinDepth; }
  double  rfWallThick()          { return frfWallThick; }
  std::string  rfWindowMaterial()     { return frfWindowMaterial; }
  double  StepSizeInRf()         { return fStepSizeInRf; }
  double  NominalEGainPerLinac() { return fNominalEGainPerLinac; }
  double  rfRiseTime()           { return frfRiseTime; }
  double  rfPulseLength()        { return frfPulseLength; }
//  std::string  rfPhaseInputFile()     { return frfPhaseInputFile; }
//  double  rfGridCavOption()      { return frfGridCavOption; }
//  std::string  EMFieldTest()          { return fEMFieldTest; }
  bool    rfInstrumentCavities() { return frfInstrumentCavities; }
  bool    rfInstrumentWindows()  { return frfInstrumentWindows; }
	double  rfCellGap() {return frfCellGap;}
	std::vector<double> rfTimeDelays() {return frfTimeDelays;}
};

#endif
