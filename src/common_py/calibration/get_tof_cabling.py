# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

"""
Get TOF cabling from DB
"""

import cdb
import json
from Configuration import Configuration
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError

class GetCabling:
    """
    Evaluator class to evaluate mathematical expressions 

    Able to see many simple math expressions and some common units; the
    standard geant4 system of units is enabled.
    """

    def __init__(self):
        """
        Initialise the evaluator with math functions and units
        """
        self._current_cabling = {}
        self.reset()
        cfg = Configuration()
        cfgdoc = cfg.getConfigJSON()
        cfgdoc_json = json.loads(cfgdoc)
        cdb_url = cfgdoc_json['cdb_download_url'] + 'cabling?wsdl'
        self.cdb_server = cdb.Cabling()
        try:
            cdb.Cabling().get_status()
        except CdbPermanentError:
            raise CdbPermanentError("CDB error") #pylint: disable = E0702
        except CdbTemporaryError:
            raise CdbTemporaryError("CDB error") #pylint: disable = E0702
        self.cdb_server.set_url(cdb_url)
        print '>Server: ', self.cdb_server.get_name(), \
                         self.cdb_server.get_version()

    def get_cabling(self, devname, fromdate):
        """
        Evaluate a string expression given by formula
        """
        tof_devs = {'TOF0', 'TOF1', 'TOF2'}
        #print 'Cabling: ', devname, fromdate
        if devname != "": 
            if devname not in tof_devs:
                raise Exception('get_tof_cabling failed. \
                                 Invalid detector.')
            # check whether we are asked for the current cabling 
            # or cabling for an older date
            if fromdate == "" or fromdate == "current":
                #print '>getting current cabling', devname
                try:
                    self._current_cabling = \
                         self.cdb_server.get_current_cabling(devname)
                except CdbPermanentError:
                    self._current_cabling = "cdb_permanent_error"
                except CdbTemporaryError:
                    self._current_cabling = "cdb_temporary_error"
            else:
                #print 'getting cabling for date', fromdate
                try:
                    self._current_cabling = \
                         self.cdb_server.get_cabling_for_date(devname,fromdate)
                except CdbPermanentError:
                    self._current_cabling = "cdb_permanent_error"
                except CdbTemporaryError:
                    self._current_cabling = "cdb_temporary_error"
            #print self._current_cabling
        else:
            raise Exception('get_tof_cabling failed.No device.')
        return self._current_cabling  

    def reset(self):
        """
        Reinitialize cabling 
        """
        self._current_cabling = {}
