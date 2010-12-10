#include "BTMultipole.hh"
#include "Interface/Squeal.hh"
#include "Interface/Squeak.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_sf_pow_int.h"
#include "gsl/gsl_odeiv.h"
#include "gsl/gsl_errno.h"

using CLHEP::pi;
using CLHEP::c_light;

const BTMultipole * BTMultipole::staticMultipole = NULL;
double              BTMultipole::staticDX        = 0.;
double              BTMultipole::_relativeError  = 1.e-5;
double              BTMultipole::_absoluteError  = 1.e-5;
double              BTMultipole::_stepSize       = 1.;



//length is the length along the midpoint
BTMultipole::BTMultipole(int pole, double fieldAtPoleTip, double length, double radiusOfCurvature, double height, double width, std::string curvature)
        : 	_pole(pole), _magnitude(1.), _length(length), _rCurv(radiusOfCurvature), 
            _height(height), _width(width), 
            _endLength(0), _endField(hardEdged), _curvature(constant)
{
	if(fabs(radiusOfCurvature)<1e-5 || curvature=="Straight") _curvature=straight;
	else if(curvature=="MomentumBased")             
		throw(Squeal(Squeal::recoverable, "Momentum based multipole must have Enge end field", "BTMultipole::BTMultipole(...)"));
	else
	{
		if(curvature=="StraightEnds")              _curvature = straightEnds;
		if(curvature=="Constant" || curvature=="") _curvature = constant;
		if(_width >= fabs(_rCurv)) 
			throw(Squeal(Squeal::nonRecoverable, "Multipole with width > radius of curvature", 
			      "BTMultipole::BTMultipole"));
		if(_length > fabs(2*pi*_rCurv)) 
			throw(Squeal(Squeal::nonRecoverable, "Multipole with bending angle > 360 degrees",
			             "BTMultipole::BTMultipole"));
		_endRotation = HepRotation(0,_length/(2*pi*_rCurv),0);
		_endPoint    = _endRotation*Hep3Vector(_rCurv,0,_length) - Hep3Vector(_rCurv,0,0);
	}
	//Automatically set scaling using field at pole tip
	Hep3Vector pos      = Hep3Vector(width/2., 0., -_length/2.);
	double     field[6] = {0., 0., 0., 0., 0., 0.};
	HardEdgedFieldValue(pos, field);
	_magnitude = fieldAtPoleTip/sqrt(field[0]*field[0]+field[1]*field[1]+field[2]*field[2]);
	BTField::bbMin[0] = -_width /2.; BTField::bbMin[1] = -_height/2.; BTField::bbMin[2] = 0.; 
	BTField::bbMax[0] = +_width /2.; BTField::bbMax[1] = +_height/2.; BTField::bbMax[2] = +_length;
}

BTMultipole::BTMultipole(int pole, double magnitudeVariable, double length, double radiusVariable, double height, double width,
                         double effectiveWidth, double endLength, int endPole, std::vector<double> engeParameters, std::string curvature)
        : 	_pole(pole), _magnitude(1.), _effectiveWidth(effectiveWidth), 
            _length(length), _rCurv(radiusVariable), _height(height), _width(width), _endLength(endLength), 
            _engeParameters(engeParameters), _endField(enge), _curvature(constant), _endFieldPole(endPole)
{
	if(curvature=="MomentumBased")
	{
		_curvature = momentumBased; 
		_momentum  = radiusVariable;
		_rCurv     = 0;
		SetupDipoleMagnitude(radiusVariable, magnitudeVariable);
		SetupReferenceTrajectory();
	}
	else
	{
		if(fabs(radiusVariable)<1e-5 || curvature=="Straight") 
			_curvature=straight;
		else
		{
			if(curvature=="StraightEnds")              _curvature = straightEnds;
			if(curvature=="Constant" || curvature=="") _curvature = constant;
			if(_width >= fabs(_rCurv)) 
				throw(Squeal(Squeal::nonRecoverable, "Multipole with width > radius of curvature", 
				      "BTMultipole::BTMultipole"));
			if(_length > fabs(2*pi*_rCurv)) 
				throw(Squeal(Squeal::nonRecoverable, "Multipole with bending angle > 360 degrees",
				             "BTMultipole::BTMultipole"));
			_endRotation = HepRotation(Hep3Vector(0,1,0), _length/_rCurv);
			_endPoint    = _endRotation*Hep3Vector(-_rCurv,0,0) + Hep3Vector(_rCurv,0,0);
		}
		//Automatically set scaling using field at pole tip
		Hep3Vector pos      = Hep3Vector(width/2., 0., -_length/2.);
		double     field[6] = {0., 0., 0., 0., 0., 0.};
		GetFieldValueRotated(pos, field);
		_magnitude = magnitudeVariable/sqrt(field[0]*field[0]+field[1]*field[1]+field[2]*field[2]);
	}

}

