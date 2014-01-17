#!/bin/env python

# Generate doxygen documentation for MAUS components. This file should be run second, after generate_third_party_doc.py
# For more information see README.

import subprocess
import os
from generate_doc_index import create_index_html

if os.environ.get('MAUS_THIRD_PARTY') == None:
    print 'MAUS environment variables not set.'
    print 'Please make sure to source env.sh before running this script. Exiting...'
    exit()

MausThirdParty = os.environ['MAUS_THIRD_PARTY']
MausRootDir = os.environ['MAUS_ROOT_DIR']
ThirdPartyLibs = ['root', 'geant4', 'clhep', 'jsoncpp']

#choice = ''
#while choice not in ['l', 's']:
#    choice = raw_input('Would you like to use third party documentation compiled [l]ocally or on the [s]erver? ')
#if choice == 's':
#    MausThirdParty = '/home/Jan/MICE/thirdparty_doc'


def main():
    MausComponents = ['framework', 'datastructure', 'input', 'map', 'reduce', 'output']
    print 'MAUS_THIRD_PARTY path is set to ' + MausThirdParty
    print 'Checking for existing tag files...'
    TagFiles = []
    # Check for tagfiles. Note that this information isn't actually used as an input to doxygen, just to let the user
    # choose whether he wants to continue if fewer tagfiles than expected were found. See below for more info.
    for TPLib in ThirdPartyLibs:
        TPPath = MausThirdParty + '/doc/doxygen_' + TPLib + '/' + TPLib + '.tag'
        if os.path.isfile(TPPath):
            TagFiles.append(TPPath)
            print 'Tagfile found: ' + TPPath
    # Case where we have 0 or 0 < # < expected tagfiles found.
    if len(TagFiles) == 0:
        print 'No Tagfiles found. Most likely, third party documentation has not been compiled.'
    elif len(TagFiles) <= 3:
        print 'Only found tagfiles for ' + str(len(TagFiles)) + ' out of 4 third party libraries.'
        print 'Maybe there was a problem with compiling third party documentation?'
    # If fewer tagfiles than expected found, prompt whether documentation should still be compiled.
    if len(TagFiles) <= 3:
        choice = ''
        while choice not in ['c', 'a']:
            choice = raw_input('Would you like to [c]ontinue anyway or [a]bort ')
            if choice == 'a':
                exit()
            else:
                continue

    raw_input('If everything is correct, please press enter to compile documentation')
    
    # A list of all tagfiles with doc location. MAUS component tagfiles are added one by one as they are created.
    TagfilesDict = dict((i, MausThirdParty + '/doc/doxygen_' + i + '/' + i + '.tag=' + MausThirdParty + '/doc/doxygen_' + i + '/html') for i in ThirdPartyLibs)
    TagfilesMausDict = dict((i, MausRootDir + '/doc/doxygen_' + i + '/' + i + '.tag=' + MausRootDir + '/doc/doxygen_' + i + '/html') for i in MausComponents)
    Tagfiles = ' '.join(TagfilesDict.values())
    
    for Component in MausComponents:
        if Component == 'framework':
            run_doxygen_framework(Tagfiles)
        elif Component == 'datastructure':
            run_doxygen_datastructure(Tagfiles)
        else:
            run_doxygen(Component, Tagfiles)
        Tagfiles = Tagfiles + ' ' + TagfilesMausDict[Component]
    create_index_html()
    exit()


# framework and datastructure need their own snippets due to some special settings

def run_doxygen_framework(Tagfiles):
    script = """( cat Doxyfile.general ;
        echo 'PROJECT_NAME = "MAUS-framework"';
        echo INPUT = ${MAUS_ROOT_DIR}/src/common_*/ ${MAUS_ROOT_DIR}/src/py_cpp/ ${MAUS_ROOT_DIR}/src/legacy/;
        echo OUTPUT_DIRECTORY = ${MAUS_ROOT_DIR}/doc/doxygen_framework/;
        echo TAGFILES = $0;
        echo QUIET = YES;
        echo EXCLUDE_PATTERNS = */DataStructure/*;
        echo GENERATE_TAGFILE = ${MAUS_ROOT_DIR}/doc/doxygen_framework/framework.tag ) | doxygen -
        """
    subprocess.call([script, str(Tagfiles)],shell=True)


def run_doxygen_datastructure(Tagfiles):
    script = """( cat Doxyfile.general ;
        echo 'PROJECT_NAME = "MAUS-datastructure"';
        echo INPUT = ${MAUS_ROOT_DIR}/src/common_cpp/DataStructure/;
        echo OUTPUT_DIRECTORY = ${MAUS_ROOT_DIR}/doc/doxygen_datastructure/;
        echo TAGFILES = $0;
        echo QUIET = YES;
        echo EXCLUDE_PATTERNS = *MausDataStructure.*
        echo GENERATE_TAGFILE = ${MAUS_ROOT_DIR}/doc/doxygen_datastructure/datastructure.tag ) | doxygen -
        """
    subprocess.call([script, str(Tagfiles)],shell=True)


def run_doxygen(Component, Tagfiles):
    script = """( cat Doxyfile.general ;
        echo PROJECT_NAME = MAUS-$0;
        echo INPUT = ${MAUS_ROOT_DIR}/src/$0/;
        echo OUTPUT_DIRECTORY = ${MAUS_ROOT_DIR}/doc/doxygen_$0/;
        echo TAGFILES = $1;
        echo QUIET = YES;
        echo GENERATE_TAGFILE = ${MAUS_ROOT_DIR}/doc/doxygen_$0/$0.tag ) | doxygen -
        """
    subprocess.call([script, str(Component), str(Tagfiles)],shell=True)


if __name__ == "__main__":
    main()
