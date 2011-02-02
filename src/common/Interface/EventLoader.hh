// @(#) $Id: EventLoader.hh,v 1.1 2004-11-28 22:48:41 torun Exp $ $Name:  $
// Declaration of abstract class EventLoader


#if !defined(EVENT_LOADER_HH)
#define EVENT_LOADER_HH



#include "MICEEvent.hh"



class EventLoader {
public:
  virtual MICEEvent& getEvent(int num) = 0;
  virtual MICEEvent& getNextEvent() = 0;
  virtual bool hasMoreEvents() = 0;
};



#endif // !defined(EVENT_LOADER_HH)
