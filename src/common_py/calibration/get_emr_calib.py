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
Get EMR calibration from DB
"""

import cdb
import json
from Configuration import Configuration
from cdb._exceptions import CdbPermanentError

class GetCalib:
    """
    Evaluator class to evaluate mathematical expressions 

    Able to see many simple math expressions and some common units; the
    standard geant4 system of units is enabled.
    """

    def __init__(self):
        """
        Initialise the evaluator with math functions and units
        """
        self._current_cali = {}
        self.reset()
        cfg = Configuration()
        cfgdoc = cfg.getConfigJSON()
        cfgdoc_json = json.loads(cfgdoc)
        cdb_url = cfgdoc_json['cdb_download_url'] + 'calibration?wsdl'
        self.cdb_server = cdb.Calibration()
        self.cdb_server.set_url(cdb_url)
        #print 'Server: ', self.cdb_server.get_name(), \
        #                  self.cdb_server.get_version()
        try:
            cdb.Calibration().get_status()
        except CdbPermanentError:
            raise CdbPermanentError("CDB error")

    def get_calib(self, devname, ctype, fromdate):
        """
        Evaluate a string expression given by formula
        """
        if devname != "" and ctype != "":
            if devname != "EMR" or ctype != "eps":
                raise Exception('get_emr_calib failed. \
                                 Invalid detector/calib type.')
            # check whether we are asked for the current calibration 
            # or calibration for an older date
            if fromdate == "" or fromdate == "current":
                #print 'getting current calib', devname, ctype
                try:
                    self._current_cali = \
                         self.cdb_server.get_current_calibration(devname, ctype)
                except CdbPermanentError:
                    self._current_cali = "cdb_permanent_error"
            else:
                #print 'getting calib for date', fromdate
                try:
                    self._current_cali = \
                         self.cdb_server.get_calibration_for_date(devname,
                                                                  fromdate, 
                                                                  ctype)
                except CdbPermanentError:
                    self._current_cali = "cdb_permanent_error"
            #print self._current_cali
        else:
            raise Exception('get_emr_calib failed. No device/calibration type.')
        return self._current_cali  

    def reset(self):
        """
        Reinitialize calibration 
        """
        self._current_cali = {}
