// SciFiKalTrack.cc
//
// M.Ellis 20/10/2003
//
// Takes a normal SciFiTrack and refits it using the Kalman Filter

#include "SciFiKalTrack.hh"
#include "SciFiSpacePoint.hh"
#include "SciFiDoubletCluster.hh"
#include "Interface/dataCards.hh"

#include "CLHEP/Random/RandGauss.h"

#include "recpack/ParticleState.h"
#include "recpack/Surface.h"

extern Recpack::RecpackManager _man;
extern Recpack::Setup _setup;

extern std::vector<Surface*> _surfaces;

double		SciFiKalTrack::chi2AddPoint( SciFiSpacePoint* point )
{
/*
  Hep3Vector pos = positionAtZ( point->getPos().z() );
  Hep3Vector posE = posErrorAtZ( point->getPos().z() );

  double chi2 = ( point->getPos().x() - pos.x() ) * ( point->getPos().x() - pos.x() ) / posE.x() / posE.x() +
                ( point->getPos().y() - pos.y() ) * ( point->getPos().y() - pos.y() ) / posE.y() / posE.y();

  return chi2;
*/
return 0;
}

SciFiKalTrack::~SciFiKalTrack()
{
  miceMemory.addDelete( Memory::SciFiKalTrack );

  if( m_it )
    delete m_it;

  for( unsigned int i = 0; i < m_measurements.size(); ++i )
    delete m_measurements[i];
}

SciFiKalTrack::SciFiKalTrack()
{
  miceMemory.addNew( Memory::SciFiKalTrack );
}

SciFiKalTrack::SciFiKalTrack( std::vector<SciFiSpacePoint*>& points, KalmanSeed kalSeed )
{
  miceMemory.addNew( Memory::SciFiKalTrack );
	
  m_tracker = points[0]->getTrackerNo();

  m_manager = & _man;

  m_it = new Recpack::Trajectory();

  // now add space points as (x,y,z) measurements..

  m_points.resize( points.size() );

  for( unsigned int i = 0; i < points.size(); ++i )
  {
    EVector x( 2, 0 );
    EVector posV( 3, 0 );
    EMatrix C( 2, 2, 0 );

    SciFiSpacePoint* pt = points[i];

    m_points[i] = pt;

    Hep3Vector pos = pt->getPos();

    x[0] = pos.x();
    x[1] = pos.y();

    posV[0] = pos.x();
    posV[1] = pos.y();
    posV[2] = pos.z();

    Hep3Vector posE = pt->getPosE();

    C[0][0] = posE.x() * posE.x();
    C[1][1] = posE.y() * posE.y();


    Surface* surf = NULL;
    double distZ = 1e9;

    for( unsigned int j = 0; j < _surfaces.size(); ++j )
    {
      EVector spos = _surfaces[j]->position();
      double dist = fabs( spos[2] - pos.z() );
      if( dist < distZ || ! surf )
      {
        surf = _surfaces[j];
        distZ = dist;
      }
    }

    if( distZ < 10 )
    {
      Measurement* meas = new Measurement();

      meas->set_name( "xy" );
      meas->set_hv( HyperVector( x, C ) );
      meas->set_position( posV );
      meas->set_surface( *surf );
      m_measurements.push_back( meas );
    }
  }

  m_it->add_measurements( m_measurements );

  // now calculate a seed state..

  State seed;

  EVector v( 6, 0 );
  EMatrix C( 6, 6, 0 );

  v[0] = kalSeed.x();
  v[1] = kalSeed.y();
  v[2] = kalSeed.z() - 0.1 * mm;
  v[3] = kalSeed.xPrime();
  v[4] = kalSeed.yPrime();
  v[5] = kalSeed.pInv();

  C[0][0] = kalSeed.xError2();
  C[1][1] = kalSeed.yError2();
  C[2][2] = EGeo::zero()/2.;//ZERO/2.;    // no error on Z since planes are perpendicular to z
  C[3][3] = kalSeed.xPrimeError2();
  C[4][4] = kalSeed.yPrimeError2();
  C[5][5] = kalSeed.pInvError2();

  _model = "particle/helix";
  seed.set_name( _model );
  seed.set_hv( HyperVector( v, C ) );
  seed.set_parameter( 0. ); // the parameter (irrelevant for "particle/helix" model)

  seed.set_hv(RP::sense, HyperVector(1,0));
  seed.set_hv(RP::qoverp, HyperVector(v[5],C[5][5]));
  seed.set_name(RP::representation, RP::slopes_z);

  m_it->sort_nodes( 1 );

  bool ok = m_manager->fitting_svc().fit( seed, *m_it );

  if( ok && m_it->ndof() > 0 )
    m_chi2 = m_it->quality();	// Chi2 per DOF
  else
    m_chi2 = -1.;
}

