"""
@brief Check that we obey cpplint portion of the style guide

This walks the directory structure of MAUS and checks for each file of ending
*.cpp, *.cc, *.h, *.hh that we do indeed obey cpplint. It's possible to set up
exceptions to the cpplint test, either by line, by file or even by directory.
Note however that exceptions for entire files or directories need to get the nod
from the MAUS manager.
"""

import os
import sys
import unittest
import glob
import copy

import cpplint_exceptions # pylint: disable=F0401
import cpplint # pylint: disable=F0401

# pylint: disable=C0301

def glob_maus_root_dir(glob_string):
    """
    @brief return glob relative to maus_root_dir

    @params glob_string file name or directory relative to maus_root_dir, with
            wildcards where appropriate

    @returns list of paths matching glob string, relative to maus_root_dir
    """
    here = os.getcwd()
    os.chdir(MAUS_ROOT_DIR)
    globs = glob.glob(glob_string)
    os.chdir(here)
    return globs

def walker(_maus_root_dir, _exclude_dirs, _exclude_files, _filename):
    """
    @brief walk up from maus_root_dir and run cpplint

    @params maus_root_dir start walking from here
    @params exclude_dirs list of directory paths relative to maus_root_dir that
            should be excluded from the walk (hence recursive)
    @params exclude_files list of file paths relative to maus_root_dir that 
            should be excluded from cpplint

    @returns number of errors found by cpplint
    """
    file_handle = open(_filename, 'w')
    cpplint._cpplint_state.SetOutputHandler(file_handle) # pylint: disable=W0212
    errors = 0
    for root_dir, ls_dirs, ls_files in os.walk(_maus_root_dir):
        maus_dir = root_dir[len(_maus_root_dir)+1:] #root_dir relative to maus
        path_exclude(_exclude_dirs, maus_dir, ls_dirs)
        path_exclude(_exclude_files, maus_dir, ls_files)
        errors += run_cpplint(root_dir, ls_files)
    file_handle.flush()
    return errors

def path_exclude(exclude_paths, path_root, path_list):
    """
    @brief Remove paths from path_list that are found in exclude_paths

    @params exclude_paths list of paths to exclude (paths relative to 
            MAUS_ROOT_DIR)
    @params path_root suffix to add to path_list using os.path.join
    @params path_list list of paths to check for exclusions and modify

    @returns reference to ls_dirs
    """
    i = 0
    while i < len(path_list): #loop over all elements in list that is changing length
        if os.path.join(path_root, path_list[i]) in exclude_paths:
            del(path_list[i])
        else:
            i += 1

CPPLINT_SUFFIXES = ['.hh', '.h', '.cpp', '.cc']
def run_cpplint(root_dir, file_list):
    """
    @brief Run cpplint style checking script against a list of files

    @params root_dir path at which files can be found
    @params file_list list of files to run against cpplint

    @returns number of errors found

    Note that this only runs against files with suffixes in the list:
              ['.hh', '.h', '.cpp', '.cc']
    """
    cpplint._cpplint_state.ResetErrorCounts() # pylint: disable=W0212
    for a_file in file_list:
        target = os.path.join(root_dir, a_file)
        for suffix in CPPLINT_SUFFIXES:
            if target.endswith(suffix):
                cpplint.ProcessFile(target, 3)
    return cpplint._cpplint_state.error_count # pylint: disable=W0212

