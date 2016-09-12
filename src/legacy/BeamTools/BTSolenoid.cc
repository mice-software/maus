// MAUS WARNING: THIS IS LEGACY CODE.
//BTSolenoid.cc
//Solenoid modelling for the G4MICE BeamTools package

#include <iostream>
using namespace std;
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "src/legacy/Interface/STLUtils.hh"
#include "BTSolenoid.hh"

using CLHEP::m;
using CLHEP::tesla;

int BTSolenoid::StaticNumberOfRCoords=0, BTSolenoid::StaticNumberOfZCoords=0,
    BTSolenoid::StaticNumberOfSheets=0;
 double BTSolenoid::StaticZExtentFactor=0, BTSolenoid::StaticRExtentFactor=0;
std::vector<MagFieldMap*> BTSolenoid::StaticFieldMaps = std::vector<MagFieldMap*>();
const double BTSolenoid::mySheetTolerance = 1e-3*CLHEP::mm;

BTSolenoid::BTSolenoid()
{
	SetToDefaults();
	double myBbMin[3]={-rMax, -rMax, zMin}, myBbMax[3]={rMax, rMax, zMax};
	BTField::bbMin = std::vector<double>(myBbMin, myBbMin+3);
	BTField::bbMax = std::vector<double>(myBbMax, myBbMax+3);
}

BTSolenoid::BTSolenoid(const BTSolenoid& rhs) : mySheets(rhs.mySheets), myNumberOfRCoords(rhs.myNumberOfRCoords),
                                    myNumberOfZCoords(rhs.myNumberOfZCoords), myMap(rhs.myMap),
                                    zMin(rhs.zMin), zMax(rhs.zMax), rMin(rhs.rMin), rMax(rhs.rMax),
                                    myNumberOfSheets(rhs.myNumberOfSheets), isAnalytic(rhs.isAnalytic)
{
}

// Simple Constructor
BTSolenoid::BTSolenoid(double length, double thickness, double innerRadius, double currentDensity, bool analytic, double tolerance, 
	                   std::string fileName, std::string interpolation)
          : myNumberOfRCoords(0), myNumberOfZCoords(0), zMin(0), zMax(0),
            rMin(0), rMax(0), interpolation("LinearCubic"), isAnalytic(analytic)
{
	if(!isAnalytic)
	{
		SetToDefaults();
		myNumberOfRCoords = StaticNumberOfRCoords;
		myNumberOfZCoords = StaticNumberOfZCoords;
		BuildSheets(length,thickness,innerRadius,currentDensity, tolerance, fileName, interpolation);
	}
	else
	{
		SetToDefaults();
		BuildSheets(length,thickness,innerRadius,currentDensity);
	}
}

// Constructor, assuming the user has written the complete solenoid
// onto a file.
BTSolenoid::BTSolenoid(char const *fileName, std::string interpolationAlgorithm): interpolation(interpolationAlgorithm), isAnalytic(false)
{
	ReadFieldMap(fileName, interpolation);
}

// Destructor
BTSolenoid::~BTSolenoid()
{
}

void BTSolenoid::SetToDefaults()
{
	SetNumberOfRCoords(StaticNumberOfRCoords);
	SetNumberOfZCoords(StaticNumberOfZCoords);
	SetZExtentFactor(StaticZExtentFactor);
	SetRExtentFactor(StaticRExtentFactor);
	SetNumberOfSheets(StaticNumberOfSheets);
	SetIsAnalytic(false);
}

// Method called from the Detector constructor user class to write a solenoidal
// map once it has been created, so that next time the code can directly
//  read a binary file and avoid the time consuming task of constructing
// the map.

void BTSolenoid::WriteFieldMap(const char *fileName)
{
	if(!isAnalytic)
	{
		//Set the sheet information...
		myMap->SetSheetInformation(GetSheetInformation());
		//Use the MagFieldMap class to write the map
		if(GetFieldMap(fileName)==NULL)
		{
			myMap->WriteG4MiceBinaryMapV1((std::string)fileName);
			StaticFieldMaps.push_back(myMap);
		}
		else 
		{
			delete myMap;
			myMap = GetFieldMap(fileName);
		}
	}
	else
		std::cerr << "Cannot write analytic field maps. Target file name was " << fileName  << std::endl;
}

