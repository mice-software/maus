// MAUS WARNING: THIS IS LEGACY CODE.
#include "Interface/Interpolator.hh"

#include "Interface/Spline1D.hh"
#include "Maths/Matrix.hh"
#include "Maths/Vector.hh"


std::string Interpolator3dSolenoidalTo3d::_interpolationAlgorithmString[3]= {"BiCubic", "LinearCubic", "BiLinear"};

Interpolator2dGridTo1d::Interpolator2dGridTo1d()
 :_coordinates(NULL), _F(NULL), _dFdX(NULL), _dFdY(NULL), _d2FdXdY(NULL)
{}

Interpolator2dGridTo1d::~Interpolator2dGridTo1d()
{
	Clear();
}

void Interpolator2dGridTo1d::Set(TwoDGrid* grid, 
                     double (*getF)(double point[2]), double (*getDFDX)(double point[2]), double (*getDFDY)(double point[2]), 
                     double (*getD2FDXDY)(double point[2]))
{
	SetGrid(grid);
	_F       = new double*[_coordinates->xSize()];
	_dFdX    = new double*[_coordinates->xSize()];
	_dFdY    = new double*[_coordinates->xSize()];
	_d2FdXdY = new double*[_coordinates->xSize()];

	for(int i=1; i<=_coordinates->xSize(); i++)
	{
		_F[i]       = new double[_coordinates->ySize()];
		_dFdX[i]    = new double[_coordinates->ySize()];
		_dFdY[i]    = new double[_coordinates->ySize()];
		_d2FdXdY[i] = new double[_coordinates->ySize()];

		for(int j=1; j<=_coordinates->ySize(); j++)
		{
			double point[2] = {_coordinates->x(i), _coordinates->y(j)};
			_F[i][j] = getF(point);
			if(!getDFDX);
			else;
			if(!getDFDY);
			else;
			if(!getD2FDXDY);
			else;
		}
	}
}

void Interpolator2dGridTo1d::Set(TwoDGrid* grid, double **F, double **dFdX, double ** dFdY, double **d2FdXdY)
{
	SetGrid(grid);
	_F = new double*[_coordinates->xSize()];
	_dFdX    = new double*[_coordinates->xSize()];
	_dFdY    = new double*[_coordinates->xSize()];
	_d2FdXdY = new double*[_coordinates->xSize()];
	for(int i=0; i<_coordinates->xSize(); i++)
	{
		_F[i]       = new double[_coordinates->ySize()];
		_dFdX[i]    = new double[_coordinates->ySize()];
		_dFdY[i]    = new double[_coordinates->ySize()];
		_d2FdXdY[i] = new double[_coordinates->ySize()];
	}

	for(Mesh::Iterator it=_coordinates->Begin(); it<_coordinates->End(); it++)
	{
		int i=it[0]-1, j=it[1]-1;

		_F[i][j] = F[i][j];

		if(dFdX!=NULL)
			_dFdX[i][j]    = dFdX[i][j];
		else if(i+1<_coordinates->xSize() ) 
			_dFdX[i][j]    = (F[i+1][j] - F[i][j])/(_coordinates->x(i+2) - _coordinates->x(i+1));
		else
			_dFdX[i][j] = 0.;

		if(dFdY!=NULL)
			_dFdY[i][j]    = dFdY[i][j];
		else if(j+1<_coordinates->ySize() )
			_dFdY[i][j]    = (F[i][j+1] - F[i][j])/(_coordinates->y(j+2) - _coordinates->y(j+1));
		else
			_dFdY[i][j] = 0.;

		if(d2FdXdY!=NULL)
			_d2FdXdY[i][j]    = d2FdXdY[i][j];
		else if(i+1<_coordinates->xSize() && j+1<_coordinates->ySize() )
			_d2FdXdY[i][j] = (F[i+1][j+1] - F[i][j])/(_coordinates->y(j+2) - _coordinates->y(j+1))/(_coordinates->x(i+2) - _coordinates->x(i+1));
		else
			_d2FdXdY[i][j] = 0.;
	}
}


void Interpolator2dGridTo1d::Clear()
{
	DeleteFunc(_F);
	DeleteFunc(_dFdX);
	DeleteFunc(_dFdY);
	DeleteFunc(_d2FdXdY);
	_coordinates->Remove(this);
}

void Interpolator2dGridTo1d::DeleteFunc(double** func)
{
	if(func==NULL) return; 
	for(int i=0; i<_coordinates->xSize(); i++)
		delete [] func[i]; 
	delete [] func; 
	func = NULL;
}

