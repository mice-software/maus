#!/usr/bin/env python

import glob
import subprocess
import os
import shutil
import sys

import Configuration

CPP_COVERAGE = os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc', 'cpp_coverage')
TEST_LOG = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', 'all_test.log')
TEST_PLOTS = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tests', 'integration', 'plots')
DOXY_LOG = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', 'doxygen.log')
LATEX_LOG = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', 'pdflatex.log')
TEMP_DST = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', 'server_build')
COPY_TARGETS = []


def build_test_output():
    print "Building test output"
    test_log = open(TEST_LOG, 'w')
    testproc = subprocess.Popen(['bash', 'run_tests.bash'], stdout=test_log,
                                                       stderr=subprocess.STDOUT)
    testproc.wait()
    if testproc.returncode != 0:
      print "ERROR - tests failed"
    else:
      COPY_TARGETS.append(TEST_PLOTS)
      COPY_TARGETS.append(TEST_LOG)
      COPY_TARGETS.append(CPP_COVERAGE)

def build_doxygen():
    print "Building doxygen"
    global COPY_TARGETS, DOXY_LOG
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
    print "Building user guide"
    latex_dir = os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc', 'doc_src')
    here = os.getcwd()
    os.chdir(os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc', 'doc_src'))
    latex_log = open(LATEX_LOG, 'w')
    for i in range(2):
        latexproc = subprocess.Popen(['pdflatex', 'maus_user_guide.tex'],
                                     stdout=latex_log, stderr=subprocess.STDOUT)
        latexproc.wait()
        if latexproc.returncode != 0:
            print "ERROR - latex failed"
        latexproc = subprocess.Popen(['latex2html', 'maus_user_guide.tex'],
                                     stdout=latex_log, stderr=subprocess.STDOUT)
        latexproc.wait()
        if latexproc.returncode != 0:
            print "ERROR - latex failed"
    COPY_TARGETS.append(os.path.join(os.getcwd(), 'maus_user_guide.pdf'))
    COPY_TARGETS.append(os.path.join(os.getcwd(), 'maus_user_guide'))
    os.chdir(here)

def make_tarball(version):
    v_number = version.split('_')[-1]
    release_log = open(RELEASE_LOG, 'w')
    subprocess.Popen('bzr', 'release', 'maus_v'+v_number+'.tar.gz', 
                      stdout=release_log, stderr=subprocess.STDOUT))

def copy_targets():
    print "Copying to tmp"
    global TEMP_DST, COPY_TARGETS
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
    print "Final scp from", scp_in, 'to', scp_out
    scp_proc = subprocess.Popen(['scp', '-r', scp_in, scp_out])
    scp_proc.wait()
    if scp_proc.returncode != 0:
        print "ERROR - scp failed"

def main():
    print "Doing server build"
    build_user_guide()
    build_doxygen()
    build_test_output()
    making_tarball()
    scp_in, version = copy_targets()
    scp_out = os.path.join(sys.argv[1], version)
    scp(scp_in, scp_out)

if __name__ == "__main__":
    main()

