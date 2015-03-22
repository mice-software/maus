#! /bin/env python
#
# A temp fix for bug 1538.
#
# Use this function to calculate standard deviations!
#

import xboa
from xboa import Common as Common
from xboa import Bunch
from xboa import Hit

from xboa.Common import rg as rg

import sys

def standard_deviation_graph(bunches, x_axis_string, x_axis_list, y_axis_list, x_axis_units='', y_axis_units='', canvas='',\
    comparator=None, xmin=None, xmax=None, ymin=None, ymax=None, line_color=rg.line_color, line_style=rg.line_style,\
    line_width=rg.line_width, fill_color=rg.graph_fill_color, hist_title_string=''):
  """
  Prints a graph of Bunch.get(...) data over a set of bunches and returns the root canvas.

  Needs correct root installation.

  - bunches       = either a dict or a list of bunches
  - x_axis_string = string for call to Bunch.get() used to calculate x axis variables
  - x_axis_list   = list of variables for call to Bunch.get() used to calculate x axis variables
  - x_axis_units  = units for x axis
  - y_axis_string = string for call to Bunch.get() used to calculate y axis variables
  - y_axis_list   = list of variables for call to Bunch.get() used to calculate y axis variables
  - y_axis_units  = units for y axis
  - canvas        = if canvas=None, root_graph will create a new tcanvas and plot graphs on that
                      else will plot on the existing canvas, attempting to plot on existing axes
  - comparator    = comparator of bunch1, bunch2 - if none given, will sort by x-axis value
  - xmin          = float that overrides auto-detection of minimum x-axis value
  - xmax          = float that overrides auto-detection of maximum x-axis value
  - ymin          = float that overrides auto-detection of minimum y-axis value
  - ymax          = float that overrides auto-detection of maximum y-axis value
  - line_color    = int that sets the line colour of the histogram
  - line_style    = int that sets the line style of the histogram
  - line_width    = int that sets the line width of the histogram
  - fill_color    = int that sets the fill color of the histogram
  - hist_title_string = specify the string that will appear as a title on the canvas

  e.g. root_graph(bunch_dict, 'mean', ['x'], 'emittance', ['x','y'], 'cm', 'mm') will graph mean x vs emittance x y. 
  Returns a tuple of (canvas, histogram, graph) where the histogram contains the axes
  """
  x_points        = []
  y_points        = []
  list_of_bunches = []
  try:    list_of_bunches = bunches.values()
  except: list_of_bunches = bunches
  if comparator != None:
    list_of_bunches.sort(comparator)
  for i, bunch in enumerate(list_of_bunches):
    try:
        x_points.append(bunch.get(x_axis_string, x_axis_list)/Common.units[x_axis_units])
        y_points.append( (bunch.get("moment", [y_axis_list[0],y_axis_list[0]])**0.5)/Common.units[y_axis_units])
    except Exception:
        if len(x_points) > len(y_points):
            x_points.pop(-1)
        sys.excepthook(*sys.exc_info())
        print 'Failed to get data from bunch', i
  x_axis_string += "( "
  y_axis_string = "standard_deviation( "
  for value in x_axis_list: x_axis_string += value+" "
  for value in y_axis_list: y_axis_string += value+" "
  x_axis_string += ")"
  y_axis_string += ")"
  name = x_axis_string+":"+y_axis_string
  if not x_axis_units == '': x_axis_string += " ["+x_axis_units+"]"
  if not y_axis_units == '': y_axis_string += " ["+y_axis_units+"]"
  graph = Common.make_root_graph(name, x_points, x_axis_string, y_points, y_axis_string, comparator==None, xmin, xmax, ymin, ymax, 
                                 line_color, line_style, line_width, fill_color, hist_title_string)
  if canvas == '' or canvas == None:
    canvas = Common.make_root_canvas(name)
    graph[0].SetStats(False)
    graph[0].Draw()
  else:
    canvas.cd()
  graph[1].Draw('l')
  canvas.Update()
  return (canvas, graph[0], graph[1])

def standard_deviation_radius_graph(bunches, x_axis_units='', y_axis_units='', canvas='',\
    comparator=None, xmin=None, xmax=None, ymin=None, ymax=None, line_color=rg.line_color, line_style=rg.line_style,\
    line_width=rg.line_width, fill_color=rg.graph_fill_color, hist_title_string=''):
  """
  Prints a graph of Bunch.get(...) data over a set of bunches and returns the root canvas.

  Needs correct root installation.

  - bunches       = either a dict or a list of bunches
  - x_axis_string = string for call to Bunch.get() used to calculate x axis variables
  - x_axis_list   = list of variables for call to Bunch.get() used to calculate x axis variables
  - x_axis_units  = units for x axis
  - y_axis_string = string for call to Bunch.get() used to calculate y axis variables
  - y_axis_list   = list of variables for call to Bunch.get() used to calculate y axis variables
  - y_axis_units  = units for y axis
  - canvas        = if canvas=None, root_graph will create a new tcanvas and plot graphs on that
                      else will plot on the existing canvas, attempting to plot on existing axes
  - comparator    = comparator of bunch1, bunch2 - if none given, will sort by x-axis value
  - xmin          = float that overrides auto-detection of minimum x-axis value
  - xmax          = float that overrides auto-detection of maximum x-axis value
  - ymin          = float that overrides auto-detection of minimum y-axis value
  - ymax          = float that overrides auto-detection of maximum y-axis value
  - line_color    = int that sets the line colour of the histogram
  - line_style    = int that sets the line style of the histogram
  - line_width    = int that sets the line width of the histogram
  - fill_color    = int that sets the fill color of the histogram
  - hist_title_string = specify the string that will appear as a title on the canvas

  e.g. root_graph(bunch_dict, 'mean', ['x'], 'emittance', ['x','y'], 'cm', 'mm') will graph mean x vs emittance x y. 
  Returns a tuple of (canvas, histogram, graph) where the histogram contains the axes
  """
  x_points        = []
  y_points        = []
  list_of_bunches = []
  try:    list_of_bunches = bunches.values()
  except: list_of_bunches = bunches
  if comparator != None:
    list_of_bunches.sort(comparator)
  for i, bunch in enumerate(list_of_bunches):
    try:
        x_points.append( bunch.get('mean', ['z'])/Common.units[x_axis_units])
        y_points.append( (bunch.moment( ['r','r'], {'r':0.0})**0.5)/Common.units[y_axis_units])
    except Exception:
        if len(x_points) > len(y_points):
            x_points.pop(-1)
        sys.excepthook(*sys.exc_info())
        print 'Failed to get data from bunch', i
  x_axis_string = "mean( z )"
  y_axis_string = "standard_deviation( r )"
  name = x_axis_string+":"+y_axis_string
  if not x_axis_units == '': x_axis_string += " ["+x_axis_units+"]"
  if not y_axis_units == '': y_axis_string += " ["+y_axis_units+"]"
  graph = Common.make_root_graph(name, x_points, x_axis_string, y_points, y_axis_string, comparator==None, xmin, xmax, ymin, ymax, 
                                 line_color, line_style, line_width, fill_color, hist_title_string)
  if canvas == '' or canvas == None:
    canvas = Common.make_root_canvas(name)
    graph[0].SetStats(False)
    graph[0].Draw()
  else:
    canvas.cd()
  graph[1].Draw('l')
  canvas.Update()
  return (canvas, graph[0], graph[1])



