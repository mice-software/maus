/* 
  SciFiDoubletMeas.cc - a measurement in the Kalman fit based on a Doublet cluster
*/

#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "Recon/SciFi/SciFiDoubletMeas.hh"
#include "Recon/SciFi/SciFiSpacePoint.hh"

#ifdef REINSTATEATHISCLASS

using std::vector;

SciFiDoubletMeas::SciFiDoubletMeas( SciFiDoubletCluster* clust ) : m_clust( clust )
{
/*
	int sn = m_clust->getStationNo();
        int tn = m_clust->getTrackerNo();
        int dn = m_clust->getDoubletNo();
        int cn = (int) m_clust->getChanNoU();
*/

	// set the Kalman stuff

	EVector measV( 3, 0 );
	EMatrix measM( 3, 3, 0 );

	measV[0] = 0.0;							// distance perpendicular to the fibre
	measV[1] = 0.0;							// this is not used
	measV[2] = 0.0; //ME decoder->position( tn, sn, dn, cn ).z() / 10; 	// z position

	measM[0][0] = 1.8654e-3; // * decoder->mux() * decoder->mux();	// 210 micron spacing divided by root 12
									// corrected for multiplexing

	m_hv = HyperVector( measV, measM );
}

SciFiDoubletMeas::~SciFiDoubletMeas()
{
}

// this stuff is needed for MeasurementB:

const HyperVector& 	SciFiDoubletMeas::measHV() const
{
  return m_hv;
}

string 			SciFiDoubletMeas::type() const
{
  return "f";
}

const ISurface& 	SciFiDoubletMeas::surface() const
{
  return *m_surface;
}

void            	SciFiDoubletMeas::setSurface( ISurface* surface )
{
  m_surface = surface;
  m_name = m_surface->volumeName();
        
  EVector measV( 3, 0 ); 
  EMatrix measM( 3, 3, 0 );
        
  measV[0] = 0.0;                                                 // distance perpendicular to the fibre
  measV[1] = 0.0;                                                 // this is not used
  measV[2] = surface->parameter( "position", 2 );
      
  measM[0][0] = 1.8654e-3;                 // 210 micron spacing divided by root 12
        
  m_hv = HyperVector( measV, measM );
}

void 			SciFiDoubletMeas::setSurface( const ISurface& surf )
{
  m_surface = (ISurface*) &surf;
  m_name = m_surface->volumeName();

  EVector measV( 3, 0 );      
  EMatrix measM( 3, 3, 0 );

  measV[0] = 0.0;                                                 // distance perpendicular to the fibre
  measV[1] = 0.0;                                                 // this is not used
  measV[2] = surf.parameter( "position", 2 );

  measM[0][0] = 1.8654e-3;                 // 210 micron spacing divided by root 12

  m_hv = HyperVector( measV, measM );
}

string 			SciFiDoubletMeas::volumeName() const
{
  return m_name;
}

void   			SciFiDoubletMeas::setVolumeName( string name )
{
  m_name = name;
}

void   			SciFiDoubletMeas::info( int a ) const
{
}

void			SciFiDoubletMeas::info() const
{
}

#endif
