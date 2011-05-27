// @(#) $Id: RFBackgroundParameters.cc,v 1.11 2006-11-03 20:01:57 hart Exp $  $Name:  $
//
//  MICE RF parameters
//
//  V. Grichine 22.11.02

#include "Config/RFBackgroundParameters.hh"
#include "Interface/dataCards.hh"


///////////////////////////////////////////////////////////////////////

RFBackgroundParameters::RFBackgroundParameters()
{
  SetRFBackgroundParameters();
}


///////////////////////////////////////////////////////////////////////

void RFBackgroundParameters::SetRFBackgroundParameters()
{
  fBGSource            = MyDataCards.fetchValueString("rfBGSource");
  fBGElectronModel     = MyDataCards.fetchValueString("rfBGElectronModel");
  fBGPhotonModel       = MyDataCards.fetchValueString("rfBGPhotonModel");
  fBGEmitterMode       = MyDataCards.fetchValueString("rfBGEmitterMode");
  fBGMode              = MyDataCards.fetchValueString("rfBGMode");
  fBGNumberOfEmitters  = MyDataCards.fetchValueInt("rfBGNumberOfEmitters");
  fBGNumberOfElectrons = MyDataCards.fetchValueInt("rfBGNumberOfElectrons");
  fBGNumberOfPhotons   = MyDataCards.fetchValueInt("rfBGNumberOfPhotons");
  fBGDirection         = MyDataCards.fetchValueInt("rfBGDirection");
  fBGElectronEnergy    = MyDataCards.fetchValueDouble("rfBGElectronEnergy");
  fBGPhotonEnergy      = MyDataCards.fetchValueDouble("rfBGPhotonEnergy");
  fBGElectronEnergyWidth = MyDataCards.fetchValueDouble("rfBGElectronEnergyWidth");
  fBGPhotonEnergyWidth = MyDataCards.fetchValueDouble("rfBGPhotonEnergyWidth");
  fBGElectronSpectrum  = MyDataCards.fetchValueString("rfBGElectronSpectrum");
  fBGPhotonSpectrum    = MyDataCards.fetchValueString("rfBGPhotonSpectrum");
  fBGElectronSpectrumFile = MyDataCards.fetchValueString("rfBGElectronSpectrumFile");
  fBGPhotonSpectrumFile = MyDataCards.fetchValueString("rfBGPhotonSpectrumFile");
  std::string bgMultipleSpectraFile = MyDataCards.fetchValueString("rfBGMultipleSpectraFile");
  fBGBankFile          = MyDataCards.fetchValueString("rfBGBankFile");
  fBGTimeWindow        = MyDataCards.fetchValueDouble("rfBGTimeWindow");
  fBGTimeDelay         = MyDataCards.fetchValueDouble("rfBGTimeDelay");
  fBGZstart            = MyDataCards.fetchValueDouble("rfBGZstart");
  fBGRadius            = MyDataCards.fetchValueDouble("rfBGRadius");
  fBGSamplingMode      = MyDataCards.fetchValueString("rfBGSamplingMode");
  fBGScaleFactor       = MyDataCards.fetchValueDouble("rfBGScaleFactor");

  // first get the environment variable MICEFILES as this is where the model files are kept
  char* MICEFILES = getenv( "MICEFILES" );

  // the full name of the input file:
  fBGMultipleSpectraFile = std::string( MICEFILES ) + "/Models/Modules/RFCC/" + bgMultipleSpectraFile;

}

RFBackgroundParameters* RFBackgroundParameters::fInstance = (RFBackgroundParameters*) NULL;

RFBackgroundParameters* RFBackgroundParameters::getInstance()
{

  if(!fInstance) fInstance = new RFBackgroundParameters;
  return fInstance;

}

