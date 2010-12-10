#include "BTFastSolenoid.hh"
#include "Interface/Squeak.hh"

#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_sf_pow_int.h"

#include "math.h"

#include <algorithm>

BTFastSolenoid::BTFastSolenoid() : _model(lookup), _highestOrder(0), _zMax(0), _rMax(0), _peakField(0), _length(0), _efoldDistance(0), 
                                   _lambda(0), _oldModel(lookup), _fields(), _zStep(0)
{}

BTFastSolenoid BTFastSolenoid::BTFastTanh(double peakField, double efoldDistance, double length, double z_max, double r_max, int highestOrder)
{
  BTFastSolenoid sol;

  sol._peakField      = peakField;
  sol._efoldDistance  = efoldDistance;
  sol._length         = length;
  sol._highestOrder   = highestOrder;
  sol._model          = tanh;
  sol._zMax           = z_max;
  sol._rMax           = r_max;

  sol.bbMin[0] = sol.bbMin[1] = -sol._rMax;
  sol.bbMax[0] = sol.bbMax[1] =  sol._rMax;
  sol.bbMin[2] = -sol._zMax;
  sol.bbMax[2] =  sol._zMax;

  Tanh::AddTanhDiffIndex(highestOrder+1);

  return sol;
}

BTFastSolenoid BTFastSolenoid::BTFastGlaser(double peakField, double lambda, double z_max, double r_max, int highestOrder)
{
  BTFastSolenoid sol;

  sol._peakField      = peakField;
  sol._lambda         = lambda;
  sol._highestOrder   = highestOrder;
  sol._model          = tanh;
  sol._zMax           = z_max;
  sol._rMax           = r_max;

  sol.bbMin[0] = sol.bbMin[1] = -sol._rMax;
  sol.bbMax[0] = sol.bbMax[1] =  sol._rMax;
  sol.bbMin[2] = -sol._zMax;
  sol.bbMax[2] =  sol._zMax;

  Glaser::AddGlaserDiffIndex(highestOrder+1);

  return sol;
}

BTFastSolenoid::~BTFastSolenoid()
{
  for(unsigned int i=0; i<_fields.size(); i++) delete [] _fields[i];
  _fields.clear();
}

//For the power law expansion, see Lee, Accelerator Physics, p 38
//Optimisation - calculate r squared and use that...
void BTFastSolenoid::GetFieldValue( const  double Point[4], double *Bfield ) const
{
  if( fabs(Point[2]) > _zMax) return;
  double r   = sqrt(Point[0]*Point[0]+Point[1]*Point[1]);
  double Br  = 0;

  int    n = 0;
  while(n < _highestOrder)
  {
    double n_fact  = double(gsl_sf_fact(n));

    double deltaBr = -pow(-1,n)*pow(r/2., 2*n+1)*BzDifferential(Point[2], 2*n+1, _model)/n_fact/double(gsl_sf_fact(n+1));
    double deltaBz =  pow(-1,n)*pow(r/2., 2*n)  *BzDifferential(Point[2], 2*n,   _model)/n_fact/n_fact;
    Bfield[2] += deltaBz;
    Br        += deltaBr;
    n++;
  }

  if(r > 0.)
  {
    Bfield[0] = Br*Point[0]/r;
    Bfield[1] = Br*Point[1]/r;
  }
}

double BTFastSolenoid::BzDifferential(const double& z, const int& order, endField model) const
{
  switch (model)
  {
    case tanh:
      return _peakField/2.*(-Tanh::Tanh(z, order+1, _efoldDistance, _length/2.)+Tanh::Tanh(z, order+1, _efoldDistance, -_length/2.));
    case glaser:
      return _peakField *(Glaser::Glaser(z, order, _lambda));
    case lookup:
    {
      int    zIndex = int(fabs(z)/_zStep);
      double delta  = fabs(z) - _zStep*zIndex;
      return (_fields[order][zIndex+1] - _fields[order][zIndex])*delta/_zStep + _fields[order][zIndex];
    }
  }
  return 0;
}

