/** @class MapCpp
 *  Base class for C++ mappers
 *
 *  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>
 *
 *  This class is meant to be inherited by all C++ mappers
 *
 */

#ifndef __MapCpp__
#define __MapCpp__

#include <json/json.h>

// MICE
#include "Simulation/MICEEventAction.hh"  
#include "Simulation/MICEPhysicsList.hh"
#include "Simulation/MICEPrimaryGeneratorAction.hh"
#include "Simulation/MICESteppingAction.hh"
#include "Simulation/MICERunAction.hh"
#include "Simulation/MICEStackingAction.hh"
#include "Simulation/MICETrackingAction.hh" 

class MapCppSimulation : MapCpp
{
public: 
  /** Begin the startup procedure for Simulation
   *
   *  This takes no aruments.  This constructs the geometry
   *  and prepares geant4 for being able to run beamOn(1)
   */
  bool Birth();

  /** Shutdowns the Simulation by closing files
   *
   *  This takes no arguments
   */
  bool Death();

  /** Process JSON input
   *
   *  The argument is a character array that is proper JSON
   *  code.  I use character arrays since it's easier to write
   *  SWIG for this.  Will extend this to use stl::strings.
   */
  char* Process(char* document);

private:
  G4RunManager* runManager;
  MICEPhysicsList* physList;
  MICEPrimaryGeneratorAction * primary;
  MICESteppingAction*          stepActM;
  MICEEventAction*             eventAct;
  MICETrackingAction*          trackAct;
  int numberOfEvents;

};  // Don't forget this trailing colon!!!!

#endif