SciFiKalTrack::SciFiKalTrack( std::vector<SciFiSpacePoint*>& points, double P, double Q )
{
  miceMemory.addNew( Memory::SciFiKalTrack );
	
  m_manager = & _man;

  m_tracker = points[0]->getTrackerNo();

  m_it = new Recpack::Trajectory();

  // now add space points as (x,y,z) measurements..

  m_points.resize( points.size() );

  for( unsigned int i = 0; i < points.size(); ++i )
  {
    EVector x( 2, 0 );
    EVector posV( 3, 0 );
    EMatrix C( 2, 2, 0 );

    SciFiSpacePoint* pt = points[i];

    m_points[i] = pt;

    Hep3Vector pos = pt->getPos();

    x[0] = pos.x();
    x[1] = pos.y();

    posV[0] = pos.x();
    posV[1] = pos.y();
    posV[2] = pos.z();

    Hep3Vector posE = pt->getPosE();

    C[0][0] = posE.x() * posE.x();
    C[1][1] = posE.y() * posE.y();


    Surface* surf = NULL;
    double distZ = 1e9;

    for( unsigned int j = 0; j < _surfaces.size(); ++j )
    {
      EVector spos = _surfaces[j]->position();
      double dist = fabs( spos[2] - pos.z() );
      if( dist < distZ || ! surf )
      {
        surf = _surfaces[j];
        distZ = dist;
      }
    }

    if( distZ < 10 )
    {
      Measurement* meas = new Measurement();

      meas->set_name( "xy" );
      meas->set_hv( HyperVector( x, C ) );
      meas->set_position( posV );
      meas->set_surface( *surf );
      m_measurements.push_back( meas );
    }
  }

  m_it->add_measurements( m_measurements );

  // now calculate a seed state..

  State seed;

  EVector v( 5, 0 );
  EMatrix C( 5, 5, 0 );

  SciFiSpacePoint* first = points[0];
  SciFiSpacePoint* last = points[0];

  for( unsigned int i = 1; i < points.size(); ++i )
  {
    if( points[i]->getPos().z() < first->getPos().z() )
      first = points[i];
    if( points[i]->getPos().z() > last->getPos().z() ) 
      last = points[i];
  }

  v[0] = first->getPos().x();
  v[1] = first->getPos().y();
  v[2] = ( last->getPos().x() - first->getPos().x() ) / ( last->getPos().z() - first->getPos().z() );
  v[3] = ( last->getPos().y() - first->getPos().y() ) / ( last->getPos().z() - first->getPos().z() );
  v[4] = 1. / P;

  C[0][0] = 1e2;
  C[1][1] = 1e2;
  C[2][2] = 1e2;
  C[3][3] = 1e2;
  C[4][4] = 1e-4;

  _model = "particle/sline";
  seed.set_name( _model );
  seed.set_hv( HyperVector( v, C ) );
  seed.set_parameter( 0. ); // the parameter (irrelevant for "particle/sline" model)

  seed.set_hv(RP::sense, HyperVector(1,0));
  seed.set_hv(RP::qoverp, HyperVector(v[4],C[4][4]));
  seed.set_name(RP::representation, RP::slopes_z);

  m_it->sort_nodes( 1 );

  bool ok = m_manager->fitting_svc().fit( seed, *m_it );

  if( ok )
    m_chi2 = m_it->quality();	// Chi2 per DOF
  else
    m_chi2 = -1.;
}

const SciFiTrack*	SciFiKalTrack::sciFiTrack() const
{
  return m_track;
}

