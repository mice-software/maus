// @(#) $Id: CoolingChannelGeom.hh,v 1.7 2006-11-03 20:01:57 hart Exp $  $Name:  $
//
//  MICE magnetic lattice parameters
//
//  Yagmur Torun

#ifndef COOLINGCHANNELGEOM_H
#define COOLINGCHANNELGEOM_H 1

#include <string>
#include "CLHEP/Units/SystemOfUnits.h"

class CoolingChannelGeom {
public:
  std::string  fFieldTrackStepper;
  double  fDeltaOneStep;
	double  fDeltaIntersection;
	double  fMissDistance;
  double  fMaxStepSizeInChannel;
	double  fEpsilonMin;
	double  fEpsilonMax;

  double  fPbBlockPosition;
  double  fPbBlockSize;
  double  fPbBlockThickness;

  double  fFeShieldPosition;
  double  fFeShieldRadius;
  double  fFeShieldHoleRadius;
  double  fFeShieldThickness;

  int     fNumberOfCoolingCells;
  int     fNumberOfMatchingCoils;
  double  fLatticePeriod;
  double  fFocusCoilGap;
  double  fFocusToCouplingCoilGap;
  double  fEndCoilGap;
  double  fEndCoilToSolenoidGap;
  double  fEndToMatchingCoilGap;
  double  fMatchingCoilGap;
  double  fMatchingToFocusCoilGap;

  static CoolingChannelGeom* fInstance;

public:

  static CoolingChannelGeom* getInstance();
  CoolingChannelGeom();

  std::string  FieldTrackStepper()      { return fFieldTrackStepper; }
  double  DeltaOneStep()                { return fDeltaOneStep; }
	double  DeltaIntersection()           { return fDeltaIntersection; }
	double  MissDistance()                { return fMissDistance; }
	double  EpsilonMax()                  { return fEpsilonMax; }
	double  EpsilonMin()                  { return fEpsilonMin; }

  double  MaxStepSizeInChannel()   { return fMaxStepSizeInChannel; }
  double  PbBlockPosition()        { return fPbBlockPosition; }
  double  PbBlockSize()            { return fPbBlockSize; }
  double  PbBlockThickness()       { return fPbBlockThickness; }
  double  FeShieldPosition()       { return fFeShieldPosition; }
  double  FeShieldRadius()         { return fFeShieldRadius; }
  double  FeShieldHoleRadius()     { return fFeShieldHoleRadius; }
  double  FeShieldThickness()      { return fFeShieldThickness; }
  int     NumberOfCoolingCells()   { return fNumberOfCoolingCells; }
  int     NumberOfMatchingCoils()  { return fNumberOfMatchingCoils; }
  double  LatticePeriod()          { return fLatticePeriod; }
  double  FocusCoilGap()           { return fFocusCoilGap; }
  double  FocusToCouplingCoilGap() { return fFocusToCouplingCoilGap; }
  double  EndCoilGap()             { return fEndCoilGap; }
  double  EndCoilToSolenoidGap()   { return fEndCoilToSolenoidGap; }
  double  EndToMatchingCoilGap()   { return fEndToMatchingCoilGap; }
  double  MatchingCoilGap()        { return fMatchingCoilGap; }
  double  MatchingToFocusCoilGap() { return fMatchingToFocusCoilGap; }

};

#endif
