// TofPid.hh - a matching between a track and a TOF space point

#ifndef REC_TOFPID_HH
#define REC_TOFPID_HH

#include "MousePid.hh"
#include "Recon/TOF/TofSpacePoint.hh"

class TofPid : public MousePid
{
  public :

  TofPid( Extrapolation, TofSpacePoint* );

  virtual ~TofPid() {};

  Hep3Vector		position() const;
  double		time() const;
  const Extrapolation&	extrapolation() const;

  const TofSpacePoint*  point() const { return m_point; };

  private :

  Extrapolation m_extrap;
  TofSpacePoint* m_point;
};

#endif

