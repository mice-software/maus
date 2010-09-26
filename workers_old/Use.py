# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
# Author: Brian Martin <brian.thomas.martin@cern.ch>
#
# March 04, 2009
#

from src.GenericWorker import *
import _mysql
import os

class Use(GenericWorker):
    "A worker for telling TUCS what runs to use"

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
        db = _mysql.connect('pcata007.cern.ch', user='reader')
        #db = _mysql.connect('pcata001.cern.ch', user='reader')  
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

        r = os.popen("date -d '%s' +%%Y-%%m-%%d" % string)
        date = r.readline()
        r.close()

        db = _mysql.connect('pcata007.cern.ch', user='reader')
        db.query("""select run from tile.comminfo where run<9999999 and date>'%s'""" % (date))
        r2 = db.store_result()

        runs = []
        for run in r2.fetch_row(maxrows=0):
            runs.append(run[0])

        db.close()
        
        return runs

    def ProcessStart(self):
        print 'Use: Using the follow runs:', self.runs
        if self.region == None or self.region == '':
            print 'Use: Using all the detector'
        else:
            print 'Use: Only using region(s)', self.region
        

    def ProcessRegion(self, region):
        if not self.region or self.region in region.GetHash() or self.region in region.GetHash(1):
            for run, rtype, date in self.runs:
                if self.keepOnlyActive and not self.isActive(region.GetHash(), int(run)):
                    if self.verbose:
                        print 'Region not in readout, removing: ', region.GetHash()
                else:
                    # staging should eventually be the runtype here...
                    region.AddEvent(Event(runType='staging', runNumber=int(run), data={}, time=date))
