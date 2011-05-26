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

#include "Interface/Squeal.hh"
#include "Interface/Mesh.hh"
#include "Interface/Spline1D.hh"

#include "gsl/gsl_sf_gamma.h"

///// VectorMap  /////

//Abstract base class
//Maps from arbitrary length array of doubles to arbitrary length array of double e.g. map position vector to field vector
class VectorMap
{
public:
	//return map value; vectors NOT checked for size
	virtual void  F    (const double*   point,       double* value)              const = 0;
	virtual void  F    (const Mesh::Iterator& point, double* value)              const //overload if mesh::pointdimension != vectormap::pointdimension 
	{double* PointA = new double[this->PointDimension()]; point.Position(PointA); F(PointA, value); delete PointA;}
  //Compare the length of in and out; if in is longer, append new items in out until they are the same length NOT TESTED YET
  virtual void FAppend(const std::vector< std::vector<double> >& point_vec, std::vector< std::vector<double> >& value_vec) const;
	//Checks for self-consistency
	inline virtual bool  CheckPoint(const std::vector<double>& point) const {return (point.size() == PointDimension());}
	inline virtual bool  CheckValue(const std::vector<double>& value) const {return (value.size() == ValueDimension());}
	//Tell me the required dimension of the input point and output value
	virtual unsigned int PointDimension()         const = 0; //would like to make static! But can't inherit static functions
	virtual unsigned int ValueDimension()         const = 0;
	//Read and write operations
	virtual              VectorMap* Clone()       const = 0; //copy function
	virtual             ~VectorMap() {;}

	//If VectorMap uses a Mesh return it else return NULL
	virtual Mesh*        GetMesh()                const {return NULL;}
private:
};


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
	Interpolator3dSolenoidalTo3d(Interpolator2dGridTo1d* interpol[2]);
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
	void         SetGrid(TwoDGrid* grid, bool regular=true){_coordinates = grid;}

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

//////// Interpolator3dGridTo1d ////////
class Interpolator3dGridTo1d : public VectorMap
{
public:
	//Constructor for grids with constant spacing; 3D arrays go like [index_x][index_y][index_z]
	Interpolator3dGridTo1d(ThreeDGrid* grid, double ***F) : _coordinates(NULL), _F(NULL)
	{Set(grid, F);}
	Interpolator3dGridTo1d() : _coordinates(NULL), _F(NULL) {}

	virtual ~Interpolator3dGridTo1d() {Clear();}

	virtual void F(const double Point[3], double Value[1]) const = 0;
	virtual Interpolator3dGridTo1d* Clone() const = 0;


	inline int       NumberOfXCoords() const {return _coordinates->xSize();}
	inline int       NumberOfYCoords() const {return _coordinates->ySize();}
	inline int       NumberOfZCoords() const {return _coordinates->zSize();}
	unsigned int     PointDimension()  const {return 3;}
	unsigned int     ValueDimension()  const {return 1;}

	ThreeDGrid* GetMesh()                     {return _coordinates;}
	ThreeDGrid* GetGrid()                     {return _coordinates;}
	void      SetGrid(ThreeDGrid* grid)       {if(_coordinates!=NULL) _coordinates->Remove(this); grid->Add(this); _coordinates = grid;}
	void      SetX(int nCoords, double* x)    {if(_coordinates!=NULL) _coordinates->SetX(nCoords, x);}
	void      SetY(int nCoords, double* y)    {if(_coordinates!=NULL) _coordinates->SetY(nCoords, y);}
	void      SetZ(int nCoords, double* z)    {if(_coordinates!=NULL) _coordinates->SetZ(nCoords, z);}
	void      SetF      (double*** inF)       {DeleteFunc(_F); _F = inF;}
	void      DeleteFunc(double*** func);

	inline double*** F()         const {return _F;}
	void             Set(ThreeDGrid* grid, double *** F) {SetGrid(grid); SetF(F);}

	void             Clear() { DeleteFunc(_F); _coordinates->Remove(this);}

	enum             interpolationAlgorithm{biCubic, linearCubic};
protected:
	ThreeDGrid   *_coordinates;
	double     ***_F;
};

//////// TriLinearInterpolator ///////
class TriLinearInterpolator : public Interpolator3dGridTo1d
{
public:
	TriLinearInterpolator(ThreeDGrid *grid, double ***F) : Interpolator3dGridTo1d(grid, F) {}
	TriLinearInterpolator(const TriLinearInterpolator&);
	~TriLinearInterpolator() {;}
	void F(const double Point[3], double Value[1]) const;
	TriLinearInterpolator* Clone() const {return new TriLinearInterpolator(*this);}

};

//////// Interpolator3dGridTo3d //////////
class Interpolator3dGridTo3d : public VectorMap
{
public:
	enum        interpolationAlgorithm{triLinear};

	//Constructor for grids with constant spacing; 3D arrays go like [index_x][index_y][index_z]
  //Takes ownership of memory of Bx, By, Bz
  Interpolator3dGridTo3d(ThreeDGrid* grid, double ***Bx, double ***By, double ***Bz, interpolationAlgorithm algo=triLinear) : _coordinates(NULL)
	{for(int i=0; i<3; i++) _interpolator[i] = NULL; Set(grid, Bx, By, Bz);}
	Interpolator3dGridTo3d(const Interpolator3dGridTo3d&);
	~Interpolator3dGridTo3d() {Clear();}
	Interpolator3dGridTo3d* Clone() const {return new Interpolator3dGridTo3d(*this);}

	void F(const double Point[3], double Value[3]) const;

	inline int       NumberOfXCoords() const {return _coordinates->xSize();}
	inline int       NumberOfYCoords() const {return _coordinates->ySize();}
	inline int       NumberOfZCoords() const {return _coordinates->zSize();}
	unsigned int     PointDimension()  const {return 3;}
	unsigned int     ValueDimension()  const {return 3;}

	ThreeDGrid* GetMesh()                    {return _coordinates;}
	ThreeDGrid* GetGrid()                    {return _coordinates;}
	void        SetGrid(ThreeDGrid* grid)       {if(_coordinates!=NULL) _coordinates->Remove(this); grid->Add(this); _coordinates = grid;}
	void        SetX(int nCoords, double* x)    {if(_coordinates!=NULL) _coordinates->SetX(nCoords, x);}
	void        SetY(int nCoords, double* y)    {if(_coordinates!=NULL) _coordinates->SetY(nCoords, y);}
	void        SetZ(int nCoords, double* z)    {if(_coordinates!=NULL) _coordinates->SetZ(nCoords, z);}

	void        Set(ThreeDGrid* grid, double *** Bx, double *** By, double *** Bz, interpolationAlgorithm algo=triLinear);
	void        Clear() {for(int i=0; i<3; i++) delete _interpolator[i]; _coordinates->Remove(this);}

protected:
	ThreeDGrid             *_coordinates;
	Interpolator3dGridTo1d *_interpolator[3];
};

inline void VectorMap::FAppend(const std::vector< std::vector<double> >& point_vec, std::vector< std::vector<double> >& value_vec) const
{
  for(size_t i=value_vec.size(); i<point_vec.size(); i++) {
    value_vec.push_back(std::vector<double>(ValueDimension()) );
    F( &point_vec[i][0], &value_vec[i][0] );
  }
}



#endif