void BTSolenoid::ReadFieldMap(const char *fileName, std::string interpolationAlgorithm)
{
	std::string theFileType   = "g4micebinary"; 
	std::string theFileName   = fileName;
	interpolation             = interpolationAlgorithm;
	if(GetFieldMap(fileName)!=NULL) 
		myMap = GetFieldMap(fileName);
	else
	{ 
		myMap = new MagFieldMap(theFileName, theFileType, interpolation);
		StaticFieldMaps.push_back(myMap);
	}
	int id, type;
	double thickness, radius, current, length, x, y, z;
	std::vector<double*> sheet = myMap->GetSheetInformation();
	mySheets = std::vector<BTSheet>();

	for(unsigned int i=0; i<sheet.size(); i++)
	{
		id=(int)sheet[i][0];
		type=(int)sheet[i][1];
		thickness=sheet[i][2];
		radius=sheet[i][3];
		length = sheet[i][4];
		current = sheet[i][5];
		x = sheet[i][6];
		y = sheet[i][7];
		z = sheet[i][8];
		BTSheet newSheet(Hep3Vector(x,y,z), id, type, thickness, radius, length, current);
		mySheets.push_back(newSheet);
	}

	zMin = myMap->zMinf();
	zMax = myMap->zMaxf();
	rMin = myMap->rMinf();
	rMax = myMap->rMaxf();

	double myBbMin[3]={-rMax, -rMax, zMin}, myBbMax[3]={rMax, rMax, zMax};
	BTField::bbMin = std::vector<double>(myBbMin, myBbMin+3);
	BTField::bbMax = std::vector<double>(myBbMax, myBbMax+3);

}

std::vector<double*> BTSolenoid::GetSheetInformation()
{
	std::vector<double*> sheetInformation;
	for(unsigned int i=0; i<mySheets.size(); i++)
	{
		sheetInformation.push_back(new double[9]);
		sheetInformation[i][0] = mySheets[i].id();
		sheetInformation[i][1] = mySheets[i].type();
		sheetInformation[i][2] = mySheets[i].thick();
		sheetInformation[i][3] = mySheets[i].rad();
		sheetInformation[i][4] = mySheets[i].len();
		sheetInformation[i][5] = mySheets[i].cur();
		sheetInformation[i][6] = mySheets[i].location()[0];
		sheetInformation[i][7] = mySheets[i].location()[1];
		sheetInformation[i][8] = mySheets[i].location()[2];
	}
	return sheetInformation;
}



void BTSolenoid::GetAnalyticFieldValue(const  double Point[4], double *Bfield) const
{
	double BfieldCylCoords[3] = {0,0,0}; //Bfield in cylindrical coordinates
	double PointCylCoords[4] = {0,0,0,0}; //Point in cylindrical coordinates
	PointCylCoords[0] = sqrt(Point[0]*Point[0]+Point[1]*Point[1]);
	PointCylCoords[2] = Point[2];
	Bfield[0]=0;
	Bfield[1]=0;
	Bfield[2]=0;

	for(unsigned int i=0; i<mySheets.size(); i++)
	{
		//Not allowed to step exactly on a sheet
		if(PointCylCoords[0] < mySheets[i].rad() + mySheetTolerance && 
                   PointCylCoords[0] > mySheets[i].rad() - mySheetTolerance)
			PointCylCoords[0] = mySheets[i].rad() - mySheetTolerance;
		mySheets[i].GetFieldValue(PointCylCoords, BfieldCylCoords);
		if(PointCylCoords[0]>0) //if r=0, Br=0 for symmetry reasons
		{
			Bfield[0]+=BfieldCylCoords[0]*Point[0]/PointCylCoords[0]; //Bx += Br*x/r
			Bfield[1]+=BfieldCylCoords[0]*Point[1]/PointCylCoords[0]; //By += Br*y/r
		}
		Bfield[2]+=BfieldCylCoords[2];
	}

}

