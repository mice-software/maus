#!/bin/env python
#
# Class to duplicate the stdout from python into a logfile
#

import sys

actual_stdout = sys.stdout

class Tee( object ) :
  """
    A class designed to behave similar the the bash "tee" command.
    It duplicates the stdout to a log file. A simple way to handle logging.

    Simply create an instance of this object to use.
    During the lifetime of this object all stdout will be duplicated to the 
    supplied filename.
  """
  def __init__( self, filename, mode = 'w' ) :
    """
      Initialise the class.

      Remember where stdout is and create a output file.
    """
    self.logfile = open( filename, mode )
    self.stdout = sys.stdout
    sys.stdout = self

  def __del__( self ) :
    """
      Flush everything, close the log file and restore stdout.
    """
    sys.stdout.flush()
    self.logfile.flush()
    sys.stdout = self.stdout
    self.logfile.close()

  def write( self, data ) :
    """
      Write to both stdout and the log file
    """
    self.logfile.write( data )
    self.stdout.write( data )

  def flush( self ) :
    """
      Flush both stdout and the log file
    """
    self.logfile.flush()
    self.stdout.flush()

  def no_logging( self, data ) :
    """
      Write only to stdout. Useful to prevent the log file from becoming
      cluttered.
    """
    self.stdout.write( data )

