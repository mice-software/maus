// MCHit.hh

#ifndef INTERFACE_MCHIT_HH
#define INTERFACE_MCHIT_HH 1

#include <algorithm>

#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;

class MCParticle;

class MCHit
{
  public :

    MCHit();

    MCHit( int, int, double, double, Hep3Vector, Hep3Vector, double, double, double );

    virtual ~MCHit() {};

    int		trackID() const			{ return _trackID; };

    int		pdg() const			{ return _pdg; };

    double	charge() const			{ return _charge; };

    double	mass() const			{ return _mass; };

    Hep3Vector	position() const		{ return _position; };

    Hep3Vector	momentum() const		{ return _momentum; };

    double	time() const			{ return _time; };

    double	Edep() const			{ return _edep; };

    double	energy() const			{ return _energy; };

    MCParticle* particle() const		{ return _particle; };


    void	setTrackID( int id )		{ _trackID = id; };

    void	setPdg( int pdg )		{ _pdg = pdg; };

    void	setCharge( double q )		{ _charge = q; };

    void	setMass( double m )		{ _mass = m; };

    void	setPosition( Hep3Vector pos )	{ _position = pos; };

    void	setMomentum( Hep3Vector mom )	{ _momentum = mom; };

    void	setTime( double t )		{ _time = t; };

    void	setEdep( double de )		{ _edep = de; };

    void	setEnergy( double e )		{ _energy = e; };

    void	setParticle( MCParticle* p )	{ _particle = p; };

  private :

    int		_trackID;
    int		_pdg;
    double	_charge;
    double	_mass;
    Hep3Vector	_position;
    Hep3Vector	_momentum;
    double	_time;
    double	_edep;
    double	_energy;
    MCParticle*	_particle;
};


//Such a trivial class that I don't think it's worth giving it it's own file...
class PrimaryGenHit : public MCHit
{
  public:
    PrimaryGenHit() : MCHit(), _eventNumber(0), _seed(0), _statisticalWeight(0) {}
    int    eventNumber()          {return _eventNumber;}
    int    seed       ()          {return _seed;}
    double weight     ()          {return _statisticalWeight;}
    CLHEP::Hep3Vector spin()      {return _spin;}

    void setEventNumber(int event)     {_eventNumber = event;}
    void setSeed       (int seed)      {_seed        = seed;}
    void setWeight     (double weight) {_statisticalWeight      = weight;}
    void setSpin       (CLHEP::Hep3Vector spin) {_spin    = spin;}

  private:
    int      _eventNumber;
    long int _seed;        //as set in datacards
    double   _statisticalWeight;
    CLHEP::Hep3Vector _spin;
};

#endif

