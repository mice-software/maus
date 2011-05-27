// @(#) $Id: BeamlineParameters.cc,v 1.3 2006-11-03 20:01:57 hart Exp $  $Name:  $
//
//  MICE beamline parameters
//
//  Tom Roberts

#include "Config/BeamlineParameters.hh"
#include "Interface/dataCards.hh"

BeamlineParameters::BeamlineParameters() {
  fBeamlineMaterial	= MyDataCards.fetchValueString("BeamlineMaterial");
  fQ35FieldLength	= MyDataCards.fetchValueDouble("Q35FieldLength");
  fQ35IronLength	= MyDataCards.fetchValueDouble("Q35IronLength");
  fQ35IronRadius	= MyDataCards.fetchValueDouble("Q35IronRadius");
  fQ35ApertureRadius	= MyDataCards.fetchValueDouble("Q35ApertureRadius");
  fQ35PoleTipRadius	= MyDataCards.fetchValueDouble("Q35PoleTipRadius");
  fQ35CoilRadius	= MyDataCards.fetchValueDouble("Q35CoilRadius");
  fQ35CoilHalfwidth	= MyDataCards.fetchValueDouble("Q35CoilHalfwidth");
  fQ35MaxStep		= MyDataCards.fetchValueDouble("Q35MaxStep");
  fQ35IronMaterial	= MyDataCards.fetchValueString("Q35IronMaterial");
  fQ35Kill		= MyDataCards.fetchValueInt("Q35Kill");
  fQ35Fringe		= MyDataCards.fetchValueString("Q35Fringe");
  fQ35FringeFactor	= MyDataCards.fetchValueDouble("Q35FringeFactor");
  fQ4Gradient		= MyDataCards.fetchValueDouble("Q4Gradient");
  fQ4Position		= MyDataCards.fetchValue3Vector("Q4Position");
  fQ5Gradient		= MyDataCards.fetchValueDouble("Q5Gradient");
  fQ5Position		= MyDataCards.fetchValue3Vector("Q5Position");
  fQ6Gradient		= MyDataCards.fetchValueDouble("Q6Gradient");
  fQ6Position		= MyDataCards.fetchValue3Vector("Q6Position");
  fQ7Gradient		= MyDataCards.fetchValueDouble("Q7Gradient");
  fQ7Position		= MyDataCards.fetchValue3Vector("Q7Position");
  fQ8Gradient		= MyDataCards.fetchValueDouble("Q8Gradient");
  fQ8Position		= MyDataCards.fetchValue3Vector("Q8Position");
  fQ9Gradient		= MyDataCards.fetchValueDouble("Q9Gradient");
  fQ9Position		= MyDataCards.fetchValue3Vector("Q9Position");
}

BeamlineParameters* BeamlineParameters::fInstance = (BeamlineParameters*) NULL;

BeamlineParameters* BeamlineParameters::getInstance()
{

  if(!fInstance) fInstance = new BeamlineParameters;
  return fInstance;

}

