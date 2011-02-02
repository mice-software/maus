//------------------------------------------------------
//
// $Id: CKOVDigitsParams.hh,v 1.2 2002-11-04 21:42:00 torun Exp $
// GEANT4 tag $Name:  $
// 
// Created: 9/26/02 Romulus Godang
//
//------------------------------------------------------

#ifndef CKOVDIGITSPARAMS_HH
#define CKOVDIGITSPARAMS_HH

#include <stdio.h>
#include "globals.hh"

class CKOVDigitsParams
{
public:

  CKOVDigitsParams();
  ~CKOVDigitsParams();
        
  void   DefineCKOVParams();

  double getCkovEdep()	             {return ckovEdep;} 
  double getCkovPathLength()         {return ckovpathLength;}
  double getCkovPMTQuantumEff()      {return ckovPMTQuantumEff;}

  double getCkovConvFactor()         {return ckovConvFactor;}
  double getCkovRefractiveIndex()    {return ckovRefractiveIndex;}
  double getCkovCriticalAngle()	     {return ckovCriticalAngle;}
  double getCkovDecayConst()	     {return ckovDecayConst;}
  double getCkovEnergyRes()	     {return ckovEnergyRes;}
  double getCkovTimeRes()	     {return ckovTimeRes;}

  int    getCkovNphotons()	     {return ckovNphotons;}
  int    getCkovNphotonsPMT()	     {return ckovNphotonsPMT;}
  int    getCkovNphotoElectrons()    {return ckovNphotoElectrons;}

  int    getAdcBits()		     {return adcBits;}
  double getAdcFactor()		     {return adcFactor;}
  int    getTdcBits()		     {return tdcBits;}
  double getTdcFactor()		     {return tdcFactor;}
	      
private:
  // Characteristics of the ckov
  double ckovEdep;               //Ckov Energy Deposit
  double ckovpathLength;         //t0 when the particle hit Ckov
  double ckovPMTQuantumEff;      //PMTQuantum Efficiency for Ckov

  double ckovConvFactor;         //Number of photons per MeV dep
  double ckovRefractiveIndex;    //Refractive Index of ckov, default 1.05
  double ckovCriticalAngle;      //In radians
  double ckovDecayConst;         //Ckov time constant

  int    ckovNphotons;           //Number of Photons in Ckov
  int    ckovNphotonsPMT;        //Number of Photons in PMT
  int    ckovNphotoElectrons;    //Number of photoelectrons including QE 

  // Characteristics of the detector layout
  double ckovEnergyRes;          //sigma in energy for CKOV
  double ckovTimeRes;            //sigma in time for CKOV

  int    adcBits;                //number of bits in the ADC
  double adcFactor;              //number of PE/count
  int    tdcBits;                //number of bits in the TDC
  double tdcFactor;              //number of ns/count
};

#endif
