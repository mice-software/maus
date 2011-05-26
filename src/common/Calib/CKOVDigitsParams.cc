#include <stdlib.h>
#include <stdio.h>
#include "Calib/CKOVDigitsParams.hh"
#include "Interface/dataCards.hh"

CKOVDigitsParams::CKOVDigitsParams()
{
  DefineCKOVParams();
  ckovConvFactor = MyDataCards.fetchValueDouble("TileCkovConvFactor");
  ckovRefractiveIndex = 
	MyDataCards.fetchValueDouble("TileCkovRefractiveIndex");
  ckovCriticalAngle = MyDataCards.fetchValueDouble("TileCkovCriticalAngle");
  ckovDecayConst = MyDataCards.fetchValueDouble("TileCkovDecayConst");

  adcBits = (int)floor(MyDataCards.fetchValueDouble("CkovadcBits"));
  adcFactor = MyDataCards.fetchValueDouble("CKOVadcFactor");
  tdcBits = (int)floor(MyDataCards.fetchValueDouble("CkovFitdcBits"));
  tdcFactor = MyDataCards.fetchValueDouble("CkovFitdcFactor");

}
CKOVDigitsParams::~CKOVDigitsParams()
{
}
void CKOVDigitsParams::DefineCKOVParams()
{
  ckovNphotons        = 820;
  ckovNphotonsPMT     = 80;    //assuming 10% of total 
  ckovNphotoElectrons = 40;
  ckovPMTQuantumEff   = 0.50;  //light collection efficiency 
}






