/*
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
** modified by Rikard Sandstrom - Nov 7 2003
*/

#include <stdlib.h>
#include <stdio.h>
#include "TpgDigitsParameters.hh"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "Interface/dataCards.hh"

TpgDigitsParameters::TpgDigitsParameters()
:  fSensitive(true),fMixture(false)
{

   SetMixture(true);

  DefineTpgParameters();
  FillParameterVectors();
}

TpgDigitsParameters::~TpgDigitsParameters()
{
}

TpgDigitsParameters* TpgDigitsParameters::fInstance =
         (TpgDigitsParameters*) NULL;

TpgDigitsParameters* TpgDigitsParameters::getInstance()
{
  if(!fInstance) fInstance = new TpgDigitsParameters;
  return fInstance;
}

void TpgDigitsParameters::DefineTpgParameters()
{
  fDriftVelocity      = 5.5*CLHEP::cm/CLHEP::microsecond;

  fSigmaTrans         = 52*CLHEP::micrometer;

  fSigmaLong          = 375*CLHEP::micrometer;

  fEnergyPerPair      = 26.2*CLHEP::eV;

  fAmplificationMean   = 10000. ;

  fAmplificationSigma = 1000.;

  if(fSensitive)
  {
    fClustersPerCM       =
      MyDataCards.fetchValueDouble("TPGClustersPerCM")/CLHEP::cm;

    fElectronsPerClusterFile =
      MyDataCards.fetchValueString("TPGElectronsPerClusterFile");
    fElectronsPerCluster       =
      MyDataCards.fetchValueDouble("TPGElectronsPerCluster");

    fDriftVelocity      =
       MyDataCards.fetchValueDouble("TPGDriftVelocity")*CLHEP::cm/CLHEP::microsecond;

    fSigmaTrans         =
       MyDataCards.fetchValueDouble("TPGSigmaTrans")*CLHEP::mm; // per sqrt[cm]
    fSigmaLong          =
       MyDataCards.fetchValueDouble("TPGSigmaLong")*CLHEP::mm;  // per sqrt[cm]

    fEnergyPerPair      =
       MyDataCards.fetchValueDouble("TPGEnergyPerPair")*CLHEP::eV;

    fAmplificationMean   =
       MyDataCards.fetchValueDouble("TPGAmplificationMean");
    fAmplificationSigma =
       MyDataCards.fetchValueDouble("TPGAmplificationSigma");

    fDigitThreshold =
      MyDataCards.fetchValueInt("TPGDigitThreshold");

    fGEMSpreadSigma =
      MyDataCards.fetchValueDouble("TPGGEMSpreadSigma")*CLHEP::mm;

    fElectronicsDecayTime =
      MyDataCards.fetchValueDouble("TPGElectronicsDecayTime")*CLHEP::ns;

    fSamplingPeriod =
      MyDataCards.fetchValueDouble("TPGSamplingPeriod")*CLHEP::ns;

    fStripSpace         = MyDataCards.fetchValueDouble("TPGStripSpacing")*CLHEP::mm;
    fStripsPerDir       = MyDataCards.fetchValueInt("TPGStripsPerDir");

    fDeadStripFraction  = MyDataCards.fetchValueDouble("TPGDeadStripFraction");
    fDeadStripFile      = MyDataCards.fetchValueString("TPGDeadStripFile");

    fAdcCountsPerfC  =
      MyDataCards.fetchValueDouble("TPGAdcCountsPerfC")/(CLHEP::coulomb*1e-15);

    fNoiseAdcSigma   = MyDataCards.fetchValueDouble("TPGNoiseAdcSigma");
  }

}

void TpgDigitsParameters::FillParameterVectors()
{
  // Read electron cluster file
  std::string TPGElectronFile = GetElectronsPerClusterFile();
  std::ifstream fileElectrons(TPGElectronFile.c_str());

  bool useHisto = (GetElectronsPerClusterFile() != "None");
  bool fileOpen = (fileElectrons.is_open());

  if ((useHisto) && (!fileOpen)){
    cout << "Couldn't open electron spectrum file " << TPGElectronFile << endl;
    cout << "Using 1/n^2 behaviour instead" << endl;
    useHisto = false;
  }

  double electrons = 0;
  double sum = 0; // cum. sum of probabilities
  if (useHisto) {
    cout << "Reading TPG cluster size probabilities from " << TPGElectronFile << endl;
    while (!fileElectrons.eof()) {
      fileElectrons >> electrons;
      sum += electrons;
      fElectronVector.push_back(sum);
    }
    fileElectrons.close();
  } else {
    cout << "Approximating TPG cluster size probabilities with P(n)=1/n^2" << endl;
    for (int j = 1; j <= 200; j++) {  // cut at 200 electrons!
      electrons = 1/pow(j,2);
      sum += electrons;
      fElectronVector.push_back(sum);
    }
  }
  int size = fElectronVector.size();
  for (int i = 0; i < size; i++) {
    if (sum) fElectronVector[i] = fElectronVector[i]/sum; // normalize
  }

  //Assign dead strips
  int stripNr, layerNr, detNr, indexedStripNr;
  fDeadStrip3D = C3DVector<bool>(2,3,fStripsPerDir);
  bool useDeadStripMap = (fDeadStripFile != "None");
  if (useDeadStripMap){
    std::ifstream fileDeadStrip(fDeadStripFile.c_str());
    if (!fileDeadStrip.is_open()) {
      cout << "Couldn't open map file of dead strips " << fDeadStripFile << endl;
      useDeadStripMap = false;
      if (fDeadStripFraction != 0) cout << "Using random dead strips instead" << endl;
    } else {
      cout << "Reading dead TPG strips from " << fDeadStripFile << endl;
      while (!fileDeadStrip.eof()) {
        fileDeadStrip >> detNr >> layerNr >> stripNr;
        indexedStripNr = GetIndexedStripNr(stripNr);
        fDeadStrip3D.SetAt(detNr, layerNr, indexedStripNr, true);
      }
    }
        fileDeadStrip.close();
  }
  if ((useDeadStripMap == false) && (fDeadStripFraction != 0)){
    //Assign dead strips randomly
    cout << fDeadStripFile << " will be created and contain generated values." <<  endl;
    std::ofstream fileDeadStrip(fDeadStripFile.c_str());
    int deadStrips = (int)floor(fDeadStripFraction*3*fStripsPerDir);
    for (int i = 0; i < deadStrips; i++){
      stripNr = (int)floor(fStripsPerDir*(CLHEP::RandFlat::shoot()-0.5));
      layerNr = (int)floor(3*(CLHEP::RandFlat::shoot()));
      detNr = (int)floor(2*(CLHEP::RandFlat::shoot()));
      indexedStripNr = GetIndexedStripNr(stripNr);
      fDeadStrip3D.SetAt(detNr, layerNr, indexedStripNr, true);
      // Create a file with the randomly assigned dead strips
      fileDeadStrip << detNr << "\t" << layerNr << "\t" << stripNr << endl;
    }
    fileDeadStrip.close();
  }
}

bool TpgDigitsParameters::isDeadStrip(int detNr, int layerNr, int stripNr)
{
  int indexedStripNr = GetIndexedStripNr(stripNr);
  return fDeadStrip3D.GetAt(detNr, layerNr, indexedStripNr);
}





