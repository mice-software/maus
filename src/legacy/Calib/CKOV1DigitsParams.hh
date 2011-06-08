//--------------------------------------------------------------
//
//      File     : CKOV1DigitsParams.hh
//      Purpose  : Upstream cherenkov detector: geometry and material
//      Created  : 14/5/03  Romulus Godang
//      Revised  : 19-Oct-2004  by Steve Kahn
//
//------------------------------------------------------------- 

#ifndef CKOV1DIGITSPARAMS_HH
#define CKOV1DIGITSPARAMS_HH

#include <stdio.h>
#include "CLHEP/Units/SystemOfUnits.h"

class CKOV1DigitsParams
{
public:

  CKOV1DigitsParams();
  ~CKOV1DigitsParams();
        
  void   DefineCKOV1Params();

  double getCkov1Edep()	              {return Ckov1Edep;} 
  double getCkov1PathLength()         {return Ckov1pathLength;}
  double getCkov1PMTQuantumEff()      {return Ckov1PMTQuantumEff;}

  double getAdcConversionFactor() const  {return Ckov1ConvFactor;}
  double getTdcConversionFactor() const { return _tdcConversionFactor;}
  double getRefractiveIndex() const   {return Ckov1RefractiveIndex;}
  double getCriticalAngle() const     {return Ckov1CriticalAngle;}
//  double getCkov1DecayConst() 	      {return Ckov1DecayConst;}
  double getEnergyRes()	const         {return Ckov1EnergyRes;}
  double getTimeRes() const	      {return Ckov1TimeRes;}
  double getPEconversionFactor() const { return _conversionFactor; }

  int    getCkov1Nphotons()	      {return Ckov1Nphotons;}
  int    getCkov1NphotonsPMT()	      {return Ckov1NphotonsPMT;}
  int    getCkov1NphotoElectrons()    {return Ckov1NphotoElectrons;}

  int    getAdcBits()	const	      {return adcBits;}
  double getAdcFactor()	const	      {return adcFactor;}
  int    getTdcBits()	const	      {return tdcBits;}
  double getTdcFactor()	const	      {return tdcFactor;}
  double getXctrPmt() const { return  _xpmt; }
  double getYctrPmt() const {  return  _ypmt; }
  double getRadiusPmt() const {  return  _rpmt; }
  double getMirrorReflectivity() const {  return _mirrorReflectivity; }
  double getRadiatorThickness() const { return _radiatorThickness; }
  double getTotalThickness() const { return _totalThickness; }
  int  getNumberPmts() const {  return  _numberPmts; }
  static  CKOV1DigitsParams*      getInstance();
	      
private:
  // Characteristics of the ckov
  double Ckov1Edep;               //Ckov Energy Deposit
  double Ckov1pathLength;         //t0 when the particle hit Ckov
  double Ckov1PMTQuantumEff;      //PMTQuantum Efficiency for Ckov

  double Ckov1ConvFactor;         //Number of photons per MeV dep
  double Ckov1RefractiveIndex;    //Refractive Index of ckov, default 1.05
  double Ckov1CriticalAngle;      //In radians
//  double Ckov1DecayConst;         //Ckov time constant

  int    Ckov1Nphotons;           //Number of Photons in Ckov
  int    Ckov1NphotonsPMT;        //Number of Photons in PMT
  int    Ckov1NphotoElectrons;    //Number of photoelectrons including QE 

  // Characteristics of the detector layout
  double Ckov1EnergyRes;          //sigma in energy for CKOV1
  double Ckov1TimeRes;            //sigma in time for CKOV1  

  int    adcBits;                 //number of bits in the ADC
  double adcFactor;               //number of PE/count
  int    tdcBits;                 //number of bits in the TDC
  double tdcFactor;               //number of ns/count
  double _totalThickness;
  double _xpmt, _ypmt;            // pmt position
  double _rpmt;                   // pmt radius
  double _mirrorReflectivity;
  double _radiatorThickness;
  double _tdcConversionFactor;
  double _conversionFactor;
  int     _numberPmts;

  static CKOV1DigitsParams*     _instance;
};

#endif
