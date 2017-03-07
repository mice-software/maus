"""
This file defines the abstract base for all Inputters
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

from API.IInput import IInput
from API.ModuleBase import ModuleBase

class InputBase(ModuleBase, IInput):
    """
    @class InputBase
    Abstract Base class for all inputters to derive from which allowes for
    abstracted behaviour to be inserted before the call to the modules
    _methods.
    """
    def __init__(self):
        """Constructor"""
        super(InputBase, self).__init__()
        
    def emitter(self):
        """
        Handles the emitting of data

        This is part of the public interface and serves to wrap the
        private _emitter method with any abstracted behaviour

        @return the data object
        """
        return self._emitter()

    def _emitter(self):
        """
        Handles the emitting of data

        This is the private method which must be overridden in derived classes
        to provide the inputters emitting behaviour.
        
        @return the data object
        """       
        raise NotImplementedError('_emitter function not implemented '\
                                  'in class %s' % self.__class__.__name__)
