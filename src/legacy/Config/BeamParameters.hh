// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: BeamParameters.hh,v 1.12 2010-03-10 08:14:10 rogers Exp $  $Name:  $
//
//  MICE beam parameters
//

#ifndef BEAMPARAMETERS_H
#define BEAMPARAMETERS_H 1

#include <string>
#include "CLHEP/Units/SystemOfUnits.h"

class BeamParameters {

  std::string  fBeamType;
  int     fRandomSeed;
  int     fNumEvts;
  std::string  fKineticMode;
  double  fAverageKineticEnergy;
  double  fNominalKineticEnergy;
  double  fKineticStart;
  double  fKineticEnd;
  double  fDeltaEoverE;
  std::string  fTransverseMode;
  double  fDeltaBetaFunction;
  double  fBetaFuncAtStart;
  double  fSigmaX;
  double  fSigmaXPrime;
  double  fSigmaY;
  double  fSigmaYPrime;
  double  fXStart;
  double  fSignXPrime;
  std::string  fFieldAngularMomentum;
  double  fModeOperation;
  double  fTransvLongCorrFact;
  double  fBunchLength;
  bool    fMuonDecay;
  double  fZOffsetStart;
  double  fTauOffsetStart;
  std::string  fInputBeamFileName;
  double  fBeamProtonFraction;
  double  fBeamPionFraction;
  double  fBeamKaonFraction;
  double  fBeamElectronFraction;

  static BeamParameters* fInstance;

public:

  static BeamParameters* getInstance();
  int RandomSeed() {return 0;}
  int RandomSeed( int eventNumber) {return eventNumber;}
  
  
};

#endif