BTMultipole BTMultipole::DipoleFromMomentum(double momentum, double length, double angle, double height, double width)
{
	double field  = (2*pi*angle/360.)*momentum/length/CLHEP::eplus/c_light; // theta p / (qL)
	double rCurv  = length*360./(2*pi*angle);       // L = 2 pi r*(theta/360)
	return BTMultipole(1, field, length, rCurv, height, width, "constant");
}

Hep3Vector BTMultipole::TransformToRotated(const double * Point) const
{
	switch(_curvature)
	{
		case straight:
			return Hep3Vector(Point[0], Point[1], fabs(Point[2]-_length/2.)-0.5*_length);
		case constant:
			return TransformToRotatedConstant(Point);
		case straightEnds:
			return TransformToRotatedStraightEnds(Point);
		case momentumBased:
			return TransformToRotatedMomentumBased(Point);
		default:
			throw(Squeal(Squeal::nonRecoverable, "Did not recognise curvature model", "BTMultipole::TransformToRotated(const double*)"));
	}
}

Hep3Vector BTMultipole::TransformToRotatedMomentumBased(const double point[3]) const
{
	int    p0             = 0;
	int    p1             = _refX.size()-1;
	double rSquaredStart  = (point[0]-_refX[p0])*(point[0]-_refX[p0]) + (point[2]-_refZ[p0])*(point[2]-_refZ[p0]);
	double rSquaredEnd    = (point[0]-_refX[p1])*(point[0]-_refX[p1]) + (point[2]-_refZ[p1])*(point[2]-_refZ[p1]);
	//This is horrid!!! Minimise rSquared (using binary interpolation) to find s(x,z)
	//Assumes sign(B) == const st dRSquared is always increasing or decreasing
	while(unsigned(p1-p0)>1)
	{
		int    pM             = (p1+p0)/2;	
		double rSquaredMiddle = (point[0]-_refX[pM])*(point[0]-_refX[pM]) + (point[2]-_refZ[pM])*(point[2]-_refZ[pM]);
		if(rSquaredStart>rSquaredEnd) 
		{
			rSquaredStart = rSquaredMiddle;
			p0            = pM;
		}
		else 
		{
			rSquaredEnd  = rSquaredMiddle;
			p1           = pM;
		}
	}
	double deltaR = sqrt(rSquaredStart);
	if(point[0]-_refX[p0]>0.) deltaR*= -1;
	double deltaS = _refS[p0];
	if(deltaS < _length/2.) deltaS *= -1;
	else                    deltaS -= _length;
	return Hep3Vector(deltaR, point[1], deltaS);
}

Hep3Vector BTMultipole::TransformToRotatedConstant(const double * Point) const
{
	double x     = Point[0];
	double z     = Point[2];
	double deltaR = sqrt(z*z + (x+_rCurv)*(x+_rCurv) ) - fabs(_rCurv);
	x += _rCurv;
	if(_rCurv<0.) x*=-1;
	double theta  = atan2(z, x);
	double deltaS = fabs(_rCurv)*theta;
	if(deltaS < _length/2.) deltaS *= -1.;
	else                    deltaS -= _length;
	return Hep3Vector(deltaR, Point[1], deltaS);
}