class CpplintPostProcessor:
    """
    @class CpplintPostProcessor
    CpplintPostProcessor is really just a collection of functions associated
    with postprocessing the cpplint file - i.e. subtracting known errors that
    should pass cpplint.
    """
    def __init__(self):
        """Does nothing"""
        pass

    def get_line_numbers(self, cpplint_output_filename): # pylint: disable=R0201
        """
        @brief Read cpplint output file

        @params cpplint_output_filename the cpplint output to postprocess

        @returns a dict like {filename:[cpplint_error, line_number, '']
        """
        file_to_error = {}
        filename = os.path.join(MAUS_ROOT_DIR, cpplint_output_filename)
        file_handle = open(filename)
        lines = file_handle.readlines()
        for line in lines:
            line = line.rstrip('\n')
            words = line.split(' ')
            if words[0] != 'Done' and words[0] != 'Ignoring' and words[0] != 'File':
                file_path = words[0][0:-1] #words[0] = file_path:line_number:
                try:
                    line_number = int(file_path[file_path.rfind(':')+1:])
                except:
                    raise IOError(str(line)+' not in file '+filename)
                file_path = file_path[len(MAUS_ROOT_DIR)+1:file_path.rfind(':')]
                if file_path not in file_to_error:
                    file_to_error[file_path] = []
                file_to_error[file_path].append([line, line_number, ''])
        return file_to_error

    def get_line_strings(self, file_to_error): # pylint: disable=R0201
        """
        @brief Find lines of source code corresponding to a given error message

        @params file_to_error a dict like
                            {filename:[cpplint_error, line_number, '']}
                    typically generated by get_line_numbers

        @returns a dict like {filename:[cpplint_error, line_number, line_text]}
        """
        for key in file_to_error.keys():
            fh_out = open(os.path.join(MAUS_ROOT_DIR, key))
            lines = fh_out.readlines()
            for i, value in enumerate(file_to_error[key]):
                if value[1]-1 >= 0: #line 0 can sometimes be error
                    file_to_error[key][i][2] = lines[value[1]-1].rstrip('\n')
        return file_to_error

    def print_error(self, file_to_error, file_handle=sys.stdout): # pylint: disable=R0201
        """
        @brief Print formatted output

        @params file_to_error dict like {filename:[cpplint_error, line_number, 
                line_text]}
        @params file_handle filehandle to which we write
        """
        for my_files, key in file_to_error.iteritems():
            print >> file_handle, 'File', my_files
            for line in key:
                print >> file_handle, '   ', line[0]

    def print_files_with_errors(self, file_to_error, file_handle): # pylint: disable=R0201
        """
        @brief Print list of files that have errors (hack used below to create
               exclude_files lists)
        """
        print >> file_handle, '['
        keys = sorted(file_to_error.keys())
        for key in keys:
            out_string = key.split('/')[-1]
            print >> file_handle, "'"+out_string+"',",
        print >> file_handle, '\n]'

    def strip_ref_dict(self, ref_dict): # pylint: disable=R0201
        """
        @brief strip the comment and name from reference values

        @params ref_dict dict like {filename:[[line, reason, developer]]}

        @returns dict like {filename:[line]}
        """
        ref_dict_2 = {}
        for fname, error_list in ref_dict.iteritems():
            error_list_2 = []
            for i, error in enumerate(error_list): # pylint: disable=W0612
                error_list_2.append(error_list[i][0])
            ref_dict_2[fname] = error_list_2
        return ref_dict_2

    def compare_strings(self, test_dict, ref_dict):
        """
        @brief Look for errors in the test strings

        @params test_dict dict like {filename:[cpplint_error, line_number, 
                line_text]}
        @params ref_dict dict like {filename:[line_text, exception_reason, 
                developer_name]}
        """
        n_errors = 0
        ref_dict_2 = self.strip_ref_dict(ref_dict)
        for key, test_item in test_dict.iteritems():
            n_errors += len(test_item)
            if key in ref_dict:
                test_item_cp = copy.deepcopy(test_item)
                for [error, line_number, line] in test_item_cp:
                    if line in ref_dict_2[key]:
                        test_item.remove([error, line_number, line])
                        n_errors -= 1
        return n_errors, test_dict

    def process(self, cpplint_output_filename, _cpplint_exceptions,
                processed_output_filename=None):
        """
        @brief Parse the cpplint output and check for exceptions

        @params cpplint_output_filename go through the cpplint output in this 
                file and look for exceptions
        @params _cpplint_exceptions list of exceptions for the cpplint output
        @params processed_output_filename if not set to None, output will be 
                written to this file

        @returns integer corresponding to the number of errors that were not in 
                the exceptions list
        """
        file_to_error = self.get_line_numbers(cpplint_output_filename)
        file_to_error = self.get_line_strings(file_to_error)
        n_errors, test_dict = self.compare_strings(file_to_error, _cpplint_exceptions) # pylint: disable=W0612
        if processed_output_filename != None:
            file_handle = open(processed_output_filename, 'w')
            self.print_error(file_to_error, file_handle)
            file_handle.close()
        return n_errors

