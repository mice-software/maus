// MAUS WARNING: THIS IS LEGACY CODE.
//Interpolator definitions
//A robust framework for arbitrary interpolation
//Thinking about field maps but should be extensible to other interpolation types
//designed to permit implementation of interpolations that e.g. see Maxwell's laws 
//or e.g. have dynamically sized grids with minimal reworking of user code 
//and optimisable for speed

#ifndef INTERPOLATOR_HH
#define INTERPOLATOR_HH

#include <math.h>
#include <vector>

#include "Interface/Mesh.hh"
#include "Interface/Spline1D.hh"
#include "Interface/Interpolation/VectorMap.hh"
#include "Interface/Interpolation/Interpolator3dGridTo1d.hh"
#include "Interface/Interpolation/TriLinearInterpolator.hh"
#include "Interface/Interpolation/Interpolator3dGridTo3d.hh"

#include "gsl/gsl_sf_gamma.h"

///// Function //////

//Wrapper for a function pointer
class Function : public VectorMap
{
public:
	Function(void (*function)(const double*, double* ), int inSize, int outSize) : _function(function), _inSize(inSize), _outSize(outSize) {;}
	~Function() {;}
	//return map value; vectors NOT checked for size
	void  F    (const double*   point,       double* value) const {_function(point, value);}
	//Checks for self-consistency
	inline bool  CheckPoint(const std::vector<double>& point) const {return (point.size() == PointDimension());}
	inline bool  CheckValue(const std::vector<double>& value) const {return (value.size() == PointDimension());}
	//Tell me the required dimension of the input point and output value
	unsigned int PointDimension()         const {return _inSize;}
	unsigned int ValueDimension()         const {return _outSize;}
	//Read and write operations
	VectorMap* Clone()       const {return new Function(*this);}
private:
	void   (*_function)(const double*, double*);
	int    _inSize;
	int    _outSize;

};

//Wrapper for a member function pointer (pointer to member of class T)
//Sounds like member function pointers are murky and compiler dependent, caveat emptor
template<class T> 
class MemberFunction : public VectorMap
{
public:
	MemberFunction( T& object, void (T::*function)(const double*, double*), int inSize, int outSize)
	              : _function(function), _object(object), _inSize(inSize), _outSize(outSize) {;}
	~MemberFunction() {;}
	//return map value; vectors NOT checked for size
	void  F    (const double*   point,       double* value) const { (_object.*_function)(point, value);}
	//Checks for self-consistency
	inline bool  CheckPoint(const std::vector<double>& point) const {return (point.size() == PointDimension());}
	inline bool  CheckValue(const std::vector<double>& value) const {return (value.size() == PointDimension());}
	//Tell me the required dimension of the input point and output value
	unsigned int PointDimension()         const {return _inSize;}
	unsigned int ValueDimension()         const {return _outSize;}
	//Pseudo-copy constructor for the VectorMap
	VectorMap* Clone()       const {return new MemberFunction(*this);}
private:
	void   (T::*_function)(const double*, double*);
	T&     _object;
	int    _inSize;
	int    _outSize;
};

template<class T>
class MemberFunctionConst : public VectorMap
{
public:
  MemberFunctionConst( T& object, void (T::*function)(const double*, double*) const, int inSize, int outSize)
                : _function(function), _object(object), _inSize(inSize), _outSize(outSize) {;}
  ~MemberFunctionConst() {;}
  //return map value; vectors NOT checked for size
  void  F    (const double* point, double* value) const { (_object.*_function)(point, value);}
  //Checks for self-consistency
  inline bool  CheckPoint(const std::vector<double>& point) const {return (point.size() == PointDimension());}
  inline bool  CheckValue(const std::vector<double>& value) const {return (value.size() == PointDimension());}
  //Tell me the required dimension of the input point and output value
  unsigned int PointDimension() const {return _inSize;}
  unsigned int ValueDimension() const {return _outSize;}
  //Pseudo-copy constructor for the VectorMap
  VectorMap* Clone() const {return new MemberFunctionConst(*this);}
private:
  void   (T::*_function)(const double*, double*) const;
  T&     _object;
  int    _inSize;
  int    _outSize;
};