Hep3Vector BTMultipole::TransformToRotatedStraightEnds(const double * Point) const
{
	if(Point[2]<0.) 
		return Hep3Vector(Point[0], Point[1], fabs(Point[2]));
	Hep3Vector aPoint(Point[0], Point[1], Point[2]);
	aPoint  = _endRotation*aPoint;
	aPoint -= _endPoint;
	if(aPoint[2]>0.)
		return aPoint;

	double x      = Point[0];
	double z      = Point[2];
	double deltaR = sqrt(z*z + (x+_rCurv)*(x+_rCurv) ) - fabs(_rCurv);
	x += _rCurv;
	if(_rCurv<0.) x*=-1;
	double theta  = atan2(z, x);
	double deltaS = fabs(_rCurv)*theta;
	if(deltaS < _length/2.) deltaS *= -1.;
	else                    deltaS -= _length;
	return Hep3Vector(deltaR, Point[1], deltaS);
}

double BTMultipole::RadiusOfCurvature(double sRelativeToEnd) const
{
	switch(_curvature)
	{
		case straight:
			return 0;
		case constant:
			return _rCurv;
		case straightEnds:
			if(sRelativeToEnd > 0.) return 0;
			else return _rCurv;
		case momentumBased:
		{
			double EMfield[3] = {0,0,0};
			GetFieldValueRotated(Hep3Vector(0,0,sRelativeToEnd), EMfield );
			return _momentum/EMfield[1]/CLHEP::eplus/CLHEP::c_light;
		}
		default:
			throw(Squeal(Squeal::nonRecoverable, "Did not recognise curvature model", "BTMultipole::RadiusOfCurvature(double)"));		
	}
}

void BTMultipole::TransformToRectangular(const double point[3], double* value) const
{
	switch(_curvature)
	{
		case straight:
			return;
		case constant:
			return TransformToRectangularConstant(point, value);
		case straightEnds:
			return TransformToRectangularStraightEnds(point, value);
		case momentumBased:
			return TransformToRectangularMomentumBased(point, value);
		default:
			throw(Squeal(Squeal::nonRecoverable, "Did not recognise curvature model", "BTMultipole::TransformToRectangular(const double*, double*)"));
	}
}

void       BTMultipole::TransformToRectangularConstant(const double point[3], double* value) const
{
	double theta  = atan2(point[2], point[0]+_rCurv);
	double bx     = value[2]*sin(theta) + value[0]*cos(theta);
	double bz     = value[2]*cos(theta) + value[0]*sin(theta);
	value[0] = bx;
	value[2] = bz;
}

void       BTMultipole::TransformToRectangularStraightEnds(const double point[3], double* value) const
{
	if(point[2]<0) return;
	double theta = 0;
	if(point[2]>_endPoint[2])
		theta = _endRotation.delta();
	else
		theta  = atan2(point[2], point[0]+_rCurv);
	double bx     = value[2]*sin(theta) + value[0]*cos(theta);
	double bz     = value[2]*cos(theta) + value[0]*sin(theta);
	value[0] = bx;
	value[2] = bz;
}

void  BTMultipole::TransformToRectangularMomentumBased(const double point[3], double* value) const
{	
	int step      = int(point[2]/_stepSize)+1;
	double theta  = 0;
	if(step > int(_angle.size())) theta = _angle.back();
	else if(step > 0)             theta = _angle[step];
	double bx     = value[2]*sin(theta) + value[0]*cos(theta);
	double bz     = value[2]*cos(theta) + value[0]*sin(theta);
	value[0] = bx;
	value[2] = bz;
}

void BTMultipole::GetFieldValueRotated( Hep3Vector pos, double *EMfield ) const
{
	switch(_endField)
	{
		case hardEdged:
			HardEdgedFieldValue(pos, EMfield);
			break;
		case enge:
			EngeFieldValue(pos, EMfield);
			break;
		default:
			throw(Squeal(Squeal::nonRecoverable, "Multipole end field model not implemented", "BTMultipole::GetFieldValue(double*, double*)"));
	}

}

