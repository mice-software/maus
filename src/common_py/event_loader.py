#!/usr/bin/env python

# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#


import libMausCpp
import ROOT

import gc

class maus_reader() :
  """
    A nice interface for loading MAUS ROOT data.

    Can be initialised with one filename, or a list of many filenames.
    Each file is loaded into memory in turn, with simple functions to extract the 
    useful class and types from the file.

    This class does not offer full coverage of the datastructure, purely a simplified
    interface to the higher level objects.
  """
  def __init__( self, filename ) :
    """
      Initialise the class variables and load the first file handle
    """
    if isinstance( filename, list ) :
      self.__num_files = len( filename )
      self.__current_filename = filename[0]
      self.__filenames = filename
    else :
      self.__num_files = 1
      self.__current_filename = filename
      self.__filenames = [ filename ]

    self.__current_filenumber = -1

    self.__tree = None
    self.__data = None
    self.__spill = None

    self.__root_file = None

    self.__current_spill_num = -1
    self.__current_num_spills = 0
    self.__current_event_num = -1
    self.__current_num_events = 0

    self.__total_num_spills = 0
    self.__total_num_events = 0

#    self.next_event()


  def reset( self ) :
    self.__current_filename = self.__filenames[0]
    self.__current_filenumber = -1

    self.__tree = None
    self.__data = None
    self.__spill = None

    self.__root_file = None

    self.__current_spill_num = -1
    self.__current_num_spills = 0
    self.__current_event_num = -1
    self.__current_num_events = 0

    self.__total_num_spills = 0
    self.__total_num_events = 0
    gc.collect()


  def __load_file( self ) :
    """
      Loads the next file in the file list.
    """
    if self.__root_file is not None :
      self.__root_file.Close()
      gc.collect()

    self.__root_file = ROOT.TFile( self.__current_filename, "READ" )
    if self.__root_file.IsZombie() :
      self.__root_file.Close()
      raise IOError( 'Could not open ROOT File: '+str( self.__current_filename ) )

    self.__data = ROOT.MAUS.Data()
    self.__tree = self.__root_file.Get("Spill")
    if not self.__tree :
      raise IOError( 'No tree extracted from ROOT File: '+str( self.__current_filename ) )
    self.__tree.SetBranchAddress("data", self.__data)

    self.__current_num_spills = self.__tree.GetEntries()
    self.__current_spill_num = 0
    self.__current_event_num = 0
    self.__current_num_events = 0
    


  def __increment_filename( self ) :
    """
      Increment the current position in the list of files.
      If one is found return True, else return False.
    """
    self.__current_filenumber += 1
    if self.__current_filenumber < self.__num_files :
      self.__current_filename = self.__filenames[ self.__current_filenumber ]
      return True
    else :
      gc.collect()
      return False


  def next_event( self ) :
    """
      Load the next event into memory.
    """
    self.__current_event_num += 1
    if self.__current_event_num >= self.__current_num_events :
      if self.next_spill() is False :
        return False

    self.__total_num_events += 1
    return True


  def next_spill( self ) :
    """
      Loads the next spill, in the current event into memory.
      If one is available return True, else return False
    """
    self.__current_spill_num += 1

    if self.__current_spill_num >= self.__current_num_spills :
      if self.__increment_filename() :
        try :
          self.__load_file()
        except IOError as ex:
          print ex
          return self.next_spill()
      else :
        return False

    self.__tree.GetEntry( self.__current_spill_num )
    self.__spill = self.__data.GetSpill()
    self.__current_num_events = self.__spill.GetReconEvents().size()
    self.__current_event_num = 0

    if self.__spill.GetDaqEventType() != "physics_event" :
      return self.next_spill()

    self.__total_num_spills += 1
    return True


  def get_current_filename( self ) :
    """
      Return the name of the current data file.
    """
    return self.__current_filename

  def get_current_filenumber( self ) :
    """
      Return the current position in the list of filenames
    """
    return self.__current_filenumber + 1

  def get_number_files( self ) :
    """
      Return the number of files stored to open.
    """
    return self.__num_files

  def get_current_number_spills( self ) :
    """
      Return the number of spills in the current file.
    """
    return self.__current_num_spills

  def get_current_spill_number( self ) :
    """
      Return the current spill number, in the current file.
    """
    return self.__current_spill_num + 1

  def get_current_number_events( self ) :
    """
      Return the number of events in the current spill.
    """
    return self.__current_num_events

  def get_current_event_number( self ) :
    """
      Return the current event number, in the current spill.
    """
    return self.__current_event_num + 1

  def get_spill( self ) :
    """
      Return a reference to the current spill being read.
    """
    return self.__spill

  def get_total_num_spills( self ) :
    """
      Return the total number of spills read so far.
    """
    return self.__total_num_spills

  def get_total_num_events( self ) :
    """
      Return the total number of events read so far.
    """
    return self.__total_num_events

  def is_good( self ) :
    """
      Returns True if the class still has events left to load.
    """
    if self.__current_event_num < self.__current_num_events and \
        self.__current_spill_num < self.__current_num_spills and \
        self.__current_filenumber < self.__num_files :
      return True
    else :
      return False
      

  def get_event( self, event_type ) :
    """
      Return a specific event type from the current spill.
    """
    if event_type == 'scifi' :
      return self.__spill.GetReconEvents()[ self.__current_event_num ].GetSciFiEvent()
    elif event_type == 'tof' :
      return self.__spill.GetReconEvents()[ self.__current_event_num ].GetTOFEvent()
    elif event_type == 'ckov' :
      return self.__spill.GetReconEvents()[ self.__current_event_num ].GetCkovEvent()
    elif event_type == 'kl' :
      return self.__spill.GetReconEvents()[ self.__current_event_num ].GetKLEvent()
    elif event_type == 'emr' :
      return self.__spill.GetReconEvents()[ self.__current_event_num ].GetEMREvent()
    elif event_type == 'trigger' :
      return self.__spill.GetReconEvents()[ self.__current_event_num ].GetTriggerEvent()
    elif event_type == 'global' :
      return self.__spill.GetReconEvents()[ self.__current_event_num ].GetGlobalEvent()
    elif event_type == 'mc' :
      return self.__spill.GetMCEvents()[ self.__current_event_num ]
    else :
      return None

