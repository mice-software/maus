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
DOC_OUTPUT_DIR = os.path.join(TEMP_DST, 'doc')
SCP_LOG = TMP+'scp.log'
COPY_TARGETS = []

THIRDPARTY_URL = 'http://heplnv152.pp.rl.ac.uk/maus/third_party/'
PYTHON_MODULES_URL = THIRDPARTY_URL + 'easy_install/'
DEPRECATED_PYTHON_MODULES = ['pil']

def build_test_output():
    """Build test output and coverage, add to copy targets"""
    print "Building test output"
    test_log = open(TEST_LOG, 'w')
    run_tests_bash = os.path.join(os.environ["MAUS_ROOT_DIR"], "tests",
                                                              "run_tests.bash")
    testproc = subprocess.Popen(['bash', run_tests_bash], stdout=test_log,
                                                       stderr=subprocess.STDOUT)
    testproc.wait() # pylint: disable=E1101
    if testproc.returncode != 0: # pylint: disable=E1101
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
    here = os.getcwd()
    os.chdir(os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc', 'doc_tools'))
    doxy_log = open(DOXY_LOG, 'w')
    for doxy_script in ['generate_third_party_doc.py', 'generate_maus_doc.py']:
        doxy_cmd = os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc', \
                                                       'doc_tools', \
                                                       doxy_script)
        doxyproc = subprocess.Popen(['python', doxy_cmd, '--noprompt'], \
                                                       stdout=doxy_log, \
                                                       stderr=subprocess.STDOUT)
        doxyproc.wait() # pylint: disable=E1101
        if doxyproc.returncode != 0: # pylint: disable=E1101
            print "ERROR - doxygen failed for doxyfile: ", doxy_script
    COPY_TARGETS += glob.glob(os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc',
                                                       'doxygen_*'))
    COPY_TARGETS.append(os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc',
                                                       'index.html'))
    os.chdir(here)

def build_user_guide():
    """Build user guide (html and pdf) add to copy targets"""
    print "Building user guide"
    here = os.getcwd()
    os.chdir(os.path.join(os.environ['MAUS_ROOT_DIR'], 'doc', 'doc_src'))
    latex_log = open(LATEX_LOG, 'w')
    for index in range(2): #pylint: disable = W0612
        latexproc = subprocess.Popen(['pdflatex', 'maus_user_guide.tex'],
                                     stdout=latex_log, stderr=subprocess.STDOUT)
        latexproc.wait() # pylint: disable=E1101
        if latexproc.returncode != 0: # pylint: disable=E1101
            print "ERROR - latex failed"
        latexproc = subprocess.Popen(['latex2html', 'maus_user_guide.tex',
                                         "-split", "4", "-html_version", "3.2"],
                                     stdout=latex_log, stderr=subprocess.STDOUT)
        latexproc.wait() # pylint: disable=E1101
        if latexproc.returncode != 0: # pylint: disable=E1101
            print "ERROR - latex failed"
    COPY_TARGETS.append(os.path.join(os.getcwd(), 'maus_user_guide.pdf'))
    COPY_TARGETS.append(os.path.join(os.getcwd(), 'maus_user_guide'))
    os.chdir(here)

def build_third_party_tarball():
    """Build tarball of third party libraries"""
    print "Refreshing python libraries"
    proc = subprocess.Popen([
                       "bash",
                       os.environ['MAUS_THIRD_PARTY']+\
                                      "/third_party/bash/40python_extras.bash",
                       "-cg"])
    proc.wait() #pylint: disable=E1101
    print "Restoring deprecated python moodules"
    restore_deprecated_modules(os.path.join(os.environ['MAUS_ROOT_DIR'], \
      "third_party/source/easy_install"))
    print "Getting targets for third_party libraries"
    os.chdir(os.path.join(os.environ['MAUS_ROOT_DIR'], "third_party"))
    glob_list = ["source/*.tar.gz", "source/easy_install/", "source/*.tgz",
                 "source/*.egg", "source/*.tar", "source/*.tarz",
                 "source/*.bz2", "source/*.pl"]
    tarball_targets = []
    tarball_name = "third_party_libraries_incl_python.tar.gz"
    for targets in glob_list:
        tarball_targets += glob.glob(targets)
    print """Building third party tarball - source is MAUS_THIRD_PARTY, """+\
          """target is MAUS_ROOT_DIR"""
    proc = subprocess.Popen(["tar", "-czf",
                             tarball_name]
                             +tarball_targets)
    proc.wait() # pylint: disable=E1101
    proc = subprocess.Popen(["md5sum", tarball_name],
                            stdout=open(tarball_name+".md5", "w"))
    proc.wait() # pylint: disable=E1101
    COPY_TARGETS.append(os.path.join(os.getcwd(), tarball_name))
    COPY_TARGETS.append(os.path.join(os.getcwd(), tarball_name+".md5"))

def copy_targets():
    """Copy targets to a temporary store in tmp"""
    import Configuration
    print "Copying to tmp"
    if os.path.exists(TEMP_DST):
        shutil.rmtree(TEMP_DST)
    os.mkdir(TEMP_DST)
    version = Configuration.Configuration().handler_maus_version(
                                                            {'maus_version':''})
    version = version.replace(' ', '_')
    tmp_out_dir = os.path.join(TEMP_DST, version)
    os.mkdir(tmp_out_dir)
    doc_out_dir = os.path.join(TEMP_DST, version, 'doc')
    os.mkdir(doc_out_dir)
    print COPY_TARGETS
    for target in COPY_TARGETS:
        if 'doxygen' in target:
            out_dir = doc_out_dir
        else:
            out_dir = tmp_out_dir
        print 'Copying target', target
        if os.path.isdir(target):
            last = target.split('/')[-1]
            shutil.copytree(target, os.path.join(out_dir, last))
        else:
            shutil.copy(target, out_dir)
    return out_dir, version

def restore_deprecated_modules(target_dir):
    """ Pull down python modules no longer on PyPI"""
    for depmod in DEPRECATED_PYTHON_MODULES:
        os.system('wget ' + PYTHON_MODULES_URL + depmod + '.tar.gz' + \
          ' -P ' + target_dir)

def scp(scp_in, scp_out):
    """scp targets across"""
    print "Final scp from", scp_in, 'to', scp_out
    scp_log = open(SCP_LOG, 'w')
    scp_proc = subprocess.Popen(['scp', '-r', scp_in, scp_out], stdout=scp_log,
                                                       stderr=subprocess.STDOUT)
    scp_proc.wait() # pylint: disable=E1101
    if scp_proc.returncode != 0: # pylint: disable=E1101
        print "ERROR - scp failed"

def main():
    """main function"""
    print "Doing server build"
    build_user_guide()
    build_doxygen()
    build_third_party_tarball()
    scp_in, version = copy_targets()
    if len(sys.argv) > 1:
        scp_out = os.path.join(sys.argv[1], version)
        scp(scp_in, scp_out)
    else:
        print "No scp target found, I am done"

if __name__ == "__main__":
    main()

