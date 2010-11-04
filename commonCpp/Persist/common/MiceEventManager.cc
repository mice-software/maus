// MiceEventManager.cc
//
// M.Ellis 25/8/2005

#include "MiceEventManager.hh"

#include "Interface/VlpcHit.hh"
#include "Interface/VlpcEventTime.hh"
#include "Interface/SciFiHit.hh"
#include "Interface/SciFiDigit.hh"
#include "Recon/SciFi/SciFiDoubletCluster.hh"
#include "Recon/SciFi/SciFiTrack.hh"
#include "Recon/SciFi/SciFiKalTrack.hh"
#include "Recon/SciFi/SciFiExtrap.hh"
#include "Interface/TofHit.hh"
#include "Interface/TofDigit.hh"
#include "Recon/TOF/TofSlabHit.hh"
#include "Recon/TOF/TofSpacePoint.hh"
#include "Recon/TOF/TofTrack.hh"
#include "Interface/VmefAdcHit.hh"
#include "Interface/VmeAdcHit.hh"
#include "Interface/VmeTdcHit.hh"
#include "Interface/EmCalHit.hh"
#include "Interface/EmCalDigit.hh"
#include "Interface/EmCalTrack.hh"
#include "Interface/KLHit.hh"
#include "Interface/KLDigit.hh"
#include "Recon/KL/KLCellHit.hh"
#include "Interface/VirtualHit.hh"
#include "Interface/SpecialHit.hh"
#include "Interface/MCParticle.hh"
#include "Interface/MCVertex.hh"
#include "Interface/ZustandVektor.hh"
#include "Interface/CkovHit.hh"
#include "Interface/CkovDigit.hh"
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

  if(event.vlpcEventTime) delete event.vlpcEventTime;
  event.vlpcEventTime = NULL;

  if(event.stagePosition) delete event.stagePosition;
  event.stagePosition = NULL;

  //Would be nice to do this without cut and paste... but at least this should be pretty well checked at compile time  
  deleteVector<VlpcHit>             (event.vlpcHits);
  deleteVector<SciFiHit>            (event.sciFiHits);
  deleteVector<SciFiDigit>          (event.sciFiDigits);
  deleteVector<SciFiDoubletCluster> (event.sciFiClusters);
  deleteVector<SciFiSpacePoint>     (event.sciFiSpacePoints);
  deleteVector<SciFiTrack>          (event.sciFiTracks);
  deleteVector<SciFiKalTrack>       (event.sciFiKalTracks);
  deleteVector<SciFiExtrap>         (event.sciFiExtraps);
  deleteVector<TofHit>              (event.tofHits);
  deleteVector<TofDigit>            (event.tofDigits);
  deleteVector<TofSlabHit>          (event.tofSlabHits);
  deleteVector<TofSpacePoint>       (event.tofSpacePoints);
  deleteVector<TofTrack>            (event.tofTracks);
  deleteVector<VmeAdcHit>           (event.vmeAdcHits);
  deleteVector<VmefAdcHit>          (event.vmefAdcHits);
  deleteVector<VmeTdcHit>           (event.vmeTdcHits);
  deleteVector<CkovDigit>           (event.ckovDigits);
  deleteVector<CkovHit>             (event.ckovHits);
  deleteVector<EmCalTrack>          (event.emCalTracks);
  deleteVector<EmCalDigit>          (event.emCalDigits);
  deleteVector<EmCalHit>            (event.emCalHits);
  deleteVector<KLHit>               (event.klHits);
  deleteVector<KLDigit>             (event.klDigits);
  deleteVector<KLCellHit>           (event.klCellHits);
  deleteVector<VirtualHit>          (event.virtualHits);
  deleteVector<SpecialHit>          (event.specialHits);
  deleteVector<MCParticle>          (event.mcParticles);
  deleteVector<MCVertex>            (event.mcVertices);
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

  if(event.vlpcEventTime) ev_out->vlpcEventTime = new VlpcEventTime(*event.vlpcEventTime);
  else                    ev_out->vlpcEventTime = NULL;

  if(event.stagePosition) ev_out->stagePosition = new StagePosition(*event.stagePosition);
  else                    ev_out->stagePosition = NULL;

  ev_out->vlpcHits          = deepCopyVector<VlpcHit>             (event.vlpcHits);
  ev_out->sciFiHits         = deepCopyVector<SciFiHit>            (event.sciFiHits);
  ev_out->sciFiDigits       = deepCopyVector<SciFiDigit>          (event.sciFiDigits);
  ev_out->sciFiClusters     = deepCopyVector<SciFiDoubletCluster> (event.sciFiClusters);
  ev_out->sciFiSpacePoints  = deepCopyVector<SciFiSpacePoint>     (event.sciFiSpacePoints);
  ev_out->sciFiTracks       = deepCopyVector<SciFiTrack>          (event.sciFiTracks);
  ev_out->sciFiKalTracks    = deepCopyVector<SciFiKalTrack>       (event.sciFiKalTracks);
  ev_out->sciFiExtraps      = deepCopyVector<SciFiExtrap>         (event.sciFiExtraps);
  ev_out->tofHits           = deepCopyVector<TofHit>              (event.tofHits);
  ev_out->tofDigits         = deepCopyVector<TofDigit>            (event.tofDigits);
  ev_out->tofSlabHits       = deepCopyVector<TofSlabHit>          (event.tofSlabHits);
  ev_out->tofSpacePoints    = deepCopyVector<TofSpacePoint>       (event.tofSpacePoints);
  ev_out->tofTracks         = deepCopyVector<TofTrack>            (event.tofTracks);
  ev_out->vmeAdcHits        = deepCopyVector<VmeAdcHit>           (event.vmeAdcHits);
  ev_out->vmefAdcHits       = deepCopyVector<VmefAdcHit>          (event.vmefAdcHits);
  ev_out->vmeTdcHits        = deepCopyVector<VmeTdcHit>           (event.vmeTdcHits);
  ev_out->ckovDigits        = deepCopyVector<CkovDigit>           (event.ckovDigits);
  ev_out->ckovHits          = deepCopyVector<CkovHit>             (event.ckovHits);
  ev_out->emCalTracks       = deepCopyVector<EmCalTrack>          (event.emCalTracks);
  ev_out->emCalDigits       = deepCopyVector<EmCalDigit>          (event.emCalDigits);
  ev_out->emCalHits         = deepCopyVector<EmCalHit>            (event.emCalHits);
  ev_out->klHits            = deepCopyVector<KLHit>               (event.klHits);
  ev_out->klDigits          = deepCopyVector<KLDigit>             (event.klDigits);
  ev_out->klCellHits        = deepCopyVector<KLCellHit>           (event.klCellHits);
  ev_out->virtualHits       = deepCopyVector<VirtualHit>          (event.virtualHits);
  ev_out->specialHits       = deepCopyVector<SpecialHit>          (event.specialHits);
  ev_out->mcParticles       = deepCopyVector<MCParticle>          (event.mcParticles);
  ev_out->mcVertices        = deepCopyVector<MCVertex>            (event.mcVertices);
  ev_out->zustandVektors    = deepCopyVector<ZustandVektor>       (event.zustandVektors);
  ev_out->rfData            = deepCopyVector<RFData>              (event.rfData);
  ev_out->killHits          = deepCopyVector<KillHit>             (event.killHits);
  ev_out->pgHits            = deepCopyVector<PrimaryGenHit>       (event.pgHits);
  ev_out->eventTimes        = deepCopyVector<EventTime>           (event.eventTimes);

  return ev_out;
}



