// Copyright 2007-2011 Chris Rogers
//
// This file is a part of G4MICE
//
// G4MICE is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// G4MICE is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with G4MICE in the doc folder.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef BTMULTIPOLE_HH
#define BTMULTIPOLE_HH

#include <vector>
#include <iostream>
#include <string>

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "Interface/MathUtils.hh"
#include "Interface/STLUtils.hh"

#include "BeamTools/BTField.hh"

/// BTMultipole class generates a multipole field (dipole, quadrupole, ...).

/// BTMultipole class generates a multipole field with various end field
/// functional forms and various models for sector-type multipoles. Field is
/// generated from a scalar magnetic potential of the form\n
/// \f$V = \sum^q \sum^n_m C_{qnm} G^{(2q)}(s) r^{n-m} y^m\f$
/// where \f$(r,y,s)\f$ is the coordinate system in the rotated frame of the
/// magnet.
///
/// \f$G(s)\f$ is the end field model, given by some function that tells how the
/// field drops off with \f$s\f$. Three end field models are provided - hard
/// edged (where field drops immediately to 0), Enge (where field falls off as
/// an exponent of a polynomial) and tanh (where field falls off as a plain
/// exponential).
///
/// For sector multipoles, the curvature can be modelled in one of 3 ways:
///   - Constant radius of curvature even in end field region
///   - Constant radius of curvature but straight in end field region
///   - Radius of curvature calculated by propagation of a test particle through
///     a dipole field
///
/// Note (0,0,0) in local coordinate system corresponds to the open end of the
/// multipole (unlike most other field types). This is so that I can place the
/// multipole
class BTMultipole : public BTField {
// TODO(rogers):
//   - Curvature model needs testing and fixing altogether - consider it suspect
//   - Curvature momentum based - needs to have a parameter like "tolerance" and
//     also needs user defined PID
//   - Clone needs to see rotation parameters
//   - StraightEnds could be brought back to life - needs to see some sort of
//     CentreLength parameter which determines the length of the curved region
//   - (For rotation, what I would do is subclass the various transformations as
//     with end field)

 public:
  /// Abstract data type for multipole EndFieldModel

  /// I put EndFields separate - so that end field model code is independent of
  /// the rest of Multipole code. I have few requirements here:
  /// (i) I want to keep the end field models independent of the field
  ///    implementation so I can share end field models between different field
  ///    types etc (i.e. FastSolenoid wants tanh end field also).
  /// (ii) I don't want to manage mass of bureaucracy with many different
  ///    constructors in BTMultipole for different end field types.
  /// (iii) I don't want to enforce an interface on external function calls that
  ///    I use for end field models.
  /// So I move the end field into subclass and use inheritance model to enforce
  /// appropriate function calls with small wrapper function in each subclass.
  class EndFieldModel {
   public:
    virtual ~EndFieldModel() {;}
    virtual std::ostream& Print(std::ostream& out) const = 0;
    virtual double Function(double x, int n) const = 0;
    virtual EndFieldModel* Clone() const = 0;
  };
  class EngeEndField;
  class TanhEndField;

  /// Default constructor.
  BTMultipole();

  /// Multipole Initialiser.
  ///
  /// Initialiser for a multipole with:
  ///   - pole is the multipole (1=dipole, 2=quadrupole, etc)
  ///   - field scales the field
  ///   - length is the length of the field map
  ///   - height is the full height of the field map
  ///   - width is the full width of the field map
  ///   - curvature is a string that labels the curvature model
  ///   - radius of curvature is a double that controls the bending radius of
  ///     multipole
  ///   - EndFieldModel controls the end field fall off (NULL for top hat).
  ///     I do not own memory associated with endfield, I take a copy instead.
  ///   - pole+endpole is the maximum pole that will be calculated on the end
  ///     field
  void Init(int pole, double field,
       double length, double height, double width,
       std::string curvature, double radiusVariable,
       const EndFieldModel* endfield, int endPole);

  /// Construct a dipole with radius of curvature determined by a test particle

  /// Construct a dipole with the radius of curvature determined by integrating
  /// the B-field along the axis of the dipole; the radius of curvature is then
  /// given by B.dl as a function of z, such that a particle with a specified
  /// momentum is bent by the specified angle.
  static BTMultipole DipoleFromMomentum(double momentum, double length,
                                    double angle, double height, double width);

  /// Destructor - deletes end field model, but this is a copy of endfield model
  ~BTMultipole() {if (_endFieldModel != NULL) delete _endFieldModel;}

