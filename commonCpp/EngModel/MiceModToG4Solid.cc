#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Polycone.hh"
#include "G4Torus.hh"
#include "G4EllipticalCone.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4VSolid.hh"
#include "G4AffineTransform.hh"

#include "Polycone.hh"
#include "MultipoleAperture.hh"
#include "MiceModToG4Solid.hh"
#include "Config/ModuleTextFileIO.hh"
#include "Config/MiceModule.hh"
#include "Interface/Squeal.hh"


namespace MiceModToG4Solid
{


//Build a G4Solid if you don't know the solid type
G4VSolid* buildSolid ( MiceModule* mod )
{
	std::string volume = mod->volType();
	for(unsigned int i=0; i<volume.size(); i++) volume[i] = tolower(volume[i]);
	if(volume == "none")      return NULL;
	if(volume == "wedge")     return buildWedge    (mod);
	if(volume == "box")       return buildBox      (mod);
	if(volume == "cylinder")  return buildCylinder (mod);
	if(volume == "tube")      return buildTube     (mod);
	if(volume == "sphere")    return buildSphere   (mod);
	if(volume == "polycone")  return buildPolycone (mod);
	if(volume == "torus")     return buildTorus    (mod);
	if(volume == "boolean")   return buildBoolean  (mod);
	if(volume == "multipole") return buildMultipole(mod);
	if(volume == "ellipticalcone")  return buildEllipticalCone (mod);
	throw(Squeal(Squeal::recoverable, "Volume "+mod->volType()+" not recognised in module "+mod->fullName(), 
	                                  "MiceModToG4Solid::buildSolid") );
}


//If you know the solid type, you may prefer to call these methods
//One for each supported G4Solid
G4VSolid * buildWedge    ( MiceModule* mod)
{
	return new G4Trd( mod->name()+"Wedge", mod->dimensions().x()/2., mod->dimensions().x()/2., 
	                  mod->dimensions().y(), 0, mod->dimensions().z()/2.);
}

G4VSolid * buildBox      ( MiceModule* mod)
{
	return new G4Box( mod->name() + "Box", mod->dimensions().x() / 2., mod->dimensions().y() / 2.,
	                  mod->dimensions().z() / 2. );
}

G4VSolid * buildCylinder ( MiceModule* mod )
{
	return new G4Tubs( mod->name() + "Tubs", 0., mod->dimensions().x(), mod->dimensions().y() / 2., 0. * deg, 360. * deg );
}

G4VSolid * buildTube     ( MiceModule* mod )
{
	return new G4Tubs( mod->name() + "Tubs", mod->dimensions().x(), mod->dimensions().y(), mod->dimensions().z() / 2., 0. * deg, 360. * deg );
}

G4VSolid * buildEllipticalCone   ( MiceModule* mod )
{
	double zcut = 2.*mod->dimensions().z();
	if( mod->propertyExistsThis( "ZCut", "double" ))
		zcut = mod->propertyDoubleThis("ZCut");
        //G4EllipticalCone takes semi_axis_x/length, semi_axis_y/length, length
        G4EllipticalCone * out = new G4EllipticalCone(mod->name() + "Cone", mod->dimensions().x()/mod->dimensions().z(), 
                                                      mod->dimensions().y()/mod->dimensions().z(), mod->dimensions().z(), zcut);
	return out;
}


G4VSolid * buildSphere   ( MiceModule* mod )
{
	Hep3Vector phi, theta;

	if( mod->propertyExistsThis( "Phi", "Hep3Vector" )) 
		phi = mod->propertyHep3VectorThis("Phi");
	else	phi = Hep3Vector(0.0*deg, 360.0*deg, -1.0);

	if( mod->propertyExistsThis( "Theta", "Hep3Vector" ))
		theta = mod->propertyHep3VectorThis("Theta");
	else	theta = Hep3Vector(0.0*deg, 360.0*deg, -1.0);

	return new G4Sphere(mod->name() + "Sphere", mod->dimensions().x(), mod->dimensions().y(), phi.x(), phi.y(), theta.x(), theta.y());
}

G4VSolid * buildPolycone ( MiceModule* mod )
{
	Polycone newPolycone(mod);
	return newPolycone.GetPolycone();
}

G4VSolid * buildTorus    ( MiceModule* mod )
{
	double pRmin      = mod->propertyDoubleThis("TorusInnerRadius");
	double pRmax      = mod->propertyDoubleThis("TorusOuterRadius");
	double rCurv      = mod->propertyDoubleThis("TorusRadiusOfCurvature");
	double angleStart = mod->propertyDoubleThis("TorusAngleStart");
	double openAngle  = mod->propertyDoubleThis("TorusOpeningAngle");

	return new G4Torus( mod->name() + "TorusSegment", pRmin, pRmax, rCurv, angleStart, openAngle );
}

G4VSolid * buildMultipole( MiceModule* mod )
{
	MultipoleAperture multipole(mod);
	G4VSolid*    ap = multipole.getMultipoleAperture();
	return       ap;
}

G4VSolid * buildBoolean  ( MiceModule* mod )
{
	MiceModule* base     = getModule(mod, mod->propertyStringThis("BaseModule"));
	G4VSolid*   baseSolid    = buildSolid(base);
	std::string boolProp = "BooleanModule1";
	int         index    = 1;
	while(mod->propertyExistsThis(boolProp, "string"))
	{
		MiceModule*         next   = getModule(mod, mod->propertyStringThis(boolProp));
		Squeak::mout(Squeak::debug) << "Adding module " << next->fullName() << " for boolean " << mod->fullName() << std::endl;
		std::string         type   = mod->propertyStringThis    (boolProp+"Type");
		G4ThreeVector       pos    = mod->propertyHep3VectorThis(boolProp+"Pos");
		CLHEP::Hep3Vector   rTemp  = mod->propertyHep3VectorThis(boolProp+"Rot");
		CLHEP::HepRotation* rot    = new CLHEP::HepRotation(HepRotationX( rTemp.x() )* 
		                            HepRotationY( rTemp.y() )*HepRotationZ( rTemp.z() ));
		G4VSolid*           solid  = buildSolid(next);
		G4DisplacedSolid*   moved  = new G4DisplacedSolid("moved", solid, G4AffineTransform(rot, pos));
		/**/ if(type == "Union")        baseSolid = new G4UnionSolid       ("base", baseSolid, moved);
		else if(type == "Intersection") baseSolid = new G4IntersectionSolid("base", baseSolid, moved);
		else if(type == "Subtraction")  baseSolid = new G4SubtractionSolid ("base", baseSolid, moved);
		else throw(Squeal(Squeal::recoverable, "Did not recognise boolean type "+type, 
		                                       "MiceModToG4Solid::buildBoolean"));
		index++;
		std::stringstream iStream; 
		iStream << "BooleanModule" << index;
		boolProp = iStream.str();
		mod->removeDaughter(next);
		delete next;
		delete rot;
		Squeak::mout(Squeak::debug) << "Searching for " << iStream.str() << std::endl;
	}
        mod->removeDaughter(base);

	delete base;
	return baseSolid;
}

MiceModule* getModule      (MiceModule* mod, std::string newModName)
{
	std::stringstream modStream1, modStream2;
	std::string       file = std::string(getenv( "MICEFILES" )) + "/Models/Modules/" + newModName;
	std::ifstream     fin(file.c_str());
	if(!fin) 
	{
		fin.close();
		fin.open(newModName.c_str());
		if(!fin) 
		{
			fin.close();
			throw(Squeal(Squeal::recoverable, "Failed to open either file "+file+" or file "+newModName+" for boolean "+mod->fullName(),
		                                  "MiceModToG4Solid::getModule"));
		}
	}
	ModuleTextFileIO::stripFile(modStream1, fin);
	ModuleTextFileIO::substitute(modStream2, modStream1);
	ModuleTextFileIO daughter  (mod, newModName, modStream2);
	return daughter.getModule  ();
}

}







