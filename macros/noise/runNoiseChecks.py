#!/usr/bin/env python
execfile('src/load.py', globals()) # don't remove this!
#===========================================================
# This macro provides an interface for:
# + computing Tile Noise constants
# + checking their consistency with the DB
# + monitoring their consistency over multiple runs
# + updating the Tile/Calo noise DB
#===========================================================
import os
import os.path
import _mysql
import optparse
parser = optparse.OptionParser()
parser.add_option('-d','--useDigits',action="store_true",dest="doDigits",default=False)
parser.add_option('-r','--useRawCh', action="store_true",dest="doRawCh", default=False)
parser.add_option('-c','--useCells', action="store_true",dest="doCells", default=False)
parser.add_option('-w','--writeDB',  action="store_true",dest="writeDB", default=False)
parser.add_option('-b','--batch',    action="store_true",dest="batch",   default=False)
parser.add_option('--minRun',  dest="minRun")
parser.add_option('--maxRun',  dest="maxRun",default='-1')
parser.add_option('--minDate', dest="minDate",help="Format=YYYY-MM-DD")
parser.add_option('--maxDate', dest="maxDate",default='-1',help="Format=YYYY-MM-DD")
parser.add_option('-R','--Region',   dest="selected_region", default='')
(options, args) = parser.parse_args()

def getRuns(min,max,byRun):
    db = _mysql.connect('pcata007.cern.ch', user='reader')
    if byRun:
        if max == -1:
            db.query("""select run from tile.comminfo where run>%i and events > 4000 and type='Ped'""" % (min-1))
        else:
            db.query("""select run from tile.comminfo where run>%i and run<%i and events > 4000 and type='Ped'""" % (min-1,max+1))
    else:
        if max == '-1':
            db.query("""select run from tile.comminfo where date>"%s" and events > 4000 and type='Ped'""" % (min))
        else:
            db.query("""select run from tile.comminfo where date>"%s" and date<"%s" and events > 4000 and type='Ped'""" % (min,max))
    
    r = db.store_result()
    
    runs = []
    for run in r.fetch_row(maxrows=0):
        runs.append(run[0])

    db.close()
        
    return runs

doDigits = options.doDigits
doRawCh  = options.doRawCh
doCells  = options.doCells
writeDB  = options.writeDB

selected_region = options.selected_region

if options.minRun:
    minRun = int(options.minRun)
    maxRun = int(options.maxRun)
    runList = getRuns(minRun,maxRun,True)
elif options.minDate:
    minDate = options.minDate
    maxDate = options.maxDate
    runList = getRuns(minDate,maxDate,False)
else:
    print "Please select either a minRun or minDate"
    sys.exit(1)

print 'Runs to analyze:'
print runList
if not options.batch:
    cont = raw_input('Run over these runs? (y/n)')
    if cont != 'y':
        sys.exit(0)

# Check for sqlite DB's make copies if needed
if (doDigits or doRawCh) and not os.path.exists('./tileSqlite.db'):
    if not options.batch:
        cont = raw_input('tileSqlite.db is missing. Would you like to make a copy from Oracle DB? (y/n)')
        if cont != 'y':
            sys.exit(0)
    os.system('macros/noise/copyDB.sh -t')
if doCells and not os.path.exists('./caloSqlite.db'):
    if not options.batch:
        cont = raw_input('caloSqlite.db is missing. Would you like to make a copy from Oracle DB? (y/n)')
        if cont != 'y':
            sys.exit(0)
    os.system('macros/noise/copyDB.sh -c')


# convert runList from string to int
for r in xrange(len(runList)): runList[r] = int(runList[r])
runList.sort()

processList = []
if doDigits:
    u=Use(run=runList,region=selected_region,verbose=True,keepOnlyActive=False)
    processList += [u,ReadChanStat(updateEvent=True),ReadDigiNoiseDB(),ReadDigiNoiseFile(),NoiseStability(),NoiseVsDB()]
    if writeDB: 
        processList.append(WriteDigiNoiseDB(offline_iov=(runList[0],0)))
if doRawCh:
    u=Use(run=runList,region=selected_region,verbose=True,keepOnlyActive=False)
    processList += [u,ReadChanNoiseDB(),ReadChanNoiseFile(),NoiseStability(parameter='chan'),NoiseVsDB(parameter='chan')]
    if writeDB: 
        processList.append(WriteChanNoiseDB(offline_iov=(runList[0],0)))
if doCells:
    u=Use(run=runList,type='physical',region=selected_region,verbose=True,keepOnlyActive=False)
    processList += [u,ReadCellNoiseDB(),ReadCellNoiseFile(),NoiseStability(parameter='cell'),NoiseVsDB(parameter='cell')]
    if writeDB: 
        processList.append(WriteCellNoiseDB(offline_iov=(runList[0],0)))

g = Go(processList)


