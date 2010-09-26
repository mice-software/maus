# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
# Author: Brian Martin <brian.thomas.martin@cern.ch>
#
# March 04, 2009
#

from src.GenericWorker import *
import _mysql,MySQLdb
import os
import time,datetime

from src.cesium import chanlists

class UseCs(GenericWorker):
    "A for creating runlists"

    #  This 'worker' will only add to the event list.  The events it add
    #  will only have the 'runnumber' set and 'runType' set.  There will
    #  only be one event per run.  It is the responsibility of the calibration
    #  reading workers to expand these events.  This is interface allows the
    #  SQL code for getting runs to be separate from the code that reads
    #  ROOT files.  This is also nice because these SQL calls are similar
    #  between calibrations.

    def __init__(self, run, type='readout', region=None, useDateProg=True,verbose=0, runType='all',keepOnlyActive=True):
        #  the variable runStuff could be many things, each of which are
        #  clearly defined below.  This constructor has much functionality.
        self.type = type
        self.runType=runType
        self.verbose = verbose
        self.runs = []
        self.region = region
        self.fragStr = {} # run number -> digiflags
        self.keepOnlyActive = keepOnlyActive
        if isinstance(run, int):  # if just a number, use as run
            self.runs = [run]
        elif isinstance(run, (list, tuple)): # if list, assume of runs
            for element in run:
                if isinstance(element, int):
                    self.runs.append(element)
        elif isinstance(run, str):  # if string, two choices
            if useDateProg:  # use the linux date program with an SQL call
                self.runs = self.dateProg(run,)
            elif os.path.exists(run): # or see if a runStuff exists as a file
                f = open(run)
                for line in f.readlines():
                    line = line.rstrip()
                    self.runs.append(int(line))
            else:
                print "run ain't a file, and you said not use it as a date. Huh?"

        runs = map(int, self.runs)
        self.runs = []
        #db = _mysql.connect('pcata007.cern.ch', user='reader')
        db = _mysql.connect('pcata001.cern.ch', user='reader')

        
        # Cesium runs demand different approach.
        # Each channel may have its own list of runs.
        # 
        if self.runType=='cesium':
            for run in runs:
                if run>40000:
                    #it is ATLAS runnumber. 
                    #
                    db.query("""SELECT date FROM tile.comminfo WHERE run>='%d' ORDER BY run DESC LIMIT 1""" % (run))
                    r = db.store_result()
                    d = r.fetch_row()
                    if d==():
                        print 'There no runs>=%i in tile.comminfo table'%run
                    else:
                        d=time.strptime(d,"%Y-%m-%d %H:%M:%S")
                        self.runs.append([run,self.runType,datetime.datetime(d[0],d[1],d[2],d[3],d[4],d[5])])
                else:
                    #it is cesium runnumber.
                    self.runs.append([run,self.runType,None])
                    
            #cesium runs are not partition-wide
            db.close()        
            return

        # All runs
        for run in runs:
            db.query("""select run, type, date, digifrags from tile.comminfo where run='%d'""" % (run))
            r = db.store_result()
            d = r.fetch_row()

            if d == ():
                run2, rtype, date, digifrags = run, None, None, None
            else:
                run2, rtype, date, digifrags = d[0] # there should only be one run with a given runnumber!

            if runType == 'all' or rtype == runType or rtype == None:  
                self.runs.append([run2, rtype, date])

            if self.keepOnlyActive and rtype == runType or rtype == None:
                if self.verbose or (digifrags and len(digifrags)/6 != 256):
                    print 'In run', run, ', modules in readout: ',len(digifrags)/6
                if digifrags == None:
                    print 'No digifrags available from database.  Turning off filter....'
                    self.keepOnlyActive = False
            self.fragStr[int(run)] = digifrags
            
        db.close()

    def isActive(self,hash, run):
        '''check if detector component was in readout'''

        if not self.fragStr.has_key(run) or not self.fragStr[run]:
            return True

        hexStr = ''
        if 'B' in hash:
            if   'LBA' in hash:
                hexStr = '0x1'
            elif 'LBC' in hash:
                hexStr = '0x2'
            elif 'EBA' in hash:
                hexStr = '0x3'
            elif 'EBC' in hash:
                hexStr = '0x4'

            if '_m' in hash:
                ind = hash.index('m')+1
                modStr = hex(int(hash[ind:ind+2])-1).lstrip('0x')
                while len(modStr)<2:
                    modStr = '0'+modStr
                hexStr += modStr
            return (hexStr in self.fragStr[int(run)])
        elif hash == 'TILECAL':
            return True
        else:
            print 'Unknown hash: ',hash
            return False
        

    def dateProg(self, string,):
        #  dateProg will do MySQL calls to grab a run-list based on the date.  The format
        #  of thes.s string is in the format the linux 'date' progra
        #

        # This really shouldn't use system calls, but it's a hack for now.  TODO fix.
        # This uses the linux date program since it can do things like -1 week.  This
        # should be replaced with some python library, but I don't want to do it myself
        # since there are a lot of boundary cases in computing dates.  Fix me please?

        r = os.popen("date -d '%s' +'%%Y-%%m-%%d %%H:%%M::%%S'" % string)
        date = r.readline()
        r.close()

        db = _mysql.connect('pcata001.cern.ch', user='reader')
        if self.runType!='cesium':
            db.query("""select run from tile.comminfo where run<9999999 and date>'%s'""" % (date))
        else:
            db.query("""select run from tile.runDescr where time>'%s'""" % (date))
        r2 = db.store_result()

        runs = []
        for run in r2.fetch_row(maxrows=0):
            if run[0] not in runs:
                runs.append(run[0])

        db.close()
        
        return runs

    def ProcessStart(self):
        print 'Use: Using the follow runs:', self.runs
        if self.region == None or self.region == '':
            print 'Use: Using all the detector'
        else:
            print 'Use: Only using region(s)', self.region

        if self.runType=='cesium':
            self.db=MySQLdb.connect(host='pcata001',db='tile')
            self.c=self.db.cursor(MySQLdb.cursors.Cursor)
            self.cc=self.db.cursor(MySQLdb.cursors.DictCursor)
            self.Ebad=[3,4,5,6]
            self.period=datetime.timedelta(days=30)

    def ProcessStop(self):
        if self.runType=='cesium':
            self.c.close()
            self.cc.close()
            self.db.close()

            
    def ProcessRegion(self, region):
        if not self.region or self.region in region.GetHash() or self.region in region.GetHash(1):
            for run, rtype, date in self.runs:
                if rtype=='cesium':
                    num=region.GetNumber(1)
                    if len(num)==3: #this is PMT, len is something like [2,4,5] - ros,mod,pmt
                        module=region.GetParent()
                        #print 'calling FillModule',run
                        self.FillModule(module,run,rtype,date)
                        for evt in module.GetEvents():
                            #print evt.runNumber, run, num, evt.data
                            if evt.data['csRun']==run and num[2] in evt.data['PMT']:
                                # this PMT is in a good channel list for this par,module,run
                                region.AddEvent(Event(runType=rtype, runNumber=evt.runNumber, data={'csRun':run}, time=evt.time))
                else:
                    # code for all runtypes except cesium
                    if self.keepOnlyActive and not self.isActive(region.GetHash(), int(run)):
                        if self.verbose:
                            print 'Region not in readout, removing: ', region.GetHash()
                    else:
                        # staging should eventually be the runtype here...
                        region.AddEvent(Event(runType='staging', runNumber=int(run), data={}, time=date))




    # -------------- this part is cesium specific --------------
    def FillModule(self,region,run,rtype,date):
        num=region.GetNumber(1)
        if len(num)==2: #this must be a module, len is something like [1,4]
            upd=True
            for ev in region.GetEvents():
                if ev.data['csRun']==run: upd=False
            if not upd:
                return
            rlist=[run]
            if date:
                rlist=self.GetCsRuns(date-self.period,date)
            (data,dict)=self.GetCsDescr(rlist,region.GetParent().GetName(),num[1])
            #print data
            if data!=None:
                # this is a good place to store ATLAS runnumber
                self.GetOneATLASRun(dict['time'])
                region.AddEvent(Event(runType=rtype, runNumber=self.GetOneATLASRun(dict['time']), data={'csRun':dict['run'],'PMT':data,'source':dict['source']}, time=dict['time']))


    def GetOneATLASRun(self,time):
        if self.c.execute("SELECT run FROM comminfo WHERE date>'%s' ORDER BY date ASC LIMIT 1"%\
                          (time.strftime("%Y-%m-%d %H:%M:%S")))>=1:
            cl=self.c.fetchall()
            for r in cl:
                return r[0]
    
    def GetCsRuns(self,time1,time2):
        runs=[]
        if self.c.execute("SELECT run FROM runDescr WHERE time>'%s' AND time<'%s' ORDER BY time DESC"%\
                          (time1.strftime("%Y-%m-%d %H:%M:%S"),time2.strftime("%Y-%m-%d %H:%M:%S")))>=1:
            cl=self.c.fetchall()
            for r in cl:
                if not r[0] in runs:
                    runs.append(r[0])
        return runs
    
    def GetCsDescr(self,csruns,par,mod):
        for csrun in csruns:
            #print csrun,par,mod
            # here we have list of runs and look for run/par/mod in the database
            # as runs are sorted in desc. order we get the latest cs run
            if self.cc.execute("SELECT * FROM runDescr WHERE run=%i AND partition='%s' AND module=%i"%(csrun,par,mod))!=1:
                continue
            dbdicts = self.cc.fetchall()
            #it should be only one record
            d=None
            for dict in dbdicts:
                dblist=self.if_ok(dict)
                return (dblist,dict)
        # if we get here there were no Cs runs during the last month
        return (None,None)
        
    def if_ok(self,dbdict):
        retlist=[]
        par=dbdict['partition']
        mod=dbdict['module']
        #print "run",dbdict['run']
        for pmt in xrange(1,49):
            if par in chanlists.deadchan and mod in chanlists.deadchan[par] and pmt in chanlists.deadchan[par][mod]:
                print 'dead channel: ',par,mod,pmt
                continue
            if par in chanlists.wrongchan and mod in chanlists.wrongchan[par] and pmt in chanlists.wrongchan[par][mod]:
                print 'wrong channel, using as is',par,mod,pmt
                retlist.append(pmt)
                continue
            if dbdict['statusPMT'+"%02i"%pmt]=='OK' and dbdict['comment']=='':
                retlist.append(pmt)
                continue
            if par[0]=='E' and pmt in self.Ebad and dbdict['statusPMT'+"%02i"%pmt]=='wrong number of peaks':
                #print 'suppressing  wrong number of peaks ',par,mod,pmt
                retlist.append(pmt)
                continue
        return retlist

