"""
This file defines the abstract base for all Reducers
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

from API.IReduce import IReduce
from API.ModuleBase import ModuleBase

class ReduceBase(ModuleBase, IReduce):
    """
    @class ReduceBase
    Abstract Base class for all reducers to derive from which allowes for
    abstracted behaviour to be inserted before the call to the modules
    _methods.
    """
    def __init__(self):
        """Constructor"""
        super(ReduceBase, self).__init__()

    def process(self, data):
        """
        Handles the processing of data

        This is part of the public interface and serves to wrap the
        private _process method with any abstracted behaviour

        @param data the data object
        @return modified data
        """
        return self._process(data)

    def _process(self, data):
        """
        Handles the processing of data

        This is the private method which must be overridden in derived classes
        to provide the reducers processing behaviour.
        
        @param data the data object
        @return modified data
        """       
        raise NotImplementedError('_process function not implemented '\
                                  'in class %s' % self.__class__.__name__)
