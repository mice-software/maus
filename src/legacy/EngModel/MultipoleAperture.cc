// MAUS WARNING: THIS IS LEGACY CODE.
#include "EngModel/MultipoleAperture.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4UnionSolid.hh"
#include "Utils/Exception.hh"

MultipoleAperture::MultipoleAperture(MiceModule * mod)
                  : _innerHeight(0), _innerWidth(0), _outerHeight(0), _outerWidth(0), 
                    _innerRadius(0), _outerRadius(0), _referenceCurvature(0), 
                    _isCurved(false), _length(0), _poleCenterRadius(0), _poleTipRadius(0),
	                _numberOfPoles(0), _mod(mod), _volume(NULL)
{
	_referenceCurvature = mod->propertyDoubleThis("ApertureCurvature");
	_isCurved           = fabs(_referenceCurvature)>1e-5;
	_length             = mod->propertyDoubleThis("ApertureLength");

	if(mod->propertyExistsThis("NumberOfPoles", "int"))
		if(mod->propertyIntThis("NumberOfPoles")>0)
		{
			_poleCenterRadius = mod->propertyDoubleThis("PoleCenterRadius");
			_poleTipRadius    = mod->propertyDoubleThis("PoleTipRadius");
			_numberOfPoles    = mod->propertyIntThis("NumberOfPoles");
		}
	_innerHeight        = mod->propertyDoubleThis("ApertureInnerHeight");
	_innerWidth         = mod->propertyDoubleThis("ApertureInnerWidth");
	_outerHeight        = mod->propertyDoubleThis("ApertureOuterHeight");
	_outerWidth         = mod->propertyDoubleThis("ApertureOuterWidth");
	if(!_isCurved) _volume = BuildBox();
	else           _volume = BuildCurvedBox();
}

MultipoleAperture::~MultipoleAperture() {}


G4VSolid * MultipoleAperture::BuildBox()
{
	G4Box*               inner       = new G4Box(_mod->name()+"inner", _innerWidth/2., _innerHeight/2., _length/2.);
	G4Box*               outer       = new G4Box("outer", _outerWidth/2., _outerHeight/2., _length/2.);
	G4Box*               veryOuter   = new G4Box("veryOuter", _outerWidth/2.+_poleCenterRadius, _outerHeight/2.+_poleCenterRadius, _length/2.);
	G4VSolid*            multipole   = new G4SubtractionSolid("outer-inner", outer, inner);
	for(int i=0; i<_numberOfPoles; i++)
		multipole = AddPole(i, multipole, false);
	if(_numberOfPoles>0) multipole   = new G4SubtractionSolid("outer-inner-veryOuter", multipole, veryOuter);
	G4ThreeVector        transform   = G4ThreeVector(0,0,_length/2.);
	multipole                        = new G4DisplacedSolid("Multipole", multipole, new G4RotationMatrix(0,0,0), transform);

	return multipole;
}

G4VSolid * MultipoleAperture::AddPole(int poleNumber, G4VSolid* inputSolid, bool isCurved)
{
	if(isCurved) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Could not build curved pole", "MultipoleAperture::AddPole") );
	double            angle         = (poleNumber+0.5)*360.*degree/double(_numberOfPoles);
	double            xCentre       = sin(angle)*_poleCenterRadius;
	double            yCentre       = cos(angle)*_poleCenterRadius;
	if(angle>180*degree && angle<360*degree) xCentre = -1.*fabs(xCentre);
	if(angle>90*degree  && angle<270*degree) yCentre = -1.*fabs(yCentre);
	G4ThreeVector     transform     = G4ThreeVector(xCentre,yCentre,0);
	G4Tubs*           pole          = new G4Tubs("pole", 0, _poleCenterRadius-_poleTipRadius, _length/2., 0, 360.*deg);
	G4DisplacedSolid* displacedPole = new G4DisplacedSolid("displaced", pole, new G4RotationMatrix(0,0,0), transform);
	return new G4UnionSolid("union", inputSolid, displacedPole);
}

G4VSolid * MultipoleAperture::BuildCurvedBox()
{
	double  dPhi  = fabs(_length/_referenceCurvature);
	G4Tubs* inner = new G4Tubs("inner", fabs(_referenceCurvature)-_innerWidth/2., fabs(_referenceCurvature)+_innerWidth/2., _innerHeight/2., 0, dPhi);
	G4Tubs* outer = new G4Tubs("outer", fabs(_referenceCurvature)-_outerWidth/2., fabs(_referenceCurvature)+_outerWidth/2., _outerHeight/2., 0, dPhi);
	G4SubtractionSolid* subtraction = new G4SubtractionSolid("outer-inner", outer, inner);
	G4ThreeVector       transform   = G4ThreeVector(-fabs(_referenceCurvature),0,0);
	G4DisplacedSolid*   displaced1  = new G4DisplacedSolid("displaced", subtraction, new G4RotationMatrix(0,0,0), transform);
	G4ThreeVector       noTransform = G4ThreeVector(0,0,0);
	G4DisplacedSolid*   displaced2  = new G4DisplacedSolid("Multipole", displaced1, new G4RotationMatrix(G4ThreeVector(1,0,0), -90*degree), noTransform);
	if(_referenceCurvature < 0.)
		displaced2 = new G4DisplacedSolid("Multipole", displaced2, new G4RotationMatrix(G4ThreeVector(0,0,1), 180*degree), noTransform);

	return displaced2;
}

