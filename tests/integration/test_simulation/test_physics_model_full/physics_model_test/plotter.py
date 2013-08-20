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
import math

import xboa 
import xboa.Common as Common

from physics_model_test.geometry import Geometry # pylint: disable=W0611
from physics_model_test.all_tests import KSTest
from physics_model_test.all_tests import Chi2Test
PLOT_DIR = os.path.join('$MAUS_ROOT_DIR', 'tests', 'integration',
                        'plots', 'test_physics_model_full')
PLOT_DIR = os.path.expandvars(PLOT_DIR)
PLOT_FORMATS = ['png', 'root', 'eps'] #list of plot formats
LEGENDS = []

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
    
    sort_name = geo.name.split(' ')[1:] # strip out code name/version number
    #f3.write(steps+"\n")
    sort_name = [item+' ' for item in sort_name[:-1]]+[sort_name[-1]] # spaces
    sort_name = ''.join(sort_name)
    sort_name = sort_name.replace('/', '')
    return sort_name
    print sort_name

def get_material(geo):
    filename=histogram_title(geo)
    sort_name = geo.name.split(' ')[1:] # strip out code name/version number
    material=sort_name[2]
    return material
    

def get_thickness(geo):
    filename=histogram_title(geo)
    sort_name = geo.name.split(' ')[1:] # strip out code name/version number
    t=sort_name[0]
    return t

def get_mom(geo):
    filename=histogram_title(geo)
    sort_name = geo.name.split(' ')[1:] # strip out code name/version number
    mom=sort_name[5]
    return mom

def get_step(geo):

    """
    This will return step size
    """
    
    filename=histogram_title(geo)
    #f3 = open('steps.txt', 'a')
    sort_name = geo.name.split(' ')[1:] # strip out code name/version number
    steps=sort_name[8]
    #f3.write(steps+"\n")
    return steps


def file_name(geo, test):
    """
    Manipulate geo.name to generate a filename string
    """
    name = histogram_title(geo)
    name += ' '+test.variable
    return name.replace(' ', '_')
  
  

def code_name(geo):
    """
    Manipulate geo.name to extract a string like '<code_name> <version>'
    """
    name = geo.name.split(':')[0]
  #  return name[9]
   
    return name.replace('_', ' ')
   

def geometry_comparator(geometry_1, geometry_2):
    """
    Return comparator of the histogram_title of geometries (i.e. name excluding
    code and version)
    """
    return cmp(histogram_title(geometry_1), histogram_title(geometry_2))

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

def build_legend(canvas, tests, test_dict, histograms):
    """
    Build a legend for the canvas
    """
    legends = [code_name(test_dict[a_test]) for a_test in tests]
    
    for index, leg in enumerate(legends):
        #f = open('output.txt', 'a')
        rms = str("RMS %.4g" % histograms[index].GetRMS())
        mean = str("Mean %.6g" % histograms[index].GetMean())
        legends[index] = '#splitline{'+legends[index]+'}'+'{'+mean+' '+rms+'}'
        print 'mean',  histograms[index].GetMean()
        #f.write(mean+"\n")
    canvas.cd()
    leg_min =  0.89-0.08*len(histograms)
    if leg_min < 0.1:
        leg_min = 0.1
    leg = ROOT.TLegend(0.0, leg_min, 0.4, 0.90) # pylint: disable=E1101
    leg.SetEntrySeparation(0.6)
    for i, hist in enumerate(histograms):
        leg.AddEntry(hist, legends[i])
    leg.SetFillColor(10)
    leg.SetBorderSize(0)
    leg.Draw()
    canvas.Update()
    LEGENDS.append(leg)

def get_mean(tests, test_dict, histograms):
    legends = [code_name(test_dict[a_test]) for a_test in tests]
    
    for index, leg in enumerate(legends):
        f = open('mean.txt', 'a')
        mean = str(histograms[index].GetMean())
        #print 'mean',  histograms[index].GetMean()
        #f.write(mean+"\n")
        return mean

def get_rms(tests, test_dict, histograms):
    legends = [code_name(test_dict[a_test]) for a_test in tests]
    
    for index, leg in enumerate(legends):
        #f = open('mean.txt', 'a')
        rms = str(histograms[index].GetRMS())
        #print 'mean',  histograms[index].GetMean()
        #f.write(mean+"\n")
        return rms

def set_margins(canvas):
    """
    Make some clearance for the legend
    """
    canvas.SetLeftMargin(0.4)
    canvas.SetRightMargin(0.05)
    canvas.Update()
    

def build_title(canvas, name):
    """
    Build a title for the canvas
    """
    #f2 = open('output2.txt', 'a')
    canvas.cd()
    leg2 = ROOT.TLegend(0.1, 0.9, 0.9, 1.0) #pylint: disable = E1101
    leg2.SetHeader(histogram_title(name))
    #title=histogram_title(name)
    #f2.write(title+"\n")
    leg2.SetFillColor(10)
    leg2.SetBorderSize(0)
    leg2.Draw()
    canvas.Update()
    LEGENDS.append(leg2)


def plot_muscat_compare(file_list):
    """
    Loop for loading tests and geometry and making plots for comparison with muscat data
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
            
            
            (canv,h_list)=Chi2Test.make_plots(test_list,test_dict[test_list[0]])
            (canv,h_list)=Chi2Test.chi_plots(test_list,test_dict[test_list[0]])
            build_legend(canv, test_list, test_dict, h_list[1:])
            build_title(canv, test_dict[test_list[0]])
            code=code_name(test_dict[test_list[0]])
            title=histogram_title(test_dict[test_list[0]])
           
     
            fname = file_name(test_dict[test_list[0]], test_list[0])
   
            for form in PLOT_FORMATS:
                plot_path = os.path.join(PLOT_DIR, fname+'.'+form)
                canv.Print(plot_path)
            
    return 0

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
            (canv,h_list)= KSTest.make_plots(test_list)
            
          

            
            
            build_legend(canv, test_list, test_dict, h_list[1:])
            build_title(canv, test_dict[test_list[0]])
            code=code_name(test_dict[test_list[0]])
            title=histogram_title(test_dict[test_list[0]])
           
            set_margins(canv)
            
            fname = file_name(test_dict[test_list[0]], test_list[0])
   
            for form in PLOT_FORMATS:
                plot_path = os.path.join(PLOT_DIR, fname+'.'+form)
                canv.Print(plot_path)
            
    return 0