//return is rMax, zMax; assume zMin=-zMax; assume rMin = 0
std::vector<double> BTSolenoid::FindGridEdge(double absoluteTolerance)
{
	std::vector<double> edge(2,0);
	std::vector<double> myR(1,0);
	std::vector<double> myZ(1,0);
	double              field[6] = {0,0,0,0,0,0};
	double              dZ       = 0.1*(zMax-zMin)/(double)(myNumberOfZCoords-1);
	double              dR       = 0.1*(rMax-rMin)/(double)(myNumberOfRCoords-1);
	double              bTotZ    = 1;
	double              bTotR    = 1;

	while(bTotZ>absoluteTolerance || bTotR>absoluteTolerance)
	{
			bTotZ = -1;
			for(unsigned int i=0; i<myR.size(); i++)
			{
				double point[4] = {myR[i],0,myZ.back(),0};
				GetAnalyticFieldValue(point, field);
				double b0 = sqrt(field[0]*field[0]+field[1]*field[1]+field[2]*field[2]);
				if(bTotZ<b0) bTotZ = b0;
			}
			dZ *= 2;
			if(bTotZ > absoluteTolerance)
				myZ.push_back(myZ.back()+dZ);

			bTotR = -1;
			for(unsigned int i=0; i<myZ.size(); i++)
			{
				double point[4] = {myR.back(),0,myZ[i],0};
				GetAnalyticFieldValue(point, field);
				double b0 = sqrt(field[0]*field[0]+field[1]*field[1]+field[2]*field[2]);
				if(bTotR<b0) bTotR = b0;
			}
			dR *= 2;
			if(bTotR > absoluteTolerance)
				myR.push_back(myR.back()+dR);
	}
	edge[0] = myR.back();
	edge[1] = myZ.back();
	return edge;
}

TwoDGrid* BTSolenoid::BuildGrid1(double absoluteTolerance)
{
	int                 rOrder = 2;
	if(interpolation == "BiCubic")  rOrder = 4;
	int                 zOrder = 4;
	if(interpolation == "BiLinear") zOrder = 2;
	std::vector<double> edge  = FindGridEdge(absoluteTolerance);
	std::vector<double> rGrid(2,0);
	std::vector<double> zGrid(2,0);
	rGrid[1]             = edge[0];
	zGrid[1]             = edge[1];
	bool   finished      = false;
	double rInner = mySheets[0].rad()*0.95;
	double rOuter = mySheets.back().rad()*1.05;
	double zOuter = mySheets[0].len()/2.*1.05;

	while(!finished)
	{
		std::vector<bool> rChanged(rGrid.size(), false);
		std::vector<bool> zChanged(zGrid.size(), false);
		finished = true;
		for(int i=rGrid.size()-2; i>=0; i--)
		{
			for(int j=zGrid.size()-2; j>=0; j--)
			{
				if(rGrid[i]<rInner || rGrid[i]>rOuter || zGrid[j]>zOuter )
				{
					std::vector<double> rError = DerivativeError(0, rOrder, zGrid[j], rGrid[i], rGrid[i+1]-rGrid[i]);
					std::vector<double> zError = DerivativeError(2, zOrder, zGrid[j], rGrid[i], zGrid[j+1]-zGrid[j]);
					if(rError[0]>absoluteTolerance && !rChanged[i]) 
					{
						rGrid.insert(rGrid.begin()+i+1, (rGrid[i+1]+rGrid[i])/2.);
						rChanged[i] = true;
						finished    = false;
					}
					if(zError[0]>absoluteTolerance && !zChanged[j]) 
					{
						zGrid.insert(zGrid.begin()+j+1, (zGrid[j+1]+zGrid[j])/2.);
						zChanged[j] = true;
						finished    = false;
					}
				}
			}
		}
	}
		
	std::vector<double> newZ(zGrid.size()*2-1, 0);
	for(unsigned int i=0; i<zGrid.size(); i++) {newZ[zGrid.size()-i-1] = -zGrid[i]; newZ[zGrid.size()+i-1] = +zGrid[i];}
	MAUS::Squeak::mout(MAUS::Squeak::debug) << "Generated grid with " << rGrid.size() << "x" << zGrid.size() << " points extending " 
	                           << (rGrid.back()-rGrid[0])/m << "x" << (zGrid.back()-zGrid[0])/m << " m with tolerance " << absoluteTolerance/tesla
	                           << " T calculated at (" << rOrder << "," << zOrder << ") order" << std::endl;

	return new TwoDGrid(rGrid, newZ);
}

