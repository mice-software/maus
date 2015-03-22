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


import ROOT
import MAUS
import numpy as np

import copy
import math
import gc

from track_point import track_point


class cut() :
  """
    A simple class that stores the information required to cut particles
    from the analysis.

    Records the variables, values and operation required, in addition to the 
    number of particles that fail the cut.
  """
  def __init__( self, variable_name, value, operation ) :
    """
      Generic class for storing cut information.
      Supply the name of the variable to act on, the value to use and the operation.

      e.g. cut( "px", 300.0, operator.lt )
      Accepts all standard Python operator module functions, plus any you can think
      to make up.
    """
    self.__variable = variable_name
    self.__value = value
    self.__operation = operation
    self.__counter = 0


  def variable( self ) :
    """
      Return the name of the variable to act on
    """
    return self.__variable

  def value( self ) :
    """
      Return the value used in the cut.
    """
    return self.__value

  def operation( self ) :
    """
      Return the operation function.
    """
    return self.__operation

  def count ( self ) :
    """
      Return the number of times this cut is broken.
    """
    return self.__counter

  def cuts( self, value ) :
    """
      Supply a value to the cut to check.
      If it is broken, the counter is incremented and "True" is returned.
      Otherwise "False" is returned.
    """
    if self.__operation( value, self.__value ) :
      self.__counter += 1
      return True
    else :
      return False




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


  def __load_file( self ) :
    """
      Loads the next file in the file list.
    """
    if self.__root_file is not None :
      self.__root_file.Close()
      self.__root_file = None
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



####################################################################################################
####################################################################################################



