#ifndef _TOFDIGITIZATIONPARAMS_HH_
#define _TOFDIGITIZATIONPARAMS_HH_   1
//----------------------------------------------------------------------------
//
//     File :      TofDigitizationParams.hh
//     Purpose :   Supply parameters for the TOF digitization
//     Created :   23-Oct-2002   by Steve Kahn
//
//----------------------------------------------------------------------------

#include <iostream>

class TofDigitizationParams
{
  public:
    double   getScintillatorConversionFactor(int) const;
    double   getEnergyResolution(int) const;
    double   getTimeResolution(int=0) const;
    double   getScintillatorDecayConst(int) const;
    double   getScintillatorRefractionIndex(int) const;
    double   getAdcFactor() const;
    int      getAdcBits() const;
    double   getTdcFactor() const;
    int      getTdcBits() const;
    double   getAttenuationLength(int) const;
//    double   getTofWidth(int) const;
    double   getTofSize(int) const;
    double   getStartGateTime(int=0) const;
    double   getEndGateTime(int=0) const;
    double   getAdcSaturationLimit(int=0) const;
    static TofDigitizationParams*  getInstance();
    TofDigitizationParams();
    ~TofDigitizationParams() {}
    friend std::ostream& operator <<(std::ostream&, const
				     TofDigitizationParams&);

  private:
    double   _conversionFactor1, _conversionFactor2, _conversionFactor3; // convert edep to nPE
    double   _energyRes1, _energyRes2, _energyRes3; // energy resolution assoc to PMT
    double   _timeRes1, _timeRes2, _timeRes3;  // time resolution of PMT
    double   _adcFactor;  // convert nPE to ADC signal
    double   _tdcFactor;  // convert time to TDC
    double   _refractionIndex1, _refractionIndex2, _refractionIndex3;
    double   _decayConst1, _decayConst2, _decayConst3;  // scintillator decay const
    double   _attenuation1, _attenuation2, _attenuation3; // attenuation length
    double   _tof1size, _tof2size, _tof3size; // transverse width
    double   _startGateTime;   //  start time for Tof data taking
    double   _endGateTime; //end time for Tof data taking
    int      _adcBits;  // number of ADC bits
    int      _tdcBits;  // number of TDC bits
    double   _adcSaturationLimit; //maximum charge on the adc
    static TofDigitizationParams* _instance;
};
#endif
