#!/bin/env python
"""Generate doxygen documentation for third party libraries"""
# This file should be run first (before generate_maus_doc.py).
# For more information see README.

import subprocess
import os
import fnmatch

if os.environ.get('MAUS_THIRD_PARTY') == None:
    print 'MAUS_THIRD_PARTY environment variable not set.'
    print 'Please make sure to source env.sh before running this script.'
    print 'Exiting...'
    exit()

def main():
    """Generate list of third party libraries, then run doxygen on them"""
    maus_thirdparty = os.environ['MAUS_THIRD_PARTY']
    maus_thirdparty_build = maus_thirdparty + '/third_party/build/'
    thirdpartylibs = ['root', 'geant4', 'clhep', 'jsoncpp']
    print 'MAUS_THIRD_PARTY path is set to ' + maus_thirdparty
    print 'Checking for third party libraries...'
    if os.listdir(maus_thirdparty_build)==[]:
        print 'third_party/build/ is empty.'
        print 'Please build third party libraries before running this script.'
        print 'Aborting.'
        exit()
    tplibpath_dict = find_paths(thirdpartylibs, maus_thirdparty_build)
    raw_input('If everything is correct, please press enter to compile '
              'documentation')
    # Input folders follow different conventions for the libraries, so have to
    # be defined manually
    tplibinput_dict = dict()
    tplibinput_dict['root'] = (tplibpath_dict.get('root', 'NA') + 'include/ ' +
                              tplibpath_dict.get('root', 'NA') + 'lib/')
    tplibinput_dict['geant4'] = tplibpath_dict.get('geant4', 'NA') + 'source/'
    tplibinput_dict['clhep'] = tplibpath_dict.get('clhep', 'NA')
    tplibinput_dict['jsoncpp'] = (tplibpath_dict.get('jsoncpp', 'NA') +
                                 'include/ ' +
                                 tplibpath_dict.get('jsoncpp', 'NA') + 'src/')
    # Run doxygen for all listed libraries for which a path has been found
    for tplib in thirdpartylibs:
        if tplibpath_dict.get(tplib) != None:
            run_doxygen(tplib, tplibinput_dict[tplib])
    print 'Third party libraries documentation built.'
    print 'Now run "generate_MAUS_doc.py" to build MAUS documentation.'
    exit()


# Check if third party libraries are installed and if there are doubles.
# Then create dictionary with paths to all the newest versions.

def find_paths(thirdpartylibs, maus_thirdparty_build):
    """Find paths to highest version numbers of installed third party libs"""
    tplib_dict = dict([ (i, []) for i in thirdpartylibs])
    tplibpath_dict = dict()
    # Find all paths matching each library to deal with the case where multiple
    # version numbers are available
    for tplib in thirdpartylibs:
        for tpldir in os.listdir(maus_thirdparty_build):
            if fnmatch.fnmatch(tpldir, tplib + '*'):
                tplib_dict[tplib].append(tpldir)
        # Case where no matching directory has been found
        if len(tplib_dict[tplib]) == 0:
            print ('WARNING: Library ' + tplib +
                  ' not found in third party directory')
            choice = ''
            while choice not in ['s', 'a']:
                choice = raw_input('[s]kip, [a]bort program ')
                if choice == 'a':
                    exit()
                else:
                    continue
        # If more than one path has been found, use max() to find the highest
        # version number
        elif len(tplib_dict[tplib]) > 1:
            tplibpath_dict[tplib] = (maus_thirdparty_build +
                                    max(tplib_dict[tplib]) + '/')
            print ('More than one version of ' + tplib +
                   ' found in third party directory.')
            print 'Using highest version number: ' +max(tplib_dict[tplib])
        elif len(tplib_dict[tplib]) == 1:
            tplibpath_dict[tplib] = (maus_thirdparty_build +
                                     tplib_dict[tplib][0] + '/')
            print (tplib + ' found in third party directory. Using ' +
                   tplib_dict[tplib][0])
    return tplibpath_dict


# No graphs for 3rd party to keep time and space requirements reasonable.
def run_doxygen(tplib, input_path):
    """Run doxygen for third party libraries"""
    script = """( cat Doxyfile.general ;
        echo PROJECT_NAME = MAUS-$0;
        echo INPUT = $1;
        echo CALLER_GRAPH = NO;
        echo CALL_GRAPH = NO;
        echo CLASS_GRAPH = NO;
        echo COLLABORATION_GRAPH = NO;
        echo DIRECTORY_GRAPH = NO;
        echo GROUP_GRAPH = NO;
        echo INCLUDED_BY_GRAPH = NO;
        echo INCLUDE_GRAPH = NO;
        echo OUTPUT_DIRECTORY = ${MAUS_THIRD_PARTY}/doc/doxygen_$0/;
        echo GENERATE_TAGFILE = ${MAUS_THIRD_PARTY}/doc/doxygen_$0/$0.tag ) | doxygen -
        """
    subprocess.call([script, str(tplib), str(input_path)], shell=True)
#        echo EXCLUDE_PATTERNS = *TCollectionProxyInfo.h;

if __name__ == "__main__":
    main()
