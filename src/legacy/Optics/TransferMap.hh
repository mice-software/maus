#ifndef TransferMap_hh
#define TransferMap_hh

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "src/legacy/Interface/Differentiator.hh"
#include "src/legacy/Optics/CovarianceMatrix.hh"
#include "src/legacy/Optics/PhaseSpaceVector.hh"
#include "src/legacy/Optics/OpticsModel.hh"
#include "src/legacy/Optics/Tensor3.hh"

namespace MAUS {
class PolynomialMap;
}

//
//TransferMap is a polynomial mapping of a phase space vector from a plane at Z1 to another plane at Z2,
//such that PhaseSpaceVector with coordinates U transform like
//   U(Z2)_p = M_ip U(Z1)_i + M_ijp U(Z1)_i U(Z1)_j + M_ijkp U(Z1)_i U(Z1)_j U(Z1)_k + ...
//extending to arbitrary order. A reference trajectory is assumed, that is the transformation is applied about
//some phase space vector that is taken to be a zero point.
//
//Currently first order transformations use the CLHEP::HepMatrix object, second order transformations use the
//Tensor3 object and higher order transformations use the Tensor object
//
//At some point I would like to move to the more general MAUS::PolynomialMap object. For now I have implemented an 
//interface to this object
//
//

class TransferMap
{
public:
	//Create the transfer map that transports from referenceIn to referenceOut using firstOrderMap
	TransferMap(HepMatrix firstOrderMap, PhaseSpaceVector referenceIn, PhaseSpaceVector referenceOut, 
	            OpticsModel* Optics);
	TransferMap(HepMatrix transverseMap, HepMatrix longitudinalMap, PhaseSpaceVector referenceIn, 
                    PhaseSpaceVector referenceOut, OpticsModel* Optics);
	//Create a transfer map that does nothing i.e. referenceIn=referenceOut, M=1
	TransferMap(PhaseSpaceVector referenceIn, OpticsModel* Optics);
	TransferMap();
	~TransferMap() {;}
	//Set up
	void SetFirstOrderMap(const HepMatrix transverseMap, const HepMatrix longitudinalMap);
	void SetFirstOrderMap(const HepMatrix firstOrderMap);
	void SetSecondOrderMap(Tensor3 secondOrderMap)            {_secondOrderMap = secondOrderMap;}
	void SetReferenceIn(const PhaseSpaceVector referenceIn)   {_referenceTrajectoryIn = referenceIn;}	
	void SetReferenceOut(const PhaseSpaceVector referenceOut) {_referenceTrajectoryOut = referenceOut;}
	void SetReferenceOutZ(double z)                           {_referenceTrajectoryOut.setZ(z);}
	void IncrementReferenceOutZ(double dz)                    {SetReferenceOutZ(_referenceTrajectoryOut.z()+dz);}
	void SetOpticsModel(OpticsModel* optics)                  {_optics = optics;}
	//Accessors or whatever
	PhaseSpaceVector GetReferenceIn()  const    {return _referenceTrajectoryIn;}
	PhaseSpaceVector GetReferenceOut() const    {return _referenceTrajectoryOut;}
	//Accessor for first order matrix
	HepMatrix GetFirstOrderMap()   const;
	HepMatrix GetTransverseMap()   const {return _firstOrderMap.sub(3,6,3,6);}
	HepMatrix GetLongitudinalMap() const {return _firstOrderMap.sub(1,2,1,2);}
	//Accessor for second and higher order maps
	Tensor3   GetSecondOrderMap()  const {return _secondOrderMap;}
	Tensor    GetMap(int order)    const;
	//Methods to transport objects
	//Return value is M^T V M
	CovarianceMatrix operator*(const CovarianceMatrix& aCovMatrix) const;
	//Return value is M u
	PhaseSpaceVector operator*(const PhaseSpaceVector& aPhaseSpaceVector) const;
	//Return value is M1*M2 where *this is M1 and aTransferMAp is M2
	TransferMap      operator*(const TransferMap&      aTransferMap) const;	
	TransferMap      operator*=(const TransferMap&     aTransferMap);
	//Return transfer map rotated by an angle theta
	TransferMap      Rotate(double angle) const;
	//Return the inverse of the transfer map
	TransferMap      Inv(int & failflag) const; 
	TransferMap      Inv() const {int i; return Inv(i);}
	//determinant of the 6d transfer map
	double           Det() const {return GetFirstOrderMap().determinant();}
	//decompose into a "matched" covariance matrix
	CovarianceMatrix Decompose(double em_t, double em_x, double em_y) const;
	//rotate by angle then decompose into a "matched" covariance matrix... then rotate cov matrix back
	CovarianceMatrix Decompose(double angle, double em_t, double em_x, double em_y) const;
	//return a cov matrix with coefficients for sxx_out/sxx_in, spp_out/spp_in and sxp_out/sxp_in
	std::vector<CovarianceMatrix> Decompose(double sxx, double sxp, double spp, double em_t, double em_x, 
	                                        double em_y) const;
	CovarianceMatrix Decompose(double sxx, double sxp, double spp, 
	                           double em_t, double em_x, double em_y, double angle) const;
	//Get the phase advance of the transfer matrix in the domain (0, pi), given by acos((m[0][0]+m[1][1])/2.); 
  //units are radians; if LarmorAngle is specified, unrotates matrix first
	double           PhaseAdvance(int axis) const;
	double           PhaseAdvance(int axis, double LarmorAngle) const;
  //
	MAUS::PolynomialMap* GetPolynomialMap();
	void              SetPolynomialMap(MAUS::PolynomialMap* poly);
	bool              IsCanonical()         {return _canonicalMap;}
	bool              IsCanonical(bool can) {_canonicalMap = can; return _canonicalMap;}

	//order
	static void      SetOrder(int order) {_order = order;}
	static int       GetOrder()          {return _order;}
	friend std::ostream& operator<<(std::ostream& out, TransferMap tm);

private:
	//Transfer maps of various orders
	HepMatrix            _firstOrderMap;
	Tensor3              _secondOrderMap;
	std::vector<Tensor*> _higherOrderMaps;
	//The reference trajectory is a property of the transfer map
	PhaseSpaceVector     _referenceTrajectoryIn;
	PhaseSpaceVector     _referenceTrajectoryOut;
	//Need this for transporting phase space vectors
	OpticsModel*         _optics;
	MAUS::PolynomialMap*    _polynomial; //set to NULL to disable
	static int           _order;
	bool                 _canonicalMap;

};

std::ostream& operator<<(std::ostream& out, TransferMap tm);

#endif

