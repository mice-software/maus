// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: MICEEvent.hh,v 1.21 2010-03-08 16:39:02 rogers Exp $ $Name:  $
// Declaration of struct MICEEvent
// Struct to encapsulate hits/digits/recon in a G4Mice event

#if !defined(MICE_EVENT_HH)
#define MICE_EVENT_HH

// Standard C++ includes
#include <vector>
#include <map>
#include <iostream>//new

// Forward declarations
class VlpcHit;
class VlpcEventTime;
class CkovHit;
class CkovDigit;
class EMRHit;
class EMRDigit;
class KLHit;
class KLDigit;
class KLCellHit;
class SciFiHit;
class SciFiDigit;
class SciFiDoubletCluster;
class SciFiSpacePoint;
class SciFiTrack;
class SciFiKalTrack;
class SciFiExtrap;
class TofHit;
class TofDigit;
class TofSlabHit;
class TofSpacePoint;
class TofTrack;
class VmeAdcHit;
class VmefAdcHit;
class VmeTdcHit;
class EmCalHit;
class EmCalDigit;
class EmCalTrack;
class VirtualHit;
class SpecialHit;
class MCParticle;
class MCVertex;
class MCHit;
class ZustandVektor;
class RFData;
class StagePosition;
class KillHit;
class EventTime;
class PrimaryGenHit;

/**
 * Encapsulates banks from a G4Mice event.  There is a vector for each
 * type of hit or digit.  Hits and digits are stored in these vectors.
 */
class MICEEvent 
{
  public :

  MICEEvent();

  ~MICEEvent() {};

  int event;
  std::vector<VlpcHit*>             vlpcHits;
  VlpcEventTime*                    vlpcEventTime;
  std::vector<CkovHit*>             ckovHits;
  std::vector<CkovDigit*>           ckovDigits;
  std::vector<EMRHit*>              emrHits;
  std::vector<EMRDigit*>            emrDigits;
  std::vector<KLHit*>               klHits;
  std::vector<KLDigit*>             klDigits;
  std::vector<KLCellHit*>           klCellHits;
  std::vector<SciFiHit*>            sciFiHits;
  std::vector<SciFiDigit*>          sciFiDigits;
  std::vector<SciFiDoubletCluster*> sciFiClusters;
  std::vector<SciFiSpacePoint*>     sciFiSpacePoints;
  std::vector<SciFiTrack*>          sciFiTracks;
  std::vector<SciFiKalTrack*>       sciFiKalTracks;
  std::vector<SciFiExtrap*>         sciFiExtraps;
  std::vector<TofHit*>              tofHits;
  std::vector<TofDigit*>            tofDigits;
  std::vector<TofSlabHit*>          tofSlabHits;
  std::vector<TofSpacePoint*>       tofSpacePoints;
  std::vector<TofTrack*>            tofTracks;
  std::vector<VmeAdcHit*>           vmeAdcHits;
  std::vector<VmefAdcHit*>          vmefAdcHits;
  std::vector<VmeTdcHit*>           vmeTdcHits;
  std::vector<EmCalHit*>            emCalHits;
  std::vector<EmCalDigit*>          emCalDigits;
  std::vector<EmCalTrack*>          emCalTracks;
  std::vector<VirtualHit*>          virtualHits;
  std::vector<SpecialHit*>          specialHits;
  std::vector<MCParticle*>          mcParticles;
  std::vector<MCVertex*>            mcVertices;
  std::vector<MCHit*>               mcHits;
  std::vector<ZustandVektor*>       zustandVektors;
  std::vector<RFData*>              rfData;
  std::vector<KillHit*>             killHits;
  std::vector<PrimaryGenHit*>       pgHits;
  std::vector<EventTime*>	    eventTimes;
  StagePosition*		    stagePosition;

  void Print();
};

#endif //!defined(MICE_EVENT_HH)
