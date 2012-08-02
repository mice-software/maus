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

import code_comparison as cc
from code_comparison import geometry
from code_comparison import ks_test
import xboa.Common as Common
import sys
import os
import ROOT
import string

"""
Make plots from some set of tests.

We have some set of tests, done with many geometries and codes. To make meaningful plots, we need to
find the same tests that were made on the same geometry in different codes (or versions of the same code)
and plot them.
"""

g4bl   = 'g4bl'              #string id for g4bl
icool  = 'icool'             #string id for icool
g4mice = 'g4mice_simulation' #string id for g4mice_simulation
maus = 'maus'
codes  = (maus, )          #tuple of codes for which TestPlot.py will attempt to make plots

plot_dir         = 'plots'              #directory to put the plots
plot_formats     = ['png'] #list of plot formats

#dicts below give version, directory of output, filename of output (within the directory), label to put in plot legend
#filename and label gets a substitution of $version for the version number
versions    = {g4mice:('release-2-3-0', 'release-2-4-0','release-2-4-1'), g4bl:('2.02','2.06'),               icool:('10', '13', '17','20',), maus:('some_version', )}
filenames   = {g4mice:'g4mice.$version.dat',    g4bl:'g4bl_$version.test_data.dat', icool:'icool_3.$version.test_data.dat',                   maus:'maus.$version.ref_data.dat'}
dirs        = {g4mice:'.',                      g4bl:'g4bl_data',                   icool:'icool_data',                                       maus:'maus_data'}
labels      = {g4mice:'g4mice $version',        g4bl:'g4bl $version',               icool:'icool v3.$version',                                maus:'maus $version'}

def version_substitute(name, the_version):
  """
  Replace all instances of $version in name with the_version
    name        = some string with format definition
    the_version = some string containing a version number    
  """
  return string.Template(name).substitute(version=the_version)

def load_geometries():
  """
  Load geometry files.
  Returns a tuple of (geometry_list, dict_of{geometry:code_version})
  """
  geometries       = []
  geometry_version = {}
  for code in codes:
    print code
    for version in versions[code]:
      fname = version_substitute(filenames[code], version)
      path  = os.path.join(dirs[code],fname)
      new_geometries = eval( open(path).read() )
      print '  ',path
      for geo in new_geometries: 
        geometry_version[geo] = version_substitute(labels[code], version)
      geometries += new_geometries
  return (geometries, geometry_version)

def group_geometries(geometry_list, geometry_comparator=lambda x,y: cmp(x.name, y.name)):
  """
  Group geometries into sub lists.
  Return value is list of lists, where list[i] contains a set of equal geometries, while all geometries
  in list[i+1] are > geometries in list[i]. Geometry comparisons are done by geometry_comparator function
  (default is to sort by name).
  """
  geo_list_out = []
  geometry_list.sort(cmp=geometry_comparator)
  for i,geo in enumerate(geometry_list):
    if i==0 or geometry_comparator(geometry_list[i-1], geo) != 0:
      geo_list_out.append([])
    geo_list_out[-1].append(geo)
  return geo_list_out

def group_tests(geometry_list, test_comparator=lambda x,y: cmp(x.variable, y.variable)):
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

def main():
  """
  Main loop - load geometry and tests, sort geometry by name, then sort tests by type, then make plots.
  """
  try:    os.mkdir(plot_dir)
  except: pass #may fail if directory already exists
  (geometries, geometry_version) = load_geometries()
  geo_groups = group_geometries(geometries)
  for geo_list in geo_groups:
    (test_group, test_dict) = group_tests(geo_list)
    for test_list in test_group:
      geo_name = test_dict[test_list[0]].name
      (canv, h_start, h_end) = ks_test.make_plots(test_list, bin_function=ks_test.pdf_function)
      h_list = Common._hist_persistent[h_start:h_end]
      for i,h in enumerate(h_list):
        geo = test_dict[test_list[i]]
        h.SetName(str(geometry_version[geo])+' ('+str(test_list[i].test_result())+')' )
      leg = canv.BuildLegend() #histogram legend
      leg.SetX1(0.13)
      leg.SetX2(0.5)
      leg.SetY1(0.6)
      leg.SetFillColor(10)
      leg.SetBorderSize(0)
      leg2 = ROOT.TLegend(0.1,0.9,0.9,1.0) #title bar
      leg2.SetHeader(geo_name)
      leg2.SetFillColor(10)
      leg2.SetBorderSize(0)
      leg2.Draw()
      canv.Update()
      outname = geo_name+'_'+test_list[0].variable
      outname = outname.replace(' ','_')
      outname = outname.replace('/','')
      for form in plot_formats:
        plot_path = os.path.join(plot_dir,outname+'.'+form)
        canv.Print(plot_path)
  return 0

if __name__=='__main__':
  sys.exit(main())