class scifi_hits() :
  """
    Class that handles the extraction of virtual hit data from MC events.

    Cuts can be added and applied to the hits on extraction.

    Reference planes must be added. Cuts are only applied to the reference plane
    hits, allowing for faster processing using only the relevant hits.
  """
  def __init__( self, plane_positions = [] ) :
    """
      Initialise the class, set sums and counters to zero.
    """
    self.__number_tracks = 0
    self.__current_number_tracks = 0
    self.__missing_trackpoints = 0
    self.__current_missing = False
    self.__plane_positions = plane_positions
    self.__cut_missing = True

    self.__number_cut = 0
    self.__number_passed = 0
    self.__cut_flag = False

    self.__cuts = []
    self.__trackpoints = []
    self.__reference_planes = []


  def load_tracks( self, mc_event ) :
    """
      Load the mc hits from the mc_event into memory such that the reference
      plane hits can be found and cuts and be applied.
    """
    self.__current_number_tracks = 0
    self.__current_missing = False
    self.__cut_flag = False
    self.__trackpoints = []

    hits = mc_event.GetSciFiHits()
    for hit in hits :
      if hit.GetTrackId() != 1 :
        continue
      channel = hit.GetChannelId()
      plane = channel.GetPlaneNumber()
      station = channel.GetStationNumber()
      tracker = channel.GetTrackerNumber()
      loc = ( tracker, station, plane )
      self.__trackpoints.append( ( loc, track_point( virtual_track_point = hit, reference=str(tracker)+"."+str(station)+"."+str(plane) ) ) )

    self.__number_tracks += len( hits )
    self.__current_number_tracks = len( hits )


  def is_cut( self ) :
    """
      Checks to see if any of the reference plane track points violate any
      of the cuts stored in the class.

      If they do, return True, else return False.
    """
    self.__cut_flag = False
    references = self.get_reference_trackpoints()

    if self.__cut_missing and ( len( references ) != len( self.__reference_planes ) or self.__current_missing is True ) :
      self.__missing_trackpoints += 1
      self.__number_cut += 1
      return True

    for tp in references :
      for c in self.__cuts :
        if c.cuts( tp.get( c.variable() ) ) :
          self.__cut_flag = True

    if self.__cut_flag is True :
      self.__number_cut += 1
      return True
    else :
      self.__number_passed += 1
      return False


  def add_cut( self, variable_name, value, operation ) :
    """
      Add a cut to the object to apply.
    """
    if not variable_name in track_point.get_variable_list() :
      raise ValueError( str( variable_name ) + ' is not a valid hit variable.' )
    self.__cuts.append( cut( variable_name, value, operation ) )


  def get_number_cuts( self ) :
    """
      Return the number of cuts in the object.
    """
    return len( self.__cuts )


  def get_reference_planes( self ) :
    """
      Return a copy of the list of reference planes being used
    """
    return copy.deepcopy( self.__reference_planes )


  def add_reference_plane( self, tracker, station, plane, reference_string ) :
    """
      Add a reference plane to the object. Planes in the tracker are deduced from 
      the tracker, station and plane numbers. The reference string allows for a 
      method of identifying which reference plane is which.
      e.g. "upstream" or "downstream" are good suggestions
    """
    self.__reference_planes.append( ( reference_string, ( tracker, station, plane ) ) )


  def get_reference_trackpoints( self ) :
    """
      Returns a list of scifi hits that were actually made on the reference
      planes.
    """
    points = []
    for loc, tp in self.__trackpoints :
      for ids, ref in self.__reference_planes :
        if loc == ref :
          tp.set_reference( ids )
          points.append( tp )

    return points

  def get_raw_trackpoints( self ) :
    """
      Returns a list of all trackpoints with no position information
    """
    points = []
    for num, tp in self.__trackpoints :
      points.append( tp )

    return points


  def get_current_trackpoints( self ) :
    """
      Return the current hits/trackpoints stored in the object.
      Each item is formatted a tuple of ( PLANE_NUMBER, MC_HIT )
    """
    return copy.deepcopy( self.__trackpoints )


  def is_event_cut( self ) :
    """
      Returns the value of the cut-flag.
      Only a useful command if the is_cut(...) function has already been used.
    """
    return self.__cut_flag


  def get_current_number_trackpoints( self ) :
    """
      Return the current number of trackpoints/hits stored in the object.
    """
    return len( self.__trackpoints )


  def get_total_passed( self ) :
    """
      Return the running total of the number of events that pass all the cuts.
    """
    return self.__number_passed


  def get_total_cut( self ) :
    """
      Return the running totalof the number of events that failed to pass the cuts.
    """
    return self.__number_cut

  def get_total_number_trackpoints( self ) :
    """
      Return the total number of trackpoints analysed.
    """
    return self.__number_tracks

  def get_number_missing( self ) :
    """
      Return the number of events that failed to produce a hit at one of the reference planes.
    """
    return self.__missing_trackpoints

  def print_cut( self, cut_num ) :
    """
      Returns a string that contains all the cut information. Useful for 
      debugging and algorithm verification.
    """
    if cut_num > self.get_number_cuts() : return None
    c = self.__cuts[cut_num]
    string = "CUT: " + c.variable() + " " + c.operation().__name__ + " " + str( c.value() ) + "  # " + str( c.count() )
    return string

  def set_cut_missing( self, val ) :
    """
      Set the internal flag "cut_missing". This will cut the event if there point missing from ANY
      of the reference planes specified.
    """
    if val is not True and val is not False :
      raise ValueError( "cut_missing Must be a boolean value. Not " + str( val ) )

    self.__cut_missing = val

  def get_cut_missing( self ) :
    """
      Return the value of the internal cut_missing flag
    """
    return self.__cut_missing



####################################################################################################




