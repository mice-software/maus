#!/usr/bin/env python

"""
Script to generate release data and put it on the micewww server. Generates:
- test log
- test plots
- C++ coverage report
- doxygen
- maus user guide in pdf and html

Then scps to a location specified on the command line (as a single directory
called <MAUS version>)
"""

import glob
import subprocess
import os
import shutil
import sys

import Configuration

# want to add version number to docs
# want to add tarball manufacture
# want to add coverage information

TMP = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', 'build_release.')
CPP_COVERAGE = os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc', 'cpp_coverage')
TEST_LOG = TMP+'all_test.log'
TEST_PLOTS = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tests', 'integration',
                                                                        'plots')
DOXY_LOG = TMP+'doxygen.log'
LATEX_LOG = TMP+'pdflatex.log'
TEMP_DST = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', 'server_build')
SCP_LOG = TMP+'scp.log'
COPY_TARGETS = []


def build_test_output():
    """Build test output and coverage, add to copy targets"""
    print "Building test output"
    test_log = open(TEST_LOG, 'w')
    run_tests_bash = os.path.join(os.environ["MAUS_ROOT_DIR"], "tests",
                                                              "run_tests.bash")
    testproc = subprocess.Popen(['bash', run_tests_bash], stdout=test_log,
                                                       stderr=subprocess.STDOUT)
    testproc.wait()
    if testproc.returncode != 0:
        print "ERROR - tests failed"
    else:
        COPY_TARGETS.append(TEST_PLOTS)
        COPY_TARGETS.append(TEST_LOG)
        if os.path.isdir(CPP_COVERAGE):
            COPY_TARGETS.append(CPP_COVERAGE)

def build_doxygen():
    """Build doxygen add to copy targets"""
    global COPY_TARGETS # pylint: disable=W0603
    print "Building doxygen"
    doc_tools = os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc', 'doc_tools')
    doxy_log = open(DOXY_LOG, 'w')
    for doxyfile in ['Doxyfile.framework', 'Doxyfile.backend']:
        path = os.path.join(doc_tools, doxyfile)
        doxyproc = subprocess.Popen(['doxygen', path], stdout=doxy_log,
                                                       stderr=subprocess.STDOUT)
        doxyproc.wait()
        if doxyproc.returncode != 0:
            print "ERROR - doxygen failed for doxyfile: "+path
    COPY_TARGETS += glob.glob(os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc',
                                                                   'doxygen_*'))

def build_user_guide():
    """Build user guide (html and pdf) add to copy targets"""
    print "Building user guide"
    here = os.getcwd()
    os.chdir(os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc', 'doc_src'))
    latex_log = open(LATEX_LOG, 'w')
    for index in range(2): #pylint: disable = W0612
        latexproc = subprocess.Popen(['pdflatex', 'maus_user_guide.tex'],
                                     stdout=latex_log, stderr=subprocess.STDOUT)
        latexproc.wait()
        if latexproc.returncode != 0:
            print "ERROR - latex failed"
        latexproc = subprocess.Popen(['latex2html', 'maus_user_guide.tex',
                                         "-split", "3", "-html_version", "3.2"],
                                     stdout=latex_log, stderr=subprocess.STDOUT)
        latexproc.wait()
        if latexproc.returncode != 0:
            print "ERROR - latex failed"
    COPY_TARGETS.append(os.path.join(os.getcwd(), 'maus_user_guide.pdf'))
    COPY_TARGETS.append(os.path.join(os.getcwd(), 'maus_user_guide'))
    os.chdir(here)

def build_third_party_tarball():
    """Build tarball of third party libraries"""
    print """Building third party tarball"""
    os.chdir(os.path.join(os.environ['MAUS_ROOT_DIR'], "third_party"))
    glob_list = ["source/*.tar.gz", "source/*.egg", "source/*.tgz"]
    tarball_targets = []
    tarball_name = "third_party_libraries.tar.gz"
    for targets in glob_list:
        tarball_targets += glob.glob(targets)
    proc = subprocess.Popen(["tar", "-czf",
                             tarball_name]
                             +tarball_targets)
    proc.wait()
    COPY_TARGETS.append(os.path.join(os.getcwd(), tarball_name))

def copy_targets():
    """Copy targets to a temporary store in tmp"""
    print "Copying to tmp"
    if os.path.exists(TEMP_DST):
        shutil.rmtree(TEMP_DST)
    os.mkdir(TEMP_DST)
    version = Configuration.Configuration().get_version_from_readme()
    version = version.replace(' ', '_')
    out_dir = os.path.join(TEMP_DST, version)
    os.mkdir(out_dir)
    print COPY_TARGETS
    for target in COPY_TARGETS:
        print 'Copying target', target
        if os.path.isdir(target):
            last = target.split('/')[-1]
            shutil.copytree(target, os.path.join(out_dir, last))
        else:
            shutil.copy(target, out_dir)
    return out_dir, version

def scp(scp_in, scp_out):
    """scp targets across"""
    print "Final scp from", scp_in, 'to', scp_out
    scp_log = open(SCP_LOG, 'w')
    scp_proc = subprocess.Popen(['scp', '-r', scp_in, scp_out], stdout=scp_log,
                                                       stderr=subprocess.STDOUT)
    scp_proc.wait()
    if scp_proc.returncode != 0:
        print "ERROR - scp failed"

def main():
    """main function"""
    print "Doing server build"
    build_user_guide()
    build_doxygen()
    build_test_output()
    build_third_party_tarball()
    scp_in, version = copy_targets()
    if len(sys.argv) > 1:
        scp_out = os.path.join(sys.argv[1], version)
        scp(scp_in, scp_out)
    else:
        print "No scp target found, I am done"

if __name__ == "__main__":
    main()

