// MAUS WARNING: THIS IS LEGACY CODE.
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

#include "CLHEP/Units/PhysicalConstants.h"

#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_sf_pow_int.h"
#include "gsl/gsl_odeiv.h"
#include "gsl/gsl_errno.h"

#include "BeamTools/BTMultipole.hh"

#include "Utils/Exception.hh"
#include "Utils/Squeak.hh"

const BTMultipole * BTMultipole::staticMultipole = NULL;
double              BTMultipole::staticDX        = 0.;
double              BTMultipole::_relativeError  = 1.e-5;
double              BTMultipole::_absoluteError  = 1.e-5;
double              BTMultipole::_stepSize       = 1.;


BTMultipole::BTMultipole()
        :   _pole(0), _magnitude(0.), _length(0.), _rCurv(0.),
            _height(0.), _width(0.), _curvature(constant),
            _endFieldModel(NULL) {
}

void BTMultipole::Init(int pole, double fieldAtWidth,
                        double length, double height, double width,
                        std::string curvature, double radiusVariable,
                        const EndFieldModel* endfield, int endPole) {
  _pole = pole;
  _magnitude = fieldAtWidth;
  _length = length;
  _rCurv  = radiusVariable;
  _height = height;
  _width  = width;
  _curvature = constant;
  _endFieldPole = endPole;
  _endFieldModel = NULL;
  if (endfield != NULL) _endFieldModel = endfield->Clone();
  if (curvature == "MomentumBased") {
    _curvature = momentumBased;
    _momentum  = radiusVariable;
    _rCurv     = 0;
    _magnitude = 1.;
    SetupDipoleMagnitude(radiusVariable, fieldAtWidth);
    SetupReferenceTrajectory();
  } else {
    if (fabs(radiusVariable) < 1e-5 || curvature == "Straight") {
      _curvature = straight;
    } else {
      if (curvature == "StraightEnds") _curvature = straightEnds;
      if (curvature == "Constant" || curvature == "") _curvature = constant;
      if (_width >= fabs(_rCurv))
        throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
              "Multipole with width > radius of curvature",
              "BTMultipole::BTMultipole"));
      if (_length > fabs(2*pi*_rCurv))
        throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
                     "Multipole with bending angle > 360 degrees",
                     "BTMultipole::BTMultipole"));
      _endRotation = CLHEP::HepRotation(CLHEP::Hep3Vector(0, 1, 0),
                                        _length/_rCurv);
      _endPoint    = _endRotation*CLHEP::Hep3Vector(-_rCurv, 0, 0)+
                                  CLHEP::Hep3Vector(_rCurv, 0, 0);
    }
  }
  // BUG - this should be as strict as EndFieldValue
  // note that the EndFieldValue uses _width/2, _height/2, _length; so
  // EndFieldValue is factor 2 more strict than BB in each dimension;
  bbMin[0] = -_width;
  bbMax[0] = +_width;
  bbMin[1] = -_height;
  bbMax[1] = +_height;
  bbMin[2] = 0.;
  bbMax[2] = _length*2.;
  if (_curvature != straight) {
      // length of the segment with curvature == _rCurv; always longer than the
      // equivalent box
      double phi = _length*2/_rCurv;
      if (phi > pi/2)
          phi = pi/2;
      bbMax[2] = (_rCurv+_width)*sin(phi);
      bbMin[0] = -(_rCurv - (_rCurv-_width)*cos(phi));
  }
}

BTMultipole BTMultipole::DipoleFromMomentum
  (double momentum, double length, double angle, double height, double width) {
  // field = theta p / (qL)
  // rcurv = l / theta
  double field  = (2*pi*angle/360.)*momentum/length/CLHEP::eplus/c_light;
  double rCurv  = length*360./(2*pi*angle);       // L = 2 pi r*(theta/360)
  BTMultipole m;
  m.Init(1, field, length, height, width, "constant", rCurv, NULL, 0);
  return m;
}

CLHEP::Hep3Vector BTMultipole::TransformToRotated(const double * Point) const {
  switch (_curvature) {
    case straight:
      return CLHEP::Hep3Vector(Point[0], Point[1], Point[2]);
    case constant:
      return TransformToRotatedConstant(Point);
    case straightEnds:
      return TransformToRotatedStraightEnds(Point);
    case momentumBased:
      return TransformToRotatedMomentumBased(Point);
    default:
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
                   "Did not recognise curvature model",
                   "BTMultipole::TransformToRotated(const double*)"));
  }
}

