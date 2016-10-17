/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <algorithm>
#include <vector>

#include "math.h"

#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_sf_pow_int.h"

#include "Utils/Squeak.hh"
#include "src/common_cpp/FieldTools/DerivativesSolenoid.hh"

namespace MAUS {

DerivativesSolenoid::DerivativesSolenoid(double peak_field, double r_max,
                    double z_max, int highest_order,
                    EndFieldModel* end_field)
    : BTField(-r_max, -r_max, -z_max, +r_max, +r_max, +z_max),
      _peak_field(peak_field), _r_max(r_max), _z_max(z_max),
      _highest_order(highest_order), _end_field(end_field) {
}

DerivativesSolenoid::~DerivativesSolenoid() {
  if (_end_field != NULL) {
    delete _end_field;
  }
}

DerivativesSolenoid* DerivativesSolenoid::Clone() const {
  return new DerivativesSolenoid(*this);
}

DerivativesSolenoid::DerivativesSolenoid(const DerivativesSolenoid& rhs)
  : _end_field(NULL) {  // make _end_field NULL to avoid deletion on assignment
  *this = rhs;
}

DerivativesSolenoid& DerivativesSolenoid::operator=
                                              (const DerivativesSolenoid& rhs) {
  if (this == &rhs) {
    return *this;
  }
  _peak_field = rhs._peak_field;
  _r_max = rhs._r_max;
  _z_max = rhs._z_max;
  _highest_order = rhs._highest_order;
  if (_end_field != NULL) {
    delete _end_field;
  }
  if (rhs._end_field != NULL) {
    _end_field = rhs._end_field->Clone();
  } else {
    _end_field = NULL;
  }
  double bb[] = {-_r_max, -_r_max, -_z_max, _r_max, _r_max, _z_max};
  BTField::bbMin = std::vector<double>(&bb[0], &bb[3]);
  BTField::bbMax = std::vector<double>(&bb[3], &bb[6]);

  return *this;
}

DerivativesSolenoid::DerivativesSolenoid()
    : _peak_field(0), _r_max(0), _z_max(0), _highest_order(0),
      _end_field(NULL) {
}

void DerivativesSolenoid::
                 GetFieldValue(const  double Point[4], double *Bfield) const {
  if (fabs(Point[2]) > _z_max) return;
  double r = sqrt(Point[0]*Point[0]+Point[1]*Point[1]);
  if (r > _r_max) return;
  double Br = 0;

  int    n = 0;
  while (n < _highest_order) {
    double n_fact = static_cast<double>(gsl_sf_fact(n));

    double deltaBr = -pow(-1, n)*pow(r/2., 2*n+1)*
                      BzDifferential(Point[2], 2*n+1)
                      /n_fact/static_cast<double>(gsl_sf_fact(n+1));
    double deltaBz = pow(-1, n)*pow(r/2., 2*n)*
                     BzDifferential(Point[2], 2*n)/n_fact/n_fact;
    Bfield[2] += deltaBz;
    Br += deltaBr;
    n++;
  }

  Br *= _peak_field;
  Bfield[2] *= _peak_field;

  if (r > 0.) {
    Bfield[0] = Br*Point[0]/r;
    Bfield[1] = Br*Point[1]/r;
  }
}

void DerivativesSolenoid::Print(std::ostream &out) const {
  out << "DerivativesSolenoid PeakField: " << _peak_field
      << " Order: " << _highest_order << " ";
  _end_field->Print(out);
  this->BTField::Print(out);
}

const EndFieldModel* DerivativesSolenoid::GetEndFieldModel() const {
  return _end_field;
}

double DerivativesSolenoid::GetPeakField() const {
  return _peak_field;
}

double DerivativesSolenoid::GetRMax() const {
  return _r_max;
}

double DerivativesSolenoid::GetZMax() const {
  return _z_max;
}

int DerivativesSolenoid::GetHighestOrder() const {
  return _highest_order;
}

/*
THIS ALL LOOKS OKAY - BUT NOT TESTED

void DerivativesSolenoid::GetVectorPotential(const double point[4],
                                        double * potential) const
{
  if (fabs(point[2]) > _z_max) return;
  double r2       = point[0]*point[0]+point[1]*point[1];
  if (r2 > _r_max*_r_max) return;
  double deltaPot = 0.;
  for (int n = 0; n < _highest_order; n++) {
    deltaPot += pow(-r2/4., n)*BzDifferential(point[2], 2*n)
    /double(gsl_sf_fact(n))/double(gsl_sf_fact(n))/2./double(n+1.);
  }
  potential[0] =  point[1]*deltaPot;
  potential[1] = -point[0]*deltaPot;
}

void DerivativesSolenoid::GetVectorPotentialDifferential
                          (const double point[4], double * diff, int axis) const
{
  if (fabs(point[2]) > _z_max) return;
  double r2 = point[0]*point[0]+point[1]*point[1];
  double deltapot1 = 0.;
  double deltapot2 = 0.;
  switch (axis)
  {
    case 0:
      for (int n = 0; n < _highest_order; n++) {
        double b2n   = BzDifferential(point[2], 2*n);
        double nfact = gsl_sf_fact(n);
        if (n > 0) {
          deltapot1 += b2n/8./(n+1.)/double(nfact*nfact)*pow(-r2/4., n-1)*n;
        }
        deltapot2   += b2n/2./(n+1.)/double(nfact*nfact)*pow(-r2/4., n); //ok
      }
      diff[0] = -2.*point[0]*point[1]*deltapot1;             //dAx/dx
      diff[1] = +2.*point[0]*point[0]*deltapot1 - deltapot2; //dAy/dx
      break;
    case 1:
      for (int n = 0; n < _highest_order; n++) {
        double b2n = BzDifferential(point[2], 2*n);
        double nfact = gsl_sf_fact(n);
        if (n > 0) {
          deltapot1 += b2n/8./(n+1.)/double(nfact*nfact)*pow(-r2/4., n-1)*n;
        }
        deltapot2   += b2n/2./(n+1.)/double(nfact*nfact)*pow(-r2/4., n); //ok
      }
      diff[0] = -2.*point[1]*point[1]*deltapot1 + deltapot2; //dAx/dy
      diff[1] = +2.*point[0]*point[1]*deltapot1;             //dAy/dy
      break;
    case 2:
      for(int n=0; n<_highest_order; n++) {
        deltapot1 += pow(-r2/4., n)*BzDifferential(point[2], 2*n+1)
        /double(pow(gsl_sf_fact(n), 2))/2./double(n+1.);
      }
      diff[0] =  deltapot1 * point[1]; //dAx/dz
      diff[1] = -deltapot1 * point[0]; //dAy/dz
      break;
    default:
      break;
  }
}
*/
}


