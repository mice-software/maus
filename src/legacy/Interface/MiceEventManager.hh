// MAUS WARNING: THIS IS LEGACY CODE.
// MiceEventManager.hh
//
// M.Ellis 25/8/2005



#ifndef PERSIST_MICEEVENTMANAGER_H
#define PERSIST_MICEEVENTMANAGER_H

#include "Interface/MICEEvent.hh"

void	clearEvent( MICEEvent& );

//Make a copy of MICEEvent* and all members
MICEEvent* deepCopyEvent(MICEEvent& ev_in);

//Return a deepcopy of std::vector<T*> where T is some class
//If members are NULL, copy a NULL
//e.g. std::vector<VlpcHit*> usage dc_vlpc_hits = DeepCopyVector<VlpcHit*>( vlpcHits ); 
template <class T>
std::vector<T*> deepCopyVector(std::vector<T*>& target);

//Delete all members in std::vector<T*> where T is some class
//Ignore members that are already NULL
//e.g. std::vector<VlpcHit*> usage dc_vlpc_hits = DeepCopyVector<VlpcHit*>( vlpcHits ); 

template <class T>
void            deleteVector  (std::vector<T*>& target);

#endif