class virtual_hits() :
  """
    Class that handles the extraction of virtual hit data from MC events.

    Cuts can be added and applied to the hits on extraction.

    Reference planes must be added. Cuts are only applied to the reference plane
    hits, allowing for faster processing using only the relevant hits.
  """
  def __init__( self, plane_positions = [] ) :
    """
      Initialise the class, set sums and counters to zero.
    """
    self.__number_tracks = 0
    self.__current_number_tracks = 0
    self.__missing_trackpoints = 0
    self.__current_missing = False
    self.__plane_positions = plane_positions

    self.__number_cut = 0
    self.__number_passed = 0
    self.__cut_flag = False

    self.__cuts = []
    self.__trackpoints = []
    self.__reference_planes = []


  def load_tracks( self, mc_event ) :
    """
      Load the mc hits from the mc_event into memory such that the reference
      plane hits can be found and cuts and be applied.
    """
    self.__current_number_tracks = 0
    self.__current_missing = False
    self.__cut_flag = False
    self.__trackpoints = []

    hits = mc_event.GetVirtualHits()
    for hit in hits :
      planenum = hit.GetStationId()
      loc = planenum
      self.__trackpoints.append( ( loc, track_point( virtual_track_point = hit, reference = planenum ) ) )

    self.__number_tracks += len( hits )
    self.__current_number_tracks = len( hits )


  def is_cut( self ) :
    """
      Checks to see if any of the reference plane track points violate any
      of the cuts stored in the class.

      If they do, return True, else return False.
    """
    self.__cut_flag = False
    references = self.get_reference_trackpoints()

    if len( references ) < len( self.__reference_planes ) or self.__current_missing is True :
      self.__missing_trackpoints += 1
      self.__number_cut += 1
      return True

    for tp in references :
      for c in self.__cuts :
        if c.cuts( tp.get( c.variable() ) ) :
          self.__cut_flag = True

    if self.__cut_flag is True :
      self.__number_cut += 1
      return True
    else :
      self.__number_passed += 1
      return False


  def add_cut( self, variable_name, value, operation ) :
    """
      Add a cut to the object to apply.
    """
    if not variable_name in track_point.get_variable_list() :
      raise ValueError( str( variable_name ) + ' is not a valid hit variable.' )
    self.__cuts.append( cut( variable_name, value, operation ) )


  def get_number_cuts( self ) :
    """
      Return the number of cuts in the object.
    """
    return len( self.__cuts )


  def get_reference_planes( self ) :
    """
      Return a copy of the list of reference planes being used
    """
    return copy.deepcopy( self.__reference_planes )


  def add_reference_plane( self, plane_num, reference_string ) :
    """
      Add a refence plane to the object to consider.
    """
    self.__reference_planes.append( ( reference_string, plane_num ) )


  def get_reference_trackpoints( self ) :
    """
      Returns a list of trackpoints that are found on the reference planes
      specified
    """
    points = []
    for num, tp in self.__trackpoints :
      for ids, ref in self.__reference_planes :
        if num == ref :
          tp.set_reference( ids )
          points.append( tp )

    return points

  def get_raw_trackpoints( self ) :
    """
      Returns a list of all trackpoints with no position information
    """
    points = []
    for num, tp in self.__trackpoints :
      points.append( tp )

    return points


  def get_current_trackpoints( self ) :
    """
      Return the current hits/trackpoints stored in the object.
      Each item is formatted a tuple of ( PLANE_NUMBER, MC_HIT )
    """
    return copy.deepcopy( self.__trackpoints )


  def is_event_cut( self ) :
    """
      Returns the value of the cut-flag.
      Only a useful command if the is_cut(...) function has already been used.
    """
    return self.__cut_flag


  def get_current_number_trackpoints( self ) :
    """
      Return the current number of trackpoints/hits stored in the object.
    """
    return len( self.__trackpoints )


  def get_total_passed( self ) :
    """
      Return the running total of the number of events that pass all the cuts.
    """
    return self.__number_passed


  def get_total_cut( self ) :
    """
      Return the running totalof the number of events that failed to pass the cuts.
    """
    return self.__number_cut

  def get_total_number_trackpoints( self ) :
    """
      Return the total number of trackpoints analysed.
    """
    return self.__number_tracks

  def get_number_missing( self ) :
    """
      Return the number of events that failed to produce a hit at one of the reference planes.
    """
    return self.__missing_trackpoints

  def print_cut( self, cut_num ) :
    """
      Returns a string that contains all the cut information. Useful for 
      debugging and algorithm verification.
    """
    if cut_num > self.get_number_cuts() : return None
    c = self.__cuts[cut_num]
    string = "CUT: " + c.variable() + " " + c.operation().__name__ + " " + str( c.value() ) + "  # " + str( c.count() )
    return string




####################################################################################################