CLHEP::Hep3Vector BTMultipole::TransformToRotatedMomentumBased
                                                 (const double point[3]) const {
  int    p0             = 0;
  int    p1             = _refX.size()-1;
  double rSquaredStart  = (point[0]-_refX[p0])*(point[0]-_refX[p0]) +
                          (point[2]-_refZ[p0])*(point[2]-_refZ[p0]);
  double rSquaredEnd    = (point[0]-_refX[p1])*(point[0]-_refX[p1]) +
                          (point[2]-_refZ[p1])*(point[2]-_refZ[p1]);
  // This is horrid!!! Minimise rSquared (using binary interpolation) to find
  // s(x,z) Assumes sign(B) == const st dRSquared is always increasing or
  // decreasing
  while (unsigned(p1-p0)>1) {
    int    pM             = (p1+p0)/2;
    double rSquaredMiddle = (point[0]-_refX[pM])*(point[0]-_refX[pM]) +
                            (point[2]-_refZ[pM])*(point[2]-_refZ[pM]);
    if (rSquaredStart > rSquaredEnd) {
      rSquaredStart = rSquaredMiddle;
      p0            = pM;
    } else {
      rSquaredEnd  = rSquaredMiddle;
      p1           = pM;
    }
  }
  double deltaR = sqrt(rSquaredStart);
  if (point[0]-_refX[p0] > 0.) deltaR *= -1;
  double deltaS = _refS[p0];
  return CLHEP::Hep3Vector(deltaR, point[1], deltaS);
}

CLHEP::Hep3Vector BTMultipole::TransformToRotatedConstant
                                                  (const double * Point) const {
  double dx     = Point[0];
  double x      = dx+_rCurv; // x in coordinates relative to centre of rotation
  double z      = Point[2];
  double q      = atan2(z, x);
  double deltaS = q*_rCurv;
  double deltaR = sqrt(x*x+z*z)-_rCurv;
  return CLHEP::Hep3Vector(deltaR, Point[1], deltaS);
}

CLHEP::Hep3Vector BTMultipole::TransformToRotatedStraightEnds
                                                  (const double * Point) const {
  if (Point[2] < 0.5*(_length-_centreLength))
    return CLHEP::Hep3Vector(Point[0], Point[1], Point[2]);
  double pos[3] = {Point[0], Point[1], Point[2]-0.5*(_length-_centreLength)};
  CLHEP::Hep3Vector trans = TransformToRotatedConstant(pos);
  if (trans[2] > _centreLength)
    trans = _endRotation*(trans-_endPoint)+_endPoint;
  trans[2] += 0.5*(_length-_centreLength);
  return trans;
}

double BTMultipole::RadiusOfCurvature(double sRelativeToEnd) const {
  switch (_curvature) {
    case straight:
      return 0;
    case constant:
      return _rCurv;
    case straightEnds:
      if (sRelativeToEnd > 0.) {
        return 0;
      } else {
        return _rCurv;
      }
    case momentumBased: {
      double EMfield[3] = {0, 0, 0};
      GetFieldValueRotated(CLHEP::Hep3Vector(0, 0, sRelativeToEnd), EMfield);
      return _momentum/EMfield[1]/CLHEP::eplus/CLHEP::c_light;
    }
    default:
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
                   "Did not recognise curvature model",
                   "BTMultipole::RadiusOfCurvature(double)"));
  }
}

void BTMultipole::TransformToRectangular
                                  (const double point[3], double* value) const {
  switch (_curvature) {
    case straight:
      return;
    case constant:
      return TransformToRectangularConstant(point, value);
    case straightEnds:
      return TransformToRectangularStraightEnds(point, value);
    case momentumBased:
      return TransformToRectangularMomentumBased(point, value);
    default:
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
                "Did not recognise curvature model",
                "BTMultipole::TransformToRectangular(const double*, double*)"));
  }
}

void       BTMultipole::TransformToRectangularConstant
                                  (const double point[3], double* value) const {
  double theta  = atan2(point[2], point[0]+_rCurv);
  double bx     = value[2]*sin(theta) + value[0]*cos(theta);
  double bz     = value[2]*cos(theta) + value[0]*sin(theta);
  value[0] = bx;
  value[2] = bz;
}

void       BTMultipole::TransformToRectangularStraightEnds
                                  (const double point[3], double* value) const {
  if (point[2] < 0) return;
  double theta = 0;
  if (point[2] > _endPoint[2])
    theta = _endRotation.delta();
  else
    theta  = atan2(point[2], point[0]+_rCurv);
  double bx     = value[2]*sin(theta) + value[0]*cos(theta);
  double bz     = value[2]*cos(theta) + value[0]*sin(theta);
  value[0] = bx;
  value[2] = bz;
}

