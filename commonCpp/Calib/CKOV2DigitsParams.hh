#ifndef _CKOV2DIGITSPARAMS_HH_
#define _CKOV2DIGITSPARAMS_HH_ 1
//-----------------------------------------------------------------------
//
//    File :      CKOV2DigitsParams.hh
//    Purpose :   Manage Parameters for the CKOV2 digitization.
//    Created :   22-MAR-2003  by Steve Kahn
//
//-----------------------------------------------------------------------

class CKOV2DigitsParams
{
  private:
    double _refractiveIndex;
    double _criticalAngle;
    double _pmtQuantumEff;
    double _adcConversionFactor;
    double _tdcConversionFactor;
    double _conversionFactor;
    int    _adcBits, _tdcBits;
    int    _numberPmts;
    double _pmtEnergyResolution;
    double _pmtTimeResolution;
    double _radiatorThickness;
    double _totalThickness;
    double  _xpmt, _ypmt;
    double  _rpmt;
    double  _mirrorReflectivity;
    static CKOV2DigitsParams*  _instance;

  public:
     CKOV2DigitsParams();
     static CKOV2DigitsParams*  getInstance();

     double getRefractiveIndex() const {  return _refractiveIndex; }
     double getCriticalAngle() const   {  return _criticalAngle;  }
     double getPmtQuantumEff() const   {  return _pmtQuantumEff;  }
     double getAdcConversionFactor() const  {  return _adcConversionFactor; }
     double getTdcConversionFactor() const  {  return _tdcConversionFactor; }
     double getEnergyRes() const      {  return _pmtEnergyResolution; }
     double getTimeRes() const        {  return _pmtTimeResolution;  }
     double getPEconversionFactor() const   { return _conversionFactor; }
     double getRadiatorThickness() const { return _radiatorThickness; }
     double getTotalThickness() const {  return _totalThickness; }
     double getXctrPmt() const { return  _xpmt; }
     double getYctrPmt() const {  return  _ypmt; }
     double getRadiusPmt() const {  return  _rpmt; }
     double getMirrorReflectivity() const {  return _mirrorReflectivity; } //ME - added return

     int  getAdcBits() const  {  return  _adcBits;  }
     int  getTdcBits() const  {  return  _tdcBits;  }
     int  getNumberPmts() const {  return  _numberPmts; }
};
#endif
