// @(#) $Id: BeamlineGeometry.cc,v 1.2 2006-11-03 20:01:57 hart Exp $  $Name:  $
//
//  MICE beamline geometry parameters
//
//  Yagmur Torun

#include "Config/BeamlineGeometry.hh"
#include "Interface/dataCards.hh"

BeamlineGeometry::BeamlineGeometry() {
  fBeamlineMaterial	= MyDataCards.fetchValueString("BeamlineMaterial");
  fQ4Position		= MyDataCards.fetchValue3Vector("Q4Position");
  fQ5Position		= MyDataCards.fetchValue3Vector("Q5Position");
  fQ6Position		= MyDataCards.fetchValue3Vector("Q6Position");
  fQ7Position		= MyDataCards.fetchValue3Vector("Q7Position");
  fQ8Position		= MyDataCards.fetchValue3Vector("Q8Position");
  fQ9Position		= MyDataCards.fetchValue3Vector("Q9Position");
}

BeamlineGeometry* BeamlineGeometry::fInstance = (BeamlineGeometry*) NULL;

BeamlineGeometry* BeamlineGeometry::getInstance()
{

  if(!fInstance) fInstance = new BeamlineGeometry;
  return fInstance;

}