class TestCppStyle(unittest.TestCase): #pylint: disable=R0904
    """@brief Interface to unittest module"""
    def test_glob_maus_root_dir(self):
        """
        @brief self check - test the glob_maus_root_dir function (above)
        """
        self.assertEqual(
            glob_maus_root_dir(os.path.join('tests', 'style', 'cpplint_tes*')),
            [os.path.join('tests', 'style', 'cpplint_test')])

    def test_walker_and_path_exclude(self):
        """
        @brief self check - test we walk the path correctly, excluding relevant 
              files and directories
        """
        test_root_dir = os.path.join(MAUS_ROOT_DIR, 'tests', 'style')
        test_exclude_dirs = \
                      [os.path.join('cpplint_test', 'cpplint_test_exclude_dir')]
        test_exclude_files = [os.path.join('cpplint_test', 'force_fail.cc')]
        test_result = walker\
              (test_root_dir, test_exclude_dirs, test_exclude_files, os.devnull)
        self.assertEqual(test_result, 3) #2 errors from force_fail_2.cc

    def test_cpplint_postprocessor(self):
        """
        @brief self check - test we postprocess cpplint output correctly
        """
        #First we make some errors
        style_dir = os.path.join \
                              (MAUS_ROOT_DIR, 'tests', 'style', 'cpplint_test')
        file_name = os.path.join(MAUS_ROOT_DIR, 'tmp', 'test_cpplint.out')
        walker(style_dir, [], [], file_name)
        # now we try to postprocess
        test_exceptions = {'tests/style/cpplint_test/force_fail.cc':[
                ('//No space at start - should fail',
                 'Deliberately broken for testing', 'Chris Rogers'),
                ('//                                                        '+ \
                 '                       more than 80 lines and space at end '+\
                 '- should fail ', 'Deliberately broken for testing',
                 'Chris Rogers')
            ]
        }
        n_errors = CpplintPostProcessor().process(file_name, test_exceptions,
                     os.path.join(MAUS_ROOT_DIR, 'tmp', 'test_cpplint_out.out'))
        self.assertEqual(n_errors, 6)

    def test_cpp_style(self):
        """
        Run cpplint looking for C++ style errors
        """
        file_name = os.path.join(MAUS_ROOT_DIR, 'tmp', 'cpplint.out')
        walker(MAUS_ROOT_DIR, EXCLUDE_DIRS, EXCLUDE_FILES, file_name)
        test_result = CpplintPostProcessor().process(file_name,
                      cpplint_exceptions.exceptions,
                      processed_output_filename=file_name)
        self.assertEqual(test_result, 0,
            msg="Failed cpp style test - detailed output in file:\n"+file_name)


MAUS_ROOT_DIR = os.environ['MAUS_ROOT_DIR']

# exclude_dirs are directories that are explicitly excluded from the style
# check, usually third party code and stuff hanging around from the build
# process. Note you MUST get explicit approval from Chris Rogers
# before adding something to exclude_dirs.
EXCLUDE_DIRS = [
'third_party',
'build',
'doc',
'tmp',
'.sconf_temp',
os.path.join('src', 'map', 'MapCppPrint'),
os.path.join('tests', 'style'),
os.path.join('tests', 'integration', 'test_optics', 'src'),
]+glob_maus_root_dir(os.path.join('src', '*', '*', 'build')
)

# exclude_files are files that are explicitly excluded from the style check,
# usually legacy code that hasn't been cleaned up yet. Note you MUST get
# explicit approval from Chris Rogers before adding something to
# exclude_files.
#
# If you do fix a file, please remove the file from this list (so it is included
# in future regression tests)
EXCLUDE_FILES = []

def file_append(path, file_list):
    """Append path to file names in file_list"""
    exclude_list = []
    for a_file in file_list:
        exclude_list.append(os.path.join(path, a_file))
    return exclude_list

