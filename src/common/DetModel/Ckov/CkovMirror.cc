/*
 * Conical Mirror for Upstream Cherenckov
 * 
 * (2007/01/11) Hideyuki Sakamoto
*/ 
#include "DetModel/Ckov/CkovMirror.hh"

CkovMirror::~CkovMirror()
{
	// Rotation Matrix
	delete fRot045;
	delete fRot090;
	delete fRot180;
	delete fRotcon;

	// Cutters
	delete fPmts;
	delete fSide;
	delete fGate;
/* 
	// Elliptical Cone
	delete fCone;
  	delete fConA;
	delete fConB;
*/
	// Simple Cone
	delete fCon2;
	delete fCon3;
	delete fCon4;

	// Solid, Logical and Physical Volumes
	delete fSolid;
	delete fLogic;
	delete fPlace;
	delete MirrorSurface;
	delete fOpticalSurface;
}

CkovMirror::CkovMirror( MiceModule* mod, G4Material* mater, G4VPhysicalVolume *mlv )
{
	// Calculate vertecies of cone
	calVertex(mod);

	// Cutters used for Boolean operation
	fPmts = new G4Box("Pmts", fDxOfPmts, fDyOfPmts, fDzOfPmts);
	fSide = new G4Box("Side", fDxOfSide, fDyOfSide, fDzOfSide);
	fGate = new G4Box("Gate", fDxOfGate, fDyOfGate, fDzOfGate);
/*
	// Elliptical Cone
	fConA = new G4EllipticalCone("OuterCone", fXSemiAxis, fYSemiAxis, fZMax, fZTopCut);
	fConB = new G4EllipticalCone("InnerCone", fXSemiAxis, fYSemiAxis, fZMax, fZTopCut);
	fEcon = new G4SubtractionSolid("Cone", fConA, fConB, 0, fPosOfEcon);
*/
	// Simple Cone as a alternative
	fCone = new G4Cons("Cone", fRmin1, fRmax1, fRmin2, fRmax2, fDz, fSPhi, fDPhi);

	// Adding four Cones
	fCon1 = new G4IntersectionSolid("Con1", fPmts, fCone, fRotcon, fPosOfCone);
	fCon2 = new G4IntersectionSolid("Con2", fCon1, fSide, fRot045, fPosOfSide);
	fCon3 = new G4UnionSolid("Con3", fCon2, fCon2, fRot180, fZero);
   fCon4 = new G4UnionSolid("Con4", fCon3, fCon3, fRot090, fZero);

	// Solid, Logical and Physical Volumes
	fSolid = new G4SubtractionSolid(mod->name(), fCon4, fGate, 0, fPosOfGate);
   fLogic = new G4LogicalVolume(fSolid, mater, mod->name(), 0, 0, 0);
	fPlace = new G4PVPlacement(0, fPosOfZero, fLogic, mod->name(), mlv->GetLogicalVolume(), 0, 0);

	fOpticalSurface = new G4OpticalSurface("MirrorSurface");
	fOpticalSurface->SetType(dielectric_dielectric);
	fOpticalSurface->SetFinish(polishedfrontpainted);
	fOpticalSurface->SetModel(glisur);
	fOpticalSurface->SetPolish(1.);

	MirrorSurface = new G4LogicalSkinSurface("MirrorSurface",fLogic,fOpticalSurface);
	G4MaterialPropertiesTable* MPT = new G4MaterialPropertiesTable();

	const G4int num = 2;
	G4double Ephoton[num] = {0.5*eV, 5.0*eV};
	G4double Reflectivity[num] = {1.0, 1.0};
	G4double Efficiency[num]   = {0.0, 0.0};

	MPT->AddProperty("REFLECTIVITY", Ephoton, Reflectivity, num);
	MPT->AddProperty("EFFICIENCY",   Ephoton, Efficiency,   num);

	fOpticalSurface->SetMaterialPropertiesTable(MPT);
}

