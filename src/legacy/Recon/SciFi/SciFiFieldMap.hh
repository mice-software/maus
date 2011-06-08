// SciFiFieldMap.hh - field map description for the Sci FI tracker used by the Kalman package

#ifndef SCIFIFIELDMAP_h
#define SCIFIFIELDMAP_h 1

#include "BeamTools/BTFieldConstructor.hh"

#include "recpack/EVectorMap.h"
#include "recpack/EGeo.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

using namespace Recpack;

#include <string>

class SciFiFieldMap : public EVectorMap
{
  public :

  // constructor takes BTFieldConstructor object
  SciFiFieldMap( BTFieldConstructor* );

  // destructor
  virtual ~SciFiFieldMap() {};

  //! return the field vector as a function of position

  EVector compute_vector( const EVector& pos ) const;

  private :

  BTFieldConstructor*		_bt;
};

#endif
