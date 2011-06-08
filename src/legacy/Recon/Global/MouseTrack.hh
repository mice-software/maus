// MouseTrack.hh - highest level track (from any potential tracker) in MICE

#ifndef REC_MOUSETRACK_HH
#define REC_MOUSETRACK_HH

#include <vector>

// Kalman

// PIDs
#include "TofPid.hh"

class MouseTrack
{
  public :

   virtual ~MouseTrack();

   virtual Extrapolation			extrapolateToZ( double ) const;
 
   virtual int						tracker() const = 0;

   virtual void					addTofPid( TofPid* );
   // other PIDs here...

   virtual const std::vector<MousePid*>&	mousePids() const;  
   virtual const std::vector<TofPid*>&		tofPids() const;
   // other PIDs here...

  private :

   std::vector<MousePid*>	m_pids;
   std::vector<TofPid*>		m_tof_pids;
   // other PIDs here...

  protected :
};

#endif
