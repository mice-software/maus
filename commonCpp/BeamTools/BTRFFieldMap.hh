//RFFieldMap
//Revamped pill box class to use new slick phasing

#ifndef BTRFFieldMap_HH
#define BTRFFieldMap_HH

#include "Interface/RFFieldMap.hh"
#include "BTPillBox.hh"
#include "BTPhaser.hh"
#include <string>
#include <iostream>

class BTRFFieldMap: public BTPillBox
{
public:
	// Constructor with no time delay argument (relative phase). User has to
	// run Ref.Part. mode first.
	BTRFFieldMap(double frequency, double length, double energyGain, std::string phasingModel, std::string fileName,
	             std::string fileType = "");
  // Constructor with time delay argument (relative phases known and provided
  // by the user). Then, ready for beam (normal) mode.
	BTRFFieldMap(double frequency, double length, double peakEField, double timeDelay,
	             std::string fileName, std::string fileType = "");

	BTRFFieldMap * Clone() const {return new BTRFFieldMap(*this);}

	//Overloaded from PillBox
	void GetPeakFields( const double Point[4], double *Bfield ) const;
	//Ditto
	void Print(std::ostream & out) const;
	static RFFieldMap* SetFieldMap(std::string fileName, std::string fileType);

private:
	RFFieldMap* _fieldMap;
	static std::vector<RFFieldMap*> _fieldMapList;

}; // class BTRFFieldMap

#endif  // BTRFFieldMap_HH
