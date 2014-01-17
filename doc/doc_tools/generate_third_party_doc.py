#!/bin/env python

# Generate doxygen documentation for third party libraries. This file should be run first.
# For more information see README.

import subprocess
import os
import fnmatch

if os.environ.get('MAUS_THIRD_PARTY') == None:
    print 'MAUS_THIRD_PARTY environment variable not set.'
    print 'Please make sure to source env.sh before running this script. Exiting...'
    exit()

MausThirdParty = os.environ['MAUS_THIRD_PARTY']
MausThirdPartyBuild = MausThirdParty + '/third_party/build/'


def main():
    ThirdPartyLibs = ['root', 'geant4', 'clhep', 'jsoncpp']
    print 'MAUS_THIRD_PARTY path is set to ' + MausThirdParty
    print 'Checking for third party libraries...'
    if os.listdir(MausThirdPartyBuild)==[]:
        print 'third_party/build/ is empty.'
        print 'Please build third party libraries before running this script. Aborting.'
        exit()
    TPLibPathDict = find_paths(ThirdPartyLibs)
    raw_input('If everything is correct, please press enter to compile documentation')
    # Input folders follow different conventions for the libraries, so have to be defined manually
    TPLibInputDict = dict()
    TPLibInputDict['root'] = TPLibPathDict.get('root', 'NA') + 'include/ ' + TPLibPathDict.get('root', 'NA') + 'lib/'
    TPLibInputDict['geant4'] = TPLibPathDict.get('geant4', 'NA') + 'source/'
    TPLibInputDict['clhep'] = TPLibPathDict.get('clhep', 'NA')
    TPLibInputDict['jsoncpp'] = TPLibPathDict.get('jsoncpp', 'NA') + 'include/ ' + TPLibPathDict.get('jsoncpp', 'NA') + 'src/'
    # Run doxygen for all listed libraries for which a path has been found
    for TPLib in ThirdPartyLibs:
        if TPLibPathDict.get(TPLib) != None:
            run_doxygen(TPLib, TPLibInputDict[TPLib])
    print 'Third party libraries documentation built. Now run "generate_MAUS_doc.py" to build MAUS documentation.'
    exit()


# Check if third party libraries are installed and if there are doubles.
# Then create dictionary with paths to all the newest versions.

def find_paths(ThirdPartyLibs):
    TPLibDict = dict([ (i, []) for i in ThirdPartyLibs])
    TPLibPathDict = dict()
    # Find all paths matching each library to deal with the case where multiple version numbers are available
    for TPLib in ThirdPartyLibs:
        for TPLDir in os.listdir(MausThirdPartyBuild):
            if fnmatch.fnmatch(TPLDir, TPLib + '*'):
                TPLibDict[TPLib].append(TPLDir)
        # Case where no matching directory has been found
        if len(TPLibDict[TPLib]) == 0:
            print 'WARNING: Library ' + TPLib + ' not found in third party directory'
            choice = ''
            while choice not in ['s', 'a']:
                choice = raw_input('[s]kip, [a]bort program ')
                if choice == 'a':
                    exit()
                else:
                    continue
        # If more than one path has been found, use max() to find the highest version number
        elif len(TPLibDict[TPLib]) > 1:
            TPLibPathDict[TPLib] = MausThirdPartyBuild + max(TPLibDict[TPLib]) + '/'
            print 'More than one version of ' + TPLib + ' found in third party directory.'
            print 'Using highest version number: ' +max(TPLibDict[TPLib])
        elif len(TPLibDict[TPLib]) == 1:
            TPLibPathDict[TPLib] = MausThirdPartyBuild + TPLibDict[TPLib][0] + '/'
            print TPLib + ' found in third party directory. Using ' + TPLibDict[TPLib][0]
    return TPLibPathDict


# Don't render graphs for 3rd party to keep time and space requirements reasonable.
# Exclude TCollectionProyInfo.h from ROOT as it somehow causes doxygen to segfault when double-referenced via tagfiles.
def run_doxygen(TPLib, InputPath):
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
    subprocess.call([script, str(TPLib), str(InputPath)],shell=True)
#        echo EXCLUDE_PATTERNS = *TCollectionProxyInfo.h;

if __name__ == "__main__":
    main()
