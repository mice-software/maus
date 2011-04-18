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

class Configuration:
    ## Returns JSON config document
    #
    #  The defaults are read from ConfigurationDefaults
    #  then (if applicable) values are added/replaced
    #  by the passed file.  A JSON file is returned.
    #
    #  \param configFile (optional) overriding configuration
    def getConfigJSON(self, configFile = None):
        MAUSRootDir = os.environ.get('MAUS_ROOT_DIR')
        assert MAUSRootDir != None

        configDict = {}
        defaultFilename = '%s/src/core/ConfigurationDefaults.py' % MAUSRootDir
        exec(open(defaultFilename,'r').read(), globals(), configDict)

        if configFile != None:
            assert not isinstance(configFile, str)
            exec(configFile.read(), globals(), configDict)

        configJSONStr = json.JSONEncoder().encode(configDict)

        return configJSONStr
    