State*			SciFiKalTrack::extrapToZ( double exz ) const
{
  Surface* surf = NULL;

  // ME find the surface we are extrapolating to

  double distZ = 1e9;

  for( unsigned int i = 0; i < _surfaces.size(); ++i )
  {
    EVector pos = _surfaces[i]->position();
    double dist = fabs( pos[2] - exz );
    if( dist < distZ || ! surf )
    {
      surf = _surfaces[i];
      distZ = dist;
    }
  }

  if( distZ < 10 )
  {
    State* extrap = m_it->state(0).clone();
    double length;

    bool good = _man.navigation_svc().propagate(*surf,*extrap,length);

    if( good )
      return extrap;
    else
    {
      delete extrap;
      return NULL;
    }
  }
  else
    return NULL;
}

Hep3Vector              SciFiKalTrack::positionAtZ( double z ) const
{
  //ME temporary hack until I get some help from Anselmo

  if( _model == "particle/sline" )
  {
    HyperVector hv = m_it->state(0).hv();
    double x = hv.vector()[0] + ( z - hv.vector()[2] ) * hv.vector()[3];
    double y = hv.vector()[1] + ( z - hv.vector()[2] ) * hv.vector()[4];
    return Hep3Vector( x, y, z );
  }

  State* state = extrapToZ( z );

  if( state )
  {
    ParticleState pstate( *state );
   
    EVector pos = pstate.position();
    return Hep3Vector( pos[0], pos[1], pos[2] );
  }
  else
    return Hep3Vector( -99999, -9999, -9999 );
}

Hep3Vector              SciFiKalTrack::posErrorAtZ( double z ) const
{
  State* state = extrapToZ( z );

  if( state )
  {
    ParticleState pstate( *state );
   
    EVector posE = pstate.position_error();
    return Hep3Vector( sqrt( posE[0] ), sqrt( posE[1] ), sqrt( posE[2] ));
  }
  else
    return Hep3Vector( -99999, -9999, -9999 );
}

Hep3Vector              SciFiKalTrack::momentumAtZ( double z ) const
{
  State* state = extrapToZ( z );

  if( state )
  {
    ParticleState pstate( *state );
   
    EVector mom = pstate.momentum();
    return Hep3Vector( mom[0], mom[1], mom[2] );
  }
  else
    return Hep3Vector( -99999, -9999, -9999 );
}

Hep3Vector              SciFiKalTrack::momErrorAtZ( double z ) const
{

  State* state = extrapToZ( z );

  if( state )
  { 
    ParticleState pstate( *state );
   
    EVector momE = pstate.momentum_error();
    return Hep3Vector( momE[0], momE[1], momE[2] );
  }
  else
    return Hep3Vector( -99999, -9999, -9999 );
}

double			SciFiKalTrack::timeAtZ( double zex ) const
{
  double t = -999999.9;
  double bestz = -999999.9;

  for( unsigned int i = 0; i < m_points.size(); ++i )
    for( int j = 0; j < m_points[i]->getNClusters(); ++j )
    {
      double z = m_points[i]->getDoubletCluster(j)->getTruePosition().z();
      if( fabs( z - zex ) < fabs( bestz - zex ) )
      {
        bestz = z;
        t = m_points[i]->getDoubletCluster(j)->getTime();
      }
    }

  return t;
}

void		SciFiKalTrack::addPoint( SciFiSpacePoint* point )
{
/*
  m_points.push_back( point );

  EVector x( 3, 0 );
  EMatrix C( 3, 3, 0 );

  Hep3Vector pos = point->getPos();
  x[0] = pos.x() / 10.0;
  x[1] = pos.y() / 10.0;
  x[2] = pos.z() / 10.0;

  Hep3Vector posE = point->getPosE();

  C[0][0] = posE.x() * posE.x() * 0.01;
  C[1][1] = posE.y() * posE.y() * 0.01;
  C[2][2] = 1e-4;

  char volName[80];
  sprintf( volName, "SciFi%dStation%d", point->getTrackerNo() + 1,
                                        point->getStationNo() + 1 );

  m_measurements.push_back( new MeasurementB( std::string( volName ), 
                                              HyperVector( x, C ), std::string( "xy" ) ) );

  m_it->addMeasurement( *m_measurements[m_measurements.size() - 1] );
*/
}

void		SciFiKalTrack::refit()
{
/*
  StateB seed;
  seed.copy( m_it->firstState() );

  //m_it->strip();
  m_it->sortPoints( "Z", "increasing" );

  m_manager->fit( *m_it, seed );
*/
}

void	SciFiKalTrack::keep()
{
  for( unsigned int i = 0; i < m_points.size(); ++i )
    m_points[i]->setUsed();
}
