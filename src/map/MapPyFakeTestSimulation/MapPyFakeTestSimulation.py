# -*- coding: utf-8 -*-
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


## @class MapPyFakeTestSimulation.MapPyFakeTestSimulation
#  MapPyFakeTestSimulation creates spills with a Stage 6
#  demo event with tracks and hits in detectors.
#
#  This is useful for testing mappers that work on simulation
#  data or doing load testing.  
#
import json
import types
import os

class MapPyFakeTestSimulation:
    def birth(self, configJSON):
        """ Setting env variables and prototype json file """
        config = json.loads(configJSON)
 
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        assert root_dir != None
        assert os.path.isdir(root_dir)

        self._filename = '%s/src/map/MapPyFakeTestSimulation/mausput_digits'    % root_dir
        assert os.path.isfile(self._filename)

        self._file = open(self._filename, 'r')
        self._document = self._file.readline().rstrip()
        self._file.close()

        return True

    def process(self, str):
        """ The process """
        return self._document

    def death(self):
        """ Destructor """
        return True

