// @(#) $Id: RFBackgroundParameters.hh,v 1.9 2006-11-03 20:01:57 hart Exp $  $Name:  $
//
//  MICE RF parameters
//  Created:
//  22.01.02 V. Grichine based on proposals of Y. Torun 

#ifndef RFBackgroundParaMETERS_H
#define RFBackgroundParaMETERS_H 1

#include <string>
#include "CLHEP/Units/SystemOfUnits.h"

class RFBackgroundParameters 
{
public:

  static RFBackgroundParameters* getInstance();
  RFBackgroundParameters();

 ~RFBackgroundParameters();
  void SetRFBackgroundParameters();

  std::string  GetBGSource()                  { return fBGSource; }
  void SetBGSource(std::string source)        { fBGSource = source; }
  
  std::string  GetBGElectronModel()           { return fBGElectronModel; }
  void SetBGElectronModel(std::string model)  { fBGElectronModel = model; }

  std::string  GetBGPhotonModel()             { return fBGPhotonModel; }
  void SetBGPhotonModel(std::string model)    { fBGPhotonModel = model; }

  std::string  GetBGEmitterMode()             { return fBGEmitterMode; }
  void SetBGEmitterMode(std::string mode)     { fBGEmitterMode = mode; }
  
  std::string  GetBGSamplingMode()            { return fBGSamplingMode; }
  void SetBGSamplingMode(std::string mode)    { fBGSamplingMode = mode; }
  
  std::string  GetBGMode()                     { return fBGMode; }
  void SetBGMode(std::string s)                { fBGMode = s; }

  int     GetBGNumberOfEmitters()        { return fBGNumberOfEmitters; }
  void SetBGNumberOfEmitters(int n)      { fBGNumberOfEmitters = n; }

  int     GetBGNumberOfElectrons()       { return fBGNumberOfElectrons; }
  void SetBGNumberOfElectrons(int n)     { fBGNumberOfElectrons = n; }

  int     GetBGNumberOfPhotons()         { return fBGNumberOfPhotons; }
  void SetBGNumberOfPhotons(int n)       { fBGNumberOfPhotons = n; }

  int     GetBGDirection()               { return fBGDirection; }
  void SetBGDirection(int d)             { fBGDirection = d; }

  double  GetBGTimeWindow()              { return fBGTimeWindow; }
  void SetBGTimeWindow(double t)         { fBGTimeWindow = t; }

  double  GetBGTimeDelay()               { return fBGTimeDelay; }
  void SetBGTimeDelay(double t)          { fBGTimeDelay = t; }

  double  GetBGElectronEnergy()          { return fBGElectronEnergy; }
  void SetBGElectronEnergy(double e)     { fBGElectronEnergy = e; }

  double  GetBGPhotonEnergy()            { return fBGPhotonEnergy; }
  void SetBGPhotonEnergy(double e)       { fBGPhotonEnergy = e; }

  double  GetBGElectronEnergyWidth()     { return fBGElectronEnergyWidth; }
  void SetBGElectronEnergyWidth(double w){ fBGElectronEnergyWidth = w; }

  double  GetBGPhotonEnergyWidth()       { return fBGPhotonEnergyWidth; }
  void SetBGPhotonEnergyWidth(double w)  { fBGPhotonEnergyWidth = w; }

  std::string  GetBGElectronSpectrum()        { return fBGElectronSpectrum; }
  void SetBGElectronSpectrum(std::string s)   { fBGElectronSpectrum = s;}

  std::string  GetBGPhotonSpectrum()          { return fBGPhotonSpectrum; }
  void SetBGPhotonSpectrum(std::string s)     { fBGPhotonSpectrum = s; }

  std::string  GetBGElectronSpectrumFile()    { return fBGElectronSpectrumFile; }
  void SetBGElectronSpectrumFile(std::string f){fBGElectronSpectrumFile = f; }
  
  std::string GetBGPhotonSpectrumFile()       { return fBGPhotonSpectrumFile; }
  void SetBGPhotonSpectrumFile(std::string f) { fBGPhotonSpectrumFile = f; }

  std::string GetBGMultipleSpectraFile()       { return fBGMultipleSpectraFile; }
  void SetBGMultipleSpectraFile(std::string f) { fBGMultipleSpectraFile = f; }
  
  std::string GetBGBankFile()                  { return fBGBankFile; }
  void SetBGBankFile(std::string f)            { fBGBankFile = f; }
  
  double  GetBGZstart()                   { return fBGZstart; }
  void SetBGZstart(double z)              { fBGZstart = z; }

  double  GetBGRadius()                   { return fBGRadius; }
  void SetBGRadius(double r)              { fBGRadius = r; }

  double  GetBGScaleFactor()              { return fBGScaleFactor; }
  void SetBGScaleFactor(double s)         { fBGScaleFactor = s; }
  
private:

  std::string  fBGSource;
  std::string  fBGElectronModel;
  std::string  fBGPhotonModel;
  std::string  fBGEmitterMode;
  std::string  fBGSamplingMode;
  std::string  fBGMode;

  int     fBGNumberOfEmitters;
  int     fBGNumberOfElectrons;
  int     fBGNumberOfPhotons;
  int     fBGDirection;

  double  fBGElectronEnergy;
  double  fBGPhotonEnergy;
  double  fBGElectronEnergyWidth;
  double  fBGPhotonEnergyWidth;
  std::string  fBGElectronSpectrum;
  std::string  fBGPhotonSpectrum;
  std::string  fBGElectronSpectrumFile;
  std::string  fBGPhotonSpectrumFile;
  std::string  fBGMultipleSpectraFile;
  std::string  fBGBankFile;
 
  double  fBGTimeWindow;
  double  fBGTimeDelay;

  double  fBGZstart;
  double  fBGRadius;
  
  double  fBGScaleFactor;

  static RFBackgroundParameters* fInstance;
};

#endif
