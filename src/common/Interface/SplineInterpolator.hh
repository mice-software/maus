//Created November 2004 Chris Rogers
#ifndef SPLINEINTERPOLATOR_HH
#define SPLINEINTERPOLATOR_HH
#include <vector>
#include "Interface/Spline1D.hh"
#include "Interface/Interpolator.hh"

using std::vector;

class SplineInterpolator : public Interpolator3dSolenoidalTo3d
{
public:
	//Constructor for variable spacing grids - doesn't work yet!
	//SplineInterpolator(double coords[][], double inputBr[][], double inputBz[][]  )

	//Constructor for grids with constant spacing; 2D arrays go like [index_r][index_z]
	SplineInterpolator(double dR, double dZ, double minZ, int numberOfRCoords, int numberOfZCoords,
	                   double **inputBz, double **inputBr);
	SplineInterpolator(const SplineInterpolator& rhs);
	SplineInterpolator();
	~SplineInterpolator();
	SplineInterpolator* Clone()   const {return new SplineInterpolator(*this);}
	SplineInterpolator& operator= (const SplineInterpolator& rhs);

	//Returns the interpolated field value for some cartesian point
	inline  void F(const double Point[3],       double BField[3]) const {GetFieldValue(Point, BField);}
	virtual void F(const Mesh::Iterator& Point, double BField[3]) const;
	void GetFieldValue(const double Point[4], double* Bfield) const;
//	void GetFieldValueForMap(const double Point[4], double* Bfield) const;

	double GetMinZ() {return _zOfGrid[0];}
	double GetMaxZ() {return _zOfGrid[_numberOfZCoords-1];}
	double GetMinR() {return _rOfGrid[0];}
	double GetMaxR() {return _rOfGrid[_numberOfRCoords-1];}
	int GetNumberOfRCoords() {return _numberOfRCoords;}
	int GetNumberOfZCoords() {return _numberOfZCoords;}
	double * GetRGrid() {return _rOfGrid;}
	double * GetZGrid() {return _zOfGrid;}
	double ** GetBrGrid() {return _inputBr;}
	double ** GetBzGrid() {return _inputBz;}

private: //Private methods should be inlined later
	//splines sit along z at each different radial step in the field
	vector<Spline1D> _bzSplines;
	vector<Spline1D> _brSplines;
	double *_rOfGrid;
	double *_zOfGrid;
	//Only called in constructor
	//If you want to call outside of constructor need to delete [] first
	//Set the field grid to Bz, Br
	void SetFields(double ** Bz, double ** Br);


	//data
	double **_inputBz;
	double **_inputBr;
	int _numberOfRCoords, _numberOfZCoords;
	double _dR;

};

#endif
