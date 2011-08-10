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


## @class Configuration
#  The Configuration tells MAUS and its processors how to
#  set themselves up.  This is meant to be a replacement
#  for the old G4MICE datacards.
#
import os
import json
import argparse
import operator
import sys

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


    def getConfigJSON(self, config_file = None):
        """
        Returns JSON config document

        The defaults are read from ConfigurationDefaults
        then (if applicable) values are added/replaced
        by the passed file or command line input.  A JSON 
        file is returned.

        Command line arguments are parsed from 
        ConfigurationDefaults as -sigmaPz 25. The command
        line can also be used to enter new arguments or 
        modify the default arguments from a data card as 
        -card /location/file
         

        \param config_file (optional) overriding configuration file handle.  If
                          None then this argument is ignored. If it is a python
                          file handle (ie. open('my_config.dat','r') ) then that
                          file is read.
        """
        maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
        assert maus_root_dir != None

#   config_dict is initialized to the values stored in ConfigurationDefaults.py
        config_dict = {}
        defaults = open(maus_root_dir+"/src/common_py/ConfigurationDefaults.py")
        exec(defaults, globals(), config_dict) # pylint: disable=W0122

#   Argparse allows for a command line parser with keywords given by ConfigurationDefaults.py
#   Additionally a keyword command has been created to store the path and filename to a datacard
        parser = argparse.ArgumentParser()
        for key, value in config_dict.iteritems():
            parser.add_argument('-'+key, action='store', dest=key, default=value)
        parser.add_argument('-card', action='store', dest='card', default=None)
        results = parser.parse_args()

#    All parsed values are input as string types, this checks the type in ConfigurationDefaults.py
#    and attempts to return the parsed values to orginal or similar type.  
#    Currently does not work for dict type arguments
        for key, value in config_dict.iteritems():
            print '\n', key, ' = ', config_dict[key]
            if config_dict[key] == getattr(results,key):
                continue

#    Checks if the default is boolean, if it is, checks for values true or false and enters
#    values accordingly.  If true or false are not entered an exception is raised.
            elif isinstance (value, bool):
                if getattr(results,key) == 'True' or getattr(results,key) == 'true':
                    config_dict[key] = True
                elif getattr(results,key) == 'False' or getattr(results,key) == 'false':
                    config_dict[key] = False
                else:
                    print key, ' needs to be a boolean value.'
                    raise ValueError ('Please enter a boolean value.')

#    Checks if the default is an integer, if it is, it will try and force the parsed argument
#    back into int type, failing that, it will try to force it into float type.  If the argument
#    is no longer a number an exception is raised.
            elif isinstance (value, int):
                try:
                    config_dict[key] = int(getattr(results,key))
                except ValueError:
                    try:
                        config_dict[key] = float(getattr(results,key))
                    except ValueError:
                        print key, ' needs to be a number.'
                        raise ValueError ('Please enter a number.')

#    Checks if the default is a float, if it is, this will try and force the parsed argument
#    back into float type.  If the argument is no longer a number an exception is raised.
            elif isinstance (value, float):
                try:
                    config_dict[key] = float(getattr(results,key))
                except ValueError:
                    print key, ' needs to be a number.'
                    raise ValueError ('Please enter a number.')

#    Checks if a change has been made to any of the dictionary arguments, if any change has occured
#    an exception is raised.
            elif isinstance (value, dict):
                if config_dict[key]!=getattr(results,key):
                    raise NotImplementedError("Unable to parse complex types. See issue #461.")
                else:
                    config_dict[key] = getattr(results,key)

#    If there are any changes to the default string values this statement will catch them
            else:
                config_dict[key] = getattr(results,key)
            print key, ' = ', config_dict[key]

#   If a data card is specified this will read in the file and either modify the arguements in
#   ConfigurationDefaults.py or add new arguments to config_dict
        if results.card != None:
            add_file = results.card
            exec(open(add_file, 'r').read(), globals(), config_dict)

        if config_file != None:
            assert not isinstance(config_file, str)
            exec(config_file.read(), globals(), config_dict) # pylint: disable=W0122,C0301

        config_dict['maus_version'] = self.get_version_from_readme()
        config_json_str = json.JSONEncoder().encode(config_dict)

        return config_json_str
    
    def get_version_from_readme(self):
        """
        Version is taken as the first line in $MAUS_ROOT_DIR/README
        """
        readme = open(self.readme)
        version = readme.readline().rstrip('\n')
        return version

