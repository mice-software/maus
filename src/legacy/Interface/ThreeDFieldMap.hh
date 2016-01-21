// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef ThreeDFieldMap_hh
#define ThreeDFieldMap_hh

#include "Interface/Mesh.hh"
#include "Interface/Interpolator.hh"

class ThreeDFieldMap
{
public:
	ThreeDFieldMap (std::string interpolation, std::string fileName, std::string fileType, 
	                std::string symmetry);
	~ThreeDFieldMap();

	void GetFieldValue( const  double Point[4], double *Bfield ) const;

	static void StripG4BLComments(std::ostream& out, std::istream& in);

	enum symmetry{none, dipole, quadrupole, solenoid};

private:
	VectorMap*              myInterpolator;

	VectorMap*              LoadMap        (std::string interpolation, std::string fileName, std::string fileType);
	Interpolator3dGridTo3d* LoadBeamlineMap(std::istream& in, Interpolator3dGridTo3d::interpolationAlgorithm);

	double***               SetArray   (int nx, int ny, int nz);
	void                    DeleteArray(double*** array, int nx, int ny, int nz);
	std::string             ReplaceVariables( std::string fileName );

	const double*           Symmetry(const double Point[4]) const;
	void                    SetSymmetry(std::string symmetry);

	symmetry   _symmetry; //"dipole", "quadrupole", "none"


};


#endif

