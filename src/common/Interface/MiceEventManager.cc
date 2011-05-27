// MiceEventManager.cc
//
// M.Ellis 25/8/2005

#include "Interface/MiceEventManager.hh"

#include "Interface/KLHit.hh"
#include "Interface/VirtualHit.hh"
#include "Interface/MCParticle.hh"
#include "Interface/ZustandVektor.hh"
#include "Interface/CkovHit.hh"
#include "Interface/RFData.hh"
#include "Interface/StagePosition.hh"
#include "Interface/KillHit.hh"
#include "Interface/EventTime.hh"


#include "Interface/Squeak.hh"

template <class T>
std::vector<T*> deepCopyVector(std::vector<T*>& target)
{
  std::vector<T*> myV(target.size());
  for(unsigned int i=0; i<target.size(); i++)
  {
    if(target[i]) myV[i] = new T(*target[i]);
    else          myV[i] = 0;
  }
  return myV;
}

template <class T>
void            deleteVector(std::vector<T*>& target)
{
  for(unsigned int i=0; i<target.size(); ++i )
    if(target[i]) delete target[i];
  target.clear();
}

void    clearEvent( MICEEvent& event )
{

  if(event.stagePosition) delete event.stagePosition;
  event.stagePosition = NULL;

  //Would be nice to do this without cut and paste... but at least this should be pretty well checked at compile time  
  deleteVector<CkovHit>             (event.ckovHits);
  deleteVector<KLHit>               (event.klHits);
  deleteVector<VirtualHit>          (event.virtualHits);
  deleteVector<MCParticle>          (event.mcParticles);
  deleteVector<ZustandVektor>       (event.zustandVektors);
  deleteVector<RFData>              (event.rfData);
  deleteVector<KillHit>             (event.killHits);
  deleteVector<PrimaryGenHit>       (event.pgHits);
  deleteVector<EventTime>           (event.eventTimes);
}

MICEEvent* deepCopyEvent(MICEEvent& event)
{
  //come cut and paste with me...
  MICEEvent* ev_out = new MICEEvent();

  if(event.stagePosition) ev_out->stagePosition = new StagePosition(*event.stagePosition);
  else                    ev_out->stagePosition = NULL;

  ev_out->ckovHits          = deepCopyVector<CkovHit>             (event.ckovHits);
  ev_out->klHits            = deepCopyVector<KLHit>               (event.klHits);
  ev_out->virtualHits       = deepCopyVector<VirtualHit>          (event.virtualHits);
  ev_out->mcParticles       = deepCopyVector<MCParticle>          (event.mcParticles);
  ev_out->zustandVektors    = deepCopyVector<ZustandVektor>       (event.zustandVektors);
  ev_out->rfData            = deepCopyVector<RFData>              (event.rfData);
  ev_out->killHits          = deepCopyVector<KillHit>             (event.killHits);
  ev_out->pgHits            = deepCopyVector<PrimaryGenHit>       (event.pgHits);
  ev_out->eventTimes        = deepCopyVector<EventTime>           (event.eventTimes);

  return ev_out;
}



