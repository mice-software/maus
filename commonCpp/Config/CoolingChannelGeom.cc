// @(#) $Id: CoolingChannelGeom.cc,v 1.8 2006-11-03 20:01:57 hart Exp $  $Name:  $
//
//  MICE magnetic lattice geometry
//
//  Yagmur Torun

#include "CoolingChannelGeom.hh"
#include "Interface/dataCards.hh"

CoolingChannelGeom::CoolingChannelGeom()
{
  miceMemory.addNew( Memory::CoolingChannelGeom ); 
  //Stepping parameters
  fDeltaOneStep          = MyDataCards.fetchValueDouble("DeltaOneStep");
	fDeltaIntersection     = MyDataCards.fetchValueDouble("DeltaIntersection");
	fEpsilonMin            = MyDataCards.fetchValueDouble("EpsilonMin");
	fEpsilonMax            = MyDataCards.fetchValueDouble("EpsilonMax");
	fMissDistance          = MyDataCards.fetchValueDouble("MissDistance");
	fFieldTrackStepper     = MyDataCards.fetchValueString("FieldTrackStepper");
  fMaxStepSizeInChannel  = MyDataCards.fetchValueDouble("MaxStepSizeInChannel");
}

CoolingChannelGeom* CoolingChannelGeom::fInstance = (CoolingChannelGeom*) NULL;

CoolingChannelGeom* CoolingChannelGeom::getInstance()
{

  if(!fInstance) fInstance = new CoolingChannelGeom;
  return fInstance;

}