const int BiCubicInterpolator::_w[16][16] = {{1,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0},
                                       {0,0,0,0,    0,0,0,0,    1,0,0,0,    0,0,0,0},
                                       {-3,0,0,3,   0,0,0,0,    0,-2,0,-1,  0,0,0,0},
                                       {2,0,0,-2,   0,0,0,0,    1,0,0,1,    0,0,0,0},
                                       {0,0,0,0,    1,0,0,0,    0,0,0,0,    0,0,0,0},
                                       {0,0,0,0,    0,0,0,0,    0,0,0,0,    1,0,0,0},
                                       {0,0,0,0,   -3,0,0,3,    0,0,0,0,   -2,0,0,-1},
                                       {0,0,0,0,    2,0,0,-2,   0,0,0,0,    1,0,0,1},
                                       {-3,3,0,0,  -2,-1,0,0,   0,0,0,0,    0,0,0,0},
                                       {0,0,0,0,    0,0,0,0,   -3,3,0,0,   -2,-1,0,0},
                                       {9,-9,9,-9,  6,3,-3,-6,  6,-6,-3,3,  4,2,1,2},
                                       {-6,6,-6,6, -4,-2,2,4,  -3,3,3,-3,  -2,-1,-1,-2},
                                       {2,-2,0,0,   1,1,0,0,    0,0,0,0,    0,0,0,0},
                                       {0,0,0,0,    0,0,0,0,    2,-2,0,0,   1,1,0,0},
                                       {-6,6,-6,6, -3,-3,3,3,  -4,4,2,-2,  -2,-2,-1,-1},
                                       {4,-4,4,-4,  2,2,-2,-2,  2,-2,-2,2,  1,1,1,1}}; //from numerical recipes


//BiCubicInterpolation from Numerical Recipes
//Compute F(X,Y)
void BiCubicInterpolator::F(const double point[2], double value[1]) const
{
//	throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "BiCubic Interpolator does not work!", "BuCibicInterpolator::F"));
  std::cout << "Getting field at " << point[0] << " " << point[1] << std::endl;
	int       xStart;
	int       yStart;
	_coordinates->LowerBound(point[0], xStart, point[1], yStart);
	double    x0 = _coordinates->x(xStart+1);
	double    y0 = _coordinates->y(yStart+1);

	MAUS::Vector<double> f (16,0.);
	MAUS::Matrix<double> XY(16,16,0.);

	//// First calculate the polynomial
	//// This should be done when the BiCubic interpolator is built - but I am too lazy
	int m=0;
	int xmax = _coordinates->xSize();
	int ymax = _coordinates->ySize();
	for(int i=xStart-1; i<xStart+3 && i<xmax; i++)
		for(int j=yStart-1; j<yStart+3 && j<ymax; j++)
		{
			if(i>0 && j>0)
			{
                                std::cout << _coordinates->x(i+1) - x0 << " " << _coordinates->y(j+1) - y0 << " ** ";
				f(m+1) = _F[i][j];
				int n=0;
				for(int k=0; k<4; k++)
					for(int l=0; l<4; l++)
					{
						if(k>0 && l>0)
							XY(m+1,n+1) = pow(_coordinates->x(i+1)-x0, k) * pow(_coordinates->y(j+1)-y0, l);
						n++;
					}
			}
                        std::cout << m << " " << f(m+1) << std::endl;
			m++;
		}
	MAUS::Vector<double> a = inverse(XY) * f;
	std::cout << XY << transpose(f) << transpose(a);
	//// 

	//// Now calculate the value
	m = 0;
	for(int i=0; i<4 ; i++)
		for(int j=0; j<4; j++)
			{value[0] = pow(point[0]-x0, i)*pow(point[1]-y0, j)*a(m+1); m++;}
        std::cout << value[0] << std::endl;
}


void LinearCubicInterpolator::F(const double Point[2], double Value[1]) const
{
	int i=0;
	if(Point[0]==_coordinates->MaxX()) {Value[0] = _splines.back()(Point[1]); return;}
	if(Point[0]==_coordinates->MinX()) {Value[0] = _splines[0]    (Point[1]); return;}
	_coordinates->xLowerBound(Point[0], i);
	// Perform a linear interpolation.
	double fAtX_i = _splines[i](Point[1]);
	double fAtX_j = _splines[i+1](Point[1]);
	double dFdX = (fAtX_j - fAtX_i) /  (_coordinates->x(i+2)-_coordinates->x(i+1));
	Value[0] = (Point[0] - _coordinates->x(i+1))*dFdX + fAtX_i;
}

void LinearCubicInterpolator::SetSplines()
{
	int a = 0;	_coordinates->xLowerBound(0, a);
	_coordinates->xLowerBound(0.1, a);
	_coordinates->xLowerBound(_coordinates->MaxX()-0.1, a);
	for(int i=0; i<_coordinates->xSize(); i++)
	{
		double * y = _coordinates->newYArray();
		_splines.push_back(Spline1D(_coordinates->ySize(), y, _F[i]) );
		delete [] y;
	}
}