///// Interpolator2dGridTo1d /////

class Interpolator2dGridTo1d : public VectorMap
{
public:
	//Constructor for grids with constant spacing; 2D arrays go like [index_r][index_z]
	Interpolator2dGridTo1d(TwoDGrid* grid, double (*getF)(double point[2])) : _coordinates(NULL)
	{Set(grid, getF);}
	Interpolator2dGridTo1d(TwoDGrid* grid, double **F) : _coordinates(NULL)
	{Set(grid, F);}
	//Other operators
	Interpolator2dGridTo1d();
	virtual ~Interpolator2dGridTo1d();

	virtual void F(const double        Point[2], double Value[1]) const = 0;

	inline int       NumberOfXCoords() const {return _coordinates->xSize();}
	inline int       NumberOfYCoords() const {return _coordinates->ySize();}
	unsigned int     PointDimension()  const {return 2;}
	unsigned int     ValueDimension()  const {return 1;}

	TwoDGrid* GetMesh()                       {return _coordinates;}
	TwoDGrid* GetGrid()                       {return _coordinates;}
	void      SetGrid(TwoDGrid* grid)         {if(_coordinates!=NULL) _coordinates->Remove(this); grid->Add(this); _coordinates = grid;}
	void      SetX(int nCoords, double* x)    {if(_coordinates!=NULL) _coordinates->SetX(nCoords, x);}
	void      SetY(int nCoords, double* y)    {if(_coordinates!=NULL) _coordinates->SetY(nCoords, y);}
	void      SetF      (double** inF)        {DeleteFunc(_F);       _F       = inF;}
	void      SetDFDX   (double** inDFDX)     {DeleteFunc(_dFdX);    _dFdX    = inDFDX;}
	void      SetDFDY   (double** inDFDY)     {DeleteFunc(_dFdY);    _dFdY    = inDFDY;}
	void      SetD2FDXDY(double** inD2FDXDY)  {DeleteFunc(_d2FdXdY); _d2FdXdY = inD2FDXDY;}
	void      DeleteFunc(double** func);       

	//if the functions aren't define, use these instead in constructor
	static double DFDXBackup   (double (*getF)(double point[2]), double point[2]);
	static double DFDYBackup   (double (*getF)(double point[2]), double point[2]);
	static double D2FDXDYBackup(double (*getF)(double point[2]), double point[2]);

	inline double ** F()         const {return _F;}
	inline double ** DFDX()      const {return _dFdX;}
	inline double ** DFDY()      const {return _dFdY;}
	inline double ** D2FDXDY()   const {return _d2FdXdY;}

	void             Set(TwoDGrid* grid, 
	                     double (*getF)(double point[2]), double (*getDFDX)(double point[2])=NULL, double (*getDFDY)(double point[2])=NULL, 
	                     double (*getD2FDXDY)(double point[2])=NULL);
	void             Set(TwoDGrid* grid, double ** F, double **dFdX=NULL, double ** dFdY=NULL, double **d2FdXdY=NULL);

	void             Clear();

	enum             interpolationAlgorithm{biCubic, linearCubic};
protected:
	TwoDGrid      *_coordinates;
	double       **_F;
	double       **_dFdX;
	double       **_dFdY;
	double       **_d2FdXdY;
//	int            _numberOfXCoords, _numberOfYCoords;
};

class Spline1D;

///// Interpolator3dSolenoidalTo3d ///////
//specific case where the value has cylindrical symmetry and no component in phi (tangential) direction
class Interpolator3dSolenoidalTo3d : public VectorMap
{
	enum                    interpolationAlgorithm{biCubic, linearCubic, biLinear};
public:
	Interpolator3dSolenoidalTo3d(TwoDGrid* grid, double ** Br, double ** Bz, std::string interpolationAlgorithm="LinearCubic");
	Interpolator3dSolenoidalTo3d() : _interpolator(), _coordinates(NULL) {;}
	~Interpolator3dSolenoidalTo3d();

