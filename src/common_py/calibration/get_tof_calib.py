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
Get TOF calibrations from DB
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
        self._run_cali = {}
        self.reset()
        cfg = Configuration()
        cfgdoc = cfg.getConfigJSON()
        cfgdoc_json = json.loads(cfgdoc)
        cdb_url = cfgdoc_json['cdb_download_url'] + 'calibration?wsdl'
        self.cdb_server = cdb.Calibration()
        self.cdb_server.set_url(cdb_url)
        self.tof_devs = {'TOF0', 'TOF1', 'TOF2'}
        self.tof_ctypes = {'t0', 'tw', 'trigger'}
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
        #print 'get_calib: ', devname, ctype, fromdate
        if devname != "" and ctype != "":
            if devname not in self.tof_devs or ctype not in self.tof_ctypes:
                raise Exception('get_tof_calib failed. \
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
            raise Exception('get_tof_calib failed.No device/calibration type.')
        return self._current_cali  

    def get_calib_for_run(self, devname, run_num, ctype):
        """
        Evaluate a string expression given by formula
        """
        self.reset()
        #print 'get_calib_for_run: ', devname, run_num, ctype
        if devname != "" and ctype != "":
            if devname not in self.tof_devs or ctype not in self.tof_ctypes:
                raise Exception('get_tof_calib failed. \
                                 Invalid detector/calib type.')
            if run_num > 0 and run_num < 1251:
                raise Exception('get_calib_for_run failed. \
                                 Invalid run number.')
            try:
                #print 'getting calibration_for_run ',devname,run_num,ctype
                self._run_cali = self.cdb_server.get_calibration_for_run(\
                                                 devname, run_num, ctype)
                #print self._run_cali
                return self._run_cali
            except CdbPermanentError:
                #print 'failed to get calibration by run. trying by date...'
                fromdate = self.date_from_run(run_num)
                return self.get_calib(devname, ctype, fromdate)
        else:
            #print 'gtc: ',self._run_cali
            return self._run_cali

    def date_from_run(self, run_num):
        """
        This is really hacky because:
         cdbServer->getDetectorCalibrationByRun is broken
        So, set the valid date corresponding to this run number
        And do a get_calibrations_for_date
        Monte Carlo runs are 0 so just set them to 'current'
        """
        self.reset()
        if run_num >= 1251 and run_num < 3201:
            fromdate = '2009-11-01 12:00:00'
        if run_num >= 3201 and run_num < 4956:
            fromdate = '2011-11-24 12:00:00'
        if run_num >= 4956 and run_num < 5790:
            fromdate = '2013-08-01 12:00:00'
        if run_num >= 5790 and run_num < 6155:
            fromdate = '2014-06-29 12:00:00'
        if run_num >= 6155 and run_num < 6288:
            fromdate = '2015-03-21 00:00:00'
        if run_num >= 6288 or run_num == 0:
            fromdate = 'current'
        return fromdate
    
    def reset(self):
        """
        Reinitialize calibration 
        """
        self._current_cali = {}
        self._run_cali = {}