void CkovMirror::calVertex( MiceModule* mod )
{
	// Read from data card.
	const G4double thick = mod->propertyDouble( "Thickness[mm]" );
	const G4double dPmt  = mod->propertyDouble( "DiameterOfPmt[mm]" );
	const G4double dWin  = mod->propertyDouble( "DiameterOfGate[mm]" );
	const G4double xpmt = mod->propertyDouble( "XoffsetOfPmt[mm]" );
	const G4double zpmt = mod->propertyDouble( "ZoffsetOfPmt[mm]" );

	// Positions of Pmt and Gate
   G4ThreeVector P(xpmt * mm, 0., zpmt * mm);
   G4ThreeVector W(0., 0., 0.);
	G4double DW = dWin*mm;
	G4double DP = dPmt*mm;
   G4ThreeVector WU = W;WU.setX(W.getX()+DW/2.);
   G4ThreeVector WD = W;WD.setX(W.getX()-DW/2.);
   G4ThreeVector PR = P;PR.setZ(P.getZ()+DP/2.);
   G4ThreeVector PL = P;PL.setZ(P.getZ()-DP/2.);

	// Apex of Cone
	G4ThreeVector A = getCross(WU/*x1*/,PL/*x2*/,WD/*x3*/,PR/*x4*/);

	// Slopes of Cone 
	G4double S1 = getLength(A,WD);
	G4double S2 = getLength(A,WU);

	// Edges of Cone
	G4ThreeVector E(A.getX()*(1-S1/S2)+DW/2.*S1/S2,0.,A.getZ()*(1-S1/S2));

	// Center at Bottom of Cone
	G4ThreeVector C = getCenter(WD,E);
	
	// Center Position of Cone
	G4ThreeVector CP = getCenter(A,C);
	
	// Point at Z=0
	G4ThreeVector Z0(A.getX()-A.getZ()*(A.getX()-C.getX())/(A.getZ()-C.getZ()),0.,0.);
	
	// Height of Cone
	G4double H = getLength(A,C);

	// Middle Height of Cone
	//ME unused G4double MH = getLength(A,Z0);
	
	// Tilt of Cone
	G4double T = getAngle(A,C);
	
	// (Half of) Short axis of Cone
	G4double S = getLength(WD,C);
	
	// (Half of) Long axis of Cone
	//ME unused G4double L = H/MH*std::sqrt(std::pow(DW/2.,2.)-std::pow(Z0.getX(),2.));

	// Center Position of Inside Cone(ConeB)
	G4ThreeVector kk = CP-C; 
	kk = kk.rotateY(-T*deg); 
	kk.setZ(kk.getZ()-thick); 
	kk.rotateY(+T*deg);
	G4ThreeVector CP2 = kk+C;

	
//	Check the results
#ifdef DEBUG_CKOV 
	std::cerr << "### Vertices in cherenkov mirror ###" << std::endl;
	std::cerr << "W "     << W    << std::endl;
	std::cerr << "DW "    << DW   << std::endl;
	std::cerr << "WU "    << WU   << std::endl;
	std::cerr << "WD "    << WD   << std::endl;
	std::cerr << "P "     << P    << std::endl;
	std::cerr << "DP "    << DP   << std::endl;
	std::cerr << "PL "    << PL   << std::endl;
	std::cerr << "PR "    << PR   << std::endl;
	std::cerr << "A "     << A    << std::endl;
	std::cerr << "S1 "    << S1   << std::endl;
	std::cerr << "S2 "    << S2   << std::endl;
	std::cerr << "E "     << E    << std::endl;
	std::cerr << "C "     << C    << std::endl;
	std::cerr << "Z0 "    << Z0   << std::endl;
	std::cerr << "H "     << H    << std::endl;
	std::cerr << "MH "    << MH   << std::endl;
	std::cerr << "T(deg) "<< T/deg<< std::endl;
	std::cerr << "L "     << L    << std::endl;
	std::cerr << "S "     << S    << std::endl;
	std::cerr << "CP "    << CP   << std::endl;
	std::cerr << "CP2 "   << CP2  << std::endl;
	std::cerr << "#####################" << std::endl;
#endif

	// Set the results
	
	// Thickness of Cone
	fThickness = thick;

	// Cutter for PMT
   fDxOfPmts  = P.getX();
   fDyOfPmts  = P.getX();
   fDzOfPmts  = PR.getZ() > -E.getZ() ? PR.getZ() : -E.getZ();
	fPosOfPmts = G4ThreeVector(0.,0.,0.);

	// Cutter for neighboring
   fDxOfSide  = A.getX();
   fDyOfSide  = A.getX();
   fDzOfSide  = fDzOfPmts;
	fPosOfSide = G4ThreeVector(fDxOfSide*std::sqrt(2.),0.,0.);

	// Cutter for Gate
   fDxOfGate  = A.getX();
   fDyOfGate  = A.getX();
   fDzOfGate  = -E.getZ()/2.;
	fPosOfGate = G4ThreeVector(0.,0.,E.getZ()/2.);

/*
	// Elliptical Cone
	fXSemiAxis = S/2.; // Half of Short Axis
	fYSemiAxis = L/2.; // Half of Long Axis 
	fZMax      = H/2.; // Length from Apex to Center
	fZTopCut   = H;
	fPosOfEcon = G4ThreeVector(-fThickness/std::cos(T*rad),0.,0.);
*/

	// Simple Cone
   fRmin1     = S;
   fRmax1     = S+fThickness; 
   fRmin2     = 0.;
   fRmax2     = fThickness;
   fDz        = H/2.;
   fSPhi      = 0*deg;
   fDPhi      = 360*deg;
	fPosOfCone = CP;

	// RotationMatricis
	fRot045 = new G4RotationMatrix(); fRot045->rotateZ(45.*deg);
	fRot090 = new G4RotationMatrix(); fRot090->rotateZ(90.*deg);
	fRot180 = new G4RotationMatrix(); fRot180->rotateZ(180.*deg);
   fRotcon = new G4RotationMatrix(); fRotcon->rotateY(-T*rad);

	fZero = G4ThreeVector(0.,0.,0.)*mm;
	fPosOfZero = mod->position();

}


