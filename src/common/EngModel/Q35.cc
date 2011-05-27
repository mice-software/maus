//	Q35.cc

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Material.hh"

#ifdef G4VIS_USE
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#endif

#include "EngModel/Q35.hh"

Q35::Q35(
	G4Material * BeamlineMaterial,
	G4double IronLength,
	G4double IronRadius,
	G4double ApertureRadius,
	G4double PoleTipRadius,
	G4double CoilRadius,
	G4double CoilHalfwidth,
	G4Material * IronMaterial) :
	fBeamlineMaterial(BeamlineMaterial),
	fQ35IronLength(IronLength),
	fQ35IronRadius(IronRadius),
	fQ35ApertureRadius(ApertureRadius),
	fQ35PoleTipRadius(PoleTipRadius),
	fQ35CoilRadius(CoilRadius),
	fQ35CoilHalfwidth(CoilHalfwidth),
	fQ35IronMaterial(IronMaterial)
{
}

G4LogicalVolume *Q35::buildQ35()
{
	// because of potential problems sharing LogicalVolumes, generate
	// a new LogicalVolume each call

	G4LogicalVolume *lviron=0;
	// if ApertureRadius>0.0 then the aperture is a circle; if not rhen it
	// is a "rounded +".
	if(fQ35ApertureRadius > 0.0)
	{
		G4VSolid *ironSolid = new G4Tubs("Q35Iron",
				fQ35ApertureRadius,
				fQ35IronRadius,
				fQ35IronLength/2.0,0.0,360.0*deg);
		G4Material *mat = fQ35IronMaterial;
		lviron = new G4LogicalVolume(ironSolid,mat,"Q35IronLV");
	}
	else
	{
		G4VSolid *box=0;
		G4VSolid *ironSolid = new G4Tubs("Q35IronLV", 0.0,
				fQ35IronRadius,
				fQ35IronLength/2.0,0.0,360.0*deg);
		box = new G4Box("Q35a",fQ35CoilRadius,
				fQ35CoilRadius,
				fQ35IronLength/2.0+0.02*mm);
		// compute circle approximating the hyperbolic pole tip --
		// center is at (x,x) on the diagonal; (x0,y0) is the pole tip,
		// (x1,y1) is the intersection of pole face with coil.
		G4double x0=fQ35PoleTipRadius/sqrt(2.0);
		G4double y0=x0;
		G4double x1=fQ35CoilRadius;
		G4double y1=fQ35CoilHalfwidth;
		G4double x=0.5*(x1*x1+y1*y1-x0*x0-y0*y0)/(x1+y1-x0-y0);
		G4double r=sqrt(2.0)*x-fQ35PoleTipRadius;
		G4Tubs *t = new G4Tubs("xxx",0.0,r,
				fQ35IronLength/2.0+0.1,0.0,360.0*deg);
		// Subtract 4 copies of t from box, to get the "rounded +"
		G4ThreeVector offset(x,x,0.0);
		G4RotationMatrix norot;
		box = new G4SubtractionSolid("Q35b",box,t,&norot,offset);
		offset[0] = -x;
		box = new G4SubtractionSolid("Q35c",box,t,&norot,offset);
		offset[1] = -x;
		box = new G4SubtractionSolid("Q35d",box,t,&norot,offset);
		offset[0] = x;
		box = new G4SubtractionSolid("Q35e",box,t,&norot,offset);
		G4Material *mat = fQ35IronMaterial;
		lviron = new G4LogicalVolume(ironSolid,mat,"Q35IronLV");
		// place the "rounded +" as daughter in the iron Tubs; black.
		mat = fBeamlineMaterial;
		G4LogicalVolume *lvbox = new G4LogicalVolume(box,mat,"Q35eLV");
		G4ThreeVector no_offset(0.0,0.0,0.0);
		new G4PVPlacement(0,no_offset,lvbox,"Q35Aperture",lviron,false,0);
	}

	return lviron;
}

