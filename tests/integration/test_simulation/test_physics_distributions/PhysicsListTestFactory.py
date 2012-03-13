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

import CodeComparison as cc
import os
import xboa.Common as Common
import operator
import sys
import exceptions

"""
PhysicsListTestFactory contains a list of data and few functions for generating
reference tests and geometry sets. In general, we don't use the test factory for
running tests, only for generating them in the first place.
"""

g4bl   = 'g4bl'
icool  = 'icool'
g4mice = 'g4mice_simulation'
codes  = (g4bl,icool,g4mice)

n_ks_divs = 500

tests = [
  cc.ks_test.new('px',     'MeV/c', map(lambda x: -50.+x,  range(n_ks_divs+1)),[0]*n_ks_divs,10000,1.0,10000,0.0,0.01),
  cc.ks_test.new('energy', 'MeV', map(lambda x: 202.+x/4., range(n_ks_divs+1)),[0]*n_ks_divs,10000,1.0,10000,0.0,0.01)
]

#list of configuration substitutions

mice_configurations = [ #thickness in mm, momentum in MeV/c
{'_material_':'lH2' , '_thickness_':350.,  '_momentum_':200., '_pid_':-13, '_step_':100., '_nev_':100000, '_seed_':1}, #used to be 100,000 events but xboa was slow/hanging on g4bl output
{'_material_':'lH2',  '_thickness_':350.,  '_momentum_':100., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'lH2',  '_thickness_':350.,  '_momentum_':400., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'lH2',  '_thickness_':350.,  '_momentum_':800., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':100., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':200., '_pid_':-13, '_step_':100., '_nev_':100000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':400., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':800., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'Al',   '_thickness_':0.5, '_momentum_':200., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'Be',   '_thickness_':0.5, '_momentum_':200., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'lHe',  '_thickness_':350.,  '_momentum_':200., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'STEEL304', '_thickness_':0.5, '_momentum_':200., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'Cu',   '_thickness_':5., '_momentum_':200., '_pid_':-13, '_step_':100., '_nev_':10000, '_seed_':1},
#ICOOL doesn't see thin regions if step size is large, so add small step size to check this
{'_material_':'Al',   '_thickness_':0.5, '_momentum_':200., '_pid_':-13, '_step_':0.1, '_nev_':10000, '_seed_':1},
{'_material_':'Be',   '_thickness_':0.5, '_momentum_':200., '_pid_':-13, '_step_':0.1, '_nev_':10000, '_seed_':1},
{'_material_':'STEEL304', '_thickness_':0.5, '_momentum_':200., '_pid_':-13, '_step_':0.1, '_nev_':10000, '_seed_':1},
{'_material_':'Cu',   '_thickness_':5., '_momentum_':200., '_pid_':-13, '_step_':1., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':100., '_pid_':211, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':200., '_pid_':211, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':800., '_pid_':211, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':0.1,'_momentum_':0.35, '_pid_':11, '_step_':0.1, '_nev_':10000, '_seed_':1}, #about 0.1 MeV kinetic energy electron
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':0.1, '_momentum_':1.,  '_pid_':11, '_step_':0.1, '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':1., '_momentum_':10.,   '_pid_':11, '_step_':0.1, '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':1., '_momentum_':100., '_pid_':11,  '_step_':0.1, '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE', '_thickness_':1., '_momentum_':200., '_pid_':11,  '_step_':0.1, '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':1., '_momentum_':800., '_pid_':11,  '_step_':0.1, '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10., '_momentum_':200., '_pid_':13,   '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10., '_momentum_':200., '_pid_':-11,  '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10., '_momentum_':200., '_pid_':-211, '_step_':100., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':400., '_pid_':2212, '_step_':100., '_nev_':10000, '_seed_':1},#about 80 MeV kinetic energy proton
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':600., '_pid_':2212, '_step_':100., '_nev_':10000, '_seed_':1},#about 180 MeV kinetic energy proton
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':1500., '_pid_':2212, '_step_':100., '_nev_':10000, '_seed_':1}, #about 800 MeV kinetic energy proton
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':8000., '_pid_':2212, '_step_':100., '_nev_':10000, '_seed_':1}, #about 8000 MeV kinetic energy proton
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':200., '_pid_':-13, '_step_':1., '_nev_':10000, '_seed_':1},
{'_material_':'LITHIUM_HYDRIDE',  '_thickness_':10.,  '_momentum_':200., '_pid_':-13, '_step_':10., '_nev_':10000, '_seed_':1},
]

pos_keys = ['_thickness_', '_step_'] #dimensions of position
mom_keys = ['_momentum_'] #dimensions of momentum
mat_keys = ['_material_'] #materials (different encoding between ICOOL and G4)
pid_keys = ['_pid_'] #pid (different encoding between ICOOL and G4)
no_keys  = ['_nev_','_seed_'] #not code dependent