class scifi_tracks() :
  """
    Class that handles the extraction of reconstructed tracks and/or
    trackpoints.

    Requires the user to add cuts and reference planes to be fully 
    functional.

    Only trackpoints recorded on the reference plane are subjected 
    to the cuts.
  """

  def __init__( self ) :
    """
      Initialise the class. Zero all sums and counters.
    """
    self.__number_tracks = 0
    self.__current_number_tracks = 0
    self.__number_upstream = 0
    self.__number_downstream = 0
    self.__current_number_upstream = 0
    self.__current_number_downstream = 0
    self.__straight_tracks = 0
    self.__allow_straight_tracks = False
    self.__cut_missing = True

    self.__number_cut = 0
    self.__number_passed = 0
    self.__cut_flag = False
    self.__missing_trackpoints = 0
    self.__current_missing = False
    self.__min_points = 0
    self.__insufficient_trackpoints = 0

    self.__reference_planes = []

    self.__tracks = []
    self.__trackpoints = []
    self.__cuts = []


  def load_tracks( self, scifi_event ) :
    """
      Loads all the scifitracks from the scifi_object into the class memory.

      Note: Unlike previous version of the code, it is possible to have more
      than 2 tracks available, if 2 and only 2 produce good, reconstructions
      at the reference planes. This introduces a negligible difference between
      this and older analyses.
    """
    self.__current_number_upstream = 0
    self.__current_number_downstream = 0
    self.__current_number_tracks = 0
    self.__current_missing = False
    self.__cut_flag = False
    self.__tracks = []
    self.__trackpoints = []

    tracks = scifi_event.scifitracks()
    for i in range( len( tracks ) ) :
      if tracks[i].GetAlgorithmUsed() == 0 :
        self.__straight_tracks += 1
        if self.__allow_straight_tracks != True :
          continue
      if tracks[i].scifitrackpoints().size() < self.__min_points :
        self.__insufficient_trackpoints += 1
        continue

      pid = 0
      if tracks[i].charge() > 0 :
        pid = -13
      else :
        pid = 13

      tracker = tracks[i].tracker()
      if tracker == 0 :
        self.__current_number_upstream += 1
      elif tracker == 1 :
        self.__current_number_downstream += 1

      self.__tracks.append( tracks[i] )

      for point in tracks[i].scifitrackpoints() :
        station = point.station()
        plane = point.plane()
        self.__trackpoints.append( ( ( tracker, station, plane ),\
                              track_point( scifi_track_point = point, p_value = tracks[i].P_value(), pid = pid,
                                            reference=str(tracker)+"."+str(station)+"."+str(plane) ) ) )


    self.__number_tracks += len( tracks )
    self.__current_number_tracks = len( tracks )
    self.__number_upstream += self.__current_number_upstream
    self.__number_downstream += self.__current_number_downstream


  def is_cut( self ) :
    """
      Check to see if any of the trackpoints violate any of the cuts 
      on the reference planes
    """
    self.__cut_flag = False
    references = self.get_reference_trackpoints()

    if self.__cut_missing and ( len( references ) != len( self.__reference_planes ) or self.__current_missing is True ) :
      self.__missing_trackpoints += 1
      self.__number_cut += 1
      return True
    for tp in references :
      for i, c in enumerate( self.__cuts ) :
        if c.cuts( tp.get( c.variable() ) ) :
          self.__cut_flag = True

    if self.__cut_flag is True :
      self.__number_cut += 1
      return True
    else :
      self.__number_passed += 1
      return False



  def add_cut( self, variable_name, value, operation ) :
     """
       Adds a cut that could be applied to the scifi track extractor.

       See the python operator module for more details on operations
     """
     if not variable_name in track_point.get_variable_list() :
      # ERROR!
       return
     self.__cuts.append( cut( variable_name, value, operation ) )


  def get_cut_count( self, cut_num ) :
    """
      Assumuing the user knows what order they stored the cuts, they can
      monitor the current number of cut particles using this command.
    """
    if cut_num > self.get_number_cuts() : return None
    c = self.__cuts[cut_num]
    return c.count()


  def print_cut( self, cut_num ) :
    """
      Returns a string that contains all the cut information. Useful for 
      debugging and algorithm verification.
    """
    if cut_num > self.get_number_cuts() : return None
    c = self.__cuts[cut_num]
    string = "CUT: " + c.variable() + " " + c.operation().__name__ + " " + str( c.value() ) + "  # " + str( c.count() )
    return string


  def get_number_cuts( self ) :
    """
      Returns the total number of cuts stored in the class
    """
    return len( self.__cuts )

  def get_total_cut( self ) :
    """
      Return a running total of the number of events that violated a cut
    """
    return self.__number_cut


  def get_reference_planes( self ) :
    """
      Return a copy of the list of the reference planes stored in the object.
    """
    return copy.deepcopy( self.__reference_planes )


  def add_reference_plane( self, tracker, station, plane, reference_string ) :
    """
      Add a reference plane to the object. Planes in the tracker are deduced from 
      the tracker, station and plane numbers. The reference string allows for a 
      method of identifying which reference plane is which.
      e.g. "upstream" or "downstream" are good suggestions
    """
    self.__reference_planes.append( ( reference_string, ( tracker, station, plane ) ) )


  def set_min_num_trackpoints( self, num ) :
    """
      Set the minimum number of trackpoints required for an analysable track.
    """
    self.__min_points = num


  def get_min_num_trackpoints( self ) :
    """
      Get the minimum number of trackpoints required for an analysable track.
    """
    return self.__min_points


  def get_reference_trackpoints( self ) :
    """
      Returns a list of trackpoints that actually land on the reference
      planes.
    """
    points = []
    for loc, tp in self.__trackpoints :
      for ids, ref in self.__reference_planes :
        if loc == ref :
          tp.set_reference( ids )
          points.append( tp )

    return points


  def get_current_trackpoints( self ) :
    """
      Return the current list of trackpoints.
      Each item is formatted as a tuple of the location and the hit data.

      ( ( TRACKER_NUM, STATION_NUM, PLANE_NUM ), RECON_HIT )
    """
    return copy.deepcopy( self.__trackpoints )


  def get_raw_trackpoints( self ) :
    """
      Return a list of trackpoints with no position information
    """
    points = []
    for loc, tp in self.__trackpoints :
      points.append( tp )

    return points 


  def get_tracks( self ) :
    """
      Return a copy of the tracks that were found in the scifi event.
    """
    return self.__tracks


  def is_event_cut( self ) :
    """
      Return the value of the internal cut_flag.
      Only really useful after the is_cut(...) function is called.
    """
    return self.__cut_flag


  def get_current_number_tracks( self ) :
    """
      Return the current number of tracks stored in memory.
    """
    return self.__current_number_tracks


  def get_total_number_tracks( self ) :
    """
      Return the running total number of tracks that have been analysed.
    """
    return self.__number_tracks


  def get_total_passed( self ) :
    """
      Return the total number of events that have passed the cuts.
    """
    return self.__number_passed


  def get_total_straight_tracks( self ) :
    """
      Return the number of tracks that were reconstruced as straight tracks.
    """
    return self.__straight_tracks


  def get_number_missing( self ) :
    """
      Return the number of trackpoints that could not be found in reference planes.
    """
    return self.__missing_trackpoints


  def get_allow_straight_tracks( self ) :
    """
      Returns true if the class is allowing straight tracks to pass the cuts.
    """
    return self.__allow_straight_tracks


  def set_allow_straight_tracks( self, allow_straights ) :
    """
      Allows the user to allow or deny straight tracks in their analysis.
    """
    self.__allow_straight_tracks = allow_straights

  def set_cut_missing( self, val ) :
    """
      Set the internal flag "cut_missing". This will cut the event if there point missing from ANY
      of the reference planes specified.
    """
    if val is not True and val is not False :
      raise ValueError( "cut_missing Must be a boolean value. Not " + str( val ) )

    self.__cut_missing = val

  def get_cut_missing( self ) :
    """
      Return the value of the internal cut_missing flag
    """
    return self.__cut_missing




