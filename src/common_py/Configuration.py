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

def get_config_json(config_file = None):
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
        exec(config_file.read(), globals(), config_dict) # pylint: disable=W0122

    config_dict['maus_version'] = get_version_from_readme()
    config_json_str = json.JSONEncoder().encode(config_dict)

    return config_json_str
    
def get_version_from_readme():
    """
    Version is taken as the first line in $MAUS_ROOT_DIR/README
    """
    readme = open(os.path.join(os.environ['MAUS_ROOT_DIR'], 'README'))
    version = readme.readline().rstrip('\n')
    return version

