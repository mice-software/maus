// FProjector.cpp

#include "Recon/SciFi/FProjector.hh"

#ifdef USE_FPROJECTOR

#include "Recon/SciFi/SciFiDoubletMeas.hh"

//****************************************************************************
EMatrix FProjector::HMatrix( const IState& st, const IMeasurement& meas )
{
  //! converts the generic IMeasurement into a SciFiDoubletMeas to access the information 
  //! of this class not present in the interface. Of course the class SciFiDoubletMeas
  //! must inherit from IMeasurement 

  //ME const SciFiDoubletMeas& fibre = static_cast<const SciFiDoubletMeas&>( meas );

  // retrieves the x,y position of the extrapolation
  EVector v = st.dynamicVector();

  m_r = EVector( 2, 0 );
  m_r[0] = v[0];
  m_r[1] = v[1];
  
  //! retrieve the normal vector to the wire segment from the singlet
  //! for a given extrapolated y
  //! The singlet should be able to return the appropriate segment for a given y

  m_nw = EVector( 2, 0 );

/* ME
  int tracker = fibre.cluster()->getTrackerNo();
  int station = fibre.cluster()->getStationNo();
  int plane = fibre.cluster()->getDoubletNo();
  int chan = (int) fibre.cluster()->getChanNoU();
*/

  Hep3Vector dir; //ME = decoder->direction( tracker, plane );
  Hep3Vector pos; //ME = decoder->position( tracker, station, plane, chan );

  m_nw[0] = dir.y();
  m_nw[1] = - dir.x();

  m_rw = EVector( 2, 0 );

  m_rw[0] = pos.x() / 10.0;
  m_rw[1] = pos.y() / 10.0;

  //computes the H matrix
  m_H = EMatrix( 1, 5, 0 );

  m_H[0][0] = m_nw[0]; 
  m_H[0][1] = m_nw[1];  
  m_H[0][2] = 0;
  m_H[0][3] = 0;
  m_H[0][4] = 0;

  m_Hdefined = true;

  return m_H;
}

//****************************************************************************
EVector FProjector::projectionVector( const IState& st, const IMeasurement& meas )
{
  // computes HMatrix and retrieves wire parameters if it is necessary

  //  if ( ! m_Hdefined )
    EMatrix dummy = HMatrix( st, meas );

  // computes the distance
  double d = dot( m_nw, m_r - m_rw );


  const SciFiDoubletMeas& fibre = static_cast<const SciFiDoubletMeas&>( meas );
  int tracker = fibre.cluster()->getTrackerNo();
  int station = fibre.cluster()->getStationNo();
  int plane = fibre.cluster()->getDoubletNo();
  int chan = (int) fibre.cluster()->getChanNoU();
  std::cout << "FPROJ " << tracker << " " << station << " " << plane << " " << chan << " " << m_nw[0] << " " << m_nw[1] << " " << m_r[0] << " " << m_r[1] << " " << m_rw[0] << " " << m_rw[1] << " " << d << std::endl;

  // converts into a vector because we must return a vector
  EVector h( 1, 0 );

  h[0] = d;

  return h;
}

//*********************************************************************
EMatrix FProjector::measurementProjector( const IState& st, const IMeasurement& meas )
{
  if( verbosity() > 9 ) { st.info(2); meas.info(2); }

  EMatrix P( 1, 3, 0 );

  P[0][0] = 1;

  return P;
}



#endif
