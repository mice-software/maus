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

#ifndef _SRC_COMMON_CPP_FIELDTOOLS_ANALYTICALSOLENOID_HH_
#define _SRC_COMMON_CPP_FIELDTOOLS_ANALYTICALSOLENOID_HH_

#include "src/legacy/BeamTools/BTField.hh"
#include "src/legacy/BeamTools/BTMultipole.hh" // for end field models

namespace MAUS {

/** This should be moved out of the BTMultipole class at some point */
typedef BTMultipole::EndFieldModel EndFieldModel;

/** DerivativesSolenoid uses on-axis field and derivatives to make field
 *
 *  Generates fields using Maxwell's equations to expand B(r,z) as a function of
 *  B(0,z) and it's derivatives (see for example S. Y. Lee p.38 for the
 *  formulae)
 *
 *  For end field model, uses BTMultipole::EndField class (I guess that this
 *  should at some point move out of BTMultipole)
 */

class DerivativesSolenoid : public BTField {
public:
  /** Construct a new DerivativesSolenoid.
   *
   *  - peak_field is the nominal peak field at the magnet centre. Note that
   *  magnets with very long end fields may never reach the "peak field"
   *  - r_max is the maximum radius of the field; r > r_max, B = 0
   *  - z_max is the maximum half length of the field; -z_max > z > z_max, B = 0
   *  - end_pole is the maximum derivative that will be used to calculate fields
   *  - end_field is the EndFieldModel (i.e. mathematical function) that will be
   *  used to make end fields. Note DerivativesSolenoid takes ownership of the
   *  end_field memory
   */
  DerivativesSolenoid(double peak_field, double r_max, double z_max,
                      int highest_order,
                      EndFieldModel* end_field);

  /** Default constuctor initialises everything to 0/NULL
   *
   *  Note don't try to do anything (e.g. GetFieldValue) with this - I don't
   *  usually check for NULL in _end_field (I am concerned with speed).
   */
  DerivativesSolenoid();

  /** Copy constructor makes a deep copy of _end_field
   */
  DerivativesSolenoid(const DerivativesSolenoid& rhs);

  /** Equality operator makes deep copy
   */
  DerivativesSolenoid& operator=(const DerivativesSolenoid& solenoid);

  /** Destructor deletes _end_field
   */
  ~DerivativesSolenoid();

  /** Make a deep copy of the DerivativesSolenoid
   */
  DerivativesSolenoid* Clone() const;

  /** Get the field
   *
   *  Get the field value in local coordinate Point = (x, y, z, time). The field
   *  is put into the field vector Bfield = (bx, by, bz, ex, ey, ez), assumed to
   *  be initialised as a six-vector. Caller owns memory allocatd to Bfield.
   */
  void GetFieldValue ( const double Point[4], double *Bfield ) const;

  /** Print vital statistics of the class
   */
  void Print(std::ostream &out) const;

  /** Get the end field model - DerivativesSolenoid owns memory still */
  const EndFieldModel* GetEndFieldModel() const;

  /** Get the peak field */
  double GetPeakField() const;

  /** Get the maximum radius of the field map */
  double GetRMax() const;

  /** Get the maximum half length of the field map */
  double GetZMax() const;

  /** Get the highest order derivative used in calculating fields */
  int GetHighestOrder() const;

private:
  inline double BzDifferential( const double& z, const int& order) const;

  double _peak_field;
  double _r_max;
  double _z_max;
  int _highest_order;
  EndFieldModel* _end_field;
}; // class DerivativesSolenoid

double DerivativesSolenoid::
                       BzDifferential(const double& z, const int& order) const {
    return _end_field->Function(z, order);
}
}

#endif  //_SRC_COMMON_CPP_FIELDTOOLS_ANALYTICALSOLENOID_HH_

