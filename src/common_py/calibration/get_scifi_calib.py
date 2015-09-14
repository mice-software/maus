#!/usr/bin/env python
""" 
Read Tracker calibrations from the CDB
Includes ADC calibrations and channel mapping
to be included in MAUS.
"""

import cdb
import json
import os
from Configuration import Configuration


class GetTrackerCalib:

  def __init__(self):
    print "Init running"
    self.config = json.loads(Configuration().getConfigJSON())
    calib_cdb_url = self.config['cdb_download_url'] + 'calibration?wsdl'
    self._calib = cdb.Calibration()
    self._calib.set_url(calib_cdb_url)
    print "Connected to calibration"
    cable_cdb_url = self.config['cdb_download_url'] + 'cabling?wsdl'
    self._cable = cdb.Cabling()
    self._cable.set_url(cable_cdb_url)
    print "Connected to cabling"
    
    try:
      cdb.Calibration().get_status()
      cdb.Cabling().get_status()
    except CdbPermanentError:
      raise CdbPermanentError("CDB error")
      
#########################################################################################
# Needs datafile set to Run/Date/Current; date formate: "1984-09-14 00:10:00.0"         #
#########################################################################################
  def Process(self):
    print "Starting process"
    method = self.config["SciFiCalibMethod"]
    input = self.config["SciFiCalibSrc"]
    if method == "Date":
      self.Date(input)
    if method == "Run":
      self.Date(self.Convert_Run(input))
    if method == "Current":
      self.Current()
      
#########################################################################################
  def Current(self):
    calib = self._calib.get_current_calibration("Trackers","trackers")
    badchan = self._calib.get_current_calibration("Trackers","bad_chan")
    cable = self._cable.get_current_cabling("Trackers")
    
    self.Output(calib, badchan, cable)

#########################################################################################
  def Date(self, date):
    calib = self._calib.get_calibration_for_date("Trackers", date, "trackers")
    badchan = self._calib.get_calibration_for_date("Trackers", date, "bad_chan")
    cable = self._cable.get_cabling_for_date("Trackers", date)
    
    self.Output(calib, badchan, cable)
    
#########################################################################################
# If new calibration files are uploaded this function needs updated                     #
#########################################################################################
  def Convert_Run(self, input):
    run = int(input)
    if run<6664:
      print "No tracker configuration data for this run"
    if run<=6664 and run>7273:
      date="2015-06-02 00:10:00.0"
    if run<=7273 and run>7294:
      date="2015-07-22 00:10:00.0"
    if run<=7294:
      date="2015-08-19 00:10:00.0"
    return date
  
#########################################################################################
  def Output(self, cal, bc, cab):
    path = os.environ["MAUS_ROOT_DIR"]
    path += "/files/calibration/"
    print path
    calib_out = open(path+"scifi_calibraion.txt","w")
    badchan_out = open(path+"scifi_bad_channels.txt","w")
    cable_out = open(path+"scifi_cabling.txt","w")

    calib_out.write(cal)
    badchan_out.write(bc)
    cable_out.write(cab)

    calib_out.close()
    badchan_out.close()
    cable_out.close()
    
if __name__ == "__main__":
  GetTrackerCalib().Process()
