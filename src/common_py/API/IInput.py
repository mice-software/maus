"""
This file defines the interface for all Inputters
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

from API.IModule import IModule

class IInput(IModule):
    """
    @class IInput
    Interface for all inputter modules.
    Structure mirrors that of the C++ API and will eventually be replaced by
    a SWIGged version of it.
    """
    def __init__(self):
        """Constructor"""
        super(IInput, self).__init__()

    def emitter(self):
        """
        Handles the emitting of data
        @return the data object
        """
        raise NotImplementedError('emitter function not implemented '\
                                  'in class %s' % self.__class__.__name__)
