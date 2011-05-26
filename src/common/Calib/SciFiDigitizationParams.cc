/* 
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#include <stdlib.h>
#include <stdio.h>
#include "Calib/SciFiDigitizationParams.hh"

#include "Interface/dataCards.hh"

SciFiDigitizationParams* SciFiDigitizationParams::_instance = (SciFiDigitizationParams*)NULL;

SciFiDigitizationParams* SciFiDigitizationParams::getInstance()
{
  if( ! _instance )
    _instance = new SciFiDigitizationParams();

  return _instance;
}

SciFiDigitizationParams::SciFiDigitizationParams()
{
  strcpy(deadChanFName, MyDataCards.fetchValueString("SciFiDeadChanFName").c_str());
  MUXNum = MyDataCards.fetchValueInt("SciFiMUXNum");
  fiberDecayConst = MyDataCards.fetchValueDouble("SciFiFiberDecayConst");
  fiberConvFactor = MyDataCards.fetchValueDouble("SciFiFiberConvFactor");
  fiberRefractiveIndex = 
	MyDataCards.fetchValueDouble("SciFiFiberRefractiveIndex");
  fiberCriticalAngle = MyDataCards.fetchValueDouble("SciFiFiberCriticalAngle");
  fiberTrappingEff =MyDataCards.fetchValueDouble("SciFiFiberTrappingEff");
  fiberMirrorEff = MyDataCards.fetchValueDouble("SciFiFiberMirrorEff");
  fiberTransmissionEff = 
	MyDataCards.fetchValueDouble("SciFiFiberTransmissionEff");
  MUXTransmissionEff = 
	MyDataCards.fetchValueDouble("SciFiMUXTransmissionEff");
  fiberRunLength = MyDataCards.fetchValueDouble("SciFiFiberRunLength");
  vlpcQE = MyDataCards.fetchValueDouble("SciFivlpcQE");
  vlpcEnergyRes = MyDataCards.fetchValueDouble("SciFivlpcEnergyRes");
  vlpcTimeRes = MyDataCards.fetchValueDouble("SciFivlpcTimeRes");
  adcBits = MyDataCards.fetchValueInt("SciFiadcBits");
  adcFactor = MyDataCards.fetchValueDouble("SciFiadcFactor");
  tdcBits = MyDataCards.fetchValueInt("SciFitdcBits");
  tdcFactor = MyDataCards.fetchValueDouble("SciFitdcFactor");

  nElecChanPerPlane = MyDataCards.fetchValueInt("SciFinElecChanPerPlane");
  nPlanes = MyDataCards.fetchValueInt("SciFinPlanes");
  nStations = MyDataCards.fetchValueInt("SciFinStations");
  nTrackers = MyDataCards.fetchValueInt("SciFinTrackers");
}

SciFiDigitizationParams::~SciFiDigitizationParams()
{
}