void  BTMultipole::TransformToRectangularMomentumBased
                                 (const double point[3], double* value) const {
  int step      = static_cast<int>(point[2]/_stepSize)+1;
  double theta  = 0;
  if (step > static_cast<int>(_angle.size())) theta = _angle.back();
  else if (step > 0)             theta = _angle[step];
  double bx     = value[2]*sin(theta) + value[0]*cos(theta);
  double bz     = value[2]*cos(theta) + value[0]*sin(theta);
  value[0] = bx;
  value[2] = bz;
}

void BTMultipole::GetFieldValueRotated
                              (CLHEP::Hep3Vector pos, double *EMfield) const {
  if (_endFieldModel == NULL) {
    HardEdgedFieldValue(pos, EMfield);
    return;
  }
  EndFieldValue(pos, EMfield);
}

void BTMultipole::GetFieldValue
                              (const double Point[4], double *EMfield) const {
  CLHEP::Hep3Vector pos = TransformToRotated(Point);
  GetFieldValueRotated(pos, EMfield);
  TransformToRectangular(Point, EMfield);
  return;
}

// pos is the position from the radius of curvature arc relative to the centre
// of the multipole - units?
void BTMultipole::HardEdgedFieldValue
                                  (CLHEP::Hep3Vector pos, double *field) const {
  if (pos.z() < 0. || pos.z() > _length) return;
  if (fabs(pos.x()) > _width/2.)  return;
  if (fabs(pos.y()) > _height/2.) return;
  for (int m = 1; m <= _pole; m+=2) {
    double alpha = GetConst(0, _pole, m);
    if (_pole-m-1 >= 0.)
      field[0] += alpha*((_pole-m)*gsl_sf_pow_int(pos.x(), _pole-m-1)*
                                   gsl_sf_pow_int(pos.y(), m));
    if (m-1 >= 0. && _pole-m >= 0.)
      field[1] += alpha*((m)*gsl_sf_pow_int(pos.x(), _pole-m)*
                             gsl_sf_pow_int(pos.y(), m-1));
    // hard edge => bz=0
  }
  for (int i = 0; i < 3; ++i) field[i] *= _magnitude;
}

double BTMultipole::GetConst(int q, int n, int m) {
  return gsl_sf_pow_int(gsl_sf_fact(n), 2)*gsl_sf_pow_int(-1, (m-1)/2)/(
         gsl_sf_pow_int(-4, q)*gsl_sf_fact(q)*gsl_sf_fact(n+q)*gsl_sf_fact(m)
        *gsl_sf_fact(n-m))/static_cast<double>(n);
}

void BTMultipole::EndFieldValue(CLHEP::Hep3Vector pos, double *field) const {
  if (pos.z() < 0. || pos.z() > _length) return;
  if (fabs(pos.y()) > _height/2.)   return;
  pos[2] -= _length/2.;
  int    n(_pole);
  const double x = pos[0];
  const double y = pos[1];
  const double z = pos[2];
  for (int q = 0; q <= _endFieldPole; ++q) {
    double f      = _endFieldModel->Function(z, 2*q);
    double fPrime = _endFieldModel->Function(z, 2*q+1);
    for (int m = 1; m <= n; m += 2) {
      double const_qnm = GetConst(q, n, m);
      if ( q > 0 )
        field[0] += const_qnm*f*(2.*q*x*gsl_sf_pow_int((x*x+y*y), q-1)
                               *gsl_sf_pow_int(x, n-m)*gsl_sf_pow_int(y, m));
      if (n-m > 0)
        field[0] += const_qnm*f*(n-m)*gsl_sf_pow_int((x*x+y*y), q)
                                 *gsl_sf_pow_int(x, n-m-1)*gsl_sf_pow_int(y, m);
      if (m > 0)
        field[1] += const_qnm*f*m*gsl_sf_pow_int(x*x+y*y, q)
                    *gsl_sf_pow_int(x, n-m)*gsl_sf_pow_int(y, m-1);
      if (q > 0)
        field[1] += const_qnm*f*2*q*y*gsl_sf_pow_int(x*x+y*y, q-1)
                             *gsl_sf_pow_int(x, n-m)*gsl_sf_pow_int(y, m);
      field[2] += const_qnm*fPrime*gsl_sf_pow_int(x*x+y*y, q)
                           *gsl_sf_pow_int(x, n-m)*gsl_sf_pow_int(y, m);
    }
  }
  for (int i = 0; i < 3; ++i) field[i] *= _magnitude;
}

void BTMultipole::Print(std::ostream &out) const {
  out << "Multipole Order: " << 2*_pole << " Length: " << _length
      << " Magnitude: " << _magnitude << " BendingRadius: " << _rCurv
      << " Height: " << _height << " Width: " << _width << " EndField: ";
  if (_endFieldModel == NULL)
    out << "HardEdged";
  else
    _endFieldModel->Print(out);
  BTField::Print(out);
}

