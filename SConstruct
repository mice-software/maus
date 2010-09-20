#-*-python-*-
# Main build script for RAT.

# Do not copy this to make your own customized RAT applications!
# Instead see user/SConstruct for a simpler example.

#### Add new source module names here ####

modules = Split('''
cmd core daq db fit gen geo io physics stlplus util
''') # ds handled separately


############### You should not need to edit below this line ############

import os
import glob
#from buildhelp import RATENVFILE, src_module, build_list

# Get build environment for making RAT applications
env = Environment()#SConscript() #envfile
"""
# Collect list of object files from source directories and prepare
# headers for copying.

# src_module() returns list for each directory.
ratobj = []
for m in modules:
    ratobj += src_module(env, m)
ratobj += src_module(env, 'ds', header_subdir='DS')


#### Target: Copy RAT header files to $RATROOT/include
# Define an alias so one can just type "scons headers" to only copy headers.
copy_headers = env.Alias('headers', env['RATHEADERS'])
env.Default(copy_headers)

#### Target: CINT dictionary for data structure classes
cint_cls= Split('DS/Root DS/MC DS/EV DS/MCParticle DS/MCPhoton '
                'DS/MCPMT DS/PMT '
                'DS/PosFit DS/EFit DS/VetoHit DS/MCHit DS/MCSample '
                'DS/PMTSample DS/MCTrack DS/MCTrackStep DS/Calib '
                'DSReader TrackNav TrackNode TrackCursor DB DBLink '
                'DBTextLoader Log DS/LinkDef')
cint_headers = ['include/RAT/' + clsname for clsname in cint_cls]

ratdict = env.RootDict(os.path.join(env['BUILDDIR'], 'RAT_Dict.cc'),
                       cint_headers)

#### Target: RAT Library
ratlib = env.StaticLibrary(env['RATLIB'], ratobj + ratdict)


#### Target: Main RAT program
ratbin = env.RATApp(env['RATBIN'], [])
env.Default(ratbin)


#### Target: Shared library for ROOT
cint_source = Split('ds/Root io/DSReader io/TrackNav io/TrackCursor '
                    'db/DB db/DBLink db/DBTextLoader db/DBTable '
                    'core/Log stlplus/fileio stlplus/multiio '
                    'stlplus/string_utilities stlplus/dprintf stlplus/textio '
                    'stlplus/exceptions stlplus/debug stlplus/file_system ')
solib_obj = env.SharedObject([os.path.join(env['BUILDDIR'], clsname+'.cc')
                              for clsname in cint_source])
ratsolib = env.SharedLibrary(env['RATSOLIB'], solib_obj + ratdict)
env.Default(ratsolib)
"""
#### Target: Documentation
#tar_html = env.Command('doc/html/index.html', 'doc/html.tar.gz',
#                       'tar xvzf $SOURCE -C doc')
dox = env.Command('does_not_exist3', 'doc/Doxyfile',
                  'doxygen doc/Doxyfile && cd doc/html')
#                  ' && ./installdox -lgeant4.tag@http://nu.ph.utexas.edu/rat/geant4/')

env.Alias('doc', [dox])

