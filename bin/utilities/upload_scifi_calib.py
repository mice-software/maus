#!/usr/bin/env python
""" 
Write TOF calibrations from file
This script must, and can, be run only from a micenet computer
Reads calibrations from a file and uploads them to the CDB
For protection against unintentional uploads, the filename is commented out
Set the appropriate filenames and uncomment the _CALIFILE set lines before 
uploading
"""

from cdb import CalibrationSuperMouse
from cdb import CablingSuperMouse

########## Set the server names and calibration service

# cdb write server
_CDB_W_SERVER = 'http://172.16.246.25:8080'
#_CDB_W_SERVER = 'http://preprodcdb.mice.rl.ac.uk'
_CALI_SM = CalibrationSuperMouse(_CDB_W_SERVER)
_CABL_SM = CablingSuperMouse(_CDB_W_SERVER)

CALIFILE = ""
MAPFILE = ""
BADFILE = ""

#####################################################################
########## Valid-from date ranges for the calibration

# old tracker calibration from Lab 7.
# _TIMESTAMP = "2015-07-28 00:00:00.0"
_TIMESTAMP = "2015-09-14 00:00:01.0"

#####################################################################
## _TYPE: is the calibration type and can be one of tw, trigger, t0
## _DEVICE: is the trigger station
## _CALIFILE: is the calibration file to read for a given _TYPE and _DEVICE
## _MAPFILE: the fiber to channel mapping file
## _BCHFILE: the list of bad channels
#####################################################################
########## timewalk calibration
_TYPE_CA = 'trackers'
_TYPE_BC = 'bad_chan'
_DEVICE = 'Trackers'

if len(CALIFILE)>0:
    _CALIFILE = open(CALIFILE, 'r')
    _CALI = _CALIFILE.read()
    print _CALI_SM.set_detector(_DEVICE, _TYPE_CA, _TIMESTAMP, _CALI)
    _CALIFILE.close()
if len(MAPFILE)>0:
    _MAPFILE = open(MAPFILE, 'r')
    _MAP  = _MAPFILE.read()
    print _CABL_SM.set_detector(_DEVICE, _TIMESTAMP, _MAP)
    _MAPFILE.close()
if len(BADFILE)>0:
    _BCHFILE = open(BADFILE, 'r')
    _BC  = _BCHFILE.read()
    print _CALI_SM.set_detector(_DEVICE, _TYPE_BC, _TIMESTAMP, _BC)
    _BCHFILE.close()

print "Completed uploads"

#####################################################################