INTERFACE_FILES = [
'Spline1D.cc', 'For003Bank.hh', 'For009Bank.hh', 'MiceEventManager.hh', 'VmeAdcHit.cc', 'MICESpill.hh', 'VirtualHit.cc', 'DataBaseAPI.hh', 'MCParticle.hh', 'Memory.cc', 'VmeTdcHit.hh', 'EventLoader.hh', 'AnalysisPlaneBank.hh', 'Differentiator.hh', 'VlpcEventTime.hh', 'TurtleBank.hh', 'Interpolator.hh', 'MiceEventManager.cc', 'DataBaseReader.cc', 'SplineInterpolator.cc', 'RFFieldMap.cc', 'RunFooter.cc', 'MCVertex.cc', 'EMRHit.hh', 'KLDigit.cc', 'MICESpill.cc', 'Reader.hh', 'VmeBaseHit.hh', 'MCHit.cc', 'SplineInterpolator.hh', 'Interpolator.cc', 'VmefAdcHit.hh', 'VmeBaseHit.cc', 'MICEEvent.cc', 'SciFiHit.hh', 'DataBaseReader.hh', 'VmeScalerData.hh', 'KillHit.hh', 'MVector.hh', 'CkovDigit.hh', 'MICERun.cc', 'dataCards.hh', 'Memory.hh', 'For003Bank.cc', 'MCParticle.cc', 'KLDigit.hh', 'TriangularMesh.cc', 'RFData.hh', 'CkovDigit.cc', 'CppErrorHandler.cc', 'MMatrixToCLHEP.cc', 'MiceMaterials.hh', 'TofHit.cc', 'RunHeader.hh', 'VmeScalerData.cc', 'ThreeDFieldMap.hh', 'AnalysisPlaneBank.cc', 'SpecialHit.hh', 'CkovHit.cc', 'For009Bank.cc', 'VlpcHit.cc', 'VmeAdcHit.hh', 'PolynomialVector.hh', 'EMRDigit.cc', 'TofDigit.hh', 'MICEUnits.cc', 'AnalysisEventBank.hh', 'PolynomialVector.cc', 'ZustandVektor.cc', 'MCHit.hh', 'CkovHit.hh', 'MMatrix.cc', 'MiceMaterials.cc', 'BetaFuncBank.hh', 'SciFiDigit.hh', 'G4BLBank.cc', 'VirtualHit.hh', 'TofHit.hh', 'StagePosition.hh', 'RunFooter.hh', 'KLHit.hh', 'Differentiator.cc', 'MVector.cc', 'XMLMessage.cc', 'Spline1D.hh', 'SciFiHit.cc', 'ZustandVektor.hh', 'RunHeader.cc', 'VlpcHit.hh', 'BetaFuncBank.cc', 'VersionInfo.hh', 'AnalysisEventBank.cc', 'MICEUnits.hh', 'MICERun.hh', 'MMatrixToCLHEP.hh', 'RFData.cc', 'RFFieldMap.hh', 'Mesh.cc', 'CppErrorHandler.hh', 'EMRDigit.hh', 'MCVertex.hh', 'MMatrix.hh', 'EventTime.hh', 'KLHit.cc', 'dataCards.cc', 'TriangularMesh.hh', 'MagFieldMap.cc', 'VersionInfo.cc', 'VmeTdcHit.cc', 'TurtleBank.cc', 'EMRHit.cc', 'ThreeDFieldMap.cc', 'EventTime.cc', 'VmefAdcHit.cc', 'MICEEvent.hh', 'G4BLBank.hh', 'MagFieldMap.hh', 'SciFiDigit.cc', 'SpecialHit.cc', 'Mesh.hh', 'XMLMessage.hh', 'TofDigit.cc', 'DataBaseAPI.cc',
]

ENGMODEL_FILES = [
'Polycone.cc', 'MiceElectroMagneticField.hh', 'Q35.cc', 'MultipoleAperture.cc', 'Polycone.hh', 'MultipoleAperture.hh', 'MiceModToG4Solid.hh', 'Q35.hh', 'MiceModToG4Solid.cc', 'MiceMagneticField.hh',
]

BEAMTOOLS_FILES = [
'BTConstantField.cc', 'BTPillBox.cc', 'BTMagFieldMap.hh', 'micegsl.cc', 'BT3dFieldMap.hh', 'BTQuad.hh', 'BTSheet.hh', 'BTPillBox.hh', 'BTFieldConstructor.cc', 'BTRFFieldMap.hh', 'BTFastSolenoid.cc', 'BTCombinedFunction.cc', 'BTTracker.cc', 'BTMagFieldMap.cc', 'BTFieldGroup.hh', 'micegsl.hh', 'BTConstantField.hh', 'BTFieldConstructor.hh', 'BTSpaceChargeField.hh', 'BTField.hh', 'BTPhaser.hh', 'BTMidplaneMap.cc', 'BTSheet.cc', 'BTFastSolenoid.hh', 'BTPhaser.cc', 'BTMidplaneMap.hh', 'BTField.cc', 'BTTracker.hh', 'BTCombinedFunction.hh', 'BTFieldGroup.cc', 'BTSpaceChargeField.cc', 'BT3dFieldMap.cc', 'BTQuad.cc', 'BTSolenoid.cc', 'BTRFFieldMap.cc', 'BTSolenoid.hh',
]

SIMULATION_FILES = [
'MICEDetectorConstruction.hh', 'FillMaterials.cc', 'FillMaterials.hh', 'MICEDetectorConstruction.cc'
]