def code_convert(config, code):
  """
  For making configuration geometry generic across different codes, we add
  functions to convert between, say, g4bl and g4mice for different data types.
  """
  for key,value in config.iteritems():
    if   key in pos_keys: config[key] = position(value,code)  
    elif key in mom_keys: config[key] = momentum(value,code)  
    elif key in mat_keys: config[key] = material(value,code)  
    elif key in pid_keys: config[key] = pid(value,code)  
    elif key in no_keys:  pass  
    else: raise(KeyError('Nothing known about key '+str(key)) )
  return config

_g4mice_material_to_icool = {
'lH2':'LH', 'LITHIUM_HYDRIDE':'LIH', 'Al':'AL', 'Be':'BE', 'lHe':'LHE', 'STEEL304':'FE','Cu':'CU'
}
_g4mice_material_to_g4bl = {
'lH2':'LH2', 'LITHIUM_HYDRIDE':'LITHIUM_HYDRIDE', 'Al':'Al', 'Be':'Be', 'lHe':'lHe', 'STEEL304':'Fe','Cu':'Cu'
}
def material(value, code):
  """Convert material names between different data formats"""
  if code==g4mice: return value
  if code==g4bl:   return _g4mice_material_to_g4bl[value] 
  if code==icool:  return _g4mice_material_to_icool[value]

def position(value, code):
  """Convert positions between different data formats (just a unit change)"""
  if code == g4bl or code==g4mice: return value
  if code == icool: return value/Common.units['m']

def momentum(value, code):
  """Convert momenta between different data formats (just a unit change)"""
  if code == g4bl or code==g4mice: return value
  if code == icool: return value/Common.units['GeV/c']

def pid(value, code):
  """Convert particle type (PID) between different data formats"""
  if code == g4bl or code==g4mice: return value
  if code == icool: return Common.pdg_pid_to_icool[value]

def make_name(config):
  """Make a human readable name out of the configuration"""
  dt  = str(config['_thickness_'])
  mat = str(config['_material_'])
  nev = str(config['_nev_'])
  pz  = str(config['_momentum_'])
  pid = Common.pdg_pid_to_name[config['_pid_']]
  dz  = str(config['_step_'])
  return dt+' mm '+mat+' with '+nev+' '+pz+' MeV/c '+pid+' '+dz+' mm steps'

def bunch_read(code):
  """Define parameters for bunch IO"""
  bunch_read = {
    g4mice:('g4mice_virtual_hit', 'Sim.out.gz'),
    icool: ('icool_for009', 'for009.dat'),
    g4bl: ('icool_for009', 'for009_g4bl.txt'),
  }
  return bunch_read[code]

def bunch_index(code):
  """Define station number for bunch IO"""
  bunch_index = {g4mice:2,icool:5, g4bl:2}
  return bunch_index[code]

def code_parameters(code):
  """Set code-specific parameters - executable, material type, etc"""
  code_parameters = {
    g4mice:(os.path.join('$MICESRC','Applications','Simulation','Simulation'),['cards'],{'cards.in':'cards','MaterialBlock.in':'MaterialBlock.dat'}),
    icool: (os.path.join('$ICOOL'),[],{'for001.in':'for001.dat'}),
    g4bl: (os.path.join('$G4BL'),['g4bl_deck'],{'g4bl_deck.in':'g4bl_deck'}),
  }
  return code_parameters[code]

def run_batch(code, geometry):
  """Run a batch job"""
  geometry.run_mc()

def build_geometry(config, tests, code, output_file):
  """Build a geometry and ks_tests"""
  if not code in codes: raise(NotImplementedError('Nothing known about code '+str(code)))
  geo = cc.geometry()
  #first setup the geometry basics
  pid      = config['_pid_']
  geo.name = make_name(config)
  geo.substitutions = code_convert(config, code)
  geo.bunch_index   = bunch_index(code)
  geo.bunch_read    = bunch_read (code)
  geo.tests         = tests
  geo.code_model    = code_parameters(code)
  #now run a batch job to get axis range right on tests
  geo.run_mc()
  bunch = geo.read_bunch()
  #adjust the geometry
  for i,test in enumerate(geo.tests):
    test.pid = pid
    bunch.conditional_remove({'pid':pid}, operator.ne)
    [xmin, xmax] = Common.min_max(bunch.list_get_hit_variable([test.variable])[0])
    xmin *= Common.units[test.units]
    xmax *= Common.units[test.units]
    test.bins = map(lambda x: xmin+(xmax-xmin)*x/float(n_ks_divs), range(n_ks_divs+1))
    geo.tests[i]  = test.run_test(bunch)
  print geo.name
  sys.stdout.flush()
  return geo

def build_reference_data(code):
  """Build physics list tests"""
  fout = open(code+'_ref_data.dat', 'w')
  print >>fout,'[\n'
  for config in mice_configurations: 
    try:
      geo = build_geometry(config, tests, code, fout)
      print >>fout,repr(geo),','
    except:
      print 'Caught exception in configuration ',config,'code',code
      sys.excepthook(*sys.exc_info())
      if sys.exc_info()[0] == exceptions.KeyboardInterrupt: raise exceptions.KeyboardInterrupt
  print >>fout,'\n]\n'
  fout.close()

if __name__=='__main__':
  for arg in sys.argv:
    print arg
    if arg in codes:
      build_reference_data(arg)
  sys.exit(0)

