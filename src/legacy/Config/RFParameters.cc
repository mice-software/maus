// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: RFParameters.cc,v 1.18 2010-03-10 08:14:10 rogers Exp $  $Name:  $
//
//  MICE RF parameters

#include "Config/RFParameters.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICERun.hh"

RFParameters::RFParameters()
{
  dataCards& MyDataCards = *(MICERun::getInstance()->DataCards);

  frfAccelerationPhase  = MyDataCards.fetchValueDouble("rfAccelerationPhase");

  fphaseModel           = MyDataCards.fetchValueString("EnergyLossModel");
  frfFrequency          = MyDataCards.fetchValueDouble("OpticsRfFrequency");
  frfPhaseTolerance     = MyDataCards.fetchValueDouble("PhaseTolerance");
  frfEnergyTolerance    = MyDataCards.fetchValueDouble("RFDeltaEnergyTolerance");

}

RFParameters* RFParameters::fInstance = (RFParameters*) NULL;

RFParameters* RFParameters::getInstance()
{

  if(!fInstance) fInstance = new RFParameters;
  return fInstance;

}