TwoDGrid* BTSolenoid::BuildGrid2(double absoluteTolerance)
{
	std::vector<double> myR(1,0);
	std::vector<double> myZ(1,0);
	//dynamically calculate grid size such that field error < tolerance and 
	//estimate error ~ d^nB/du^n * du^n where n is the order
	//relative error is absolute error/B
	//linear interpolation along r means r order is 2
	//cubic  interpolation along z means z order is 4
	int                 rOrder = 2;
	if(interpolation == "BiCubic")  rOrder = 4;
	int                 zOrder = 4;
	if(interpolation == "BiLinear") zOrder = 2;
	double              dZ       = 0.1*(zMax-zMin)/(double)(myNumberOfZCoords-1);
	double              dR       = 0.1*(rMax-rMin)/(double)(myNumberOfRCoords-1);
	double              bTotZ    = 1e6*tesla;
	double              bTotR    = 1e6*tesla;
	double              dMin     = 1;
	double              dMax     = 1e5; //enforce stability
	std::vector<double> edge     = FindGridEdge(absoluteTolerance);

	while( myR.back()<edge[0] || myZ.back()<edge[1] )
	{
		std::vector<double> error = DerivativeError(2, zOrder, myR[0], myZ.back(), dZ);
		while(error[0]>absoluteTolerance && absoluteTolerance>0 && dZ>dMin)
		{
			error = DerivativeError(2, zOrder, myR[0], myZ.back(), dZ);
			dZ/=2;
			for(unsigned int i=0; i<myR.size(); i++) error = Largest(error, DerivativeError(2, zOrder, myR[i], myZ.back(), dZ) );
		}
		while( (error[0]<absoluteTolerance && absoluteTolerance>0) && bTotZ>absoluteTolerance && dZ<dMax)
		{
			error = DerivativeError(2, zOrder, myR[0], myZ.back(), dZ);
			dZ*=2;
			for(unsigned int i=0; i<myR.size(); i++) error = Largest(error, DerivativeError(2, zOrder, myR[i], myZ.back(), dZ) );
		}
		dZ/=2;
		myZ.push_back(myZ.back()+dZ);

		error = DerivativeError(0, rOrder, myR.back(), myZ[0], dR);
		while(error[0]>absoluteTolerance && absoluteTolerance>0 && dR>dMin)
		{
			error = DerivativeError(0, rOrder, myR.back(), myZ[0], dR);
			dR/=2;
			for(unsigned int i=0; i<myZ.size(); i++) error = Largest(error, DerivativeError(0, rOrder, myR.back(), myZ[i], dR));
		}
		while((error[0]<absoluteTolerance && absoluteTolerance>0) && bTotR>absoluteTolerance && dR < dMax)
		{
			error = DerivativeError(0, rOrder, myR.back(), myZ[0], dR);
			dR*=2;
			for(unsigned int i=0; i<myZ.size(); i++) error = Largest(error, DerivativeError(0, rOrder, myR.back(), myZ[i], dR));
		}
		dR/=2;
		myR.push_back(myR.back()+dR);

	}
	MAUS::Squeak::mout(MAUS::Squeak::info) << "Generated grid with " << myR.size() << "x" << myZ.size() << " points extending " 
	                           << (myR.back()-myR[0])/m << "x" << (myZ.back()-myZ[0])/m << " m with tolerance " << absoluteTolerance/tesla
	                           << " T calculated at (" << rOrder << "," << zOrder << ") order and max (B_r, B_z) (" 
	                           << bTotR/tesla << ", " << bTotZ/tesla  << ") T on grid edge" << std::endl;

	return new TwoDGrid(myR, myZ);
}

TwoDGrid* BTSolenoid::BuildGrid()
{
	double              dZ      = (zMax-zMin)/(double)(myNumberOfZCoords-1);
	double              dR      = (rMax-rMin)/(double)(myNumberOfRCoords-1);
	return new TwoDGrid(dR, dZ, rMin, zMin, myNumberOfRCoords, myNumberOfZCoords);
}

void BTSolenoid::SetTheFieldMap(double tolerance, std::string interpolation)
{
	TwoDGrid* grid;
	if(tolerance>0) grid = BuildGrid1(tolerance);
	else            grid = BuildGrid();
	myNumberOfRCoords = grid->xSize();
	myNumberOfZCoords = grid->ySize(); // number of nodes in the map grid
	rMin = grid->x(1);
	rMax = grid->x(grid->xSize());
	zMin = grid->y(1);
	zMax = grid->y(grid->ySize());

	double    B0[6]= {0,0,0,0,0,0};
	double ** myBr = new double*[grid->xSize()];
	double ** myBz = new double*[grid->xSize()];
	for(int i=0; i<myNumberOfRCoords; i++)
	{
		myBr[i] = new double[grid->ySize()];
		myBz[i] = new double[grid->ySize()];
	}

	for(int i=0; i<grid->xSize(); i++)
		for(int j=0; j<grid->ySize(); j++)
		{
			myBr[i][j] = 0;
			myBz[i][j] = 0;

			double point[4] = {grid->x(i+1),0,grid->y(j+1),0};
			for(unsigned int k=0; k<mySheets.size(); k++)
			{
				mySheets[k].GetFieldValue(point, B0);
				myBr[i][j] += B0[0];
				myBz[i][j] += B0[2];
			}
		}
	myMap = new MagFieldMap(new Interpolator3dSolenoidalTo3d(grid, myBr, myBz, interpolation));
	myMap->SetSheetInformation(GetSheetInformation());

	for(int i=0; i<myNumberOfRCoords; i++)
	{
		delete [] myBr[i];
		delete [] myBz[i];
	}
	delete [] myBr;
	delete [] myBz;
	double myBbMin[3]={-rMax, -rMax, zMin}, myBbMax[3]={rMax, rMax, zMax};
	BTField::bbMin = std::vector<double>(myBbMin, myBbMin+3);
	BTField::bbMax = std::vector<double>(myBbMax, myBbMax+3);
}

