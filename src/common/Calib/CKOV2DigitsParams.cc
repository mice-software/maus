//-----------------------------------------------------------------------
//
//    File :      CKOV2DigitsParams.cc
//    Purpose :   Manage Parameters for the CKOV2 digitization.
//    Created :   22-MAR-2003  by Steve Kahn
//
//-----------------------------------------------------------------------

#include "Calib/CKOV2DigitsParams.hh"
#include "Interface/dataCards.hh"

CKOV2DigitsParams::CKOV2DigitsParams()
{
  _refractiveIndex = MyDataCards.fetchValueDouble("Ckov2RefractiveIndex");
  _criticalAngle = MyDataCards.fetchValueDouble("Ckov2CriticalAngle");
  _pmtQuantumEff = MyDataCards.fetchValueDouble("Ckov2PMTQuantumEff");
  _adcConversionFactor=MyDataCards.fetchValueDouble("Ckov2adcConversionFactor");
  _tdcConversionFactor=MyDataCards.fetchValueDouble("Ckov2tdcConversionFactor");
  _conversionFactor = MyDataCards.fetchValueDouble("Ckov2ConversionFactor");
  _pmtEnergyResolution=MyDataCards.fetchValueDouble("Ckov2pmtEnergyResolution");
  _pmtTimeResolution = MyDataCards.fetchValueDouble("Ckov2pmtTimeResolution");
  _adcBits = (int) MyDataCards.fetchValueDouble("Ckov2adcBits");
  _tdcBits = (int) MyDataCards.fetchValueDouble("Ckov2tdcBits");
  _radiatorThickness=2*MyDataCards.fetchValueDouble("Ckov2RadiatorZhalfLeng");
  _totalThickness=_radiatorThickness + 
        MyDataCards.fetchValueDouble("Ckov2MirrorBoxZhalfLeng") +
        MyDataCards.fetchValueDouble("Ckov2MirrorBoxYhalfLeng");
  _numberPmts = MyDataCards.fetchValueInt("Ckov2NumberPmts");
  _mirrorReflectivity = MyDataCards.fetchValueDouble("Ckov2MirrorReflectivity");
  _xpmt = MyDataCards.fetchValueDouble("Ckov2PmtXctr");
  _ypmt = MyDataCards.fetchValueDouble("Ckov2PmtYctr");
  _rpmt = MyDataCards.fetchValueDouble("Ckov2PmtRadius");
}

CKOV2DigitsParams* CKOV2DigitsParams::_instance = (CKOV2DigitsParams*) NULL;

CKOV2DigitsParams* CKOV2DigitsParams::getInstance()
{
  if(_instance) return _instance;
  _instance = new CKOV2DigitsParams;
  return _instance;
}
