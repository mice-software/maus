"""
The Configuration tells MAUS and its processors how to
set themselves up.  This is meant to be a replacement
for the old G4MICE datacards.
"""

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

import os
import json
import ErrorHandler # pylint: disable = W0403
import argparse

class Configuration:
    """
    Configuration class is responsible for holding and parsing configuration
    default information and converting it to json
    """
    def __init__(self):
        """
        Initialise path to readme file
        """        
        self.readme = os.path.join(os.environ['MAUS_ROOT_DIR'], 'README')

    def getConfigJSON(self, config_file = None, command_line_args = False): # pylint:disable = C0301, C0103
        """
        Returns JSON config document

        The defaults are read from ConfigurationDefaults
        then (if applicable) values are added/replaced
        by the passed file.  A JSON file is returned.

        @param config_file (optional) overriding configuration file handle.  If
                          None then this argument is ignored. If it is a python
                          file handle (ie. open('my_config.dat','r') ) then that
                          file is read.
        @param command_line_args if set to True, take arguments from the command
               line. Can produce undesirable results if you are running the main
               process from e.g. nosetests so disabled by default.
        """
        maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
        assert maus_root_dir != None

        config_dict = {}
        defaults = open(maus_root_dir+"/src/common_py/ConfigurationDefaults.py")
        exec(defaults, globals(), config_dict) # pylint: disable=W0122
        config_dict = self.check_config_dict(config_dict)

        if command_line_args:
            config_dict = self.command_line_arguments(config_dict)

        if config_dict["configuration_file"] != "":
            cmd_line_config_file = open(config_dict["configuration_file"])
            exec(cmd_line_config_file.read(), globals(), config_dict) # pylint: disable=W0122,C0301

        if config_file != None:
            assert not isinstance(config_file, str)
            exec(config_file.read(), globals(), config_dict) # pylint: disable=W0122,C0301

        config_dict['maus_version'] = self.handler_maus_version(config_dict)
        config_dict['reconstruction_geometry_filename'] = \
                      self.handler_reconstruction_geometry_filename(config_dict)
        config_dict = self.check_config_dict(config_dict)
        self.configuration_to_error_handler(config_dict)
        config_json_str = json.JSONEncoder().encode(config_dict)
        return config_json_str

    def command_line_arguments(self, config_dict):
        """
        Parse arguments from the command line

        Use argparse module to control command line arguments. First add the
        arguments to the file taken from ConfigurationDefaults; force the 
        arguments into the right type; and then push them to config_dict.

        @returns the modified config_dict
        """
        # Argparse allows for a command line parser with keywords given by 
        # ConfigurationDefaults.p Additionally a keyword command has been
        # created to store the path and filename to a datacard
        parser = argparse.ArgumentParser()
        for key in sorted(config_dict.keys()):
            parser.add_argument('--'+key, '-'+key, action='store', dest=key, 
                                default=config_dict[key])
        results = parser.parse_args()

        # All parsed values are input as string types, this checks the type in
        # ConfigurationDefaults.py and attempts to return the parsed values to
        # orginal or similar type. Currently does not work for dict type
        # arguments
        for key, def_value in config_dict.iteritems(): # value is from defaults
            ap_value = getattr(results, key)
            if def_value == ap_value:
                continue
            elif isinstance (def_value, str):
                config_dict[key] = ap_value # shouldn't throw
            elif isinstance (def_value, bool):
                try:
                    config_dict[key] = self.string_to_bool(ap_value)
                except ValueError:
                    raise ValueError('Could not convert command line argument '\
                          +str(key)+' with value '+ap_value+\
                          ' to a boolean')

            # Checks if the default is an integer, if it is, it will try to
            # force the parsed argument back into int type, failing that, it
            # will try to force it into float type.  If the argument is no
            # longer a number an exception is raised.
            elif isinstance (def_value, int):
                try:
                    config_dict[key] = int(ap_value)
                except ValueError:
                    raise ValueError('Could not convert command line '+\
                          'argument '+str(key)+' with value '+\
                          str(ap_value)+' to an int')

            # Checks if the default is a float, if it is, this will try and
            # force the parsed argument back into float type.  If the argument
            # is no longer a number an exception is raised.
            elif isinstance (def_value, float):
                try:
                    config_dict[key] = float(ap_value)
                except ValueError:
                    raise ValueError('Could not convert command line argument '\
                          +str(key)+' with value '+ap_value+\
                          ' to a float')
            # If attempt to modify something that was neither int, bool, string,
            # float; raise NotImplementedError (json allows dict and arrays)
            else:
                raise NotImplementedError("Could not convert command line "+\
                "argument "+str(key)+" of type "+str(type(def_value))+\
                ". Can only parse string, int, bool, float types.")
        return config_dict
        
    def handler_maus_version(self, _config_dict):
        """
        Version is taken as the first line in $MAUS_ROOT_DIR/README
        """
        if _config_dict['maus_version'] != "":
            raise ValueError("MAUS version cannot be changed by the user")
        readme = open(self.readme)
        version = readme.readline().rstrip('\n')
        return version

    def handler_reconstruction_geometry_filename(self, _config_dict): # pylint: disable=C0103, R0201, C0301
        """
        reconstruction_geometry_filename follows simulation_geometry_filename
        if an empty string
        """
        if _config_dict['reconstruction_geometry_filename'] == '':
            return _config_dict['simulation_geometry_filename']
        return _config_dict['reconstruction_geometry_filename']
        

    def configuration_to_error_handler(self, config_dict): #pylint:disable=R0201
        """
        Hand configuration parameters to the error handler, so it can set itself
        up
        """
        ErrorHandler.DefaultHandler().ConfigurationToErrorHandler(config_dict) # pylint:disable = C0301, C0103

    def check_config_dict(self, config_dict): #pylint:disable=R0201
        """
        @brief Strip config_dict of inappropriate items

        Strip config_dict of any items that cannot or should not be encoded into
        json e.g. module objects or the docstring.

        @params config_dict dictionary to be checked
        @returns Copy of config_dict with non-encodable items stripped out
        """
        dict_copy = {}
        for key, value in config_dict.iteritems():
            try:
                json.JSONEncoder().encode({key:value})
                if key != '__doc__': # explicitly exclude the docstring
                    dict_copy[key] = value
            except TypeError:
                pass
        return dict_copy
    
    def string_to_bool(self, string_in): #pylint:disable=R0201
        """
        Convert from a string to a boolean value

        @params string_in if string_in is any one of true, 1, yes, y, t returns
                 true. Case insensitive.

        @returns boolean True or False
        """
        if str.lower(string_in) in ["true", "1", "yes", "y", "t"]:
            return True
        elif str.lower(string_in) in ["false", "0", "no", "n", "f"]:
            return False
        else:
            raise ValueError("Could not parse "+string_in+" as a boolean")

