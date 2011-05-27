// @(#) $Id: MagnetParameters.hh,v 1.12 2009-12-10 09:18:06 rogers Exp $  $Name:  $
//
//  MICE magnetic lattice parameters
//
//  Yagmur Torun

#ifndef SFOFOMAGNETPARAMETERS_H
#define SFOFOMAGNETPARAMETERS_H 1

#include <string>
#include <vector>
#include "CLHEP/Units/SystemOfUnits.h"

class MagnetParameters {

  // Field map generation
  std::string  fSolDataFiles;
  std::string  fMagneticFieldMap;
  std::string  fMagneticFieldMapFile;
  std::string  fFieldMode;
  int          fdefaultNumberOfSheets;
  double  fMagneticFieldMapZ1;
  double  fMagneticFieldMapZ2;
  double  fMagneticFieldMapdZ;
  double  fMagneticFieldMapR1;
  double  fMagneticFieldMapR2;
  double  fMagneticFieldMapdR;

  double  fQuadrupoleFringeMaxZFactor;
  double  fQuadrupoleUseFringeFields;
  std::vector<double>  fQuadrupoleFringeParameters;

  int     fFieldPolarity;
  int     fFieldFlipOption;
  double  fConstantFieldBz;
//  bool    fMakeVolumeSheet;
  int     fNumberNodesZGrid;
  int     fNumberNodesRGrid;
  double  fSolzMapExtendFactor;
  double  fSolrMapExtendFactor;

  // Coil definitions
  double  fFocusCoilRadius;
  double  fFocusCoilThickness;
  double  fFocusCoilLength;
  double  fFocusCoilCurrent;
  int     fFocusCoilNumSheet;

  double  fCouplingCoilRadius;
  double  fCouplingCoilThickness;
  double  fCouplingCoilLength;
  double  fCouplingCoilCurrent;
  int     fCouplingCoilNumSheet;

  double  fMatchingCoilRadius;
  double  fMatchingCoilThickness;
  double  fMatchingCoil2Thickness;
  double  fMatchingCoilLength;
  double  fMatchingCoilCurrent;
  double  fMatchingCoil2Current;
  int     fMatchingCoilNumSheet;
  int     fMatchingCoil2NumSheet;

  double  fEndCoilRadius;
  double  fEndCoil1Thickness;
  double  fEndCoil2Thickness;
  double  fEndCoilLength;
  double  fEndCoil1Current;
  double  fEndCoil2Current;
  int     fEndCoil1NumSheet;
  int     fEndCoil2NumSheet;

  double  fSolenoidRadius;
  double  fSolenoidThickness;
  double  fSolenoidLength;
  double  fSolenoidCurrent;
  int     fSolenoidNumSheet;
  std::string  fRunMode;

  int fGridX;
  int fGridY;
  int fGridZ;

  static MagnetParameters* fInstance;

public:

  static MagnetParameters* getInstance();
  MagnetParameters();
  ~MagnetParameters(){};
  
  std::string  SolDataFiles() { return fSolDataFiles; }
  std::string  MagneticFieldMap() { return fMagneticFieldMap; }
  std::string  MagneticFieldMapFile() { return fMagneticFieldMapFile; }
  std::string  FieldMode() {return fFieldMode;}
  int     NumberNodesZGrid() { return fNumberNodesZGrid; }
  int     NumberNodesRGrid() { return fNumberNodesRGrid; }
  int     DefaultNumberOfSheets() { return fdefaultNumberOfSheets; }

  double  QuadrupoleFringeMaxZFactor() { return fQuadrupoleFringeMaxZFactor;}
  double  QuadrupoleUseFringeFields()  { return fQuadrupoleUseFringeFields;}
  std::vector<double>  QuadrupoleFringeParameters() { return fQuadrupoleFringeParameters;}

	//NOTE ALL THESE METHODS ARE DISABLED AND INCLUDED ONLY FOR BACKWARDS COMPATIBILITY
  double  MagneticFieldMapZ1() { return fMagneticFieldMapZ1; }
  double  MagneticFieldMapZ2() { return fMagneticFieldMapZ2; }
  double  MagneticFieldMapdZ() { return fMagneticFieldMapdZ; }
  double  MagneticFieldMapR1() { return fMagneticFieldMapR1; }
  double  MagneticFieldMapR2() { return fMagneticFieldMapR2; }
  double  MagneticFieldMapdR() { return fMagneticFieldMapdR; }
  int     FieldPolarity() { return fFieldPolarity; }
  int     FieldFlipOption() { return fFieldFlipOption; }
  double  ConstantFieldBz() { return fConstantFieldBz; }
//  bool    MakeVolumeSheet() { return fMakeVolumeSheet; }
  double  SolzMapExtendFactor() { return fSolzMapExtendFactor; }
  double  SolrMapExtendFactor() { return fSolrMapExtendFactor; }
  double  FocusCoilRadius() { return fFocusCoilRadius; }
  double  FocusCoilThickness() { return fFocusCoilThickness; }
  double  FocusCoilLength() { return fFocusCoilLength; }
  double  FocusCoilCurrent() { return fFocusCoilCurrent; }
  int     FocusCoilNumSheet() { return fFocusCoilNumSheet; }
  double  CouplingCoilRadius() { return fCouplingCoilRadius; }
  double  CouplingCoilThickness() { return fCouplingCoilThickness; }
  double  CouplingCoilLength() { return fCouplingCoilLength; }
  double  CouplingCoilCurrent() { return fCouplingCoilCurrent; }
  int     CouplingCoilNumSheet() { return fCouplingCoilNumSheet; }
  double  MatchingCoilRadius() { return fMatchingCoilRadius; }
  double  MatchingCoilThickness() { return fMatchingCoilThickness; }
  double  MatchingCoil2Thickness() { return fMatchingCoil2Thickness; }
  double  MatchingCoilLength() { return fMatchingCoilLength; }
  double  MatchingCoilCurrent() { return fMatchingCoilCurrent; }
  double  MatchingCoil2Current() { return fMatchingCoil2Current; }
  int     MatchingCoilNumSheet() { return fMatchingCoilNumSheet; }
  int     MatchingCoil2NumSheet() { return fMatchingCoil2NumSheet; }
  double  EndCoilRadius() { return fEndCoilRadius; }
  double  EndCoil1Thickness() { return fEndCoil1Thickness; }
  double  EndCoil2Thickness() { return fEndCoil2Thickness; }
  double  EndCoilLength() { return fEndCoilLength; }
  double  EndCoil1Current() { return fEndCoil1Current; }
  double  EndCoil2Current() { return fEndCoil2Current; }
  int     EndCoil1NumSheet() { return fEndCoil1NumSheet; }
  int     EndCoil2NumSheet() { return fEndCoil2NumSheet; }
  double  SolenoidRadius() { return fSolenoidRadius; }
  double  SolenoidThickness() { return fSolenoidThickness; }
  double  SolenoidLength() { return fSolenoidLength; }
  double  SolenoidCurrent() { return fSolenoidCurrent; }
  int     SolenoidNumSheet() { return fSolenoidNumSheet; }

  int     FieldGridX() { return fGridX; };
  int     FieldGridY() { return fGridY; };
  int     FieldGridZ() { return fGridZ; };

  std::string  RunMode() {return fRunMode; }

};

#endif
