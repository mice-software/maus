"""
This file defines the interface for all Outputters
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

class IOutput(IModule):#pylint: disable=R0921
    """
    @class IOutput
    Interface for all outputter modules.
    Structure mirrors that of the C++ API and will eventually be replaced by
    a SWIGged version of it.
    """
    def __init__(self):
        """Constructor"""
        super(IOutput, self).__init__()

    def save_spill(self, data):
        """
        Handles the saving of spill data
        @param data the spill data object
        @return bool success/failure
        """
        raise NotImplementedError('save_spill function not implemented '\
                                  'in class %s' % self.__class__.__name__)

    def save_job_header(self, data):
        """
        Handles the saving of job header data
        @param data the job header data object
        @return bool success/failure
        """
        raise NotImplementedError('save_job_header function not implemented '\
                                  'in class %s' % self.__class__.__name__)

