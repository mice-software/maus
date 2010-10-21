/** @class CppPrint
 *  Print events to some stream
 *
 *  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>
 *
 *  processor which simulates the trigger.
 *
 *  The "update" parameter controls the update interval.  See SetI()
 *
 */
#ifndef __CppPrint__
#define __CppPrint__
#include <json/json.h>

/**
 * This class is the simulation class
 */
class CppPrint
{
public: 
  /** Create new count processor.
   *
   *  Default update interval is to print a status line for every
   *  physics event.
   */
  CppPrint();

  //  int ProcessEvent(TMap run, TMap spill, TMap event);
  
  char* Process(char* document);

};  // Don't forget this trailing colon!!!!

#endif