RECON_FILES = [
'SciFiDoubletCluster.cc', 'SciFiClusterRec.hh', 'TriggerKey.hh', 'TofPid.cc', 'KLChannelMap.hh', 'SciFiSeed.cc', 'Extrapolation.cc', 'TofRec.cc', 'PmtKey.cc', 'SciFiCMN.hh', 'SciFiTrack.hh', 'MousePid.hh', 'CkovChannelMap.hh', 'SciFiFieldMap.cc', 'SciFiNtupleFill.hh', 'SciFiPointRec.hh', 'SciFiDigitRec.hh', 'TofChannelMap.cc', 'KLCellHit.hh', 'KLRec.hh', 'TofCalibrationMap.hh', 'MouseTrack.hh', 'SciFiStraightTrackRec.cc', 'GlobalRec.hh', 'SciFiBadChans.cc', 'TofChannelMap.hh', 'TofTrigger.hh', 'PmtKey.hh', 'SciFiSeed.hh', 'TofSpacePoint.cc', 'SciFiDoubletCluster.hh', 'TofSpacePoint.hh', 'TriggerKey.cc', 'TofTrack.cc', 'SciFiRec.cc', 'EMRRec.cc', 'TofPid.hh', 'KalmanSeed.hh', 'SciFiKalTrack.cc', 'FProjector.cc', 'SciFiHelixTrackRec.cc', 'SciFiBadChans.hh', 'SciFiExtrap.hh', 'GlobalRec.cc', 'TofCalibrationMap.cc', 'SciFiHelixTrackRec.hh', 'TofTrack.hh', 'SciFiNtuple.hh', 'SciFiPointRec.cc', 'SciFiKalTrack.hh', 'SetupKalmanRec.hh', 'TofTrigger.cc', 'MouseTrack.cc', 'Extrapolation.hh', 'SciFiTrack.cc', 'SciFiCMN.cc', 'CkovChannelMap.cc', 'TofSlabHit.cc', 'SciFiStraightTrackRec.hh', 'SciFiDoubletMeas.hh', 'SciFiDigitRec.cc', 'CkovRec.cc', 'KLChannelMap.cc', 'TofRec.hh', 'TofSlabHit.hh', 'CkovRec.hh', 'KLCellHit.cc', 'EMRRec.hh', 'SciFiExtrap.cc', 'SciFiClusterRec.cc', 'SciFiRec.hh', 'KLRec.cc', 'SciFiSpacePoint.cc', 'SciFiDoubletMeas.cc', 'SciFiSpacePoint.hh', 'SetupKalmanRec.cc', 'SciFiFieldMap.hh', 'FProjector.hh',
]

DETMODEL_FILES = [
'MAUSSD.hh', 'MAUSSD.cc', 'KLSD.hh', 'DoubletFiberParam.cc', 'SpecialVirtualSD.hh', 'SpecialDummySD.hh', 'EMRSD.cc', 'TofSD.cc', 'CkovMirror.cc', 'CKOVSD.cc', 'SciFiSD.hh', 'KLFiber.cc', 'TofSD.hh', 'KLGlue.hh', 'DoubletFiberParam.hh', 'KLFiber.hh', 'KLSD.cc', 'EMRSD.hh', 'SciFiPlane.cc', 'SpecialVirtualSD.cc', 'KLGlue.cc', 'SciFiSD.cc', 'SciFiPlane.hh', 'CkovMirror.hh', 'CKOVSD.hh',
]

CALIB_FILES = [
'TpgDigitsParameters.hh', 'TofCalib.cc', 'VlpcCalib.hh', 'SciFiDigitizationParams.hh', 'VlpcCalib.cc', 'TofReconParams.cc', 'CKOV1DigitsParams.hh', 'TofDigitizationParams.cc', 'TpgReconstructionParams.hh', 'AnaParams.cc', 'CKOV1DigitsParams.cc', 'SciFiReconstructionParams.cc', 'TofDigitizationParams.hh', 'TpgDigitsParameters.cc', 'AnaParams.hh', 'CKOVDigitsParams.hh', 'CKOV2DigitsParams.cc', 'TofCalib.hh', 'TofReconParams.hh', 'SciFiDigitizationParams.cc', 'TpgReconstructionParams.cc', 'SciFiReconstructionParams.hh', 'CKOV2DigitsParams.hh', 'CKOVDigitsParams.cc',
]