void BiLinearInterpolator::F(const double Point[2], double Value[1]) const
{
	int i=0, j=0;
	int xSize = _coordinates->xSize();
	int ySize = _coordinates->ySize();
	_coordinates->LowerBound(Point[0], i, Point[1], j);
	if(Point[0]==_coordinates->MinX()) i = 0;
	if(Point[1]==_coordinates->MinY()) j = 0;

	//for this interpolation I triangulate the 2d grid and interpolate across each triangle
	if(i+1 != xSize && j+1 != ySize)
	{
		//to make the function continuous: 
		//if point is above line from f10 to f01 use f01,f10,f11 to define interpolation
		//if point is below use f01,f10,f00 to define interpolation
		double x1=_coordinates->x(i+2), x2=_coordinates->x(i+1), y1=_coordinates->y(j+1), y2=_coordinates->y(j+2); 
		//if y < yMid I am below the line connecting (i+1,j) to (j+1,i)
		double yMid = (Point[0]-x2)*(y1-y2)/(x1-x2)+y2;
		if(Point[1] < yMid)
		{
			double f00      = _F[i][j];
			Value[0] = (Point[0]-_coordinates->x(i+1))*(_F[i+1][j]-f00)/(_coordinates->x(i+2) - _coordinates->x(i+1))+
			           (Point[1]-_coordinates->y(j+1))*(_F[i][j+1]-f00)/(_coordinates->y(j+2) - _coordinates->y(j+1))+
						f00;
		}
		else
		{
			double f11      = _F[i+1][j+1];
			Value[0] = -(_coordinates->x(i+2)-Point[0])*(f11-_F[i][j+1])/(_coordinates->x(i+2) - _coordinates->x(i+1))+
			           -(_coordinates->y(j+2)-Point[1])*(f11-_F[i+1][j])/(_coordinates->y(j+2) - _coordinates->y(j+1))+
						f11;
		}
		return;
	}
	else if(i+1 == xSize && j+1 == ySize) //on x and y boundary
	{
		Value[0] = _F[i][j];
	}
	else if(i+1 != xSize) //on y boundary so interpolate only in x
		Value[0] = (Point[0]-_coordinates->x(i+1))*(_F[i+1][j]-_F[i][j])/(_coordinates->x(i+2) - _coordinates->x(i+1))+_F[i][j];
	else if(j+1 != ySize) //on x boundary so interpolate only in y
		Value[0] = (Point[1]-_coordinates->y(j+1))*(_F[i][j+1]-_F[i][j])/(_coordinates->y(j+2) - _coordinates->y(j+1))+_F[i][j];

}


Interpolator3dSolenoidalTo3d::Interpolator3dSolenoidalTo3d(TwoDGrid* grid, double ** Br, double ** Bz, std::string algorithm)
{
	SetGrid(grid);
	interpolationAlgorithm algo = Algorithm(algorithm);
	switch(algo)
	{
		case linearCubic:
			_interpolator[0] = new LinearCubicInterpolator(grid, Br);
			_interpolator[1] = new LinearCubicInterpolator(grid, Bz);
			break;
		case biLinear:
			_interpolator[0] = new BiLinearInterpolator(grid, Br);
			_interpolator[1] = new BiLinearInterpolator(grid, Bz);
			break;
		case biCubic:
			_interpolator[0] = new BiCubicInterpolator(grid, Br);
			_interpolator[1] = new BiCubicInterpolator(grid, Bz);
			break;
		default:
			throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Interpolation algorithm not recognised", "Interpolator3dSolenoidalTo3d::Interpolator3dSolenoidalTo3d"));
	}

}

void Interpolator3dSolenoidalTo3d::F(const double Point[3], double Value[3]) const
{
	if(fabs(Point[2]) > _coordinates->MaxY()) return;
	double cylPoint[2] = {Point[0]*Point[0]+Point[1]*Point[1], Point[2]}; 
	if(fabs(cylPoint[0]) > _coordinates->MaxX()*_coordinates->MaxX()) return;
	cylPoint[0] = sqrt(cylPoint[0]);
	double       cylValue[2] = {0, 0};
	_interpolator[0]->F(cylPoint, &cylValue[0]); 
	_interpolator[1]->F(cylPoint, &cylValue[1]); 

	Value[2] = cylValue[1];
	if(cylPoint[0]>0)
	{
		Value[0] = cylValue[0]*Point[0]/cylPoint[0]; //Bx = Br*x/r
		Value[1] = cylValue[0]*Point[1]/cylPoint[0]; //By = Br*y/r
	}
}


void Interpolator3dSolenoidalTo3d::F(const Mesh::Iterator& Point, double BField[3]) const
{
	double PointA[3];
	Point.Position(PointA);
	PointA[2] = PointA[1];
	PointA[1] = 0;
	F(PointA, BField);
}


Interpolator3dSolenoidalTo3d::~Interpolator3dSolenoidalTo3d() 
{
	if(_interpolator[0]) delete _interpolator[0]; 
	if(_interpolator[1]) delete _interpolator[1];
  if(_coordinates) _coordinates->Remove(this); 
}


Interpolator3dSolenoidalTo3d::interpolationAlgorithm Interpolator3dSolenoidalTo3d::Algorithm(std::string algo)
{
	for(int i=0; i<3; i++) 
		if(_interpolationAlgorithmString[i]==algo) 
			return interpolationAlgorithm(i); 
	throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Did not recognise interpolation algorithm "+algo, "Interpolator3dSolenoidalTo3d::Algorithm") );
}




