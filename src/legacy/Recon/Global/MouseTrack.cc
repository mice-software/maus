// MouseTrack.cc - highest level track (from any potential tracker) in MICE

#include "MouseTrack.hh"

MouseTrack::~MouseTrack()
{
  // delete TofPids

  for( unsigned int i = 0; i < m_tof_pids.size(); ++i )
    delete m_tof_pids[i];
}

Extrapolation		MouseTrack::extrapolateToZ( double exz ) const
{
/*
  double z = exz / 10;

  Hep3Vector pos( 3, 0 );
  Hep3Vector dir1( 3, 0 );
  Hep3Vector dir2( 3, 0 );
  Hep3Vector size( 2, 0 );

  dir1[0] = 0;    dir1[1] = 0;    dir1[2] = 1;
  dir2[0] = 1;    dir2[1] = 0;    dir2[2] = 0;
  size[0] = 5000;   size[1] = 5000;

  pos[2] = z;

  SurfaceB plane( "Rectangle" , pos, size, dir1, dir2 );

  plane.setMainParameter( "z pos", "position", 2 );

  std::string name = m_manager->volumeAtMainParameter(z);
  plane.setVolumeName( name );

std::cout << "extrap to " << name << " at z " << exz << std::endl;

  StateB* finalState = new StateB();

  bool good = false;

  if( name != "default" )
    good = m_manager->propagate( *m_it, plane, *finalState );

  if( good )
    return Extrapolation(); //ME finalState, m_manager );
  else
  {
    delete finalState;
    return Extrapolation();
  }
*/
  return Extrapolation();
}
 
void			MouseTrack::addTofPid( TofPid* tof )
{
  m_pids.push_back( tof );
  m_tof_pids.push_back( tof );
}

const std::vector<MousePid*>&	MouseTrack::mousePids() const
{
  return m_pids;
}

const std::vector<TofPid*>&	MouseTrack::tofPids() const
{
  return m_tof_pids;
}
