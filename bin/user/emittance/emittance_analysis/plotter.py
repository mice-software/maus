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

import math

from track_point import track_point


class plotter_base() :
  """
    Base class for the plotters
  """
  def __init__( self, identifier, plot_dict ) :
    pass

  def save_file( self, filename ) :
    pass



class plot_residuals( plotter_base ) :
  """
    Give it two track_points and this class will plot all the parameter
    residuals
  """
  def __init__( self, identifier ) :
    self.__plot_x = ROOT.TH1F( str(identifier) + "_residual_x", "Residual in X-Positions", 200, -10.0, 10.0 )
    self.__plot_y = ROOT.TH1F( str( identifier ) + "_residual_y", "Residual in Y-Positions", 200, -10.0, 10.0 )
    self.__plot_z = ROOT.TH1F( str( identifier ) + "_residual_z", "Residual in Z-Positions", 200, -10.0, 10.0 )
    self.__plot_px = ROOT.TH1F( str( identifier ) + "_residual_px", "Residual in X-Momentum", 500, -25.0, 25.0 )
    self.__plot_py = ROOT.TH1F( str( identifier ) + "_residual_py", "Residual in Y-Momentum", 500, -25.0, 25.0 )
    self.__plot_pt = ROOT.TH1F( str( identifier ) + "_residual_pt", "Residual in Transverse Momentum", 500, -50.0, 50.0 )
    self.__plot_pz = ROOT.TH1F( str( identifier ) + "_residual_pz", "Residual in Longitudinal Momentum", 500, -50.0, 50.0 )
    self.__plot_phi = ROOT.TH1F( str( identifier ) + "_residual_phi", "Residual in Phi-Positions", 200, -6.3, 6.3 )

#    self.__plot = ROOT.TH2F( "residual_pt_pz_up", "Residuals in Upstream Transverse Momentum Against the MC Longitudinal Momentum", 500, -50.0, 50.0, 500, 100.0, 300.0 )
#    self.__plot = ROOT.TH2F( "residual_pz_pz_up", "Residuals in Upstream Longitudinal Momentum Against the MC Longitudinal Momentum", 500, -50.0, 50.0, 500, 100.0, 300.0 )


  def fill( self, measured, actual ) :
    """
      Records the residuals in all the track_point parameters
    """
    self.__plot_x.Fill( measured.get_x() - actual.get_x() )
    self.__plot_y.Fill( measured.get_y() - actual.get_y() )
    self.__plot_z.Fill( measured.get_z() - actual.get_z() )
    self.__plot_px.Fill( measured.get_px() - actual.get_px() )
    self.__plot_py.Fill( measured.get_py() - actual.get_py() )
    self.__plot_pt.Fill( measured.get_pt() - actual.get_pt() )
    self.__plot_pz.Fill( measured.get_pz() - actual.get_pz() )
    self.__plot_phi.Fill( math.atan2( measured.get_y(), measured.get_x() ) -\
                          math.atan2( actual.get_y(), actual.get_x() ) )

  def save_file( self, filename ) :
    """
      Saves all the histograms to a root file called <filename>
    """
    outfile = ROOT.TFile( filename, "RECREATE" )

    self.__plot_x.Write()
    self.__plot_y.Write()
    self.__plot_z.Write()
    self.__plot_px.Write()
    self.__plot_py.Write()
    self.__plot_pt.Write()
    self.__plot_pz.Write()
    self.__plot_phi.Write()

    outfile.Close()


class plot_means( plotter_base ) :
  pass

