#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""
Contains setup information for each of the different codes
"""

import os
import xboa.Common as Common
import geometry # pylint: disable=W0403

class CodeSetup:
    """
    Base class for handling conversions of units/etc between different codes.
    """
    def __init__(self):
        """Initialise class"""
        pass

    def __str__(self):
        """Return code name"""
        raise NotImplementedError(__name__+" not implemented in this code")

    def convert_position(self, value):
        """Convert from position units of mm"""
        raise NotImplementedError(__name__+" not implemented in this code")

    def convert_momentum(self, value):
        """Convert from momentum units of MeV/c"""
        raise NotImplementedError(__name__+" not implemented in this code")

    def convert_pid(self, value):
        """Convert from PDG PID"""
        raise NotImplementedError(__name__+" not implemented in this code")

    def convert_material(self, value):
        """Convert from MAUS Material definitions"""
        raise NotImplementedError(__name__+" not implemented in this code")

    def get_substitutions(self):
        """
        Return dict of file_in:file_out files in which substitutions are made
        """
        raise NotImplementedError(__name__+" not implemented in this code")

    def get_executable(self):
        """
        Return name of the executable
        """
        raise NotImplementedError(__name__+" not implemented in this code")

    def get_parameters(self):
        """
        Return list of command line parameters to be passed to the code
        """
        raise NotImplementedError(__name__+" not implemented in this code")

    def get_output_filename(self):
        """
        Return output filename to which reference data should be written.

        Note that TestFactory will automatically put this into the ref_data
        directory
        """
        raise NotImplementedError(__name__+" not implemented in this code")

    def get_bunch_read_keys(self):
        """
        Return arguments for xboa.new_dict_from_read_builtin, typically a dict
        in the format (output_format, output_filename)
        """
        raise NotImplementedError(__name__+" not implemented in this code")

    def get_bunch_index(self):
        """
        Return the index (typically station number) for items for this test
        """
        raise NotImplementedError(__name__+" not implemented in this code")

class MausSetup(CodeSetup):
    """
    Conversions and setup for MAUS - which is the default format and hence
    mostly null-ops
    """
    def __init__(self):
        """Initialise - does nothing"""
        CodeSetup.__init__(self)
        maus_version = open(os.path.expandvars('$MAUS_ROOT_DIR/README'))\
                                                                     .readline()
        maus_version = maus_version.rstrip('\n')
        maus_version = maus_version.split(' ')[-1]
        self._version = maus_version

    def __str__(self):
        """Return name of this code"""
        return 'maus_'+self._version

    def convert_position(self, value):
        """Does nothing"""
        return value

    def convert_momentum(self, value):
        """Does nothing"""
        return value

    def convert_pid(self, value):
        """Does nothing"""
        return value

    def convert_material(self, value):
        """Does nothing"""
        return value

    def get_substitutions(self):
        """Input file is configuration file and geometry definitions"""
        return {geometry.source('maus/configuration.in'):\
                geometry.temp('configuration.py'),
                geometry.source('maus/MaterialBlock.in'):\
                geometry.temp('MaterialBlock.dat')}

    def get_executable(self):
        """Executable is simulate_mice.py"""
        path = os.path.join('$MAUS_ROOT_DIR', 'bin', 'simulate_mice.py')
        return os.path.expandvars(path)

    def get_parameters(self):
        """Command line parameter to specify control files"""
        return ['--configuration_file', geometry.temp('configuration.py'),
                '--simulation_geometry_filename', 
                                          geometry.temp('MaterialBlock.dat'),
                '--output_root_file_name', geometry.temp('maus_output.root')]

    def get_output_filename(self):
        """Output filename dynamically pulls MAUS version from README"""
        return 'maus.'+self._version+'.ref_data.dat'

    def get_bunch_read_keys(self):
        """I/O type is maus_root_virtual_hit"""
        return ('maus_root_virtual_hit', geometry.temp('maus_output.root'))

    def get_bunch_index(self):
        return 2

class IcoolSetup(CodeSetup):
    """
    Conversions and setup for MAUS - which is the default format and hence
    mostly null-ops
    """
    def __init__(self):
        """Initialise - does nothing"""
        CodeSetup.__init__(self)
        if os.getenv('ICOOL') == None:
            raise EnvironmentError("Need to define $ICOOL environment "+\
                                   "variable with ICOOL executable path")
        if os.getenv('ICOOL_VERSION') == None:
            raise EnvironmentError("Need to define $ICOOL_VERSION "+\
                                   "environment variable with ICOOL version")
        self._version = os.getenv('ICOOL_VERSION')


    def __str__(self):
        """Return name of this code"""
        return 'icool_'+self._version

    def convert_position(self, value):
        """Does nothing"""
        return value/Common.units['m']

    def convert_momentum(self, value):
        """Does nothing"""
        return value/Common.units['GeV/c']

    def convert_pid(self, value):
        """Does nothing"""
        return Common.pdg_pid_to_icool[value]

    def convert_material(self, value):
        """Does nothing"""
        conversions = {
              'lH2':'LH',
              'LITHIUM_HYDRIDE':'LIH',
              'Al':'AL',
              'Be':'BE',
              'lHe':'LHE',
              'STEEL304':'FE',
              'Cu':'CU'
        }
        return conversions[value]

    def get_substitutions(self):
        """Input file is configuration file and geometry definitions"""
        return {geometry.source('icool/for001.in'):\
                './for001.dat',}

    def get_executable(self):
        """Executable is simulate_mice.py"""
        return os.path.expandvars('$ICOOL')

    def get_parameters(self):
        """Command line parameter to specify control files"""
        return []

    def get_output_filename(self):
        """Output filename dynamically pulls MAUS version from README"""
        return 'icool.'+self._version+'.ref_data.dat'

    def get_bunch_read_keys(self):
        """I/O type is maus_root_virtual_hit"""
        return ('icool_for009', geometry.temp('for009.dat'))

    def get_bunch_index(self):
        """Bunch is station 2"""
        return 5

class G4blSetup(CodeSetup):
    """
    Conversions and setup for G4Beamline
    """
    def __init__(self):
        """Initialise - does nothing"""
        CodeSetup.__init__(self)
        if os.getenv('G4BL') == None:
            raise EnvironmentError("Need to define $G4BL environment "+\
                                   "variable with G4Beamline executable path")
        if os.getenv('G4BL_VERSION') == None:
            raise EnvironmentError("Need to define $G4BL_VERSION "+\
                                   "environment variable with G4bl version")
        self._version = os.getenv('G4BL_VERSION')

    def __str__(self):
        """Return name of this code"""
        return 'g4bl_'+self._version

    def convert_position(self, value):
        """Does nothing"""
        return value

    def convert_momentum(self, value):
        """Does nothing"""
        return value

    def convert_pid(self, value):
        """Does nothing"""
        return value

    def convert_material(self, value):
        """Does nothing"""
        conversions = {
            'lH2':'LH2', 'LITHIUM_HYDRIDE':'LITHIUM_HYDRIDE', 'Al':'Al', 
            'Be':'Be', 'lHe':'lHe', 'STEEL304':'Fe','Cu':'Cu'
        }
        return conversions[value] 

    def get_substitutions(self):
        """Input file is configuration file and geometry definitions"""
        return {geometry.source('g4bl/g4bl_deck.in'):\
                geometry.temp('g4bl_deck')}

    def get_executable(self):
        """Executable is simulate_mice.py"""
        return os.getenv('G4BL')

    def get_parameters(self):
        """Command line parameter to specify control files"""
        return [geometry.temp('g4bl_deck')]

    def get_output_filename(self):
        """Output filename dynamically pulls MAUS version from README"""
        return 'g4bl.'+self._version+'.ref_data.dat'

    def get_bunch_read_keys(self):
        """I/O type is maus_root_virtual_hit"""
        return ('icool_for009',  geometry.temp('for009_g4bl.txt'))

    def get_bunch_index(self):
        return 2

class G4MICESetup(CodeSetup):
    """
    Conversions and setup for MAUS - which is the default format and hence
    mostly null-ops
    """
    def __init__(self):
        """Initialise - does nothing"""
        CodeSetup.__init__(self)
        if os.getenv('MICESRC') == None:
            raise EnvironmentError("Need to define $MICESRC environment "+\
                                   "variable with G4MICE root directory")
        if os.getenv('G4MICE_VERSION') == None:
            raise EnvironmentError("Need to define $G4MICE_VERSION "+\
                                   "environment variable with G4MICE version")
        self._version = os.getenv('G4MICE_VERSION')

    def __str__(self):
        """Return name of this code"""
        return 'g4mice_'+self._version


    def convert_position(self, value):
        """Does nothing"""
        return value

    def convert_momentum(self, value):
        """Does nothing"""
        return value

    def convert_pid(self, value):
        """Does nothing"""
        return value

    def convert_material(self, value):
        """Does nothing"""
        return value

    def get_substitutions(self):
        """Input file is configuration file and geometry definitions"""
        return {geometry.source('g4mice/cards.in'):\
                geometry.temp('cards'),
                geometry.source('g4mice/MaterialBlock.in'):\
                'MaterialBlock.dat'}

    def get_executable(self):
        """Executable is simulate_mice.py"""
        return os.path.join('$MICESRC', 'Applications', 'Simulation', 
                                                                   'Simulation')

    def get_parameters(self):
        """Command line parameter to specify control files"""
        return [geometry.temp('cards')]

    def get_output_filename(self):
        """Output filename dynamically pulls MAUS version from README"""
        g4mice_version = os.getenv('G4MICE_VERSION')
        return 'maus.'+self._version+'.ref_data.dat'

    def get_bunch_read_keys(self):
        """I/O type is maus_root_virtual_hit"""
        return ('g4mice_virtual_hit', geometry.temp('Sim.out.gz'))

    def get_bunch_index(self):
        return 2


