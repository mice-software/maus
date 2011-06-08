// TofPid.cc - a matching between a track and a TOF space point

#include "TofPid.hh"

TofPid::TofPid( Extrapolation e, TofSpacePoint* p )
{
  m_extrap = e;
  m_point = p;
}

Hep3Vector		TofPid::position() const
{
  return m_point->position();
}

double			TofPid::time() const
{
  return m_point->time();
}

const Extrapolation&	TofPid::extrapolation() const
{
  return m_extrap;
}

