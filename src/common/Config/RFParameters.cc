// @(#) $Id: RFParameters.cc,v 1.18 2010-03-10 08:14:10 rogers Exp $  $Name:  $
//
//  MICE RF parameters
//
//  Yagmur Torun

#include "Config/RFParameters.hh"
#include "Interface/dataCards.hh"

RFParameters::RFParameters()
{
  miceMemory.addNew( Memory::RFParameters );
	
  frfAccelerationPhase  = MyDataCards.fetchValueDouble("rfAccelerationPhase");
//  frfRefCharge          = MyDataCards.fetchValueDouble("MuonCharge");
//  frfRefEnergy          = MyDataCards.fetchValueDouble("NominalKineticEnergy");
  fphaseModel           = MyDataCards.fetchValueString("EnergyLossModel");
  frfFrequency          = MyDataCards.fetchValueDouble("OpticsRfFrequency");
  frfPhaseTolerance     = MyDataCards.fetchValueDouble("PhaseTolerance");
  frfEnergyTolerance    = MyDataCards.fetchValueDouble("RFDeltaEnergyTolerance");

/*
  frfOffsetZ            = MyDataCards.fetchValueDouble("rfOffsetZ");
  frfCellType           = MyDataCards.fetchValueString("rfCellType");
  frfFrequency          = MyDataCards.fetchValueDouble("rfFrequency");
  frfPeakField          = MyDataCards.fetchValueDouble("rfPeakField");
  frfCellLength         = MyDataCards.fetchValueDouble("rfCellLength");
  frfCavityRadius       = MyDataCards.fetchValueDouble("rfCavityRadius");
  frfNumCellPerLinac    = MyDataCards.fetchValueInt("rfNumCellPerLinac");
  frfWindowType         = MyDataCards.fetchValueString("rfWindowType");
  frfWindowThick        = MyDataCards.fetchValueDouble("rfWindowThick");
  frfWindowRadius       = MyDataCards.fetchValueDouble("rfWindowRadius");
  frfCellSkinDepth      = MyDataCards.fetchValueDouble("rfCellSkinDepth");
  frfWallThick          = MyDataCards.fetchValueDouble("rfWallThick");
  frfWindowMaterial     = MyDataCards.fetchValueString("rfWindowMaterial");
  fStepSizeInRf         = MyDataCards.fetchValueDouble("StepSizeInRf");
  fNominalEGainPerLinac = MyDataCards.fetchValueDouble("NominalEGainPerLinac");
  frfRiseTime           = MyDataCards.fetchValueDouble("rfRiseTime");
  frfPulseLength        = MyDataCards.fetchValueDouble("rfPulseLength");
//  frfPhaseInputFile     = MyDataCards.fetchValueString("rfPhaseInputFile");
//  frfGridCavOption      = MyDataCards.fetchValueDouble("rfGridCavOption");
//  fEMFieldTest          = MyDataCards.fetchValueString("EMFieldTest");
  frfInstrumentCavities = (MyDataCards.fetchValueString("InstrumentRf") == "True");
  frfInstrumentWindows  = (MyDataCards.fetchValueString("InstrumentRfWindows") == "True");
  frfCellGap            = (MyDataCards.fetchValueDouble("rfCellGap"));
  frfTimeDelays         = (MyDataCards.fetchValueVector("rfTimeDelays"));
*/
}

RFParameters* RFParameters::fInstance = (RFParameters*) NULL;

RFParameters* RFParameters::getInstance()
{

  if(!fInstance) fInstance = new RFParameters;
  return fInstance;

}

