//Gutted and rewritten by CTR, October 2005

#ifndef BTMAGFIELDMAP_HH
#define BTMAGFIELDMAP_HH

#include <iostream>
#include <string>

#include "Interface/MagFieldMap.hh"
#include "Interface/SplineInterpolator.hh"
#include "BTField.hh"
#include "Config/MiceModule.hh"

#include "CLHEP/Vector/LorentzVector.h"

class BTMagFieldMap: public BTField {

public:
	//For reading
	BTMagFieldMap(const std::string& mapFileName, std::string algorithm);
	BTMagFieldMap(const std::string& mapFileName, const std::string& fileType, std::string algorithm);

	//For writing
  BTMagFieldMap(const BTField* aField);

  // Map destructor

  ~BTMagFieldMap();

	BTMagFieldMap * Clone() const {return new BTMagFieldMap(*this);}


  // Method to retrieve the field of the field map at Point[4]
  inline void GetFieldValue( const  double Point[4], double *Bfield ) const
  {myFieldMap->GetFieldValue(Point, Bfield);}
	//Need to write this method - straightforward but doesn't fit with my scheme!
	inline double zMinf() {return myFieldMap->zMinf();}
	inline double zMaxf() {return myFieldMap->zMaxf();}
	inline double rMinf() {return myFieldMap->rMinf();}
	inline double rMaxf() {return myFieldMap->rMaxf();}	//write map in text format
	void WriteG4MiceTextMap(const std::string& fileName, double Z1,
                    double Z2, double dZ, double R1, double R2, double dR);

	void Print(std::ostream& out) const;
	CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector position) const;

private:
	const std::string myFileType, myMapFileName;
	MagFieldMap* myFieldMap;
	const BTField* myField;

	//gets the value of myField (e.g. analytical field value for writing)
	void GetBTFieldValue( const  double Point[4], double *Bfield ) const
	{myField->GetFieldValue(Point, Bfield);}

}; // class BTMagFieldMap

#endif  // BTMAGFIELDMAP _HH
