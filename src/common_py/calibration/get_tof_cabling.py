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
        self._run_cabling = {}
        self.reset()
        cfg = Configuration()
        cfgdoc = cfg.getConfigJSON()
        cfgdoc_json = json.loads(cfgdoc)
        cdb_url = cfgdoc_json['cdb_download_url'] + 'cabling?wsdl'
        self.cdb_server = cdb.Cabling()
        self.tof_devs = {'TOF0', 'TOF1', 'TOF2'}
        try:
            cdb.Cabling().get_status()
        except CdbPermanentError:
            raise CdbPermanentError("CDB error") #pylint: disable = E0702
        except CdbTemporaryError:
            raise CdbTemporaryError("CDB error") #pylint: disable = E0702
        self.cdb_server.set_url(cdb_url)
        #print '>Server: ', self.cdb_server.get_name(), \
        #                 self.cdb_server.get_version()

    def get_cabling(self, devname, fromdate):
        """
        Evaluate a string expression given by formula
        """
        #tof_devs = {'TOF0', 'TOF1', 'TOF2'}
        #print 'Cabling: ', devname, fromdate
        if devname != "": 
            if devname not in self.tof_devs:
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

    def get_cabling_for_run(self, devname, run_num):
        """
        Get tof cabling map for given run number
        If the run number does not exist in the CDB,
        then get set the date based on run-range 
        as defined in the date_from_run method below
        and get cabling map for that date
        """
        self.reset()
        #print 'get_cabling_for_run: ', devname, run_num
        if devname != "" and run_num != "":
            if devname not in self.tof_devs:
                raise Exception('get_tof_cabling failed. \
                                 Invalid detector/cabling type.')
            if run_num > 0 and run_num < 1251:
                raise Exception('get_cabling_for_run failed. \
                                 Invalid run number.')
            try:
                #print 'getting cabling ',devname,run_num
                self._run_cabling = self.cdb_server.get_cabling_for_run(\
                                                 devname, run_num)
                #print self._run_cabling
                return self._run_cabling
            except CdbPermanentError:
                print '> Failed to get tof cabling by run. trying by date...', \
                      run_num
                fromdate = self.date_from_run(run_num)
                return self.get_cabling(devname, fromdate)
        else:
            #print 'gtc: ',self._run_cabling
            return self._run_cabling

    def date_from_run(self, run_num):
        """
        This is really hacky because:
         cdbServer->getDetectorCalibrationByRun is broken
        So, set the valid date corresponding to this run number
        And do a get_calibrations_for_date
        Monte Carlo runs are 0 so just set them to 'current'
        """
        self.reset()
        if run_num >= 1251 and run_num < 7496:
            fromdate = '2009-11-01 12:00:00'
        if run_num >= 7496 or run_num == 0:
            fromdate = 'current'
        return fromdate
    
    def reset(self):
        """
        Reinitialize cabling 
        """
        self._current_cabling = {}