void BTMultipole::GetFieldValue( const double Point[4], double *EMfield ) const
{
	Hep3Vector pos = TransformToRotated(Point);
	GetFieldValueRotated  ( pos, EMfield );
	TransformToRectangular(Point, EMfield);
	return;
}

//pos is the position from the radius of curvature arc relative to the centre of the multipole
//units?
void BTMultipole::HardEdgedFieldValue(Hep3Vector pos, double *field) const
{
	if(pos.z() > 0.)               return;
	if(fabs(pos.x()) > _width/2.)  return;
	if(fabs(pos.y()) > _height/2.) return;
	for(int m=0; m<=_pole; m++)
	{
		double alpha = sin(m*pi/2.)/(gsl_sf_fact(m)*gsl_sf_fact(_pole-m));
		if(_pole-m-1 >= 0.)            field[0] += alpha*( (_pole-m) *gsl_sf_pow_int(pos.x(), _pole-m-1) * gsl_sf_pow_int(pos.y(), m) );
		if(m-1 >= 0. && _pole-m >= 0.) field[1] += alpha*(       (m) *gsl_sf_pow_int(pos.x(), _pole-m)   * gsl_sf_pow_int(pos.y(), m-1) );
		//hard edge => bz=0
	}
	for(int i=0; i<3; i++) field[i] *= _magnitude;
}

//Units?
void BTMultipole::EngeFieldValue(Hep3Vector pos, double *field) const
{
	pos.setZ(pos.z()*4.);
	if(pos.z() > _length+_effectiveWidth*4.) return;
	if(fabs(pos.x()) > _width/2.)    return;
	if(fabs(pos.y()) > _height/2.)   return;
	double nFacSquare  = gsl_sf_fact(_pole);
	double curvFac     = 1.;
	if(_curvature==constant || (_curvature==straightEnds && pos.z()<0.) ) 
		curvFac = RadiusOfCurvature(pos.z())/(pos.x()+RadiusOfCurvature(pos.z()));
	nFacSquare        *= nFacSquare;
	for(int q=0; q<=_endFieldPole; q++)
	{
		double enge      = Enge(pos.z() - _endLength, 2*q);
		double engePrime = Enge(pos.z() - _endLength, 2*q+1);
		for(int m=0; m<=_pole; m++)
		{
			double alpha = gsl_sf_pow_int(-0.25, q)*sin(m*pi/2.)/(gsl_sf_fact(q)*gsl_sf_fact(_pole+q)*gsl_sf_fact(m)*gsl_sf_fact(_pole-m));
			if(_pole-m-1>=0)
				field[0]    += alpha*enge*( (2*q+_pole-m) *gsl_sf_pow_int(pos.x(), _pole-m-1+2*q) * gsl_sf_pow_int(pos.y(), m) + 
				                                (_pole-m) *gsl_sf_pow_int(pos.x(), _pole-m-1)     * gsl_sf_pow_int(pos.y(), m+2*q) );
			if(_pole-m>=0 && m-1>=0)
				field[1]    += alpha*enge*(           (m) *gsl_sf_pow_int(pos.x(), _pole-m+2*q)   * gsl_sf_pow_int(pos.y(), m-1) +
				                                  (2*q+m) *gsl_sf_pow_int(pos.x(), _pole-m)       * gsl_sf_pow_int(pos.y(), m-1+2*q) );
			if(_pole-m>=0)
				field[2]    += alpha*engePrime*curvFac*(   gsl_sf_pow_int(pos.x(), _pole-m+2*q)   * gsl_sf_pow_int(pos.y(),m) +
				                                           gsl_sf_pow_int(pos.x(), _pole-m)       * gsl_sf_pow_int(pos.y(),m+2*q));
		}
	}
	for(int i=0; i<3; i++) field[i] *= _magnitude;
}

