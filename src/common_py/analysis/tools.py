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

# pylint: disable = W0311, E1101, W0613, C0111, R0911, W0621, C0103, R0902
# pylint: disable = W0102, W0612

import ROOT
import json
import os
import array
import types
import itertools
import sys
import linecache

STRAIGHT_ALGORITHM_ID = 0
HELICAL_ALGORITHM_ID = 1
MUON_MASS = 105.6583715

################################################################################
## Misc
################################################################################

def print_exception() :
  exc_type, exc_obj, tb = sys.exc_info()
  f = tb.tb_frame
  lineno = tb.tb_lineno
  filename = f.f_code.co_filename
  linecache.checkcache(filename)
  line = linecache.getline(filename, lineno, f.f_globals)
  print 'EXCEPTION IN ({}, LINE {} "{}"): {}'.format(filename, \
                                                 lineno, line.strip(), exc_obj)

def load_virtual_plane_dict( filename ) :
  temp_dict = None
  with open(filename, 'r') as infile :
    temp_dict = json.load(infile)

  new_dict = {}
  for key in temp_dict :
    new_dict[int(key)] = temp_dict[key]

  return new_dict


################################################################################
## Random Analysis Tasks
################################################################################

def calculate_plane_id(tracker, station, plane) :
  """
    Helper function to calculate the scifi plane ids
  """
  return int((1 + plane + (station - 1) * 3) * \
                                             ( -1.0 if tracker == 0 else 1.0 ))


def fit_gaussian(hist, fit_min=0, fit_max=0) :
  """
    Fits a Gaussian to the histogram and returns a tuple of (mean, stddev)
    for the fitted gaussian.
  """
  fit_result = hist.Fit( "gaus", "Q0S", "", fit_min, fit_max )
  func = hist.GetFunction( "gaus" )
  if not func :
    return 0.0, 0.0, -1.0, 0.0
  mean = func.GetParameter(1)
  mean_err = func.GetParError(1)
  stddev = func.GetParameter(2)
  stddev_err = func.GetParError(2)
  return mean, mean_err, stddev, stddev_err


def gaussian_profile_x(hist, fit_min=0, fit_max=0) :
  """
    Makes a profile plot if the supplied 2D histogram, assuming each bin is 
    gaussian-distrubuted. Note ROOT does not do this!
  """
  name = hist.GetName()

  x_pos = array.array( 'd' )
  y_pos = array.array( 'd' )
  x_error = array.array( 'd' )
  y_error = array.array( 'd' )

  for i in range( hist.GetXaxis().GetNbins()+2 ) :
    projection = hist.ProjectionY( \
                   name+'_pro_'+str(i), i, i )
    if projection.GetEntries() == 0 :
      continue

    pro_mean, pro_mean_err, pro_std, pro_std_err = \
                                   fit_gaussian( projection, fit_min, fit_max )

    x_pos.append( hist.GetXaxis().GetBinCenter( i ) )
    y_pos.append( pro_mean )
    x_error.append( 0.5*hist.GetXaxis().GetBinWidth( i ) )
    y_error.append( pro_mean_err )

  if len( x_pos ) < 1 :
    return None 
  else :
    profile = ROOT.TGraphErrors( len( x_pos ), x_pos, y_pos, x_error, y_error )
    profile.SetName( name+'_profile' )
    return profile


def gaussian_profile_y(hist, fit_min=0, fit_max=0) :
  """
    Makes a profile plot if the supplied 2D histogram, assuming each bin is 
    gaussian-distrubuted. Note ROOT does not do this!
  """
  name = hist.GetName()

  x_pos = array.array( 'd' )
  y_pos = array.array( 'd' )
  x_error = array.array( 'd' )
  y_error = array.array( 'd' )

  for i in range( hist.GetYaxis().GetNbins()+2 ) :
    projection = hist.ProjectionX( \
                   name+'_pro_'+str(i), i, i )
    if projection.GetEntries() == 0 :
      continue

    pro_mean, pro_mean_err, pro_std, pro_std_err = \
                                   fit_gaussian( projection, fit_min, fit_max )

    x_pos.append( pro_mean )
    y_pos.append( hist.GetYaxis().GetBinCenter( i ) )
    x_error.append( pro_mean_err )
    y_error.append( 0.5*hist.GetYaxis().GetBinWidth( i ) )

  if len( x_pos ) < 1 :
    return None 
  else :
    profile = ROOT.TGraphErrors( len( x_pos ), x_pos, y_pos, x_error, y_error )
    profile.SetName( name+'_profile' )
    return profile


def sort_arrays(array_lists, key_list=0, descending=True) :
  lists = itertools.izip(*sorted(itertools.izip(*array_lists), \
                                reverse=descending, key=lambda x: x[key_list]))
  arrays = [ array.array('d', lis) for lis in lists ]
  return arrays


##############################################################################
## Plotting and Saving Functionality
##############################################################################

def save_plots(plot_dict, filename) :
  """
    Save all the plots to file. Assumes a directory like strucutre of plots to
    recursively save them.
  """
  if not os.path.exists( os.path.dirname( filename ) ) :
    os.makedirs( os.path.dirname( filename ) )

  outfile = ROOT.TFile(filename, "RECREATE")

  for key in sorted(plot_dict) :
    if type( plot_dict[key] ) is types.DictType :
      directory = outfile.mkdir( key )
      directory.cd()
      save_plot( plot_dict[key], directory )
      outfile.cd()
    else :
      if plot_dict[key] is not None :
        plot_dict[key].Write(key)
      
  outfile.Close()


def save_plot(plot_dict, outfile) :
  """
    The recursive saving function for the plot dictionary
  """
  for key in sorted(plot_dict) :
    if type( plot_dict[key] ) is types.DictType :
      directory = outfile.mkdir( key )
      directory.cd()
      save_plot( plot_dict[key], directory )
      outfile.cd()
    else :
      if plot_dict[key] is not None :
        plot_dict[key].Write(key)


def print_plots(plot_dict, location, plot_options={}) :
  """
    Print the plots to PDF files rather than a root file. Assumes a recursive
    structure of directories and plots to save then to file
  """
  if not os.path.exists( location ) :
    os.makedirs( location )

  for key in sorted(plot_dict) :
    if type( plot_dict[key] ) is types.DictType :
      new_location = os.path.join( location, key )
      print_plots( plot_dict[key], new_location )
      
    else :
      canvas = ROOT.TCanvas( key+'_canvas' )
      plot_dict[key].Draw()
      if key in plot_options :
        apply_options( canvas, plot_dict[key], plot_options[key] )
      canvas.SaveAs( os.path.join(location, key ) + ".pdf", "pdf" )


def apply_options( canvas, hist, options ) :
  """
    Change some simple plot options using a dictionary of options.
  """
  for opt in options :
    if opt == "logy" :
      canvas.SetLogy()
    elif opt == "logz" :
      canvas.SetLogz()
    elif opt == "colz" :
      canvas.SetDrawOption( 'colz' )
      hist.Draw("colz")
      canvas.Update()
    else :
      print "ERROR : Unknown Canvas/Histogram Option:", opt

