"""
This file defines the interface for all Modules
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

class IModule(object):
    """
    @class IModule
    Interface for all types of modules, be they maps or outputters etc.
    Structure mirrors that of the C++ API and will eventually be replaced by
    a SWIGged version of it.
    """
    def __init__(self):
        """Constructor"""
        super(IModule, self).__init__()

    def birth(self, config):
        """
        Handles the initialisation of the module
        @param config the configuration object
        """
        raise NotImplementedError('birth function not implemented '\
                                  'in class %s' % self.__class__.__name__)

    def death(self):
        """
        Handles the finalisation of the module
        """
        raise NotImplementedError('death function not implemented '\
                                  'in class %s' % self.__class__.__name__)
    
