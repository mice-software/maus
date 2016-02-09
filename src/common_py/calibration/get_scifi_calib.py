#!/usr/bin/env python
#pylint: disable=C0103
#pylint: disable=R0201
""" 
Read Tracker calibrations from the CDB
Includes ADC calibrations and channel mapping
to be included in MAUS.
"""

import cdb
import json
import os
from Configuration import Configuration
from cdb._exceptions import CdbPermanentError


class GetTrackerCalib:
    """
    Connects to the CDB and downloads tracker
    calibration, mapping, and bad channel lists
    """

    def __init__(self):
        print "Init running"
        self.config = json.loads(\
                      Configuration().getConfigJSON(command_line_args = True))
        calib_cdb_url = self.config['cdb_download_url'] + 'calibration?wsdl'
        self._calib = cdb.Calibration()
        self._calib.set_url(calib_cdb_url)
        print "Connected to calibration"
        cable_cdb_url = self.config['cdb_download_url'] + 'cabling?wsdl'
        self._cable = cdb.Cabling()
        self._cable.set_url(cable_cdb_url)
        print "Connected to cabling"

    def Process(self):
        """
        Needs datafile set to Run/Date/Current; date formate: "1984-09-14 00:10:00.0"
        """
        print "Starting process"
        method = self.config["SciFiCalibMethod"]
        _input = self.config["SciFiCalibSrc"]
        if method == "Date":
            self.Date(_input)
        if method == "Run":
            try:
                self.Run(_input)
            except CdbPermanentError:
                self.Date(self.Convert_Run(_input))
        if method == "Current":
            self.Current()

    def Current(self):
        """
        Collects current calibration info
        """
        calib = self._calib.get_current_calibration("Trackers","trackers")
        badchan = self._calib.get_current_calibration("Trackers","bad_chan")
        cable = self._cable.get_current_cabling("Trackers")

        self.Output(calib, badchan, cable)

    def Run(self, run_num):
        """
        Collects calibration info by run
        """
        try:
            print "Getting SciFi calibrations and maps for Run ", run_num
            calib = self._calib.get_calibration_for_run\
                ("Trackers", run_num, "trackers")
            badchan = self._calib.get_calibration_for_run\
                  ("Trackers", run_num, "bad_chan")
            cable = self._cable.get_cabling_for_run\
                ("Trackers", run_num)
            self.Output(calib, badchan, cable)
        except CdbPermanentError:
            raise CdbPermanentError\
                   ("CDB error getting scifi calibration by run")

    def Date(self, date):
        """
        Collects calibration info by date
        """
        calib = self._calib.get_calibration_for_date\
                ("Trackers", date, "trackers")
        badchan = self._calib.get_calibration_for_date\
                  ("Trackers", date, "bad_chan")
        cable = self._cable.get_cabling_for_date\
                ("Trackers", date)

        self.Output(calib, badchan, cable)

    def Convert_Run(self, _input):
        """
        If new calibration files are uploaded this function needs updated
        """
        run = int(_input)
        if run < 6664:
            print "No tracker configuration data for this run"
        if run <= 6664 and run > 7273:
            date = "2015-06-02 00:10:00.0"
        if run <= 7273 and run > 7294:
            date = "2015-07-22 00:10:00.0"
        if run <= 7294:
            date = "2015-08-19 00:10:00.0"
        return date

    def Output(self, CAL, BC, CAB):
        """
        Outputs calibration files
        """
        path = os.environ["MAUS_ROOT_DIR"]
        path_calib = path + "/files/calibration/"
        path_cable = path + "/files/cabling/"
        print path
        calib_out = open(path_calib+"scifi_calibration.txt","w")
        badchan_out = open(path_calib+"scifi_bad_channels.txt","w")
        cable_out = open(path_cable+"scifi_mapping.txt","w")

        calib_out.write(CAL)
        badchan_out.write(BC)
        cable_out.write(CAB)

        calib_out.close()
        badchan_out.close()
        cable_out.close()

if __name__ == "__main__":
    GetTrackerCalib().Process()