std::vector<double> BTSolenoid::Largest(std::vector<double> v1, std::vector<double> v2)
{
	std::vector<double> vout(v1.size(), 0);
	for(unsigned int i=0; i<v1.size(); i++)
		if(v1[i] > v2[i]) vout[i] = v1[i]; else vout[i] = v2[i];
	return vout;
}

//calculate b at various points across the grid; get f(r,z) and apply polynomial fit; calculate derivative; return abs value
std::vector<double> BTSolenoid::DerivativeError (int axis, int order, double z0, double r0, double dAxis) const 
{
	std::vector<double> errors(2,0);
	double point[4] = {0,0,0,0};
	double field[6] = {0,0,0,0,0,0};
	double bMean    = 0;
	std::vector<double> fieldZ(order+1,0);
	std::vector<double> fieldR(order+1,0);
	for(int i=0; i<=order; i++)
	{
			point[0]     = r0;
			point[2]     = z0;
			point[axis] += dAxis*i/double(order);
			GetAnalyticFieldValue(point, field);
			fieldR[i] = field[0];
			fieldZ[i] = field[2];
			bMean += sqrt(fieldR[i]*fieldR[i] + fieldZ[i]*fieldZ[i])/double(order+1);
	}

	while(fieldZ.size() > 1)
	{
		for(unsigned int i=0; i<fieldZ.size()-1; i++)
		{
			fieldZ[i] = (fieldZ[i+1]-fieldZ[i]);
			fieldR[i] = (fieldR[i+1]-fieldR[i]);
//			std::cout << fieldZ[i] << " " << fieldR[i] << " @@ ";
		}
//		std::cout << std::endl;
		fieldZ.resize(fieldZ.size()-1);
		fieldR.resize(fieldZ.size()-1);
	}
	if(fabs(fieldZ[0]) > fabs(fieldR[0]))  errors[0] = fabs(fieldZ[0]);
	else                                   errors[0] = fabs(fieldR[0]);
	errors[1] = errors[0]/bMean;
//	std::cout << axis << " " << order << " " << z0 << " " << r0 << " " << dAxis << " * " << errors[0] << std::endl;
	return errors;
}


void BTSolenoid::BuildSheets(double length, double thickness, double innerRadius,
                             double currentDensity, double tolerance=-1, std::string fileName, std::string interpolationAlgorithm)
{
	interpolation = interpolationAlgorithm;
	int nSheets = myNumberOfSheets;
	BTSheet aSheet(Hep3Vector(0,0,0), 0, 0, 0, 0, 0, 0);
	double sheetRadius, sheetCurrent, sheetSeparation;
	sheetSeparation = thickness/(double)(nSheets);
	sheetCurrent = currentDensity * thickness/(double)nSheets;
	for(int i=0; i<nSheets; i++)
	{
		sheetRadius = innerRadius + (i+0.5)*sheetSeparation;
		aSheet = BTSheet(Hep3Vector(0,0,-0.5*length), i, 0, 0, sheetRadius, length, sheetCurrent);
		mySheets.push_back(aSheet);
	}
	//use numberOfRCoords+1 for bacwards compatibility
	zMax = myZExtentFactor * innerRadius + length;
	rMax = myRExtentFactor * innerRadius;
	rMin = 0;
	zMin = -1*zMax;
	SetTheFieldMap(tolerance, interpolation);
	if(fileName!="") WriteFieldMap(fileName.c_str());
	double myBbMin[3]={-rMax, -rMax, zMin}, myBbMax[3]={rMax, rMax, zMax};
	BTField::bbMin = std::vector<double>(myBbMin, myBbMin+3);
	BTField::bbMax = std::vector<double>(myBbMax, myBbMax+3);

}

