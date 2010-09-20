# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# For DB constants
#
# March 04, 2009
#
# July 16, 2009 : Adapted to LASER by Seb Viret <viret@in2p3.fr>
#

from src.ReadGenericCalibration import *
from src.region import *
import os
from array import array

# For reading from DB
from TileCalibBlobPython import TileCalibTools, TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK, LASPARTCHAN
from TileCalibBlobObjs.Classes import *

# For turning off annoying logging
import logging
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger

class WriteDB(ReadGenericCalibration):
    "write out a tileSqlite.db file with database constants"

    def get_latest_run(self):
        """
        Usage: get_latest_run()
        
        Arguments:
        
        None
        
        Returns:
        
        * integer indicating the latest run
        """
   
        f = urllib.urlopen('http://atlas-service-db-runlist.web.cern.ch/atlas-service-db-runlist/cgi-bin/latestRun.py')
        rv = f.read()
        f.close()
        return int(rv)


    def __init__(self, runType = 'CIS', offline_tag = 'HLT-UPD1-00',version = 1):
        self.runType = runType

        #
        # S.Viret (24/08/2009) :
        # 'version' is the version number of the offline database you want to use
        # This is usually 1 but it appears that in some cases (cesium and LASER
        # references) it's 2.
        #
    


        #
        # Here things are done depending on runType
        #

        if runType == 'CIS':
            offline_tag           = 'TileOfl0%dCalibCisFitLin-%s' % (version,offline_tag)
            self.offline_folder   = TileCalibTools.getTilePrefix()+"CALIB/CIS/FIT/LIN"
            self.offline_tag      = offline_tag
            self.online_folder    = TileCalibTools.getTilePrefix(ofl=False)+"CALIB/CIS/LIN"
            self.online_tag       = ""
            
        elif runType == 'Las':
            self.offline_tag      = 'TileOfl0%dCalibLasLin-%s' % (version,offline_tag)
            self.offline_tag_F    = 'TileOfl0%dCalibLasFiber-%s' % (version,offline_tag)
            self.offline_folder   = TileCalibTools.getTilePrefix()+"CALIB/LAS/LIN"
            self.offline_folder_F = TileCalibTools.getTilePrefix()+"CALIB/LAS/FIBER"
            self.online_folder    = TileCalibTools.getTilePrefix(ofl=False)+"CALIB/LAS/LIN"
            self.online_tag       = ""
            
        elif runType == 'Las_REF' or runType=='cesium':
            offline_tag           = 'TileOfl0%dCalibCes-%s' % (version,offline_tag)
            self.offline_folder   = '/TILE/OFL0%d/CALIB/CES' % version
            self.offline_tag      = offline_tag
            self.online_folder    = TileCalibTools.getTilePrefix(ofl=False)+"CALIB/CES" 
            self.online_tag       = ""
            
        else:
            print 'WriteDB: Failed to initialize'
            return

        # use the latest run number as starting point (common to everyone)
        self.offline_iov  = (self.get_latest_run(), 0) 
        self.online_iov   = (self.get_latest_run(), 0)
        
        PyCintex.makeClass('std::vector<float>')
        PyCintex.makeClass('std::vector<unsigned int>')


    #
    # Here we instantiate the different parameters
    #
    
    def ProcessStart(self):
        # open DB connection to sqlite file
        self.db = TileCalibTools.openDb('SQLITE', 'COMP200', 'UPDATE')
        
        self.blobWriterOnline  = TileCalibTools.TileBlobWriter(self.db,self.online_folder, 'Flt', False)
        self.blobWriterOffline = TileCalibTools.TileBlobWriter(self.db,self.offline_folder, 'Flt')

        if self.runType == 'Las': # For fiber and partition stuff
            self.blobWriterOffline_F = TileCalibTools.TileBlobWriter(self.db,self.offline_folder_F, 'Flt')   

        if self.runType == 'Las_REF' or self.runType == 'cesium': # For recovering Cesium reference values
            #self.db_REF = TileCalibTools.openDb('ORACLE', 'COMP200', 'READONLY')
            self.db_REF = TileCalibTools.openDbConn('COOLOFL_TILE/COMP200', 'READONLY')
            self.blobReader = TileCalibTools.TileBlobReader(self.db_REF, self.offline_folder, self.offline_tag)
        
        util = PyCintex.gbl.TileCalibUtils()

        #
        # Once again things are done depending on runType
        #

        if self.runType == 'CIS':

            loGainDef=(1023./800.)
            hiGainDef=(64.*1023./800.)
            loGainDefVec = PyCintex.gbl.std.vector('float')()
            loGainDefVec.push_back(loGainDef)
            hiGainDefVec = PyCintex.gbl.std.vector('float')()
            hiGainDefVec.push_back(hiGainDef)
            defVec = PyCintex.gbl.std.vector('std::vector<float>')()
            defVec.push_back(loGainDefVec)
            defVec.push_back(hiGainDefVec)
            
        elif self.runType == 'Las':

            lohiGainDefVec = PyCintex.gbl.std.vector('float')()
            lohiGainDefVec.push_back(1.)
            defVec = PyCintex.gbl.std.vector('std::vector<float>')()
            defVec.push_back(lohiGainDefVec)
            defVec.push_back(lohiGainDefVec)
         
        elif self.runType == 'Las_REF' or self.runType == 'cesium':

            default = PyCintex.gbl.std.vector('float')()
            default.push_back(  1.) # cesium constant
            default.push_back( -1.) # laser default value
            default.push_back(700.) # reference HV
            default.push_back( 20.) # reference temperature (same for all channels)
            defVec = PyCintex.gbl.std.vector('std::vector<float>')()
            defVec.push_back(default)
            defVec.push_back(default)

        # Then we initialize everything
            
        for ros in xrange(util.max_ros()):
            for drawer in xrange(util.getMaxDrawer(ros)):
                flt = self.blobWriterOffline.getDrawer(ros,drawer)
                flt.init(defVec, 48, 1)

                if self.runType == 'Las':
                    flt = self.blobWriterOffline_F.getDrawer(ros,drawer)
                    flt.init(defVec, 48, 1)
                
                flt = self.blobWriterOnline.getDrawer(ros,drawer)
                flt.init(defVec, 48, 1)
        
    def ProcessStop(self):
        # iov until is the end of the interval of validity, so infinity here
        iovUntil = (MAXRUN,MAXLBK)
        author   = "%s" % os.getlogin()
                        
        self.blobWriterOnline.setComment(author, "TUCS %s" % (" ".join(sys.argv)))
        self.blobWriterOnline.register(self.online_iov, iovUntil, self.online_tag)

        self.blobWriterOffline.setComment(author, "TUCS %s" % (" ".join(sys.argv)))
        self.blobWriterOffline.register(self.offline_iov, iovUntil, self.offline_tag)

        if self.runType == 'Las':
            self.blobWriterOffline_F.setComment(author, "TUCS %s" % (" ".join(sys.argv)))
            self.blobWriterOffline_F.register(self.offline_iov, iovUntil, self.offline_tag_F)

        if self.runType == 'Las_REF' or self.runType == 'cesium':
            self.db_REF.closeDatabase() 
        
        self.db.closeDatabase()



    #
    # Here we do the writing
    #
    
    def ProcessRegion(self, region):
        
        if 'gain' not in region.GetHash():
            return
        
        # First the LASER case
        
        if self.runType == 'Las' or self.runType == 'Las_REF':
            
            part, mod, chan, gain = region.GetNumber()
            for event in region.GetEvents():
                
                # Case 1: relative variation
                if self.runType == 'Las' and event.data.has_key('calibration'): 
                    
                    #print 'region updated: ', event.data['region'], event.data['calibration']
                    
                    drawer   = self.blobWriterOffline.getDrawer(part, mod-1)
                    drawer_F = self.blobWriterOffline_F.getDrawer(part, mod-1)
                    
                    # Then we update if there is a new value (coefficients are in %, so we put in correct format)
                    
                    # First update PMT variation
                    drawer.setData(int(chan), int(gain), 0, float(1+event.data['deviation']/100))           
                    
                    # Then update Fiber variation (optional)
                    if event.data.has_key('fiber_var'):
                        drawer_F.setData(int(chan), int(gain), 0, float(1+event.data['fiber_var']/100))
                        
                    # Finally update Partition variation (optional)
                    if event.data.has_key('part_var'):
                        drawer_F.setData(LASPARTCHAN, int(gain), 0, float(1+event.data['part_var']/100))
                        
                # Case 2: reference values
                if self.runType == 'Las_REF': 
                    
                    drawer_R = self.blobReader.getDrawer(part, mod-1, (event.runNumber, 0))
                    
                    if event.data.has_key('calibration_db'): # Channel has a relevant value, store it
                        
                        reference = event.data['calibration']
                        drawer    = self.blobWriterOffline.getDrawer(part, mod-1)
                        
                        drawer.setData(int(chan), int(gain), 0, drawer_R.getData(chan, gain, 0))
                        drawer.setData(int(chan), int(gain), 1, float(reference))
                        drawer.setData(int(chan), int(gain), 2, drawer_R.getData(chan, gain, 2))
                        drawer.setData(int(chan), int(gain), 3, drawer_R.getData(chan, gain, 3))
                        
                    elif event.data.has_key('is_OK'): 
                        
                        print 'No data, set default for region', region.GetHash()
                        drawer = self.blobWriterOffline.getDrawer(part, mod-1)
                        
                        drawer.setData(int(chan), int(gain), 0, drawer_R.getData(chan, gain, 0))
                        drawer.setData(int(chan), int(gain), 1, float(-1))
                        drawer.setData(int(chan), int(gain), 2, drawer_R.getData(chan, gain, 2))
                        drawer.setData(int(chan), int(gain), 3, drawer_R.getData(chan, gain, 3))
        
        # Then the other cases
        
        elif self.runType=='cesium':
            
            part, mod, chan, gain = region.GetNumber()
            if gain!=0: return
            
            drawer = self.blobWriterOffline.getDrawer(part, mod-1)
            if not region.GetEvents():
                # region is empty, writing defaults
                self.writeCsDef(region,drawer,chan,gain)

            # write defaults if 
            writeDef=True
            for event in region.GetEvents():
                if event.runType==self.runType:
                    if event.data.has_key('calibration') and event.data['calibration']:
                        # channel was calibrated
                        writeDef=False
                        drawer.setData(int(chan), int(gain), 0, event.data['calibration'])
                        drawer.setData(int(chan), int(gain), 1, -1.0)
                        drawer.setData(int(chan), int(gain), 2, event.data['HV'])
                        drawer.setData(int(chan), int(gain), 3, event.data['temp'])
            if writeDef:
                self.writeCsDef(region,drawer,chan,gain)
            
                
        else:
            
            x, y, z, w = region.GetNumber()

            calibration = None
            for event in region.GetEvents():
                if event.runType == self.runType:
                    if event.data.has_key('calibratableRegion') and event.data['calibratableRegion']:
                        if event.data.has_key('mean'):
                            calibration = event.data['mean']
                        #print 'yo'

            blobObjVersion = 1
            if calibration:
                print 'region updated: ', region.GetHash(), calibration
                modBlob = self.blobWriterOnline.getDrawer(int(x), int(y-1))
                modBlob.setData(int(z), int(w), 0, float(calibration))

                modBlob = self.blobWriterOffline.getDrawer(int(x), int(y-1))
                modBlob.setData(int(z), int(w), int(0), float(calibration))
            else:
                default_val = (64.*1023./800.) # highgain
                if 'low' in region.GetHash():
                    default_val = (1023./800.)

                print 'region default: ', region.GetHash(), default_val
            
                modBlob = self.blobWriterOnline.getDrawer(int(x), int(y-1))
                modBlob.setData(int(z), int(w), 0, float(default_val))
                
                modBlob = self.blobWriterOffline.getDrawer(int(x), int(y-1))
                modBlob.setData(int(z), int(w), int(0), float(default_val))
                                                              
    def writeCsDef(self,region,drawer,chan,gain):
        # writing defaults
        calibration=1.0
        tower=region.GetParent('physical').GetParent('physical')
        if 'sD' in tower.GetHash():
            calibration=1.2
        if 'sE' in tower.GetHash():
            calibration=1.5
        drawer.setData(int(chan), int(gain), 0, calibration)
        drawer.setData(int(chan), int(gain), 1, -1.0)
        drawer.setData(int(chan), int(gain), 2, 700.0 )
        drawer.setData(int(chan), int(gain), 3, 20.0)
    

                
