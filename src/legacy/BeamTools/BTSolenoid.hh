// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: BTSolenoid.hh,v 1.18 2008-02-29 15:01:41 rogers Exp $  $Name:  $
//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of     *
// * FERMILAB.                                                        *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// BTSolenoid.hh
//
// Created: V.Daniel Elvira (5/00)
//
// Modified: P. Lebrun - restrict the field to only two degrees of freedom
//                       (solenoids have phi symmetry).
//
//   The BTSolenoid Class inherits from G4MagneticField. The class objects are
//   field maps in the form of a grid in r-z space. They are generated by a
//   set of infinitelly thin solenoidal current sheets. The BTSheet objects
//   forming the solenoid are also data members of BTSolenoid.
//   No geometric volumes or materials are associated with BTSheets.
//
#ifndef BTSOLENOID_HH
#define BTSOLENOID_HH

#include <iostream>
#include <string>
#include <vector>

#include "BTField.hh"
#include "BTSheet.hh"
#include "Interface/MagFieldMap.hh"
#include "Interface/Squeak.hh"

#include "Interface/SplineInterpolator.hh"

class BTSolenoid: public BTField
{
public:

  //REMEMBER TO SET DEFAULT VALUES BEFORE CALLING CONSTRUCTOR
  // Constructor of the solenoid from sheets
  //Leaves an empty solenoid for later call to BuildSheets
  BTSolenoid();
  // Simple Constructor which builds sheets
  // BUG - BuildSheets fails in this constructor - use BTSolenoid() then BuildSheets(blah blah)
  BTSolenoid(double length, double thickness, double innerRadius, double currentDensity=1, bool analytic=false, double tolerance=-1,  
             std::string fileName="", std::string interpolation="LinearCubic");
  // Constructor of the solenoid from a binary file
  BTSolenoid(char const *fileName, std::string interpolation);
  // Copy Constructor of the solenoid from a binary file
  BTSolenoid(const BTSolenoid& rhs);
  // Destructor
  ~BTSolenoid();

	BTSolenoid * Clone() const {return new BTSolenoid(*this);}


	// Writes map in binary format
	void ReadFieldMap(const char *fileName, std::string interpolation);

	//Returns the analytic field value at some point calculated from the sheets
	void GetAnalyticFieldValue(const  double Point[4], double *Bfield) const;

	//Return field at cartesian position, time Point[4].
	inline void GetFieldValue( const  double Point[4], double *Bfield ) const
  	{
		if(!isAnalytic) myMap->GetFieldValue(Point, Bfield);
		else GetAnalyticFieldValue(Point, Bfield);
	}

	//Override static defaults
	void  SetNumberOfRCoords(int numberOfCoords) { myNumberOfRCoords = numberOfCoords; }
	void  SetNumberOfZCoords(int numberOfCoords) { myNumberOfZCoords = numberOfCoords; }
	void  SetZExtentFactor(double factor)  { myZExtentFactor = factor; }
	void  SetRExtentFactor(double factor)  { myRExtentFactor = factor; }
	void  SetNumberOfSheets(int numberOfSheets) { myNumberOfSheets = numberOfSheets; }
	void  SetIsAnalytic(bool analytic) { isAnalytic = analytic; }

	//Return size
	double ZMin() {return zMin;}
	double ZMax() {return zMax;}
	double RMin() {return rMin;}
	double RMax() {return rMax;}

	//Set static defaults
	static void SetStaticVariables(int NumberOfRCoords, int NumberOfZCoords, int NumberOfSheets,
	            double ZExtentFactor, double RExtentFactor);
	//Write solenoid to output
	void Print(std::ostream& out) const;
	//Write as an icool sheet input, relative to point
	int WriteIcoolSheets(std::ostream& out, Hep3Vector point, int firstSheet) const;
	int GetNumberOfSheets() const {return mySheets.size();}
	//Build the iheets
	void BuildSheets(double length, double thickness, double innerRadius,
	                 double currentDensity, double tolerance, std::string fileName="", std::string interpolation="LinearCubic");
	void BuildSheets(double length, double thickness, double innerRadius,
	                 double currentDensity, std::string fileName="", std::string interpolation="LinearCubic")
	{BuildSheets(length, thickness, innerRadius, currentDensity, -1, fileName, interpolation);}
	//Calculate the vector potential at a point - only linear for now! Next term O(x^3)
	CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector Point) const;
	static void  ClearStaticMaps();
	static void  WriteIcoolSheetFile(std::string filename, Hep3Vector point, std::vector<BTSolenoid*> Solenoids);
	MagFieldMap* GetFieldMap() {return myMap;}

private:
	//Return the field map if it is already used in a BTSolenoid or NULL if it is not
	MagFieldMap*        GetFieldMap(std::string fileName);
	void                WriteFieldMap(const char *fileName);
	void                SetTheFieldMap(double tolerance); //set myMap assuming sheets are built
	TwoDGrid*           BuildGrid();
	std::vector<double> FindGridEdge(double absoluteTolerance);
	//Dynamically choose grid spacing
	//Choose grid spacing by interpolation
	TwoDGrid*           BuildGrid1(double absoluteTolerance);
	//Choose grid spacing by growing the grid outwards
	TwoDGrid*           BuildGrid2(double absoluteTolerance);
	void                SetTheFieldMap(double absoluteTolerance, std::string interpolation); //now choose grid spacing dynamically
	void                SetToDefaults(); //set variables to (static) defaults
	double              Getd2Bdz2(double z) const {return 0;}
	std::vector<double> Largest(std::vector<double> v1, std::vector<double> v2);
	//calculate the largest of d^nBr/du^n or d^nBz/du^n 
	//axis=0 => u=r, axis=2 => u=z; n = order; du = dAxis
	//return value is {derivative, derivative/Btotal} 
	std::vector<double> DerivativeError (int axis, int order, double z0, double r0, double dAxis) const;

	std::vector<BTSheet> mySheets; // vector of current sheets

	int myNumberOfRCoords, myNumberOfZCoords; // number of nodes in the map grid
	MagFieldMap * myMap;
	double zMin, zMax, rMin, rMax;
	double myZExtentFactor, myRExtentFactor;
	int myNumberOfSheets;
	static int StaticNumberOfRCoords, StaticNumberOfZCoords, StaticNumberOfSheets;
	static double StaticZExtentFactor, StaticRExtentFactor;
	static const double mySheetTolerance;

	std::string interpolation;
	bool isAnalytic;
	//Allocate memory containing sheetInformation
	std::vector<double*> GetSheetInformation();
	//Static vector of field maps
	static std::vector<MagFieldMap*> StaticFieldMaps;

}; // class BTSolenoid

#endif  // BTSOLENOID_HH