void BTSolenoid::Print(std::ostream & out) const
{
	double length, currentDensity, radialSeparation, innerRadius, thickness;
	length = mySheets[0].len();
	radialSeparation = mySheets[1].rad() - mySheets[0].rad();
	thickness = radialSeparation*(double)mySheets.size();
	innerRadius = mySheets[0].rad() - radialSeparation/2.;
	currentDensity=mySheets[0].cur()*(double)mySheets.size()/thickness;
	Hep3Vector position = GetGlobalPosition();
	out << "Solenoid Length: " << length << " Inner Radius: " << innerRadius << " Thickness: "
	    << thickness << " Current Density: " << currentDensity << " ";
	BTField::Print(out);
}

void BTSolenoid::SetStaticVariables(int NumberOfRCoords, int NumberOfZCoords, int NumberOfSheets,
	            double ZExtentFactor, double RExtentFactor)
{
	StaticNumberOfRCoords = NumberOfRCoords;
	StaticNumberOfZCoords = NumberOfZCoords;
	StaticNumberOfSheets = NumberOfSheets;
	StaticZExtentFactor = ZExtentFactor;
	StaticRExtentFactor = RExtentFactor;
}

MagFieldMap* BTSolenoid::GetFieldMap(std::string fileName)
{
  fileName = STLUtils::ReplaceVariables(fileName);
	for(unsigned int i=0; i<StaticFieldMaps.size(); i++)
	{
		if(StaticFieldMaps[i]->GetFileName()==fileName) return StaticFieldMaps[i];
	}
	return NULL;
}

CLHEP::HepLorentzVector BTSolenoid::GetVectorPotential(CLHEP::HepLorentzVector Point) const
{
	CLHEP::HepLorentzVector A(0,0,0,0);
	//Ax = sum^{oo}_{n=0} [ -y 1/(2*(-4)^n)n!(n+1)! d^{2n}B/dz^{2n} ]
	//Ay = sum^{oo}_{n=0} [  x 1/(2*(-4)^n)n!(n+1)! d^{2n}B/dz^{2n} ]
	//where B = B_z(r=0, z)
	double f          = 0; 
	double x          = Point.x();
	double y          = Point.y();
	double z          = Point.z();
	double Bfield[6]  = {0,0,0,0,0,0};
	double Pos[4]     = {0,0,z,0};
	double r2         = 0; //DISABLED
	GetFieldValue(Pos,Bfield);
	f  = Bfield[2]/2. - 1/16.*r2*Getd2Bdz2(z);
	A.setX( -y*f );
	A.setY(  x*f );
	return A;
}

void BTSolenoid::ClearStaticMaps()
{
	for(int i=0; i<(int)StaticFieldMaps.size(); i++)
		delete StaticFieldMaps[i];
	StaticFieldMaps.clear();
}

int BTSolenoid::WriteIcoolSheets(std::ostream & out, Hep3Vector point, int firstSheetNumber) const
{
	Hep3Vector centrePos = GetGlobalPosition() - point;
	
	for(int i=0; i<myNumberOfSheets; i++)
	{
		double length  = mySheets[i].len();
		double radius  = mySheets[i].rad();
		double current = mySheets[i].cur()*GetScaleFactor(this);
		double zStart  = centrePos.z() - length/2.;
		out << firstSheetNumber+i << " " << zStart/1e3 << " " << length/1e3 << " " 
		    << radius/1e3 << " " << current*1e3 << "\n";
	}
	out << std::flush;
	return firstSheetNumber + myNumberOfSheets;
}

void BTSolenoid::WriteIcoolSheetFile(std::string filename, Hep3Vector point, std::vector<BTSolenoid*> Solenoids)
{
	std::ofstream fout(filename.c_str());
	int numberOfSheets=0;
	if(!fout)
	{
		MAUS::Squeak::mout(MAUS::Squeak::warning) << "Could not open file " << filename << " for icool output" << std::endl;
		return;
	}
	for(unsigned int i=0; i<Solenoids.size(); i++) numberOfSheets+=Solenoids[i]->GetNumberOfSheets();
	fout << "Sheets_from_G4MICE\n" << numberOfSheets << "\n" << 1 << "\n";

	int firstSheet = 0;
	for(unsigned int i=0; i<Solenoids.size(); i++)
		firstSheet = Solenoids[i]->WriteIcoolSheets(fout, point, firstSheet);
	if(!fout) 
		MAUS::Squeak::mout(MAUS::Squeak::warning) << "There was an error while writing " << filename 
		                              << " for icool output" << std::endl;
	fout.close();

}

