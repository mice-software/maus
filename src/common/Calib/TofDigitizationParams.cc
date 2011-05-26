//---------------------------------------------------------------------------
//
//     File :        TofDigitizationParams.cc
//     Purpose :     Supply parameters for the TOF digitization
//     Created :     23-OCT-2002  by Steve Kahn
//
//---------------------------------------------------------------------------

#include "Calib/TofDigitizationParams.hh"
#include "Interface/dataCards.hh"
#include "globals.hh"

using namespace std;

TofDigitizationParams::TofDigitizationParams()
{
  _conversionFactor1 = MyDataCards.fetchValueDouble("TOF1conversionFactor");
  _conversionFactor2 = MyDataCards.fetchValueDouble("TOF2conversionFactor");
  _conversionFactor3 = MyDataCards.fetchValueDouble("TOF3conversionFactor");
  _refractionIndex1 = MyDataCards.fetchValueDouble("TOF1scintRefractionIndex");
  _refractionIndex2 = MyDataCards.fetchValueDouble("TOF2scintRefractionIndex");
  _refractionIndex3 = MyDataCards.fetchValueDouble("TOF3scintRefractionIndex");
  _energyRes1 = MyDataCards.fetchValueDouble("TOF1pmtEnergyResolution");
  _energyRes2 = MyDataCards.fetchValueDouble("TOF2pmtEnergyResolution");
  _energyRes3 = MyDataCards.fetchValueDouble("TOF3pmtEnergyResolution");
  _timeRes1 = MyDataCards.fetchValueDouble("TOF1pmtTimeResolution");
  _timeRes2 = MyDataCards.fetchValueDouble("TOF2pmtTimeResolution");
  _timeRes3 = MyDataCards.fetchValueDouble("TOF3pmtTimeResolution");
  _adcFactor = MyDataCards.fetchValueDouble("TOFadcConversionFactor");
  _tdcFactor = MyDataCards.fetchValueDouble("TOFtdcConversionFactor");
  _adcBits = MyDataCards.fetchValueInt("TOFadcBits");
  _tdcBits = MyDataCards.fetchValueInt("TOFtdcBits");
  _attenuation1 = MyDataCards.fetchValueDouble("TOF1attenuationLength")*mm;
  _attenuation2 = MyDataCards.fetchValueDouble("TOF2attenuationLength")*mm;
  _attenuation3 = MyDataCards.fetchValueDouble("TOF3attenuationLength")*mm;
  _tof1size = MyDataCards.fetchValueDouble("TOF1Size")*mm;
  _tof2size = MyDataCards.fetchValueDouble("TOF2Size")*mm;
  _tof3size = MyDataCards.fetchValueDouble("TOF3Size")*mm;
  _decayConst1 = MyDataCards.fetchValueDouble("TOF1decayConstant");
  _decayConst2 = MyDataCards.fetchValueDouble("TOF2decayConstant");
  _decayConst3 = MyDataCards.fetchValueDouble("TOF3decayConstant");
  _startGateTime = MyDataCards.fetchValueDouble("TOFstartGateTime");
  _endGateTime = MyDataCards.fetchValueDouble("TOFendGateTime");
  _adcSaturationLimit = MyDataCards.fetchValueDouble("AdcSaturationLimit");
}

TofDigitizationParams* TofDigitizationParams::_instance =
         (TofDigitizationParams*) NULL;

TofDigitizationParams* TofDigitizationParams::getInstance()
{
  if(!_instance) _instance = new TofDigitizationParams;
  return _instance;
}

double TofDigitizationParams::getScintillatorConversionFactor(int ista) const
{
	if(ista == 1) return _conversionFactor1;
	else if(ista == 2) return _conversionFactor2;
  else  return _conversionFactor3;
}

double TofDigitizationParams::getEnergyResolution(int ista) const
{
	if( ista == 1) return _energyRes1;
	else if(ista == 2) return _energyRes2;
  else return _energyRes3;
}

double TofDigitizationParams::getTimeResolution(int ista) const
{
	if( ista == 1) return _timeRes1;
	else if( ista == 2) return _timeRes2;
  else return _timeRes3;
}

double TofDigitizationParams::getScintillatorDecayConst(int ista) const
{
	 if( ista == 1) return _decayConst1;
	 else if( ista == 2) return _decayConst2;
   else return  _decayConst3;
}

double TofDigitizationParams::getScintillatorRefractionIndex(int ista) const
{
	if( ista == 1) return _refractionIndex1;
	else if( ista == 2) return _refractionIndex2;
  else return _refractionIndex3;
}

double TofDigitizationParams::getAdcFactor() const {  return _adcFactor; }

double TofDigitizationParams::getTdcFactor() const {  return _tdcFactor; }

int TofDigitizationParams::getAdcBits() const { return _adcBits; }

int TofDigitizationParams::getTdcBits() const { return _tdcBits; }

double TofDigitizationParams::getAttenuationLength(int ista) const
{
	if( ista == 1) return _attenuation1;
	if( ista == 2) return _attenuation2;
	return  _attenuation3;
}

double TofDigitizationParams::getTofSize(int ista) const
{  if( ista == 1) return _tof1size;
   else if( ista == 2) return _tof2size;
   else if( ista == 3) return _tof3size;
   return _tof2size;
}

double TofDigitizationParams::getStartGateTime(int ista) const
{
  return _startGateTime;
}

double TofDigitizationParams::getEndGateTime(int ista) const
{
  return _endGateTime;
}

double TofDigitizationParams::getAdcSaturationLimit(int ista) const
{
  return _adcSaturationLimit;
}

