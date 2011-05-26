// MCParticle.hh

#ifndef INTERFACE_MCPARTICLE_HH
#define INTERFACE_MCPARTICLE_HH 1

#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;

#include <vector>
#include "Interface/Memory.hh"

class MCVertex;

class MCParticle
{
  public :

  MCParticle();

  MCParticle( int, double, int, double, double, double, Hep3Vector, Hep3Vector, Hep3Vector, Hep3Vector, double, double, 
              bool isRef);

  ~MCParticle() { miceMemory.addDelete( Memory::MCParticle ); };

  void      SetVertices( MCVertex* ini_vertex, MCVertex* fin_vertex );

  void                  SetInitialVertex(MCVertex* ini_vertex) {m_ini_vertex = ini_vertex;};
  void                  SetFinalVertex(MCVertex* fin_vertex) {m_fin_vertex = fin_vertex;};

  int       trackID() const     { return m_trackID; };

  double    charge() const      { return m_charge; };

  int       pdg() const       { return m_pdg; };

  double    mass() const      { return m_mass; };

  double    initialKinE() const     { return m_ini_kinE; };

  double    finalKinE() const     { return m_fin_kinE; };

  Hep3Vector    initialPosition() const   { return m_ini_pos; };

  Hep3Vector    finalPosition() const     { return m_fin_pos; };

  Hep3Vector    initialMomentum() const   { return m_ini_mom; };

  Hep3Vector    finalMomentum() const   { return m_fin_mom; };

  double    initialTime() const     { return m_ini_time; };

  double    finalTime() const   { return m_fin_time; };

  bool      isRefPart() const        { return m_ref_part; };

  void      setRefPart( bool r )    { m_ref_part = r; };
  
  MCVertex*     initialVertex() const     { return m_ini_vertex; };

  MCVertex*     finalVertex() const   { return m_fin_vertex; };

  inline double initialTotalE() const { return sqrt(m_ini_mom.mag2() + m_mass*m_mass); };

  inline double finalTotalE()   const { return sqrt(m_fin_mom.mag2() + m_mass*m_mass); };

  unsigned int	numPoints() const		{ return _points.size(); };

  Hep3Vector	point( int i ) const		{ return _points[i]; };

  void		addPoint( Hep3Vector p )	{ _points.push_back( p ); };

  void		replacePoints( std::vector<Hep3Vector>& pts );

private :

    int m_trackID;
    double m_charge;
    int m_pdg;
    double m_mass;
    double m_ini_kinE;
    double m_fin_kinE;
    Hep3Vector m_ini_pos;
    Hep3Vector m_fin_pos;
    Hep3Vector m_ini_mom;
    Hep3Vector m_fin_mom;
    double m_ini_time;
    double m_fin_time;
    bool m_ref_part;
    
    std::vector<Hep3Vector>	_points;

    MCVertex* m_ini_vertex;
    MCVertex* m_fin_vertex;
};

#endif

