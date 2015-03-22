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


import MAUS
import ROOT

import copy
import math
import gc


class recon_container( ) :

  histogram_list = [ "residual", "mc", "recon", "phase_space" ]
  component_list = [ "x", "y", "z", "r", "px", "py", "pz", "pt", "p", "mx", "my" ]
  phase_space_list = [ ( "x", "px" ), ( "y", "py" ), ( "t", "energy" ), ( "z", "pz" ), ( "x", "y" ) ]
  component_range_dict = { "x" : ( -200.0, 200.0 ), "y" : ( -200.0, 200.0 ), "z" : ( -10000.0, 10000.0 ), "r" : ( 0.0, 200.0 ), "t" : ( -1.0e-6, +1.0e-6 ),\
              "px" : ( -200.0, 200.0 ), "py" : ( -200.0, 200.0 ), "pz" : ( 100.0, 300.0 ), "pt" : ( 0.0, 200.0 ),\
               "p" : ( 100.0, 300.0 ), "energy" : ( 100.0, 300.0 ), "mx" : ( 0.0, 2.0 ), "my" : ( 0.0, 2.0 ) }
  component_num_bins = 5000

  def __init__( self, ref=None ) :

    self.__has_mc = False
    self.__has_recon = False
    self.__plot_hists = False
    self.__plot_residuals = False
    self.__plot_phase_space = False
    self.__store_hits = True

    self.__virtual_plane = None
    self.__measurement_plane = None
    self.__particle_count = 0
    self.__plots = {}
    self.__is_init = False

    self.__recon_hits = []
    self.__mc_hits = []

    self.__ref = str( "" )
    if ref is not None :
      self.__ref = str(ref)+"_"

  def init( self, virtual_plane_number=None, measurement_plane=None, plot_hists=True, plot_residuals=True, plot_phase_space=True, store_hits=True ) :
    if plot_hists :
      self.__plot_hists = True

    if plot_phase_space :
      self.__plot_phase_space = True

    if virtual_plane_number is None :
      self.__has_mc = False
    else :
      self.__has_mc = True
      self.__virtual_plane = virtual_plane_number

    if measurement_plane is None :
      self.__has_recon is False
    else :
      self.__has_recon = True
      self.__measurement_plane = measurement_plane

    if plot_residuals is True :
      if self.__has_mc and self.__has_recon :
        self.__plot_residuals = True
      else :
        raise ValueError( "Must provide a measurement plane and a virtual plane to plot residuals" )

    self._init_plots()


  def add_hit( self, recon_hit=None, mc_hit=None ) :
    if self.__has_mc and mc_hit is None :
      raise ValueError( "No MC Hit provided" )
    if self.__has_recon and recon_hit is None :
      raise ValueError( "No Recon Hit provided" )

    if self.__has_mc :
      if self.__store_hits :
        self.__mc_hits.append( mc_hit )

      if self.__plot_hists :
        for component in self.component_list :
          self.__plots[ "mc_"+component ].Fill( mc_hit.get( component ) )

      if self.__plot_phase_space :
        for pair1, pair2 in self.phase_space_list :
          self.__plots[ "mc_phase_space_" + pair1 + "_" + pair2 ].Fill( mc_hit.get( pair1 ), mc_hit.get( pair2 ) )

    if self.__has_recon :
      if self.__store_hits :
        self.__recon_hits.append( recon_hit )

      if self.__plot_hists :
        for component in self.component_list :
          self.__plots[ "recon_"+component ].Fill( recon_hit.get( component ) )

      if self.__plot_phase_space :
        for pair1, pair2 in self.phase_space_list :
          self.__plots[ "recon_phase_space_" + pair1 + "_" + pair2 ].Fill( recon_hit.get( pair1 ), recon_hit.get( pair2 ) )

    if self.__plot_residuals :
      for component in self.component_list :
        self.__plots[ "residual_" + component ].Fill( recon_hit.get( component ) - mc_hit.get( component ) )


  def _init_plots( self ) :
    if self.__has_mc :
      if self.__plot_phase_space :
        for pair1, pair2 in self.phase_space_list :
          name = self.__ref + "mc_phase_space_" + pair1 + "_" + pair2 + "_" + str( self.__virtual_plane )
          range1 = self.component_range_dict[ pair1 ]
          range2 = self.component_range_dict[ pair2 ]
          self.__plots[ "mc_phase_space_"+pair1+"_"+pair2 ] = ROOT.TH2F( name, "MC Phase Space." + name + ". Plane " + str( self.__virtual_plane ), 500, range1[0], range1[1], 500, range2[0], range2[1] )

      if self.__plot_hists :
        for component in self.component_list :
          name = self.__ref + "mc_"+component+"_"+str( self.__virtual_plane )
          range1 = self.component_range_dict[ component ]
          self.__plots[ "mc_"+component ] = ROOT.TH1F( name, "MC "+component+" histogram", 1000, range1[0], range1[1] )

    if self.__has_recon :
      if self.__plot_phase_space :
        for pair1, pair2 in self.phase_space_list :
          name = self.__ref + "recon_phase_space_" + pair1 + "_" + pair2 + "_" + str( self.__measurement_plane )
          range1 = self.component_range_dict[ pair1 ]
          range2 = self.component_range_dict[ pair2 ]
          self.__plots[ "recon_phase_space_"+pair1+"_"+pair2 ] = ROOT.TH2F( name, "Recon Phase Space." + name + ". Plane " + str( self.__measurement_plane ), 500, range1[0], range1[1], 500, range2[0], range2[1] )

      if self.__plot_hists :
        for component in self.component_list :
          name = self.__ref + "recon_"+component+"_"+str( self.__measurement_plane )
          range1 = self.component_range_dict[ component ]
          self.__plots[ "recon_"+component ] = ROOT.TH1F( name, "Recon "+component+" histogram", 1000, range1[0], range1[1] )

    if self.__plot_residuals :
      for component in self.component_list :
        name = self.__ref + "residual_"+component+"_"+str( self.__virtual_plane )
        range1 = self.component_range_dict[ component ]
        range2 = ( range1[1] - range1[0] ) / 10.0
        self.__plots[ "residual_" + component ] = ROOT.TH1F( name, "Residual "+component, 1000, -range2, range2 )



  def get_measurement_plane( self ) :
    return self.__measurement_plane


  def get_virtual_plane( self ) :
    return self.__virtual_plane

  def save_histograms( self, root_file_name, leave_open=False ) :
    if not self.__plot_hists :
      raise ValueError( "No histograms to plot" )

    outfile = ROOT.TFile.Open( root_file_name, "RECREATE" )

    if self.__has_recon :
      for component in self.component_list :
        recon_name = "recon_"+component
        self.__plots[ recon_name ].Write()

    if self.__has_mc :
      for component in self.component_list :
        mc_name = "mc_"+component
        self.__plots[ mc_name ].Write()

    if not leave_open :
      outfile.Close()


  def save_phase_spaces( self, root_file_name, leave_open=False ) :
    if not self.__plot_phase_space :
      raise ValueError( "No phase spaces to plot" )

    outfile = ROOT.TFile.Open( root_file_name, "RECREATE" )

    if self.__has_recon :
      for pair1, pair2 in self.phase_space_list :
        name = "recon_phase_space_" + pair1 + "_" + pair2
        self.__plots[ name ].Write()

    if self.__has_mc :
      for pair1, pair2 in self.phase_space_list :
        name = "mc_phase_space_" + pair1 + "_" + pair2
        self.__plots[ name ].Write()

    if not leave_open :
      outfile.Close()


  def save_residuals( self, root_file_name, leave_open=False ) :
    if not self.__plot_residuals :
      raise ValueError( "No residuals to plot" )

    outfile = ROOT.TFile.Open( root_file_name, "RECREATE" )

    for component in self.component_list :
      name = "residual_"+component
      self.__plots[ name ].Write()

    if not leave_open :
      outfile.Close()

  def get_residual_plot( self, component ) :
    if component not in self.component_list :
      raise ValueError( "Could not find residuals for the component: "+str(component) )
    name = "residual_"+str(component)
    return self.__plots[ name ]

