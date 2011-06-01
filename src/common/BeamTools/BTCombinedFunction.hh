// MAUS WARNING: THIS IS LEGACY CODE.
//BTFieldGroup contains and places groups of fields
//Generic but intended to hold field data for e.g. a module in MICE

#ifndef BTCOMBINEDFUNCTION_HH
#define BTCOMBINEDFUNCTION_HH

#include <vector>
#include <iostream>
#include "BTField.hh"
#include "BTMultipole.hh"

/// 

class BTCombinedFunction : public BTField {
public:
  /// Generate a BTCombinedFunction from dipole field and index
  BTCombinedFunction(int maxPole, double By, double fieldIndex,
              double length, double height, double width,
              std::string curvature, double radiusVariable,
              const BTMultipole::EndFieldModel* endfield, int endPole);
  /// Destructor (nothing to destruct)
  ~BTCombinedFunction() {;}

  /// Print a summary of the combined function magnet
  void Print(std::ostream &out) const;

  /// Return the sum of fields in the CF magnet
  void GetFieldValue( const double Point[4], double *EMfield ) const {
    for(unsigned int i=0; i<_fields.size(); i++)
    { 
      double field[6] = {0,0,0,0,0,0};
      _fields[i]->GetFieldValue(Point, field);
      for(int i=0; i<6; i++) EMfield[i] += field[i];
    }
  }
  /// Copy constructor
  BTCombinedFunction * Clone() const {return new BTCombinedFunction(*this);}

  /// Calculate radius of curvature if bending model is MomentumBased
  void SetMomentumBased();

  /// Return the vector potential - NOT IMPLEMENTED
  CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector Point) const;

  /// ??
  CLHEP::Hep3Vector       GetExternalPoint() const;

  /// Return the reference trajectory if bending model is "momentum based"
  void   GetReferenceTrajectory(std::vector<double>& x, std::vector<double>& z, std::vector<double>& angle, std::vector<double>& s); 

private:
  std::vector<BTMultipole*>   _fields;
  double                     _by;
  double                     _fieldIndex;
  double                     _length;
  double                     _radiusOfCurvature;
  double                     _height;
  double                     _width;

  double GetFieldAtPoleTip(int pole);// {if(pole == 1) return _by; else return 0;}
};

#endif