	//Build interpolator from component info
	//Return 3 vector from Point[2]; 
	virtual void F(const double Point[3], double Value[3]) const;
	virtual void F(const Mesh::Iterator& Point, double BField[3]) const;
	//Utility methods
	virtual Interpolator3dSolenoidalTo3d* Clone()    const {return new Interpolator3dSolenoidalTo3d(*this);}
	unsigned int PointDimension()                    const {return 3;}
	unsigned int ValueDimension()                    const {return 3;}
	TwoDGrid*    GetMesh()                                 {return _coordinates;}
	TwoDGrid*    GetGrid()                                 {return _coordinates;}
	void         SetGrid(TwoDGrid* grid, bool regular=true){_coordinates = grid; _coordinates->Add(this);}

private:
	static interpolationAlgorithm Algorithm(std::string algo);
	static std::string            Algorithm(interpolationAlgorithm algo) {return _interpolationAlgorithmString[int(algo)];}

	Interpolator2dGridTo1d* _interpolator[2];
	TwoDGrid*               _coordinates;
	static std::string      _interpolationAlgorithmString[3];
};


////// BiCubicInterpolator ////////
//BUG THIS DOES NOT WORK!
class BiCubicInterpolator : public Interpolator2dGridTo1d
{
public:
	BiCubicInterpolator(TwoDGrid* grid, double (*getF)(double point[2])) : Interpolator2dGridTo1d(grid, getF) {;}
	BiCubicInterpolator(TwoDGrid* grid, double **F) : Interpolator2dGridTo1d(grid, F) {;}
	~BiCubicInterpolator() {}

	//GetFieldValue at a Point relative to multipole start
	void F(const double Point[2], double Value[1]) const;
	//Utility methods
	BiCubicInterpolator* Clone() const {return new BiCubicInterpolator(*this);}

private:
	void   BiCubicCoefficients(double f[4], double dfdx[4], double dfdz[4], double d2fdxdz[4], double xLength, double yLength, double* coeffOut) const;

	double Extrapolation(const double point[4], int axis) const {return 0;}

	//field and derivatives
	static const int _w[16][16];
};


////// LinearCubicInterpolator (SplineInterpolator) ////////
class LinearCubicInterpolator : public Interpolator2dGridTo1d
{
public:
	LinearCubicInterpolator(TwoDGrid* grid, double (*getF)(double point[2])) : Interpolator2dGridTo1d(grid, getF) {SetSplines();}
	LinearCubicInterpolator(TwoDGrid* grid, double **F) : Interpolator2dGridTo1d(grid, F) {SetSplines();}
	~LinearCubicInterpolator() {}

	//GetFieldValue at a Point relative to multipole start
	void F(const double Point[2], double Value[1]) const;
	//Utility methods
	LinearCubicInterpolator* Clone() const {return new LinearCubicInterpolator(*this);}

private:
	void LinearCubicInterpolation(const double Point[2], double Value[1]) const;
	void SetSplines();

	std::vector<Spline1D> _splines;
};


////// BiLinearInterpolator ////////
class BiLinearInterpolator : public Interpolator2dGridTo1d
{
public:
	BiLinearInterpolator(TwoDGrid* grid, double (*getF)(double point[2])) : Interpolator2dGridTo1d(grid, getF) {;}
	BiLinearInterpolator(TwoDGrid* grid, double **F) : Interpolator2dGridTo1d(grid, F) {;}
	~BiLinearInterpolator() {}

	//GetFieldValue at a Point relative to multipole start
	void F(const double Point[2], double Value[1]) const;
	//Utility methods
	BiLinearInterpolator* Clone() const {return new BiLinearInterpolator(*this);}

private:
	std::vector<Spline1D> _splines;
};

#endif