  /// GetFieldValue at a Point relative to multipole start
  void GetFieldValue(const double Point[4], double *EMfield) const;

  /// GetFieldValue at a Point in the rotated coordinate system

  /// Calculates the field value assuming magnet is straight - perform any
  /// coordinate transformations before calling this function.
  void GetFieldValueRotated(CLHEP::Hep3Vector Point, double *EMfield) const;

  /// Get the field value for a straight, hard edged magnet type

  /// In the case where there is no end field, we can make the field value
  /// calculation a bit quicker.
  void HardEdgedFieldValue(CLHEP::Hep3Vector Point, double *EMfield)  const;

  /// Get the field value for a straight magnet with some end field

  /// The field for an arbitrary multipole is given by\n
  /// \f$ Bx = \f$
  void EndFieldValue(CLHEP::Hep3Vector Point, double *EMfield)  const;
  /// Calculate the constant term for contributions of each multipole term

  /// The multipole is defined as sum( a_qnm P(x,y)) where q is the contribution
  /// from higher order terms in the multipole, n and m determine the pole of
  /// the lowest order term.
  static inline double    GetConst(int q, int n, int m);

  /// Transform to magnet coordinate system

  /// In magnet coordinate system z=0 corresponds to the start of the magnet,
  /// and (r,y,s) bent magnet coordinates -> (x,y,z) cartesian coordinates
  CLHEP::Hep3Vector TransformToRotated
                                      (const double point[3]) const;
  /// Transform to magnet coordinate system for constant radius of curvature
  CLHEP::Hep3Vector TransformToRotatedConstant
                                                  (const double point[3]) const;
  /// Transform to magnet coordinate system for constant radius of curvature
  /// but with no radius of curvature in the end field of the magnet
  CLHEP::Hep3Vector TransformToRotatedStraightEnds
                                                  (const double point[3]) const;
  /// Transform to magnet coordinate system for radius of curvature determined
  /// from momentum and \f$B_y\f$ in the magnet.
  CLHEP::Hep3Vector TransformToRotatedMomentumBased
                                                  (const double point[3]) const;

  /// In-place transformation from magnet coordinate system

  /// Transform from the coordinate system of the magnet back to the
  /// coordinate system relative to magnet centre.
  void TransformToRectangular(double * point) const;
  /// Transform point from magnet coordinate system

  /// Transform from the coordinate system of the magnet back to the
  /// coordinate system relative to magnet centre. Put new coordinates into
  /// value.
  void TransformToRectangular(const double point[3], double* value) const;
  /// Transform point from magnet coordinate system for magnet with constant
  /// radius of curvature.
  void TransformToRectangularConstant
                                   (const double point[3], double* value) const;
  /// Transform point from magnet coordinate system for magnet with constant
  /// radius of curvature but no curvature in the ends.
  void TransformToRectangularStraightEnds
                                   (const double point[3], double* value) const;
  /// Transform point from magnet coordinate system for magnet with radius of
  /// curvature determined from dipole field and some nominal momentum.
  void TransformToRectangularMomentumBased
                                   (const double point[3], double* value) const;
  /// Print the field definition to out
  void Print(std::ostream &out) const;  // nb fails cpplint.py
  /// Make a copy of *this; does not copy rotation parameters
  inline BTMultipole * Clone() const;
  /// Return integral (By(dx, z)) dz / (By(z=centre)/dx). This is the length of
  /// the equivalent magnet with no end fields.
  double QuadEffectiveLength(double dx) const;
  /// Return the field magnitude
  inline double GetMagnitude() const {return _magnitude;}
  /// Return the width of the field
  inline double GetWidth() const {return _width;}
  /// Return the height of the field
  inline double GetHeight() const {return _height;}
  /// Return the length of the field
  inline double GetLength() const {return _length;}
  /// Return a pointer to the end field model
  inline const  EndFieldModel* GetEndField() const {return _endFieldModel;}
  /// Return the pole of the magnet (1=dipole, 2=quad, ...)
  inline int    GetPole() const {return _pole;}
  /// Return the extra poles calculated in the end field - i.e. if pole = n and
  /// EndPole = m, then the maximum pole calculated in the end field will be n+m
  inline int    GetEndPole() const {return _endFieldPole;}
  /// Return the trajectory of a notional reference particle
  void   GetReferenceTrajectory(std::vector<double>& x, std::vector<double>& z,
                            std::vector<double>& angle, std::vector<double>& s);
  /// Set the trajectory of a notional reference particle
  void   SetReferenceTrajectory(std::vector<double>  x, std::vector<double>  z,
                            std::vector<double>  angle, std::vector<double>  s);
  /// List of possible curvature models
  enum   curvatureModel {straight, constant, straightEnds, momentumBased};
  /// Return the actual curvature model
  curvatureModel GetCurvatureModel() {return _curvature;}