void BTFastSolenoid::GetVectorPotential(const double point[4], double * potential) const 
{
  if( fabs(point[2]) > _zMax) return;
  double r2       = point[0]*point[0]+point[1]*point[1];
  double deltaPot = 0.;
  for(int n=0; n<_highestOrder; n++)
    deltaPot += pow(-r2/4., n)*BzDifferential(point[2], 2*n, _model)/double(gsl_sf_fact(n))/double(gsl_sf_fact(n))/2./double(n+1.);
  potential[0] =  point[1]*deltaPot;
  potential[1] = -point[0]*deltaPot;
}

void BTFastSolenoid::GetVectorPotentialDifferential(const double point[4], double * diff, int axis) const 
{
  if( fabs(point[2]) > _zMax) return;
  double r2 = point[0]*point[0]+point[1]*point[1];
  double deltapot1 = 0.;
  double deltapot2 = 0.;
  switch (axis)
  {
    case 0:
      for(int n=0; n<_highestOrder; n++)
      {
        double b2n   = BzDifferential(point[2], 2*n, _model);
        double nfact = gsl_sf_fact(n);
        if(n>0)
          deltapot1 += b2n/8./(n+1.)/double(nfact*nfact)*pow(-r2/4., n-1)*n;
        deltapot2   += b2n/2./(n+1.)/double(nfact*nfact)*pow(-r2/4., n); //ok
      }
      diff[0] = -2.*point[0]*point[1]*deltapot1;             //dAx/dx
      diff[1] = +2.*point[0]*point[0]*deltapot1 - deltapot2; //dAy/dx
      break;
    case 1:
      for(int n=0; n<_highestOrder; n++)
      {
        double b2n   = BzDifferential(point[2], 2*n, _model);
        double nfact = gsl_sf_fact(n);
        if(n>0)
          deltapot1 += b2n/8./(n+1.)/double(nfact*nfact)*pow(-r2/4., n-1)*n;
        deltapot2   += b2n/2./(n+1.)/double(nfact*nfact)*pow(-r2/4., n); //ok
      }
      diff[0] = -2.*point[1]*point[1]*deltapot1 + deltapot2; //dAx/dy
      diff[1] = +2.*point[0]*point[1]*deltapot1;             //dAy/dy
      break;
    case 2:
      for(int n=0; n<_highestOrder; n++)
        deltapot1 += pow(-r2/4., n)*BzDifferential(point[2], 2*n+1, _model)/double(pow(gsl_sf_fact(n), 2))/2./double(n+1.);
      diff[0] =  deltapot1 * point[1]; //dAx/dz
      diff[1] = -deltapot1 * point[0]; //dAy/dz
      break;
    default: break;
  }
}

void BTFastSolenoid::Print(std::ostream &out) const
{
  if(_model == tanh || _oldModel == glaser)
    out << "FastSolenoid Tanh_Mode Length: " << _length << " EFoldLength: " << _efoldDistance << " PeakField: " << _peakField << " Order: " << _highestOrder;
  else if(_model == glaser || _oldModel == glaser)
    out << "FastSolenoid Glaser_Mode Lambda: " << _lambda << " PeakField: " << _peakField << " Order: " << _highestOrder;
  if(_model == lookup)
    out << " Lookup_Step_Length: " << _zStep;
  this->BTField::Print(out);
}

void BTFastSolenoid::DoLookup(double stepSize)
{
  int      nSteps  = int(_zMax/_zStep) + 1;
  for(int i=0; i<=_highestOrder; i++)
  {
    _fields.push_back(new double[nSteps]);
    for(int j=0; j<nSteps; j++)
      _fields[i][j] = BzDifferential(j*_zStep, i, _model);
  }
}

