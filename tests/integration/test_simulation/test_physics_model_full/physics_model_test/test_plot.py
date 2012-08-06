#This file is a part of g4mice
#
#g4mice is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#g4mice is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with g4mice in the doc folder.  If not, see 
#<http://www.gnu.org/licenses/>.
#

"""
Make plots from some set of tests.

We have some set of tests, done with many geometries and codes. To make
meaningful plots, we need to find the same tests that were made on the same
geometry in different codes (or versions of the same code) and plot them.

So the excercise here is to manipulate the geometry name to identify the code,
code version, and geometry; group tests belonging to the same tests
"""

import os
import ROOT

from physics_model_test.geometry import Geometry # pylint: disable=W0611
from physics_model_test.tests import KSTest

PLOT_DIR = os.path.join('$MAUS_ROOT_DIR', 'tests', 'integration',
                        'plots', 'test_physics_model_full')
PLOT_DIR = os.path.expandvars(PLOT_DIR)
PLOT_FORMATS = ['png', 'root', 'eps'] #list of plot formats

def load_geometries(file_list):
    """
    Load geometry files.
    Returns a tuple of (geometry_list, dict_of{geometry:code_version})
    """
    geometries = []
    file_paths = file_list
    for path in file_paths:
        print path
        new_geometries = eval( open(path).read() )
        geometries += new_geometries
    return geometries

def histogram_title(geo):
    """
    Manipulate geo.name to generate a histogram title string
    """
    return file_name(geo).replace('_', ' ')

def file_name(geo):
    """
    Manipulate geo.name to generate a filename string
    """
    print geo.name
    sort_name = geo.name.split(' ')[1:] # strip out code name/version number
    sort_name = [item+'_' for item in sort_name[:-1]]+[sort_name[-1]] # spaces
    sort_name = ''.join(sort_name)
    sort_name = sort_name.replace('/', '')
    return sort_name  

def code_name(geo):
    """
    Manipulate geo.name to extract a string like '<code_name> <version>'
    """
    name = geo.name.split(':')[0]
    return name.replace('_', ' ')

def geometry_comparator(geometry_1, geometry_2):
    """
    Return comparator of the file_name of geometries (i.e. name excluding code
    and version)
    """
    return cmp(file_name(geometry_1), file_name(geometry_2))

def group_geometries(geometry_list):
    """
    Group geometries into sub lists.
    Return value is list of lists, where list[i] contains a set of equal
    geometries (i.e. name is the same, bar the code).
    """
    geo_list_out = []
    geometry_list.sort(cmp=geometry_comparator)
    for i, geo in enumerate(geometry_list):
        if i == 0 or geometry_comparator(geometry_list[i-1], geo) != 0:
            geo_list_out.append([])
        geo_list_out[-1].append(geo)
    return geo_list_out

def test_comparator(test_1, test_2):
    """
    Return comparator of the test.variable
    """
    return cmp(test_1.variable, test_2.variable)

def group_tests(geometry_list):
    """
    Group all tests in the geometry list by variable
    Return value is a tuple of (list_of_list_of_tests, dict_of{test:geometry})
    """
    test_lists = []
    test_dict  = {}
    for test in geometry_list[0].tests:
        test_lists.append([test])
        test_dict[test] = geometry_list[0]
    for geo  in geometry_list[1:]:
        for test_list in test_lists:
            for test in geo.tests:
                if test_comparator(test, test_list[0]) == 0: 
                    test_list.append(test)
                    test_dict[test] = geo
    return (test_lists, test_dict)

def build_legend(canvas):
    """
    Build a legend for the canvas
    """
    canvas.cd()
    leg = canvas.BuildLegend() #histogram legend
    leg.SetX1(0.13)
    leg.SetX2(0.5)
    leg.SetY1(0.6)
    leg.SetFillColor(10)
    leg.SetBorderSize(0)
    canvas.Update()

def build_title(canvas, name):
    """
    Build a title for the canvas
    """
    canvas.cd()
    leg2 = ROOT.TLegend(0.1, 0.9, 0.9, 1.0) #pylint: disable = E1101
    leg2.SetHeader(histogram_title(name))
    leg2.SetFillColor(10)
    leg2.SetBorderSize(0)
    leg2.Draw()
    canvas.Update()

def plot(file_list):
    """
    Main loop - load geometry and tests, sort geometry by name, then sort 
    tests by type, then make plots.
    """
    try:
        os.mkdir(PLOT_DIR)
    except OSError:
        pass #may fail if directory already exists
    geometries = load_geometries(file_list)
    geo_groups = group_geometries(geometries)
    for geo_list in geo_groups:
        (test_group, test_dict) = group_tests(geo_list)
        for test_list in test_group:
            (canv, h_list) = KSTest.make_plots(test_list)
            for i, histogram in enumerate(h_list):
                if i > 0:
                    histogram.SetName(code_name(test_dict[test_list[i-1]]))
            build_legend(canv)
            build_title(canv, test_dict[test_list[0]])
            fname = file_name(test_dict[test_list[0]])
            for form in PLOT_FORMATS:
                plot_path = os.path.join(PLOT_DIR, fname+'.'+form)
                canv.Print(plot_path)
    return 0

