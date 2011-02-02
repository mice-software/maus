// StagePosition.hh - holds information about the position of the scanning stage used in the tracker QA
//
// M.Ellis - 3rd October 2007

#ifndef STAGEPOSITION_HH
#define STAGEPOSITION_HH

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/ThreeVector.h"

using CLHEP::Hep3Vector;
using CLHEP::micrometer;

class StagePosition
{
  public :

    StagePosition( double ax0, double ax0t, double ax1, double ax1t )
    {
      _ax0 = ax0;
      _ax0t = ax0t;
      _ax1 = ax1;
      _ax1t = ax1t;

      _pos = Hep3Vector( _ax0 * micrometer, ax1 * micrometer, 0. );
      _post = Hep3Vector( ax0t * micrometer, ax1t * micrometer, 0. );
    }

    Hep3Vector 		pos() const			{ return _pos; };

    Hep3Vector 		actualPos() const		{ return _post; };
    
    void		dump() const			{ std::cout << _ax0 << " " << _ax0t << " " << _ax1 << " " << _ax1t << " " << pos() << " " << actualPos() << std::endl; };

  private :

    double _ax0;
    double _ax0t;
    double _ax1;
    double _ax1t;

    Hep3Vector _pos;
    Hep3Vector _post;
};

#endif