void BTMultipole::Print(std::ostream &out) const
{
	std::string           endFieldModel = "HardEdged";
	if(_endField == enge) endFieldModel = "Enge";

	out << "Multipole Order: " << 2*_pole << " Length: " << _length << " Magnitude: " << _magnitude << " BendingRadius: " << _rCurv
	    << " Height: " << _height << " Width: " << _width << " EndField: "+endFieldModel;
	//if(_pole == 2) out << " EffectiveLength: " << QuadEffectiveLength(1.);
	BTField::Print(out);
}

double BTMultipole::EngeN(double x, int n) const
{
	const double dx = 1e-5; //BUG
	if(n<5)  throw( Squeal(Squeal::recoverable, "Derivative out of range in Enge derivative calculation", "BTMultipole::EngeN(double, int)") );
	if(n==5) return ( Enge(x+dx,4) - Enge(x-dx,4) )/2/dx;
	return (EngeN(x+dx, n-1) - Enge(x-dx, n-1) )/2/dx; //Comes out as Sum[ (nCr) f(x+{n-r}dx) ]/(2dx) I think; errors O(f^(n+2)(x))
}

//h     = a_0+a_1 (x/w)+a_2 (x/w)^2+a_3 (x/w)^3+...+a_m (x/w)^m
//h^(n) = d^nh/dx^n = sum^m_{i=n} a_i x^{i-n}/w^i i!/n!
double BTMultipole::hN(double x, int n) const
{
	double hN   = 0;
	double nFac = gsl_sf_fact(n);
	//optimise by precalculating factor gsl_sf_fact(i)/(gsl_sf_pow_int(_effectiveWidth, i)
	for(unsigned int i=n; i<_engeParameters.size(); i++)
		hN += _engeParameters[i]*gsl_sf_pow_int(x,i-n)*(double)gsl_sf_fact(i)/(gsl_sf_pow_int(_effectiveWidth, i)*nFac); 
	return hN;
}

//g     = 1+exp(h)
//g^(n) = d^ng/dx^n 
double BTMultipole::gN(double x, int n) const
{
	std::vector<double> hn;
	for(int i=0; i<=n; i++) hn.push_back(hN(x,i));
	double exp_h0 = exp(hn[0]);
	switch(n)
	{
		case 0:
			return 1+exp_h0;
		case 1:
			return exp_h0*(hn[1]);
		case 2:
			return exp_h0*(gsl_sf_pow_int(hn[1],2)+hn[2]);
		case 3:
			return exp_h0*(gsl_sf_pow_int(hn[1],3)+3.*hn[1]*hn[2]+hn[3]);
		case 4:
			return exp_h0*(gsl_sf_pow_int(hn[1],4)+6.*hn[1]*hn[1]*hn[2]+4.*hn[1]*hn[3]+3.*hn[2]*hn[2]+hn[4]);
		default:
			throw( Squeal(Squeal::recoverable, "Out of range in Enge derivative calculation", "BTMultipole::gN(double, int)") );
	}
}

