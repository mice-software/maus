// FProjector - a projector for the specific case of Sci Fi fibres

#ifndef FProjector_h
#define FProjector_h 1

//#include "Kalman/Base/ProjectorB.h"

/** 
* The FProjector is a class that defines a projection from a measurement onto the
* state vector of the Kalman Fitted trajectory.
* It is used in order to allow the individual DoubletClusters to be added as
* measurements to the Helix fit done in the Kalman package and as such it
* inherits from the ProjectorB type defined in the Kalman package
*/

class FProjector // : public ProjectorB
{
 public:

  //! default constructor
  FProjector() {m_Hdefined=false;}

  //! default destructor
  virtual ~FProjector() {};
 
/* 
  //! returns the H matrix 
  EMatrix HMatrix(const IState& is, const IMeasurement& meas);

  //! project the state vector 
  EVector projectionVector(const IState& is, const IMeasurement& meas);

  //! project the measurement 
  EMatrix measurementProjector( const IState& st, const IMeasurement& meas );
  */

 protected:

/*
  //! Extrapolated point (x,y) to the plane in which the wire is contained
  EVector m_r;

  //! Position of the wire (only x and y)
  EVector m_rw;

  //! Vector normal to the wire (only x and y)
  EVector m_nw;

  //! projection matrix
  EMatrix m_H;
*/

  //! true is H is already calculated
  bool m_Hdefined;
};

#endif
