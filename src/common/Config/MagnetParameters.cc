// @(#) $Id: MagnetParameters.cc,v 1.12 2009-12-10 09:18:06 rogers Exp $  $Name:  $
//
//  MICE magnetic lattice parameters
//
//  Yagmur Torun

#include "Config/MagnetParameters.hh"
#include "Interface/dataCards.hh"

MagnetParameters::MagnetParameters()
{
  // Field map generation
  fSolDataFiles          = MyDataCards.fetchValueString("SolDataFiles");
  fNumberNodesZGrid      = MyDataCards.fetchValueInt("NumberNodesZGrid");
  fNumberNodesRGrid      = MyDataCards.fetchValueInt("NumberNodesRGrid");
  fSolzMapExtendFactor   = MyDataCards.fetchValueDouble("SolzMapExtendFactor");
  fSolrMapExtendFactor   = MyDataCards.fetchValueDouble("SolrMapExtendFactor");
  fFieldMode             = MyDataCards.fetchValueString("FieldMode");
  fdefaultNumberOfSheets = MyDataCards.fetchValueInt("DefaultNumberOfSheets");
  //Quadrupoles
  fQuadrupoleFringeMaxZFactor  = MyDataCards.fetchValueDouble("QuadrupoleFringeMaxZFactor");
  fQuadrupoleUseFringeFields   = MyDataCards.fetchValueInt("QuadrupoleUseFringeFields");
  fQuadrupoleFringeParameters  = MyDataCards.fetchValueVector("QuadrupoleFringeParameters");
  //Field grid
  fGridX                       = MyDataCards.fetchValueInt("FieldGridX");
  fGridY                       = MyDataCards.fetchValueInt("FieldGridY");
  fGridZ                       = MyDataCards.fetchValueInt("FieldGridZ");
}

MagnetParameters* MagnetParameters::fInstance = (MagnetParameters*) NULL;

MagnetParameters* MagnetParameters::getInstance()
{

  if(!fInstance) fInstance = new MagnetParameters;
  return fInstance;

}

