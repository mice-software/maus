// MAUS WARNING: THIS IS LEGACY CODE.
//Created November 2004 Chris Rogers

#include <vector>
#include "Interface/SplineInterpolator.hh"
#include "Interface/Spline1D.hh"
#include <cmath>

using std::vector;

SplineInterpolator::SplineInterpolator(double dR, double dZ, double minZ,
	                   int numberOfRCoords, int numberOfZCoords, double **inputBz, double **inputBr)
                  : _rOfGrid(NULL), _zOfGrid(NULL), _inputBz(NULL), _inputBr(NULL),
	                _numberOfRCoords(numberOfRCoords), _numberOfZCoords(numberOfZCoords), _dR(dR)
{
	//Make splines for each radial position and store the radial positions
	_rOfGrid = new double[numberOfRCoords];
	_zOfGrid = new double[numberOfZCoords];

	for(int i=0; i<_numberOfZCoords; i++)
		_zOfGrid[i] = (double)dZ*i + minZ;

	SetFields(inputBz, inputBr);

	for(int i=0; i<_numberOfRCoords; i++)
	{
		_bzSplines.push_back(Spline1D(_numberOfZCoords, _zOfGrid, _inputBz[i]));
		_brSplines.push_back(Spline1D(_numberOfZCoords, _zOfGrid, _inputBr[i]));
		_rOfGrid[i] = (double)_dR*i;
	}
	SetGrid(new TwoDGrid(_numberOfRCoords, _rOfGrid, _numberOfZCoords, _zOfGrid));
	GetGrid()->Add(this);
}

SplineInterpolator::SplineInterpolator(): _rOfGrid(NULL), _zOfGrid(NULL), _inputBz(NULL),
                      _inputBr(NULL), _numberOfRCoords(0), _numberOfZCoords(0), _dR(0)
{
}

SplineInterpolator::SplineInterpolator(const SplineInterpolator& rhs)
                   : _bzSplines(rhs._bzSplines), _brSplines(rhs._brSplines),
                    _rOfGrid(NULL), _zOfGrid(NULL), _inputBz(NULL), _inputBr(NULL),
                    _numberOfRCoords(rhs._numberOfRCoords), _numberOfZCoords(rhs._numberOfZCoords), _dR(rhs._dR)
{
	_rOfGrid = new double[_numberOfRCoords];
	_zOfGrid = new double[_numberOfZCoords];

	for(int i=0; i<_numberOfRCoords; i++)
		_rOfGrid[i] = rhs._rOfGrid[i];

	for(int i=0; i<_numberOfZCoords; i++)
		_zOfGrid[i] = rhs._zOfGrid[i];
	SetFields(rhs._inputBz, rhs._inputBr);
	SetGrid(new TwoDGrid(_numberOfRCoords, _rOfGrid, _numberOfZCoords, _zOfGrid));
	GetGrid()->Add(this);
}

SplineInterpolator::~SplineInterpolator()
{
	if((_rOfGrid!=NULL)&&(_zOfGrid!=NULL))
	{
		delete [] _rOfGrid;
		delete [] _zOfGrid;
	}
	if((_inputBr!=NULL)&&(_inputBz!=NULL))
	for(int i=0; i<_numberOfRCoords; i++)
	{
		delete [] _inputBr[i];
		delete [] _inputBz[i];
	}
	delete [] _inputBr;
	delete [] _inputBz;
}

SplineInterpolator& SplineInterpolator::operator= (const SplineInterpolator& rhs)
{
	if (this == &rhs) return *this;

	_bzSplines = rhs._bzSplines;
	_brSplines = rhs._brSplines;
	_numberOfRCoords = rhs._numberOfRCoords;
	_numberOfZCoords = rhs._numberOfZCoords;
	_dR = rhs._dR;
	_rOfGrid = new double[_numberOfRCoords];
	_zOfGrid = new double[_numberOfZCoords];

	for(int i=0; i<_numberOfRCoords; i++)
		_rOfGrid[i] = rhs._rOfGrid[i];

	for(int i=0; i<_numberOfZCoords; i++)
		_zOfGrid[i] = rhs._zOfGrid[i];
	SetFields(rhs._inputBz, rhs._inputBr);
	return *this;
}


void SplineInterpolator::SetFields(double ** Bz, double ** Br)
{
	_inputBr = new double*[_numberOfRCoords];
	_inputBz = new double*[_numberOfRCoords];
	for(int i=0; i<_numberOfRCoords; i++)
	{
		_inputBr[i] = new double[_numberOfZCoords];
		_inputBz[i] = new double[_numberOfZCoords];
		for(int j=0; j<_numberOfZCoords; j++)
		{
			_inputBr[i][j] = Br[i][j];
			_inputBz[i][j] = Bz[i][j];
		}
	}
}

void SplineInterpolator::GetFieldValue(const double Point[4], double* Bfield) const
{

	//we want the field at z and r
  double z = Point[2];

  // Init to zero the field vector
  Bfield[0] = 0.0;
  Bfield[1] = 0.0;
  Bfield[2] = 0.0;

  // Bound checking
	//Far from field, B ~ 1/d^3
	if( (z<_zOfGrid[0])||(z>_zOfGrid[_numberOfZCoords-1] ) ) return;

  double r = sqrt( Point[0]*Point[0] + Point[1]*Point[1] );

  // Auxiliary variables for interpolation of spline objects in the r direction

  double dBr_dR, dBz_dR;

  //The subscripts of the r-values just above and just below our r
  int i = (int)(r / _dR);
  int j = i+1;

	//Bound checking
	//Bound checking in SplineIntepolator for cylindrical fields is more efficient
	//If we are exactly on the edge of the grid, j=i
	//If we are outside the edge of the grid, return with Bfield[]=0
	if(i >= _numberOfRCoords - 1)
	{
		if (i >= _numberOfRCoords) return;
		j--;
	}

  // Perform a linear interpolation.
	double brAtR_i = _brSplines[i](z);
	double brAtR_j = _brSplines[j](z);

	//Note dr is (still) the step size between grid nodes
  dBr_dR = (brAtR_j - brAtR_i) / _dR ;

	double br = (r - _rOfGrid[i]) * dBr_dR + brAtR_i;

  double bzAtR_i = _bzSplines[i](z);
  double bzAtR_j = _bzSplines[j](z);

  dBz_dR = ( bzAtR_j - bzAtR_i ) / ( _dR );

  double bz = (r - _rOfGrid[i]) * dBz_dR + bzAtR_i;

  // Calculate by projection the x,y,z components of the solenoidal field.
	if(r!=0)
	{
	  Bfield[0] = br * Point[0]/r;
	  Bfield[1] = br * Point[1]/r;
	}
  Bfield[2] = bz;

}

void SplineInterpolator::F(const Mesh::Iterator& Point, double BField[3]) const
{
	double PointA[4];
	Point.Position(PointA);
	PointA[2] = PointA[1];
	PointA[3] = PointA[1] = 0;
	GetFieldValue(PointA, BField);
}