void BTMultipole::SetupReferenceTrajectory() {
  _angle = _refX = _refS = _refZ = std::vector<double>();
  staticMultipole = this;
  const gsl_odeiv_step_type* T = gsl_odeiv_step_rk4;
  gsl_odeiv_step* stepper = gsl_odeiv_step_alloc(T, 4);
  gsl_odeiv_control* control = gsl_odeiv_control_y_new(_absoluteError,
                                                         _relativeError);
  gsl_odeiv_evolve* evolve = gsl_odeiv_evolve_alloc(4);
  gsl_odeiv_system system = {EvolveReferenceTrajectory, NULL, 4, NULL};
  double h = 0.1;
  double s = 0.;
  double y[4] = {0, 0, 0, _momentum};
  for (double sMax = s; sMax <= _length; sMax += _stepSize) {
    while (s < sMax) {
      int status =  gsl_odeiv_evolve_apply(evolve, control, stepper, &system,
                                           &s, sMax, &h, y);
      if (status != GSL_SUCCESS) {
        Print(MAUS::Squeak::mout(MAUS::Squeak::debug));
        throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
                    "Error calculating reference trajectory",
                    "BTMultipole::SetupReferenceTrajectory()"));
      }
      _refS.push_back(s);
      _refX.push_back(y[0]);
      _refZ.push_back(y[1]);
      _angle.push_back(atan(y[2]/y[3]));
    }
  }
  gsl_odeiv_evolve_free(evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free(stepper);
  staticMultipole = NULL;
}

double BTMultipole::IntegralB(double dxOffset) const {
  staticMultipole = this;
  staticDX        = dxOffset;
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  gsl_odeiv_step    * stepper   = gsl_odeiv_step_alloc(T, 1);
  gsl_odeiv_control * control   = gsl_odeiv_control_y_new(_absoluteError,
                                                          _relativeError);
  gsl_odeiv_evolve  * evolve    = gsl_odeiv_evolve_alloc(1);
  gsl_odeiv_system    system    = {EvolveIntegralB, NULL, 1, NULL};
  double h    = 10.;
  double s    = 0.;
  double y[1] = {0};
  int    step = 0;
  while (s < _length) {
    if (step > 10000) {
      MAUS::Squeak::mout(MAUS::Squeak::warning) << "Stepper stuck while calculating "
                                    << "BTMultipole Effective Length"
                                    << std::endl;
      return 0.;
    }
    int status =  gsl_odeiv_evolve_apply(evolve, control, stepper, &system, &s,
                                         _length, &h, y);
    if (status != GSL_SUCCESS) {
      Print(MAUS::Squeak::mout(MAUS::Squeak::debug));
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
                   "Error integrating reference By",
                   "BTMultipole::SetupMagnitude()"));
    }
    step++;
    h = 10.;
  }
  gsl_odeiv_evolve_free(evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free(stepper);
  staticMultipole = NULL;
  return y[0];
}

void BTMultipole::SetupDipoleMagnitude(double momentum, double angleInRadians) {
  double intB  = IntegralB(0.);  // integral B dl [T mm]
  double theta = CLHEP::eplus*c_light/momentum*intB;
  _magnitude   = angleInRadians/theta;
}

int BTMultipole::EvolveReferenceTrajectory
                        (double s, const double y[], double f[], void *params) {
  double bfield[3] = {0, 0, 0};
  staticMultipole->GetFieldValueRotated(CLHEP::Hep3Vector(0, 0, s), bfield);
  f[0] = y[2]/staticMultipole->_momentum;  // dx/ds = px/ps
  f[1] = y[3]/staticMultipole->_momentum;  // dz/ds = pz/ps
  f[2] = -CLHEP::eplus*c_light*f[1]*bfield[1];  // dpx/ds = -q*c*dz/ds*by
  f[3] =  CLHEP::eplus*c_light*f[0]*bfield[1];  // dpz/ds =  q*c*dx/ds*by
  return GSL_SUCCESS;
}

int BTMultipole::EvolveIntegralB
                        (double s, const double y[], double f[], void *params) {
  double bfield[3] = {0, 0, 0};
  staticMultipole->GetFieldValueRotated(Hep3Vector(staticDX, 0, s), bfield);
  f[0] = bfield[1];
  return GSL_SUCCESS;
}

void   BTMultipole::GetReferenceTrajectory(std::vector<double>& x,
                    std::vector<double>& z, std::vector<double>& angle,
                    std::vector<double>& s) {
  x     = _refX;
  z     = _refZ;
  angle = _angle;
  s     = _refS;
}

void   BTMultipole::SetReferenceTrajectory(std::vector<double> x,
                    std::vector<double> z, std::vector<double> angle,
                    std::vector<double> s) {
  _refX  = x;
  _refZ  = z;
  _refS  = s;
  _angle = angle;
}

