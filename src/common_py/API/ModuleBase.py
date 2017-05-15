"""
This file defines the abstract base for all Mappers
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

class ModuleBase(IModule):
    """
    @class ModuleBase
    Abstract Base class for all Modules to derive from which allowes for
    abstracted behaviour to be inserted before the call to the modules
    _methods.
    """
    def birth(self, config):
        """
        Handles the initialisation of the module.

        This is part of the public interface and serves to wrap the
        private _birth method with any abstracted behaviour

        @param config the configuration object
        """
        return self._birth(config)
    def death(self):
        """
        Handles the finalisation of the module

        This is part of the public interface and serves to wrap the
        private _death method with any abstracted behaviour
        """
        return self._death()

    def _birth(self, config):
        """
        Handles the initialisation of the module.

        This is the private method which must be overridden in derived classes
        to provide the initialisation behaviour for the module
        
        @param config the configuration object
        """       
        raise NotImplementedError('_birth function not implemented '\
                                  'in class %s' % self.__class__.__name__)

    def _death(self):
        """
        Handles the finalisation of the module
        
        This is the private method which must be overridden in derived classes
        to provide the finalisation behaviour for the module
        """
        raise NotImplementedError('_death function not implemented '\
                                  'in class %s' % self.__class__.__name__)
