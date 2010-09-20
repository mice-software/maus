execfile('src/load.py', globals()) # don't remove this!
import os

selected_region = 'EBA_m08'
#selected_region = 'EBA'
runList = [115472]
#runList = [91569,91585,91841,92018,92417,92973,92974,92976,92977,93403,93413,93414,93885,94110,95385,96632,96635,96637,97077]


#os.remove('noise.HIST.root')        
#u = Use(run=99027) # only EBA
u=Use(run=runList,type='physical',region=selected_region,verbose=True,keepOnlyActive=False)
#processList = [u,ReadDigiNoiseFile(),ReadChanNoiseFile(),ReadDigiNoiseDB(),ReadChanNoiseDB(),CheckNoiseBounds()]
#processList = [u,ReadDigiNoiseDB(),Print()]
#processList = [u,ReadDigiNoiseDB(),ReadDigiNoiseFile(),NoiseStability(plotDetail=1),NoiseVsDB(),WriteNoiseDB(runNumber=runList[-1])]
#processList = [u,ReadDigiNoiseDB(),ReadDigiNoiseFile(),NoiseStability(plotDetail=1),NoiseVsDB()]

#processList = [u,ReadCellNoiseDB(),ReadCellNoiseFile(),NoiseStability(parameter='cell'),NoiseVsDB(parameter='cell'),WriteCellNoiseDB(offline_iov=(runList[0],0))]
processList = [u,ReadCellNoiseDB(),Print(type='physical')]

g = Go(processList)


