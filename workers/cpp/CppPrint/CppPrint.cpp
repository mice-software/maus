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

#include <iostream>
#include <TMap.h>

/**
 * This class is the simulation class
 */
class CppPrint ////: public Processor TODO
{
private:
  int x; /// Data member x
  int y; /// Data member y
public: 
  /** Create new count processor.
   *
   *  Default update interval is to print a status line for every
   *  physics event.
   */
  CppPrint()
  { 
    x = 0;
    y = 0;
  }

  /** Destroy count processor. */
  ~CppPrint() 
  { } 
  
  int ProcessEvent(TMap run, TMap spill, TMap event) 
  { 
    return x+y;
  }

  char* Process(char* document){
    std::cout<<document<<std::endl;
  }
  

};  // Don't forget this trailing colon!!!!