G4double CkovMirror::getLength(const G4ThreeVector& a ,const G4ThreeVector& b) const
{ 
	/*
	 * Retern a distance between two points.
	*/
   return std::sqrt(std::pow((a-b).getX(),2.)+std::pow((a-b).getY(),2.)+std::pow((a-b).getZ(),2.));
}

G4double CkovMirror::getAngle(const G4ThreeVector& a, G4ThreeVector& b) const
{
	/*
	 * Retern an angle projected in X-Z plane.
	*/
	return std::atan2((a-b).getX(),(a-b).getZ());
}

G4ThreeVector CkovMirror::getCenter(const G4ThreeVector& a, const G4ThreeVector& b) const
{
	/*
	 * Retern a center point.
	*/
	return (a+b)/2.;
}

G4ThreeVector CkovMirror::getCross(const G4ThreeVector& x1,
										     const G4ThreeVector& x2,
										     const G4ThreeVector& x3,
										     const G4ThreeVector& x4) const
{
	/* 
	 *  This method calculates the crossing point of two lines.
	 *  First line is made by vector of x1 and x2, 
	 *  second line is made by vector of x3 and x4.
	*/

	G4ThreeVector a=x2-x1;
	G4ThreeVector b=x4-x3;
	G4ThreeVector c=x3-x1;

	G4ThreeVector ab=a.cross(b);
	G4double ab2=ab*ab;
	if (ab2 == 0 ) 
	{
		std::cerr << "### Fatal error in CkovMirror..."    << std::endl;
		std::cerr << "### Two lines will not cross...."    << std::endl;
		std::cerr << "### Please check input parameters..."<< std::endl;
		exit(1);
	}

	G4double s=c.cross(b)*a.cross(b)/ab2;
	return s*a+x1;
}