namespace Tanh
{

std::vector< std::vector<int> > tdi; //indexes nth differential of tanh

double Tanh(const double& x, const int& n, const double& lambda, const double& x0)
{
  double tanh_x = tanh( (x-x0)/lambda );
  double out    = 0;
  std::vector<double>      tanh_x_pow(n+1,1.);
  for(int i=1; i<n+1; i++) tanh_x_pow[i] = tanh_x_pow[i-1]*tanh_x;
  for(int i=0; i<n+1; i++)
    out += tdi[n][i]*tanh_x_pow[i];
  return out/pow(lambda, n-1);
}

void AddTanhDiffIndex(int n)
{
  if(tdi.size()<2)
  {
    tdi.push_back(std::vector<int>(1,0));
    tdi.push_back(std::vector<int>(2,0));
    tdi.back()[1] = 1.;
  }
  while(size_t(n) >= tdi.size() )
  {
    tdi.push_back(std::vector<int>(tdi.back().size()+1, 0));
    std::vector<int>& tdi_last = tdi[tdi.size()-2];
    for(size_t i=1; i<tdi_last.size(); i++)
    {
      tdi.back()[i-1] += i*tdi_last[i];
      tdi.back()[i+1] -= i*tdi_last[i];
    }
  }

}

std::vector< std::vector<int> > TanhDiffIndices(int n)
{ AddTanhDiffIndex(n)
      return tdi;}

}

namespace Glaser
{

std::vector< std::vector<int*> > gdi;

double Glaser(const double& x, const int& n, const double& lambda)
{
  double _x  = x/lambda;
  double _x2 = _x*_x;
  double _y  = 0;
  for(unsigned int i=0; i<m; i++)
  {
    int* a = gdi[n][i];
    _y += a[2]*pow(_x,a[1])/pow(1+_x2, a[0]);
  }
  return _y;
}

void AddGlaserDiffIndex(unsigned int n)
{
  //Apply recursion relation: if y^(n)   = Sum_i ( a[2]*x^a[1]*(1+x^2)^(-a[0]) )
  //Then:                        y^(n+1) = Sum_i ( a[2]*a[1]*x^(a[1]-1)*(1+x^2)^a[0] - 2*a[2]*(a[0]+1)*x^(a[1]+1)*(1+x^2)^-(a[0]+1) )
  //So we get two new terms:
  //First:  b[2] = a[2]*a[1];        b[1] = a[1]-1; b[0] = a[0]
  //Second: c[2] = -2*a[2]*(a[0]+1); c[1] = a[1]+1; c[0] = a[0]+1 
  if(gdi.size() == 0)
  {
    gdi = std::vector< std::vector<int*> >( 1, std::vector<int*>( 1, new int[3] ) ) ;
    gdi[0][0][0] = 1; gdi[0][0][1] = 0; gdi[0][0][2] = 1; //0th order is 1*x^0*(1+x^2)^-1 = 1/(1+x^2)
  }

  for(unsigned int i = gdi.size(); i < n; i++)
  {
    gdi.push_back(std::vector<int*>(gdi[i-1].size()*2, NULL) );
    std::vector<int*> a = gdi[i-1];
    std::vector<int*> b = gdi[i];
    for(unsigned int j=0; j<a.size(); j++)
    {
      b[2*j]    = new int[3];
      b[2*j][2] = a[j][2]*a[j][1];
      b[2*j][1] = a[j][1]-1;
      b[2*j][0] = a[j][0];
      if(b[2*j][1] < 0) b[2*j][2] = 0;

      b[2*j+1]    = new int[3];
      b[2*j+1][2] = a[j][2]*(gdi[i-1][j][0]+1)*-2;
      b[2*j+1][1] = a[j][1]+1;
      b[2*j+1][0] = a[j][0]+1;
    }
    //Now rationalise 
    //  look for terms with same power indices and add them together
    //  look for terms with 0 size and remove them
    std::sort(b.begin(), b.end(), less_than<3, int>);
    for(unsigned int j=1; j<b.size(); j++)
    {
      while(b[j-1][0] == b[j][0] && b[j-1][1] == b[j][1] && j < b.size())
      {
        b[j-1][2] += b[j][2];
        delete [] b[j];
        b.erase(b.begin()+j);
      }
    }
    for(unsigned int j=0; j<b.size(); j++)
      while(b[j][2] == 0  && j < b.size()) {delete [] b[j]; b.erase(b.begin()+j);}
  }
}


//return true if a[0] < b[0]; if a[0] == b[0] return true if a[1] < b[1], etc; if all a == all b return false
//should go in algorithms namespace
template<unsigned int len, class T> 
bool less_than( T a[], T b[] )
{
    unsigned int i=0;
    while(i<len)
    {
      /**/ if( a[i] < b[i] ) return true;
      else if( a[i] > b[i] ) return false;
      i++;
    }
    return false;
}


}




