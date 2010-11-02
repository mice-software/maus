/*
 * DataCardsMessenger.hh	2003-06-16
 *
 * Defines a Geant4 messenger class, DataCardsMessenger, to be associated
 * with the dataCards class.
 */

#ifndef DataCardsMessenger_h
#define DataCardsMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class dataCards;
class G4UIdirectory;
class G4UIcmdWithAString;

/**
 * Geant4 messenger class for the dataCards class.  Like the dataCards class,
 * this class is intended to be a global singleton.  However, this class must
 * be explicitly instantiated.
 * <p>To make use of this class, Geant4's bash-type shell must be enabled.
 * This class creates a directory, "/mice/", and a command to print the value of
 * a data card "printDataCard".  This command invokes the corresponding
 * dataCards method with the string parameter given.  The output is to stdout.
 *
 * @author	Hart Wilson
 */
class DataCardsMessenger: public G4UImessenger {

private:
  dataCards*            myDataCards;
  G4UIdirectory*        dataCardsDir;
  G4UIcmdWithAString*   printDataCardCmd;
public:
  /**
   * Constructs a new DataCardsMessenger object associated with the dataCards
   * object pointed to by cards.
   *
   * @param cards a pointer to the dataCards object.
   */
  DataCardsMessenger(dataCards* cards);

  /**
   * Destroys the DataCardsMessenger object, cleaning up all objects created
   * in the constructor.
   */
  ~DataCardsMessenger();

  /**
   * Processes a command from the shell.  This method should generally not be
   * called by the user.  It is used by the Geant4 shell to signal that a
   * command belonging to this messenger has been given.
   *
   * @param command  the command that was given
   * @param newValue the parameter given
   */
  void SetNewValue(G4UIcommand* command,G4String newValue);
};

#endif