####################################################################################################




class scifi_patrec() :
  """
    Class that handles the extraction of pattern recognition tracks

    Requires the user to add cuts and reference planes to be fully 
    functional.

    Pattern recognition produce values for the reference plane ONLY!
  """

  def __init__( self ) :
    """
      Initialise the class. Zero all sums and counters.
    """
    self.__number_tracks = 0
    self.__current_number_tracks = 0
    self.__number_upstream = 0
    self.__number_downstream = 0
    self.__current_number_upstream = 0
    self.__current_number_downstream = 0
    self.__straight_tracks = 0
    self.__allow_straight_tracks = False

    self.__number_cut = 0
    self.__number_passed = 0
    self.__cut_flag = False
    self.__min_points = 0

    self.__tracks = []
    self.__trackpoints = []
    self.__cuts = []

    self.__c = 2.99792458
    self.__Bz = 0.4
    self.__account_Eloss = False

    self.__upstream_z = 0.0
    self.__downstream_z = 0.0


####################################################
# TODO : find a way around these explicit functions!
####################################################
  def set_upstream_z( self, z ) :
    self.__upstream_z = z 

  def set_downstream_z( self, z ) :
    self.__downstream_z = z 
####################################################


  def load_tracks( self, scifi_event ) :
    """
      Loads all the pattern recognition tracks from the scifi_object into
      the class memory.
    """
    self.__current_number_upstream = 0
    self.__current_number_downstream = 0
    self.__current_number_tracks = 0
    self.__cut_flag = False
    self.__tracks = []
    self.__trackpoints = []

    helical_tracks = scifi_event.helicalprtracks()
    for i in range( len( helical_tracks ) ) :
      pid = 0
      if helical_tracks[i].get_charge() > 0 :
        pid = -13
      else :
        pid = 13

      tracker = helical_tracks[i].get_tracker()
      if tracker == 0 :
        self.__current_number_upstream += 1
      elif tracker == 1 :
        self.__current_number_downstream += 1

      self.__tracks.append( helical_tracks[i] )

      r  = helical_tracks[i].get_R()
      pt = math.fabs(helical_tracks[i].get_charge()*self.__c*self.__Bz*r)
      dsdz = math.fabs(helical_tracks[i].get_dsdz())
      x0 = helical_tracks[i].get_circle_x0()
      y0 = helical_tracks[i].get_circle_y0()
      s = helical_tracks[i].get_line_sz_c()
      phi_0 = s / r
      phi = phi_0 + math.pi / 2.0
      patrec_momentum = [ pt*math.cos(phi), pt*math.sin(phi), math.fabs(pt/dsdz) ]
      if self.__account_Eloss :
        P = np.linalg.norm( patrec_momentum )
        patrec_bias = 0.0
        if tracker == 0 :
          patrec_bias = (P - 1.4) / P
        else :
          patrec_bias = (P + 1.4) / P
        for i in range( len( patrec_momentum ) ) : patrec_momentum[i] *= patrec_bias

      x = x0 + r*math.cos(phi_0)
      y = y0 + r*math.sin(phi_0)
      z = 0.0
      px = patrec_momentum[0]
      py = patrec_momentum[1]
      pz = patrec_momentum[2]
      if tracker == 0 :
        x = -x
        px = -px
        z = self.__upstream_z
      else :
        z = self.__downstream_z

      self.__trackpoints.append( ( tracker,\
                    track_point( x=x, y=y, z=z, px=px, py=py, pz=pz, reference="helical."+str(tracker) ) ) )

    self.__number_tracks += len( helical_tracks )
    self.__current_number_tracks = len( helical_tracks )
    self.__number_upstream += self.__current_number_upstream
    self.__number_downstream += self.__current_number_downstream

    if self.__allow_straight_tracks :
      straight_tracks = scifi_event.straightprtracks()
      for i in range( len( straight_tracks ) ) :
        pid = 0

        tracker = straight_tracks[i].get_tracker()
        if tracker == 0 :
          self.__current_number_upstream += 1
        elif tracker == 1 :
          self.__current_number_downstream += 1

        self.__tracks.append( straight_tracks[i] )

        x = straight_tracks[i].get_x0()
        y = straight_tracks[i].get_y0()
        z = 0.0
        mx = straight_tracks[i].get_mx()
        my = straight_tracks[i].get_my()
        px = mx*200.0
        py = my*200.0
        pz = math.sqrt( 200.0**2 - px**2 + py**2 )

        self.__trackpoints.append( ( tracker,\
                    track_point( x=x, y=y, z=z, px=px, py=py, pz=pz, reference="straight."+str(tracker) ) ) )

      self.__number_tracks += len( straight_tracks )
      self.__current_number_tracks = len( straight_tracks )
      self.__number_upstream += self.__current_number_upstream
      self.__number_downstream += self.__current_number_downstream


  def is_cut( self ) :
    """
      Check to see if any of the trackpoints violate any of the cuts 
      on the reference planes
    """
    self.__cut_flag = False
    references = self.get_reference_trackpoints()

    if self.__cut_missing and ( len( references ) != len( self.__reference_planes ) or self.__current_missing is True ) :
      self.__missing_trackpoints += 1
      self.__number_cut += 1
      return True
    for tp in references :
      for i, c in enumerate( self.__cuts ) :
        if c.cuts( tp.get( c.variable() ) ) :
          self.__cut_flag = True

    if self.__cut_flag is True :
      self.__number_cut += 1
      return True
    else :
      self.__number_passed += 1
      return False



  def add_cut( self, variable_name, value, operation ) :
     """
       Adds a cut that could be applied to the scifi track extractor.

       See the python operator module for more details on operations
     """
     if not variable_name in track_point.get_variable_list() :
      # ERROR!
       return
     self.__cuts.append( cut( variable_name, value, operation ) )


  def get_cut_count( self, cut_num ) :
    """
      Assumuing the user knows what order they stored the cuts, they can
      monitor the current number of cut particles using this command.
    """
    if cut_num > self.get_number_cuts() : return None
    c = self.__cuts[cut_num]
    return c.count()


  def print_cut( self, cut_num ) :
    """
      Returns a string that contains all the cut information. Useful for 
      debugging and algorithm verification.
    """
    if cut_num > self.get_number_cuts() : return None
    c = self.__cuts[cut_num]
    string = "CUT: " + c.variable() + " " + c.operation().__name__ + " " + str( c.value() ) + "  # " + str( c.count() )
    return string


  def get_number_cuts( self ) :
    """
      Returns the total number of cuts stored in the class
    """
    return len( self.__cuts )


  def get_total_cut( self ) :
    """
      Return a running total of the number of events that violated a cut
    """
    return self.__number_cut


  def get_reference_trackpoints( self ) :
    """
      Returns a list of trackpoints that actually land on the reference
      planes.
    """
    return copy.deepcopy( self.__trackpoints )


  def get_current_trackpoints( self ) :
    """
      Return the current list of trackpoints.
      Each item is formatted as a tuple of the location and the hit data.

      ( ( TRACKER_NUM, STATION_NUM, PLANE_NUM ), RECON_HIT )
    """
    return copy.deepcopy( self.__trackpoints )


  def get_raw_trackpoints( self ) :
    """
      Return a list of trackpoints with no position information
    """
    points = []
    for loc, tp in self.__trackpoints :
      points.append( tp )

    return points 


  def get_tracks( self ) :
    """
      Return a copy of the tracks that were found in the scifi event.
    """
    return self.__tracks


  def is_event_cut( self ) :
    """
      Return the value of the internal cut_flag.
      Only really useful after the is_cut(...) function is called.
    """
    return self.__cut_flag


  def get_current_number_tracks( self ) :
    """
      Return the current number of tracks stored in memory.
    """
    return self.__current_number_tracks


  def get_total_number_tracks( self ) :
    """
      Return the running total number of tracks that have been analysed.
    """
    return self.__number_tracks


  def get_total_passed( self ) :
    """
      Return the total number of events that have passed the cuts.
    """
    return self.__number_passed


  def get_total_straight_tracks( self ) :
    """
      Return the number of tracks that were reconstruced as straight tracks.
    """
    return self.__straight_tracks


  def get_allow_straight_tracks( self ) :
    """
      Returns true if the class is allowing straight tracks to pass the cuts.
    """
    return self.__allow_straight_tracks


  def set_allow_straight_tracks( self, allow_straights ) :
    """
      Allows the user to allow or deny straight tracks in their analysis.
    """
    self.__allow_straight_tracks = allow_straights


