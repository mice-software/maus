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
import glob
import xboa.Common as Common
from physics_model_test import geometry

class CodeSetup(object):
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
        if os.getenv('MAUS_VERSION') == None:
            maus_version = open(os.path.expandvars('$MAUS_ROOT_DIR/README'))\
                                                                     .readline()
            maus_version = maus_version.rstrip('\n')
            maus_version = maus_version.split(' ')[-1]
            self._version = maus_version
        else:
            self._version = os.getenv('MAUS_VERSION')

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
            icool_version = open(os.path.\
                                  expandvars('/home/hep/sm1208/icool/README'))\
                                                                     .readline()
            icool_version = icool_version.rstrip('\n')
            icool_version = icool_version.split(' ')[-1]
            self._version = icool_version

        else:

            self._version = os.getenv('ICOOL')


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
              'Cu':'CU',
              'C':'C',
              'Ti':'TI',
              'Li':'LI',
              'Fe':'FE'
        }
        return conversions[value]

    def get_substitutions(self):
        """Input file is configuration file and geometry definitions"""
        return {geometry.source('icool/for001.in'):\
                './for001.dat',}

    def get_executable(self):
        """Executable is simulate_mice.py"""

        return os.path.join('/home/hep/sm1208/icool','icool')


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
        return 'maus.'+self._version+'.ref_data.dat'

    def get_bunch_read_keys(self):
        """I/O type is maus_root_virtual_hit"""
        return ('g4mice_virtual_hit', geometry.temp('Sim.out.gz'))

    def get_bunch_index(self):
        return 2

class Muon1Setup(CodeSetup):
    """
    Muon1 needs to run in it's local directory. I assume a script exists called

    muon1.bash
    
    that sits in $MUON1_DIR and calls muon1 with appropriate command line flags
    then moves output file 'muon1_output.csv' to geometry.temp()
    """
    def __init__(self):
        """Initialise - does nothing"""
        CodeSetup.__init__(self)
        if os.getenv('MUON1_DIR') == None:
            raise EnvironmentError("Need to define $MUON1_DIR "+\
                                   "environment variable with MUON1 directory")
        if os.getenv('MUON1_VERSION') == None:
            raise EnvironmentError("Need to define $MUON1_VERSION "+\
                                   "environment variable with MUON1 version")
        self._version = os.getenv('MUON1_VERSION')
        self._lattice = os.path.expandvars('$MUON1_DIR/lattices/')

    def __str__(self):
        """Return name of this code"""
        return 'muon1_'+self._version

    def convert_material(self, value):
        """Does nothing"""
        conversions = {
            'lH2':'LH2lumped\probarray%03d.dat' # pylint: disable = W1401
        }
        return conversions[value]

    def convert_position(self, value):
        """Does nothing"""
        return value

    def convert_momentum(self, value):
        """Converts to energy"""
        return (value**2.+Common.pdg_pid_to_mass[13]**2.)**0.5-\
               Common.pdg_pid_to_mass[13]

    def convert_pid(self, value):
        """Only mu+ are allowed"""
        if value == -13:
            return "muon"
        else:
            raise IndexError("PID "+str(value)+" not available in muon1")

    def get_substitutions(self):
        """Input file is configuration file and geometry definitions"""
        return {geometry.source('muon1/absorber_test.in'):\
                os.path.join(self._lattice, 'absorber_test.txt')}

    def get_executable(self):
        """Executable is simulate_mice.py"""
        return os.path.expandvars('$MUON1_DIR/muon1.bash')

    def get_parameters(self):
        """Command line parameter to specify control files"""
        return []

    def get_output_filename(self):
        """Output filename dynamically pulls MAUS version from README"""
        return 'muon1.'+self._version+'.ref_data.dat'

    def get_bunch_read_keys(self):
        """I/O type is maus_root_virtual_hit"""
        return ('muon1_csv',  geometry.temp('muon1_output.csv'))

    def get_bunch_index(self):
        return 0


class IcoolElmsSetup(IcoolSetup):
    """
    Conversions and setup for ICOOL_ELMS
    
    Need to set up a file called elmscom.txt which instructs icool where to get
    ELMS data from. I assume a file exists like

    RunDirectory.txt
    
    and a set of files like

    ELMSFv3run*

    in directory pointed to by $ICOOL_ELMSDB environment variable.
    """
    def __init__(self):
       
        super(IcoolElmsSetup, self).__init__()
        #if os.getenv('ICOOL_ELMSDB') == None:
            #raise EnvironmentError("/home/hep/sm1208/icool/elms")
        run_dir = os.path.expandvars(\
                                      "/home/hep/sm1208/icool/elms/"+\
                                        "RunDirectory.txt")
        if not os.path.isfile(run_dir):
            raise EnvironmentError("Couldn't find "+run_dir+" needed for elms")
        elms_db = "/home/hep/sm1208/icool/elms/ELMSFv3run*"
        if len(glob.glob(os.path.expandvars(elms_db))) < 1:
            raise EnvironmentError("Couldn't find files like "+elms_db+\
                                                            " needed for elms")
        IcoolElmsSetup._substitute_elmscom()
        
      
       

    def __str__(self):
        """Return icool_elms_<version>"""
        return 'icool-elms_'+self._version

    def convert_material(self, value):
        """Convert material strings"""
        conversions = {
              'lH2':'LH',
        }
        return conversions[value]

    def get_substitutions(self):
        """Input file is configuration file and geometry definitions"""
        return {
            geometry.source('icool/for001_elms.in'):'./for001.dat',
        }

    def get_output_filename(self):
        """Output filename dynamically pulls MAUS version from README"""
        return 'icool_elms.'+self._version+'.ref_data.dat'

    def _substitute_elmscom():
        """Switch out environment variable into _elmscom.txt"""
        Common.substitute(
            geometry.source('icool/_elmscom.txt'), 'elmscom.txt',
            {'${ICOOL_ELMSDB}':os.path.expandvars('${ICOOL_ELMSDB}')}
        )
    _substitute_elmscom = staticmethod(_substitute_elmscom)

