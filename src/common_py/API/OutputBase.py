"""
This file defines the abstract base for all Outputters
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

from API.IOutput import IOutput
from API.ModuleBase import ModuleBase

class OutputBase(ModuleBase, IOutput):#pylint: disable=R0921
    """
    @class OutputBase
    Abstract Base class for all ouputters to derive from which allowes for
    abstracted behaviour to be inserted before the call to the modules
    _methods.
    """
    def __init__(self):
        """Constructor"""
        super(OutputBase, self).__init__()
        
    def save_spill(self, data):
        """
        Handles the saving of data

        This is part of the public interface and serves to wrap the
        private _save_spill method with any abstracted behaviour

        @param data the spill data object
        @return bool success/failure
        """
        return self._save_spill(data)

    def save_job_header(self, data):
        """
        Handles the saving of data

        This is part of the public interface and serves to wrap the
        private _save method with any abstracted behaviour

        @param data the job_header object
        @return bool success/failure
        """
        return self._save_job_header(data)

    def _save_spill(self, data):
        """
        Handles the saving of data

        This is the private method which must be overridden in derived classes
        to provide the outputters saving behaviour.
        
        @param data the spill object
        @return bool success/failure
        """       
        raise NotImplementedError('_save_spill function not implemented '\
                                  'in class %s' % self.__class__.__name__)

    def _save_job_header(self, data):
        """
        Handles the saving of job header data

        This is the private method which must be overridden in derived classes
        to provide the outputters saving behaviour.
        
        @param data the job header object
        @return bool success/failure
        """       
        raise NotImplementedError('_save_job_header function not implemented '\
                                  'in class %s' % self.__class__.__name__)

