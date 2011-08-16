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
import copy
import ErrorHandler

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
        by the passed file.  A JSON file is returned.

        \param config_file (optional) overriding configuration file handle.  If
                          None then this argument is ignored. If it is a python
                          file handle (ie. open('my_config.dat','r') ) then that
                          file is read.
        """
        maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
        assert maus_root_dir != None

        config_dict = {}
        defaults = open(maus_root_dir+"/src/common_py/ConfigurationDefaults.py")
        exec(defaults, globals(), config_dict) # pylint: disable=W0122

        if config_file != None:
            assert not isinstance(config_file, str)
            exec(config_file.read(), globals(), config_dict) # pylint: disable=W0122,C0301

        config_dict['maus_version'] = self.get_version_from_readme()
        config_dict = self.check_config_dict(config_dict)
        self.configuration_to_error_handler(config_dict)
        config_json_str = json.JSONEncoder().encode(config_dict)

        return config_json_str
        
    def get_version_from_readme(self):
        """
        Version is taken as the first line in $MAUS_ROOT_DIR/README
        """
        readme = open(self.readme)
        version = readme.readline().rstrip('\n')
        return version

    def configuration_to_error_handler(self, config_dict):
        """
        Hand configuration parameters to the error handler, so it can set itself
        up
        """
        ErrorHandler.DefaultHandler().ConfigurationToErrorHandler(config_dict)

    def check_config_dict(self, config_dict):
        """
        @brief Check to see if config_dict can be encoded into json
        @params config_dict dictionary to be checked
        @returns Copy of config_dict with non-encodable items stripped out
        """
        dict_copy = {}
        for key, value in config_dict.iteritems():
            try:
                json.JSONEncoder().encode({key:value})
                dict_copy[key] = value
            except:
                print 'Failed to encode',str(key)+':'+str(value),'as json object'
        return dict_copy
        