CONFIG_FILES = [
'BeamlineParameters.cc', 'ModuleTextFileIO.hh', 'SciFiCableManager.hh', 'CoolingChannelGeom.cc', 'VlpcCableOsaka2.cc', 'TofCable.cc', 'ModuleConverter.cc', 'ModuleConverter.hh', 'VlpcCableOsaka3.cc', 'RFBackgroundParameters.cc', 'TofCable.hh', 'BeamlineGeometry.hh', 'BeamlineGeometry.cc', 'VlpcCableOsaka.hh', 'SciFiCableManager.cc', 'RFParameters.hh', 'MagnetParameters.hh', 'BeamlineParameters.hh', 'BeamParameters.cc', 'MiceModule.hh', 'VlpcCableOsaka2.hh', 'CoolingChannelGeom.hh', 'VlpcCableImperial.hh', 'MagnetParameters.cc', 'ModuleTextFileIO.cc', 'VlpcCableImperial.cc', 'BeamParameters.hh', 'RFParameters.cc', 'VlpcCable.hh', 'VlpcCableOsaka3.hh', 'RFBackgroundParameters.hh', 'VlpcCableOsaka.cc', 'MiceModule.cc',
]

OPTICS_FILES = [
'AnalysisPlaneBank.cc', 'CovarianceMatrix.cc', 'Material.cc', 'MICEStackingAction.cc', 'MICETrackingAction.cc', 'OpticsModel.cc', 'PhaseSpaceVector.cc', 'Tensor3.cc', 'Tensor.cc', 'TransferMapCalculator.cc', 'TransferMap.cc', 'TransportManager.cc',
'AnalysisPlaneBank.hh', 'CovarianceMatrix.hh', 'Material.hh', 'MICEStackingAction.hh', 'MICETrackingAction.hh', 'OpticsModel.hh', 'PhaseSpaceVector.hh', 'Tensor3.hh', 'Tensor.hh', 'TransferMapCalculator.hh', 'TransferMap.hh', 'TransportManager.hh',
]

CPP_UNIT_FILES = [
'MeshTest.cc', 'MAUSPrimaryGeneratorActionTest.cc', 'BTSolenoidTest.cc', 'JsonWrapperTest.cc', 'MAUSTrackingActionTest.cc', 'MAUSGeant4ManagerTest.cc', 'BTMultipoleTest.cc', 'MVectorTest.cc', 'DifferentiatorTest.cc', 'BTFieldConstructorTest.cc', 'MMatrixTest.cc', 'MiceModToG4SolidTest.cc', 'PolynomialTest.cc', 'BTTrackerTest.cc', 'MAUSSteppingActionTest.cc', 'CppErrorHandlerTest.cc', 'MiceModuleTest.cc', 'TriangularMeshTest.cc', 'dataCardsTest.cc',
]

CPP = os.path.join('src', 'legacy')
TEST = os.path.join('tests', 'cpp_unit')

# glob files here because of
GLOB_FILES = []
GLOB_FILES += file_append(os.path.join(CPP, 'Recon', '*'), RECON_FILES)
GLOB_FILES += file_append(os.path.join(CPP, 'DetModel'), DETMODEL_FILES)
GLOB_FILES += file_append(os.path.join(CPP, 'DetModel', '*'), DETMODEL_FILES)
GLOB_FILES += file_append(os.path.join(TEST, '*'), CPP_UNIT_FILES)

for f in GLOB_FILES:
    EXCLUDE_FILES += glob_maus_root_dir(f)

EXCLUDE_FILES += file_append(os.path.join(CPP, 'Interface'), INTERFACE_FILES)
EXCLUDE_FILES += file_append(os.path.join(CPP, 'EngModel'), ENGMODEL_FILES)
EXCLUDE_FILES += file_append(os.path.join(CPP, 'BeamTools'), BEAMTOOLS_FILES)
EXCLUDE_FILES += file_append(os.path.join(CPP, 'Simulation'), SIMULATION_FILES)
EXCLUDE_FILES += file_append(os.path.join(CPP, 'Calib'), CALIB_FILES)
EXCLUDE_FILES += file_append(os.path.join(CPP, 'Config'), CONFIG_FILES)
EXCLUDE_FILES += file_append(os.path.join(CPP, 'Optics'), OPTICS_FILES)

EXCLUDE_FILES += file_append \
  (os.path.join('src', 'common_cpp', 'DataStructure'), ['MausDataStructure.cc'])

if __name__ == "__main__":
    unittest.main()
