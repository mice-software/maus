// MAUS WARNING: THIS IS LEGACY CODE.
// MCParticle.cc

#include "Interface/MCParticle.hh"

MCParticle::MCParticle()
{
  _points.resize(0);
}

MCParticle::MCParticle( int trackid, double charge, int pdg, double mass, double ike, double fke, Hep3Vector ipos, Hep3Vector fpos, Hep3Vector imom, Hep3Vector fmom, double it, double ft , bool isRef)
{
  
  m_trackID = trackid;
  m_charge = charge;
  m_pdg = pdg;
  m_mass = mass;
  m_ini_kinE = ike;
  m_fin_kinE = fke;
  m_ini_pos = ipos;
  m_fin_pos = fpos;
  m_ini_mom = imom;
  m_fin_mom = fmom;
  m_ini_time = it;
  m_fin_time = ft;
  m_ini_vertex = NULL;
  m_fin_vertex = NULL;
  m_ref_part = isRef;
  _points = std::vector<Hep3Vector>(0);
}

 void MCParticle::SetVertices( MCVertex* ini_vertex, MCVertex* fin_vertex )
  {
    m_ini_vertex = ini_vertex;
    m_fin_vertex = fin_vertex;
  }

void 	MCParticle::replacePoints( std::vector<Hep3Vector>& pts )
{
  _points.resize( pts.size() );
  for( unsigned int i = 0; i < pts.size(); ++i )
    _points[i] = pts[i];
}
