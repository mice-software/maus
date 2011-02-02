//--------------------------------------------------------------
//
// File     : CKOV1DigitsParams.cc
// Purpose  : Upstream cherenkov detector: geometry and material
// Created  : 14/5/03  Romulus Godang
//
//------------------------------------------------------------- 

#include <stdlib.h>
#include <stdio.h>
#include "CKOV1DigitsParams.hh"
#include "Interface/dataCards.hh"

CKOV1DigitsParams::CKOV1DigitsParams()
{
  DefineCKOV1Params();
  //  Ckov1ConvFactor = MyDataCards.fetchValueDouble("Ckov1ConvFactor");
  Ckov1ConvFactor = MyDataCards.fetchValueDouble("Ckov1adcConversionFactor");
  Ckov1RefractiveIndex = 
  MyDataCards.fetchValueDouble("Ckov1RefractiveIndex");
  Ckov1CriticalAngle = MyDataCards.fetchValueDouble("Ckov1CriticalAngle");

//  Ckov1DecayConst = MyDataCards.fetchValueDouble("Ckov1DecayConst");
  _conversionFactor = MyDataCards.fetchValueDouble("Ckov1ConversionFactor");
  Ckov1EnergyRes = MyDataCards.fetchValueDouble("Ckov1pmtEnergyResolution");
  Ckov1TimeRes = MyDataCards.fetchValueDouble("Ckov1pmtTimeResolution");

  adcBits = (int)floor(MyDataCards.fetchValueDouble("Ckov1adcBits"));
//  adcFactor = MyDataCards.fetchValueDouble("Ckov1adcFactor");
  adcFactor = MyDataCards.fetchValueDouble("Ckov1adcConversionFactor");
  tdcBits = (int)floor(MyDataCards.fetchValueDouble("Ckov1tdcBits"));
  tdcFactor = MyDataCards.fetchValueDouble("Ckov1tdcConversionFactor");
  _tdcConversionFactor = 
              MyDataCards.fetchValueDouble("Ckov1tdcConversionFactor");
  _xpmt = MyDataCards.fetchValueDouble("Ckov1PmtXctr");
  _ypmt = MyDataCards.fetchValueDouble("Ckov1PmtYctr");
  _rpmt = MyDataCards.fetchValueDouble("Ckov1PmtRadius");
  _mirrorReflectivity =MyDataCards.fetchValueDouble("Ckov1MirrorReflectivity");
  _numberPmts = MyDataCards.fetchValueInt("Ckov1NumberPmts");
  _radiatorThickness = 2.*MyDataCards.fetchValueDouble("Ckov1HeightRad");
  _totalThickness = _radiatorThickness +
    2*MyDataCards.fetchValueDouble("Ckov1LengthZMother") -
    0.7071*MyDataCards.fetchValueDouble("Ckov1LengthZMirror") +
    MyDataCards.fetchValueDouble("Ckov1LengthYMother");
}

CKOV1DigitsParams* CKOV1DigitsParams::_instance = (CKOV1DigitsParams*) NULL;

CKOV1DigitsParams* CKOV1DigitsParams::getInstance()
{
  if(_instance) return _instance;
  _instance = new CKOV1DigitsParams;
  return _instance;
}

CKOV1DigitsParams::~CKOV1DigitsParams()
{
}
void CKOV1DigitsParams::DefineCKOV1Params()
{
  //  Ckov1Nphotons        = 820;
  // Ckov1NphotonsPMT     = 80;    //assuming 10% of total 
  //  Ckov1NphotoElectrons = 40;
  // Ckov1PMTQuantumEff   = 0.50;  //light collection efficiency 
}
