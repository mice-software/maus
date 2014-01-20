#!/bin/env python
"""Generate doxygen documentation for MAUS components"""
# This file should be run second, after generate_third_party_doc.py
# For more information see README.

import subprocess
import os
from generate_doc_index import create_index_html

if os.environ.get('MAUS_THIRD_PARTY') == None:
    print 'MAUS environment variables not set.'
    print 'Please make sure to source env.sh before running this script.'
    print 'Exiting...'
    exit()

def main():
    """Generate list of tagfiles, then trigger doxygen for components"""
    maus_thirdparty = os.environ['MAUS_THIRD_PARTY']
    maus_rootdir = os.environ['MAUS_ROOT_DIR']
    thirdpartylibs = ['root', 'geant4', 'clhep', 'jsoncpp']
    maus_components = ['framework', 'datastructure', 'input', 'map', 'reduce',
                      'output']
    print 'MAUS_THIRD_PARTY path is set to ' + maus_thirdparty
    print 'Checking for existing tag files...'
    tagfiles = []
    # Check for tagfiles. Note that this information isn't actually used as an
    # input to doxygen, just to let the user choose whether he wants to
    # continue if fewer tagfiles than expected were found. See below for more
    # info.
    for tplib in thirdpartylibs:
        tppath = (maus_thirdparty + '/doc/doxygen_' + tplib + '/' + tplib +
                  '.tag')
        if os.path.isfile(tppath):
            tagfiles.append(tppath)
            print 'Tagfile found: ' + tppath
    # Case where we have 0 or 0 < # < expected tagfiles found.
    if len(tagfiles) == 0:
        print ('No Tagfiles found. Most likely, third party documentation has '
              'not been compiled.')
    elif len(tagfiles) <= 3:
        print ('Only found tagfiles for ' + str(len(tagfiles)) +
              ' out of 4 third party libraries.')
        print ('Maybe there was a problem with compiling third party '
              'documentation?')
    # If fewer tagfiles than expected found, prompt whether documentation
    # should still be compiled.
    if len(tagfiles) <= 3:
        choice = ''
        while choice not in ['c', 'a']:
            choice = raw_input('Would you like to [c]ontinue anyway or '
                               '[a]bort ')
            if choice == 'a':
                exit()
            else:
                continue

    raw_input('If everything is correct, please press enter to compile ' +
              'documentation')
    
    # A list of all tagfiles with doc location. MAUS component tagfiles are
    # added one by one as they are created.
    tagfiles_dict = dict((i, maus_thirdparty + '/doc/doxygen_' + i + '/' + i +
                        '.tag=' + maus_thirdparty + '/doc/doxygen_' + i +
                        '/html') for i in thirdpartylibs)
    tagfilesmaus_dict = dict((i, maus_rootdir + '/doc/doxygen_' + i + '/' + i +
                            '.tag=' + maus_rootdir + '/doc/doxygen_' + i +
                            '/html') for i in maus_components)
    tagfiles = ' '.join(tagfiles_dict.values())
    
    for component in maus_components:
        if component == 'framework':
            run_doxygen_framework(tagfiles)
        elif component == 'datastructure':
            run_doxygen_datastructure(tagfiles)
        else:
            run_doxygen(component, tagfiles)
        tagfiles = tagfiles + ' ' + tagfilesmaus_dict[component]
    create_index_html()
    exit()


# framework and datastructure need own snippets due to some special settings

def run_doxygen_framework(tagfiles):
    """Run doxygen for the MAUS framework"""
    script = """( cat Doxyfile.general ;
        echo 'PROJECT_NAME = "MAUS-framework"';
        echo INPUT = ${MAUS_ROOT_DIR}/src/common_*/ ${MAUS_ROOT_DIR}/src/py_cpp/ ${MAUS_ROOT_DIR}/src/legacy/;
        echo OUTPUT_DIRECTORY = ${MAUS_ROOT_DIR}/doc/doxygen_framework/;
        echo TAGFILES = $0;
        echo QUIET = YES;
        echo EXCLUDE_PATTERNS = */DataStructure/*;
        echo GENERATE_TAGFILE = ${MAUS_ROOT_DIR}/doc/doxygen_framework/framework.tag ) | doxygen -
        """
    subprocess.call([script, str(tagfiles)], shell=True)


def run_doxygen_datastructure(tagfiles):
    """Run doxygen for the MAUS DataStructure"""
    script = """( cat Doxyfile.general ;
        echo 'PROJECT_NAME = "MAUS-datastructure"';
        echo INPUT = ${MAUS_ROOT_DIR}/src/common_cpp/DataStructure/;
        echo OUTPUT_DIRECTORY = ${MAUS_ROOT_DIR}/doc/doxygen_datastructure/;
        echo TAGFILES = $0;
        echo QUIET = YES;
        echo EXCLUDE_PATTERNS = *MausDataStructure.*
        echo GENERATE_TAGFILE = ${MAUS_ROOT_DIR}/doc/doxygen_datastructure/datastructure.tag ) | doxygen -
        """
    subprocess.call([script, str(tagfiles)], shell=True)


def run_doxygen(component, tagfiles):
    """Run doxygen for other MAUS components"""
    script = """( cat Doxyfile.general ;
        echo PROJECT_NAME = MAUS-$0;
        echo INPUT = ${MAUS_ROOT_DIR}/src/$0/;
        echo OUTPUT_DIRECTORY = ${MAUS_ROOT_DIR}/doc/doxygen_$0/;
        echo TAGFILES = $1;
        echo QUIET = YES;
        echo GENERATE_TAGFILE = ${MAUS_ROOT_DIR}/doc/doxygen_$0/$0.tag ) | doxygen -
        """
    subprocess.call([script, str(component), str(tagfiles)], shell=True)


if __name__ == "__main__":
    main()