 private:
  /// staticMultipole used during integration for numerical determination of
  /// integral(B)dl, integral(dB/dx)dl
  static const BTMultipole * staticMultipole;
  /// Equations of motion for calculating reference trajectory in momentumbased
  static int EvolveReferenceTrajectory
                         (double s, const double y[], double f[], void *params);
  /// Equation of motion for calculating integral By dl
  static int EvolveIntegralB
                         (double s, const double y[], double f[], void *params);
  /// Calculate integral By dl at some x position
  double     IntegralB(double dxOffset) const;
  /// Calculate scale factor so that dipole bends a particle by a defined angle
  void   SetupDipoleMagnitude(double momentum, double angleInRadians);
  /// Setup a reference trajectory
  void   SetupReferenceTrajectory();
  bool   isInside(const double Point[4]) const;
  double RadiusOfCurvature(double sRelativeToEnd) const;

  int     _pole;  // lowest order term is a multipole of what order?
  int     _endFieldPole;  // pole number to go up to in end field

  double  _magnitude;  // constant of proportionality
  double  _length;
  double  _rCurv;
  double  _height;
  double  _width;
  curvatureModel _curvature;

  // Geometry info used for _curvature==straightEnds
  double             _centreLength;
  CLHEP::Hep3Vector  _endPoint;    // point in middle at far end of multipole
  CLHEP::HepRotation _endRotation;  // rotation to angle of far end of multipole

  // Geometry info used for _curvature==momentumBased
  double      _momentum;  // momentum of reference particle
  // angle of tangent to reference trajectory relative to (x,y,z)=(0,0,1)
  std::vector<double> _angle;
  std::vector<double> _refX;  // x position of reference trajectory
  std::vector<double> _refZ;  // z position of reference trajectory
  std::vector<double> _refS;  // s position of reference trajectory

  // parameters used for calculating (numerically) reference trajectory,
  // effective length, etc; //parameters used for calculating (numerically)
  // reference trajectory, effective length, etc
  static double staticDX;
  // parameters used for calculating (numerically) reference trajectory,
  // effective length, etc
  static double _absoluteError;
  static double _relativeError;
  static double _stepSize;

  const EndFieldModel* _endFieldModel;
  /// Disable = and copy ctor
  DISALLOW_COPY_AND_ASSIGN(BTMultipole);
};

// Bug - rotation model not implemented here
BTMultipole * BTMultipole::Clone() const {
  BTMultipole * out = new BTMultipole();
  out->Init(_pole, _magnitude, _length, _height, _width, std::string(""),
            0., _endFieldModel, _endFieldPole);
  return out;
}

/// EngeEndField interfaces MathUtils Enge function and EndFieldModel

/// Small wrapper class to make interface to Enge function
class BTMultipole::EngeEndField : public BTMultipole::EndFieldModel {
 public:
  EngeEndField(const std::vector<double> a, double x0, double lambda,
              int max_pole) : _enge(a, x0, lambda, max_pole*2+2) {}
  ~EngeEndField() {}
  inline double Function(double x, int n) const {
    return _enge.GetDoubleEnge(x, n);
  }
  Enge GetEnge() {return _enge;}
  EngeEndField* Clone() const {
    EngeEndField* eef = new EngeEndField(std::vector<double>(), 0., 0., 0);
    eef->_enge = _enge;
    return eef;
  }
  std::ostream& Print(std::ostream& out) const {
    out << "Enge";
    return out;
  }
 private:
  Enge _enge;
};

/// EngeEndField interfaces MathUtils Tanh function and EndFieldModel

/// Small wrapper class to make interface to Tanh function
class BTMultipole::TanhEndField : public BTMultipole::EndFieldModel {
 public:
  TanhEndField(double x0, double lambda, int max_pole)
    : _tanh(x0, lambda, max_pole*2+2) {}
  TanhEndField(const TanhEndField& rhs) : _tanh(rhs._tanh) {}
  ~TanhEndField() {}
  inline double Function(double x, int n) const {
    return _tanh.GetDoubleTanh(x, n);
  }
  Tanh GetTanh() {return _tanh;}
  TanhEndField* Clone() const {
    return new TanhEndField(*this);
  }
  std::ostream& Print(std::ostream& out) const {
    out << "Tanh";
    return out;
  }
 private:
  Tanh _tanh;
};

#endif