double BTMultipole::Enge(double x, int n) const
{
	double              enge = 0;
	std::vector<double> hn; //d^nh/dx^n
	if(n <= 4)
		for(int i=0; i<=n; i++) hn.push_back(hN(x,i));
	switch(n)
	{
		case 0:
			enge = 1./(1+exp(hn[0])); break;
		case 1:
			enge = -1.*gsl_sf_pow_int( exp(hn[0]/2.)+exp(-hn[0]/2.), -2)*hn[1];	
			break;
		case 2:
			enge =  2.*gsl_sf_pow_int( exp(hn[0]/3.)+exp(-2.*hn[0]/3.),-3)*(hn[1]*hn[1])
			       -1.*gsl_sf_pow_int( exp(hn[0]/2.)+exp(-1.*hn[0]/2.),-2)*(hn[1]*hn[1]+hn[2]); 
			break;
		case 3:
		    enge = -1.*gsl_sf_pow_int( exp(hn[0]/2.)+exp(-1.*hn[0]/2.),-2)*(hn[1]*hn[1]*hn[1]+3*hn[1]*hn[2]+hn[3])+
			        6.*gsl_sf_pow_int( exp(hn[0]/3.)+exp(-2.*hn[0]/3.),-3)*(hn[1]*hn[1]+hn[2])*hn[1]
			       -6.*gsl_sf_pow_int( exp(hn[0]/4.)+exp(-3.*hn[0]/4.),-4)* hn[1]*hn[1]*hn[1];
			break;
		case 4:
		    enge = -24.*gsl_sf_pow_int( exp(hn[0]/5.)+exp(-4.*hn[0]/5.),-5)*(hn[1]*hn[1]*hn[1]*hn[1])+
			       -36.*gsl_sf_pow_int( exp(hn[0]/4.)+exp(-3.*hn[0]/4.),-4)*(hn[1]*hn[1])*(hn[1]*hn[1]*hn[2])+
			            gsl_sf_pow_int( exp(hn[0]/3.)+exp(-2.*hn[0]/3.),-3)*( 6.*(hn[1]*hn[1]+hn[2])*(hn[1]*hn[1]+hn[2]) +
			                                                                  8.*hn[1]*(hn[1]*hn[1]*hn[1]+3*hn[1]*hn[2]+hn[3]) )+
			         1.*gsl_sf_pow_int( exp(hn[0]/2.)+exp(-1.*hn[0]/2.),-2)*(gsl_sf_pow_int(hn[1],4)+6*hn[1]*hn[1]*hn[2]+4*hn[1]*hn[3]+hn[4]);
			break;
		default:
			enge = 0;//EngeN(x, n); break;
	}
	return enge;
}

void BTMultipole::SetupReferenceTrajectory()
{
	_angle = _refX = _refS = _refZ = std::vector<double>();
	staticMultipole = this;
	const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
	gsl_odeiv_step    * stepper   = gsl_odeiv_step_alloc(T,4);
	gsl_odeiv_control * control   = gsl_odeiv_control_y_new(_absoluteError, _relativeError);	
	gsl_odeiv_evolve  * evolve    = gsl_odeiv_evolve_alloc(4);
	gsl_odeiv_system    system    = {EvolveReferenceTrajectory, NULL, 4, NULL};
	double h    = 0.1;
	double s    = -_effectiveWidth*4.;

	double y[4] = {0,s,0,_momentum};
	for(double sMax=s; sMax<_length+_effectiveWidth*4.; sMax+=_stepSize)
	{
		while(s<sMax)
		{
			int status =  gsl_odeiv_evolve_apply(evolve, control, stepper, &system, &s, sMax, &h, y);
			if(status != GSL_SUCCESS)
			{
				Print(Squeak::mout(Squeak::debug));
				throw(Squeal(Squeal::nonRecoverable, "Error calculating reference trajectory", "BTMultipole::SetupReferenceTrajectory()"));
			}
			_refS.push_back(s);
			_refX.push_back(y[0]);
			_refZ.push_back(y[1]);
			_angle.push_back( atan(y[2]/y[3]) );
		}
	}
	gsl_odeiv_evolve_free (evolve);
	gsl_odeiv_control_free(control);
	gsl_odeiv_step_free   (stepper);
	staticMultipole = NULL;
}

double BTMultipole::QuadEffectiveLength(double dx) const
{
	//return integral (By(dx, z)) dz / (By(dx, z=centre)/dx)
	//which is the effective length if field gradient is constant along whole quad
	double intB            = IntegralB(dx); //integral B(x=dx) dl [T mm]
	double bfield[6]       = {0,0,0,0,0,0};
	GetFieldValueRotated(Hep3Vector(dx,0,-_length/2.), bfield ); //-_length/2. = middle!
	double nominalGradient  = bfield[1]/dx; //kT/mm
	double integralGradient = intB/dx; //mm * kT/mm
	double effectiveLength  = integralGradient/nominalGradient;
	return effectiveLength;
}

