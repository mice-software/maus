// MAUS WARNING: THIS IS LEGACY CODE.
//block=X row=Z col=Y nX=1 dX=330.2 nY=77 dY=1.0 nZ=1001 dZ=1.0 extendX=1 extendY=1 extendZ=-1 omitBx=1

#ifndef BT3DFIELDMAP_HH
#define BT3DFIELDMAP_HH

#include <string>

#include "Interface/ThreeDFieldMap.hh"
#include "BTField.hh"

class BT3dFieldMap : public BTField
{
public:
	BT3dFieldMap(std::string interpolation, std::string fileName, std::string fileType, std::string symmetry);
	~BT3dFieldMap() {if(myFieldMap) delete myFieldMap;}
	//Copy
	BT3dFieldMap * Clone() const {return new BT3dFieldMap(*this);}
	BT3dFieldMap           (const BT3dFieldMap &rhs);
	BT3dFieldMap& operator=(const BT3dFieldMap& rhs);

	// Method to retrieve the field of the field map at Point[4]
	inline void GetFieldValue( const double Point[4], double *Bfield ) const;

	//Write output
	void Print(std::ostream& out) const;
	//VectorPotential not implemented!!!
	CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector position) const {return HepLorentzVector();}

private:
  void SetBoundingBox();

	ThreeDFieldMap* myFieldMap;
};

void BT3dFieldMap::GetFieldValue( const double Point[4], double *Bfield ) const {
    myFieldMap->GetFieldValue(Point, Bfield);
}


#endif
