/*
 * MiceMessenger.hh		2003-06-25
 *
 * Defines a Geant4 messenger class, MiceMessenger, to handle MICE-specific
 * commands.
 */

#ifndef MiceMessenger_h
#define MiceMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;

/**
 * Geant4 messenger class to handle MICE-specific commands in the MICE
 * Simulation executable.  This class is intended to be a global singleton.
 * This class's responsibility is to create a directory "/mice/" and construct
 * each of the messengers that handle subdirectories of "/mice/".  Currently,
 * these messenger classes and their corresponding directories are:
 * DataCardsMessenger	"/mice/dataCards/"
 * <p>To make use of this class, Geant4's bash-type shell must be enabled.
 *
 * @author	Hart Wilson
 */
class MiceMessenger: public G4UImessenger {

private:
  G4UIdirectory*        miceDir;

public:
  /**
   * Constructs a new MiceMessenger object.  Calls the constructors of the
   * subdirectory messengers.
   *
   */
  MiceMessenger();

  /**
   * Destroys the MiceMessenger object, cleaning up all objects created in the
   * constructor.
   */
  ~MiceMessenger();

  /**
   * Processes a command from the shell.  This method should generally not be
   * called by the user.  It is used by the Geant4 shell to signal that a
   * command belonging to this messenger has been given.
   * <p>This method currently does nothing as their are no commands.
   *
   * @param command  the command that was given
   * @param newValue the parameter given
   */
  void SetNewValue(G4UIcommand* command,G4String newValue);
};

#endif