void BTMultipole::SetupDipoleMagnitude(double momentum, double angleInRadians)
{
	double intB  = IntegralB(0.); //integral B dl [T mm]
	double theta = CLHEP::eplus*c_light/momentum*intB;
	_magnitude   = angleInRadians/theta;
}

double BTMultipole::IntegralB(double dxOffset) const
{
	staticMultipole = this;
	staticDX        = dxOffset;
	const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
	gsl_odeiv_step    * stepper   = gsl_odeiv_step_alloc(T,1);
	gsl_odeiv_control * control   = gsl_odeiv_control_y_new(_absoluteError, _relativeError);	
	gsl_odeiv_evolve  * evolve    = gsl_odeiv_evolve_alloc(1);
	gsl_odeiv_system    system    = {EvolveIntegralB, NULL, 1, NULL};
	double h    = 10.;
	double s    = -_effectiveWidth*4.;
	double y[1] = {0};
  int    step = 0;
	while(s<_length+_effectiveWidth*4.)
	{
    if(step>10000) {Squeak::mout(Squeak::warning) << "Stepper stuck while calculating BTMultipole Effective Length" << std::endl; return 0.;}
		int status =  gsl_odeiv_evolve_apply(evolve, control, stepper, &system, &s, _length+_effectiveWidth*4., &h, y);
		if(status != GSL_SUCCESS)
		{
			Print(Squeak::mout(Squeak::debug));
			throw(Squeal(Squeal::nonRecoverable, "Error integrating reference By", "BTMultipole::SetupMagnitude()"));
		}
    step++;
		h = 10.;
	}
	gsl_odeiv_evolve_free (evolve);
	gsl_odeiv_control_free(control);
	gsl_odeiv_step_free   (stepper);
	staticMultipole = NULL;
	return y[0];
}


int BTMultipole::EvolveReferenceTrajectory(double s, const double y[], double f[], void *params)
{
	double bfield[3] = {0,0,0};
	double refS = s;
	if(refS < staticMultipole->_length/2.) refS *= -1.;
	else                                   refS -= staticMultipole->_length;
	staticMultipole->GetFieldValueRotated(Hep3Vector(0,0,refS), bfield );
	f[0] = y[2]/staticMultipole->_momentum; //dx/ds = px/ps
	f[1] = y[3]/staticMultipole->_momentum; //dz/ds = pz/ps
	f[2] = -CLHEP::eplus*c_light*f[1]*bfield[1]; //dpx/ds = -q*c*dz/ds*by
	f[3] =  CLHEP::eplus*c_light*f[0]*bfield[1]; //dpz/ds =  q*c*dx/ds*by
	return GSL_SUCCESS;
}

int BTMultipole::EvolveIntegralB(double s, const double y[], double f[], void *params)
{
	double bfield[3] = {0,0,0};
	double refS = s;
	if(refS < staticMultipole->_length/2.) refS *= -1.;
	else                                   refS -= staticMultipole->_length;
	staticMultipole->GetFieldValueRotated(Hep3Vector(staticDX,0,refS), bfield );
	f[0] = bfield[1];
	return GSL_SUCCESS;
}



void   BTMultipole::GetReferenceTrajectory(std::vector<double>& x, std::vector<double>& z, std::vector<double>& angle, std::vector<double>& s)
{
	x     = _refX;
	z     = _refZ;
	angle = _angle;
	s     = _refS;
}

void   BTMultipole::SetReferenceTrajectory(std::vector<double> x, std::vector<double> z, std::vector<double> angle, std::vector<double> s)
{
	_refX  = x;
	_refZ  = z;
	_refS  = s;
	_angle = angle;
}

CLHEP::Hep3Vector BTMultipole::GetExternalPoint() const
{
	if(_endField==hardEdged) return CLHEP::Hep3Vector(0,0,0);
	for(unsigned int i=0; i<_angle.size(); i++)	
		if(fabs(_angle[i])>1e-9)
			return CLHEP::Hep3Vector(0,0,_refS[i]);
	return CLHEP::Hep3Vector(0,0,_refS[0]);
}


